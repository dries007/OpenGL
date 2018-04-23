#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <GL/glut.h>

#include "helpers.h"
#include "stl.h"
#include "text.h"

extern bool overlay;
static Model* model = NULL;

typedef struct planet {
	const char * name;
	double x, y, r;
	double incl, phase, period;
	unsigned char color[3];
} Planet;

unsigned long idleTime = 0;

Planet planets[] = {
	{"Sun", 	0.00, 	0.00, 	1.00, 	0.00,	0*M_PI_4, 0.00, 	{244, 226, 66}},
	{"Mercury", 	4.06, 	3.06, 	0.30, 	3.38,	0*M_PI_4, 87.97,	{104, 29, 0}},
	{"Venus", 	5.68, 	5.00, 	0.50, 	3.86,	1*M_PI_4, 224.70,	{198, 149, 25}},
	{"Home", 	7.01, 	7.71, 	0.55, 	7.155,	2*M_PI_4, 365.26,	{66, 131, 244}},
	{"Mars", 	8.42, 	9.42, 	0.40, 	5.65,	3*M_PI_4, 686.98,	{198, 62, 7}},
	{"Jupiter", 	10.66,	10.00,	1.05, 	6.09,	4*M_PI_4, 4332.82,	{173, 178, 137}},
	{"Saturn", 	12.26,	11.26,	1.03, 	5.51,	5*M_PI_4, 10755.70,	{126, 183, 115}},
	{"Uranus",	13.96,	14.46,	0.85, 	6.48,	6*M_PI_4, 30687.15,	{168, 204, 255}},
	{"Neptune",	15.41,	16.41,	0.825, 	6.43, 	7*M_PI_4, 60190.03,	{4, 60, 140}},
	{ 0 }
};

void display(void);
void idle(void);
void drawPlanet(Planet p);

void drawPlanet(Planet p)
{
	glPushMatrix();
	glColor3ubv(p.color);
	glRotated(p.incl, 1, 0, 0);
	if (p.period != 0) glTranslated(p.x * cos(p.phase + (2 * M_PI * idleTime)/p.period), 0, p.y * sin(p.phase + (2 * M_PI * idleTime)/p.period));
	glutSolidSphere(p.r, 16, 8);
	glPopMatrix();
}

void idle()
{
	idleTime += 10;
	
	glutPostRedisplay();
}

void display()
{
//    fprintf(stderr, "Display\n");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* Nuke everything */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
  
	glEnable(GL_DEPTH_TEST);  
/*
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/
  //  drawAxis(100);
  //  drawCheckersZ(1, 10);
    
    Planet * p = planets;
    while (p->name != NULL)
    {
    	drawPlanet(*p++);
    }

    glPushMatrix();
    glColor3f(0.6, 0, 0);
    glTranslated(0, 5, 0);
    glRotated(-90, 1, 0, 0);
    glScaled(0.1, 0.1, 0.1);
    stlDisplayModel(model);
    glPopMatrix();

    drawOverlay();

    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH /*| GLUT_ALPHA*/);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow(TITLE);

    glClearColor(.7, .7, .7, 0);
    glClearDepth(1.0);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_INFO);

    puts("Planetarium 2018 - Dries007\n");
    /*puts(KEYMAP);
    puts(KEYMAP_ABSOLUTE);
    puts(MOUSE_ABSOLUTE);
    puts(KEYMAP_AZERTY);
    puts(KEYMAP_QWERTY);
    puts(MOUSE_GAME);*/

    model = stlLoadFile("tesla.stl");

    if (model == NULL) puts("Failed to load model");
    else puts("Model loaded!");
    
    glutMainLoop();
}
