#include <arch/i386/isr.h>

#define INT_CASE(n) \
    case (n): __asm__ volatile("int $" #n); break;

#define DO_INT(i)                          \
    do {                                   \
        switch (i) {                       \
            INT_CASE(0x00);                \
            INT_CASE(0x01);                \
            INT_CASE(0x02);                \
            INT_CASE(0x03);                \
            INT_CASE(0x04);                \
            INT_CASE(0x05);                \
            INT_CASE(0x06);                \
            INT_CASE(0x07);                \
            INT_CASE(0x08);                \
            INT_CASE(0x09);                \
            INT_CASE(0x0A);                \
            INT_CASE(0x0B);                \
            INT_CASE(0x0C);                \
            INT_CASE(0x0D);                \
            INT_CASE(0x0E);                \
            INT_CASE(0x0F);                \
            INT_CASE(0x10);                \
            INT_CASE(0x11);                \
            INT_CASE(0x12);                \
            INT_CASE(0x13);                \
            INT_CASE(0x14);                \
            INT_CASE(0x15);                \
            INT_CASE(0x16);                \
            INT_CASE(0x17);                \
            INT_CASE(0x18);                \
            INT_CASE(0x19);                \
            INT_CASE(0x1A);                \
            INT_CASE(0x1B);                \
            INT_CASE(0x1C);                \
            INT_CASE(0x1D);                \
            INT_CASE(0x1E);                \
            INT_CASE(0x1F);                \
            default: break;                \
        }                                  \
    } while (0)

void clear_interrupt_flag() {
    __asm__ volatile("cli");
}

void set_interrupt_flag() {
    __asm__ volatile("sti");
}

void isr_interrupt(int i) {
    DO_INT(i);
}

#undef INT_CASE
#undef DO_INT
