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

/**
 * 'normal' keyboard key
 *
 * Keymap:
 *  - 1-4: Toggle lights 1-4
 *  - 5: Toggle wireframe mode
 *  - 6: Toggle bezier drawing mode
 *  - 7: Toggle flat/smooth shading
 *  - 8, 9, 0: Change colors of Arch, Body, Chassis
 *  - SPACE: Start (or pause) the race
 *  - r/R: Shininess +- 5
 *  - t/T: Add/remove cars
 *  - o/O: Transparency
 *  - g/G: Change spot height
 *  - v/V: Change spot exponent
 *  - b/B: Change spot cutoff
 */
void keyboard(unsigned char key, int x, int y)
{
    int modifiers = glutGetModifiers();

    if (key < 0x20 && modifiers & GLUT_ACTIVE_CTRL) key |= 0x60;
    if (isupper(key)) key = tolower(key);

    /* If key was handled via motion functions, return.
     * See function header for which keys can no longer be used. */
    if (handle_move(key, modifiers, x, y)) return;

    switch (key)
    {
        default: return; /* No need to re-draw if unknown key. */
        case 27: exit(0); /* Escape */

        /* Toggle Lights */
        case '1': SETTINGS.light0 = !SETTINGS.light0; break;
        case '2': SETTINGS.light1 = !SETTINGS.light1; break;
        case '3': SETTINGS.light2 = !SETTINGS.light2; break;
        case '4': SETTINGS.light3 = !SETTINGS.light3; break;

        /* Rendering settings */
        case '5': SETTINGS.debug = !SETTINGS.debug; break;
        case '6': SETTINGS.bezierMode = SETTINGS.bezierMode == GL_FILL ? GL_LINE : GL_FILL; break;
        case '7': SETTINGS.flat = !SETTINGS.flat; break;

        /* Color pickers */
        case '8': SETTINGS.colorArch = SETTINGS.colorArch != M_GEEL ? M_GEEL : M_LILA; break;
        case '9': SETTINGS.colorBody = SETTINGS.colorBody != M_GRIJS ? M_GRIJS : M_WITACHTIG; break;
        case '0': SETTINGS.colorChassis = SETTINGS.colorChassis != M_CHROME ? M_CHROME : M_BRONS; break;

        /* Toggle 'race' feature (movement) */
        case ' ': SETTINGS.move = !SETTINGS.move; break;

        /* Shininess */
        case 'r': SETTINGS.shininess += ((modifiers & GLUT_ACTIVE_SHIFT) ? -5 : 5); break;

        /* Add/Remove cars */
        case 't':
        {
            int delta = ((modifiers & GLUT_ACTIVE_SHIFT) ? -1 : 1);
            if (SETTINGS.nCars < 1 && delta < 0) break; /* 1 car min. */
            SETTINGS.nCars += delta;
            SETTINGS.cars = realloc(SETTINGS.cars, SETTINGS.nCars * sizeof(Car));
            for (size_t i = SETTINGS.nCars - delta; i < SETTINGS.nCars; i++)
            {
                SETTINGS.cars[i].draw = draw_soapbox;
                SETTINGS.cars[i].drawing_data = NULL;

                SETTINGS.cars[i].finished = false;
                SETTINGS.cars[i].pos = (float) (rand() % (int)ROAD_LENGHT);
                SETTINGS.cars[i].speed = 0;
                SETTINGS.cars[i].acceleration = 0;

                SETTINGS.cars[i].max_speed = 5;
                SETTINGS.cars[i].max_acceleration = 1;
                SETTINGS.cars[i].power = .1;
            }
            break;
        }

        /* Transparent bodywork */
        case 'o':
            SETTINGS.transp += ((modifiers & GLUT_ACTIVE_SHIFT) ? -0.1 : 0.1);
            if (SETTINGS.transp < 0.1) SETTINGS.transp = 0.1;
            if (SETTINGS.transp > 0.9) SETTINGS.transp = 0.9;
            M_GRIJS[0].w = M_GRIJS[1].w = M_GRIJS[2].w = 1-SETTINGS.transp;
            M_WITACHTIG[0].w = M_WITACHTIG[1].w = M_WITACHTIG[2].w = 1-SETTINGS.transp;
            break;

        /* Change spot settings */
        case 'g': SETTINGS.light3Pos.y += ((modifiers & GLUT_ACTIVE_SHIFT) ? -0.25 : 0.25); break;
        case 'v': SETTINGS.light3Exp += ((modifiers & GLUT_ACTIVE_SHIFT) ? -2.5 : 2.5); break;
        case 'b': SETTINGS.light3Cutoff += ((modifiers & GLUT_ACTIVE_SHIFT) ? -5 : 5); break;
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

/**
 * Keys used by movement:
 * AZERTY:
 * - Move head: ae (rotate) zsqd (move) fc (tilt) -> relative
 * - Move head: ijkl (move) hn (up-down) -> absolute
 * QWERTY:
 * - Move head: eq (rotate) wasd (move) fc (tilt) -> relative
 * - Move head: ijkl (move) hn (up-down) -> absolute
 * ABSOLUTE:
 * - Move head: xyz (absolute)
 * - Move target: uvw (absolute)
 */
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
