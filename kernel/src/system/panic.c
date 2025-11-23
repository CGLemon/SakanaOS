#include <system/panic.h>
#include <drivers/video/vga/vga.h>
#include <utils/string.h>

void kpanic(int code, const char * message, const char * file, uint32_t line, isr_cpu_state_t * state) {
    char panic_buf[256];

    vga_init(VGA_80x25_16_TEXT);
    vga_strwrite("==== KERNEL PANIC ====\n");

    sprintf(panic_buf, "Location : %s:%d\n", file, line);
    vga_strwrite(panic_buf);

    sprintf(panic_buf, "Code     : %d\n", code);
    vga_strwrite(panic_buf);

    sprintf(panic_buf, "Reason   : %s\n", message);
    vga_strwrite(panic_buf);

    if (state) {
        // TODO:
    }

    // Halt the system
    while(1);
}
