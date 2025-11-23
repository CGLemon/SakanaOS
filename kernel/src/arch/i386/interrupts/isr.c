#include <arch/i386/isr.h>

void clear_interrupt_flag() {
    __asm__ volatile("cli");
}

void set_interrupt_flag() {
    __asm__ volatile("sti");
}
