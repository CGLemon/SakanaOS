#ifndef _KERNEL_UTILS_RAND_H
#define _KERNEL_UTILS_RAND_H

#define RAND_MAX 32767

void rand_seed(unsigned int seed);
int rand_next(void);

#endif
