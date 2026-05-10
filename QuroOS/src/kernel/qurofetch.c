#include "../../include/terminal.h"

#define VGA_MEMORY ((unsigned short*)0xB8000)
#define VGA_WIDTH 80

extern int cursor_x;
extern int cursor_y;

void qurofetch(void) {
    terminal_set_color(0x07, 0x00);
    terminal_writeline("");
    
    terminal_set_color(0x0B, 0x00);
    terminal_write("      /\\_____/\\      ");
    terminal_set_color(0x0B, 0x00);
    terminal_write("OS:       ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("Quro OS 1.0.0 (Quantum Edition)");
    
    terminal_set_color(0x09, 0x00);
    terminal_write("     /  o   o  \\     ");
    terminal_set_color(0x0B, 0x00);
    terminal_write("Kernel:   ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("Quro Monolithic v1.0");
    
    terminal_set_color(0x0B, 0x00);
    terminal_write("    ( ==  ^  == )    ");
    terminal_set_color(0x0B, 0x00);
    terminal_write("Arch:     ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("x86 (i386)");
    
    terminal_set_color(0x09, 0x00);
    terminal_write("     )         (     ");
    terminal_set_color(0x0B, 0x00);
    terminal_write("Display:  ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("VGA 80x25 Text Mode");
    
    terminal_set_color(0x0B, 0x00);
    terminal_write("    (           )    ");
    terminal_set_color(0x0B, 0x00);
    terminal_write("Shell:    ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("qurosh 1.0");
    
    terminal_set_color(0x09, 0x00);
    terminal_write("   ( (  )   (  ) )   ");
    terminal_set_color(0x0B, 0x00);
    terminal_write("Built:    ");
    terminal_set_color(0x07, 0x00);
    terminal_writeline(__DATE__ "  " __TIME__);
    
    terminal_set_color(0x08, 0x00);
    terminal_write("  (__(__)___(__)__)  ");
    terminal_writeline("");
    
    terminal_set_color(0x07, 0x00);
    terminal_write("  Palette:  ");
    
    for(uint8_t c = 1; c < 16; c++) {
        VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] = ((c << 4) | c) << 8 | ' ';
        VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x + 1] = ((c << 4) | c) << 8 | ' ';
        cursor_x += 2;
    }
    terminal_writeline("");
}
