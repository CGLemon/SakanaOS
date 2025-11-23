#ifndef _KERNEL_UTILS_TYPES_H
#define _KERNEL_UTILS_TYPES_H

#if !defined(__i386__)
#ifdef __x86_64__
#error "Detected x86_64 build. This header requires 32-bit x86 (i386). Use -m32 or a i686-elf cross-compiler."
#else
#error "Unsupported target: build for 32-bit x86 (i386). Use -m32 or -target i386-elf."
#endif
#endif

typedef unsigned int   __u32;
typedef          int   __s32;
typedef unsigned short __u16;
typedef          short __s16;
typedef unsigned char  __u8;
typedef          char  __s8;

typedef __u32 uint32_t;
typedef __s32 int32_t;
typedef __u16 uint16_t;
typedef __s16 int16_t;
typedef __u8  uint8_t;
typedef __s8  int8_t;

typedef __s32 bool;
#define true 1
#define false 0

typedef __u32 size_t;

typedef __u32 uintptr_t;
typedef __s32 intptr_t;
#define NULL 0

#endif
