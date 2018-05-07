#ifndef OPENGL_TYPES_H
#define OPENGL_TYPES_H

/**
 * Contains global includes, types and variables (extern only)
 */

/*
 * LIB INCLUDES
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <GL/glut.h>

/*
 * TYPES
 */

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
    CAM_TYPE_ABSOLUTE, CAM_TYPE_GAME_AZERTY, CAM_TYPE_GAME_QWERTY
} CameraType;
typedef struct {
    CameraType type;
    Vect3d pos;

    Vect3d target;
    double yaw, pitch;

} Camera;

typedef enum {
    PERSP_TYPE_ORTHO, PERSP_TYPE_FRUSTUM, PERSP_TYPE_FOV
} PerspectiveType;

typedef struct {
    PerspectiveType type;
    double left, right, bottom, top;
    double fov, aspect;
    double near, far;
} Perspective;

typedef struct {
    int width, height;
} Window;

/*
 * GLOBALS
 */

extern Camera camera;
extern Perspective perspective;
extern Window window;

/*
 * PROJECT INCLUDES
 */

#include "stl.h"
#include "draw.h"
#include "input.h"
#include "text.h"

#endif //OPENGL_TYPES_H
