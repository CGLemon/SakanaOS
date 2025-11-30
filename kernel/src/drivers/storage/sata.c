#include <drivers/storage/ata.h>
#include <drivers/storage/sata.h>
#include <drivers/pci/pci.h>
#include <device/device.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_class_codes.h>
#include <memory/vmm.h>
#include <memory/heap.h>
#include <utils/linked_list.h>

int sata_init() {
    linked_list_t * devices = device_find_all_by_bustype(DEVICE_BUS_TYPE_PCI);

    linked_list_foreach(devices) {
        device_t * device = (device_t *)node->data;
        pci_device_t * pci_device = (pci_device_t *)device->bus.data;

        if (pci_device->classcode == PCI_CLASS_MASS_STORAGE_CONTROLLER &&
                pci_device->subclass == PCI_SUBCLASS_SATA_CONTROLLER) {
        }
    } linked_list_foreach_end;

    linked_list_destroy(devices, false, kfree);
    return 0;
}
