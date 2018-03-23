#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
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

/**
 * GAME_KEYS
 * if 1: use ZQSD to move camera + FC for up/down, AE to rotate, IJKL to move focus + H/N for up/down
 * if 2: Same as 1 for for QWERTY
 */
#define GAME_KEYS 1

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
    double delta = 1;

    int modifiers = glutGetModifiers();
    if (modifiers & GLUT_ACTIVE_ALT) delta *= 10.0;
    if (modifiers & GLUT_ACTIVE_CTRL) delta *= 0.1;
    if (modifiers & GLUT_ACTIVE_SHIFT) delta *= -1.0;

    /* Make ctrl+letter into lowercase letter */
    if (key < 0x20 && modifiers & GLUT_ACTIVE_CTRL) key |= 0x60;
    if (isupper(key)) key = tolower(key);

    if (isprint(key)) fprintf(stderr, "Keypress %d (%c), mouse %d;%d\n", key, key, x, y);
    else fprintf(stderr, "Keypress %d (0x%X), mouse %d;%d\n", key, key, x, y);

    switch (key)
    {
        case 27: exit(0);
        default: return;
#if GAME_KEYS
        case 's':
            delta *= -1.0;
        case 'z':
        {
            Vect3d norm = norm3d(diff3d(camera.center, camera.pos));
            camera.pos.x += norm.x * delta;
            camera.pos.y += norm.y * delta;
            camera.pos.z += norm.z * delta;
        }
            break;
        case 'q':
            delta *= -1.0;
        case 'd':
        {
            Vect3d view = norm3d(diff3d(camera.center, camera.pos));
            Vect3d right = crossProduct3d(view, camera.up);
            printf("Right: %.2lf %.2lf %.2lf\n", right.x, right.y, right.z);
//            right = norm3d(right);

            camera.pos.x += right.x * delta;
            camera.pos.y += right.y * delta;
            camera.pos.z += right.z * delta;
        }
            break;
        case 'a':
            break;
        case 'e':
            break;
        case 'c':
            delta *= -1.0;
        case 'f':
            camera.pos.y += delta;
            break;
//todo: GAME_KEYS 2
#else
        case 'x':
            camera.pos.x += delta;
            fprintf(stderr, "Camera pos X%+.1lf = %lf\n", delta, camera.pos.x);
            break;

        case 'y':
            camera.pos.y += delta;
            fprintf(stderr, "Camera pos Y%+.1lf = %lf\n", delta, camera.pos.y);
            break;

        case 'z':
            camera.pos.z += delta;
            fprintf(stderr, "Camera pos Z%+.1lf = %lf\n", delta, camera.pos.z);
            break;

        case 'u':
            camera.center.y += delta;
            fprintf(stderr, "Camera center Y%+.1lf = %lf\n", delta, camera.center.y);
            break;

        case 'v':
            camera.center.x += delta;
            fprintf(stderr, "Camera center X%+.1lf = %lf\n", delta, camera.center.x);
            break;

        case 'w':
            camera.center.z += delta;
            fprintf(stderr, "Camera center Z%+.1lf = %lf\n", delta, camera.center.z);
            break;
#endif
        case 'o': overlay = !overlay; break;
        case 'p':
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
    fprintf(stderr, "Reshape, Window size %d;%d\n", w, h);

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
    fprintf(stderr, "Display\n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* Nuke everything */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z,
              camera.center.x, camera.center.y, camera.center.z,
              camera.up.x, camera.up.y, camera.up.z);

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
        Vect2i cursor = {0, font_line_height};
        cursor = disp_printf(cursor, ALIGN_LEFT,
                    "Eye:    X: %4.2lf Y: %4.2lf Z: %4.2lf\n"
                    "Center: X: %4.2lf Y: %4.2lf Z: %4.2lf\n"
                    "Delta:  %4.2lf",
                    camera.pos.x, camera.pos.y, camera.pos.z,
                    camera.center.x, camera.center.y, camera.center.z, dist(diff3d(camera.pos, camera.center))
        );
        cursor.y += font_line_height;
        cursor = disp_printf(cursor, ALIGN_LEFT,
                    "Render distance %4.2lf - %4.2lf",
                    perspective.near, perspective.far
        );

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

    glutMainLoop();
}