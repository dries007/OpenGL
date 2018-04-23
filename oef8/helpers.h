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

/* Declared with defaults in helpers.c */

extern Camera camera;
extern Perspective perspective;
extern Window window;

/* Helper functions */

void error(int status, const char *message);
void drawAxis(double size);
void drawCheckersZ(double size, int count);
void drawCrosshair(int size);

bool handleMove(unsigned char key, int modifiers, int x, int y);
void moveCamera(double forwards, double strafe, double yaw, double pitch, double x, double y, double z);

extern const char *KEYMAP_QWERTY;
extern const char *MOUSE_ABSOLUTE;
extern const char *MOUSE_GAME;
extern const char *KEYMAP_AZERTY;
extern const char *KEYMAP_ABSOLUTE;
extern const char *KEYMAP;
extern const char *TITLE;

/* Callback functions */
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void drawOverlay();

#endif //OPENGL_HELPERS_H
