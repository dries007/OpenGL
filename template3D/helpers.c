#include "helpers.h"

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

/* Sane defaults for globals */

Camera camera = {
        /*
        CAM_TYPE_LOOK_AT,
        (Vect3d){2.0, 1.0, 1.0},
        (Vect3d){0.0, 0.0, 0.0},
        (Vect3d){0.0, 1.0, 0.0}
         */
        CAM_TYPE_GAME_AZERTY,
        (Vect3d){10, 1, 0},
        { -90, 0 }
};

Perspective perspective = {
        PERSP_TYPE_FOV,
        {90, 1}, /* FOV, aspect ratio */
        0.1, 1000, /* near, far */
};

Window window = {
        600, 600
};

/* Helper functions */

void error(int status, const char *message)
{
    fputs(message, stderr);
    exit(status);
}

void drawAxis(double size)
{
    glPushMatrix();
    glPushAttrib(GL_LINE_STIPPLE);
    glPushAttrib(GL_LINE_STIPPLE_PATTERN);
    glPushAttrib(GL_LINE_STIPPLE_REPEAT);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(size, 0, 0);
    glColor3f(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, size, 0);
    glColor3f(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, size);
    glEnd();

    glLineStipple((int) (size / 100), 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(-size, 0, 0);
    glColor3f(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, -size, 0);
    glColor3f(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, -size);
    glEnd();

    glPopAttrib();/* GL_LINE_STIPPLE_REPEAT */
    glPopAttrib();/* GL_LINE_STIPPLE_PATTERN */
    glPopAttrib();/* GL_LINE_STIPPLE */
    glPopMatrix();
}

void drawCheckersZ(double size, int count)
{
    glPushMatrix();
    glRotated(90, 1, 0, 0);
    glScaled(size, size, 1);

    Vect4d back = {0, 0, 0, 0.1};
    Vect4d white = {1, 1, 1, 0.1};

    for (int x = -count; x < count; x++)
    {
        for (int y = -count; y < count; y++)
        {
            glColor4dv((double*)((x + y) % 2 == 0 ? &back : &white));
            glRectd(x, y, x+1, y+1);
        }
    }

    glPopMatrix();
}

Vect3d diff3d(Vect3d a, Vect3d b)
{
    return (Vect3d) {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vect3d add3d(Vect3d a, Vect3d b)
{
    return (Vect3d) {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vect3d norm3d(Vect3d a)
{
    double d = dist(a);
    return (Vect3d) {a.x / d, a.y / d, a.z / d};
}

double dist(Vect3d a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vect3d crossProduct3d(Vect3d a, Vect3d b)
{
    return (Vect3d) {
            a.y * b.z - a.z * b.y,
            a.x * b.z - a.z * b.x,
            a.x * b.y - a.y * b.x
    };
}

Vect3d mult3ds(Vect3d a, double s)
{
    return (Vect3d) {a.x * s, a.y * s, a.z * s};
}

void moveCamera(double forwards, double strafe, double yaw, double pitch, double x, double y, double z)
{
    if (forwards != 0)
    {
        double dz = -cos(camera.yaw*M_PI/180.0);
        double dy = sin(camera.pitch*M_PI/180.0);
        double dx = sin(camera.yaw*M_PI/180.0);
        camera.pos.x += dx * forwards;
        camera.pos.y += dy * forwards;
        camera.pos.z += dz * forwards;
    }
    if (strafe != 0)
    {
        double dz = sin(camera.yaw*M_PI/180.0);
        /* double dy = 0; */
        double dx = cos(camera.yaw*M_PI/180.0);
        camera.pos.x += dx * strafe;
        /* camera.pos.y += dy * delta; */
        camera.pos.z += dz * strafe;
    }
    if (yaw != 0)
    {
        camera.yaw += yaw;
        if (camera.yaw > 180) camera.yaw -= 360;
        if (camera.yaw < -180) camera.yaw += 360;
    }
    if (pitch != 0)
    {
        camera.pitch += pitch;
        if (camera.pitch > 90) camera.pitch = 90;
        if (camera.pitch < -90) camera.pitch = -90;
    }
    if (x != 0 || y != 0 || z != 0)
    {
        camera.pos.x += x;
        camera.pos.y += y;
        camera.pos.z += z;
    }
}

bool handleMove(unsigned char key, int modifiers, int x, int y)
{
    double delta = 1;
    if (modifiers & GLUT_ACTIVE_SHIFT) delta *= 5.0;
    if (modifiers & GLUT_ACTIVE_ALT) delta *= 10.0;
    if (modifiers & GLUT_ACTIVE_CTRL) delta *= 0.1;

    switch (camera.type)
    {
        case CAM_TYPE_LOOK_AT:
            switch (key)
            {
                default: return false;

                case 'x':camera.pos.x += delta; break;
                case 'y': camera.pos.y += delta; break;
                case 'z': camera.pos.z += delta; break;

                case 'u': camera.target.y += delta; break;
                case 'v': camera.target.x += delta; break;
                case 'w': camera.target.z += delta; break;
            }
            break;

        case CAM_TYPE_GAME_AZERTY:
            switch (key)
            {
                default: return false;

                case 's': delta *= -1.0;
                case 'z': moveCamera(delta, 0, 0, 0, 0, 0, 0); break;

                case 'q': delta *= -1.0;
                case 'd': moveCamera(0, delta, 0, 0, 0, 0, 0); break;

                case 'a': delta *= -1.0;
                case 'e': delta *= 5; moveCamera(0, 0, delta, 0, 0, 0, 0); break;

                case 'c': delta *= -1.0;
                case 'f': delta *= 5; moveCamera(0, 0, 0, delta, 0, 0, 0); break;

                case 'k': delta *= -1.0;
                case 'i': moveCamera(0, 0, 0, 0, delta, 0, 0); break;

                case 'j': delta *= -1.0;
                case 'l': moveCamera(0, 0, 0, 0, 0, 0, delta); break;

                case 'n': delta *= -1.0;
                case 'h': moveCamera(0, 0, 0, 0, 0, delta, 0); break;
            }
            break;
        case CAM_TYPE_GAME_QWERTY:
            switch (key)
            {
                default: return false;

                case 's': delta *= -1.0;
                case 'w': moveCamera(delta, 0, 0, 0, 0, 0, 0); break;

                case 'a': delta *= -1.0;
                case 'd': moveCamera(0, delta, 0, 0, 0, 0, 0); break;

                case 'q': delta *= -1.0;
                case 'e': delta *= 5; moveCamera(0, 0, delta, 0, 0, 0, 0); break;

                case 'c': delta *= -1.0;
                case 'f': delta *= 5; moveCamera(0, 0, 0, delta, 0, 0, 0); break;

                case 'k': delta *= -1.0;
                case 'i': moveCamera(0, 0, 0, 0, delta, 0, 0); break;

                case 'j': delta *= -1.0;
                case 'l': moveCamera(0, 0, 0, 0, 0, 0, delta); break;

                case 'n': delta *= -1.0;
                case 'h': moveCamera(0, 0, 0, 0, 0, delta, 0); break;
            }
            break;
    }

    glutPostRedisplay();
    return true;
}
