#ifndef _KERNEL_DRIVERS_VIDEO_VGA_VGA_H
#define _KERNEL_DRIVERS_VIDEO_VGA_VGA_H

#include <utils/types.h>
#include <memory/mm_layout.h>

#define VGA_TM_VIDEO_MEMORY (VMM_REAL_MODE_MEMORY_BASE + 0xB8000)
#define VGA_GC_ADDRESS_REGISTER_PORT 0x3CE
#define VGA_GC_DATA_REGISTER_PORT 0x3CF

#define VGA_CRTC_ADDRESS_REGISTER_PORT 0x3D4
#define VGA_CRTC_DATA_REGISTER_PORT 0x3D5

#define VGA_CRTC_CURSOR_START_REGISTER 0x0A
#define VGA_CRTC_CURSOR_END_REGISTER 0x0B

typedef enum {
    VGA_80x25_16_TEXT = 0x03
} vga_video_mode_t;

typedef enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} vga_color_t;

typedef struct vga_state {
    vga_video_mode_t mode;
    size_t width;
    size_t height;
    uint8_t * buffer;
    uint8_t attri;
    bool cursor_enabled;
    int x;
    int y;
    int bytes;
} vga_state_t;

void vga_init(vga_video_mode_t mode);
void vga_strwrite(const char * data);
void vga_write(char c);
void vga_clean_screen();

void vga_enable_cursor(uint8_t start, uint8_t end);
void vga_disable_cursor();

#endif
