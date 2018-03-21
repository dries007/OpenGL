#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <ctype.h>
#include <stdbool.h>

#include "helpers.h"
#include "text.h"

/*
 * Todo: Allow change in camera focus
 * Todo: Add WASD/ZQSD controls for moving camera ('game mode'?)
 * Todo: Allow tilting of the 'up' direction (?)
 * Todo: Add animated model
 * Todo: Mouse stuff (?)
 * Todo: Add scroll for zoom (?)
 */

const char *TITLE = "Template 3D Project";

bool overlay = true;

/* Callback functions */
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void reshape(int w, int h);
void display(void);

/* Keyboard input callback */
void keyboard(unsigned char key, int x, int y)
{
    if (isgraph(key))
        fprintf(stderr, "Keypress %d (%c), mouse %d;%d\n", key, key, x, y);
    else
        fprintf(stderr, "Keypress %d (0x%X), mouse %d;%d\n", key, key, x, y);

    int modifiers = glutGetModifiers();
    int delta = 1;
    if (modifiers & GLUT_ACTIVE_ALT) delta *= 10;

    switch (key)
    {
        case 27: exit(0);
        default: return;

        case 'Y':
            delta *= -1;
        case 'y':
            camera.eye.y += delta;
            fprintf(stderr, "Camera eye Y%+d = %lf\n", delta, camera.eye.y);
            break;

        case 'X':
            delta *= -1;
        case 'x':
            camera.eye.x += delta;
            fprintf(stderr, "Camera eye X%+d = %lf\n", delta, camera.eye.x);
            break;

        case 'Z':
            delta *= -1;
        case 'z':
            camera.eye.x += delta;
            fprintf(stderr, "Camera eye Z%+d = %lf\n", delta, camera.eye.x);
            break;

        case 'p': case 'P':
            switch (perspective.type)
            {
                case PERSP_TYPE_ORTHO:
                    fprintf(stderr, "Switching to Frustum projection.\n");
                    perspective.type = PERSP_TYPE_FRUSTUM;
                    break;
                case PERSP_TYPE_FRUSTUM:
                    fprintf(stderr, "Switching to FOV projection.\n");

                    perspective.type = PERSP_TYPE_FOV;
                    perspective.fov = 90;
                    perspective.aspect = (double) window.width / window.height;
                    break;
                case PERSP_TYPE_FOV:
                    fprintf(stderr, "Switching to Ortho projection.\n");
                    perspective.type = PERSP_TYPE_ORTHO;
                    perspective.left = -10;
                    perspective.right = 10;
                    perspective.bottom = -10;
                    perspective.top = 10;
                    break;
            }
            reshape(window.width, window.height);
            break;
    }

    glutPostRedisplay();
}

/* Special (meta kays) keyboard input callback */
void special(int key, int x, int y)
{
    fprintf(stderr, "Special keypress %d, mouse %d;%d\n", key, x, y);

    switch (key)
    {
        default: return;
    }

    glutPostRedisplay();
}

/* Window reshaped callback */
void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    window.width = w;
    window.height = h;

    switch (perspective.type)
    {
        case PERSP_TYPE_ORTHO:
            fprintf(stderr, "Ortho projection (%lf;%lf;%lf) -> (%lf;%lf;%lf).\n",
                    perspective.left, perspective.bottom, perspective.near,
                    perspective.right, perspective.top, perspective.far);
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top,
                    perspective.near, perspective.far);
            break;
        case PERSP_TYPE_FRUSTUM:
            fprintf(stderr, "Frustum projection (%lf;%lf;%lf) -> (%lf;%lf;%lf).\n",
                    perspective.left, perspective.bottom, perspective.near,
                    perspective.right, perspective.top, perspective.far);
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top,
                    perspective.near, perspective.far);
            break;
        case PERSP_TYPE_FOV:
            perspective.aspect = (double) window.width / window.height;
            fprintf(stderr, "FOV projection %lf° asp: %lf n: %lf f: %lf\n",
                    perspective.fov, perspective.aspect, perspective.near, perspective.far);
            gluPerspective(perspective.fov, perspective.aspect,
                           perspective.near, perspective.far);
            break;
    }

    glViewport(0, 0, w, h);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Overlay! */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, window.width, 0, window.height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    /* 2D overlay mode, */

    glColor3f(1, 1, 0);
    display_string(0, window.height - font_line_height, ALIGN_LEFT, "Test Test\nNL");
    glColor3f(1, 0, 1);
    display_string(0, window.height - 2 * font_line_height, ALIGN_CENTER, "Test Test\nNL");
    glColor3f(0, 1, 1);
    display_string(0, window.height - 3 * font_line_height, ALIGN_RIGHT, "Test Test\nNL");

    glPopAttrib();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    /* Back to normal */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* todo: replace? */
    gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z,
              camera.center.x, camera.center.y, camera.center.z,
              camera.up.x, camera.up.y, camera.up.z);

    drawAxis(1000);

    glColor3f(0, 0, 0);
    glutWireCube(1);
    glutSolidTeapot(1);
    glColor3f(1, 1, 1);
    glutWireTeapot(1);

    glFlush();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow(TITLE);

    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutMainLoop();
}