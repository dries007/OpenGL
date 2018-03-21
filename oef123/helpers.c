#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

int scale_x(int x) { return x * (wc_right - wc_left) / vp_w; }
int scale_y(int y) { return y * (wc_top - wc_bottom) / vp_h; }

void error(int status, const char *message)
{
    fputs(message, stderr);
    exit(status);
}

void display_string(const int x, const int y, Align mode, const char *string)
{
    if (string == NULL || string[0] == '\0') return;
    int xx = x, yy = y; /* tmp coords */
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
                    yy -= scale_y(FONT_LINE_HEIGHT);
                    glRasterPos2i(xx, yy);
                }
                else
                {
                    glutBitmapCharacter(FONT, *cp);
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
                    xx = x - scale_x(w) / div;
                    w = 0;
                    glRasterPos2i(xx, yy);

                    /* Print current line */
                    while (*line != '\n' && *line != '\0') glutBitmapCharacter(FONT, *line++);

                    if (*cp == '\0') return;

                    /* Skip over \n */
                    line = cp + 1;
                    /* Move down */
                    yy -= scale_y(FONT_LINE_HEIGHT);
                }
                else
                {
                    w += glutBitmapWidth(FONT, *cp);
                }
                cp++;
            }
        }
    }
}

void display_number(int x, int y, Align mode, int number)
{
    /*
     * Find buffer size required to store number as string. Thanks StackOverflow!
     * +3 because there is always at least 1 char, a sign (-) is possible and we need a NULL byte.
     */
    char* buffer = malloc((number == 0 ? 0 : (int) floor(log10(abs(number)))) + 3);
    if (buffer == NULL) error(1, "Out of memory on display_number.\n");

    sprintf(buffer, "%d", number);
    display_string(x, y, mode, buffer);

    free(buffer);
}
