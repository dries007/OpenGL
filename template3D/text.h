#ifndef OPENGL_TEXT_H
#define OPENGL_TEXT_H

typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } Align;

extern void * font;
extern int font_line_height;

void display_string(int x, int y, Align mode, const char *string);
void display_number(int x, int y, Align mode, int number, const char* format);
void display_decimal(int x, int y, Align mode, int number);

#endif //OPENGL_TEXT_H
