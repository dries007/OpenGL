#include "user_input.h"
#include "text.h"
#include <GL/glut.h>
#include <ctype.h>


bool overlay = true;
bool mouseLeftDown = false;
bool mouseRightDown = false;
bool mouseMiddleDown = false;
double mouseZoomDiv = 10;
double mouseRotateDiv = 2.5;
double mousePanDiv = 10;
Vect2i prevMouse = {0, 0};
bool wire = false;
bool move = false;
bool flat = false;

const char *TITLE = "3D Project";
const char *KEYMAP = "Common keys:\n"
                     "- ESC: Quit\n"
                     "- F1: Toggle overlay\n"
                     "- F2: Change perspective\n"
                     "- F3: Change mode\n"
                     "- F4: Flat or smooth shading\n"
                     "Movement modifiers:\n"
                     "- Shift = x5 or x-1\n"
                     "- Alt = x10\n"
                     "- Ctrl = x0.1\n"
                     "Features:\n"
                     "- 1: Toggle wireframe\n"
                     "- 2: Toggle movement\n"
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
        case '1':
            wire = !wire;
            break;
        case '2':
            move = !move;
            break;
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
                    /*camera.target = (Vect3d){0, 0, 0};*/
                    camera.target.x = 0;
                    camera.target.y = 0;
                    camera.target.z = 0;
                    break;
            }
            break;
        case GLUT_KEY_F4:
            flat = !flat;
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

    /* prevMouse = (Vect2i) {x, y};*/
    prevMouse.x = x;
    prevMouse.y = y;

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


void drawOverlay()
{
    /* START "context" switch, to overlay mode */
    glMatrixMode(GL_PROJECTION); /* Change to projection required. */
    glPushMatrix(); /* Save Projection matrix */
    glLoadIdentity(); /* Cleanup */
    gluOrtho2D(0, window.width, window.height, 0); /* Got to 2D view with (0;0) TL -> (w;h) BR */
    glMatrixMode(GL_MODELVIEW); /* Back to drawing stuff */
    glLoadIdentity(); /* Cleanup */
    //glPushAttrib(GL_DEPTH_TEST); /* Save GL_DEPTH_TEST */
    glDisable(GL_DEPTH_TEST); /* No depth test for overlay elements */
    /* END "context" switch, to overlay mode */

//    glColor3f(0, 0, 0);
    glColor3f(1, 1, 1);

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

    if (flat) disp_puts(&cursorR, ALIGN_RIGHT, "Flat shading\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Smooth shading\n");

    if (wire) disp_puts(&cursorR, ALIGN_RIGHT, "Wire mode\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Full mode\n");

    if (move) disp_puts(&cursorR, ALIGN_RIGHT, "Movement on\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Movement off\n");

    if (camera.type != CAM_TYPE_ABSOLUTE)
    {
        glColor4f(1, 1, 1, .9);
        drawCrosshair(10);
    }

    //glPopAttrib(); /* Restore GL_DEPTH_TEST */
    glMatrixMode(GL_PROJECTION); /* Change to projection required. */
    glPopMatrix(); /* Restore projection matrix */
}

