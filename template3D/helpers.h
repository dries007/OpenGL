#ifndef OPENGL_HELPERS_H
#define OPENGL_HELPERS_H

#include <stdbool.h>

/* Types */

typedef struct {
    int x, y;
} Vect2i;

typedef struct {
    double x, y, z;
} Vect3d;

typedef struct {
    double x, y, z, w;
} Vect4d;

typedef enum {
    CAM_TYPE_LOOK_AT, CAM_TYPE_GAME_AZERTY, CAM_TYPE_GAME_QWERTY
} CameraType;
typedef struct {
    CameraType type;
    Vect3d pos;
    union {
        Vect3d target;
        struct { double yaw, pitch; };
    };
} Camera;

typedef enum {
    PERSP_TYPE_ORTHO, PERSP_TYPE_FRUSTUM, PERSP_TYPE_FOV
} PerspectiveType;

typedef struct {
    PerspectiveType type;
    union {
        struct { double left, right, bottom, top; };
        struct { double fov, aspect; };
    };
    double near, far;
} Perspective;

typedef struct {
    int width, height;
} Window;

/* Declared with defaults in helpers.c */

extern Camera camera;
extern Perspective perspective;
extern Window window;

/* Helper functions */

void error(int status, const char *message);
void drawAxis(double size);
void drawCheckersZ(double size, int count);

Vect3d diff3d(Vect3d a, Vect3d b);
Vect3d add3d(Vect3d a, Vect3d b);
Vect3d norm3d(Vect3d a);
double dist(Vect3d a);
Vect3d crossProduct3d(Vect3d a, Vect3d b);
Vect3d mult3ds(Vect3d a, double s);
bool handleMove(unsigned char key, int modifiers, int x, int y);

#endif //OPENGL_HELPERS_H
