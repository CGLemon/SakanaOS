#ifndef _KERNEL_DEVICE_VIDEO_H
#define _KERNEL_DEVICE_VIDEO_H

#include <utils/types.h>

typedef struct video_driver {
    bool (*tm_probe)();

    struct {
        size_t (*get_width)();
        size_t (*get_height)();
        void (*clean_screen)();
        void (*write)(char c);
        void (*strwrite)(const char * data);
        void (*enable_cursor)(uint8_t start, uint8_t end);
        void (*disable_cursor)();
    } tm;
} video_driver_t;

#endif
