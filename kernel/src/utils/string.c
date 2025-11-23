#include <utils/string.h>

size_t strlen(const char * str) {
	size_t len = 0;

	while(str && *str != '\0') {
		++str;
		++len;
	}

	return len;
}

char * strcat(char * dest, const char * src) {
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);

    for(size_t i = 0; i < src_len; i++) {
        dest[dest_len + i] = src[i];
    }

    dest[dest_len + src_len] = '\0';
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 && *str1 == *str2) {
        str1++;
        str2++;
    }

    return *(unsigned char*) str1 - *(unsigned char*) str2;
}

char * strcpy(char* dest, const char* src) {
    char* d = dest;
    const char* s = src;

    while ((*d++ = *s++));

    return dest;
}

char tolower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return (c - 'A') + 'a';
    }
    return c;
} 

char toupper(char c) {
    if (c >= 'a' && c <= 'z') {
        return (c - 'a') + 'A';
    }
    return c;
}

bool isalpha(char c) {
    c = tolower(c);
    return c >= 'a' && c <= 'z';
}

bool isdigit(char c) {
    return c >= '0' && c <= '9';
}

bool isalnum(char c) {
    return isalpha(c) || isdigit(c);
}

void * memcpy(void * dest, const void * src, size_t n) {
    char *d = dest;
    const char *s = src;
    while(n--) {
        *d++ = *s++;
    }
    return dest;
}

void * memset(void * dest, uint8_t ch, size_t n) {
    uint8_t *ptr = (uint8_t *) dest;
    uint8_t *end = ptr + n;

    while(ptr != end) {
        *ptr++ = ch;
    }

    return dest;
}

char * itoa(int n, char * str) {
    int i, j, start, end, sign;
    if ((sign = n) < 0) {
        n = -n;
    }

    start = 0;
    i = start;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        str[i++] = '-';
    }
    end = i;
    str[end] = '\0';

    char tmp;
    for (i = start, j = end-1; i < j; i++, j--) {
        tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    return str + end;
}

static int place_at(char * str, char c, int i) {
    str[i] = c;
    str[i+1] = '\0';
    return i+1;
}

char * htoa(uint32_t n, char * str) {
    int len = 0;
    len = place_at(str, '0', len);
    len = place_at(str, 'x', len);
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) {
            continue;
        }
        zeros = 1;
        if (tmp >= 0xA) {
            len = place_at(str, tmp - 0xA + 'a', len);
        } else {
            len = place_at(str, tmp + '0', len);
        }
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) {
        len = place_at(str, tmp - 0xA + 'a', len);
    } else {
        len = place_at(str, tmp + '0', len);
    }
    return str + len;
}

char * ftoa(double n, char * str, int prec) {
    char *p = str;
    uint32_t ipart;
    double fpart;
    int i;

    if (n < 0.0) {
        *p++ = '-';
        n = -n;
    }

    ipart = (uint32_t)n;
    fpart = n - (double)ipart;

    p = itoa(ipart, p);
    if (prec > 0) {
        *p++ = '.';

        for (i = 0; i < prec; ++i) {
            fpart *= 10.0;
            int digit = (int)fpart;
            if (digit < 0) digit = 0;
            if (digit > 9) digit = 9;
            *p++ = '0' + digit;
            fpart -= digit;
            if (fpart < 0.0) fpart = 0.0;
        }
    }
    *p = '\0';
    return p;
}

int vsprintf(char * str, const char * fmt, va_list args) {
    char * p = str;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    p = itoa(va_arg(args, int32_t), p);
                    break;
                case 'x':
                case 'p':
                    p = htoa(va_arg(args, uint32_t), p);
                    break;
                case 'f':
                    p = ftoa(va_arg(args, double), p, 6);
                    break;
                case 's':
                    char * src = va_arg(args, char *);
                    strcpy(p, src);
                    p += strlen(src);
                    break;
                case 'c':
                    *p++ = (char)va_arg(args, int);
                    break;
                default:
                    *p++ = *fmt;
            }
        } else {
            *p++ = *fmt;
        }
        fmt++;
    }

    *p = '\0';
    return p - str;
}

int sprintf(char * str, const char * format, ... ) {
    va_list args;
    va_start(args, format);
    int ret = vsprintf(str, format, args);
    va_end(args);
    return ret;
}
