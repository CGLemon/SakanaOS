#ifndef _KERNEL_DRIVER_PCI_PCI_H
#define _KERNEL_DRIVER_PCI_PCI_H

#include <utils/types.h>

#define PCI_MAX_NUM_BUSES 256
#define PCI_DEVICES_PER_BUS 32
#define PCI_FUNCTIONS_PER_DEVICE 8

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_CONFIG_ENABLE_BIT  0x80000000
#define PCI_CONFIG_BUS_MASK    0xFF
#define PCI_CONFIG_SLOT_MASK   0x1F
#define PCI_CONFIG_FUNC_MASK   0x07
#define PCI_CONFIG_OFFSET_MASK 0xFC

#define PCI_CONFIG_INVALID_VENDOR 0xFFFF


// PCI common header fields

#define PCI_VENDOR_ID           0x00
#define PCI_DEVICE_ID           0x02
#define PCI_COMMAND             0x04
#define PCI_STATUS              0x06
#define PCI_REVISION_ID         0x08
#define PCI_PROG_IF             0x09
#define PCI_SUBCLASS            0x0A
#define PCI_CLASS_CODE          0x0B
#define PCI_CACHE_LINE_SIZE     0x0C
#define PCI_LATENCY_TIMER       0x0D
#define PCI_HEADER_TYPE         0x0E
#define PCI_BIST                0x0F

// PCI BAR address register

#define PCI_BAR_IO_SPACE        0x01
#define PCI_BAR_MEMORY_SPACE    0x00

#define PCI_BAR_MEMORY_32BIT    0x00
#define PCI_BAR_MEMORY_64BIT    0x04


// PCI Header type register
#define PIC_HEADER_TYPE_MULTIFUNCTION   0x80

// General PCI device
#define PCI_HEADER_TYPE_0        0x00

// PCI-to-PCI bridge
#define PCI_HEADER_TYPE_1        0x01

// PCI-to-CardBus bridge
#define PCI_HEADER_TYPE_2        0x02

// PCI header type 0 fields

#define PCI_H0_BAR0             0x10
#define PCI_H0_BAR1             0x14
#define PCI_H0_BAR2             0x18
#define PCI_H0_BAR3             0x1C
#define PCI_H0_BAR4             0x20
#define PCI_H0_BAR5             0x24
#define PCI_H0_INTERRUPT_LINE   0x3C
#define PCI_H0_SECONDARY_BUS    0x19

typedef struct pci_bar {
    uint8_t type;
    union {
        uint32_t base_address;
        uint16_t io_port;
    };
    size_t size;
    uint8_t flags;
} pci_bar_t;

typedef struct pci_general_device_info {
    struct pci_bar bar[6];
    uint8_t interrupt_line;
} pci_general_device_info_t;

typedef struct pci_device {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t classcode;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t header_type;

    union {
        pci_general_device_info_t general;
    } data;
} __attribute__((packed)) pci_device_t;

int pci_init();
int pci_load_bar_info(pci_device_t * pci_device, uint8_t bar_index);

/*
 * ===================== PCI CONFIGURATION ADDRESS (0xCF8) =====================
 *
 * The CPU writes to I/O port 0xCF8 to select a PCI configuration register.
 * The format of the 32-bit value is defined by the PCI specification:
 *
 *       31         30-24      23-16     15-11     10-8       7-0
 *  +------------+----------+----------+--------+--------+-----------+
 *  | Enable=1   | Reserved |   Bus    | Device | Func   | Register  |
 *  +------------+----------+----------+--------+--------+-----------+
 *
 *  Field meaning
 *  -------------------------------------------------------------------------
 *  Enable bit (bit 31):
 *      Must be 1 to indicate a valid PCI config access.
 *      If 0, access is ignored. This is why we OR the address with 0x80000000.
 *
 *  Reserved (bits 30-24):
 *      Must be zero.
 *
 *  Bus Number (bits 23-16):
 *      PCI bus number (0–255).
 *
 *  Device/Slot Number (bits 15-11):
 *      Device number on the bus (0–31).
 *
 *  Function Number (bits 10-8):
 *      Function number (0–7). For multi-function PCI devices.
 *
 *  Register Offset (bits 7-0):
 *      PCI configuration register index.
 *      Must be aligned to 4 bytes (so bits 1-0 must be 0).
 *
 * ============================================================================
 */
uint8_t pci_config_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_config_read_long(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_config_write_long(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);

#endif
