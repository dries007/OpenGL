#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <GL/glut.h>

#include "helpers.h"
#include "text.h"

/*
 * Todo: Add WASD/ZQSD controls for moving camera ('game mode'?)
 * Todo: Allow tilting of the 'up' direction (?)
 * Todo: Add animated model
 * Todo: Mouse stuff (?)
 * Todo: Add scroll for zoom (?)
 */

const char *TITLE = "Template 3D Project";
const char *KEYMAP = "Keymap:\n"
        "- ESC: Quit\n"
        "- F1: Toggle overlay\n"
        "- F2: Change perspective\n"
        "- F3: Change mode\n"
;

bool overlay = true;

/* Callback functions */
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void reshape(int w, int h);
void display(void);

/* Keyboard input callback */
void keyboard(unsigned char key, int x, int y)
{
    int modifiers = glutGetModifiers();

    /* Make ctrl+letter into lowercase letter */
    if (key < 0x20 && modifiers & GLUT_ACTIVE_CTRL) key |= 0x60;
    if (isupper(key)) key = tolower(key);

    if (isprint(key)) fprintf(stderr, "Keypress %d (%c), mouse %d;%d\n", key, key, x, y);
    else fprintf(stderr, "Keypress %d (0x%X), mouse %d;%d\n", key, key, x, y);

    if (handleMove(key, modifiers, x, y)) return;

    switch (key)
    {
        default: return;
        case 27: exit(0);
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

        case GLUT_KEY_F1:
            overlay = !overlay;
            break;

        case GLUT_KEY_F2:
            switch (perspective.type)
            {
                case PERSP_TYPE_ORTHO:
                    perspective.type = PERSP_TYPE_FRUSTUM;
                    break;

                case PERSP_TYPE_FRUSTUM:
                    perspective.type = PERSP_TYPE_FOV;
                    perspective.fov = 90;
                    perspective.aspect = (double) window.width / window.height;
                    break;

                case PERSP_TYPE_FOV:
                    perspective.type = PERSP_TYPE_ORTHO;
                    perspective.left = -10;
                    perspective.right = 10;
                    perspective.bottom = -10;
                    perspective.top = 10;
                    break;
            }
            reshape(window.width, window.height);
            break;

        case GLUT_KEY_F3:
            switch (camera.type)
            {
                case CAM_TYPE_LOOK_AT:
                    camera.type = CAM_TYPE_GAME_AZERTY;
                    camera.pitch = 0;
                    camera.yaw = 0;
                    break;

                case CAM_TYPE_GAME_AZERTY:
                    camera.type = CAM_TYPE_GAME_QWERTY;
                    break;

                case CAM_TYPE_GAME_QWERTY:
                    camera.type = CAM_TYPE_LOOK_AT;
                    camera.target = (Vect3d){0, 0, 0};
                    break;
            }
            break;

    }

    glutPostRedisplay();
}

/* Window reshaped callback */
void reshape(int w, int h)
{
    fprintf(stderr, "Reshape, Window size %d;%d\n", w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    window.width = w;
    window.height = h;

    switch (perspective.type)
    {
        case PERSP_TYPE_ORTHO:
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top,
                    perspective.near, perspective.far);
            break;

        case PERSP_TYPE_FRUSTUM:
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top,
                    perspective.near, perspective.far);
            break;

        case PERSP_TYPE_FOV:
            perspective.aspect = (double) window.width / window.height;
            gluPerspective(perspective.fov, perspective.aspect,
                           perspective.near, perspective.far);
            break;
    }

    glViewport(0, 0, w, h);
}

void display(void)
{
    fprintf(stderr, "Display\n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* Nuke everything */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (camera.type)
    {
        case CAM_TYPE_LOOK_AT:
            gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0, 1, 0);
            break;

        case CAM_TYPE_GAME_AZERTY:
        case CAM_TYPE_GAME_QWERTY:
            glRotated(-camera.pitch, 1, 0, 0);
            glRotated(camera.yaw, 0, 1, 0);
            glTranslated(-camera.pos.x, -camera.pos.y, -camera.pos.z);
            break;
    }

    drawAxis(1000);
    drawCheckersZ(1, 100);

    glTranslated(0, 0.5, 0);

    glColor3f(0, 0, 0);
    glutWireCube(1);
    glutSolidTeapot(1);
    glColor3f(1, 1, 1);
    glutWireTeapot(1);

    glPushMatrix();

    glTranslated(-3, 0, -3);
    glColor3f(0, 0, 0);
    glutWireDodecahedron();
    glColor3f(1, 0, 0);
    glutSolidDodecahedron();

    glPopMatrix();

    if (overlay) /* Draw overlay last, so it's always 'on top' of the scene. */
    {
        /* START "context" switch, to overlay mode */
        glMatrixMode(GL_PROJECTION); /* Change to projection required. */
        glPushMatrix(); /* Save Projection matrix */
        glLoadIdentity(); /* Cleanup */
        gluOrtho2D(0, window.width, window.height, 0); /* Got to 2D view with (0;0) TL -> (w;h) BR */
        glMatrixMode(GL_MODELVIEW); /* Back to drawing stuff */
        glLoadIdentity(); /* Cleanup */
        glPushAttrib(GL_DEPTH_TEST); /* Save GL_DEPTH_TEST */
        glDisable(GL_DEPTH_TEST); /* No depth test for overlay elements */
        /* END "context" switch, to overlay mode */

        glColor3f(0, 0, 0);

        Vect2i cursorL = {0, font_line_height};
        Vect2i cursorR = {window.width, font_line_height};

        cursorL = disp_puts(cursorL, ALIGN_LEFT, KEYMAP);

        cursorR = disp_printf(cursorR, ALIGN_RIGHT, "Camera: X: %4.2lf Y: %4.2lf Z: %4.2lf\n", camera.pos.x, camera.pos.y, camera.pos.z);
        switch (camera.type)
        {
            case CAM_TYPE_LOOK_AT:
                cursorR = disp_printf(cursorR, ALIGN_RIGHT, "Target: X: %4.2lf Y: %4.2lf Z: %4.2lf\n", camera.target.x, camera.target.y, camera.target.z);
                break;
            case CAM_TYPE_GAME_AZERTY:
            case CAM_TYPE_GAME_QWERTY:
                cursorR = disp_printf(cursorR, ALIGN_RIGHT, "Yaw: %4.2lf Pitch: %4.2lf\n", camera.yaw, camera.pitch);
                break;
        }

        cursorR = disp_printf(cursorR, ALIGN_RIGHT,
                    "Render distance %4.2lf - %4.2lf\n",
                    perspective.near, perspective.far
        );

        switch (perspective.type)
        {
            case PERSP_TYPE_ORTHO: cursorR = disp_puts(cursorR, ALIGN_RIGHT, "Perspective: Ortho\n"); break;
            case PERSP_TYPE_FRUSTUM: cursorR = disp_puts(cursorR, ALIGN_RIGHT, "Perspective: Frustum\n"); break;
            case PERSP_TYPE_FOV: cursorR = disp_puts(cursorR, ALIGN_RIGHT, "Perspective: FOV\n"); break;
        }
        switch (camera.type)
        {
            case CAM_TYPE_LOOK_AT: cursorR = disp_puts(cursorR, ALIGN_RIGHT, "Camera Look-at mode.\n"); break;
            case CAM_TYPE_GAME_AZERTY: cursorR = disp_puts(cursorR, ALIGN_RIGHT, "Camera AZERTY mode\n"); break;
            case CAM_TYPE_GAME_QWERTY: cursorR = disp_puts(cursorR, ALIGN_RIGHT, "Camera QWERTY mode\n"); break;
        }

        glPopAttrib(); /* Restore GL_DEPTH_TEST */
        glMatrixMode(GL_PROJECTION); /* Change to projection required. */
        glPopMatrix(); /* Restore projection matrix */
    }

    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow(TITLE);

    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    puts("Basic OpenGL/GLU/GLUT template program. 2018 - Dries007\n");
    puts(KEYMAP);

    glutMainLoop();
}