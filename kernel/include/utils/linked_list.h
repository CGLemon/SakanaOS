#ifndef _KERNEL_UTILS_LINKED_LIST_H
#define _KERNEL_UTILS_LINKED_LIST_H

#include <utils/types.h>

typedef void * (*list_allocator_t)(size_t size);
typedef void (*list_free_t)(void * ptr);
typedef int (*list_compare_t)(void * a, void * b);

typedef struct linked_list_node {
    struct linked_list_node * prev;
    struct linked_list_node * next;
    void * data;
} linked_list_node_t;

typedef struct linked_list {
    struct linked_list_node * head;
    struct linked_list_node * tail;
    size_t size;
} linked_list_t;

#define linked_list_foreach(list) \
    do { \
        linked_list_node_t * node; \
        for(node = list->head; node != NULL; node = node->next)

#define linked_list_foreach_end \
    } while (0);

linked_list_t * linked_list_create(list_allocator_t allocator);
linked_list_node_t * linked_list_node_create(void * data, list_allocator_t allocator);
void linked_list_destroy(linked_list_t * list, bool free_data, list_free_t deallocator);

bool linked_list_init(linked_list_t * list);
bool linked_list_node_init(linked_list_node_t * node, void * data);

bool linked_list_empty(linked_list_t * list);
void linked_list_append(linked_list_t * list, linked_list_node_t * node);
void linked_list_remove(linked_list_t * list, linked_list_node_t * node);
linked_list_node_t * linked_list_pop_head(linked_list_t * list);
linked_list_node_t * linked_list_pop_tail(linked_list_t * list);
void linked_list_sort(linked_list_t * list, list_compare_t comp);

void linked_list_node_swap(linked_list_node_t * a, linked_list_node_t * b);

#endif
