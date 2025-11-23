#ifndef _KERNEL_MEMORY_PAGING_H
#define _KERNEL_MEMORY_PAGING_H

/*
 * The kernel's virtual memory layout:
 * 
 * 0x00000000 - 0x00001000 : NULL pointer dereference
 * 0x00001000 - 0xBFFFF000 : User space
 * 0xC0000000 - 0xFFFFFFFF : Kernel space
 *      0xC0000000 - 0xC00FFFFF : Real Mode Memory
 *          0xC00A0000 - 0xC00BFFFF : Video RAM
 *          0xC00C0000 - 0xC00C7FFF : Video BIOS
 *      0xC0100000 - 0x???????? : Higher half Kernel (Code/Data/BSS)
 *      0x???????? - 0xFFFFFFFF : Kernel free use (Heap, DMA, etc.)
 */

#define VMM_USER_SPACE_BASE 0x00001000
#define VMM_USER_SPACE_SIZE 0xBFFFF000

#define VMM_KERNEL_SPACE_BASE 0xC0000000
#define VMM_KERNEL_SPACE_SIZE 0x3FFFFFFF

#define VMM_REAL_MODE_MEMORY_BASE 0xC0000000
#define VMM_REAL_MODE_MEMORY_SIZE 0x00100000

extern char kernel_virtual_start[];
extern char kernel_virtual_end[];

/*
 * The kernel's physical memory layout:
 * 
 * 0x00000000 - 0x000FFFFF : Real Mode Memory
 *     0x000A0000 - 0x000BFFFF : Video RAM
 *     0x000C0000 - 0x000C7FFF : Video BIOS
 * 0x00100000 - 0x???????? : Kernel
 * 0x???????? - 0xFFFFFFFF : Free space (3GB Kernel space + 1GB User space)
 *
 */

// Physical Address Space (PAS) : 32GB
#define PMM_PAS_SIZE 0xFFFFFFFF
#define PMM_PAS_BASE 0x00000000

extern char kernel_physical_start[];
extern char kernel_physical_end[];

#endif
