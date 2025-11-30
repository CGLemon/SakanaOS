#include <drivers/video/vga/vga.h>
#include <drivers/input/ps2/keyboard.h>
#include <drivers/pci/pci.h>
#include <drivers/storage/ata.h>
#include <drivers/storage/sata.h>

#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/tss.h>
#include <arch/i386/isr.h>
#include <arch/i386/pic.h>
#include <arch/i386/paging.h>

#include <memory/pmm.h>
#include <memory/vmm.h>
#include <memory/heap.h>
// #include <memory/mm_layout.h>

#include <device/device.h>

#include <system/message.h>
#include <shell/shell.h>
#include <io/tty.h>
#include <multiboot.h>
#include <utils/types.h>

void init_kernel(multiboot_info_t * multiboot_info) {

    kmessage_init();

    gdt_init();
    idt_init();
    tss_init(gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 0), 0x0);
    pmm_init(multiboot_info);
    vmm_init();

    pic_8259_init();

    tss_update_ring0_stack(
        gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 0),
        tss_get_stack_point());
    multiboot_info = (multiboot_info_t *)((uintptr_t)multiboot_info + VMM_KERNEL_SPACE_BASE);

    kheap_init();
    device_init();

    vga_init(VGA_80x25_16_TEXT);
    ps2_keyboard_init();
    pci_init();
    ata_init();
}

void execute_shell() {
    tty_t * tty = tty_create();
    shell_t * s = shell_create(tty);

    shell_execute(s);

    tty_destroy(tty);
    shell_destroy(s);
}

void kmain(multiboot_info_t * multiboot_info, uint32_t magic)  {
    clear_interrupt_flag();
    init_kernel(multiboot_info);
    set_interrupt_flag();

    execute_shell();
}
