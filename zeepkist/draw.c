#include "draw.h"
#include "globals.h"

void draw_cuboid(double x, double y, double z)
{
    glPushMatrix();
    glScaled(x, y, z);
    if (SETTINGS.wireframe) glutWireCube(1);
    else glutSolidCube(1);
    glPopMatrix();
}

void draw_cylinder(double radius_bottom, double radius_top, double height, double radius_hole)
{
    const int roundness = 64;
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();

    glTranslated(0, 0, -height/2);

    gluQuadricDrawStyle(quad, SETTINGS.wireframe ? GLU_LINE : GLU_FILL);

    gluDisk(quad, radius_hole, radius_bottom, (int) (roundness * radius_bottom), roundness); // Bottom
    gluCylinder(quad, radius_bottom, radius_top, height, (int) (roundness * (radius_bottom > radius_top ? radius_bottom : radius_top)), roundness); // Mantle
    if (radius_hole != 0)
        gluCylinder(quad, radius_hole, radius_hole, height, (int) (roundness * radius_hole), roundness); // Hole Mantle
    glTranslated(0, 0, height);
    gluDisk(quad, radius_hole, radius_top, (int) (roundness * radius_top), roundness); // Top
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

void draw_checkers(double size, int count)
{
    glPushMatrix();
    glRotated(90, 1, 0, 0);
    glScaled(size, size, 1);

    Vect4d back = {0, 0, 0, 0.1};
    Vect4d white = {1, 1, 1, 0.1};

    for (int x = -count; x < count; x++)
    {
        for (int y = -count; y < count; y++)
        {
            glColor4dv((double*)((x + y) % 2 == 0 ? &back : &white));
            glRectd(x, y, x+1, y+1);
        }
    }

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
            break;

        case CAM_TYPE_GAME_QWERTY:
            disp_printf(&cursorR, ALIGN_RIGHT, "Yaw: %4.2lf Pitch: %4.2lf\n", CAMERA.yaw, CAMERA.pitch);
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

    if (SETTINGS.wireframe) disp_puts(&cursorR, ALIGN_RIGHT, "Wire mode\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Full mode\n");

    if (SETTINGS.flat) disp_puts(&cursorR, ALIGN_RIGHT, "Flat shading\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Smooth shading\n");

    if (SETTINGS.move) disp_puts(&cursorR, ALIGN_RIGHT, "Movement on\n");
    else disp_puts(&cursorR, ALIGN_RIGHT, "Movement off\n");

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

void draw_road(size_t lanes)
{
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    glRotated(90, 1, 0, 0);

    const float stripe = 0.1;
    const float step = 10;

    const float start = -LANE_WIDTH;
    const float end = ROAD_LENGHT + LANE_WIDTH;

    glPushMatrix();
    glTranslated(0, 0, -.001);
    glColor3ub(66, 244, 229); // Lila-ish
    glRectf(-LANE_WIDTH/2-stripe, start + 2*LANE_WIDTH + stripe, lanes*LANE_WIDTH-LANE_WIDTH/2+stripe, start + 2*LANE_WIDTH - stripe);
    glRectf(-LANE_WIDTH/2-stripe, end - 2*LANE_WIDTH + stripe, lanes*LANE_WIDTH-LANE_WIDTH/2+stripe, end - 2*LANE_WIDTH - stripe);
    glPopMatrix();

    for (float i = start; i < end; i += step)
    {
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glRectf(-LANE_WIDTH/2-stripe, i, -LANE_WIDTH/2+stripe, i+step);

        for (int lane = 0; lane < lanes; ++lane)
        {
            glColor3ub(66, 66, 66);
            glRectf(-LANE_WIDTH/2+stripe, i, LANE_WIDTH/2-stripe, i+step);
            glColor3ub(255, 255, 255);
            glRectf(LANE_WIDTH/2-stripe, i, LANE_WIDTH/2+stripe, i+step);

            glTranslated(LANE_WIDTH, 0, 0);
        }
        glPopMatrix();
    }

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
}

void draw_soapbox(Car *car)
{
    glPushMatrix();
    glColor3ub(255, 255, 255);
    draw_cuboid(1, 1, 1);
    glPopMatrix();
}

void draw_model(Car* car)
{
    if (car->drawing_data == NULL) abort();

    glPushMatrix();

    glColor3ub(85, 3, 64); // Midnight cherry
    glTranslated(0, 1.25, 0);
    glScaled(0.05, 0.05, 0.05);
    glRotated(-90, 1, 0, 0);
    stlDisplayModel(car->drawing_data);
    glPopMatrix();
}

void draw_lamp(float* pos, bool on)
{
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT); /* For material */

    glTranslatef(pos[0], pos[1], pos[2]);
    float color[] = {1, 0.9, 0.01, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    if (on) glMaterialfv(GL_FRONT, GL_EMISSION, color);
    if (SETTINGS.wireframe) glutWireSphere(1, 16, 16); else glutSolidSphere(1, 16, 16);

    glPopAttrib(); /* GL_LIGHTING_BIT */
    glPopMatrix();
}
