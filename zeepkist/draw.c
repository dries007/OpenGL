#include "draw.h"
#include "globals.h"

void draw_cuboid(double x, double y, double z)
{
    glPushMatrix();
    glScaled(x, y, z);
    if (SETTINGS.debug) glutWireCube(1);
    else glutSolidCube(1);
    glPopMatrix();
}

void draw_cylinder(double radius_bottom, double radius_top, double height, double radius_hole)
{
    const int roundness = 10;
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();

    glTranslated(0, 0, -height/2);

    gluQuadricDrawStyle(quad, SETTINGS.debug ? GLU_LINE : GLU_FILL);

    gluDisk(quad, radius_hole, radius_bottom, (int) (roundness * radius_bottom), roundness); /* Bottom */
    gluCylinder(quad, radius_bottom, radius_top, height, (int) (roundness * (radius_bottom > radius_top ? radius_bottom : radius_top)), roundness); /* Mantle */
    if (radius_hole != 0)
        gluCylinder(quad, radius_hole, radius_hole, height, (int) (roundness * radius_hole), roundness); /* Hole Mantle */
    glTranslated(0, 0, height);
    gluDisk(quad, radius_hole, radius_top, (int) (roundness * radius_top), roundness); /* Top */
    glTranslated(0, 0, -height);

    glPopMatrix();
    gluDeleteQuadric(quad);
}

void draw_axis(double size)
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

void draw_axis_at(float* pos, double size)
{
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    draw_axis(size);
    glPopMatrix();
}

void draw_overlay()
{
    /*
     * Why is this such a mess? Because the old, old, old, old version of openGL on the school
     * 'mainframe' does not support most of glEnable/glDisable 'enums' on glPopAttrib/glPushAttrib.
     */

    /* START "context" switch, to overlay mode */
    glMatrixMode(GL_PROJECTION); /* Change to projection required. */
    glPushMatrix(); /* Save Projection matrix */
    glLoadIdentity(); /* Cleanup */
    gluOrtho2D(0, WINDOW.width, WINDOW.height, 0); /* Got to 2D view with top-left (0;0) -> bottom-right (w;h) */
    glMatrixMode(GL_MODELVIEW); /* Back to drawing stuff */
    glLoadIdentity(); /* Cleanup */
    glDisable(GL_DEPTH_TEST); /* No depth test for overlay elements */
    glDisable(GL_LIGHTING); /* No lighting either */
    /* END "context" switch, to overlay mode */

    /* These vectors are updated by the string drawing functions and set to the next new line. */
    Vect2i cursorL = {5, font_line_height};
    Vect2i cursorR = {WINDOW.width-5, font_line_height};

    glColor3f(1, 1, 1); /* Draw the most important line in white */
    disp_puts(&cursorL, ALIGN_LEFT, "Zeepkist - Dries Kennes");

    glColor4f(1, 1, 1, .5); /* Everything else can be a lot more grey */
    disp_printf(&cursorR, ALIGN_RIGHT, "Camera: X: %4.2lf Y: %4.2lf Z: %4.2lf\n", CAMERA.pos.x, CAMERA.pos.y, CAMERA.pos.z);
    switch (CAMERA.type)
    {
        case CAM_TYPE_ABSOLUTE:
            disp_printf(&cursorR, ALIGN_RIGHT, "Target: X: %4.2lf Y: %4.2lf Z: %4.2lf\n", CAMERA.target.x, CAMERA.target.y, CAMERA.target.z);
            break;
        case CAM_TYPE_GAME_AZERTY:
            disp_printf(&cursorR, ALIGN_RIGHT, "Yaw: %4.2lf Pitch: %4.2lf\n", CAMERA.yaw, CAMERA.pitch);
            disp_puts(&cursorR, ALIGN_RIGHT, "Mouse enabled\n");
            break;

        case CAM_TYPE_GAME_QWERTY:
            disp_printf(&cursorR, ALIGN_RIGHT, "Yaw: %4.2lf Pitch: %4.2lf\n", CAMERA.yaw, CAMERA.pitch);
            disp_puts(&cursorR, ALIGN_RIGHT, "Mouse enabled\n");
            break;
    }

    disp_printf(&cursorR, ALIGN_RIGHT, "Render distance %4.2lf - %4.2lf\n", PERSPECTIVE.near, PERSPECTIVE.far);

    switch (PERSPECTIVE.type)
    {
        case PERSP_TYPE_ORTHO: disp_puts(&cursorR, ALIGN_RIGHT, "Perspective: Ortho\n"); break;
        case PERSP_TYPE_FRUSTUM: disp_puts(&cursorR, ALIGN_RIGHT, "Perspective: Frustum\n"); break;
        case PERSP_TYPE_FOV: disp_puts(&cursorR, ALIGN_RIGHT, "Perspective: FOV\n"); break;
    }
    switch (CAMERA.type)
    {
        case CAM_TYPE_ABSOLUTE: disp_puts(&cursorR, ALIGN_RIGHT, "Camera Absolute mode.\n"); break;
        case CAM_TYPE_GAME_AZERTY: disp_puts(&cursorR, ALIGN_RIGHT, "Camera AZERTY mode\n"); break;
        case CAM_TYPE_GAME_QWERTY: disp_puts(&cursorR, ALIGN_RIGHT, "Camera QWERTY mode\n"); break;
    }

    if (SETTINGS.debug) disp_puts(&cursorR, ALIGN_RIGHT, "DEBUG drawing mode\n");

    if (SETTINGS.bezierMode == GL_FILL) disp_puts(&cursorR, ALIGN_RIGHT, "Full bezier mode\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Line bezier mode\n");

    if (SETTINGS.flat) disp_puts(&cursorR, ALIGN_RIGHT, "Flat shading\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Smooth shading\n");

    disp_printf(&cursorR, ALIGN_RIGHT, "Colors: Arch=%s Body=%s Chassis=%s\n",
                SETTINGS.colorArch == M_GEEL ? "geel" : "lila",
                SETTINGS.colorBody == M_GRIJS ? "grijs" : "witachtig",
                SETTINGS.colorChassis == M_CHROME ? "chrome" : "brons");

    if (SETTINGS.move) disp_puts(&cursorR, ALIGN_RIGHT, "Movement on\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Movement off\n");

    disp_printf(&cursorR, ALIGN_RIGHT, "Shininess: %f\n", SETTINGS.shininess);

    disp_printf(&cursorR, ALIGN_RIGHT, "Cars: %d\n", (int) SETTINGS.nCars);

    disp_printf(&cursorR, ALIGN_RIGHT, "Transparency: %0.2f\n", SETTINGS.transp);

    disp_printf(&cursorR, ALIGN_RIGHT, "Spot: exp=%.1f cutoff=%.1f\n", SETTINGS.light3Exp, SETTINGS.light3Cutoff);

    if (CAMERA.type != CAM_TYPE_ABSOLUTE)
    {
        glColor4f(1, 1, 1, .9);
        draw_crosshair(10);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); /* Change to projection required. */
    glPopMatrix(); /* Restore projection matrix */
}

void draw_crosshair(int size)
{
    glBegin(GL_LINES);
    glVertex2i(WINDOW.width/2-size, WINDOW.height/2);
    glVertex2i(WINDOW.width/2+size, WINDOW.height/2);
    glVertex2i(WINDOW.width/2, WINDOW.height/2-size);
    glVertex2i(WINDOW.width/2, WINDOW.height/2+size);
    glEnd();
}

void draw_road(size_t lanes, uint banner_texture)
{
    glPushMatrix();

    glRotated(90, 1, 0, 0);

    const float stripe = 0.1;
    const float step = 10;

    const float start = -LANE_WIDTH;
    const float end = ROAD_LENGHT + LANE_WIDTH;

    /* Start & Finish lines */
    glPushMatrix();
    glTranslated(0, 0, -.001);
    glColor3ub(250, 250, 250);
    for (float i = -LANE_WIDTH/2; i < lanes*LANE_WIDTH-LANE_WIDTH/2; i += 4 * stripe)
    {
        glRectf(i, start + 2*LANE_WIDTH + stripe, i + 2 * stripe, start + 2*LANE_WIDTH - stripe);
        glRectf(i, end - 2*LANE_WIDTH + stripe, i + 2 * stripe, end - 2*LANE_WIDTH - stripe);
    }
    glPopMatrix();

    for (float i = start; i < end; i += step)
    {
        glPushMatrix();
        glColor3ub(250, 250, 250);
        glRectf(-LANE_WIDTH/2-stripe, i, -LANE_WIDTH/2+stripe, i+step);

        for (int lane = 0; lane < lanes; ++lane)
        {
            glColor3ub(66, 66, 66);
            glRectf(-LANE_WIDTH/2+stripe, i, LANE_WIDTH/2-stripe, i+step);
            glColor3ub(250, 250, 250);
            glRectf(LANE_WIDTH/2-stripe, i, LANE_WIDTH/2+stripe, i+step);

            glTranslated(LANE_WIDTH, 0, 0);
        }
        glPopMatrix();
    }

    glPopMatrix(); /* Outside of rotation to draw flat planes */
    glPushMatrix(); /* Draw barriers */

    glColor3ub(255, 255, 255);
    if (banner_texture != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, banner_texture);
        glBegin(GL_QUADS);
        glTexCoord2i(1, 0); glVertex3d(-LANE_WIDTH/2.0+0.01,   0, 0);
        glTexCoord2i(0, 0); glVertex3d(-LANE_WIDTH/2.0+0.01,   0, ROAD_LENGHT);
        glTexCoord2i(0, 1); glVertex3d(-LANE_WIDTH/2.0+0.01, 2.5, ROAD_LENGHT);
        glTexCoord2i(1, 1); glVertex3d(-LANE_WIDTH/2.0+0.01, 2.5, 0);

        glTexCoord2i(0, 0); glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0-0.01,   0, 0);
        glTexCoord2i(1, 0); glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0-0.01,   0, ROAD_LENGHT);
        glTexCoord2i(1, 1); glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0-0.01, 2.5, ROAD_LENGHT);
        glTexCoord2i(0, 1); glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0-0.01, 2.5, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_QUADS);
        glVertex3d(-LANE_WIDTH/2.0,   0, ROAD_LENGHT);
        glVertex3d(-LANE_WIDTH/2.0, 2.5, ROAD_LENGHT);
        glVertex3d(-LANE_WIDTH/2.0, 2.5, 0);
        glVertex3d(-LANE_WIDTH/2.0,   0, 0);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0,   0, ROAD_LENGHT);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0, 2.5, ROAD_LENGHT);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0, 2.5, 0);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0,   0, 0);
        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);
        glVertex3d(-LANE_WIDTH/2.0,   0, ROAD_LENGHT);
        glVertex3d(-LANE_WIDTH/2.0, 2.5, ROAD_LENGHT);
        glVertex3d(-LANE_WIDTH/2.0, 2.5, 0);
        glVertex3d(-LANE_WIDTH/2.0,   0, 0);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0,   0, ROAD_LENGHT);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0, 2.5, ROAD_LENGHT);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0, 2.5, 0);
        glVertex3d(lanes*LANE_WIDTH-LANE_WIDTH/2.0,   0, 0);
        glEnd();
    }

    glPopMatrix();
    glPushMatrix();
    glTranslatef(-LANE_WIDTH/2.0, 0.0, ROAD_LENGHT);
    draw_arch(lanes * LANE_WIDTH);
    glPopMatrix();
}

void draw_arch(float width)
{
    const float height = 10;

    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float *) &SETTINGS.colorArch[0]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float *) &SETTINGS.colorArch[1]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float *) &SETTINGS.colorArch[2]);

    glTranslatef(-1, height/2.0, 1);
    glRotated(-90, 1, 0, 0);

    draw_cylinder(1, 1, height, 0);
    glPushMatrix();
    glTranslatef(width + 2, 0.0, 0.0);
    draw_cylinder(1, 1, height, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(width/2.0 + 1.0, 0.0, height/2.0);
    draw_cuboid(width + 4, 2, 2);
    glPopMatrix();

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
}

void draw_wheel(double distance)
{
    const double radius = 1;

    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    glRotated(90, 0, 1, 0);
    glRotated((distance * 180.0)/(radius * M_PI), 0, 0, 1);

    draw_cylinder(0.1, 0.1, 0.4, 0); /* Axel */
    draw_cylinder(0.1, 0.1, 0.4, 0); /* Axel */

    glEnable(GL_COLOR_MATERIAL);
    glColor3ub(0, 0, 0);
    draw_cylinder(1, 1, 0.2, 0.8); /* Wheel */

    glColor3ub(200, 200, 200);
    for (int a = 0; a < 360; a += (360/7)) /* Spokes */
    {
        glPushMatrix();
        glRotated(a, 0, 0, 1);
        glTranslated(0, 0.4, 0);
        draw_cuboid(0.1, 0.8, 0.1);
        glPopMatrix();
    }

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
}

void draw_soapbox(Car *car)
{
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();

    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float *) &SETTINGS.colorChassis[0]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float *) &SETTINGS.colorChassis[1]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float *) &SETTINGS.colorChassis[2]);

    glTranslated(0, 1, 0);

    draw_cuboid(2, 0.1, 2); /* back chassis body cuboid */

    glPushMatrix();
    glTranslated(1.2, 0, 0);
    draw_wheel(car->pos); /* back wheel 1 */
    glPopMatrix();
    glPushMatrix();
    glTranslated(-1.2, 0, 0);
    draw_wheel(car->pos); /* back wheel 2 */
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0, 2);
    draw_cuboid(1, 0.1, 2); /* front chassis body cuboid */
    glTranslated(0, 0, 1.7);
    glPushMatrix();
    glTranslated(0.3, 0, 0);
    draw_cuboid(0.2, 0.1, 1.4); /* front chassis wheel connector 1 */
    glPopMatrix();
    glPushMatrix();
    glTranslated(-0.3, 0, 0);
    draw_cuboid(0.2, 0.1, 1.4); /* front chassis wheel connector 2 */
    glPopMatrix();
    glTranslated(0, 0, 0.5);
    draw_wheel(car->pos);  /* front wheel */
    glPopMatrix();

    glPushMatrix();
    glRotated(270, 1, 0, 0);
    glTranslated(0, 0, 0.45);
    draw_cylinder(0.5, 0.3, 1, 0);  /* Chair bottom */
    glTranslated(0, 0, 0.55);
    draw_cylinder(0.8, 0.8, 0.1, 0);  /* Chair bottom */
    glPopMatrix();

    glPushMatrix();
    glColor3ub(200, 200, 200);

    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);
    glTranslatef(-1.5, 0, -0.75);
    glScaled(0.85, 0.9, 1);

    static const Vect3f ctrlpoints[4][6] = {
            {
                    {10.0, 0.0, 0.0},
                    {8.0, 1.0, 0.0},
                    {6.0, 2.0, 0.0},
                    {4.0, 3.0, 0.0},
                    {0.0, 3.0, 0.0},
                    {0.0, 0.0, 0.0},
            },
            {
                    {10.0, 0.0, 0.2},
                    {8.0, 1.0, 1.0},
                    {6.0, 2.0, 1.0},
                    {4.0, 3.0, 1.0},
                    {0.0, 3.0, 1.0},
                    {0.0, 0.0, 1.0},
            },
            {
                    {10.0, 0.0, 0.4},
                    {8.0, 0.0, 3.0},
                    {6.0, 2.0, 2.0},
                    {4.0, 3.0, 2.0},
                    {0.0, 3.0, 2.0},
                    {0.0, 0.0, 2.0},
            },
            {
                    {10.0, 0.0, 0.6},
                    {6.0, 0.0, 3.0},
                    {2.0, -0.25, 3.0},
                    {2.0, 0.7, 3.0},
                    {0.0, 1.5, 3.0},
                    {0.0, 0.0, 3.0},
            }
    };
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 6, 0, 1, 3*6, 4, &ctrlpoints[0][0].x);

    if (SETTINGS.debug)
    {
        glPushAttrib(GL_LIGHTING_BIT); /* For material */
        glDisable(GL_LIGHTING); /* debug mode engaged */
        glPointSize(10);
        glBegin(GL_POINTS);
        const unsigned char debug_colors[4][3] = {
                {255, 0, 0}, /* R */
                {0, 255, 0}, /* G */
                {0, 0, 255}, /* B */
                {255, 255, 255}, /* W */
        };
        for (int i = 0; i < 4; ++i)
        {
            glColor3ubv(debug_colors[i]);
            for (int j = 0; j < 6; ++j)
            {
                glVertex3fv(&ctrlpoints[i][j].x);
            }
        }
        glPointSize(1);
        glEnd();
        glPopAttrib(); /* GL_LIGHTING_BIT */
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float *) &SETTINGS.colorBody[0]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float *) &SETTINGS.colorBody[1]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float *) &SETTINGS.colorBody[2]);

    glEnable(GL_MAP2_VERTEX_3);
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
    glEvalMesh2(SETTINGS.bezierMode, 0, 20, 0, 20);
    glScaled(1, -1, 1);
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
    glEvalMesh2(SETTINGS.bezierMode, 0, 20, 0, 20);
    glDisable(GL_MAP2_VERTEX_3);
    glPopMatrix();

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
    gluDeleteQuadric(quad);
}

void draw_model(Car* car)
{
    if (car->drawing_data == NULL) abort();

    glPushMatrix();

    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    const static Vect4f colorAmbient  = {0.19225,    0.19225,    0.19225,    1.0};
    const static Vect4f colorDiffuse  = {0.50754,    0.50754,    0.50754,    1.0};
    const static Vect4f colorSpecular = {0.508273,   0.508273,   0.508273,   1.0};

    glDisable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (float *) &colorAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (float *) &colorDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (float *) &colorSpecular);

    glColor3ub(85, 3, 64); /* Midnight cherry */

    glTranslated(0, 1.25, 0);
    glScaled(0.05, 0.05, 0.05);
    glRotated(-90, 1, 0, 0);
    stlDisplayModel(car->drawing_data);

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
}

void draw_lamp(GLenum id)
{
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    GLenum type;
    switch (id)
    {
        case GL_LIGHT0:
            glTranslatef(SETTINGS.light0Pos.x, SETTINGS.light0Pos.y, SETTINGS.light0Pos.z);
            type = GL_AMBIENT;
            break;
        case GL_LIGHT1:
            glTranslatef(SETTINGS.light1Pos.x, SETTINGS.light1Pos.y, SETTINGS.light1Pos.z);
            type = GL_DIFFUSE;
            break;
        case GL_LIGHT2:
            glTranslatef(SETTINGS.light2Pos.x, SETTINGS.light2Pos.y, SETTINGS.light2Pos.z);
            type = GL_DIFFUSE;
            break;
        case GL_LIGHT3:
            glTranslatef(SETTINGS.light3Pos.x, SETTINGS.light3Pos.y, SETTINGS.light3Pos.z);
            type = GL_SPECULAR;
            break;
        default:
            abort();
    }
    float data[4];
    glGetLightfv(id, type, data);
    data[4] = 1.0;
    glColor3fv(data);
    if (glIsEnabled(id)) glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, data);
    if (SETTINGS.debug) glutWireSphere(1, 16, 8); else glutSolidSphere(1, 16, 8);

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
}
