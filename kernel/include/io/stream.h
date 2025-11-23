#ifndef _KERNEL_IO_STRAM_H
#define _KERNEL_IO_STRAM_H

#include <utils/types.h>
#include <utils/string.h>

typedef struct stream {
    void (*putchar)(struct stream * s, char c);
    char (*getchar)(struct stream * s);
    void (*puts)(struct stream * s, const char * str);
    char * (*gets)(struct stream * s);
    void * data;
    char buf[32];
} stream_t;

void stream_putchar(stream_t * s, char c);
char stream_getchar(stream_t * s);
void stream_puts(stream_t * s, const char * str);
char * stream_gets(stream_t * s);

int stream_printf(stream_t* s, const char * format, ...);
int stream_vprintf(stream_t* s, const char * format, va_list args);

#endif
