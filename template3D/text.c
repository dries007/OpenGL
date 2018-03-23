#include "text.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <GL/glut.h>


void* font = GLUT_BITMAP_9_BY_15;
int font_line_height = 15;

Vect2i disp_puts(Vect2i pos, Align mode, const char *string)
{
    glPushMatrix();
    if (string == NULL || string[0] == '\0') return pos;
    int xx = pos.x, yy = pos.y; /* tmp coords */
    int div = 1; /* 1 = right, 2 = center */
    switch (mode)
    {
        case ALIGN_LEFT:
        {
            /* Move to starting position, first line */
            glRasterPos2i(xx, yy);
            for (const char * cp = string; *cp != '\0'; cp++)
            {
                if (*cp == '\n')
                {
                    yy += font_line_height;
                    glRasterPos2i(xx, yy);
                }
                else
                {
                    glutBitmapCharacter(font, *cp);
                }
            }
            break;
        }
        case ALIGN_CENTER:
        {
            div = 2;
        }
        case ALIGN_RIGHT:
        {
            int w = 0; /* width */
            const char *cp = string; /* for with */
            const char *line = string; /* for printing */
            while (true)
            {
                if (*cp == '\n' || *cp == '\0')
                {
                    /* Move to center */
                    xx = pos.x - w / div;
                    w = 0;
                    glRasterPos2i(xx, yy);

                    /* Print current line */
                    while (*line != '\n' && *line != '\0') glutBitmapCharacter(font, *line++);

                    if (*cp == '\0') break;

                    /* Skip over \n */
                    line = cp + 1;
                    /* Move down */
                    yy += font_line_height;
                }
                else
                {
                    w += glutBitmapWidth(font, *cp);
                }
                cp++;
            }
        }
    }
    glPopMatrix();
    return (Vect2i) {xx, yy};
}

Vect2i disp_printf(Vect2i pos, Align mode, const char *format, ...)
{
    static char* buffer;
    static int bufferSize;

    va_list args;
    va_start(args, format);
    /* vsnprintf(NULL, 0, ...) returns length string would have been, +1 for NULL */
    int requiredBuffer = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    if (requiredBuffer > bufferSize) buffer = realloc(buffer, requiredBuffer);
    if (buffer == NULL) error(1, "Out of memory on display_number.\n");

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return disp_puts(pos, mode, buffer);
}
