#include <arch/i386/ports.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <system/message.h>
#include <utils/string.h>
#include <device/device.h>
#include <device/uuid.h>
#include <drivers/pci/pci.h>


pci_device_t * pci_probe_device(uint8_t bus, uint8_t slot, uint8_t func);
char * pci_get_device_name(pci_device_t * pci_device);
int pci_general_load_bar_info(pci_device_t* pci_device, uint8_t bar_index);
int pci_pci2pci_load_bar_info(pci_device_t* pci_device, uint8_t bar_index);

int pci_init() {
    uint16_t bus, slot, func;
    char kernel_message[64];

    for (bus = 0; bus < PCI_MAX_NUM_BUSES; bus++) {
        for (slot = 0; slot < PCI_DEVICES_PER_BUS; slot++) {
            device_t * slot_device = NULL;

            for (func = 0; func < PCI_FUNCTIONS_PER_DEVICE; func++) {
                pci_device_t * pci_device = pci_probe_device(bus, slot, func);

                if (!pci_device) {
                    continue;
                }

                device_t * device = (device_t *)kmalloc(sizeof(device_t));

                generate_uuid_v4(&device->id);
                device->name = pci_get_device_name(pci_device);
                device->type = DEVICE_TYPE_UNKNOWN;
                device->bus.type = DEVICE_BUS_TYPE_PCI;
                device->bus.data = pci_device;

                if (!slot_device) {
                    slot_device = device;
                    device_register(slot_device);
                } else {
                    device_register_at(device, slot_device);
                }

                sprintf(kernel_message,
                            "pci: Found device Vendor ID: %x, Device ID: %x at %d:%d.%d",
                             pci_device->vendor_id,
                             pci_device->device_id,
                             bus, slot, func);
                kmessage(KMESSAGE_LEVEL_INFO, kernel_message);

                if (func == 0 && (pci_device->header_type & PIC_HEADER_TYPE_MULTIFUNCTION) == 0) {
                    break;
                }
            }
        }
    }
    return 0;
}

char * pci_get_device_name(pci_device_t * pci_device) {
    char * name = (char*) kmalloc(32);
    if (!name) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    sprintf(name, "PCI: %x/%x (%d:%d.%d)",
                pci_device->vendor_id,
                pci_device->device_id,
                pci_device->bus,
                pci_device->slot,
                pci_device->function);
    return name;
}

pci_device_t * pci_probe_device(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t vendor_id = pci_config_read_word(bus, slot, func, PCI_VENDOR_ID);

    if (vendor_id == PCI_CONFIG_INVALID_VENDOR) {
        return NULL;
    }

    uint16_t device_id = pci_config_read_word(bus, slot, func, PCI_DEVICE_ID);
    uint8_t classcode = pci_config_read_byte(bus, slot, func, PCI_CLASS_CODE);
    uint8_t subclass = pci_config_read_byte(bus, slot, func, PCI_SUBCLASS);
    uint8_t prog_if = pci_config_read_byte(bus, slot, func, PCI_PROG_IF);
    uint8_t header_type = pci_config_read_byte(bus, slot, func, PCI_HEADER_TYPE);

    pci_device_t *device = (pci_device_t *)kmalloc(sizeof(pci_device_t));

    if (!device) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->bus = bus;
    device->slot = slot;
    device->function = func;
    device->vendor_id = vendor_id;
    device->device_id = device_id;
    device->classcode = classcode;
    device->subclass = subclass;
    device->prog_if = prog_if;
    device->header_type = header_type;

    return device;
}

int pci_load_bar_info(pci_device_t * pci_device, uint8_t bar_index) {
    uint8_t header_type = pci_device->header_type & 0x7F;

    switch (header_type) {
        case PCI_HEADER_TYPE_0:
            return pci_general_load_bar_info(pci_device, bar_index);
        case PCI_HEADER_TYPE_1:
            return pci_pci2pci_load_bar_info(pci_device, bar_index);
        case PCI_HEADER_TYPE_2:
        default:
            return -1;
    }
}

int pci_general_load_bar_info(pci_device_t* pci_device, uint8_t bar_index) {
    uint8_t offset;

    switch (bar_index) {
        case 0:
            offset = PCI_H0_BAR0;
            break;
        case 1:
            offset = PCI_H0_BAR1;
            break;
        case 2:
            offset = PCI_H0_BAR2;
            break;
        case 3:
            offset = PCI_H0_BAR3;
            break;
        case 4:
            offset = PCI_H0_BAR4;
            break;
        case 5:
            offset = PCI_H0_BAR5;
            break;
        default:
            return -1;
    }

    uint32_t bar_address = pci_config_read_long(
        pci_device->bus, pci_device->slot, pci_device->function, offset);

    if (bar_address & PCI_BAR_IO_SPACE) {
        pci_device->data.general.bar[bar_index].type = PCI_BAR_IO_SPACE;

        // Get BAR size through BAR masking

        pci_config_write_long(pci_device->bus, pci_device->slot, pci_device->function, offset, 0xFFFFFFFF);

        uint32_t mask = pci_config_read_long(pci_device->bus, pci_device->slot, pci_device->function, offset);

        pci_config_write_long(pci_device->bus, pci_device->slot, pci_device->function, offset, bar_address);

        // Apply size and io port

        pci_device->data.general.bar[bar_index].size = ~(mask & ~0x3) + 1;
        pci_device->data.general.bar[bar_index].io_port = bar_address & ~0x3;
        pci_device->data.general.bar[bar_index].flags = bar_address & 0x3;

    } else {
        uint8_t bar_address_type = bar_address & 0x3;

        if (bar_address_type != PCI_BAR_MEMORY_32BIT) {
            return -1;
        }

        pci_device->data.general.bar[bar_index].type = PCI_BAR_MEMORY_SPACE;

        // Get BAR size through BAR masking

        pci_config_write_long(pci_device->bus, pci_device->slot, pci_device->function, offset, 0xFFFFFFFF);

        uint32_t mask = pci_config_read_long(pci_device->bus, pci_device->slot, pci_device->function, offset);

        pci_config_write_long(pci_device->bus, pci_device->slot, pci_device->function, offset, bar_address);

        // Apply size and base address

        pci_device->data.general.bar[bar_index].size = ~(mask & ~0xf) + 1;
        pci_device->data.general.bar[bar_index].base_address = bar_address;
        pci_device->data.general.bar[bar_index].flags = bar_address & 0xf;
    }

    return 0;
}

int pci_pci2pci_load_bar_info(pci_device_t* pci_device, uint8_t bar_index) {
    return -1;
}

inline uint32_t pci_config_get_address(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t lbus  = (uint32_t)((bus & PCI_CONFIG_BUS_MASK) << 16);
    uint32_t lslot = (uint32_t)((slot & PCI_CONFIG_SLOT_MASK) << 11);
    uint32_t lfunc = (uint32_t)((func & PCI_CONFIG_FUNC_MASK) << 8);
    uint32_t loffset = (uint32_t)(offset & PCI_CONFIG_OFFSET_MASK);

    uint32_t address =
        (uint32_t)PCI_CONFIG_ENABLE_BIT | lbus | lslot | lfunc | loffset;
    return address;
}

inline uint8_t pci_config_make_byte(uint32_t data, uint8_t offset) {
    return (uint8_t)((data >> ((offset & 3) * 8)) & 0xFF); 
}

inline uint16_t pci_config_make_word(uint32_t data, uint8_t offset) {
    return (uint16_t)((data >> ((offset & 2) * 8)) & 0xFFFF);
}

inline uint32_t pci_config_make_long(uint32_t data, uint8_t offset) {
    (void) offset;
    return data;
}

uint8_t pci_config_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address =
        pci_config_get_address(bus, slot, func, offset);
    port_long_out(PCI_CONFIG_ADDRESS, address);

    uint32_t data = port_long_in(PCI_CONFIG_DATA);
    return pci_config_make_byte(data, offset);
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address =
        pci_config_get_address(bus, slot, func, offset);
    port_long_out(PCI_CONFIG_ADDRESS, address);

    uint32_t data = port_long_in(PCI_CONFIG_DATA);
    return pci_config_make_word(data, offset);
}

uint32_t pci_config_read_long(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address =
        pci_config_get_address(bus, slot, func, offset);
    port_long_out(PCI_CONFIG_ADDRESS, address);

    uint32_t data = port_long_in(PCI_CONFIG_DATA);
    return pci_config_make_long(data, offset);
}

void pci_config_write_long(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address =
        pci_config_get_address(bus, slot, func, offset);
    port_long_out(PCI_CONFIG_ADDRESS, address);
    port_long_out(PCI_CONFIG_DATA, value);
}
