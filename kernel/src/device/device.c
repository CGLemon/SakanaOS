#include <device/device.h>
#include <device/uuid.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <utils/generic_tree.h>
#include <utils/string.h>

generic_tree_t * device_tree;

int _device_register_compare(void * node_data, void * compare_data);
int _device_type_compare(void * node_data, void * compare_data);

void device_init() {
    device_tree = generic_tree_create(kmalloc);

    if (!device_tree) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device_t * root_device = (device_t *) kmalloc(sizeof(device_t));
    if (!root_device) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    root_device->name = (char*) kmalloc(9);
    if (!root_device->name) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    generate_uuid_v4(&root_device->id);
    strcpy(root_device->name, "Computer");
    root_device->type = DEVICE_TYPE_RESERVED;
    root_device->bus.type = DEVICE_BUS_TYPE_RESERVED;
    root_device->bus.data = NULL;

    generic_node_t * root_node = generic_node_create(root_device, kmalloc);
    if (!root_node) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    device_tree->root = root_node;
}

int _device_register_compare(void * node_data, void * compare_data) {
    device_t * node_device = (device_t *) node_data;
    device_t * compare_device = (device_t *) compare_data;

    return node_device == compare_device;
}

int device_register(device_t * device) {
    generic_node_t * parent_node = device_tree->root;
    generic_node_t * device_node = generic_node_create(device, kmalloc);

    if (!device_node) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    generic_tree_insert(parent_node, device_node);

    return 0;
}

int device_unregister(device_t * device) {
    generic_node_t * device_node =
        generic_tree_find(device_tree, device, _device_register_compare);

    if (!device_node) {
        return -1;
    }
    generic_tree_remove(device_tree, device_node);

    return 0;
}

int _device_type_compare(void * node_data, void * compare_data) {
    device_t * node_device = (device_t *) node_data;
    uint16_t * compare_type = (uint16_t *) compare_data;

    return node_device->type == (*compare_type);
}

const device_t * device_find_by_type(uint16_t type) {
    generic_node_t * device_node =
        generic_tree_find(device_tree, (void *)(&type), _device_type_compare);
    if (!device_node) {
        return NULL;
    }
    return (device_t *)(device_node->data);
}
