#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <GL/glut.h>

#include "helpers.h"
#include "text.h"

/*
 * Todo: Add animated model
 * Todo: Mouse stuff (?)
 * Todo: Add scroll for zoom (?)
 */

const char *TITLE = "Template 3D Project";
const char *KEYMAP = "Common keys:\n"
        "- ESC: Quit\n"
        "- F1: Toggle overlay\n"
        "- F2: Change perspective\n"
        "- F3: Change mode\n"
        "Movement modifiers:\n"
        "- Shift = x5 or x-1\n"
        "- Alt = x10\n"
        "- Ctrl = x0.1\n"
;
const char *KEYMAP_ABSOLUTE = "Absolute mode:\n"
        "- XTY: Move cam\n"
        "- UVW: Move focus\n"
;
const char *KEYMAP_AZERTY = "AZERTY mode:\n"
        "- ZS: Forward & Back\n"
        "- QD: Left & Right\n"
        "- AE: Rotate (Yaw)\n"
        "- FC: Rotate (Pitch)\n"
        "- HN: Absolute Y\n"
        "- IK: Absolute X\n"
        "- JL: Absolute Z\n"
;
const char *KEYMAP_QWERTY = "QWERTY mode:\n"
        "- WS: Forward & Back\n"
        "- AD: Left & Right\n"
        "- QE: Rotate (Yaw)\n"
        "- FC: Rotate (Pitch)\n"
        "- HN: Absolute Y\n"
        "- IK: Absolute X\n"
        "- JL: Absolute Z\n"
;
const char *MOUSE_ABSOLUTE = "Mouse:\n"
    //todo
;
const char *MOUSE_GAME = "Mouse:\n"
        "- Left: Strafe, Pan Y\n"
        "- Right: Rotate\n"
        "- Middle: Pan XZ\n"
        "- Scroll: Zoom\n"
;

static bool overlay = true;
static bool mouseLeftDown = false;
static bool mouseRightDown = false;
static bool mouseMiddleDown = false;
static double mouseZoomDiv = 10;
static double mouseRotateDiv = 2.5;
static double mousePanDiv = 10;
static Vect2i prevMouse = {0, 0};


/* Callback functions */
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void reshape(int w, int h);
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

/* Keyboard input callback */
void keyboard(unsigned char key, int x, int y)
{
    int modifiers = glutGetModifiers();

    /* Make ctrl+letter into lowercase letter */
    if (key < 0x20 && modifiers & GLUT_ACTIVE_CTRL) key |= 0x60;
    if (isupper(key)) key = tolower(key);

//    if (isprint(key)) fprintf(stderr, "Keypress %d (%c), mouse %d;%d\n", key, key, x, y);
//    else fprintf(stderr, "Keypress %d (0x%X), mouse %d;%d\n", key, key, x, y);

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
//    fprintf(stderr, "Special keypress %d, mouse %d;%d\n", key, x, y);

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
                case CAM_TYPE_ABSOLUTE:
                    camera.type = CAM_TYPE_GAME_AZERTY;
                    camera.pitch = 0;
                    camera.yaw = 0;
                    break;

                case CAM_TYPE_GAME_AZERTY:
                    camera.type = CAM_TYPE_GAME_QWERTY;
                    break;

                case CAM_TYPE_GAME_QWERTY:
                    camera.type = CAM_TYPE_ABSOLUTE;
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
//    fprintf(stderr, "Reshape, Window size %d;%d\n", w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    window.width = w;
    window.height = h;

    switch (perspective.type)
    {
        case PERSP_TYPE_ORTHO:
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top, perspective.near, perspective.far);
            break;

        case PERSP_TYPE_FRUSTUM:
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top, perspective.near, perspective.far);
            break;

        case PERSP_TYPE_FOV:
            perspective.aspect = (double) window.width / window.height;
            gluPerspective(perspective.fov, perspective.aspect, perspective.near, perspective.far);
            break;
    }

    glViewport(0, 0, w, h);
}

/* Mouse buttons */
void mouse(int button, int state, int x, int y)
{
//    fprintf(stderr, "Mouse %x %x %d %d\n", button, state, x, y);

    prevMouse = (Vect2i) {x, y};

    switch (button)
    {
        default: return;
        case GLUT_LEFT_BUTTON: mouseLeftDown = state == GLUT_DOWN; break;
        case GLUT_RIGHT_BUTTON: mouseRightDown = state == GLUT_DOWN; break;
        case GLUT_MIDDLE_BUTTON: mouseMiddleDown = state == GLUT_DOWN; break;
        case 3:
        case 4:
            // Scroll wheel
            switch (camera.type)
            {
                case CAM_TYPE_ABSOLUTE:
                    //todo
                    break;
                case CAM_TYPE_GAME_AZERTY:
                case CAM_TYPE_GAME_QWERTY:
                    moveCamera(button == 3 ? 1 : -1, 0, 0, 0, 0, 0, 0); // zoom
                    break;
            }
    }
}

/* Mouse dragging */
void motion(int x, int y)
{
//    fprintf(stderr, "Motion %d %d\n", x, y);

    double dx = prevMouse.x - x;
    double dy = prevMouse.y - y;
    prevMouse.x = x;
    prevMouse.y = y;

    switch (camera.type)
    {
        case CAM_TYPE_ABSOLUTE:
            //todo
            break;
        case CAM_TYPE_GAME_AZERTY:
        case CAM_TYPE_GAME_QWERTY:
            if (mouseLeftDown) moveCamera(0, dx/mouseZoomDiv, 0, 0, 0, dy/mousePanDiv, 0); // Y & strafe
            if (mouseRightDown) moveCamera(0, 0, -dx/mouseRotateDiv, dy/mouseRotateDiv, 0, 0, 0); // yaw & pitch
            if (mouseMiddleDown) moveCamera(0, 0, 0, 0, dx/mousePanDiv, 0, dy/mousePanDiv); // pan X Z
            break;
    }
}

void display(void)
{
//    fprintf(stderr, "Display\n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* Nuke everything */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (camera.type)
    {
        case CAM_TYPE_ABSOLUTE:
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

    glPushMatrix();
    glColor3f(0, 0, 0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 1.8, 0);
    glColor3f(1, 1, 1);
    glutSolidTeapot(1);
    glColor3f(0, 0, 0);
    glutWireTeapot(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-5, 1.5, -5);
    glColor3f(1, 0, 0);
    glutSolidDodecahedron();
    glColor3f(0, 0, 0);
    glutWireDodecahedron();
    glPopMatrix();

    glPushMatrix();
    glTranslated(5, 1.5, -5);
    glColor3f(0, 1, 0);
    glutSolidOctahedron();
    glColor3f(0, 0, 0);
    glutWireOctahedron();
    glPopMatrix();

    glPushMatrix();
    glTranslated(5, 1.5, 5);
    glColor3f(0, 0, 1);
    glutSolidTetrahedron();
    glColor3f(0, 0, 0);
    glutWireTetrahedron();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-5, 1.5, 5);
    glColor3f(1, 0, 1);
    glutSolidIcosahedron();
    glColor3f(0, 0, 0);
    glutWireIcosahedron();
    glPopMatrix();

    if (camera.type == CAM_TYPE_ABSOLUTE)
    {
        glPushMatrix();
        glTranslated(camera.target.x, camera.target.y, camera.target.z);
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(3);
        drawAxis(1);
        glPopAttrib(); /* GL_LINE_WIDTH */
        glPopMatrix();
    }

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

        Vect2i cursorL = {5, font_line_height};
        Vect2i cursorR = {window.width-5, font_line_height};

        disp_puts(&cursorL, ALIGN_LEFT, KEYMAP);

        disp_printf(&cursorR, ALIGN_RIGHT, "Camera: X: %4.2lf Y: %4.2lf Z: %4.2lf\n", camera.pos.x, camera.pos.y, camera.pos.z);
        switch (camera.type)
        {
            case CAM_TYPE_ABSOLUTE:
                disp_printf(&cursorR, ALIGN_RIGHT, "Target: X: %4.2lf Y: %4.2lf Z: %4.2lf\n", camera.target.x, camera.target.y, camera.target.z);
                disp_puts(&cursorL, ALIGN_LEFT, KEYMAP_ABSOLUTE);
                disp_puts(&cursorL, ALIGN_LEFT, MOUSE_ABSOLUTE);
                break;
            case CAM_TYPE_GAME_AZERTY:
                disp_printf(&cursorR, ALIGN_RIGHT, "Yaw: %4.2lf Pitch: %4.2lf\n", camera.yaw, camera.pitch);
                disp_puts(&cursorL, ALIGN_LEFT, KEYMAP_AZERTY);
                disp_puts(&cursorL, ALIGN_LEFT, MOUSE_GAME);
                break;

            case CAM_TYPE_GAME_QWERTY:
                disp_printf(&cursorR, ALIGN_RIGHT, "Yaw: %4.2lf Pitch: %4.2lf\n", camera.yaw, camera.pitch);
                disp_puts(&cursorL, ALIGN_LEFT, KEYMAP_QWERTY);
                disp_puts(&cursorL, ALIGN_LEFT, MOUSE_GAME);
                break;
        }

        disp_printf(&cursorR, ALIGN_RIGHT, "Render distance %4.2lf - %4.2lf\n", perspective.near, perspective.far);

        switch (perspective.type)
        {
            case PERSP_TYPE_ORTHO: disp_puts(&cursorR, ALIGN_RIGHT, "Perspective: Ortho\n"); break;
            case PERSP_TYPE_FRUSTUM: disp_puts(&cursorR, ALIGN_RIGHT, "Perspective: Frustum\n"); break;
            case PERSP_TYPE_FOV: disp_puts(&cursorR, ALIGN_RIGHT, "Perspective: FOV\n"); break;
        }
        switch (camera.type)
        {
            case CAM_TYPE_ABSOLUTE: disp_puts(&cursorR, ALIGN_RIGHT, "Camera Absolute mode.\n"); break;
            case CAM_TYPE_GAME_AZERTY: disp_puts(&cursorR, ALIGN_RIGHT, "Camera AZERTY mode\n"); break;
            case CAM_TYPE_GAME_QWERTY: disp_puts(&cursorR, ALIGN_RIGHT, "Camera QWERTY mode\n"); break;
        }

        if (camera.type != CAM_TYPE_ABSOLUTE)
        {
            glColor4f(1, 1, 1, .9);
            drawCrosshair(10);
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

    glClearColor(.7, .7, .7, 0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_INFO);

    puts("Basic OpenGL/GLU/GLUT template program. 2018 - Dries007\n");
    puts(KEYMAP);
    puts(KEYMAP_ABSOLUTE);
    puts(MOUSE_ABSOLUTE);
    puts(KEYMAP_AZERTY);
    puts(KEYMAP_QWERTY);
    puts(MOUSE_GAME);

    glutMainLoop();
}