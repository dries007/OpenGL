#include "textures.h"

/**
 * Contains texture helpers
 * Mostly based off this:
 *
 * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
 */


uint textures_load(const char *filename)
{
    /* Data read from the header of the BMP file */
    unsigned char header[54]; /* Each BMP file begins by a 54-bytes header */
    unsigned int dataPos;     /* Position in the file where the actual data begins */
    unsigned int width, height;
    unsigned int imageSize;   /* = width*height*3 */
    /* Actual RGB data */
    unsigned char *data;

    /* Open the file */
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fputs("Image could not be opened\n", stderr);
        return 0;
    }

    if (fread(header, 1, 54, file) != 54) /* If not 54 bytes read : problem */
    {
        printf("Not a correct BMP file\n");
        return 0;
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Not a correct BMP file\n");
        return 0;
    }

    /* Read ints from the byte array. Use byte shift because of school's 'mainframe' endianness is wrong */
    dataPos = (unsigned int) header[0x0A] | (header[0x0A+1] << 8) | (header[0x0A+2] << 16) | (header[0x0A+3] << 24);
    imageSize = (unsigned int) header[0x22] | (header[0x22+1] << 8) | (header[0x22+2] << 16) | (header[0x22+3] << 24);
    width = (unsigned int) header[0x12] | (header[0x12+1] << 8) | (header[0x12+2] << 16) | (header[0x12+3] << 24);
    height = (unsigned int) header[0x16] | (header[0x16+1] << 8) | (header[0x16+2] << 16) | (header[0x16+3] << 24);

	printf("Texture WxH: %dx%d\n", width, height);

    /* Some BMP files are misformatted, guess missing information */
    if (imageSize==0)    imageSize=width*height*3; /* 3 : one byte for each Red, Green and Blue component */
    if (dataPos==0)      dataPos=54; /* The BMP header is done that way */
    /* Create a buffer */
    data = malloc(imageSize);
    if (data == NULL)
    {
        fclose(file);
        fputs("Image buffer could not be allocated\n", stderr);
        return 0;
    }

    fseek(file, dataPos, SEEK_SET);
    /* Read the actual data from the file into the buffer */
    if (fread(data, 1, imageSize, file) != imageSize)
    {
        fclose(file);
        free(data);
        fputs("Image buffer could not be fully read\n", stderr);
        return 0;
    }

    /* Everything is in memory now, the file can be closed */
    fclose(file);

    /* Create one OpenGL texture */
    GLuint textureID;
    glGenTextures(1, &textureID);

    /* "Bind" the newly created texture : all future texture functions will modify this texture */
    glBindTexture(GL_TEXTURE_2D, textureID);

    /* Give the image to OpenGL */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    /* Cleanup */
    free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    printf("TextureID for %s: %d\n", filename, textureID);

    return textureID;
}
