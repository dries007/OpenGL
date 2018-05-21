/**
 * Soapbox Main
 * Contains "main", race logic and misc glut code.
 */

#include "globals.h"

/* Get normally distributed double. See function header for attribution. */
static double randn(double mu, double sigma);

/* Private functions for callbacks */
static void idle(void);
static void display(void);
static void reshape(int w, int h);

/* Private globals */
static Model * tesla;

static size_t n_cars;
static Car* cars;

/* Light sources are positional */
static float posLight0[] = {  0.0f, 10.0f, -50.0f, 1.0f};
static float posLight1[] = {-10.0f, 10.0f, -25.0f, 1.0f};
static float posLight2[] = { 10.0f, 10.0f,  25.0f, 1.0f};
static float posLight3[] = {  0.0f, 10.0f,  50.0f, 1.0f};

/* Globals definitions. They are declared in globals.h */
const float ROAD_LENGHT = 100;
const float LANE_WIDTH = 5;

Camera CAMERA = {
        CAM_TYPE_GAME_AZERTY,
        10, 3, 5,
        {0.0, 0.0, 0.0},
        -60, -3
};

Perspective PERSPECTIVE = {
        PERSP_TYPE_FOV,
        0, 0, 0, 0, /* view */
        90, 1, /* FOV, aspect ratio */
        0.1, 100, /* near, far */
};

Window WINDOW = {
        800, 600
};

Settings SETTINGS = {
        false, /* Wireframe */
        false, /* Flat shading */
        true, /* Debug Lights */
        false, /* Light0 */
        true, /* Light1 */
        true, /* Light2 */
        false, /* Light3 */
        {1, 1, 1, 1}, /* Material */
        64, /* Shininess */
        true, /* Debug LookAt */
        true, /* Draw overlay */
        true, /* Move */
};



/* Callback function definitions */
/**
 * Idle callback
 * does motion
 */
static void idle()
{
    if (!SETTINGS.move) return;

    bool allFinished = true;

    for (int i = 0; i < n_cars; i++)
    {
        if (cars[i].finished) continue;

        allFinished = false;
        cars[i].pos += cars[i].speed;
        cars[i].speed += cars[i].acceleration;
        cars[i].acceleration += cars[i].power;

        if (cars[i].pos > ROAD_LENGHT)
        {
            cars[i].pos = ROAD_LENGHT;
            cars[i].finished = true;
        }
        if (cars[i].speed > cars[i].max_speed) cars[i].speed = cars[i].max_speed;
        if (cars[i].acceleration > cars[i].max_acceleration) cars[i].acceleration = cars[i].max_acceleration;
    }

    if (allFinished)
    {
        for (int i = 0; i < n_cars; i++)
        {
            cars[i].finished = false;
            cars[i].pos = 0;
            cars[i].speed = 0;
            cars[i].acceleration = 0;

            cars[i].max_speed = fabs(randn(0.1, 0.5));
            cars[i].max_acceleration = fabs(randn(0.01, 0.05));
            cars[i].power = fabs(randn(0.001, 0.005));
        }
    }

    glutPostRedisplay();
}

/**
 * Draw callback
 */
static void display()
{
    /* Nuke everything */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Start with 3D camera */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Reset sane defaults */
    glColor3d(0, 0, 0);
    glShadeModel(SETTINGS.flat ? GL_FLAT : GL_SMOOTH);

    /* Setup 3D camera */
    switch (CAMERA.type)
    {
        case CAM_TYPE_ABSOLUTE:
            gluLookAt(CAMERA.pos.x, CAMERA.pos.y, CAMERA.pos.z, CAMERA.target.x, CAMERA.target.y, CAMERA.target.z, 0, 1, 0);
            break;

        case CAM_TYPE_GAME_AZERTY:
        case CAM_TYPE_GAME_QWERTY:
            glRotated(-CAMERA.pitch, 1, 0, 0);
            glRotated(CAMERA.yaw, 0, 1, 0);
            glTranslated(-CAMERA.pos.x, -CAMERA.pos.y, -CAMERA.pos.z);
            break;
    }

    /* Setup lights */
    glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*) &posLight0);
    glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat*) &posLight1);
    glLightfv(GL_LIGHT2, GL_POSITION, (GLfloat*) &posLight2);
    glLightfv(GL_LIGHT3, GL_POSITION, (GLfloat*) &posLight3);

    glDisable(GL_LIGHTING); /* Disabled for drawing debug symbols */

    draw_axis(50);
//    draw_checkers(1, 10);

    if (SETTINGS.debug_lookat && CAMERA.type == CAM_TYPE_ABSOLUTE)
    {
        glPushMatrix();
        glTranslated(CAMERA.target.x, CAMERA.target.y, CAMERA.target.z);
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(3);
        draw_axis(1);
        glPopAttrib(); /* GL_LINE_BIT */
        glPopMatrix();
    }

    if (SETTINGS.debug_lights)
    {
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(3);
        if (SETTINGS.light0) draw_axis_at(posLight0, 0.5);
        if (SETTINGS.light1) draw_axis_at(posLight1, 0.5);
        if (SETTINGS.light2) draw_axis_at(posLight2, 0.5);
        if (SETTINGS.light3) draw_axis_at(posLight3, 0.5);
        glPopAttrib(); /* GL_LINE_BIT */
    }

    /* Must be called first */
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    if (SETTINGS.light0) glEnable(GL_LIGHT0); else glDisable(GL_LIGHT0);
    if (SETTINGS.light1) glEnable(GL_LIGHT1); else glDisable(GL_LIGHT1);
    if (SETTINGS.light2) glEnable(GL_LIGHT2); else glDisable(GL_LIGHT2);
    if (SETTINGS.light3) glEnable(GL_LIGHT3); else glDisable(GL_LIGHT3);

    draw_lamp(posLight0, SETTINGS.light0);
    draw_lamp(posLight1, SETTINGS.light1);
    draw_lamp(posLight2, SETTINGS.light2);
    draw_lamp(posLight3, SETTINGS.light3);

    glPushMatrix();

    glTranslated(LANE_WIDTH / 2.0 + (n_cars * LANE_WIDTH) / -2.0, 0, -ROAD_LENGHT / 2.0);

    draw_road(n_cars);

    for (int i = 0; i < n_cars; i++)
    {
        glPushMatrix();
        glTranslatef(i * LANE_WIDTH, 0, cars[i].pos);
        cars[i].draw(&cars[i]);
        glPopMatrix();
    }

    glPopMatrix();

    /* Draw overlay last, so it's always 'on top' of the scene. */
    if (SETTINGS.overlay) draw_overlay();

    glutSwapBuffers();
}

/**
 * Window reshape callback
 */
static void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    WINDOW.width = w;
    WINDOW.height = h;

    switch (PERSPECTIVE.type)
    {
        case PERSP_TYPE_ORTHO:
            glOrtho(PERSPECTIVE.left, PERSPECTIVE.right, PERSPECTIVE.bottom, PERSPECTIVE.top, PERSPECTIVE.near, PERSPECTIVE.far);
            break;

        case PERSP_TYPE_FRUSTUM:
            glOrtho(PERSPECTIVE.left, PERSPECTIVE.right, PERSPECTIVE.bottom, PERSPECTIVE.top, PERSPECTIVE.near, PERSPECTIVE.far);
            break;

        case PERSP_TYPE_FOV:
            PERSPECTIVE.aspect = (double) WINDOW.width / WINDOW.height;
            gluPerspective(PERSPECTIVE.fov, PERSPECTIVE.aspect, PERSPECTIVE.near, PERSPECTIVE.far);
            break;
    }

    glViewport(0, 0, w, h);
}

/**
 * Entry point
 */
int main(int argc, char** argv)
{
    // todo : parse args for default settings?

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(WINDOW.width, WINDOW.height);
    glutCreateWindow("Zeepkist - Dries Kennes");

    tesla = stlLoadFile("Tesla.stl");
    if (tesla == NULL) fputs("Tesla model was not loaded :(", stderr);

    n_cars = 3;
    cars = calloc(n_cars, sizeof(Car));
    for (int i = 0; i < n_cars; i++)
    {
        cars[i].draw = draw_soapbox;
        cars[i].drawing_data = NULL;
        cars[i].pos = 0;
        cars[i].speed = 0;
        cars[i].acceleration = 0;
        cars[i].power = .01;
        cars[i].max_acceleration = .1;
        cars[i].max_speed = 1;
    }

    if (tesla != NULL)
    {
        cars[0].draw = draw_model;
        cars[0].drawing_data = tesla;
    }

    glClearColor(0.1, 0.1, 0.1, 0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float ambient[] = {0.25, 0.25, 0.25, 1};
    const float white[] = {1.0, 1.0, 1.0, 1.0};
    const float black[] = {0.0, 0.0, 0.0, 1.0};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
//    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, black);
    glLightfv(GL_LIGHT1, GL_SPECULAR, black);
    glLightfv(GL_LIGHT2, GL_SPECULAR, black);
    glLightfv(GL_LIGHT3, GL_SPECULAR, black);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.5);
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 0.5);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.01);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.01);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.001);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.001);
    glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.001);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    /* From input.h */
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSetCursor(GLUT_CURSOR_INFO);

    glutMainLoop();
    /* No return, since glutMainLoop never returns. */
}

/**
 * Generating random numbers from Normal distribution in C
 * Source: https://phoxis.org/2013/05/04/generating-random-numbers-from-normal-distribution-in-c/
 */
double randn(double mu, double sigma)
{
    double U1, U2, W, mult;
    static double X1, X2;
    static int call = 0;

    if (call == 1)
    {
        call = !call;
        return (mu + sigma * (double) X2);
    }

    do
    {
        U1 = -1 + ((double) rand () / RAND_MAX) * 2;
        U2 = -1 + ((double) rand () / RAND_MAX) * 2;
        W = pow (U1, 2) + pow (U2, 2);
    }
    while (W >= 1 || W == 0);

    mult = sqrt ((-2 * log (W)) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;

    call = !call;

    return (mu + sigma * (double) X1);
}
