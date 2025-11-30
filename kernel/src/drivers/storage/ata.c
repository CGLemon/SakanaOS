#include <arch//i386/ports.h>
#include <drivers/storage/ata.h>
#include <device/device.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <utils/string.h>

#include <system/message.h>

ata_device_t ata_devices[ATA_DRIVE_SIZE];

bool ata_device_probe(ata_device_t * device);
uint16_t ata_get_io_base(ata_device_t * device);
bool ata_is_master(ata_device_t * device);

void ata_write_sector_lba28(ata_device_t * device, uint32_t lba, uint8_t * buf);
void ata_read_sector_lba28(ata_device_t * device, uint32_t lba, uint8_t * buf);

size_t ata_sector_size();
size_t ata_total_size_primary_master();
size_t ata_write_primary_master(uint32_t address, void * buf, size_t size);
size_t ata_read_primary_master(uint32_t address, void * buf, size_t size);
size_t ata_total_size_primary_slave();
size_t ata_write_primary_slave(uint32_t address, void * buf, size_t size);
size_t ata_read_primary_slave(uint32_t address, void * buf, size_t size);
size_t ata_total_size_secondary_master();
size_t ata_write_secondary_master(size_t address, void * buf, size_t size);
size_t ata_read_secondary_master(size_t address, void * buf, size_t size);
size_t ata_total_size_secondary_slave();
size_t ata_write_secondary_slave(uint32_t address, void * buf, size_t size);
size_t ata_read_secondary_slave(uint32_t address, void * buf, size_t size);

void ata_init_device(int device_idx, char * name,
                     size_t (*driver_sector_size)(),
                     size_t (*driver_total_size)(),
                     size_t (*driver_read)(uint32_t address, void * buf, size_t size),
                     size_t (*driver_write)(uint32_t address, void * buf, size_t size)) {
    if (!ata_device_probe(&ata_devices[device_idx])) {
        return;
    }

    storage_device_t * device = (storage_device_t*) kmalloc(sizeof(storage_device_t));
    if (!device) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->info.name = (char *)kmalloc(strlen(name) + 1);
    if (!device->info.name) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    generate_uuid_v4(&device->info.id);
    strcpy(device->info.name, name);
    device->info.type = DEVICE_TYPE_STORAGE;
    device->info.bus.type = DEVICE_BUS_TYPE_PLATFORM;
    device->info.bus.data = NULL;

    device->driver = (storage_driver_t *)kmalloc(sizeof(storage_driver_t));

    if (!device->driver) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->driver->sector_size = driver_sector_size;
    device->driver->total_size = driver_total_size;
    device->driver->read = driver_read;
    device->driver->write = driver_write;

    device_register((device_t *)device);

    char kernel_message[48];
    sprintf(kernel_message, "ata: Found device at %d (%d MB)",
                device_idx, driver_total_size()/1024/1024);
    kmessage(KMESSAGE_LEVEL_INFO, kernel_message);
}

int ata_init() {
    for (int i = 0; i < ATA_DRIVE_SIZE; ++i) {
        ata_devices[i].drive = (ata_drive_t)i;
        ata_devices[i].present = false;
        ata_devices[i].lba_supported = false;
        ata_devices[i].lba48_supported = false;
        ata_devices[i].size = 0;
    }

    ata_init_device(
        ATA_PRIMARY_MASTER_DRIVE,
        "ATA Primary Master Drive",
        ata_sector_size,
        ata_total_size_primary_master,
        ata_read_primary_master,
        ata_write_primary_master);
    ata_init_device(
        ATA_PRIMARY_SLAVE_DRIVE,
        "ATA Primary Slave Drive",
        ata_sector_size,
        ata_total_size_primary_slave,
        ata_read_primary_slave,
        ata_write_primary_slave);
    ata_init_device(
        ATA_SECONDARY_MASTER_DRIVE,
        "ATA Secondary Master Drive",
        ata_sector_size,
        ata_total_size_secondary_master,
        ata_read_secondary_master,
        ata_write_secondary_master);
    ata_init_device(
        ATA_SECONDARY_SLAVE_DRIVE,
        "ATA Secondary Slave Drive",
        ata_sector_size,
        ata_total_size_secondary_slave,
        ata_read_secondary_slave,
        ata_write_secondary_slave);
    return 0;
}

bool ata_device_probe(ata_device_t * device) {
    uint16_t io_base = ata_get_io_base(device);
    bool is_master = ata_is_master(device);

    uint16_t io_data = io_base + ATA_DATA_REGISTER_OFFSET;
    uint16_t io_sect = io_base + ATA_SECTOR_COUNT_REGISTER_OFFSET;
    uint16_t io_lba_low = io_base + ATA_LBA_LOW_REGISTER_OFFSET;
    uint16_t io_lba_mid = io_base + ATA_LBA_MID_REGISTER_OFFSET;
    uint16_t io_lba_high = io_base + ATA_LBA_HIGH_REGISTER_OFFSET;
    uint16_t io_reg = io_base + ATA_DRIVE_REGISTER_OFFSET;
    uint16_t io_cmd = io_base + ATA_COMMAND_REGISTER_OFFSET;
    uint16_t io_status = io_base + ATA_STATUS_REGISTER_OFFSET;

    // Choose master/slave drive
    port_byte_out(io_reg, is_master ? ATA_DRIVE_MASTER : ATA_DRIVE_SLAVE);

    for (int i = 0; i < 4; i++) {
        port_byte_in(io_status); // 400ns delay
    }

    // Set the sector count and LBA registers to 0
    port_byte_out(io_sect, 0x00);
    port_byte_out(io_lba_low, 0x00);
    port_byte_out(io_lba_mid, 0x00);
    port_byte_out(io_lba_high, 0x00);

    // Send the identify command
    port_byte_out(io_cmd, ATA_COMMAND_IDENTIFY);

    if (port_byte_in(io_status) == 0x00) {
        return false;
    }

    // Wait for the drive to be ready
    while (port_byte_in(io_status) & ATA_STATUS_BSY) {}

    // Check if the drive is an ATA drive
    if (port_byte_in(io_lba_mid) != 0x00 ||
            port_byte_in(io_lba_high) != 0x00) {
        return false;
    }

    uint8_t drq = 0;
    uint8_t err = 0;

    // Wait for the drive to be ready
    while (!drq && !err) {
        drq = port_byte_in(io_status) & ATA_STATUS_DRQ;
        err = port_byte_in(io_status) & ATA_STATUS_ERR;
    }

    uint16_t identify_data[256];
    for (uint16_t i = 0; i < 256; i++) {
        identify_data[i] = port_word_in(io_data);
    }

    // Check if LBA is supported
    if (identify_data[49] & (1 << 9)) {
        device->lba_supported = true;
    }

    // Check if LBA48 is supported
    if (identify_data[83] & (1 << 10)) {
        device->lba48_supported = true;
    }

    // Get the drive's size
    if (device->lba_supported) {
        // LBA48 size but we don't support it.
        //
        // device->size = (
        //     identify_data[100] |
        //     (identify_data[101] << 16) |
        //     (identify_data[102] << 32) |
        //     (identify_data[103] << 48)
        // ) * ATA_SECTOR_SIZE;

        device->size = (
            identify_data[60] |
            (identify_data[61] << 16)
        ) * ATA_SECTOR_SIZE;
    } else {
        /*
         * If LBA is not supported, the drive's size is calculated using CHS. The formula is:
         * Capacity = (Cyliners * Heads * Sectors) * 512 bytes
         */

        uint16_t cylinders = identify_data[1];
        uint16_t heads = identify_data[3];
        uint16_t sectors = identify_data[6];
        
        device->size = (cylinders * heads * sectors) * ATA_SECTOR_SIZE;
    }
    device->present = true;
    return true;
}

size_t ata_sector_size() {
    return ATA_SECTOR_SIZE;
}

size_t ata_total_size_primary_master() {
    return ata_devices[ATA_PRIMARY_MASTER_DRIVE].size;
}

size_t ata_write_primary_master(uint32_t address, void * buf, size_t size) {
    return ata_write(&ata_devices[ATA_PRIMARY_MASTER_DRIVE], address, buf, size);
}

size_t ata_read_primary_master(uint32_t address, void * buf, size_t size) {
    return ata_read(&ata_devices[ATA_PRIMARY_MASTER_DRIVE], address, buf, size);
}

size_t ata_total_size_primary_slave() {
    return ata_devices[ATA_PRIMARY_SLAVE_DRIVE].size;
}

size_t ata_write_primary_slave(uint32_t address, void * buf, size_t size) {
    return ata_write(&ata_devices[ATA_PRIMARY_SLAVE_DRIVE], address, buf, size);
}

size_t ata_read_primary_slave(uint32_t address, void * buf, size_t size) {
    return ata_read(&ata_devices[ATA_PRIMARY_SLAVE_DRIVE], address, buf, size);
}

size_t ata_total_size_secondary_master() {
    return ata_devices[ATA_SECONDARY_MASTER_DRIVE].size;
}

size_t ata_write_secondary_master(size_t address, void * buf, size_t size) {
    return ata_write(&ata_devices[ATA_SECONDARY_MASTER_DRIVE], address, buf, size);
}

size_t ata_read_secondary_master(size_t address, void * buf, size_t size) {
    return ata_read(&ata_devices[ATA_SECONDARY_MASTER_DRIVE], address, buf, size);
}

size_t ata_total_size_secondary_slave() {
    return ata_devices[ATA_SECONDARY_SLAVE_DRIVE].size;
}

size_t ata_write_secondary_slave(uint32_t address, void * buf, size_t size) {
    return ata_write(&ata_devices[ATA_SECONDARY_SLAVE_DRIVE], address, buf, size);
}

size_t ata_read_secondary_slave(uint32_t address, void * buf, size_t size) {
    return ata_read(&ata_devices[ATA_SECONDARY_SLAVE_DRIVE], address, buf, size);
}

uint16_t ata_get_io_base(ata_device_t * device) {
    if (device->drive == ATA_PRIMARY_MASTER_DRIVE ||
            device->drive == ATA_PRIMARY_SLAVE_DRIVE) {
        return ATA_PRIMARY_IO_BASE;
    }
    return ATA_SECONDARY_IO_BASE;
}

bool ata_is_master(ata_device_t * device) {
    return device->drive == ATA_PRIMARY_MASTER_DRIVE ||
               device->drive == ATA_SECONDARY_MASTER_DRIVE;
}

size_t ata_write(ata_device_t * device, uint32_t address, void * buf, size_t size) {
    if (device->size < address + size) {
        return 0;
    }
    if (!device->present) {
        return 0;
    }
    if (!device->lba_supported) {
        return 0;
    }

    size_t start_sector = address / ATA_SECTOR_SIZE;
    size_t start_sector_offset = address % ATA_SECTOR_SIZE;

    size_t end_sector = (address + size - 1) / ATA_SECTOR_SIZE;
    size_t end_sector_offset = (address + size - 1) % ATA_SECTOR_SIZE;

    uint8_t * sector_buf = (uint8_t *)kmalloc(ATA_SECTOR_SIZE);

    if (!sector_buf) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    uint8_t * data_ptr = (uint8_t *)buf;
    size_t total_size = 0;

    for (size_t idx = start_sector; idx <= end_sector; ++idx) {
        size_t write_offset = 0;
        size_t write_size = ATA_SECTOR_SIZE;

        if (idx == start_sector) {
            write_offset += start_sector_offset;
            write_size -= start_sector_offset;
        }
        if (idx == end_sector) {
            write_size = end_sector_offset - write_offset + 1;
        }

        // Read sector
        ata_read_sector_lba28(device, idx, sector_buf);

        // Alter the sector buffer
        memcpy(sector_buf + write_offset, data_ptr, write_size);

        // Write back
        ata_write_sector_lba28(device, idx, sector_buf);

        data_ptr += write_size;
        total_size += write_size;
    }
    return total_size;
}

size_t ata_read(ata_device_t * device, uint32_t address, void * buf, size_t size) {
    if (device->size < address + size) {
        return 0;
    }
    if (!device->present) {
        return 0;
    }
    if (!device->lba_supported) {
        return 0;
    }

    size_t start_sector = address / ATA_SECTOR_SIZE;
    size_t start_sector_offset = address % ATA_SECTOR_SIZE;

    size_t end_sector = (address + size - 1) / ATA_SECTOR_SIZE;
    size_t end_sector_offset = (address + size - 1) % ATA_SECTOR_SIZE;

    uint8_t * sector_buf = (uint8_t *)kmalloc(ATA_SECTOR_SIZE);

    if (!sector_buf) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    uint8_t * data_ptr = (uint8_t *)buf;
    size_t total_size = 0;

    for (size_t idx = start_sector; idx <= end_sector; ++idx) {
        size_t read_offset = 0;
        size_t read_size = ATA_SECTOR_SIZE;

        if (idx == start_sector) {
            read_offset += start_sector_offset;
            read_size -= start_sector_offset;
        }
        if (idx == end_sector) {
            read_size = end_sector_offset - read_offset + 1;
        }

        // Read sector
        ata_read_sector_lba28(device, idx, sector_buf);

        // Copy the sector buffer to the output buffer
        memcpy(data_ptr, sector_buf + read_offset, read_size);

        data_ptr += read_size;
        total_size += read_size;
    }
    return total_size;
}

void ata_write_sector_lba28(ata_device_t * device, uint32_t lba, uint8_t * buf) {
    uint16_t io_base = ata_get_io_base(device);
    bool is_master = ata_is_master(device);

    uint16_t io_data = io_base + ATA_DATA_REGISTER_OFFSET;
    uint16_t io_sect = io_base + ATA_SECTOR_COUNT_REGISTER_OFFSET;
    uint16_t io_lba_low = io_base + ATA_LBA_LOW_REGISTER_OFFSET;
    uint16_t io_lba_mid = io_base + ATA_LBA_MID_REGISTER_OFFSET;
    uint16_t io_lba_high = io_base + ATA_LBA_HIGH_REGISTER_OFFSET;
    uint16_t io_reg = io_base + ATA_DRIVE_REGISTER_OFFSET;
    uint16_t io_cmd = io_base + ATA_COMMAND_REGISTER_OFFSET;
    uint16_t io_status = io_base + ATA_STATUS_REGISTER_OFFSET;

    // Choose master/slave drives and set the LBA mode
    port_byte_out(
        io_reg,
        (is_master ? ATA_READWRITE_MASTER : ATA_READWRITE_SLAVE) | ((lba >> 24) & 0x0F));

    // Set number of sectors to write
    port_byte_out(io_sect, 0x01);

    // Set the LBA
    port_byte_out(io_lba_low,  (lba & 0x000000FF) >> 0);
    port_byte_out(io_lba_mid,  (lba & 0x0000FF00) >> 8);
    port_byte_out(io_lba_high, (lba & 0x00FF0000) >> 16);

    // Send the read command
    port_byte_out(io_cmd, ATA_COMMAND_WRITE_SECTORS);

    // Wait for the drive to be ready
    while (port_byte_in(io_status) & ATA_STATUS_BSY) {}

    // Read the sector
    for (uint16_t i = 0; i < 256; i++) { 
        port_word_out(io_data, ((uint16_t *)buf)[i]);
    }
}

void ata_read_sector_lba28(ata_device_t * device, uint32_t lba, uint8_t * buf) {
    uint16_t io_base = ata_get_io_base(device);
    bool is_master = ata_is_master(device);

    uint16_t io_data = io_base + ATA_DATA_REGISTER_OFFSET;
    uint16_t io_sect = io_base + ATA_SECTOR_COUNT_REGISTER_OFFSET;
    uint16_t io_lba_low = io_base + ATA_LBA_LOW_REGISTER_OFFSET;
    uint16_t io_lba_mid = io_base + ATA_LBA_MID_REGISTER_OFFSET;
    uint16_t io_lba_high = io_base + ATA_LBA_HIGH_REGISTER_OFFSET;
    uint16_t io_reg = io_base + ATA_DRIVE_REGISTER_OFFSET;
    uint16_t io_cmd = io_base + ATA_COMMAND_REGISTER_OFFSET;
    uint16_t io_status = io_base + ATA_STATUS_REGISTER_OFFSET;

    // Choose master/slave drives and set the LBA mode
    port_byte_out(
        io_reg,
        (is_master ? ATA_READWRITE_MASTER : ATA_READWRITE_SLAVE) | ((lba >> 24) & 0x0F));

    // Set number of sectors to read
    port_byte_out(io_sect, 0x01);

    // Set the LBA
    port_byte_out(io_lba_low,  (lba & 0x000000FF) >> 0);
    port_byte_out(io_lba_mid,  (lba & 0x0000FF00) >> 8);
    port_byte_out(io_lba_high, (lba & 0x00FF0000) >> 16);

    // Send the read command
    port_byte_out(io_cmd, ATA_COMMAND_READ_SECTORS);

    // Wait for the drive to be ready
    while (port_byte_in(io_status) & ATA_STATUS_BSY) {}

    // Read the sector
    for (uint16_t i = 0; i < 256; i++) {
        ((uint16_t *)buf)[i] = port_word_in(io_data);
    }
}
