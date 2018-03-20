/*
	De propere versie
*/
#include <stdio.h>
#include <GL/glut.h>
#include <stdbool.h>

#include "data.h"
#include "helpers.h"

/* Camera stuff */
GLdouble xlens, ylens, zlens;
GLdouble xref = 0.0, yref = 0.0, zref = 0.0;
GLdouble xvw = 0.0, yvw = 1.0, zvw = 0.0;

/* Required globals for coordinate calculation */
GLdouble xmin, xmax, ymin, ymax;
GLdouble near = -1.0, far = 100000.0;
GLdouble verhouding;

/* Data globals */
int start_week, end_week, categories, weeks, max_sum;
Gebruik data[AANT];
int sums[WLEN];
Colorf *colorsBars;

bool solid = true;

void error(int status, const char *message)
{
    fputs(message, stderr);
    exit(status);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);

    gluLookAt(xlens, ylens, zlens, xref, yref, zref, xvw, yvw, zvw);
    
    //glColor3f(0, 0, 0);
    //glutWireCube(1);
    //glutSolidCube(1);
    
    //glScalef(10.0/weeks, 10.0/max_sum, 1.0);
    
    for (int i = start_week; i <= end_week; i++)
    {
        glColor3fv((GLfloat *) &colorsBars[i - start_week]);
	
	glPushMatrix();
	glScalef(1.0, sums[i], 1.0);
	glTranslatef(i + 0.5, 0.5, 0);
	glScalef(0.9, 1.0, 1.0);
	
	if (!solid) glutWireCube(1);
	else glutSolidCube(1);
	
        glPopMatrix();
    }

    glFlush();
}

void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xmin, xmax, ymin, ymax, near, far);
    glViewport(0, 0, w, h);
    /*if (w <= h * verhouding)
        glViewport(0, 0, w, w/verhouding);
    else
        glViewport(0, 0, verhouding*h, h);*/
}

void keyboard(unsigned char code, int mouseX, int mouseY)
{
    switch (code)
    {
        case 27: case 'q': case 'Q': exit(0); /* escape */
            
        case 's': case 'S': solid = !solid; break;

        case 'x': xlens+=(weeks/10.0);break;case 'X': xlens-=(weeks/10.0);break;
	case 'y': ylens+=(max_sum/10.0);break; case 'Y': ylens-=(max_sum/10.0);break;
	case 'z': zlens+=(max_sum/10.0);break; case 'Z': zlens-=(max_sum/10.0);break;
	
	case 'u' : xref++;   break;     case 'U' : xref--;   break;
	case 'v' : yref++;   break;     case 'V' : yref--;   break;
	case 'w' : zref++;   break;     case 'W' : zref--;   break;

        default:
            return;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    categories = read_gebruik_dat(data, AANT, &start_week, &end_week);
    if (categories <= 0) error(1, "Input data error. (categories <= 0)\n");

    weeks = end_week - start_week + 1;
    if (weeks <= 0) error(1, "Input data error. (weeks <= 0)\n");
    
    colorsBars = calloc(weeks, sizeof(Colorf));
    if (colorsBars == NULL) error(1, "Impossibly low on memory.\n");
    color_wheel(colorsBars, weeks, 1, 1, 0);
    
    max_sum = sum_uren(data, categories, sums);
	
	xmin = start_week - (weeks * 0.05);
	xmax = end_week + 1 + (weeks * 0.05);
	ymin = -(max_sum * 0.05);
	ymax = max_sum + (max_sum * 0.05);
/*
	xref = start_week + weeks / 2.0;
	yref = max_sum / 2.0;
	zref = 0.0;
*/
	xlens = start_week + weeks / 2.0;
	ylens = max_sum / 2.0;
	zlens = max_sum;
	
	verhouding = xmax / ymax;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Excel '75");

    glClearColor(0.66, 0.66, 0.66, 0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();

    return 0;
}
