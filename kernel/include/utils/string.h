#ifndef _KERNEL_UTILS_STRING_H
#define _KERNEL_UTILS_STRING_H

#include <utils/types.h>

typedef __builtin_va_list va_list;
#define va_start(ap, last)  __builtin_va_start(ap, last)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
#define va_end(ap)          __builtin_va_end(ap)
#define va_copy(dest, src)  __builtin_va_copy(dest, src)

size_t strlen(const char * str);
char * strcat(char * dest, const char * src);
int strcmp(const char* str1, const char * str2);
char * strcpy(char* dest, const char * src);
char * strchr(char * str, char c);
char * strtok(char * str, const char * delim);

char toupper(char c);
char tolower(char c);
bool isalpha(char c);
bool isdigit(char c);
bool isalnum(char c);

void * memcpy(void * dest, const void * src, size_t n);
void * memset(void * dest, uint8_t ch, size_t n);

int atoi(char * str);

char * itoa(int n, char * str);
char * htoa(uint32_t n, char * str);
char * ftoa(double n, char * str, int prec);
int vsprintf(char * str, const char * fmt, va_list args);
int sprintf(char * str, const char * format, ... );

#endif
