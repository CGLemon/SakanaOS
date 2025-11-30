#ifndef _KERNEL_DEVICE_STORAGE_H
#define _KERNEL_DEVICE_STORAGE_H

#include <utils/types.h>

typedef struct storage_driver {
    size_t (*sector_size)();
    size_t (*total_size)();
    size_t (*read)(uint32_t addr, void * buf, size_t size);
    size_t (*write)(uint32_t addr, void * buf, size_t size);
} storage_driver_t;

#endif
