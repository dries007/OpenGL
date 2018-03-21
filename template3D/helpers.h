#ifndef OPENGL_HELPERS_H
#define OPENGL_HELPERS_H

/* Types */

typedef struct {
    double x, y, z;
} Vect3d;

typedef struct {
    Vect3d eye;
    Vect3d center;
    Vect3d up;
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

#endif //OPENGL_HELPERS_H
