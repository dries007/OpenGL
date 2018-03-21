#include "text.h"
#include "helpers.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <GL/glut.h>
#include <stdbool.h>


void* font = GLUT_BITMAP_9_BY_15;
int font_line_height = 15;

double scale_y(double y)
{
    return y; //(int) (y * window.height);
}
double scale_x(double x)
{
    return x / (double) window.width; //x; //(int) (x * window.width);
}

void display_string(const int x, const int y, Align mode, const char *string)
{
    glPushMatrix();
    if (string == NULL || string[0] == '\0') return;
    double xx = x, yy = y; /* tmp coords */
    double div = 1; /* 1 = right, 2 = center */
    switch (mode)
    {
        case ALIGN_LEFT:
        {
            /* Move to starting position, first line */
            glRasterPos2d(xx, yy);
            for (const char * cp = string; *cp != '\0'; cp++)
            {
                if (*cp == '\n')
                {
                    yy -= scale_y(font_line_height);
                    glRasterPos2d(xx, yy);
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
            float w = 0; /* width */
            const char *cp = string; /* for with */
            const char *line = string; /* for printing */
            while (true)
            {
                if (*cp == '\n' || *cp == '\0')
                {
                    /* Move to center */
                    xx = x - scale_x(w) / div;
                    w = 0;
                    glRasterPos2d(xx, yy);

                    /* Print current line */
                    while (*line != '\n' && *line != '\0') glutBitmapCharacter(font, *line++);

                    if (*cp == '\0') break;

                    /* Skip over \n */
                    line = cp + 1;
                    /* Move down */
                    yy -= scale_y(font_line_height);
                }
                else
                {
                    w += glutBitmapWidth(font, *cp);
                    printf("%d\n", glutBitmapWidth(font, *cp));
                }
                cp++;
            }
        }
    }
    glPopMatrix();
}

void display_number(int x, int y, Align mode, int number, const char* format)
{
    static char* buffer;
    static int bufferSize;
    /* snprintf(NULL, 0, ...) returns length string would have been, +1 for NULL */
    int requiredBuffer = snprintf(NULL, 0, format, number) + 1;

    if (requiredBuffer > bufferSize) buffer = realloc(buffer, requiredBuffer);
    if (buffer == NULL) error(1, "Out of memory on display_number.\n");

    sprintf(buffer, format, number);
    display_string(x, y, mode, buffer);
}

void display_decimal(int x, int y, Align mode, int number)
{
    display_number(x, y, mode, number, "%d");
}
