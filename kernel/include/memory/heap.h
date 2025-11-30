#ifndef _KERNEL_MEMORY_HEAP_H
#define _KERNEL_MEMORY_HEAP_H

#include <utils/types.h>

#define KHEAP_PLACEMENT_SIZE 0x00100000
#define KHEAP_HEAP_SIZE      0x02000000
#define KHEAP_MAGIC          0xD11EF00D

#define MALLOC_FLAG_NULL  0x0
#define MALLOC_FLAG_ALIGN 0x1
#define MALLOC_FLAG_INIT  0x2

typedef struct kheap_block_head {
    uint32_t magic;
    size_t size;
    bool free;
    struct kheap_block_head * prev;
    struct kheap_block_head * next;
} kheap_block_head_t;

void kheap_init();
bool kheap_is_enabled();
kheap_block_head_t * kheap_get_heap_head();

void * kmalloc(size_t size);
void * kmalloc_flag(size_t size, uint32_t flag);
void * krealloc(void* ptr, size_t size);
void * krealloc_flag(void* ptr, size_t size, uint32_t flag);

void kfree(void * ptr);

#endif
