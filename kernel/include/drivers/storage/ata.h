#ifndef _KERNEL_DRIVER_STORAGE_ATA_H
#define _KERNEL_DRIVER_STORAGE_ATA_H

#include <utils/types.h>

#define ATA_PRIMARY_IO_BASE 0x1F0
#define ATA_PRIMARY_CONTROL_BASE 0x3F6

#define ATA_SECONDARY_IO_BASE 0x170
#define ATA_SECONDARY_CONTROL_BASE 0x376

#define ATA_DRIVE_MASTER 0xA0
#define ATA_DRIVE_SLAVE 0xB0

#define ATA_READWRITE_MASTER 0xE0
#define ATA_READWRITE_SLAVE 0xF0

#define ATA_DATA_REGISTER_OFFSET         0
#define ATA_ERROR_REGISTER_OFFSET        1
#define ATA_FEATURES_REGISTER_OFFSET     1
#define ATA_SECTOR_COUNT_REGISTER_OFFSET 2
#define ATA_LBA_LOW_REGISTER_OFFSET      3
#define ATA_LBA_MID_REGISTER_OFFSET      4
#define ATA_LBA_HIGH_REGISTER_OFFSET     5
#define ATA_DRIVE_REGISTER_OFFSET        6
#define ATA_STATUS_REGISTER_OFFSET       7
#define ATA_COMMAND_REGISTER_OFFSET      7

#define ATA_STATUS_ERR  0x01
#define ATA_STATUS_IDX  0x02
#define ATA_STATUS_CORR 0x04
#define ATA_STATUS_DRQ  0x08
#define ATA_STATUS_SRV  0x10
#define ATA_STATUS_DF   0x20
#define ATA_STATUS_RDY  0x40
#define ATA_STATUS_BSY  0x80

#define ATA_COMMAND_READ_SECTORS 0x20
#define ATA_COMMAND_WRITE_SECTORS 0x30
#define ATA_COMMAND_IDENTIFY 0xEC

#define ATA_ALT_STATUS_REGISTER 0x00
#define ATA_DEVICE_CONTROL_REGISTER 0x00
#define ATA_DRIVE_ADDRESS_REGISTER 0x01

#define ATA_SECTOR_SIZE 512

typedef enum {
    ATA_PRIMARY_MASTER_DRIVE,
    ATA_PRIMARY_SLAVE_DRIVE,
    ATA_SECONDARY_MASTER_DRIVE,
    ATA_SECONDARY_SLAVE_DRIVE,
    ATA_DRIVE_SIZE
} ata_drive_t;

typedef struct ata_device {
    ata_drive_t drive;
    bool present;
    bool lba_supported;
    bool lba48_supported;
    uint32_t size;
} ata_device_t;

int ata_init();
size_t ata_write(ata_device_t * device, uint32_t address, void * buffer, size_t size);
size_t ata_read(ata_device_t * device, uint32_t address, void * buffer, size_t size);

#endif
