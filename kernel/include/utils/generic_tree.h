#ifndef _KERNEL_UTILS_GENERIC_TREE_H
#define _KERNEL_UTILS_GENERIC_TREE_H

#include <utils/types.h>

typedef void * (*tree_allocator_t)(size_t size);
typedef void (*tree_free_t)(void * ptr);
typedef int (*tree_compare_t)(void * a, void * b);

typedef struct generic_node_t {
    struct generic_node_t * parent;
    struct generic_node_t * slide;
    struct generic_node_t * child;
    void * data;
} generic_node_t;

typedef struct generic_tree {
    struct generic_node_t * root;
} generic_tree_t;

generic_tree_t * generic_tree_create(tree_allocator_t allocator);
generic_node_t * generic_node_create(void * data, tree_allocator_t allocator);
void generic_tree_destroy(generic_tree_t * tree,
                          bool free_data, tree_free_t deallocator);

void generic_tree_init(generic_tree_t * tree);
void generic_node_init(generic_node_t * node, void * data);

void generic_tree_insert(generic_node_t * parent, generic_node_t * node);
void generic_tree_remove(generic_tree_t * tree, generic_node_t * node);
generic_node_t * generic_tree_find(generic_tree_t * tree, void * data, tree_compare_t comp);

#endif
