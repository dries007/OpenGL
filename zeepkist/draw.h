#ifndef OPENGL_DRAW_H
#define OPENGL_DRAW_H

/**
 * Contains drawing (helper) functions
 */

#include "globals.h"

void draw_cuboid(double x, double y, double z);
void draw_cylinder(double radius_bottom, double radius_top, double height, double radius_hole);
void draw_axis(double size);
void draw_checkers(double size, int count);
void draw_axis_at(float* pos, double size);
void draw_overlay(void);
void draw_crosshair(int size);
void draw_road(size_t lanes);
void draw_soapbox(Car *car);
void draw_model(Car* car);
void draw_lamp(float* pos, bool on);

#endif //OPENGL_DRAW_H
