#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

void terminal_initialize(void);
void terminal_run(void);
void terminal_putchar(char c);
void terminal_write(const char* str);
void terminal_writeline(const char* str);
void terminal_clear(void);
void terminal_set_color(uint8_t fg, uint8_t bg);
void terminal_read_line(char* buffer, int max_len);
char terminal_getchar(void);
void terminal_write_int(int num);

extern int cursor_x;
extern int cursor_y;
extern int header_lines;
void draw_header(void);

void game_qmatrix(void);
void cmd_sysinfo(void);
void qurofetch(void);

// Mouse support
void mouse_initialize(void);
void mouse_poll(void);
void mouse_draw(void);
void mouse_clear(void);

#endif
