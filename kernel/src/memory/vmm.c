#include <memory/heap.h>
#include <memory/vmm.h>
#include <memory/pmm.h>
#include <arch/i386/paging.h>
#include <system/message.h>
#include <system/panic.h>
#include <utils/numeric.h>

static page_directory_t * kernel_page_directory = NULL;
static page_directory_t * curr_page_directory = NULL;

page_table_entry_t * vmm_get_page_entry(const page_directory_t * const page_directory,
                                        void * const virtual_address);

void vmm_map_page(page_directory_t * const page_directory,
                  page_directory_t * const page_directory_dict,
                  void * const virtual_address,
                  void * physical_address,
                  bool is_kernel,
                  bool is_writeable);

void * vmm_virtual_to_physical_address(const page_directory_t * const page_directory,
                                       void * const virtual_address);

bool vmm_is_page_used(const page_directory_t * const page_directory,
                      void * const virtual_address);

void vmm_unmap_memory(void * virtual_address, size_t size);
void vmm_unmap_page(void * const virtual_address);

void vmm_init() {
    kernel_page_directory = (page_directory_t *)kmalloc_flag(
        sizeof(page_directory_t), MALLOC_FLAG_ALIGN | MALLOC_FLAG_INIT);
    if (!kernel_page_directory) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    uint32_t num_pages, kernel_memory_size;

    // Mapping real mode memory (Physical: 0x00000000 - 0x000FFFFF)
    num_pages = div_up(VMM_REAL_MODE_MEMORY_SIZE, PAGE_SIZE);
    for (uint32_t i = 0; i < num_pages; ++i) {
        uint32_t virtual_address = VMM_REAL_MODE_MEMORY_BASE + i * PAGE_SIZE;
        uint32_t physical_address =
            (uint32_t)vmm_virtual_to_physical_address(NULL, (void *)virtual_address);

        vmm_map_page(kernel_page_directory, NULL,
                     (void *)virtual_address,
                     (void *)physical_address,
                     true, true);
        pmm_mark_frame_reserved((void *)physical_address);
    }

    // Mapping kernel memory (Physical: 0x00100000 - 0x????????)
    kernel_memory_size = (uint32_t)kernel_virtual_end - (uint32_t)kernel_virtual_start;
    num_pages = div_up(kernel_memory_size, PAGE_SIZE);
    for (uint32_t i = 0; i < num_pages; ++i) {
        uint32_t virtual_address = (uint32_t)kernel_virtual_start + i * PAGE_SIZE;
        uint32_t physical_address =
            (uint32_t)vmm_virtual_to_physical_address(NULL, (void *)virtual_address);
        
        vmm_map_page(kernel_page_directory, NULL,
                     (void *)virtual_address,
                     (void *)physical_address,
                     true, true);
        pmm_mark_frame_reserved((void *)physical_address);
    }

    paging_switch_page_directory(
        vmm_virtual_to_physical_address(NULL, kernel_page_directory));
    curr_page_directory = kernel_page_directory;

    paging_enable();
    kmessage(KMESSAGE_LEVEL_INFO, "memory: VMM initialized");
}


page_table_entry_t * vmm_get_page_entry(const page_directory_t * const page_directory,
                                        void * const virtual_address) {
    if (!page_directory) {
        return NULL;
    }
    uint32_t page_directory_index = paging_directory_index(virtual_address);
    uint32_t page_table_index = paging_table_index(virtual_address);

    page_table_t * table = page_directory->tables[page_directory_index];
    if (!table) {
        return NULL;
    }

    return &table->entries[page_table_index];
}

void * vmm_virtual_to_physical_address(const page_directory_t * const page_directory,
                                       void * const virtual_address) {
    if (!page_directory) {
        // Before paging is enabled, the memory mapping is linear.
        return (void*) (virtual_address - VMM_KERNEL_SPACE_BASE);
    }
    page_table_entry_t * entry = vmm_get_page_entry(page_directory, virtual_address);

    uint32_t page_offset = paging_memory_offset(virtual_address);
    uint32_t present = entry->present;
    uint32_t base = entry->page_base;
    if (!present) {
        return NULL;
    }
    return (void*) ((base << 12) + page_offset);
}

void vmm_map_page(page_directory_t * const page_directory,
                  page_directory_t * const page_directory_dict,
                  void * const virtual_address,
                  void * physical_address,
                  bool is_kernel,
                  bool is_writeable) {
    page_table_t * table = NULL;
    uint32_t page_directory_index = paging_directory_index(virtual_address);
    uint32_t page_table_index = paging_table_index(virtual_address);

    // Allocate a new page table if it does not exist
    if (page_directory->tables[page_directory_index] == NULL) {
        table = (page_table_t *)kmalloc_flag(
            sizeof(page_table_t), MALLOC_FLAG_ALIGN | MALLOC_FLAG_INIT);
        if (!table) {
            KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
        }

        uint32_t table_physical_address =
            (uint32_t)vmm_virtual_to_physical_address(page_directory_dict, table);

        page_directory->entries[page_directory_index].present = 1;
        page_directory->entries[page_directory_index].read_write = 1;
        page_directory->entries[page_directory_index].user_supervisor = 1;
        page_directory->entries[page_directory_index].page_table_base = table_physical_address >> 12;
        page_directory->entries[page_directory_index].page_size = 0;

        page_directory->tables[page_directory_index] = table;
    } else {
        table = page_directory->tables[page_directory_index];
    }

    if (table->entries[page_table_index].present) {
        paging_invalidate_page(virtual_address);
    }

    uint32_t frame_index = pmm_address_to_index(physical_address);

    table->entries[page_table_index].present = 1;
    table->entries[page_table_index].read_write = is_writeable ? 1 : 0;
    table->entries[page_table_index].user_supervisor = is_kernel ? 0 : 1;
    table->entries[page_table_index].available = PAGE_FLAG_USED | PAGE_FLAG_PRESENT;
    table->entries[page_table_index].page_base = frame_index;
}

bool vmm_is_page_used(const page_directory_t * const page_directory,
                      void * const virtual_address) {
    page_table_entry_t * entry =
        vmm_get_page_entry(page_directory, virtual_address);
    if (!entry) {
        return false;
    }
    return (entry->available & PAGE_FLAG_USED) != 0;
}

void * vmm_find_free_memory(size_t size, bool is_kernel) {
    uint32_t space_base_address = is_kernel ? VMM_KERNEL_SPACE_BASE : VMM_USER_SPACE_BASE;
    uint32_t space_size = is_kernel ? VMM_KERNEL_SPACE_SIZE : VMM_USER_SPACE_SIZE;
    uint32_t max_pages = div_up(space_size, PAGE_SIZE);

    for (uint32_t i = 0; i < max_pages; ++i) {
        uint32_t page_address = space_base_address + i * PAGE_SIZE;
        
        if (!vmm_is_page_used(curr_page_directory, (void *)page_address)) {
            bool is_free = true;

            for (uint32_t offset = 0; offset < size; offset += PAGE_SIZE) {
                // Ensure memory region is still within memory space bounds
                uint32_t curr_page_address = page_address + offset;
                if (curr_page_address >= space_base_address + space_size) {
                    is_free = false;
                }
                if (vmm_is_page_used(curr_page_directory, (void*) (curr_page_address))) {
                    is_free = false;
                }
                if (!is_free) {
                    break;
                }
            }

            if (is_free) {
                return (void*) page_address;
            }
        }
    }
    return NULL;
}

void * vmm_map_memory(void * virtual_address,
                      void * physical_address,
                      size_t size,
                      bool is_kernel,
                      bool is_writeable) {
    if (!virtual_address) {
        virtual_address = vmm_find_free_memory(size, is_kernel);

        if (!virtual_address) {
            KPANIC(KPANIC_OUT_OF_MEMORY, "Virtaul memory allocation failed.", NULL);
        }
    }

    if (!physical_address) {
        physical_address = pmm_find_free_frame();

        if (!physical_address) {
            return NULL;
        }
    }
    virtual_address = paging_align_address_down(virtual_address);
    uint32_t num_pages = div_up(size, PAGE_SIZE);

    for (uint32_t i = 0; i < num_pages; ++i) {
        uint32_t free_virtual_address =
            (uint32_t)virtual_address + i * PAGE_SIZE;
        uint32_t free_physical_address =
            (uint32_t)pmm_find_free_frame_from(physical_address);

        if (!free_physical_address) {
            vmm_unmap_memory(virtual_address, size);
            return NULL;
        }

        vmm_map_page(curr_page_directory,
                     curr_page_directory,
                     (void *)free_virtual_address,
                     (void *)free_physical_address,
                     true, true);
        pmm_mark_frame_reserved((void *)free_physical_address);
    }
    return virtual_address;
}

void vmm_unmap_memory(void * virtual_address, size_t size) {
    virtual_address = paging_align_address_down(virtual_address);
    uint32_t num_pages = div_up(size, PAGE_SIZE);

    for (uint32_t i = 0; i < num_pages; ++i) {
        uint32_t unmap_virtual_address =
            (uint32_t)virtual_address + i * PAGE_SIZE;
        vmm_unmap_page((void *)unmap_virtual_address);
    }
}

void vmm_unmap_page(void * const virtual_address) {
    void * frame_address =
        vmm_virtual_to_physical_address(curr_page_directory, virtual_address);

    if (frame_address) {
        pmm_mark_frame_available(frame_address);
    }
}

