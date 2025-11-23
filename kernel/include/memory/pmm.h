#ifndef _KERNEL_MEMORY_PMM_H
#define _KERNEL_MEMORY_PMM_H

#include <multiboot.h>
#include <memory/mm_layout.h>
#include <arch/i386/paging.h>
#include <utils/types.h>

#define PMM_FRAME_SIZE (PAGE_SIZE)
#define PMM_FRAMES_PER_BITMAP_BYTE 8

typedef struct pmm_memory_region {
    uint32_t base;
    uint32_t length;
    uint32_t type;
} pmm_memory_region_t;

typedef struct pmm_memory {
    pmm_memory_region_t * regions;
    size_t num_regions;
    size_t region_size;
    size_t memory_size;

    size_t num_memory_frames;
    size_t num_memory_frames_used;

    uint8_t * bitmap;
    size_t bitmap_size;
} pmm_memory_t;

void pmm_init(multiboot_info_t * multiboot_info);
uint32_t pmm_address_to_index(void * address);
void * pmm_find_free_frame();
void * pmm_find_free_frame_from(void * address);

bool pmm_test_frame(uint32_t frame_idx);
void pmm_set_frame(uint32_t frame_idx);
void pmm_unset_frame(uint32_t frame_idx);

void pmm_mark_frame_reserved(void * frame_addr);
void pmm_mark_frame_available(void * frame_addr);
#endif
