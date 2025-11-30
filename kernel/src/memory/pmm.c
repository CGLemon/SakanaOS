#include <memory/pmm.h>
#include <memory/heap.h>
#include <system/message.h>
#include <system/panic.h>
#include <utils/array.h>
#include <utils/numeric.h>

static pmm_memory_t pmm_memory;

void pmm_fetch_memory_regions(multiboot_info_t * multiboot_info);
void pmm_memory_regions_sort();
int pmm_memory_region_compare(void * a, void * b);

void pmm_init(multiboot_info_t * multiboot_info) {
    pmm_memory.memory_size = 0;
    pmm_fetch_memory_regions(multiboot_info);

    pmm_memory.num_memory_frames = PMM_PAS_SIZE / PMM_FRAME_SIZE;
    pmm_memory.num_memory_frames_used = pmm_memory.num_memory_frames;
    pmm_memory.bitmap_size = div_up(
        pmm_memory.num_memory_frames, PMM_FRAMES_PER_BITMAP_BYTE);
    pmm_memory.bitmap =
        (uint8_t *)kmalloc(pmm_memory.bitmap_size);
    if (!pmm_memory.bitmap) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    // By default, all memory frames are used
    memset(pmm_memory.bitmap, 0xFF, pmm_memory.bitmap_size);

    array_foreach(pmm_memory.num_regions) {
        pmm_memory_region_t * region = pmm_memory.regions + i;

        if (region->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t region_start_frame =
                pmm_address_to_index((void *)region->base);
            uint32_t region_end_frame =
                pmm_address_to_index((void *)(region->base + region->length - 1));

            // Mark the memory region as available
            uint32_t frame;
            for (frame = region_start_frame; frame <= region_end_frame; frame++) {
                pmm_unset_frame(frame);
            }
        }
    } array_foreach_end;

    char kernel_message[96];
    size_t num_frames =  pmm_memory.num_memory_frames - pmm_memory.num_memory_frames_used;
    size_t memory_size = pmm_memory.memory_size;
    sprintf(kernel_message, "memory: PMM initialized with %d (%d MB) memory frames",
                num_frames, memory_size / 1024 / 1024);
    kmessage(KMESSAGE_LEVEL_INFO, kernel_message);
}

void pmm_fetch_memory_regions(multiboot_info_t * multiboot_info) {
    pmm_memory.region_size =
        sizeof(pmm_memory_region_t);
    pmm_memory.num_regions =
        multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
    pmm_memory.regions =
        kmalloc(pmm_memory.region_size * pmm_memory.region_size);
    if (!pmm_memory.regions) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    multiboot_memory_map_t * mmap_addr =
        (multiboot_memory_map_t *)multiboot_info->mmap_addr;

    array_foreach(pmm_memory.num_regions) {
        pmm_memory_region_t * region = pmm_memory.regions + i;
        multiboot_memory_map_t * mmap = mmap_addr + i ;

        region->base = mmap->addr;
        region->length = mmap->len;
        region->type = mmap->type;
        pmm_memory.memory_size += region->length;
    } array_foreach_end;

    array_bubble_sort(
        pmm_memory.regions, pmm_memory.num_regions,
        pmm_memory.region_size, pmm_memory_region_compare);
}

int pmm_memory_region_compare(void * a, void * b) {
    pmm_memory_region_t * region_a = (pmm_memory_region_t*) a;
    pmm_memory_region_t * region_b = (pmm_memory_region_t*) b;

    if (region_a->base < region_b->base) {
        return -1;
    } else if(region_a->base > region_b->base) {
        return 1;
    }

    if (region_a->length < region_b->length) {
        return -1;
    } else if(region_a->length > region_b->length) {
        return 1;
    }

    return 0;
}

uint32_t pmm_address_to_index(void * address) {
    return paging_frame_index(address);
}

void * pmm_find_free_frame_from(void * address) {
    uint32_t start = pmm_address_to_index(address);

    for (int i = start; i < pmm_memory.num_memory_frames; ++i) {
        if (!pmm_test_frame(i)) {
            void * physical_address = (void *)(i * PMM_FRAME_SIZE);
            return physical_address;
        }
    }
    return NULL;
}

void * pmm_find_free_frame() {
    return pmm_find_free_frame_from(PMM_PAS_BASE);
}

bool pmm_test_frame(uint32_t frame_idx) {
    uint32_t block_idx = frame_idx / PMM_FRAMES_PER_BITMAP_BYTE;
    uint32_t bit_idx = frame_idx % PMM_FRAMES_PER_BITMAP_BYTE;
    return pmm_memory.bitmap[block_idx] & (1 << bit_idx);
}

void pmm_set_frame(uint32_t frame_idx) {
    if (pmm_test_frame(frame_idx)) {
        return;
    }

    uint32_t block_idx = frame_idx / PMM_FRAMES_PER_BITMAP_BYTE;
    uint32_t bit_idx = frame_idx % PMM_FRAMES_PER_BITMAP_BYTE;
    pmm_memory.bitmap[block_idx] |= (1 << bit_idx);
    pmm_memory.num_memory_frames_used++;
}

void pmm_unset_frame(uint32_t frame_idx) {
    if (!pmm_test_frame(frame_idx)) {
        return;
    }

    uint32_t block_idx = frame_idx / PMM_FRAMES_PER_BITMAP_BYTE;
    uint32_t bit_idx = frame_idx % PMM_FRAMES_PER_BITMAP_BYTE;
    pmm_memory.bitmap[block_idx] &= ~(1 << bit_idx);
    pmm_memory.num_memory_frames_used--;
}

void pmm_mark_frame_reserved(void * frame_addr) {
    pmm_set_frame(pmm_address_to_index(frame_addr));
}

void pmm_mark_frame_available(void * frame_addr) {
    pmm_unset_frame(pmm_address_to_index(frame_addr));
}
