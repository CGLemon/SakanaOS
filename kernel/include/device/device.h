#ifndef _KERNEL_DEVICE_DEVICE_H
#define _KERNEL_DEVICE_DEVICE_H

#include <utils/types.h>
#include <device/uuid.h>
#include <device/keyboard.h>
#include <device/video.h>

// Bus types

#define DEVICE_BUS_TYPE_PLATFORM    0x00
#define DEVICE_BUS_TYPE_ISA         0x01
#define DEVICE_BUS_TYPE_PCI         0x02
#define DEVICE_BUS_TYPE_USB         0x03
#define DEVICE_BUS_TYPE_RESERVED    0xFF

// Device types

#define DEVICE_TYPE_UNKNOWN     0x0000
#define DEVICE_TYPE_KEYBOARD    0x0100
#define DEVICE_TYPE_STORAGE     0x0200
#define DEVICE_TYPE_VIDEO       0x0300
#define DEVICE_TYPE_CONTROLLER  0x0400
#define DEVICE_TYPE_RESERVED    0xFF00

typedef struct bus {
    uint8_t type;
    void * data;
} __attribute__((packed)) but_t;

typedef struct device {
    uuid_t id;
    char * name;
    uint16_t type;
    struct bus bus;
} __attribute__((packed)) device_t;

typedef struct keyboard_device {
    device_t info;
    keyboard_driver_t * driver;
} __attribute__((packed)) keyboard_device_t;

typedef struct video_device {
    device_t info;
    video_driver_t * driver;
} __attribute__((packed)) video_device_t;

void device_init();

int device_register(device_t * device);
int device_unregister(device_t * device);
const device_t * device_find_by_type(uint16_t type);

#endif
