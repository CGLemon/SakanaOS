#ifndef _KERNEL_DEVICE_KEYBOARD_H
#define _KERNEL_DEVICE_KEYBOARD_H

#include <utils/types.h>

#define ASCII_CTRL_NUL          0
#define ASCII_CTRL_SOH          1
#define ASCII_CTRL_STX          2
#define ASCII_CTRL_ETX          3
#define ASCII_CTRL_EOT          4
#define ASCII_CTRL_ENQ          5
#define ASCII_CTRL_ACK          6
#define ASCII_CTRL_BEL          7
#define ASCII_CTRL_BS           8
#define ASCII_CTRL_TAB          9
#define ASCII_CTRL_LF          10
#define ASCII_CTRL_VT          11
#define ASCII_CTRL_FF          12
#define ASCII_CTRL_CR          13
#define ASCII_CTRL_SO          14
#define ASCII_CTRL_SI          15
#define ASCII_CTRL_DLE         16
#define ASCII_CTRL_DC1         17
#define ASCII_CTRL_DC2         18
#define ASCII_CTRL_DC3         19
#define ASCII_CTRL_DC4         20
#define ASCII_CTRL_NAK         21
#define ASCII_CTRL_SYN         22
#define ASCII_CTRL_ETB         23
#define ASCII_CTRL_CAN         24
#define ASCII_CTRL_EM          25
#define ASCII_CTRL_SUB         26
#define ASCII_CTRL_ESC         27
#define ASCII_CTRL_FS          28
#define ASCII_CTRL_GS          29
#define ASCII_CTRL_RS          30
#define ASCII_CTRL_US          31

#define ASCII_SPACE            32

#define ASCII_PUNCT_EXCLAM     33
#define ASCII_PUNCT_QUOTE      34
#define ASCII_PUNCT_HASH       35
#define ASCII_PUNCT_DOLLAR     36
#define ASCII_PUNCT_PERCENT    37
#define ASCII_PUNCT_AMPERSAND  38
#define ASCII_PUNCT_APOSTROPHE 39
#define ASCII_PUNCT_LPAREN     40
#define ASCII_PUNCT_RPAREN     41
#define ASCII_PUNCT_ASTERISK   42
#define ASCII_PUNCT_PLUS       43
#define ASCII_PUNCT_COMMA      44
#define ASCII_PUNCT_MINUS      45
#define ASCII_PUNCT_DOT        46
#define ASCII_PUNCT_SLASH      47

#define ASCII_DIGIT_0          48
#define ASCII_DIGIT_1          49
#define ASCII_DIGIT_2          50
#define ASCII_DIGIT_3          51
#define ASCII_DIGIT_4          52
#define ASCII_DIGIT_5          53
#define ASCII_DIGIT_6          54
#define ASCII_DIGIT_7          55
#define ASCII_DIGIT_8          56
#define ASCII_DIGIT_9          57

#define ASCII_PUNCT_COLON      58
#define ASCII_PUNCT_SEMICOLON  59
#define ASCII_PUNCT_LESS       60
#define ASCII_PUNCT_EQUAL      61
#define ASCII_PUNCT_GREATER    62
#define ASCII_PUNCT_QUESTION   63
#define ASCII_PUNCT_AT         64

#define ASCII_UPPER_A          65
#define ASCII_UPPER_B          66
#define ASCII_UPPER_C          67
#define ASCII_UPPER_D          68
#define ASCII_UPPER_E          69
#define ASCII_UPPER_F          70
#define ASCII_UPPER_G          71
#define ASCII_UPPER_H          72
#define ASCII_UPPER_I          73
#define ASCII_UPPER_J          74
#define ASCII_UPPER_K          75
#define ASCII_UPPER_L          76
#define ASCII_UPPER_M          77
#define ASCII_UPPER_N          78
#define ASCII_UPPER_O          79
#define ASCII_UPPER_P          80
#define ASCII_UPPER_Q          81
#define ASCII_UPPER_R          82
#define ASCII_UPPER_S          83
#define ASCII_UPPER_T          84
#define ASCII_UPPER_U          85
#define ASCII_UPPER_V          86
#define ASCII_UPPER_W          87
#define ASCII_UPPER_X          88
#define ASCII_UPPER_Y          89
#define ASCII_UPPER_Z          90

#define ASCII_PUNCT_LBRACKET   91
#define ASCII_PUNCT_BACKSLASH  92
#define ASCII_PUNCT_RBRACKET   93
#define ASCII_PUNCT_CARET      94
#define ASCII_PUNCT_UNDERSCORE 95
#define ASCII_PUNCT_GRAVE      96

#define ASCII_LOWER_A          97
#define ASCII_LOWER_B          98
#define ASCII_LOWER_C          99
#define ASCII_LOWER_D         100
#define ASCII_LOWER_E         101
#define ASCII_LOWER_F         102
#define ASCII_LOWER_G         103
#define ASCII_LOWER_H         104
#define ASCII_LOWER_I         105
#define ASCII_LOWER_J         106
#define ASCII_LOWER_K         107
#define ASCII_LOWER_L         108
#define ASCII_LOWER_M         109
#define ASCII_LOWER_N         110
#define ASCII_LOWER_O         111
#define ASCII_LOWER_P         112
#define ASCII_LOWER_Q         113
#define ASCII_LOWER_R         114
#define ASCII_LOWER_S         115
#define ASCII_LOWER_T         116
#define ASCII_LOWER_U         117
#define ASCII_LOWER_V         118
#define ASCII_LOWER_W         119
#define ASCII_LOWER_X         120
#define ASCII_LOWER_Y         121
#define ASCII_LOWER_Z         122

#define ASCII_PUNCT_LBRACE    123
#define ASCII_PUNCT_PIPE      124
#define ASCII_PUNCT_RBRACE    125
#define ASCII_PUNCT_TILDE     126

#define ASCII_CTRL_DEL        127


// Alphanumeric Keys

#define KEYBOARD_SCANCODE_ESCAPE          0x01
#define KEYBOARD_SCANCODE_1               0x02
#define KEYBOARD_SCANCODE_2               0x03
#define KEYBOARD_SCANCODE_3               0x04
#define KEYBOARD_SCANCODE_4               0x05
#define KEYBOARD_SCANCODE_5               0x06
#define KEYBOARD_SCANCODE_6               0x07
#define KEYBOARD_SCANCODE_7               0x08
#define KEYBOARD_SCANCODE_8               0x09
#define KEYBOARD_SCANCODE_9               0x0A
#define KEYBOARD_SCANCODE_0               0x0B
#define KEYBOARD_SCANCODE_MINUS           0x0C
#define KEYBOARD_SCANCODE_EQUAL           0x0D
#define KEYBOARD_SCANCODE_BACKSPACE       0x0E
#define KEYBOARD_SCANCODE_TAB             0x0F

#define KEYBOARD_SCANCODE_Q               0x10
#define KEYBOARD_SCANCODE_W               0x11
#define KEYBOARD_SCANCODE_E               0x12
#define KEYBOARD_SCANCODE_R               0x13
#define KEYBOARD_SCANCODE_T               0x14
#define KEYBOARD_SCANCODE_Y               0x15
#define KEYBOARD_SCANCODE_U               0x16
#define KEYBOARD_SCANCODE_I               0x17
#define KEYBOARD_SCANCODE_O               0x18
#define KEYBOARD_SCANCODE_P               0x19
#define KEYBOARD_SCANCODE_LEFT_BRACKET    0x1A
#define KEYBOARD_SCANCODE_RIGHT_BRACKET   0x1B
#define KEYBOARD_SCANCODE_ENTER           0x1C
#define KEYBOARD_SCANCODE_LEFT_CTRL       0x1D
#define KEYBOARD_SCANCODE_A               0x1E
#define KEYBOARD_SCANCODE_S               0x1F

#define KEYBOARD_SCANCODE_D               0x20
#define KEYBOARD_SCANCODE_F               0x21
#define KEYBOARD_SCANCODE_G               0x22
#define KEYBOARD_SCANCODE_H               0x23
#define KEYBOARD_SCANCODE_J               0x24
#define KEYBOARD_SCANCODE_K               0x25
#define KEYBOARD_SCANCODE_L               0x26
#define KEYBOARD_SCANCODE_SEMICOLON       0x27
#define KEYBOARD_SCANCODE_APOSTROPHE      0x28
#define KEYBOARD_SCANCODE_BACKTICK        0x29
#define KEYBOARD_SCANCODE_LEFT_SHIFT      0x2A
#define KEYBOARD_SCANCODE_BACKSLASH       0x2B
#define KEYBOARD_SCANCODE_Z               0x2C
#define KEYBOARD_SCANCODE_X               0x2D
#define KEYBOARD_SCANCODE_C               0x2E
#define KEYBOARD_SCANCODE_V               0x2F

#define KEYBOARD_SCANCODE_B               0x30
#define KEYBOARD_SCANCODE_N               0x31
#define KEYBOARD_SCANCODE_M               0x32
#define KEYBOARD_SCANCODE_COMMA           0x33
#define KEYBOARD_SCANCODE_PERIOD          0x34
#define KEYBOARD_SCANCODE_SLASH           0x35
#define KEYBOARD_SCANCODE_RIGHT_SHIFT     0x36
#define KEYBOARD_SCANCODE_KP_ASTERISK     0x37
#define KEYBOARD_SCANCODE_LEFT_ALT        0x38
#define KEYBOARD_SCANCODE_SPACE           0x39
#define KEYBOARD_SCANCODE_CAPSLOCK        0x3A

// Function Keys

#define KEYBOARD_SCANCODE_F1              0x3B
#define KEYBOARD_SCANCODE_F2              0x3C
#define KEYBOARD_SCANCODE_F3              0x3D
#define KEYBOARD_SCANCODE_F4              0x3E
#define KEYBOARD_SCANCODE_F5              0x3F
#define KEYBOARD_SCANCODE_F6              0x40
#define KEYBOARD_SCANCODE_F7              0x41
#define KEYBOARD_SCANCODE_F8              0x42
#define KEYBOARD_SCANCODE_F9              0x43
#define KEYBOARD_SCANCODE_F10             0x44
#define KEYBOARD_SCANCODE_F11             0x57
#define KEYBOARD_SCANCODE_F12             0x58

// Numeric Keypad

#define KEYBOARD_SCANCODE_NUMLOCK         0x45
#define KEYBOARD_SCANCODE_SCROLLLOCK      0x46

#define KEYBOARD_SCANCODE_KP_7            0x47
#define KEYBOARD_SCANCODE_KP_8            0x48
#define KEYBOARD_SCANCODE_KP_9            0x49
#define KEYBOARD_SCANCODE_KP_MINUS        0x4A
#define KEYBOARD_SCANCODE_KP_4            0x4B
#define KEYBOARD_SCANCODE_KP_5            0x4C
#define KEYBOARD_SCANCODE_KP_6            0x4D
#define KEYBOARD_SCANCODE_KP_PLUS         0x4E
#define KEYBOARD_SCANCODE_KP_1            0x4F
#define KEYBOARD_SCANCODE_KP_2            0x50
#define KEYBOARD_SCANCODE_KP_3            0x51
#define KEYBOARD_SCANCODE_KP_0            0x52
#define KEYBOARD_SCANCODE_KP_PERIOD       0x53

// Extended Keys

#define KEYBOARD_SCANCODE_E0              0xE0

#define KEYBOARD_SCANCODE_KP_DIVIDE       0x35   /* real code: E0 35 */
#define KEYBOARD_SCANCODE_KP_ENTER        0x1C   /* real code: E0 1C */

#define KEYBOARD_SCANCODE_RIGHT_CTRL      0x1D   /* E0 1D */
#define KEYBOARD_SCANCODE_RIGHT_ALT       0x38   /* E0 38 */

#define KEYBOARD_SCANCODE_INSERT          0x52   /* E0 52 */
#define KEYBOARD_SCANCODE_DELETE          0x53   /* E0 53 */
#define KEYBOARD_SCANCODE_HOME            0x47   /* E0 47 */
#define KEYBOARD_SCANCODE_END             0x4F   /* E0 4F */
#define KEYBOARD_SCANCODE_PAGE_UP         0x49   /* E0 49 */
#define KEYBOARD_SCANCODE_PAGE_DOWN       0x51   /* E0 51 */

#define KEYBOARD_SCANCODE_ARROW_UP        0x48   /* E0 48 */
#define KEYBOARD_SCANCODE_ARROW_LEFT      0x4B   /* E0 4B */
#define KEYBOARD_SCANCODE_ARROW_RIGHT     0x4D   /* E0 4D */
#define KEYBOARD_SCANCODE_ARROW_DOWN      0x50   /* E0 50 */

typedef struct keyboard_event {
    uint32_t keycode;
    bool pressed;
} keyboard_event_t;

typedef void (*keyboard_listener_t)(keyboard_event_t * event);

typedef struct keyboard_driver {
    void (*register_listener)(keyboard_listener_t listener);
} keyboard_driver_t;

void keyboard_transfer_scancode(uint32_t scancode, keyboard_event_t * event);

#endif
