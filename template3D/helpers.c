#include "helpers.h"
#include <GL/glut.h>
#include <stdio.h>

/* Sane defaults for globals */

Camera camera = {
        {2.0, 1.0, 1.0}, // eye
        {0.0, 0.0, 0.0}, // center
        {0.0, 1.0, 0.0}  // up
};

Perspective perspective = {
        PERSP_TYPE_FOV,
        {90, 1}, // FOV, aspect ratio
        1, 1000, // near, far
};

Window window = {
        400, 400
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

    glLineStipple(10, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(-size, 0, 0);
    glColor3f(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, -size, 0);
    glColor3f(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, -size);
    glEnd();

    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    glPopMatrix();
}
