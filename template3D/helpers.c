#include "helpers.h"

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

/* Sane defaults for globals */

Camera camera = {
        {2.0, 1.0, 1.0}, // pos
        {0.0, 0.0, 0.0}, // center
        {0.0, 1.0, 0.0}  // up
};

Perspective perspective = {
        PERSP_TYPE_FOV,
        {90, 1}, // FOV, aspect ratio
        0.1, 1000, // near, far
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
            glColor4dv((x + y) % 2 == 0 ? &back : &white);
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
