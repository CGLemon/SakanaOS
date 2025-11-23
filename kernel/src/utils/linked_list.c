#include <utils/linked_list.h>

linked_list_t * linked_list_create(list_allocator_t allocator) {
    linked_list_t * list =
        (linked_list_t *)allocator(sizeof(linked_list_t));
    if (!linked_list_init(list)) {
        return NULL;
    }
    return list;
}

linked_list_node_t * linked_list_node_create(void * data, list_allocator_t allocator) {
    linked_list_node_t * node =
        (linked_list_node_t *)allocator(sizeof(linked_list_node_t));
    if (!linked_list_node_init(node, data)) {
        return NULL;
    }
    return node;
}

void linked_list_destroy(linked_list_t * list, bool free_data, list_free_t deallocator) {
    if (!list) {
        return;
    }
    linked_list_foreach(list) {
        if (free_data) {
            deallocator(node->data);
        }
        deallocator(node);
    } linked_list_foreach_end;

    deallocator(list);
}

bool linked_list_init(linked_list_t * list) {
    if (!list) {
        return false;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return true;
}

bool linked_list_node_init(linked_list_node_t * node, void * data) {
    if (!node) {
        return false;
    }

    node->prev = NULL;
    node->next = NULL;
    node->data = data;
    return true;
}

bool linked_list_empty(linked_list_t * list) {
    return list->head == NULL;
}

void linked_list_append(linked_list_t * list, linked_list_node_t * node) {
    if (!list || !node) {
        return;
    }

    if (linked_list_empty(list)) {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
    } else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->size += 1;
}

void linked_list_remove(linked_list_t * list, linked_list_node_t * node) {
    if (!list || !node) {
        return;
    }

    if (list->head == node) {
        list->head = node->next;
    }

    if (list->tail == node) {
        list->tail = node->prev;
    }

    if (node->prev) {
        node->prev->next = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    }

    list->size -= 1;
    node->prev = NULL;
    node->next = NULL;
}

linked_list_node_t * linked_list_pop_head(linked_list_t * list) {
    if (!list) {
        return NULL;
    }
    linked_list_node_t * head = list->head;
    linked_list_remove(list, head);

    return head;
}

linked_list_node_t * linked_list_pop_tail(linked_list_t * list) {
    if (!list) {
        return NULL;
    }
    linked_list_node_t * tail = list->head;
    linked_list_remove(list, tail);

    return tail;
}

void linked_list_sort(linked_list_t * list, list_compare_t comp) {
    if (!list) {
        return;
    }

    linked_list_node_t * node;
    linked_list_node_t * next;
    for (node = list->head; node != NULL; node = node->next) {
        for (next = node->next; next != NULL; next = next->next) {
            if (comp(node->data, next->data) > 0) {
                linked_list_node_swap(node, next);
            }
        }
    }
}

void linked_list_node_swap(linked_list_node_t * a, linked_list_node_t * b) {
    void * temp = a->data;
    a->data = b->data;
    b->data = temp;
}
