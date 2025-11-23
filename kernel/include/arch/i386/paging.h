#ifndef _KERNEL_ARCH_I386_PAGING_H
#define _KERNEL_ARCH_I386_PAGING_H

#include <utils/types.h>

#define PAGE_FLAG_FREE      0b000
#define PAGE_FLAG_USED      0b001
#define PAGE_FLAG_PRESENT   0b000
#define PAGE_FLAG_SWAPPED   0b010

#define PAGE_SIZE 4096
#define PAGE_TABLE_SIZE 1024
#define PAGE_DIRECTORY_SIZE 1024

// 4 bytes
typedef struct page_table_entry {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t write_through : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t attribute_table : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t page_base : 20;
} __attribute__((packed)) page_table_entry_t;

// 4 KB
typedef struct page_table {
    page_table_entry_t entries[PAGE_TABLE_SIZE];
} __attribute__((packed)) page_table_t;

// 4 bytes
typedef struct page_directory_entry {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t write_through : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t reserved : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t page_table_base : 20;
} __attribute__((packed)) page_directory_entry_t;

// 8 MB
typedef struct page_directory {
    page_directory_entry_t entries[PAGE_DIRECTORY_SIZE];
    page_table_t * tables[PAGE_DIRECTORY_SIZE];
} __attribute__((packed)) page_directory_t;

extern const page_directory_t * const prepaging_page_directory;

void paging_enable();
void paging_invalidate_page(void * virtual_address);
void paging_switch_page_directory(void * physical_address);
void * paging_get_directory();

/*
 *  Virtual Address Layout (32-bit Paging)
 *
 *   31                22 21             12 11                 0
 *  +---------------------+----------------+---------------------+
 *  |   Directory Index   |  Table Index   |        Offset       |
 *  +---------------------+----------------+---------------------+
 *       (10 bits)             (10 bits)          (12 bits)
 *
 *
 *  Physical Address Layout (32-bit frame)
 *
 *    31                                12 11                 0
 *   +-------------------------------------+---------------------+
 *   |        Physical Frame Number        |        Offset       |
 *   +-------------------------------------+---------------------+
 *                (20 bits)                      (12 bits)
 *
 */
uint32_t paging_directory_index(void * virtual_address);
uint32_t paging_table_index(void * virtual_address);
uint32_t paging_memory_offset(void * virtual_address);
uint32_t paging_frame_index(void * physical_address);

bool paging_is_aligned_address(void * address);
void * paging_align_address_up(void * address);
void * paging_align_address_down(void * address);
uint32_t paging_align_address_offset(void * address);

#endif
