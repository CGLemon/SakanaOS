#ifndef _KERNEL_UTILS_ARRAY_H
#define _KERNEL_UTILS_ARRAY_H

#include <utils/types.h>

typedef int (*sort_compare_t)(void * a, void * b);

#define array_foreach(n) \
    do { \
        int i; \
        for(i = 0; i < n; ++i)

#define array_foreach_end \
    } while (0);

void array_swap(void * a, void * b, size_t size);
void array_bubble_sort(void * base, size_t nitems, size_t esize, sort_compare_t comp);


#endif
