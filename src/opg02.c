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
Colorf *colorsBars;
Colorf *colorsBarsMuted;
Colorf *colorsParts;
int selected = -1;

/* Rendering options, toggled by keymap */
bool line = false;
bool values = false;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int i = start_week; i <= end_week; i++)
    {
        if (selected == -1) glColor3fv((GLfloat *) &colorsBars[i - start_week]);
        else glColor3fv((GLfloat *) &colorsBarsMuted[i - start_week]);
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

    if (selected != -1)
    {
        int bottom = 0;
        for (int i = 0; i < categories; i++)
        {
            if (data[i].uren[selected] == 0) continue;

            printf("%s %d\n", data[i].naam, data[i].uren[selected]);

            glColor3fv((GLfloat *) &colorsParts[i]);
            glRecti(COL_WIDTH * selected, bottom, COL_WIDTH * selected + COL_WIDTH, bottom + data[i].uren[selected]);

            glColor3f(0, 0, 0);
            display_string(COL_WIDTH * selected + COL_WIDTH, bottom, ALIGN_LEFT, data[i].naam);
            if (values) display_number(COL_WIDTH * selected, bottom, ALIGN_RIGHT, data[i].uren[selected]);

            bottom += data[i].uren[selected];
        }
    }

    if (line)
    {
        glColor3f(0, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        for (int i = start_week; i <= end_week; i++) glVertex2d(COL_WIDTH * i + COL_WIDTH / 2, sums[i]);
        glEnd();
    }

    display_string(wc_right - scale_x(5), wc_top - scale_y(FONT_LINE_HEIGHT), ALIGN_RIGHT, "Click on a column to show details\n"
            "Keymap:\n"
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

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        int xx = wc_left + scale_x(x);
        int yy = wc_bottom + scale_y(vp_h - y);

        selected = -1;
        for (int i = start_week; i <= end_week; i++)
        {
            if (xx > i * COL_WIDTH && xx < i * COL_WIDTH + COL_WIDTH && yy > 0 && yy < sums[i])
            {
                selected = i;
                break;
            }
        }
        glutPostRedisplay();
    }
}

int main(int argc, char **argv)
{
    categories = read_gebruik_dat(data, AANT, &start_week, &end_week);
    if (categories <= 0) error(1, "Input data error. (categories <= 0)\n");
    colorsParts = calloc(categories, sizeof(Colorf));
    if (colorsParts == NULL) error(1, "Impossibly low on memory.\n");
    color_wheel(colorsParts, categories, 1, 1, 0);

    weeks = end_week - start_week + 1;
    if (weeks <= 0) error(1, "Input data error. (weeks <= 0)\n");
    colorsBars = calloc(weeks, sizeof(Colorf));
    if (colorsBars == NULL) error(1, "Impossibly low on memory.\n");
    color_wheel(colorsBars, weeks, 1, 1, 0);
    colorsBarsMuted = calloc(weeks, sizeof(Colorf));
    if (colorsBarsMuted == NULL) error(1, "Impossibly low on memory.\n");
    color_wheel(colorsBarsMuted, weeks, 1, 0.5, 0);

    int max_sum = sum_uren(data, categories, sums);

    wc_left = COL_WIDTH * start_week - MARGIN_L;
    wc_right = COL_WIDTH * end_week + COL_WIDTH + MARGIN_R;
    wc_bottom = -MARGIN_B;
    wc_top = max_sum + MARGIN_T;

    printf("left: %d right: %d bottom: %d top: %d\n", wc_left, wc_right, wc_bottom, wc_top);

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
    glutMouseFunc(mouse);

    glutMainLoop();

    free(colorsBars);

    return 0;
}
