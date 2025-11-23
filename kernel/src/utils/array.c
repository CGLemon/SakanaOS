#include <utils/array.h>

void array_swap(void * a, void * b, size_t size) {
    int i;
    uint8_t tmp;
    uint8_t * aptr = (uint8_t *)a;
    uint8_t * bptr = (uint8_t *)b; 

    for (i = 0; i < size; ++i) {
        tmp = aptr[i];
        aptr[i] = bptr[i];
        bptr[i] = tmp;
    }
}

void array_bubble_sort(void * base, size_t nitems, size_t esize, sort_compare_t comp) {
    bool swapped;
    int i, j;
    uintptr_t bptr = (uintptr_t)base;

    for (i = 0; i < nitems - 1; i++) {
        swapped = false;

        for (j = 0; j < nitems - 1 - i; j++) {
            void * a = (void *)(bptr + j * esize);
            void * b = (void *)(bptr + (j+1) * esize);

            if (comp(a, b) > 0) {
                array_swap(a, b, esize);
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }
    }
}
