#ifndef _KERNEL_DRIVERS_INPUT_PS2_KEYBOARD_H
#define _KERNEL_DRIVERS_INPUT_PS2_KEYBOARD_H

#include <utils/types.h>

#define PS2_COMMAND_REGISTER 0x64
#define PS2_STATUS_REGISTER 0x64
#define PS2_DATA_REGISTER 0x60

int ps2_keyboard_init();

#endif
