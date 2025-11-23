#include <arch/i386/paging.h>
#include <system/message.h>

void paging_enable() {
    uint32_t cr4;
    uint32_t cr0;

    __asm__ volatile("mov %%cr4, %0" : "=r" (cr4));
    __asm__ volatile("mov %%cr0, %0" : "=r" (cr0));

    cr4 &= 0xFFFFFFeF; // Disable 4MB pages
    cr0 |= 0x80000000; // Enable paging

    __asm__ volatile("mov %0, %%cr4" : : "r" (cr4));
    __asm__ volatile("mov %0, %%cr0" : : "r" (cr0));
}

void paging_invalidate_page(void * virtual_address) {
    __asm__ volatile("invlpg (%0)" : : "r" (virtual_address) : "memory");
}

void paging_switch_page_directory(void * physical_address) {
    __asm__ volatile("mov %0, %%cr3" : : "r" (physical_address) : "memory");
}

void * paging_get_directory() {
    uint32_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r" (cr3));
    return (void *)cr3;
}

uint32_t paging_directory_index(void * virtual_address) {
    return ((uint32_t)virtual_address) >> 22;
}

uint32_t paging_table_index(void * virtual_address) {
    return (((uint32_t)virtual_address) >> 12) & 0x3FF;
}

uint32_t paging_memory_offset(void * virtual_address) {
    return ((uint32_t)virtual_address) & 0xFFF;
}

uint32_t paging_frame_index(void * physical_address) {
    return ((uint32_t)physical_address) >> 12;
}

bool paging_is_aligned_address(void * address) {
    return ((uintptr_t)address & (PAGE_SIZE - 1)) == 0;
}

void * paging_align_address_up(void * address) {
    return (void *)(((uintptr_t)address + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
}

void * paging_align_address_down(void * address) {
    return (void *)((uintptr_t)address & ~(PAGE_SIZE - 1));
}

uint32_t paging_align_address_offset(void * address) {
    return (uint32_t)((uintptr_t)(address) & (PAGE_SIZE - 1));
}
