#include <arch/i386/pic.h>
#include <arch/i386/ports.h>

void pic_8259_init() {
    uint8_t mask1 = port_byte_in(PIC_8259_MASTER_DATA_REGISTER);
    uint8_t mask2 = port_byte_in(PIC_8259_SLAVE_DATA_REGISTER);

    // ICW1: Initialize remapping for maser + slave by sending ICW1 (Initialisation Command Word)

    port_byte_out(PIC_8259_MASTER_COMMAND_REGISTER, PIC_8259_ICW1_INIT | PIC_8259_ICW1_ICW4);
    port_byte_out(PIC_8259_SLAVE_COMMAND_REGISTER, PIC_8259_ICW1_INIT | PIC_8259_ICW1_ICW4);

    // ICW2: Set PIC offset for master + slave

    /*
     * The following command remaps the PICs IRQ 0-15 to IDT entries 32-47. This is because the CPU
     * reserves the first 32 IDT entries for exceptions. The master PIC is set to 0x20 and the slave
     * PIC is set to 0x28. This is because the master PIC has IRQs 0-7 and the slave PIC has IRQs
     * 8-15.
     */

    port_byte_out(PIC_8259_MASTER_DATA_REGISTER, 0x20);
    port_byte_out(PIC_8259_SLAVE_DATA_REGISTER, 0x28);

    // ICW3: Set master + slave PIC connection

    port_byte_out(PIC_8259_MASTER_DATA_REGISTER, 0x04);
    port_byte_out(PIC_8259_SLAVE_DATA_REGISTER, 0x02);

    // ICW4: Set master + slave PIC mode

    port_byte_out(PIC_8259_MASTER_DATA_REGISTER, PIC_8259_ICW4_8086);
    port_byte_out(PIC_8259_SLAVE_DATA_REGISTER, PIC_8259_ICW4_8086);

    // Restore masks

    port_byte_out(PIC_8259_MASTER_DATA_REGISTER, mask1);
    port_byte_out(PIC_8259_SLAVE_DATA_REGISTER, mask2);

    // Unmask all IRQs
    for (uint8_t i = 0; i < 16; i++) {
        pic_8259_unmask_irq(i);
    }
}

void pic_8259_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        port_byte_out(PIC_8259_SLAVE_COMMAND_REGISTER, PIC_8259_EOI);
    } else {
        port_byte_out(PIC_8259_MASTER_COMMAND_REGISTER, PIC_8259_EOI);
    }
}

void pic_8259_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC_8259_MASTER_DATA_REGISTER;
    } else {
        port = PIC_8259_SLAVE_DATA_REGISTER;
        irq -= 8;
    }

    value = port_byte_in(port) | (1 << irq);
    port_byte_out(port, value);
}

void pic_8259_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if(irq < 8) {
        port = PIC_8259_MASTER_DATA_REGISTER;
    } else {
        port = PIC_8259_SLAVE_DATA_REGISTER;
        irq -= 8;
    }

    value = port_byte_in(port) & ~(1 << irq);
    port_byte_out(port, value);
}
