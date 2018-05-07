/**
 * Soapbox Main
 * Contains "main", race logic and misc glut code.
 */

#include "globals.h"

void idle(void);
void display(void);
void reshape(int w, int h);

Camera camera = {
        CAM_TYPE_GAME_AZERTY,
        10, 3, 5,
        {0.0, 0.0, 0.0},
        -60, -3
};

Perspective perspective = {
        PERSP_TYPE_FOV,
        0, 0, 0, 0, /* view */
        90, 1, /* FOV, aspect ratio */
        0.1, 100, /* near, far */
};

Window window = {
        800, 600
};

void idle()
{

}

void display()
{
    glutSwapBuffers();
}

void reshape(int w, int h)
{

}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(window.width, window.height);
    glutCreateWindow("Zeepkist");

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

    glutMainLoop();
}
