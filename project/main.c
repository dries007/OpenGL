#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <GL/glut.h>

#include "helpers.h"
#include "user_input.h"
#include "text.h"

const float white[] = {1.0, 1.0, 1.0, 1.0};
const float black[] = {0.0, 0.0, 0.0, 1.0};
float posLight0[] = {10.0, 5.0, 5.0, 1.0};
float posLight1[] = {0.0, 0.0, 0.0, 1.0};

double distance = 0;

void idle(void);

void idle()
{
    if (!move) return;
    distance += 0.05;
    if (distance > 10) distance = -10;
    glutPostRedisplay();
}

void cuboid(double x, double y, double z)
{
    glPushMatrix();
    glScaled(x, y, z);
    if (wire) glutWireCube(1);
    else glutSolidCube(1);
    glPopMatrix();
}

void cylinder(double radius_bottom, double radius_top, double height, double radius_hole)
{
    const int roundness = 16;
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();

    glTranslated(0, 0, -height/2);

    gluQuadricDrawStyle(quad, wire ? GLU_LINE : GLU_FILL);

    gluDisk(quad, radius_hole, radius_bottom, (int) (roundness * radius_bottom), 1); // Bottom
    gluCylinder(quad, radius_bottom, radius_top, height, (int) (roundness * (radius_bottom > radius_top ? radius_bottom : radius_top)), 1); // Mantle
    if (radius_hole != 0)
        gluCylinder(quad, radius_hole, radius_hole, height, (int) (roundness * radius_hole), 1); // Hole Mantle
    glTranslated(0, 0, height);
    gluDisk(quad, radius_hole, radius_top, (int) (roundness * radius_top), 1); // Top
    glTranslated(0, 0, -height);

    glPopMatrix();
    gluDeleteQuadric(quad);
}

void wheel()
{
    const double radius = 1;

    glPushMatrix();
    glRotated(90, 0, 1, 0);
    glRotated((distance * 180.0)/(radius * M_PI), 0, 0, 1);

    cylinder(1, 1, 0.2, 0.8); // Wheel
    cylinder(0.1, 0.1, 0.4, 0); // Axel

    for (int a = 0; a < 360; a += (360/7)) // Spokes
    {
        glPushMatrix();
        glRotated(a, 0, 0, 1);
        glTranslated(0, 0.4, 0);
        cuboid(0.1, 0.8, 0.1);
        glPopMatrix();
    }

    glPopMatrix();
}

void zeepkist()
{
    GLUquadric* quad = gluNewQuadric();

    glPushMatrix();

    glTranslated(0, 1, 0);
    glTranslated(0, 0, distance);

    cuboid(2, 0.1, 2);

    glPushMatrix();
    glTranslated(1.2, 0, 0);
    wheel();
    glPopMatrix();
    glPushMatrix();
    glTranslated(-1.2, 0, 0);
    wheel();
    glPopMatrix();

    glTranslated(0, 0, 2);
    cuboid(1, 0.1, 2);
    glTranslated(0, 0, 1.7);
    glPushMatrix();
    glTranslated(0.3, 0, 0);
    cuboid(0.2, 0.1, 1.4);
    glPopMatrix();
    glPushMatrix();
    glTranslated(-0.3, 0, 0);
    cuboid(0.2, 0.1, 1.4);
    glPopMatrix();
    glTranslated(0, 0, 0.5);
    wheel();

    glPopMatrix();
    gluDeleteQuadric(quad);
}

void display(void)
{
//    fprintf(stderr, "Display\n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* Nuke everything */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3d(0, 0, 0);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(flat ? GL_FLAT : GL_SMOOTH);



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

    glLightfv(GL_LIGHT0, GL_POSITION, (float*) &posLight0);
    glLightfv(GL_LIGHT1, GL_POSITION, (float*) &posLight1);

    drawAxis(50);
    drawCheckersZ(1, 10);

    //Debug symbols for lights
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(3);
    glPushMatrix();
    glTranslated(posLight0[0], posLight0[1], posLight0[2]);
    drawAxis(0.5);
    glPopMatrix();
    glPushMatrix();
    glTranslated(posLight1[0], posLight1[1], posLight1[2]);
    drawAxis(0.5);
    glPopMatrix();
    glPopAttrib(); /* GL_LINE_BIT */

    glPushAttrib(GL_LIGHTING_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    float material[] = {0.7, 0.2, 0.2, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0);

    zeepkist();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
    glPopAttrib();

    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

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

    if (overlay) drawOverlay(); /* Draw overlay last, so it's always 'on top' of the scene. */

    glutSwapBuffers();
    /*glFlush();*/
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    //glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow(TITLE);

//    glClearColor(.5, .5, .5, 0);
    glClearColor(0.1, 0.1, 0.1, 0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT1, GL_AMBIENT, black);
    glLightfv(GL_LIGHT2, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, black);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, black);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, black);
    glLightfv(GL_LIGHT0, GL_SPECULAR, black);
    glLightfv(GL_LIGHT1, GL_SPECULAR, black);
    glLightfv(GL_LIGHT2, GL_SPECULAR, black);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_INFO);

    puts("OpenGL/GLU/GLUT Project. 2018 - Dries007\n");
    puts(KEYMAP);
    puts(KEYMAP_ABSOLUTE);
    puts(MOUSE_ABSOLUTE);
    puts(KEYMAP_AZERTY);
    puts(KEYMAP_QWERTY);
    puts(MOUSE_GAME);

    glutMainLoop();
}
