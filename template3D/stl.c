#include "stl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#define STL_HEADER_SIZE 80

static const char* STL_ASCII_MARKER = "solid ";

Model* stlLoadFile(char *path)
{
    Model * model = NULL;

    FILE* f = fopen(path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file '%s'.\n", path);
        return NULL;
    }
    char* buffer = malloc(STL_HEADER_SIZE+1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Error allocating %d bytes.\n", STL_HEADER_SIZE+1);
        fclose(f);
        return NULL;
    };

    if (fread(buffer, STL_HEADER_SIZE, 1, f) != 1)
    {
        fprintf(stderr, "Cannot read file header.\n");
        free(buffer);
        fclose(f);
        return NULL;
    }
    buffer[STL_HEADER_SIZE] = '\0';

    if (strncmp(STL_ASCII_MARKER, buffer, strlen(STL_ASCII_MARKER)) == 0)
    {
        /* ASCII */
        printf("Loading ASCII STL file '%s'. Sorry no support.\n", path);
        // todo: implement?
        return NULL;
    }

    printf("Loading binary STL file '%s' with header: '%s'\n", path, buffer);

    model = calloc(1, sizeof(Model));
    if (model == NULL)
    {
        fprintf(stderr, "Cannot allocate RAM for model object.\n");
        free(buffer);
        fclose(f);
        return NULL;
    }
    model->path = malloc(strlen(path)+1);
    if (model->path == NULL)
    {
        fprintf(stderr, "Cannot allocate RAM for model path.\n");
        stlFreeModel(model);
        free(buffer);
        fclose(f);
        return NULL;
    }
    strcpy(model->path, path);

    if (fread(buffer, 4, 1, f) != 1)
    {
        fprintf(stderr, "Cannot read nr of facets.\n");
        stlFreeModel(model);
        free(buffer);
        fclose(f);
        return NULL;
    }
    model->nrOfFacets = (uint32_t) (buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
    printf("%u facets\n", model->nrOfFacets);
    model->facets = calloc(model->nrOfFacets, sizeof(Facet));
    if (model->facets == NULL)
    {
        fprintf(stderr, "Cannot allocate RAM for %u facets (%u bytes).\n", model->nrOfFacets, model->nrOfFacets * sizeof(Facet));
        stlFreeModel(model);
        free(buffer);
        fclose(f);
        return NULL;
    }

    if (fread(model->facets, sizeof(Facet), model->nrOfFacets, f) != model->nrOfFacets)
    {
        fprintf(stderr, "Cannot read %u facets (%u bytes) from file.\n", model->nrOfFacets, model->nrOfFacets * sizeof(Facet));
        stlFreeModel(model);
        free(buffer);
        fclose(f);
        return NULL;
    }

    free(buffer);
    fclose(f);
    return model;
}

void stlFreeModel(Model* model)
{
    if (model == NULL) return;
    free(model->path);
    free(model->facets);
    free(model);
}

void stlDisplayModel(Model *model)
{
    if (model == NULL) return;

//    glBegin(GL_TRIANGLES);
    glBegin(GL_POINTS);
    for (int i = 0; i < model->nrOfFacets; i++)
    {
        Facet f = model->facets[i];
        glVertex3fv(f.v1);
        glVertex3fv(f.v2);
        glVertex3fv(f.v3);
    }
    glEnd();
}

