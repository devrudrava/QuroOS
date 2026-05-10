#include "../../include/terminal.h"
#include "../../include/keyboard.h"
#include "../../include/bootsplash.h"

void kernel_main(unsigned int magic, unsigned int addr) {
    (void)magic;
    (void)addr;
    
    show_boot_splash();
    terminal_initialize();
    keyboard_initialize();
    terminal_run();
    
    while(1) { asm volatile("hlt"); }
}
