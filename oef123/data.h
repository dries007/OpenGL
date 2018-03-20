#ifndef OPENGL_DATA_H
#define OPENGL_DATA_H

#define AANT 20
#define NLEN 11
#define WLEN 53

typedef struct gebruik
{
    char naam[NLEN];
    int uren[WLEN];
} Gebruik;

int read_gebruik_dat(Gebruik *buf, int max, int *startw, int *eindw);
int sum_uren(Gebruik *buf, int n, int *sums);

#endif
