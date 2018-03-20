#ifndef OPENGL_COLORS_H
#define OPENGL_COLORS_H

typedef struct colorf
{
    float r, g, b;
} Colorf;

typedef struct colorb
{
    unsigned char r, g, b;
} Colorb;


Colorb colorf2b(Colorf color);
Colorf colorb2f(Colorb color);

void color_wheel(Colorf *colors, int n, float v, float s, float h);

#endif
