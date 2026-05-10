#include "../../include/bootsplash.h"

#define VGA_MEMORY ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define BLACK   0x00
#define LTGREEN 0x0A
#define LTCYAN  0x0B
#define LTRED   0x0C
#define LTMAG   0x0D
#define YELLOW  0x0E
#define WHITE   0x0F
#define LTBLUE  0x09
#define DKGREY  0x08

static void outb_wrapper(unsigned short port, unsigned char val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void disable_cursor() {
    outb_wrapper(0x3D4, 0x0A);
    outb_wrapper(0x3D5, 0x20);
}

static void clear_screen() {
    for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = (BLACK << 8) | ' ';
}

static void print_at(const char* str, int x, int y, unsigned char color) {
    int pos = y * VGA_WIDTH + x;
    while(*str) VGA_MEMORY[pos++] = (color << 8) | *str++;
}

// VERY slow delay using nested loops
static void delay_one_sec(void) {
    for(volatile unsigned long a = 0; a < 3000; a++) {
        for(volatile unsigned long b = 0; b < 30000; b++) {
            asm volatile("nop");
        }
    }
}

void show_boot_splash() {
    disable_cursor();
    clear_screen();
    
    print_at("   ___  _   _ ____   ___   ", 22, 3, LTRED);
    print_at("  / _ \\| | | |  _ \\ / _ \\  ", 22, 4, LTMAG);
    print_at(" | | | | | | | |_) | | | | ", 22, 5, LTCYAN);
    print_at(" | |_| | |_| |  _ <| |_| | ", 22, 6, LTGREEN);
    print_at("  \\__\\_\\\\___/|_| \\_\\\\___/  ", 22, 7, YELLOW);
    
    print_at("+------------------------------------------+", 15, 9, LTCYAN);
    print_at("Quro OS v1.0.0  |  x86 32-bit Protected Mode", 12, 11, WHITE);
    print_at("Build: " __DATE__ " " __TIME__, 22, 12, DKGREY);
    print_at("+------------------------------------------+", 15, 14, LTCYAN);
    
    // 5 seconds countdown
    for(int sec = 5; sec >= 1; sec--) {
        for(int i=0;i<80;i++) VGA_MEMORY[16*VGA_WIDTH+i]=(BLACK<<8)|' ';
        for(int i=0;i<80;i++) VGA_MEMORY[18*VGA_WIDTH+i]=(BLACK<<8)|' ';
        for(int i=0;i<80;i++) VGA_MEMORY[20*VGA_WIDTH+i]=(BLACK<<8)|' ';
        
        print_at("Starting in", 25, 16, WHITE);
        
        char num[2];
        num[0] = '0' + sec;
        num[1] = 0;
        print_at(num, 38, 16, LTRED);
        
        print_at("seconds...", 40, 16, WHITE);
        
        // Progress bar
        print_at("[", 10, 18, WHITE);
        int filled = (6 - sec) * 12;
        for(int i=0; i<filled; i++) VGA_MEMORY[18*VGA_WIDTH+11+i]=(LTGREEN<<8)|'=';
        for(int i=filled; i<60; i++) VGA_MEMORY[18*VGA_WIDTH+11+i]=(DKGREY<<8)|'-';
        VGA_MEMORY[18*VGA_WIDTH+71]=(WHITE<<8)|']';
        
        // Status
        if(sec==5) print_at("Initializing kernel...", 20, 20, DKGREY);
        if(sec==4) print_at("Setting up memory manager...", 20, 20, DKGREY);
        if(sec==3) print_at("Loading device drivers...", 20, 20, DKGREY);
        if(sec==2) print_at("Mounting filesystem...", 20, 20, DKGREY);
        if(sec==1) print_at("Starting Quro shell...", 20, 20, LTGREEN);
        
        delay_one_sec();
    }
    
    // Final screen
    clear_screen();
    print_at("   ___  _   _ ____   ___   ", 22, 3, LTRED);
    print_at("  / _ \\| | | |  _ \\ / _ \\  ", 22, 4, LTMAG);
    print_at(" | | | | | | | |_) | | | | ", 22, 5, LTCYAN);
    print_at(" | |_| | |_| |  _ <| |_| | ", 22, 6, LTGREEN);
    print_at("  \\__\\_\\\\___/|_| \\_\\\\___/  ", 22, 7, YELLOW);
    
    print_at("+------------------------------------------+", 15, 10, LTCYAN);
    print_at("SYSTEM READY - Starting terminal...", 18, 13, LTGREEN);
    print_at("+------------------------------------------+", 15, 16, LTCYAN);
    
    delay_one_sec();
}
