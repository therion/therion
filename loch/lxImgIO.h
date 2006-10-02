#ifndef lxImgIO_h
#define lxImgIO_h

// Standard libraries
#ifndef LXDEPCHECK
#include <stdlib.h>
#include <stdio.h>
#endif  
//LXDEPCHECK - standart libraries

#ifndef UCHAR
#define UCHAR
typedef unsigned char uchar;
#endif

extern char * lxImgIOError;

// struct for handling images
struct lxImageRGB {
	uchar* data; //pixel data in RGB format. sizeof(data) == 3 * width * height;
	int width, height;
  lxImageRGB() : data(NULL), width(0), height(0) {}
};

/* Compress image into JPEG, and save it to disk
   quality must be in range 0-100 */
bool lxWrite_JPEG_file (const char * filename, int quality, lxImageRGB img);

/* Load and decompress JPEG image from disk */
lxImageRGB lxRead_JPEG_file (const char * filename, FILE * infile = NULL);

void lxImageRGBFree(lxImageRGB & img);

#endif


