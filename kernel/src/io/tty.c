#include <io/tty.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <utils/string.h>
#include <drivers/video/vga/vga.h>
#include <drivers/input/ps2/keyboard.h>
#include <device/keyboard.h>

static tty_t * tty_stdterm = NULL;
const tty_t * tty_get_stdterm();

void tty_keyboard_listener(keyboard_event_t * event);

void tty_putchar(tty_t * tty, char c);
char tty_getchar(tty_t * tty);
void tty_puts(tty_t * tty, const char * str);
char * tty_gets(tty_t * tty);

void tty_stream_putchar(stream_t * stream, char c);
char tty_stream_getchar(stream_t* stream);
void tty_stream_puts(stream_t* stream, const char * str);
char * tty_stream_gets(stream_t* stream);

void tty_set_stdterm(tty_t * tty) {
    tty_stdterm = tty;
}

const tty_t * tty_get_stdterm() {
    return tty_stdterm;
}

tty_t * tty_create() {
    tty_t * tty = (tty_t *)kmalloc(sizeof(tty_t));
    if (!tty) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    tty->input = circular_buffer_create(TTY_BUFFER_SIZE, sizeof(char), kmalloc);
    if (!tty->input) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    tty->output = circular_buffer_create(TTY_BUFFER_SIZE, sizeof(char), kmalloc);
    if (!tty->output) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    if (tty_get_stdterm() == NULL) {
        tty_set_stdterm(tty);
    }
    tty->keyboard = (keyboard_device_t *)device_find_by_type(DEVICE_TYPE_KEYBOARD);
    tty->keyboard->driver->register_listener(tty_keyboard_listener);

    tty->video = (video_device_t *)device_find_by_type(DEVICE_TYPE_VIDEO);

    return tty;
}

void tty_destroy(tty_t * tty) {
    kfree(tty);
}

void tty_keyboard_listener(keyboard_event_t * event)  {
    const tty_t * tty = tty_get_stdterm();
    if (!tty) {
        return;
    }
    char c = (char)(event->keycode);
    if (c && event->pressed) {
        circular_buffer_enqueue(tty->input, &c);
    }
}

char * tty_gets(tty_t * tty) {
    size_t buf_size = 2;
    size_t buf_idx = 0;
    char * buf = (char *)kmalloc(buf_size);
    char c;

    if (!buf) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    while (true) {
        c = tty_getchar(tty);

        if (c == -1 || c == '\0') {
            continue;
        }
        if (c == ASCII_CTRL_CR) {
            tty_putchar(tty, '\n');
            break;
        }
        if (c == ASCII_CTRL_BS && buf_idx > 0) {
            buf_idx--;
            buf[buf_idx] = '\0';
            tty_putchar(tty, c);
            continue;
        }

        if (isalnum(c) || c == ASCII_SPACE) {
            if (buf_idx == buf_size-1) {
                buf_size *= 2;
                buf = krealloc(buf, buf_size);
            }
            buf[buf_idx] = c;
            buf_idx++;
            tty_putchar(tty, c);
        }
    }
    buf[buf_idx] = '\0';

    return buf;
}

void tty_puts(tty_t * tty, const char * str) {
    size_t size = strlen(str);
    for (size_t i = 0; i < size; ++i) {
        tty_putchar(tty, str[i]);
    }
}

char tty_getchar(tty_t * tty) {
    char c;
    if (circular_buffer_empty(tty->input)) {
        return -1;
    }

    circular_buffer_dequeue(tty->input, &c);
    return c;
}

void tty_putchar(tty_t * tty, char c) {
    circular_buffer_enqueue(tty->output, &c);
    tty_flush(tty);
}

void tty_flush(tty_t * tty) {
    while (!circular_buffer_empty(tty->output)) {
        char ch = 0;
        circular_buffer_dequeue(tty->output, &ch);
        tty_render(tty, ch);
    }
}

void tty_render(tty_t * tty, char c) {
    tty->video->driver->tm.write(c);
}

void tty_stream_putchar(stream_t * s, char c) {
    tty_putchar((tty_t *)(s->data), c);
}

char tty_stream_getchar(stream_t * s) {
    return tty_getchar((tty_t *)(s->data));
}

void tty_stream_puts(stream_t * s, const char * str) {
    tty_puts((tty_t *)(s->data), str);
}

char * tty_stream_gets(stream_t * s) {
    return tty_gets((tty_t *)(s->data));
}

stream_t * tty_get_in_stream(tty_t * tty) {
    stream_t * s = (stream_t *)kmalloc(sizeof(stream_t));
    if (!s) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    s->putchar = NULL;
    s->getchar = tty_stream_getchar;
    s->puts = NULL;
    s->gets = tty_stream_gets;
    s->data = (void *)tty;
    return s;
}

stream_t * tty_get_out_stream(tty_t * tty) {
    stream_t * s = (stream_t *)kmalloc(sizeof(stream_t));
    if (!s) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    s->putchar = tty_stream_putchar;
    s->getchar = NULL;
    s->puts = tty_stream_puts;
    s->gets = NULL;
    s->data = (void *)tty;
    return s;
}

stream_t * tty_get_err_stream(tty_t * tty) {
    stream_t * s = (stream_t *)kmalloc(sizeof(stream_t));
    if (!s) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    s->putchar = tty_stream_putchar;
    s->getchar = NULL;
    s->puts = tty_stream_puts;
    s->gets = NULL;
    s->data = (void *)tty;
    return s;
}
