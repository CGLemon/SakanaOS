#include <device/keyboard.h>
#include <utils/string.h>

static const char keyboard_scancode_ascii_map[] = {

    /* 0x00 */
    ASCII_CTRL_NUL,          // 0x00
    ASCII_CTRL_ESC,          // 0x01 ESC
    ASCII_DIGIT_1,           // 0x02 1
    ASCII_DIGIT_2,           // 0x03 2
    ASCII_DIGIT_3,           // 0x04 3
    ASCII_DIGIT_4,           // 0x05 4
    ASCII_DIGIT_5,           // 0x06 5
    ASCII_DIGIT_6,           // 0x07 6
    ASCII_DIGIT_7,           // 0x08 7
    ASCII_DIGIT_8,           // 0x09 8
    ASCII_DIGIT_9,           // 0x0A 9
    ASCII_DIGIT_0,           // 0x0B 0
    ASCII_PUNCT_MINUS,       // 0x0C -
    ASCII_PUNCT_EQUAL,       // 0x0D =
    ASCII_CTRL_BS,           // 0x0E Backspace
    ASCII_CTRL_TAB,          // 0x0F Tab

    /* 0x10 */
    ASCII_UPPER_Q,           // 0x10 Q
    ASCII_UPPER_W,           // 0x11 W
    ASCII_UPPER_E,           // 0x12 E
    ASCII_UPPER_R,           // 0x13 R
    ASCII_UPPER_T,           // 0x14 T
    ASCII_UPPER_Y,           // 0x15 Y
    ASCII_UPPER_U,           // 0x16 U
    ASCII_UPPER_I,           // 0x17 I
    ASCII_UPPER_O,           // 0x18 O
    ASCII_UPPER_P,           // 0x19 P
    ASCII_PUNCT_LBRACKET,    // 0x1A [
    ASCII_PUNCT_RBRACKET,    // 0x1B ]
    ASCII_CTRL_CR,           // 0x1C Enter
    ASCII_CTRL_NUL,          // 0x1D Left Ctrl
    ASCII_UPPER_A,           // 0x1E A
    ASCII_UPPER_S,           // 0x1F S

    /* 0x20 */
    ASCII_UPPER_D,           // 0x20 D
    ASCII_UPPER_F,           // 0x21 F
    ASCII_UPPER_G,           // 0x22 G
    ASCII_UPPER_H,           // 0x23 H
    ASCII_UPPER_J,           // 0x24 J
    ASCII_UPPER_K,           // 0x25 K
    ASCII_UPPER_L,           // 0x26 L
    ASCII_PUNCT_SEMICOLON,   // 0x27 ;
    ASCII_PUNCT_APOSTROPHE,  // 0x28 '
    ASCII_PUNCT_GRAVE,       // 0x29 `
    ASCII_CTRL_NUL,          // 0x2A Left Shift
    ASCII_PUNCT_BACKSLASH,   // 0x2B 
    ASCII_UPPER_Z,           // 0x2C Z
    ASCII_UPPER_X,           // 0x2D X
    ASCII_UPPER_C,           // 0x2E C
    ASCII_UPPER_V,           // 0x2F V

    /* 0x30 */
    ASCII_UPPER_B,           // 0x30 B
    ASCII_UPPER_N,           // 0x31 N
    ASCII_UPPER_M,           // 0x32 M
    ASCII_PUNCT_COMMA,       // 0x33 ,
    ASCII_PUNCT_DOT,         // 0x34 .
    ASCII_PUNCT_SLASH,       // 0x35 /
    ASCII_CTRL_NUL,          // 0x36 Right Shift
    ASCII_PUNCT_ASTERISK,    // 0x37 (Keypad *)
    ASCII_CTRL_NUL,          // 0x38 Left Alt
    ASCII_SPACE,             // 0x39 Space
    ASCII_CTRL_NUL,          // 0x3A Caps Lock

    /* Function keys 0x3B–0x44 -> no ASCII */
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL, 

    /* 0x45 */
    ASCII_CTRL_NUL,          // Numlock
    ASCII_CTRL_NUL,          // Scroll Lock

    /* Keypad 7–. (0x47–0x53) */
    ASCII_DIGIT_7,           // 0x47
    ASCII_DIGIT_8,           // 0x48
    ASCII_DIGIT_9,           // 0x49
    ASCII_PUNCT_MINUS,       // 0x4A (KP -)
    ASCII_DIGIT_4,           // 0x4B
    ASCII_DIGIT_5,           // 0x4C
    ASCII_DIGIT_6,           // 0x4D
    ASCII_PUNCT_PLUS,        // 0x4E (KP +)
    ASCII_DIGIT_1,           // 0x4F
    ASCII_DIGIT_2,           // 0x50
    ASCII_DIGIT_3,           // 0x51
    ASCII_DIGIT_0,           // 0x52
    ASCII_PUNCT_DOT,         // 0x53 (KP .)

    /* 0x54–0x58 -> no ASCII */
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL,
    ASCII_CTRL_NUL
};

char keyboard_transfer_scancode(uint32_t scancode, keyboard_event_t * event) {
    event->keycode = ASCII_CTRL_NUL;
    event->pressed = false;

    if (scancode <= 0x58) {
        event->keycode = keyboard_scancode_ascii_map[scancode];
        event->pressed = true;
    } else if (scancode >= 0x80 && scancode <= 0xD8) {
        event->keycode = keyboard_scancode_ascii_map[scancode];
        event->pressed = false;
    }
    event->keycode = tolower(event->keycode);
}
