#ifndef _KERNEL_ARCH_I386_PIC_H
#define _KERNEL_ARCH_I386_PIC_H

#include <utils/types.h>

#define PIC_8259_MASTER_COMMAND_REGISTER  0x20
#define PIC_8259_MASTER_DATA_REGISTER     0x21

#define PIC_8259_SLAVE_COMMAND_REGISTER   0xA0
#define PIC_8259_SLAVE_DATA_REGISTER      0xA1

#define PIC_8259_EOI 0x20

#define PIC_8259_ICW1_ICW4       0x01
#define PIC_8259_ICW1_SINGLE     0x02
#define PIC_8259_ICW1_INTERVAL4  0x04
#define PIC_8259_ICW1_LEVEL      0x08
#define PIC_8259_ICW1_INIT       0x10

#define PIC_8259_ICW4_8086       0x01
#define PIC_8259_ICW4_AUTO       0x02
#define PIC_8259_ICW4_BUF_SLAVE  0x08
#define PIC_8259_ICW4_BUF_MASTER 0x0C
#define PIC_8259_ICW4_SFNM       0x10


void pic_8259_init();
void pic_8259_send_eoi(uint8_t irq);
void pic_8259_mask_irq(uint8_t irq);
void pic_8259_unmask_irq(uint8_t irq);

#endif
