#ifndef OPENGL_TEXT_H
#define OPENGL_TEXT_H

typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } Align;

extern void * font;
extern int font_line_height;

void disp_puts(int x, int y, Align mode, const char *string);
void disp_printf(int x, int y, Align mode, const char* format, ...)
    __attribute__ ((__format__ (__printf__, 4, 5)));

#endif //OPENGL_TEXT_H
