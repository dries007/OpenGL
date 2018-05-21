#include "input.h"
#include "globals.h"

/* Private global */

/* shared between the two mouse functions to find the movement delta. */
static Vect2i prevMouse = {0, 0};
static bool mouseLeftDown = false;
static bool mouseRightDown = false;
static bool mouseMiddleDown = false;
static double mouseZoomDiv = 10;
static double mouseRotateDiv = 2.5;
static double mousePanDiv = 10;

/* Private functions */
static void move_camera(double forwards, double strafe, double yaw, double pitch, double x, double y, double z);
static bool handle_move(unsigned char key, int modifiers, int x, int y);

/* Public (GLUT callback) functions */

/* 'normal' keyboard key */
void keyboard(unsigned char key, int x, int y)
{
    int modifiers = glutGetModifiers();

    if (key < 0x20 && modifiers & GLUT_ACTIVE_CTRL) key |= 0x60;
    if (isupper(key)) key = tolower(key);

    if (handle_move(key, modifiers, x, y)) return;

    switch (key)
    {
        case 'P': case 'p': SETTINGS.move = !SETTINGS.move; break;
        case '1': case '&': SETTINGS.light0 = !SETTINGS.light0; break;
        case '2': /*case 'é':*/ SETTINGS.light1 = !SETTINGS.light1; break;
        case '3': case '"': SETTINGS.light2 = !SETTINGS.light2; break;
        case '4': case '\'': SETTINGS.light3 = !SETTINGS.light3; break;

        default: return;
    }
    glutPostRedisplay();
}

/* 'special' keyboard key */
void special(int key, int x, int y)
{
    glutPostRedisplay();
}

/* Mouse buttons */
void mouse(int button, int state, int x, int y)
{
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
            switch (CAMERA.type)
            {
                case CAM_TYPE_ABSOLUTE:
                    //todo
                    break;
                case CAM_TYPE_GAME_AZERTY:
                case CAM_TYPE_GAME_QWERTY:
                    move_camera(button == 3 ? 1 : -1, 0, 0, 0, 0, 0, 0); // zoom
                    break;
            }
    }
}

/* Mouse dragging */
void motion(int x, int y)
{
    double dx = prevMouse.x - x;
    double dy = prevMouse.y - y;
    prevMouse.x = x;
    prevMouse.y = y;

    switch (CAMERA.type)
    {
        case CAM_TYPE_ABSOLUTE:
            //todo
            break;
        case CAM_TYPE_GAME_AZERTY:
        case CAM_TYPE_GAME_QWERTY:
            if (mouseLeftDown) move_camera(0, dx/mouseZoomDiv, 0, 0, 0, dy/mousePanDiv, 0); // Y & strafe
            if (mouseRightDown) move_camera(0, 0, -dx/mouseRotateDiv, dy/mouseRotateDiv, 0, 0, 0); // yaw & pitch
            if (mouseMiddleDown) move_camera(0, 0, 0, 0, dx/mousePanDiv, 0, dy/mousePanDiv); // pan X Z
            break;
    }
}

/**
 * Only use when camera mode is NOT absolute! It would break everything.
 * Does magic to make camera move.
 * All parameters are deltas!
 * @param forwards Move towards crosshair
 * @param strafe Move side-to-side
 * @param yaw Rotate head/camera in the 'NO' direction
 * @param pitch Rotate head/camera in the 'YES' direction
 * @param x Move head/camera absolute X
 * @param y Move head/camera absolute Y
 * @param z Move head/camera absolute Z
 */
static void move_camera(double forwards, double strafe, double yaw, double pitch, double x, double y, double z)
{
    if (forwards != 0)
    {
        double dz = -cos(CAMERA.yaw*M_PI/180.0);
        double dy = sin(CAMERA.pitch*M_PI/180.0);
        double dx = sin(CAMERA.yaw*M_PI/180.0);
        CAMERA.pos.x += dx * forwards;
        CAMERA.pos.y += dy * forwards;
        CAMERA.pos.z += dz * forwards;
    }
    if (strafe != 0)
    {
        double dz = sin(CAMERA.yaw*M_PI/180.0);
        double dx = cos(CAMERA.yaw*M_PI/180.0);
        CAMERA.pos.x += dx * strafe;
        CAMERA.pos.z += dz * strafe;
    }
    if (yaw != 0)
    {
        CAMERA.yaw += yaw;
        if (CAMERA.yaw > 180) CAMERA.yaw -= 360;
        if (CAMERA.yaw < -180) CAMERA.yaw += 360;
    }
    if (pitch != 0)
    {
        CAMERA.pitch += pitch;
        if (CAMERA.pitch > 90) CAMERA.pitch = 90;
        if (CAMERA.pitch < -90) CAMERA.pitch = -90;
    }
    if (x != 0 || y != 0 || z != 0)
    {
        CAMERA.pos.x += x;
        CAMERA.pos.y += y;
        CAMERA.pos.z += z;
    }
    glutPostRedisplay();
}

static bool handle_move(unsigned char key, int modifiers, int x, int y)
{
    double delta = 1;
    if (modifiers & GLUT_ACTIVE_ALT) delta *= 10.0;
    if (modifiers & GLUT_ACTIVE_CTRL) delta *= 0.1;

    switch (CAMERA.type)
    {
        case CAM_TYPE_ABSOLUTE:
            if (modifiers & GLUT_ACTIVE_SHIFT) delta *= -1.0;
            switch (key)
            {
                default: return false;

                case 'x': CAMERA.pos.x += delta; break;
                case 'y': CAMERA.pos.y += delta; break;
                case 'z': CAMERA.pos.z += delta; break;

                case 'u': CAMERA.target.y += delta; break;
                case 'v': CAMERA.target.x += delta; break;
                case 'w': CAMERA.target.z += delta; break;
            }
            glutPostRedisplay();
            break;

        case CAM_TYPE_GAME_AZERTY:
            if (modifiers & GLUT_ACTIVE_SHIFT) delta *= 5.0;
            switch (key)
            {
                default: return false;

                case 's': delta *= -1.0;
                case 'z': move_camera(delta, 0, 0, 0, 0, 0, 0); break;

                case 'q': delta *= -1.0;
                case 'd': move_camera(0, delta, 0, 0, 0, 0, 0); break;

                case 'a': delta *= -1.0;
                case 'e': delta *= 5; move_camera(0, 0, delta, 0, 0, 0, 0); break;

                case 'c': delta *= -1.0;
                case 'f': delta *= 5; move_camera(0, 0, 0, delta, 0, 0, 0); break;

                case 'k': delta *= -1.0;
                case 'i': move_camera(0, 0, 0, 0, delta, 0, 0); break;

                case 'j': delta *= -1.0;
                case 'l': move_camera(0, 0, 0, 0, 0, 0, delta); break;

                case 'n': delta *= -1.0;
                case 'h': move_camera(0, 0, 0, 0, 0, delta, 0); break;
            }
            break;
        case CAM_TYPE_GAME_QWERTY:
            switch (key)
            {
                default: return false;

                case 's': delta *= -1.0;
                case 'w': move_camera(delta, 0, 0, 0, 0, 0, 0); break;

                case 'a': delta *= -1.0;
                case 'd': move_camera(0, delta, 0, 0, 0, 0, 0); break;

                case 'q': delta *= -1.0;
                case 'e': delta *= 5; move_camera(0, 0, delta, 0, 0, 0, 0); break;

                case 'c': delta *= -1.0;
                case 'f': delta *= 5; move_camera(0, 0, 0, delta, 0, 0, 0); break;

                case 'k': delta *= -1.0;
                case 'i': move_camera(0, 0, 0, 0, delta, 0, 0); break;

                case 'j': delta *= -1.0;
                case 'l': move_camera(0, 0, 0, 0, 0, 0, delta); break;

                case 'n': delta *= -1.0;
                case 'h': move_camera(0, 0, 0, 0, 0, delta, 0); break;
            }
            break;
    }

    return true;
}
