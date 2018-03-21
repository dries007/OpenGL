#ifndef OPENGL_COLORS_H
#define OPENGL_COLORS_H

typedef struct colorf
{
    float r, g, b;
} Colorf;

void color_wheel(Colorf *colors, int n, float v, float s, float h);

#endif
