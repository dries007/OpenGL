#include <stdio.h>
#include <GL/glut.h>
#include <stdbool.h>

#include "helpers.h"

#define COL_WIDTH 100
#define MARGIN_L (COL_WIDTH/2)
#define MARGIN_R (COL_WIDTH/2)
#define MARGIN_T 100
#define MARGIN_B 10

/* Required globals for coordinate calculation */
int wc_left, wc_right, wc_bottom, wc_top;
int vp_w, vp_h;

/* Data globals */
int start_week, end_week, categories, weeks;
Gebruik data[AANT];
int sums[WLEN];
Colorf *colors;

/* Rendering options, toggled by keymap */
bool line = true;
bool values = true;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int i = start_week; i <= end_week; i++)
    {
        glColor3fv((GLfloat *) &colors[i - start_week]);
        glRecti(COL_WIDTH * i, 0, COL_WIDTH * i + COL_WIDTH, sums[i]);
        glColor3f(0, 0, 0);
        if (values)
        {
            display_number(COL_WIDTH * i + COL_WIDTH / 2, scale_y(5) + sums[i], ALIGN_CENTER, sums[i]);
            /* Meh, numbers above col looks better.
            display_number(wc_left, sums[i] + 5, ALIGN_LEFT, FONT, sums[i]);
            glLineStipple(10, 0xAAAA);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);
            glVertex2i(wc_left, sums[i]);
            glVertex2i(COL_WIDTH * i, sums[i]);
            glEnd();
            glDisable(GL_LINE_STIPPLE);
            */
        }
    }

    if (line)
    {
        glColor3f(0, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        for (int i = start_week; i <= end_week; i++)
        {
            glVertex2d(COL_WIDTH * i + COL_WIDTH / 2, sums[i]);
        }
        glEnd();
    }

    display_string(wc_left + scale_x(5), wc_top - scale_y(FONT_LINE_HEIGHT), ALIGN_LEFT, "Dries007\nTest\nLeft aligned!");
    display_string(wc_left + (wc_right - wc_left) / 2, wc_top - scale_y(FONT_LINE_HEIGHT), ALIGN_CENTER, "Dries007\nTest\nCentered aligned string!");
    display_string(wc_right - scale_x(5), wc_top - scale_y(FONT_LINE_HEIGHT), ALIGN_RIGHT, "Keymap:\n"
            "ESC, q, Q: Quit\n"
            "l, L: Toggle line\n"
            "v, V: Toggle values");

    glFlush();
}

void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(wc_left, wc_right, wc_bottom, wc_top);
    vp_w = w;
    vp_h = h;
    glViewport(0, 0, w, h);
}

void keyboard(unsigned char code, int mouseX, int mouseY)
{
    switch (code)
    {
        case 27: /* escape */
        case 'q':
        case 'Q':
            exit(0);
        case 'l':
        case 'L':
            line = !line;
            break;
        case 'v':
        case 'V':
            values = !values;
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    categories = read_gebruik_dat(data, AANT, &start_week, &end_week);
    if (categories <= 0) error(1, "Input data error. (categories <= 0)\n");
    weeks = end_week - start_week + 1;
    if (weeks <= 0) error(1, "Input data error. (weeks <= 0)\n");

    colors = calloc(weeks, sizeof(Colorf));
    if (colors == NULL) error(1, "Impossibly low on memory.\n");

    color_wheel(colors, end_week - start_week + 1, 1, 1, 0);

    int max_sum = sum_uren(data, categories, sums);

    wc_left = COL_WIDTH * start_week - MARGIN_L;
    wc_right = COL_WIDTH * end_week + COL_WIDTH + MARGIN_R;
    wc_bottom = -MARGIN_B;
    wc_top = max_sum + MARGIN_T;

    printf("Keymap:\n"
                   "ESC, q, Q: Quit\n"
                   "l, L: Toggle line\n"
                   "v, V: Toggle values\n"
    );

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Opgave 1");

    glClearColor(1, 1, 1, 0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    free(colors);

    return 0;
}
