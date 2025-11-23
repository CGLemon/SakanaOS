#include <utils/generic_tree.h>

void _generic_tree_destroy_recursive(generic_node_t * node,
                                     bool free_data, tree_free_t deallocator);
generic_node_t * _generic_tree_find_recursive(generic_node_t * node,
                                              void * data, tree_compare_t comp);

generic_tree_t * generic_tree_create(tree_allocator_t allocator) {
    generic_tree_t * tree = (generic_tree_t *)allocator(sizeof(generic_tree_t));
    if (!tree) {
        return NULL;
    }
    generic_tree_init(tree);

    return tree;
}

generic_node_t * generic_node_create(void * data, tree_allocator_t allocator) {
    generic_node_t * node = (generic_node_t *)allocator(sizeof(generic_node_t));
    if (!node) {
        return NULL;
    }
    generic_node_init(node, data);

    return node;
}

void _generic_tree_destroy_recursive(generic_node_t * node,
                                     bool free_data, tree_free_t deallocator) {
    if (node->child) {
        generic_node_t * curr = node->child;

        while (curr) {
            generic_node_t * slide = curr->slide;
            _generic_tree_destroy_recursive(curr, free_data, deallocator);
            curr = slide;
        }
    }

    if (free_data) {
        deallocator(node->data);
    }

    deallocator(node);
}

void generic_tree_destroy(generic_tree_t * tree,
                          bool free_data, tree_free_t deallocator) {
    if (!tree) {
        return;
    }

    if (tree->root) {
        _generic_tree_destroy_recursive(tree->root, free_data, deallocator);
    }
    deallocator(tree);
}

void generic_tree_init(generic_tree_t * tree) {
    if (tree) {
        tree->root = NULL;
    }
}

void generic_node_init(generic_node_t * node, void * data) {
    if (node) {
        node->parent = NULL;
        node->slide = NULL;
        node->child = NULL;
        node->data = data;
    }
}

void generic_tree_insert(generic_node_t * parent, generic_node_t * node) {
    if (!parent || !node) {
        return;
    }

    if (!parent->child) {
        parent->child = node;
    } else {
        generic_node_t * last = parent->child;
        while (last->slide) {
            last = last->slide;
        }
        last->slide = node;
    }
    node->parent = parent;
}

void generic_tree_remove(generic_tree_t * tree, generic_node_t * node) {
    if (!tree || !node) {
        return;
    }

    generic_node_t * parent = node->parent;
    if (parent) {
        generic_node_t * prev = parent->child;
        generic_node_t * next = node->slide;

        if (prev == node) {
            parent->child = next;
        } else {
            while (prev->slide != node) {
                prev = prev->slide;
            }
            prev->slide = next;
        }
    }
    if (tree->root == node) {
        tree->root = NULL;
    }
    node->parent = NULL;
    node->slide = NULL;
}

generic_node_t * _generic_tree_find_recursive(generic_node_t * node,
                                              void * data, tree_compare_t comp) {
    if (!node || !data) {
        return NULL;
    }

    if (comp(node->data, data)) {
        return node;
    }

    if (node->child) {
        generic_node_t * curr = node->child;

        while (curr) {
            generic_node_t * result =
                _generic_tree_find_recursive(curr, data, comp);
            if (result) {
                return result;
            }
            curr =  curr->slide;
        }
    }

    return NULL;
}

generic_node_t * generic_tree_find(generic_tree_t * tree,
                                   void * data, tree_compare_t comp) {
    if (tree && tree->root) {
        return _generic_tree_find_recursive(tree->root, data, comp);
    }
    return NULL;
}
