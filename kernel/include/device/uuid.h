#ifndef _KERNEL_DEVICE_UUID_H
#define _KERNEL_DEVICE_UUID_H

#include <utils/types.h>

typedef struct uuid uuid_t;

struct uuid {
    uint8_t bytes[16];
};

void generate_uuid_v4(uuid_t * uuid);
void uuid_v4_to_string(uuid_t * uuid, char * buffer);
int uuid_v4_compare(uuid_t * uuid1, uuid_t * uuid2);

#endif
