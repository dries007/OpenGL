#ifndef OPENGL_HELPERS_H
#define OPENGL_HELPERS_H

#include "data.h"
#include "colors.h"

#define FONT GLUT_BITMAP_9_BY_15
#define FONT_LINE_HEIGHT 15

/* Required globals for coordinate calculation */
extern int wc_left, wc_right, wc_bottom, wc_top;
extern int vp_w, vp_h;

int scale_x(int x);
int scale_y(int y);

int scale_p2w_x(int x);
int scale_p2w_y(int y);

void error(int status, const char *message);

typedef enum align { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } Align;

void display_string(int x, int y, Align mode, const char *string);
void display_number(int x, int y, Align mode, int number);

#endif
