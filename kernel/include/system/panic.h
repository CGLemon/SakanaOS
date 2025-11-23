#ifndef _KERNEL_SYSTEN_PANIC_H
#define _KERNEL_SYSTEN_PANIC_H

#include <utils/types.h>
#include <arch/i386/isr.h>

#define KPANIC(code, message, info) kpanic(code, message, __FILE__, __LINE__, info)

#define KPANIC_OUT_OF_MEMORY 0

void kpanic(int code, const char * message, const char* file, uint32_t line, isr_cpu_state_t * state);

#endif
