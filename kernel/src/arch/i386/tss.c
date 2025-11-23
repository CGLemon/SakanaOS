#include <arch/i386/tss.h>
#include <arch/i386/gdt.h>
#include <utils/string.h>
#include <system/message.h>

static tss_segment_descriptor_t tss;

extern void tss_flush();

void tss_init(uint16_t ss0, uint32_t esp0) {
    uint32_t tss_base = (uint32_t) &tss;
    uint32_t tss_limit = sizeof(tss);

    gdt_init_descriptor(GDT_TASK_STATE_SELEKTOR, tss_base, tss_limit,
        GDT_ACCESS_PRESENT|GDT_ACCESS_RING3|GDT_ACCESS_EXECUTABLE|GDT_ACCESS_ACCESSED,
        0x00);
 
    memset(&tss, 0, sizeof(tss));

    tss.ss0 = ss0;
    tss.esp0 = esp0;

    /*
     * Initialize segment registers used when processor switches to kernel mode. We use basically our
     * kernel data segment (offset 0x10) and kernel code segment (offset 0x08). But we also need to
     * set the last two bits (Request Privilege Level) for ring 3, so we get 0x13 for data segment
     * and 0x0B for code segment.
     */

    tss.cs = gdt_get_selector_value(GDT_KERNEL_CODE_SELEKTOR, 3);
    tss.ss = gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 3);
    tss.ds = gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 3);
    tss.es = gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 3);
    tss.fs = gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 3);
    tss.gs = gdt_get_selector_value(GDT_KERNEL_DATA_SELEKTOR, 3);

    tss_flush();

    kmessage(KMESSAGE_LEVEL_INFO, "cpu: TSS initialized");
}

void tss_update_ring0_stack(uint16_t ss0, uint32_t esp0) {
    tss.ss0 = ss0;
    tss.esp0 = esp0;
}

uint32_t tss_get_stack_point() {
    uint32_t kernel_stack;
    __asm__ volatile("mov %%esp, %0" : "=r" (kernel_stack));
    return kernel_stack;
}
