#include "data.h"

#include <stdio.h>

int read_gebruik_dat(Gebruik *buf, int max, int *startw, int *eindw)
{
    FILE *f = fopen("gegev.dat", "r");

    if (f == NULL) return -1;

    fread(startw, sizeof(int), 1, f);
    fread(eindw, sizeof(int), 1, f);

    int n = fread(buf, sizeof(Gebruik), max, f);

    fclose(f);

    return n;
}

int sum_uren(Gebruik *buf, int n, int *sums)
{
    int max = 0;

    for (int i = 0; i < WLEN; i++)
    {
        sums[i] = 0;
        for (int j = 0; j < n; j++)
        {
            sums[i] += buf[j].uren[i];
        }
        if (sums[i] > max) max = sums[i];
    }

    return max;
}
