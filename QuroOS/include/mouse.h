#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

typedef struct {
    int x;
    int y;
    int left_button;
    int right_button;
    int middle_button;
} mouse_state_t;

void mouse_init(void);
void mouse_poll(void);
void mouse_draw_cursor(void);
void mouse_erase_cursor(void);
int mouse_get_x(void);
int mouse_get_y(void);
int mouse_left_pressed(void);
int mouse_right_pressed(void);
int mouse_middle_pressed(void);

extern mouse_state_t mouse;

#endif
