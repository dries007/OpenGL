#ifndef OPENGL_INPUT_H
#define OPENGL_INPUT_H

/**
 * Contains input handling
 */

#include "globals.h"

void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

#endif //OPENGL_INPUT_H
