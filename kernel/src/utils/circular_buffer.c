#include <utils/circular_buffer.h>
#include <utils/string.h>

circular_buffer_t * circular_buffer_create(size_t capacity,
                                           size_t element_size,
                                           cb_allocator_t allocator) {
    circular_buffer_t * cb = allocator(sizeof(circular_buffer_t));

    if (!circular_buffer_init(cb, capacity, element_size, allocator)) {
        return NULL;
    }
    return cb;
}

void circular_buffer_destroy(circular_buffer_t * cb,
                             cb_free_t deallocator) {
    deallocator(cb->buffer);
    deallocator(cb);
}

bool circular_buffer_init(circular_buffer_t * cb,
                          size_t capacity,
                          size_t element_size,
                          cb_allocator_t allocator) {
    cb->buffer = allocator(capacity * element_size);
    if (!cb->buffer) {
        return false;
    }
    memset(cb->buffer, 0, capacity * element_size);

    cb->capacity = capacity;
    cb->element_size = element_size;
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    return true;
}

void circular_buffer_enqueue(circular_buffer_t * cb, void * element) {
    if (circular_buffer_full(cb)) {
        return;
    }

    size_t offset = cb->tail * cb->element_size;
    void * bufstart =  (void *)((uint8_t *)(cb->buffer) + offset);

    if (element) {
        memcpy(bufstart, element, cb->element_size);
    }
    cb->size += 1;
    cb->tail = (1 + cb->tail) % cb->capacity;
}

void circular_buffer_dequeue(circular_buffer_t * cb, void * element) {
    if (circular_buffer_empty(cb)) {
        return;
    }

    size_t offset = cb->head * cb->element_size;
    void * bufstart =  (void *)((uint8_t *)(cb->buffer) + offset);

    if (element) {
        memcpy(element, bufstart, cb->element_size);
    }
    cb->size -= 1;
    cb->head = (cb->head + 1) % cb->capacity;
}

bool circular_buffer_full(circular_buffer_t * cb) {
    return cb->size == cb->capacity;
}

bool circular_buffer_empty(circular_buffer_t * cb) {
    return cb->size == 0;
}

void * circular_buffer_get(circular_buffer_t * cb, size_t index) {
    size_t offset_idx = (index + cb->head) % cb->capacity;
    size_t offset = offset_idx * cb->element_size;
    void * element =  (void *)((uint8_t *)(cb->buffer) + offset);
    return element;
}
