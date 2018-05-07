#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>										// Header File For The OpenGL32 Library
#include <GL/glu.h>										// Header File For The GLu32 Library

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#include "jpeglib.h"

/////// * /////////// * ///////// * NEW * /////// * /////////// * /////////// *

#define SCREEN_WIDTH 800				// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600				// We want our screen height 600 pixels
#define SCREEN_DEPTH 16					// We want 16 bits per pixel

#define MAX_TEXTURES 1			// This says how many texture we will be using


extern GLuint g_Texture[MAX_TEXTURES];		// This is our texture data array


// This structure below was added.  This is in
// no way connected to the jpeg library and can be deleted or changed.

// This stores the important jpeg data
typedef struct flippo
	{
		int rowSpan;
		int sizeX;
		int sizeY;
		unsigned char *data;
	} tImageJPG;



//// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This loads a jpeg file and returns it's data
tImageJPG *LoadJPG(const char *filename);

///// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This loads a texture into OpenGL from a file (IE, "image.jpg")
void CreateTexture(GLuint textureArray[], char * strFileName, int textureID);

#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this version we included the jpeglib.h, as well as placed a prototype for:
// 
// tImageJPG *LoadJPG(const char *filename);
//
// This is our cool function that loads the jpeg, then returns it's data.  
// Here is the info for the jpeg library:
//
// * Copyright (C) 1991-1998, Thomas G. Lane.
// * The jpeglib.h and jpeg.lib files are part of the Independent JPEG Group's software.
// * For conditions of distribution and use, visit the website at: http://www.ijg.org/
//
// Note, that I added my own structure for the jpeg info, tImageJPG.  This is not
// part of the library.  I also combined the jconfig.h and jmoreconfig.h into jpeglib.h
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
