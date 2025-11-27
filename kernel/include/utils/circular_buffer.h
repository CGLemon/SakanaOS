#ifndef _KERNEL_UITLS_CIRCULAR_BUFFER_H
#define _KERNEL_UITLS_CIRCULAR_BUFFER_H

#include <utils/types.h>

typedef void * (*cb_allocator_t)(size_t size);
typedef void (*cb_free_t)(void * ptr);

typedef struct circular_buffer {
    void * buffer;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t element_size;
    size_t size;
} circular_buffer_t;

#define circular_buffer_foreach(cb) \
    do { \
        size_t i; \
        void * element = circular_buffer_get(cb, 0); \
        for(i = 0; i < cb->size; element = circular_buffer_get(cb, ++i))

#define circular_buffer_foreach_end \
    } while (0);

circular_buffer_t * circular_buffer_create(size_t capacity,
                                           size_t element_size,
                                           cb_allocator_t allocator);
void circular_buffer_destroy(circular_buffer_t * cb,
                             cb_free_t deallocator);

bool circular_buffer_init(circular_buffer_t * cb,
                          size_t capacity,
                          size_t element_size,
                          cb_allocator_t allocator);

void circular_buffer_enqueue(circular_buffer_t * cb, void * element);
void circular_buffer_dequeue(circular_buffer_t * cb, void * element);
bool circular_buffer_full(circular_buffer_t * cb);
bool circular_buffer_empty(circular_buffer_t * cb);
void * circular_buffer_get(circular_buffer_t * cb, size_t index);

#endif
