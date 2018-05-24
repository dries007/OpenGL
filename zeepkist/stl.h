#ifndef OPENGL_STL_H
#define OPENGL_STL_H

/* Thank you Wikipedia for explaining how this format works. */

/**
 *  WARNING
 *  HERE BE DRAGONS
 */

#include "globals.h"

typedef struct {
    float normal[3];
    float v1[3], v2[3], v3[3];
} Facet;

typedef struct {
    char* path;
    unsigned int nrOfFacets;
    Facet* facets;
} Model;

Model* stlLoadFile(char *path);
void stlFreeModel(Model* model);
void stlDisplayModel(Model *model);

#endif
