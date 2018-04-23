#include "colors.h"
#include <math.h>

/**
 * Generate color wheel in array. Varies hue in equal steps starting from `h`.
 * Uses HSV model with constant value `v` and saturation `v`.
 *
 * Based on https://dystopiancode.blogspot.be/2012/06/hsv-rgb-conversion-algorithms-in-c.html
 *
 * @param colors Data array. Must be at least `n` elements long.
 * @param n Number of colors
 * @param v Color value [0, 1]
 * @param s Color saturation [0, 1]
 * @param h Starting hue [0, 360[
 */
void color_wheel(Colorf *colors, const int n, const float v, const float s, float h)
{
    for (int i = 0; i < n; i++)
    {
        float c = v * s;
        float x = (float) (c * (1.0f - fabs(fmod(h / 60.0f, 2) - 1.0f)));
        float m = v - c;
        if (h >= 0.0 && h < 60.0)
        {
            colors[i].r = c + m;
            colors[i].g = x + m;
            colors[i].b = m;
        }
        else if (h >= 60.0 && h < 120.0)
        {
            colors[i].r = x + m;
            colors[i].g = c + m;
            colors[i].b = m;
        }
        else if (h >= 120.0 && h < 180.0)
        {
            colors[i].r = m;
            colors[i].g = c + m;
            colors[i].b = x + m;
        }
        else if (h >= 180.0 && h < 240.0)
        {
            colors[i].r = m;
            colors[i].g = x + m;
            colors[i].b = c + m;
        }
        else if (h >= 240.0 && h < 300.0)
        {
            colors[i].r = x + m;
            colors[i].g = m;
            colors[i].b = c + m;
        }
        else if (h >= 300.0 && h < 360.0)
        {
            colors[i].r = c + m;
            colors[i].g = m;
            colors[i].b = x + m;
        }
        else
        {
            colors[i].r = m;
            colors[i].g = m;
            colors[i].b = m;
        }

        h += fmod((360.0 / n), 360.0);
    }
}

