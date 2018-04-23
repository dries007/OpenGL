#ifndef OPENGL_TEXT_H
#define OPENGL_TEXT_H

#include "helpers.h"

typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } Align;

extern void * font;
extern int font_line_height;

void disp_puts(Vect2i * pos, Align mode, const char *string);
void disp_printf(Vect2i * pos, Align mode, const char* format, ...)
    __attribute__ ((__format__ (__printf__, 3, 4)));

#endif //OPENGL_TEXT_H
