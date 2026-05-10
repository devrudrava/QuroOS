#include "../../include/terminal.h"
#include "../../include/string.h"

void cmd_sysinfo(void) {
    char vendor[13] = "Generic x86";
    char brand[49] = "x86 Compatible CPU";
    
    terminal_set_color(0x0B, 0x00);
    terminal_writeline("System Information");
    terminal_writeline("==================");
    terminal_set_color(0x07, 0x00);
    
    terminal_write("OS: ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("Quro OS 1.0.0 (Quantum Edition)");
    
    terminal_set_color(0x07, 0x00);
    terminal_write("CPU Vendor: ");
    terminal_set_color(0x0A, 0x00);
    terminal_writeline(vendor);
    
    terminal_set_color(0x07, 0x00);
    terminal_write("CPU Brand: ");
    terminal_set_color(0x0A, 0x00);
    terminal_writeline(brand);
    
    terminal_set_color(0x07, 0x00);
    terminal_write("Display: ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("VGA 80x25 Text Mode");
    
    terminal_set_color(0x07, 0x00);
    terminal_write("Memory: ");
    terminal_set_color(0x0F, 0x00);
    terminal_writeline("256MB RAM");
}
