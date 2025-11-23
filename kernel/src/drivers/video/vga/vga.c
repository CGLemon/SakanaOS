#include <arch/i386/ports.h>
#include <device/device.h>
#include <device/video.h>
#include <drivers/video/vga/vga.h>
#include <drivers/input/ps2/keyboard.h>
#include <memory/heap.h>
#include <system/panic.h>
#include <utils/string.h>

static vga_state_t * vga_state = NULL;

int vga_get_offset();
uint8_t vga_get_attri(vga_color_t fg, vga_color_t bg);

void vga_setpos(int x, int y);
void vga_setchar(char c);
void vga_nextpos(char c);
void vga_putchar(char c);

size_t vga_get_width();
size_t vga_get_height();
void vga_write(char c);
void vga_strwrite(const char * data);
void vga_scroll();

void vga_enable_cursor(uint8_t start, uint8_t end);
void vga_disable_cursor();
void vga_update_cursor();

void vga_init(vga_video_mode_t mode) {
    if (!vga_state) {
        vga_state = (vga_state_t *)kmalloc(sizeof(vga_state_t));
        if (!vga_state) {
            KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
        }
    }

    if (mode == VGA_80x25_16_TEXT) {
        vga_state->width = 80;
        vga_state->height = 25;
        vga_state->buffer = (uint8_t *)VGA_MEMORY;
        vga_state->attri = vga_get_attri(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_state->cursor_enabled = true;
        vga_state->x = 0;
        vga_state->y = 0;
        vga_state->bytes = 2;
    } else {
        return;
    }
    vga_state->mode = mode;

    video_device_t * device = (video_device_t *)kmalloc(sizeof(video_device_t ));
    if (!device) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->info.name = (char *)kmalloc(15);
    if (!device->info.name) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }
    generate_uuid_v4(&device->info.id);
    strcpy(device->info.name, "VGA Controller");
    device->info.type = DEVICE_TYPE_VIDEO;
    device->info.bus.type = DEVICE_BUS_TYPE_PLATFORM;
    device->info.bus.data = NULL;

    video_driver_t * driver = (video_driver_t *)kmalloc(sizeof(video_driver_t));
    if (!driver) {
        KPANIC(KPANIC_OUT_OF_MEMORY, "Memory allocation failed.", NULL);
    }

    device->driver = driver;
    device->driver->tm.get_width = vga_get_width;
    device->driver->tm.get_height = vga_get_height;
    device->driver->tm.write = vga_write;
    device->driver->tm.strwrite = vga_strwrite;
    device->driver->tm.clean_screen = vga_clean_screen;
    device->driver->tm.enable_cursor = vga_enable_cursor;
    device->driver->tm.disable_cursor = vga_disable_cursor;

    device_register((device_t *)device);
 
    vga_enable_cursor(0, 15);
    vga_clean_screen();
}

size_t vga_get_width() {
    return vga_state->width;
}

size_t vga_get_height() {
    return vga_state->height;
}

int vga_get_offset() {
    int x = vga_state->x;
    int y = vga_state->y;
    return vga_state->bytes * (y * vga_state->width + x);
}

int vga_get_offset_x(int offset) {
    offset = offset / vga_state->bytes;
    return offset % vga_state->width;
}

int vga_get_offset_y(int offset) {
    offset = offset / vga_state->bytes;
    return offset / vga_state->width;
}

uint8_t vga_get_attri(vga_color_t fg, vga_color_t bg) {
	return fg | bg << 4;
}

void vga_write(char c) {
    vga_putchar(c);
}

void vga_strwrite(const char * data)  {
    int size, i;

    size = strlen(data);
    for (i = 0; i < size; i++) {
        vga_putchar(data[i]);
    }
}

void vga_scroll() {
    int i;
    int line_width_bytes = vga_state->bytes * vga_state->width;
    for (i = 1; i < vga_state->height; ++i) {
        void * dst = vga_state->buffer + (i - 1) * line_width_bytes;
        void * src = vga_state->buffer + i * line_width_bytes;
        memcpy(dst, src, line_width_bytes);
    }
    uint8_t * last_line = vga_state->buffer + (vga_state->height - 1) * line_width_bytes;
    for (i = 0; i < vga_state->width; ++i) {
        last_line[i * 2 + 0] = '\0';
        last_line[i * 2 + 1] = vga_state->attri;
    }
}

void vga_clean_screen() {
    int height = vga_state->height;
    int width = vga_state->width;

    int i;
    vga_setpos(0, 0);
    for (i = 0; i < height * width; i++) {
        vga_putchar('\0');
    }
    vga_setpos(0, 0);
    vga_update_cursor();
}

void vga_setpos(int x, int y) {
    vga_state->x = 0;
    vga_state->y = 0;
}

void vga_putchar(char c) {
    if (vga_state->mode == VGA_80x25_16_TEXT) {
        int width = vga_state->width;
        int height = vga_state->height;

        int bytes = vga_state->bytes;
        int offset = vga_get_offset();

        if (c == '\n') {
            offset += bytes * (width - ((offset / bytes) % width));
        } else if (c == ASCII_CTRL_BS) {
            if (offset > 0) {
                do {
                    offset -= bytes;
                    if (vga_state->buffer[bytes] != '\0') {
                        break;
                    }
                } while(true);
            }
	        vga_state->buffer[offset + 0] = '\0';
            vga_state->buffer[offset + 1] = vga_state->attri;
        } else {
	        vga_state->buffer[offset + 0] = c;
            vga_state->buffer[offset + 1] = vga_state->attri;
            offset += bytes;
        }

        if (offset == bytes * width * height) {
            vga_scroll();
            offset -= bytes * width;
        }
        vga_state->x = vga_get_offset_x(offset);
        vga_state->y = vga_get_offset_y(offset);
    }
    vga_update_cursor();
}

void vga_enable_cursor(uint8_t start, uint8_t end) {
    uint8_t data;
	port_byte_out(VGA_CRTC_ADDRESS_REGISTER_PORT, VGA_CRTC_CURSOR_START_REGISTER);
    data = port_byte_in(VGA_CRTC_DATA_REGISTER_PORT) & 0xC0;
	port_byte_out(VGA_CRTC_DATA_REGISTER_PORT, data | start);
 
	port_byte_out(VGA_CRTC_ADDRESS_REGISTER_PORT, VGA_CRTC_CURSOR_END_REGISTER);
    data = port_byte_in(VGA_CRTC_DATA_REGISTER_PORT) & 0xE0;
	port_byte_out(VGA_CRTC_DATA_REGISTER_PORT, data | end);
    vga_state->cursor_enabled = true;
    vga_update_cursor();
}

void vga_disable_cursor() {
    port_byte_out(VGA_CRTC_ADDRESS_REGISTER_PORT, VGA_CRTC_CURSOR_START_REGISTER);
	port_byte_out(VGA_CRTC_DATA_REGISTER_PORT, 0x20);
    vga_state->cursor_enabled = false;
}

void vga_update_cursor() {
    if (!vga_state->cursor_enabled) {
        return;
    }
    int index = vga_get_offset() / vga_state->bytes;
    port_byte_out(VGA_CRTC_ADDRESS_REGISTER_PORT, 14);
    port_byte_out(VGA_CRTC_DATA_REGISTER_PORT, (uint8_t)(index >> 8));
    port_byte_out(VGA_CRTC_ADDRESS_REGISTER_PORT, 15);
    port_byte_out(VGA_CRTC_DATA_REGISTER_PORT, (uint8_t)(index & 0xff));
}
