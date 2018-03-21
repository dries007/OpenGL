#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

/*
 * Todo: Allow change in camera focus
 * Todo: Add WASD/ZQSD controls for moving camera ('game mode'?)
 * Todo: Allow tilting of the 'up' direction (?)
 * Todo: Add animated model
 * Todo: Mouse stuff (?)
 * Todo: Add scroll for zoom (?)
 */

const char *TITLE = "Template 3D Project";

struct vect3d {
    double x, y, z;
};

struct {
    struct vect3d eye;
    struct vect3d center;
    struct vect3d up;
} camera = {
        {2.0, 1.0, 1.0},
        {0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0}
};

enum persp_type {PERSP_TYPE_ORTHO, PERSP_TYPE_FRUSTUM, PERSP_TYPE_FOV};
struct {
    enum persp_type type;
    union {
        struct { double left, right, bottom, top; };
        struct { double fov, aspect; };
    };
    double near, far;
} perspective = {
        PERSP_TYPE_ORTHO,
        {-10, 10, -10, 10},
        1, 1000,
};

struct {
    int width, height;
} window = {
        400, 400
};

void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void reshape(int w, int h);
void display(void);

void keyboard(unsigned char key, int x, int y)
{
    fprintf(stderr, "Keypress %d (%c), mouse %d;%d\n", key, key, x, y);

    int modifiers = glutGetModifiers();
    int delta = 1;
    if (modifiers & GLUT_ACTIVE_ALT) delta *= 10;

    switch (key)
    {
        case 27: exit(0);
        default: return;

        case 'Y':
            delta *= -1;
        case 'y':
            camera.eye.y += delta;
            fprintf(stderr, "Camera eye Y%+d = %lf\n", delta, camera.eye.y);
            break;

        case 'X':
            delta *= -1;
        case 'x':
            camera.eye.x += delta;
            fprintf(stderr, "Camera eye X%+d = %lf\n", delta, camera.eye.x);
            break;

        case 'Z':
            delta *= -1;
        case 'z':
            camera.eye.x += delta;
            fprintf(stderr, "Camera eye Z%+d = %lf\n", delta, camera.eye.x);
            break;

        case 'p': case 'P':
            switch (perspective.type)
            {
                case PERSP_TYPE_ORTHO:
                    fprintf(stderr, "Switching to Frustum projection.\n");
                    perspective.type = PERSP_TYPE_FRUSTUM;
                    break;
                case PERSP_TYPE_FRUSTUM:
                    fprintf(stderr, "Switching to FOV projection.\n");

                    perspective.type = PERSP_TYPE_FOV;
                    perspective.fov = 90;
                    perspective.aspect = (double) window.width / window.height;
                    break;
                case PERSP_TYPE_FOV:
                    fprintf(stderr, "Switching to Ortho projection.\n");
                    perspective.type = PERSP_TYPE_ORTHO;
                    perspective.left = -10;
                    perspective.right = 10;
                    perspective.bottom = -10;
                    perspective.top = 10;
                    break;
            }
            reshape(window.width, window.height);
            break;
    }

    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    fprintf(stderr, "Special keypress %d, mouse %d;%d\n", key, x, y);

    switch (key)
    {
        default: return;
    }

    glutPostRedisplay();
}

void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    window.width = w;
    window.height = h;

    switch (perspective.type)
    {
        case PERSP_TYPE_ORTHO:
            fprintf(stderr, "Ortho projection (%lf;%lf;%lf) -> (%lf;%lf;%lf).\n",
                    perspective.left, perspective.bottom, perspective.near,
                    perspective.right, perspective.top, perspective.far);
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top,
                    perspective.near, perspective.far);
            break;
        case PERSP_TYPE_FRUSTUM:
            fprintf(stderr, "Frustum projection (%lf;%lf;%lf) -> (%lf;%lf;%lf).\n",
                    perspective.left, perspective.bottom, perspective.near,
                    perspective.right, perspective.top, perspective.far);
            glOrtho(perspective.left, perspective.right, perspective.bottom, perspective.top,
                    perspective.near, perspective.far);
            break;
        case PERSP_TYPE_FOV:
            perspective.aspect = (double) w / h;
            fprintf(stderr, "FOV projection %lf° asp: %lf n: %lf f: %lf\n",
                    perspective.fov, perspective.aspect, perspective.near, perspective.far);
            gluPerspective(perspective.fov, perspective.aspect,
                           perspective.near, perspective.far);
            break;
    }

    glViewport(0, 0, w, h);
}

void drawAxis()
{
    glPushMatrix();
    glPushAttrib(GL_LINE_STIPPLE);
    glPushAttrib(GL_LINE_STIPPLE_PATTERN);
    glPushAttrib(GL_LINE_STIPPLE_REPEAT);

    glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(100, 0, 0);
        glColor3f(0, 1, 0); glVertex3d(0, 0, 0);glVertex3d(0, 100, 0);
        glColor3f(0, 0, 1); glVertex3d(0, 0, 0);glVertex3d(0, 0, 100);
    glEnd();

    glLineStipple(1, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(-100, 0, 0);
    glColor3f(0, 1, 0); glVertex3d(0, 0, 0);glVertex3d(0, -100, 0);
    glColor3f(0, 0, 1); glVertex3d(0, 0, 0);glVertex3d(0, 0, -100);
    glEnd();

    glPopAttrib();
    glPopAttrib();
    glPopAttrib();
    glPopMatrix();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z,
              camera.center.x, camera.center.y, camera.center.z,
              camera.up.x, camera.up.y, camera.up.z);

    drawAxis();

    glColor3f(0, 0, 0);
    glutSolidCube(1);
    glColor3f(1, 1, 1);
    glutWireCube(1.01);

    glFlush();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow(TITLE);

    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutMainLoop();
}