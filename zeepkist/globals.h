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
#include <time.h>
#include <GL/glut.h>
#include "stl.h"

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
    float x, y, z;
} Vect3f;

typedef struct {
    float x, y, z, w;
} Vect4f;

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

typedef struct car {
    void (*draw)(struct car*);
    void * drawing_data;
    double pos;
    double speed;
    double acceleration;
    double max_speed;
    double max_acceleration;
    double power;
    bool finished;
} Car;

typedef struct {
    bool debug;
    bool light0;
    bool light1;
    bool light2;
    bool light3;
    Vect4f light0Pos;
    Vect4f light1Pos;
    Vect4f light2Pos;
    Vect4f light3Pos;
    bool flat;
    float shininess;
    bool overlay;
    bool move;
    GLenum bezierMode;
    Vect4f* colorArch;
    Vect4f* colorChassis;
    Vect4f* colorBody;
    float transp;
    size_t nCars;
    Car* cars;
    float light3Exp;
    float light3Cutoff;
} Settings;

/*
 * GLOBALS
 */

/* Variables defined in main.c */
extern const float ROAD_LENGHT;
extern const float LANE_WIDTH;
extern Camera CAMERA;
extern Perspective PERSPECTIVE;
extern Window WINDOW;
extern Settings SETTINGS;

extern Vect4f M_GRIJS[3];
extern Vect4f M_WITACHTIG[3];
extern Vect4f M_CHROME[3];
extern Vect4f M_BRONS[3];
extern Vect4f M_GEEL[3];
extern Vect4f M_LILA[3];

/* Functions defined in main.c */

double randn(double mu, double sigma);
void reshape(int w, int h);

/*
 * PROJECT INCLUDES
 */

#include "stl.h"
#include "draw.h"
#include "input.h"
#include "text.h"
#include "textures.h"

#endif //OPENGL_TYPES_H
