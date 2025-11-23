#ifndef _KERNEL_ARCH_I386_TSS_H
#define _KERNEL_ARCH_I386_TSS_H

#include <utils/types.h>

// https://wiki.osdev.org/Task_State_Segment

typedef struct tss_segment_descriptor {
    uint16_t link;
    uint16_t reserved0;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t reserved1;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t reserved2;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t reserved3;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t reserved4;
    uint16_t cs;
    uint16_t reserved5;
    uint16_t ss;
    uint16_t reserved6;
    uint16_t ds;
    uint16_t reserved7;
    uint16_t fs;
    uint16_t reserved8;
    uint16_t gs;
    uint16_t reserved9;
    uint16_t ldt;
    uint16_t reserved10;
    uint16_t reserved11;
    uint16_t iopb;
    uint32_t ssp;
} __attribute__ ((packed)) tss_segment_descriptor_t;

void tss_init(uint16_t ss0, uint32_t esp0);

void tss_update_ring0_stack(uint16_t ss0, uint32_t esp0);

uint32_t tss_get_stack_point();

#endif
