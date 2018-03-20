#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>

GLdouble xlens = 1.0, ylens = 1.0, zlens = 3.0;
GLdouble xref = 1.0, yref = 1.0, zref = 0.0;
GLdouble xvw = 0.0, yvw = 1.0, zvw = 0.0;
GLdouble xmin = -2.0, xmax = 4.0, ymin = -2.0, ymax = 3.0;
GLdouble near = 1.0, far = 100.0;
GLdouble hoek = 60.0;
GLfloat ttx = 0.25, tty = 0.25, ttz = 0.25;
GLubyte projectie = 'p';

void myinit(void)
{
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}

void toetsen( unsigned char key, int x, int y)
{
	switch ( key )
	{
		case 'x' : xlens++;  break;     case 'X' : xlens--;  break;
		case 'y' : ylens++;  break;     case 'Y' : ylens--;  break;
		case 'z' : zlens++;  break;     case 'Z' : zlens--;  break;
		
		case 'u' : xref++;   break;     case 'U' : xref--;   break;
		case 'v' : yref++;   break;     case 'V' : yref--;   break;
		case 'w' : zref++;   break;     case 'W' : zref--;   break;
		
		case 'j' : xvw = 1.0; yvw = zvw = 0.0;   break;
		case 'k' : yvw = 1.0; xvw = zvw = 0.0;   break;
		case 'l' : zvw = 1.0; yvw = xvw = 0.0;   break;
		case 'i' : xlens = ylens = zlens = 3.0; 
			      xvw  = zvw  = 0.0;  yvw = 1.0;  
			      xref = yref =  zref = 0.0;    break;
		case 'r' : 
			xlens = 1.0; ylens = 1.0; zlens = 3.0;
			xref = 1.0; yref = 1.0; zref = 0.0;
			xvw = 0.0; yvw = 1.0; zvw = 0.0;
			break;
		case 'q' : exit(0); break;
	}
	glutPostRedisplay();
}

void kubus(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xlens, ylens, zlens, xref, yref, zref, xvw, yvw, zvw);
	
	glBegin(GL_QUADS);
		// Achter
		glColor3f(1, 0, 0);
		glVertex3d(-1, -1, -1);
		glVertex3d(1, -1, -1);
		glVertex3d(1, 1, -1);
		glVertex3d(-1, 1, -1);
		
		// Voor
		glColor3f(0, 1, 0);
		glVertex3d(-1, -1, 1);
		glVertex3d(1, -1, 1);
		glVertex3d(1, 1, 1);
		glVertex3d(-1, 1, 1);
		
		// Boven		
		glColor3f(0, 0, 1);
		glVertex3d(-1, 1, 1);
		glVertex3d(1, 1, 1);
		glVertex3d(1, 1, -1);
		glVertex3d(-1, 1, -1);
		
		// Onder
		glColor3f(1, 0, 1);
		glVertex3d(-1, -1, 1);
		glVertex3d(1, -1, 1);
		glVertex3d(1, -1, -1);
		glVertex3d(-1, -1, -1);
		
		// Links
		glColor3f(1, 1, 0);
		glVertex3d(-1, 1, -1);
		glVertex3d(-1, 1, 1);
		glVertex3d(-1, -1, 1);
		glVertex3d(-1, -1, -1);
		
		// Links
		glColor3f(0, 1, 1);
		glVertex3d(1, 1, -1);
		glVertex3d(1, 1, 1);
		glVertex3d(1, -1, 1);
		glVertex3d(1, -1, -1);
	glEnd();
	
	glPopMatrix();
	glFlush();
}
void herschaal(GLint n_w, GLint n_h) 
{
	GLdouble grens;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch ( projectie )
	{
		case 'o':
				glOrtho(xmin, xmax,  ymin, ymax, near, far);
				break;
		case 'f':
				glFrustum(xmin, xmax,  ymin, ymax, 2*near, 8*far);
				break;
		case 'F':
				grens = near*tan(M_PI*(hoek/2.0)/180.0);
				glFrustum(-grens, grens,  -grens, grens, near, far);
				break;
		default:
		case 'p':
				gluPerspective(hoek, 1.0,  near, far);
				break;
	}
	glViewport(0, 0, n_w, n_h);
}
int main( int argc, char * argv[])
{
	char titel[20] = "kubus projectie    ";
	if (argc > 1) projectie = argv[1][0];
	titel[16] = projectie;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowPosition(50, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow(titel);
	myinit();
	glutKeyboardFunc(toetsen);
	glutReshapeFunc(herschaal);
	glutDisplayFunc(kubus);
	glutMainLoop();
}
