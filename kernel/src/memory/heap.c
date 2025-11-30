#include <memory/heap.h>
#include <memory/vmm.h>
#include <utils/string.h>

#include <system/message.h>
#include <system/panic.h>
#include <memory/mm_layout.h>
#include <arch/i386/paging.h>

static uint8_t kheap_placement_memory[KHEAP_PLACEMENT_SIZE];
static uint8_t * kheap_placement_memory_head = kheap_placement_memory;
static size_t kheap_placement_memory_free_memory = KHEAP_PLACEMENT_SIZE;

static bool kheap_enabled = false;
static void * kheap_base = NULL;
static kheap_block_head_t * kheap_head = NULL;
static kheap_block_head_t * kheap_tail = NULL;

void * kmalloc_base(bool enable_heap, size_t size, uint32_t flag);
void * kmalloc_heap_base(size_t size, uint32_t flag);
void * kmalloc_placement_base(size_t size, uint32_t flag);
bool kheap_is_placement_memory(void * ptr);
kheap_block_head_t * kheap_get_heap_block(void * ptr);

void * kheap_find_best_fit(size_t size, bool align);

void kheap_init() {
    kheap_base = vmm_map_memory(NULL, NULL, KHEAP_HEAP_SIZE, true, true);

    if (!kheap_base) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    kheap_head = (kheap_block_head_t *)kheap_base;
    kheap_head->prev = NULL;
    kheap_head->next = NULL;
    kheap_head->magic = KHEAP_MAGIC;
    kheap_head->size = KHEAP_HEAP_SIZE - sizeof(kheap_block_head_t);
    kheap_head->free = true;

    kheap_tail = kheap_head;
    kheap_enabled = true;

    kmessage(KMESSAGE_LEVEL_INFO, "memory: Kernel heap initialized");
}

bool kheap_is_enabled() {
    return kheap_enabled;
}

kheap_block_head_t * kheap_get_heap_head() {
    return kheap_head;
}

void * kmalloc(size_t size) {
    return kmalloc_base(kheap_enabled, size, MALLOC_FLAG_NULL);
}

void * kmalloc_flag(size_t size, uint32_t flag) {
    return kmalloc_base(kheap_enabled, size, flag);
}

void * kmalloc_base(bool enable_heap, size_t size, uint32_t flag) {
    if (enable_heap) {
        return kmalloc_heap_base(size, flag);
    }
    return kmalloc_placement_base(size, flag);
}

void * kmalloc_heap_base(size_t size, uint32_t flag) {
    kheap_block_head_t * best_fit;
    void * data_addr;

    if (size == 0) {
        return NULL;
    }
    best_fit = kheap_find_best_fit(size, flag & MALLOC_FLAG_ALIGN);

    if (!best_fit) {
        return NULL;
    }
    data_addr = (void *)((uintptr_t)best_fit + sizeof(kheap_block_head_t));

    if ((flag & MALLOC_FLAG_ALIGN) && !paging_is_aligned_address(data_addr)) {
        void * new_data_addr =
            paging_align_address_up(data_addr + sizeof(kheap_block_head_t));
        size_t padding = (uintptr_t)new_data_addr - (uintptr_t)data_addr;

        // Ensure block has enough space for alignment padding block
        // and can be split
        if (best_fit->size < size + padding) {
            return NULL;
        }

        kheap_block_head_t * new_best_fit =
            (kheap_block_head_t *)((uintptr_t)new_data_addr - sizeof(kheap_block_head_t));

        new_best_fit->free = true;
        new_best_fit->magic = KHEAP_MAGIC;
        new_best_fit->size = best_fit->size - padding;
        new_best_fit->next = best_fit->next;
        new_best_fit->prev = best_fit;


        kheap_block_head_t * padding_block = best_fit;
        padding_block->free = true;
        padding_block->size = padding - sizeof(kheap_block_head_t);
        padding_block->next = new_best_fit;


        best_fit = new_best_fit;
    }

    size_t remaining_size = best_fit->size - size;
    best_fit->free = false;

    // Split the block if there is enough space for a new block
    if (remaining_size > sizeof(kheap_block_head_t)) {
        kheap_block_head_t * remaining_block =
            (kheap_block_head_t *)((uintptr_t)best_fit + sizeof(kheap_block_head_t) + size);
        remaining_block->free = true;
        remaining_block->magic = KHEAP_MAGIC;
        remaining_block->size = remaining_size - sizeof(kheap_block_head_t);
        remaining_block->next = best_fit->next;
        remaining_block->prev = best_fit;

        if (remaining_block->next == NULL) {
            kheap_tail = remaining_block;
        } else {
            remaining_block->next->prev = remaining_block;
        }

        best_fit->size = size;
        best_fit->next = remaining_block;
    }
    data_addr = (void *)((uintptr_t)best_fit + sizeof(kheap_block_head_t));
    if (flag & MALLOC_FLAG_INIT) {
        memset(data_addr, 0, best_fit->size);
    }

    return data_addr;
}

void * kmalloc_placement_base(size_t size, uint32_t flag) {
    uint8_t * addr = kheap_placement_memory_head;
    size_t alloc_size = size;
    size_t real_alloc_size = alloc_size;

    if (flag & MALLOC_FLAG_ALIGN) {
        uint8_t * aligned_addr = (uint8_t *)(paging_align_address_up(addr));
        size_t padding = (uintptr_t)aligned_addr - (uintptr_t)addr;

        real_alloc_size = alloc_size + padding;      
        addr = aligned_addr;
    }

    if (real_alloc_size >
            kheap_placement_memory_free_memory) {
        return NULL;
    }
    kheap_placement_memory_head += real_alloc_size;
    kheap_placement_memory_free_memory -= real_alloc_size;

    if (flag & MALLOC_FLAG_INIT) {
        memset(addr, 0, alloc_size);
    }
    return addr;
}

kheap_block_head_t * kheap_get_heap_block(void * ptr) {
    kheap_block_head_t * block =
        (kheap_block_head_t *)((uintptr_t)ptr - sizeof(kheap_block_head_t));

    if (block->magic != KHEAP_MAGIC) {
        return NULL;
    }
    return block;
}

void * krealloc(void * ptr, size_t size) {
    return krealloc_flag(ptr, size, MALLOC_FLAG_NULL);
}

void * krealloc_flag(void* ptr, size_t size, uint32_t flag) {
    if (ptr == NULL) {
        return kmalloc_flag(size, flag);
    }
    if (kheap_is_placement_memory(ptr)) {
        return NULL;
    }
    if ((uintptr_t)ptr < (uintptr_t) kheap_base ||
            (uintptr_t)ptr >= (uintptr_t) kheap_base + KHEAP_HEAP_SIZE) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Pointer is outside heap range.", NULL);
    }
    if ((uintptr_t)ptr < (uintptr_t) kheap_base ||
            (uintptr_t)ptr >= (uintptr_t) kheap_base + KHEAP_HEAP_SIZE) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Pointer is outside heap range.", NULL);
    }

    kheap_block_head_t * block = kheap_get_heap_block(ptr);

    if (!block) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Invalid heap block magic: memory corrupted.", NULL);
    }
    if (block->size >= size) {
        return ptr;
    }

    void * new_ptr = kmalloc_flag(size, flag);

    if (new_ptr == NULL) {
        return NULL;
    }
    memcpy(new_ptr, ptr, block->size);
    kfree(ptr);

    return new_ptr;
}

void kfree(void * ptr) {
    if (ptr == NULL) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Pointer is NULL", NULL);
    }
    if (kheap_is_placement_memory(ptr)) {
        return;
    }
    if ((uintptr_t)ptr < (uintptr_t) kheap_base ||
            (uintptr_t)ptr >= (uintptr_t) kheap_base + KHEAP_HEAP_SIZE) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Pointer is outside heap range.", NULL);
    }

    kheap_block_head_t * free_block = kheap_get_heap_block(ptr);

    if (!free_block) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Invalid heap block magic: memory corrupted.", NULL);
    }
    if (free_block->free) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Double free detected.", NULL);
    }

    free_block->free = true;
    kheap_block_head_t * prev_block = free_block->prev;
    kheap_block_head_t * next_block = free_block->next;

    if (prev_block && prev_block->free) {
        prev_block->next = next_block;
        prev_block->size += (free_block->size + sizeof(kheap_block_head_t));

        free_block->magic = 0;

        if (next_block == NULL) {
            kheap_tail = prev_block;
        } else {
            next_block->prev = prev_block;
        }
        free_block = prev_block;
    }
    if (next_block && next_block->free) {
        kheap_block_head_t * next_2_block = next_block->next;
        free_block->next = next_2_block;
        free_block->size += (next_block->size + sizeof(kheap_block_head_t));

        next_block->magic = 0;

        if (next_2_block == NULL) {
            kheap_tail = free_block;
        } else {
            next_2_block->prev = free_block;
        }
    }
}

void * kheap_find_best_fit(size_t size, bool align) {
    kheap_block_head_t * current = kheap_head;
    kheap_block_head_t * best_fit = NULL;

    while (current != NULL) {
        if (current->free) {
            void * data_addr = (void *)(current + sizeof(kheap_block_head_t));
            void * end_addr = (void *)((uintptr_t)data_addr + current->size);
            if (align && !paging_is_aligned_address(data_addr)) {
                data_addr = paging_align_address_up(
                    data_addr + sizeof(kheap_block_head_t));
            }
            if ((uintptr_t)data_addr + size <= (uintptr_t)end_addr) {
                if (best_fit == NULL || current->size < best_fit->size) {
                    best_fit = current;
                }
            }
        }
        current = current->next;
    }
    return best_fit;
}

bool kheap_is_placement_memory(void * ptr) {
    uint32_t start = (uint32_t)kheap_placement_memory;
    uint32_t end = start + KHEAP_PLACEMENT_SIZE;
    return (uint32_t)ptr >= start && (uint32_t)ptr < end;
}
