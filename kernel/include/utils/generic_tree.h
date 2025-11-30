#ifndef _KERNEL_UTILS_GENERIC_TREE_H
#define _KERNEL_UTILS_GENERIC_TREE_H

#include <utils/types.h>

struct generic_tree_node;

typedef void * (*tree_allocator_t)(size_t size);
typedef void (*tree_free_t)(void * ptr);
typedef int (*tree_compare_t)(void * a, void * b);
typedef void (*tree_callback_t)(struct generic_tree_node * node, void * userdata);

typedef struct generic_tree_node {
    struct generic_tree_node * parent;
    struct generic_tree_node * slide;
    struct generic_tree_node * child;
    void * data;
} generic_tree_node_t;

typedef struct generic_tree {
    struct generic_tree_node * root;
} generic_tree_t;

generic_tree_t * generic_tree_create(tree_allocator_t allocator);
generic_tree_node_t * generic_tree_node_create(void * data, tree_allocator_t allocator);
void generic_tree_destroy(generic_tree_t * tree,
                          bool free_data, tree_free_t deallocator);

void generic_tree_init(generic_tree_t * tree);
void generic_tree_node_init(generic_tree_node_t * node, void * data);

void generic_tree_insert(generic_tree_node_t * parent, generic_tree_node_t * node);
void generic_tree_remove(generic_tree_t * tree, generic_tree_node_t * node);
generic_tree_node_t * generic_tree_find(generic_tree_t * tree, void * data, tree_compare_t comp);

void generic_tree_callback_foreach(generic_tree_t * tree,
                                   tree_callback_t callback,
                                   void * userdata);

#endif
