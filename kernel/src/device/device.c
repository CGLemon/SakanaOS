#include <device/device.h>
#include <device/uuid.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <utils/generic_tree.h>
#include <utils/string.h>

typedef struct _device_generic_payload {
    linked_list_t * devices;
    uint32_t type;
} _device_generic_payload_t;

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

    generic_tree_node_t * root_node =
        generic_tree_node_create(root_device, kmalloc);
    if (!root_node) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    device_tree->root = root_node;
}

int _device_register_compare(void * node_data, void * compare_data) {
    device_t * node_device = (device_t *)node_data;
    device_t * compare_device = (device_t *)compare_data;

    return node_device == compare_device;
}

int device_register(device_t * device) {
    generic_tree_node_t * parent_node = device_tree->root;
    generic_tree_node_t * device_node = generic_tree_node_create(device, kmalloc);

    if (!device_node) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    generic_tree_insert(parent_node, device_node);

    return 0;
}

int device_register_at(device_t * device, device_t * parent) {
    generic_tree_node_t * parent_node =
        generic_tree_find(device_tree, parent, _device_register_compare);
    if (!parent_node) {
        return -1;
    }

    generic_tree_node_t * device_node = generic_tree_node_create(device, kmalloc);

    if (!device_node) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    generic_tree_insert(parent_node, device_node);

    return 0;   
}

int device_unregister(device_t * device) {
    generic_tree_node_t * device_node =
        generic_tree_find(device_tree, device, _device_register_compare);

    if (!device_node) {
        return -1;
    }
    generic_tree_remove(device_tree, device_node);
    kfree(device_node);

    return 0;
}

int _device_type_compare(void * node_data, void * compare_data) {
    device_t * node_device = (device_t *) node_data;
    uint16_t * compare_type = (uint16_t *) compare_data;

    return node_device->type == (*compare_type);
}

const device_t * device_find_by_type(uint16_t type) {
    generic_tree_node_t * device_node =
        generic_tree_find(device_tree, (void *)(&type), _device_type_compare);
    if (!device_node) {
        return NULL;
    }
    return (device_t *)(device_node->data);
}

void device_find_all_by_type_callback(generic_tree_node_t * device_node, void * userdata) {
    _device_generic_payload_t * payload = (_device_generic_payload_t *)userdata;
    device_t * device = (device_t *)(device_node->data);

    if (device->type == (uint16_t)(payload->type)) {
        linked_list_node_t * node =
            linked_list_node_create(device_node->data, kmalloc);
        if (!node) {
            KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
        }
        linked_list_append(payload->devices, node);
    }
}

linked_list_t * device_find_all_by_type(uint16_t type) {
    linked_list_t * devices = linked_list_create(kmalloc);
    if (!devices) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    _device_generic_payload_t payload;
    payload.devices = devices;
    payload.type = type;

    generic_tree_callback_foreach(
        device_tree, device_find_all_by_type_callback, &payload);

    return devices;
}

void device_find_all_by_bustype_callback(generic_tree_node_t * device_node, void * userdata) {
    _device_generic_payload_t * payload = (_device_generic_payload_t *)userdata;
    device_t * device = (device_t *)(device_node->data);

    if (device->bus.type == (uint8_t)(payload->type)) {
        linked_list_node_t * node =
            linked_list_node_create(device_node->data, kmalloc);
        if (!node) {
            KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
        }
        linked_list_append(payload->devices, node);
    }
}

linked_list_t * device_find_all_by_bustype(uint8_t type) {
    linked_list_t * devices = linked_list_create(kmalloc);
    if (!devices) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    _device_generic_payload_t payload;
    payload.devices = devices;
    payload.type = type;

    generic_tree_callback_foreach(
        device_tree, device_find_all_by_bustype_callback, &payload);

    return devices;
}
