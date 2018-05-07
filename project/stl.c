#include "stl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#define STL_HEADER_SIZE 80
#define STL_FACET_SIZE (4*3*4+2)

/* Endianness is weird */
float readLittleEndianFloat(char *c)
{
    static int endianTest = 1;
    bool is_be = ((*(char*)&endianTest) == 0);

    float i;
    char *p = (char*)&i;

    if (!is_be)
    {
        p[0] = c[0];
        p[1] = c[1];
        p[2] = c[2];
        p[3] = c[3];
    }
    else
    {
        p[0] = c[3];
        p[1] = c[2];
        p[2] = c[1];
        p[3] = c[0];
    }

    return i;
}

void readLittleEndianFloatVector(float* vector, char* c)
{
    vector[0] = readLittleEndianFloat(c + 0);
    vector[1] = readLittleEndianFloat(c + 4);
    vector[2] = readLittleEndianFloat(c + 8);
}

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
    char buffer[STL_HEADER_SIZE + 1]; /* Biggest of the 2 + 1 for NULL.*/

    if (fread(buffer, STL_HEADER_SIZE, 1, f) != 1)
    {
        fprintf(stderr, "Cannot read file header.\n");
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
        fclose(f);
        return NULL;
    }
    model->path = malloc(strlen(path)+1);
    if (model->path == NULL)
    {
        fprintf(stderr, "Cannot allocate RAM for model path.\n");
        stlFreeModel(model);
        fclose(f);
        return NULL;
    }
    strcpy(model->path, path);

    if (fread(buffer, 4, 1, f) != 1)
    {
        fprintf(stderr, "Cannot read nr of facets.\n");
        stlFreeModel(model);
        fclose(f);
        return NULL;
    }
    printf("0x%08X\n", (unsigned int) buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
    model->nrOfFacets = (unsigned int) buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    model->facets = calloc(model->nrOfFacets, sizeof(Facet));
    if (model->facets == NULL)
    {
        fprintf(stderr, "Cannot allocate RAM for %u facets (%u bytes).\n", model->nrOfFacets, model->nrOfFacets * sizeof(Facet));
        stlFreeModel(model);
        fclose(f);
        return NULL;
    }

    for (unsigned int i = 0; i < model->nrOfFacets; i++)
    {
        if (fread(buffer, STL_FACET_SIZE, 1, f) != 1)
        {
            fprintf(stderr, "Cannot read facet nr %u from file.\n", i);
            stlFreeModel(model);
            fclose(f);
            return NULL;
        }
        readLittleEndianFloatVector(model->facets[i].normal, &buffer[0]);
        readLittleEndianFloatVector(model->facets[i].v1, buffer + 12);
        readLittleEndianFloatVector(model->facets[i].v2, buffer + 24);
        readLittleEndianFloatVector(model->facets[i].v3, buffer + 36);
        // Ignore 2 bytes
    }

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

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model->nrOfFacets; i++)
    {
        Facet f = model->facets[i];
        glVertex3fv(f.v1);
        glVertex3fv(f.v2);
        glVertex3fv(f.v3);
    }
    glEnd();
}

