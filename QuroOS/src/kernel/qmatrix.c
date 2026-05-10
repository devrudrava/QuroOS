#include "../../include/terminal.h"
#include "../../include/keyboard.h"
#include "../../include/io.h"

#define VGA_MEMORY ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void game_qmatrix(void) {
    // Save screen state
    terminal_clear();
    
    int drops[VGA_WIDTH];
    int speeds[VGA_WIDTH];
    int lengths[VGA_WIDTH];
    int counters[VGA_WIDTH];
    int chars[VGA_WIDTH][VGA_HEIGHT];
    
    // Initialize
    for(int x = 0; x < VGA_WIDTH; x++) {
        drops[x] = -(x % 25);
        speeds[x] = 3 + (x % 8);
        lengths[x] = 5 + (x % 20);
        counters[x] = 0;
        for(int y = 0; y < VGA_HEIGHT; y++) {
            chars[x][y] = ' ';
        }
    }
    
    // Clear screen to black
    for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = (0x00 << 8) | ' ';
    }
    
    int running = 1;
    int frame = 0;
    
    while(running) {
        // Check for ESC key to exit immediately
        if(key_available()) {
            char c = keyboard_getchar();
            if(c == 27) {  // ESC key
                running = 0;
            }
        }
        
        // Update and draw all drops
        for(int x = 0; x < VGA_WIDTH; x += 1) {
            counters[x]++;
            if(counters[x] >= speeds[x]) {
                counters[x] = 0;
                drops[x]++;
                
                // Reset if off screen
                if(drops[x] > VGA_HEIGHT + lengths[x]) {
                    drops[x] = -(lengths[x]);
                    lengths[x] = 5 + ((x * 7 + frame) % 25);
                }
            }
            
            // Draw the drop
            int head_y = drops[x];
            for(int y = 0; y < VGA_HEIGHT; y++) {
                int pos = y * VGA_WIDTH + x;
                int distance = head_y - y;
                
                if(distance >= 0 && distance < lengths[x]) {
                    // Generate persistent character for this position
                    if(chars[x][y] == ' ') {
                        chars[x][y] = '!' + ((x * y * 7 + frame * 3) % 93);
                    }
                    
                    char c = chars[x][y];
                    
                    if(distance == 0) {
                        // Head - bright white
                        VGA_MEMORY[pos] = (0x0F << 8) | c;
                    } else if(distance == 1) {
                        // Near head - bright green
                        VGA_MEMORY[pos] = (0x0A << 8) | c;
                    } else if(distance < 4) {
                        // Middle - green
                        VGA_MEMORY[pos] = (0x02 << 8) | c;
                    } else if(distance < 8) {
                        // Lower - dark green
                        VGA_MEMORY[pos] = (0x08 << 8) | c;
                    } else {
                        // Tail - very dark
                        VGA_MEMORY[pos] = (0x00 << 8) | c;
                    }
                } else if(distance >= lengths[x] && 
                          VGA_MEMORY[pos] != ((0x00 << 8) | ' ')) {
                    // Fade out old characters
                    unsigned char attr = (VGA_MEMORY[pos] >> 8) & 0x0F;
                    if(attr == 0x00) {
                        VGA_MEMORY[pos] = (0x00 << 8) | ' ';
                        chars[x][y] = ' ';
                    } else if(attr == 0x08) {
                        VGA_MEMORY[pos] = (0x00 << 8) | chars[x][y];
                    }
                }
            }
        }
        
        // Slow delay
        for(volatile int d = 0; d < 200000; d++) {
            asm volatile("nop");
        }
        
        frame++;
        
        // Change characters periodically
        if(frame % 10 == 0) {
            for(int x = 0; x < VGA_WIDTH; x += 3) {
                for(int y = 0; y < VGA_HEIGHT; y += 2) {
                    if(chars[x][y] != ' ') {
                        chars[x][y] = '!' + ((x * y * 7 + frame * 3) % 93);
                    }
                }
            }
        }
    }
    
    // Clear screen on exit
    for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = (0x00 << 8) | ' ';
    }
    
    // Restore terminal
    terminal_clear();
    draw_header();
    cursor_x = 0;
    cursor_y = header_lines;
    terminal_set_color(0x0F, 0x00);
}
