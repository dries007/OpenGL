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
static uint banners;

/* Globals definitions. They are declared in globals.h */
const float ROAD_LENGHT = 100;
const float LANE_WIDTH = 5;

Camera CAMERA = {
        CAM_TYPE_GAME_AZERTY,
        25, 25, 55,
        0.0, 0.0, 0.0,
        -25, -20
};

Perspective PERSPECTIVE = {
        PERSP_TYPE_FOV,
        0, 0, 0, 0, /* view */
        90, 1, /* FOV, aspect ratio */
        0.1, 250, /* near, far */
};

Window WINDOW = {
        1000, 600
};

/*                       Ambient               Diffuse               Specular       */
Vect4f M_GRIJS[3] =     {{.22, .22, .22, 0.7}, {.33, .33, .33, 0.7}, {.11, .11, .11, 0.7}};
Vect4f M_WITACHTIG[3] = {{.66, .66, .66, 0.7}, {.77, .77, .77, 0.7}, {.55, .55, .55, 0.7}};
Vect4f M_CHROME[3] =    {{.46, .58, .35, 1.0}, {.23, .29, .17, 1.0}, {.69, .87, .52, 1.0}};
Vect4f M_BRONS[3] =     {{.21, .13, .10, 1.0}, {.39, .27, .17, 1.0}, {.71, .43, .18, 1.0}};
Vect4f M_GEEL[3] =      {{.65, .55, .15, 1.0}, {.75, .45, .15, 1.0}, {.85, .35, .15, 1.0}};
Vect4f M_LILA[3] =      {{.45, .15, .75, 1.0}, {.55, .15, .65, 1.0}, {.35, .15, .85, 1.0}};

Settings SETTINGS = {
        false, /* Debug mode (wireframe etc) */
        true, /* Light 0 */
        true, /* Light 2 */
        true, /* Light 3 */
        true, /* Light 1 */
        {  0.0f, 10.0f, -50.0f, 1.0f}, /* Light 0 */
        {-10.0f, 10.0f, -25.0f, 1.0f}, /* Light 1 */
        { 10.0f, 10.0f,  25.0f, 1.0f}, /* Light 1 */
        {-10.0f, 10.0f,  25.0f, 1.0f}, /* Light 3 */
        false, /* Flat shading */
        10, /* Shininess */
        true, /* Draw overlay */
        false, /* Move */
        GL_FILL, /* Bezier mode */
        M_GEEL, /* Color Arch */
        M_CHROME, /* Color Chassis */
        M_GRIJS, /* Color Body */
        0.3, /* Transparency */
        0, /* nr of Cars */
        NULL, /* Cars data */
        20, /* Spot (light 3) Exp */
        30, /* Spot (light 3) Cutoff */
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

    for (int i = 0; i < SETTINGS.nCars; i++)
    {
        if (SETTINGS.cars[i].finished) continue;

        allFinished = false;
        SETTINGS.cars[i].pos += SETTINGS.cars[i].speed;
        SETTINGS.cars[i].speed += SETTINGS.cars[i].acceleration;
        SETTINGS.cars[i].acceleration += SETTINGS.cars[i].power;

        if (SETTINGS.cars[i].pos > ROAD_LENGHT)
        {
            SETTINGS.cars[i].pos = ROAD_LENGHT;
            SETTINGS.cars[i].finished = true;
        }
        if (SETTINGS.cars[i].speed > SETTINGS.cars[i].max_speed) SETTINGS.cars[i].speed = SETTINGS.cars[i].max_speed;
        if (SETTINGS.cars[i].acceleration > SETTINGS.cars[i].max_acceleration) SETTINGS.cars[i].acceleration = SETTINGS.cars[i].max_acceleration;
    }

    if (allFinished)
    {
        for (int i = 0; i < SETTINGS.nCars; i++)
        {
            SETTINGS.cars[i].finished = false;
            SETTINGS.cars[i].pos = 0;
            SETTINGS.cars[i].speed = 0;
            SETTINGS.cars[i].acceleration = 0;

            SETTINGS.cars[i].max_speed = 0.01 + fabs(randn(0.1, 0.5));
            SETTINGS.cars[i].max_acceleration = 0.001 + fabs(randn(0.01, 0.05));
            SETTINGS.cars[i].power = 0.0001 + fabs(randn(0.001, 0.005));

            if (tesla != NULL && rand() % 10 == 0)
            {
                SETTINGS.cars[i].draw = draw_model;
                SETTINGS.cars[i].drawing_data = tesla;
            }
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
    glLightfv(GL_LIGHT0, GL_POSITION, (float*) &SETTINGS.light0Pos);
    glLightfv(GL_LIGHT1, GL_POSITION, (float*) &SETTINGS.light1Pos);
    glLightfv(GL_LIGHT2, GL_POSITION, (float*) &SETTINGS.light2Pos);
    glLightfv(GL_LIGHT3, GL_POSITION, (float*) &SETTINGS.light3Pos);

    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, SETTINGS.light3Exp);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, SETTINGS.light3Cutoff);
    /* Finish is at 0, 0, ROADLEN/2*/
    float direction[] = {
            -SETTINGS.light3Pos.x,
            -SETTINGS.light3Pos.y,
            ROAD_LENGHT/2.0 - SETTINGS.light3Pos.z,
    };
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction);

    /*
    glBegin(GL_LINES);
    glVertex3f(0, 5, ROAD_LENGHT/2.0);
    glVertex3fv(&SETTINGS.light3Pos.x);
    glEnd();
    */

    glDisable(GL_LIGHTING); /* Disabled for drawing debug symbols */

    /* Draw debug stuff */
    if (SETTINGS.debug)
    {
        draw_axis(50);

        if (CAMERA.type == CAM_TYPE_ABSOLUTE)
        {
            glPushMatrix();
            glTranslated(CAMERA.target.x, CAMERA.target.y, CAMERA.target.z);
            glPushAttrib(GL_LINE_BIT);
            glLineWidth(3);
            draw_axis(1);
            glPopAttrib(); /* GL_LINE_BIT */
            glPopMatrix();
        }

        if (SETTINGS.light0) draw_axis_at((float*) &SETTINGS.light0Pos, 0.5);
        if (SETTINGS.light1) draw_axis_at((float*) &SETTINGS.light1Pos, 0.5);
        if (SETTINGS.light2) draw_axis_at((float*) &SETTINGS.light2Pos, 0.5);
        if (SETTINGS.light3) draw_axis_at((float*) &SETTINGS.light3Pos, 0.5);
    }

    /* Start non-debug stuff */

    glEnable(GL_LIGHTING);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, SETTINGS.shininess);

    if (SETTINGS.light0) glEnable(GL_LIGHT0); else glDisable(GL_LIGHT0);
    if (SETTINGS.light1) glEnable(GL_LIGHT1); else glDisable(GL_LIGHT1);
    if (SETTINGS.light2) glEnable(GL_LIGHT2); else glDisable(GL_LIGHT2);
    if (SETTINGS.light3) glEnable(GL_LIGHT3); else glDisable(GL_LIGHT3);

    draw_lamp(GL_LIGHT0);
    draw_lamp(GL_LIGHT1);
    draw_lamp(GL_LIGHT2);
    draw_lamp(GL_LIGHT3);

    glPushMatrix();

    glTranslated(LANE_WIDTH / 2.0 + (SETTINGS.nCars * LANE_WIDTH) / -2.0, 0, -ROAD_LENGHT / 2.0);

    draw_road(SETTINGS.nCars, banners);

    for (int i = 0; i < SETTINGS.nCars; i++)
    {
        glPushMatrix();
        glTranslatef(i * LANE_WIDTH, 0, SETTINGS.cars[i].pos);
        SETTINGS.cars[i].draw(&SETTINGS.cars[i]);
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

    /* initialize random seed: */
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(WINDOW.width, WINDOW.height);
    glutCreateWindow("Zeepkist - Dries Kennes");

    tesla = stlLoadFile("Tesla.stl");
    if (tesla == NULL) fputs("Tesla model was not loaded :(", stderr);

    banners = textures_load("banners.bmp");

    SETTINGS.nCars = 3;
    SETTINGS.cars = calloc(SETTINGS.nCars, sizeof(Car));
    for (int i = 0; i < SETTINGS.nCars; i++)
    {
        SETTINGS.cars[i].draw = draw_soapbox;
        SETTINGS.cars[i].drawing_data = NULL;

        SETTINGS.cars[i].finished = false;
        SETTINGS.cars[i].pos = (float) (rand() % (int)ROAD_LENGHT);
        SETTINGS.cars[i].speed = 0;
        SETTINGS.cars[i].acceleration = 0;

        SETTINGS.cars[i].max_speed = 0.01 + fabs(randn(0.1, 0.5));
        SETTINGS.cars[i].max_acceleration = 0.001 + fabs(randn(0.01, 0.05));
        SETTINGS.cars[i].power = 0.0001 + fabs(randn(0.001, 0.005));

        if (tesla != NULL && rand() % 10 == 0)
        {
            SETTINGS.cars[i].draw = draw_model;
            SETTINGS.cars[i].drawing_data = tesla;
        }
    }

    glClearColor(0.0, 0.0, 0.0, 0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    const float ambient[] = {0.1, 0.1, 0.1, 1};
    const float white[] = {1.0, 1.0, 1.0, 1.0};
    const float black[] = {0.0, 0.0, 0.0, 1.0};
    const float weird1[] = {0.25, 1.0, 1.0, 1.0};
    const float weird2[] = {1.0, 0.25, 0.25, 1.0};
    const float yellow[] = {1.0, 1.0, 0.0, 1.0};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, black);
    glLightfv(GL_LIGHT0, GL_SPECULAR, black);

    glLightfv(GL_LIGHT1, GL_AMBIENT, black);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, weird1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, black);

    glLightfv(GL_LIGHT2, GL_AMBIENT, black);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, weird2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, black);

    glLightfv(GL_LIGHT3, GL_AMBIENT, yellow);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, yellow);
    glLightfv(GL_LIGHT3, GL_SPECULAR, yellow);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.75);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.75);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.75);
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 0.75);
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
