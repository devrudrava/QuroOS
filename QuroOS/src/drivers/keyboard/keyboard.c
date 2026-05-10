#include "../../../include/keyboard.h"
#include "../../../include/io.h"

static int shift_pressed = 0;
static int caps_lock = 0;

static const char keymap_normal[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const char keymap_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_initialize(void) { shift_pressed = 0; caps_lock = 0; }

char keyboard_getchar(void) {
    unsigned char scancode;
    while (!(inb(0x64) & 0x01)) { asm volatile("pause"); }
    scancode = inb(0x60);
    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = 1; return 0; }
    if (scancode == 0xAA || scancode == 0xB6) { shift_pressed = 0; return 0; }
    if (scancode == 0x3A) { caps_lock = !caps_lock; return 0; }
    if (scancode & 0x80) return 0;
    char c = (shift_pressed || caps_lock) ? keymap_shift[scancode] : keymap_normal[scancode];
    if (caps_lock && !shift_pressed && c >= 'a' && c <= 'z') c -= 32;
    return c;
}
int key_available(void) { return inb(0x64) & 0x01; }
