#include <utils/rand.h>

static unsigned long next = 1;

void rand_set_seed(unsigned int seed) {
    next = seed;
}

int rand_next(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int) (next / 65536) % 32768;
}
