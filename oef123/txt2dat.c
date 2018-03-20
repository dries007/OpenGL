#include <stdio.h>

#include "helpers.h"

int main(int argc, char** argv)
{
    char* input;
    char* output;

    /* Argument handling */
    if (argc == 1)
    {
        input = "gegev.txt";
        output = "gegev.dat";
    }
    else if (argc == 3)
    {
        input = argv[1];
        output = argv[2];
    }
    else
    {
        fprintf(stderr, "Turn text data into binary data suitable for current platform.\n"
            "Usage: %s [input] [output]\n"
            "Default args: gegev.txt gegev.dat\n", argv[0]);
        return 1;
    }

    /* Open file handles */
    FILE* fin = fopen(input, "r");
    if (fin == NULL)
    {
        fprintf(stderr, "Cannot open '%s' to read.\n", input);
        return 1;
    }

    FILE* fout = fopen(output, "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "Cannot open '%s' to write.\n", output);
        fclose(fin);
        return 1;
    }

    /* Do the actual reading-writing */
    int ints[2];
    if (fscanf(fin, "%d %d", ints, ints + 1) != 2)
    {
        fprintf(stderr, "Input file format error.\n");
        fclose(fout);
        fclose(fin);
        return 1;
    }
    fwrite(ints, sizeof(int), 2, fout);

    int n = 0;
    Gebruik tmp;
    while (fscanf(fin, "%s", tmp.naam) == 1)
    {
        for (int i = 0; i < WLEN; i++)
        {
            if (fscanf(fin, "%d", tmp.uren + i) != 1)
            {
                fprintf(stderr, "Input file format error. (struct %d, int %d)\n", n, i);
                fclose(fout);
                fclose(fin);
                return 1;
            }
        }

        fwrite(&tmp, sizeof(tmp), 1, fout);

        n++;
    }

    /* If loop broke and we are not EOF, something is broken */
    if (!feof(fin))
    {
        fprintf(stderr, "Input file format error. (struct %d)\n", n);
        fclose(fout);
        fclose(fin);
        return 1;
    }

    /* Close file handles */
    fclose(fout);
    fclose(fin);

    printf("OK, got %d structs.\n", n);

    return 0;
}
