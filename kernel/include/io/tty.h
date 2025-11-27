#ifndef _KERNEL_IO_TTY_H
#define _KERNEL_IO_TTY_H

#include <utils/types.h>
#include <utils/circular_buffer.h>
#include <device/device.h>
#include <device/keyboard.h>
#include <io/stream.h>

#define TTY_BUFFER_SIZE 1024

typedef struct tty {
    size_t rows;
    size_t columns;
    size_t cursor_x;
    size_t cursor_y;
    uint8_t fgcolor;
    uint8_t bgcolor;
    circular_buffer_t * input;
    circular_buffer_t * output;
    // video_device_t * video;
    keyboard_device_t * keyboard;
    // tty_keyboard_layout_t * layout;
} tty_t;

tty_t * tty_create();
void tty_destroy(tty_t * tty);

void tty_putchar(tty_t * tty, char c);
void tty_flush(tty_t * tty);
void tty_render(tty_t * tty, char c);

stream_t * tty_get_in_stream(tty_t * tty);
stream_t * tty_get_out_stream(tty_t * tty);
stream_t * tty_get_err_stream(tty_t * tty);

#endif
