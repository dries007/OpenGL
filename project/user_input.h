#include <stdbool.h>
#include "helpers.h"

extern bool overlay;
extern bool mouseLeftDown;
extern bool mouseRightDown;
extern bool mouseMiddleDown;
extern double mouseZoomDiv;
extern double mouseRotateDiv;
extern double mousePanDiv;
extern Vect2i prevMouse;
extern bool wire;
extern bool move;
extern bool flat;

/* Callback functions */
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void reshape(int w, int h);
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void drawOverlay(void);

extern const char *TITLE;
extern const char *KEYMAP;
extern const char *KEYMAP_ABSOLUTE;
extern const char *KEYMAP_AZERTY;
extern const char *KEYMAP_QWERTY;
extern const char *MOUSE_ABSOLUTE;
extern const char *MOUSE_GAME;