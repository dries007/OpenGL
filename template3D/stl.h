#ifndef OPENGL_STL_H
#define OPENGL_STL_H

/* Thank you Wikipedia for explaining how this format works. */

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float normal[3];
    float v1[3], v2[3], v3[3];
    uint16_t unused;
} __attribute__((packed)) Facet;

typedef struct {
    char* path;
    uint32_t nrOfFacets;
    Facet* facets;
} Model;

Model* stlLoadFile(char *path);
void stlFreeModel(Model* model);
void stlDisplayModel(Model *model);

#endif
