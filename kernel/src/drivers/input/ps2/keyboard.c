#include <drivers/input/ps2/keyboard.h>
#include <arch/i386/ports.h>
#include <arch/i386/isr.h>
#include <memory/heap.h>
#include <device/device.h>
#include <device/uuid.h>
#include <utils/string.h>
#include <system/panic.h>

#include <device/keyboard.h>

static keyboard_listener_t ps2_keyboard_listener = NULL;

void ps2_keyboard_register_listener(keyboard_listener_t listener) {
    ps2_keyboard_listener = listener;
}

void ps2_keyboard_interrupt_handler(isr_cpu_state_t * state) {
    uint8_t scancode = port_byte_in(PS2_DATA_REGISTER);

    if (ps2_keyboard_listener) {
        keyboard_event_t event;
        keyboard_transfer_scancode(scancode, &event);
        ps2_keyboard_listener(&event);
    }
}

int32_t ps2_keyboard_init() {
    keyboard_device_t * device = (keyboard_device_t *)kmalloc(sizeof(keyboard_device_t));
    if (!device) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->info.name = (char*) kmalloc(14);

    if(!device->info.name) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    generate_uuid_v4(&device->info.id);
    strcpy(device->info.name, "PS/2 Keyboard");
    device->info.type = DEVICE_TYPE_KEYBOARD;
    device->info.bus.type = DEVICE_BUS_TYPE_PLATFORM;
    device->info.bus.data = NULL;

    device->driver = (keyboard_driver_t*) kmalloc(sizeof(keyboard_driver_t));

    if (!device->driver) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->driver->register_listener = ps2_keyboard_register_listener;

    device_register((device_t *)device);

    isr_register_listener(KEYBOARD_INTERRUPT, ps2_keyboard_interrupt_handler);

    // Enable keyboard scanning
    port_byte_out(PS2_DATA_REGISTER, 0xF4);
}

