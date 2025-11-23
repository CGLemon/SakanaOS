#include <io/stream.h>

static int stream_putint(stream_t * s, int n);
static int stream_puthex(stream_t * s, uint32_t n);
static int stream_putfloat(stream_t * s, double n, int prec);

void stream_putchar(stream_t * s, char c) {
    if (s && s->putchar) {
        s->putchar(s, c);
    }
}

void stream_puts(stream_t * s, const char* str) {
    if (s && s->puts) {
        s->puts(s, str);
    }
}

int stream_putint(stream_t * s, int n) {
    char * buf = s->buf;
    itoa(n, buf);

    size_t size = strlen(buf);
    for (int i = 0; i < size; ++i) {
        stream_putchar(s, buf[i]);
    }
    return (int)size;
}

int stream_puthex(stream_t * s, uint32_t n) {
    char * buf = s->buf;
    htoa(n, buf);

    size_t size = strlen(buf);
    for (int i = 0; i < size; ++i) {
        stream_putchar(s, buf[i]);
    }
    return (int)size;
}

int stream_putfloat(stream_t * s, double n, int prec) {
    char * buf = s->buf;
    ftoa(n, buf, prec);

    size_t size = strlen(buf);
    for (int i = 0; i < size; ++i) {
        stream_putchar(s, buf[i]);
    }
    return (int)size;
}

char stream_getchar(stream_t * s) {
    if (s && s->getchar) {
        return s->getchar(s);
    }
}

char * stream_gets(stream_t * s) {
    if (s && s->gets) {
        return s->gets(s);
    }
}

int stream_printf(stream_t * s, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int value = stream_vprintf(s, format, args);
    va_end(args);

    return value;
}

int stream_vprintf(stream_t * s, const char * fmt, va_list args) {
    int count = 0;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    count += stream_putint(s, va_arg(args, int32_t));
                    break;
                case 'x':
                case 'p':
                    count += stream_puthex(s, va_arg(args, uint32_t));
                    break;
                case 'f':
                    count += stream_putfloat(s, va_arg(args, double), 6);
                    break;
                case 's':
                    char * src = va_arg(args, char *);
                    while (*src != '\0') {
                        stream_putchar(s, *src);
                        count++;
                        src++;
                    }
                    break;
                case 'c':
                    stream_putchar(s, (char)va_arg(args, int));
                    count++;
                    break;
                default:
                    stream_putchar(s, *fmt);
                    count++;
            }
        } else {
            stream_putchar(s, *fmt);
            count++;
        }
        fmt++;
    }
    return count;
}
