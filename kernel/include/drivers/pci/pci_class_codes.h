#ifndef _KERNEL_DRIVERS_PCI_PCI_CLASS_CODES_H
#define _KERNEL_DRIVERS_PCI_PCI_CLASS_CODES_H


// PCI Class Codes (Base Class)

#define PCI_CLASS_UNDEFINED                        0x00
#define PCI_CLASS_MASS_STORAGE_CONTROLLER          0x01
#define PCI_CLASS_NETWORK_CONTROLLER               0x02
#define PCI_CLASS_DISPLAY_CONTROLLER               0x03
#define PCI_CLASS_MULTIMEDIA_CONTROLLER            0x04
#define PCI_CLASS_MEMORY_CONTROLLER                0x05
#define PCI_CLASS_BRIDGE_DEVICE                    0x06
#define PCI_CLASS_SIMPLE_COMMUNICATIONS_CONTROLLER 0x07
#define PCI_CLASS_BASE_SYSTEM_PERIPHERAL           0x08
#define PCI_CLASS_INPUT_DEVICE_CONTROLLER          0x09
#define PCI_CLASS_DOCKING_STATION                  0x0A
#define PCI_CLASS_PROCESSOR                        0x0B
#define PCI_CLASS_SERIAL_BUS_CONTROLLER            0x0C
#define PCI_CLASS_WIRELESS_CONTROLLER              0x0D
#define PCI_CLASS_INTELLIGENT_IO_CONTROLLER        0x0E
#define PCI_CLASS_SATELLITE_COMMUNICATION          0x0F
#define PCI_CLASS_ENCRYPTION_CONTROLLER            0x10
#define PCI_CLASS_SIGNAL_PROCESSING_CONTROLLER     0x11
#define PCI_CLASS_PROCESSING_ACCELERATOR           0x12
#define PCI_CLASS_NON_ESSENTIAL_INSTRUMENTATION    0x13

#define PCI_CLASS_COPROCESSOR                      0x40
#define PCI_CLASS_UNASSIGNED                       0xFF

// ---- Class 0x00: UNDEFINED ----
#define PCI_SUBCLASS_NON_VGA_UNDEFINED_DEVICE 0x0
#define PCI_SUBCLASS_VGA_UNDEFINED_DEVICE     0x1


// ---- Class 0x01: MASS STORAGE ----
#define PCI_SUBCLASS_SCSI_CONTROLLER              0x00
#define PCI_SUBCLASS_IDE_CONTROLLER               0x01
#define PCI_SUBCLASS_FLOPPY_CONTROLLER            0x02
#define PCI_SUBCLASS_IPI_BUS_CONTROLLER           0x03
#define PCI_SUBCLASS_RAID_CONTROLLER              0x04
#define PCI_SUBCLASS_ATA_CONTROLLER               0x05
#define PCI_SUBCLASS_SATA_CONTROLLER              0x06
#define PCI_SUBCLASS_SAS_CONTROLLER               0x07
#define PCI_SUBCLASS_NVM_CONTROLLER               0x08
#define PCI_SUBCLASS_MASS_STORAGE_OTHER           0x80

// ---- Class 0x02: NETWORK CONTROLLER ----
#define PCI_SUBCLASS_ETHERNET_CONTROLLER          0x00
#define PCI_SUBCLASS_TOKEN_RING_CONTROLLER        0x01
#define PCI_SUBCLASS_FDDI_CONTROLLER              0x02
#define PCI_SUBCLASS_ATM_CONTROLLER               0x03
#define PCI_SUBCLASS_ISDN_CONTROLLER              0x04
#define PCI_SUBCLASS_WORLDFIP_CONTROLLER          0x05
#define PCI_SUBCLASS_PICMG_CONTROLLER             0x06
#define PCI_SUBCLASS_INFINIBAND_CONTROLLER        0x07
#define PCI_SUBCLASS_NETWORK_OTHER                0x80

// ---- Class 0x03: DISPLAY CONTROLLER ----
#define PCI_SUBCLASS_VGA_CONTROLLER               0x00
#define PCI_SUBCLASS_XGA_CONTROLLER               0x01
#define PCI_SUBCLASS_3D_CONTROLLER                0x02
#define PCI_SUBCLASS_DISPLAY_OTHER                0x80

// ---- Class 0x04: MULTIMEDIA ----
#define PCI_SUBCLASS_VIDEO_DEVICE                 0x00
#define PCI_SUBCLASS_AUDIO_DEVICE                 0x01
#define PCI_SUBCLASS_TELEPHONY_DEVICE             0x02
#define PCI_SUBCLASS_AUDIO_DEVICE_OTHER           0x03
#define PCI_SUBCLASS_MULTIMEDIA_OTHER             0x80

// ---- Class 0x05: MEMORY CONTROLLER ----
#define PCI_SUBCLASS_RAM_CONTROLLER               0x00
#define PCI_SUBCLASS_FLASH_CONTROLLER             0x01
#define PCI_SUBCLASS_MEMORY_OTHER                 0x80

// ---- Class 0x06: BRIDGE DEVICE ----
#define PCI_SUBCLASS_HOST_BRIDGE                  0x00
#define PCI_SUBCLASS_ISA_BRIDGE                   0x01
#define PCI_SUBCLASS_EISA_BRIDGE                  0x02
#define PCI_SUBCLASS_MCA_BRIDGE                   0x03
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE            0x04
#define PCI_SUBCLASS_PCMCIA_BRIDGE                0x05
#define PCI_SUBCLASS_NUBUS_BRIDGE                 0x06
#define PCI_SUBCLASS_CARDBUS_BRIDGE               0x07
#define PCI_SUBCLASS_RACEWAY_BRIDGE               0x08
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE_SECS       0x09
#define PCI_SUBCLASS_INFINIBAND_BRIDGE            0x0A
#define PCI_SUBCLASS_BRIDGE_OTHER                 0x80

// ---- Class 0x07: SIMPLE COMMUNICATIONS ----
#define PCI_SUBCLASS_SERIAL_CONTROLLER            0x00
#define PCI_SUBCLASS_PARALLEL_CONTROLLER          0x01
#define PCI_SUBCLASS_MULTIPORT_SERIAL             0x02
#define PCI_SUBCLASS_MODEM                        0x03
#define PCI_SUBCLASS_GPIB                         0x04
#define PCI_SUBCLASS_SMARTCARD                    0x05
#define PCI_SUBCLASS_COMMUNICATION_OTHER          0x80

// ---- Class 0x08: BASE SYSTEM PERIPHERAL ----
#define PCI_SUBCLASS_PIC_CONTROLLER               0x00
#define PCI_SUBCLASS_DMA_CONTROLLER               0x01
#define PCI_SUBCLASS_TIMER_CONTROLLER             0x02
#define PCI_SUBCLASS_RTC_CONTROLLER               0x03
#define PCI_SUBCLASS_PCI_HOTPLUG                  0x04
#define PCI_SUBCLASS_SD_CONTROLLER                0x05
#define PCI_SUBCLASS_IOMMU_CONTROLLER             0x06
#define PCI_SUBCLASS_BASE_SYSTEM_OTHER            0x80

// ---- Class 0x09: INPUT DEVICE ----
#define PCI_SUBCLASS_KEYBOARD_CONTROLLER          0x00
#define PCI_SUBCLASS_DIGITIZER_PEN                0x01
#define PCI_SUBCLASS_MOUSE_CONTROLLER             0x02
#define PCI_SUBCLASS_SCANNER_CONTROLLER           0x03
#define PCI_SUBCLASS_GAMEPORT_CONTROLLER          0x04
#define PCI_SUBCLASS_INPUT_OTHER                  0x80

// ---- Class 0x0A: DOCKING STATION ----
#define PCI_SUBCLASS_DOCKING_GENERIC              0x00
#define PCI_SUBCLASS_DOCKING_OTHER                0x80

// ---- Class 0x0B: PROCESSOR ----
#define PCI_SUBCLASS_386                          0x00
#define PCI_SUBCLASS_486                          0x01
#define PCI_SUBCLASS_PENTIUM                      0x02
#define PCI_SUBCLASS_ALPHA                        0x10
#define PCI_SUBCLASS_POWERPC                      0x20
#define PCI_SUBCLASS_MIPS                         0x30
#define PCI_SUBCLASS_CO_PROCESSOR                 0x40
#define PCI_SUBCLASS_PROCESSOR_OTHER              0x80

// ---- Class 0x0C: SERIAL BUS ----
#define PCI_SUBCLASS_FIREWIRE_CONTROLLER          0x00
#define PCI_SUBCLASS_ACCESS_BUS                   0x01
#define PCI_SUBCLASS_SSA                          0x02
#define PCI_SUBCLASS_USB_CONTROLLER               0x03
#define PCI_SUBCLASS_FIBRE_CHANNEL                0x04
#define PCI_SUBCLASS_SMBUS_CONTROLLER             0x05
#define PCI_SUBCLASS_INFINIBAND                   0x06
#define PCI_SUBCLASS_IPMI_INTERFACE               0x07
#define PCI_SUBCLASS_SERCOS_INTERFACE             0x08
#define PCI_SUBCLASS_CANBUS                       0x09
#define PCI_SUBCLASS_SERIAL_OTHER                 0x80

// ---- Class 0x0D: WIRELESS ----
#define PCI_SUBCLASS_IRDA_CONTROLLER              0x00
#define PCI_SUBCLASS_CONSUMER_IR                  0x01
#define PCI_SUBCLASS_RF_CONTROLLER                0x10
#define PCI_SUBCLASS_BLUETOOTH_CONTROLLER         0x11
#define PCI_SUBCLASS_BROADBAND_CONTROLLER         0x12
#define PCI_SUBCLASS_ETHERNET_802_11A             0x20
#define PCI_SUBCLASS_ETHERNET_802_11B             0x21
#define PCI_SUBCLASS_WIRELESS_OTHER               0x80

// ---- Class 0x0E: INTELLIGENT I/O ----
#define PCI_SUBCLASS_I2O_CONTROLLER               0x00

// ---- Class 0x0F: SATELLITE ----
#define PCI_SUBCLASS_TV                           0x01
#define PCI_SUBCLASS_AUDIO                        0x02
#define PCI_SUBCLASS_VOICE                        0x03
#define PCI_SUBCLASS_DATA                         0x04

// ---- Class 0x10: ENCRYPTION ----
#define PCI_SUBCLASS_NETWORK_ENCRYPTION           0x00
#define PCI_SUBCLASS_ENTERTAINMENT_ENCRYPTION     0x10
#define PCI_SUBCLASS_OTHER_ENCRYPTION             0x80

// ---- Class 0x11: SIGNAL PROCESSING ----
#define PCI_SUBCLASS_DPIO_MODULE                  0x00
#define PCI_SUBCLASS_PERFORMANCE_COUNTERS         0x01
#define PCI_SUBCLASS_COMMUNICATION_SYNCHRONIZER   0x10
#define PCI_SUBCLASS_SIGNAL_PROCESSING_OTHER      0x80

// ---- Class 0x12: PROCESSING ACCELERATOR ----
#define PCI_SUBCLASS_ACCELERATOR                  0x00

// ---- Class 0x13: NON-ESSENTIAL INSTRUMENTATION ----
#define PCI_SUBCLASS_INSTRUMENTATION              0x00

#endif
