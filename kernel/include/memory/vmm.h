#ifndef _KERNEL_MEMORY_VMM_H
#define _KERNEL_MEMORY_VMM_H

#include <memory/mm_layout.h>

void vmm_init();
void * vmm_find_free_memory(size_t size, bool is_kernel);
void * vmm_map_memory(void * virtual_address,
                      void * physical_address,
                      size_t size,
                      bool is_kernel,
                      bool is_writeable);
#endif
