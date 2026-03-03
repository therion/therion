#ifndef lxImgIO_h
#define lxImgIO_h

#include <stdlib.h>
#include <stdio.h>
#include <vector>

extern const char * lxImgIOError;

// struct for handling images
struct lxImageRGB {
	std::vector<unsigned char> data; //pixel data in RGB format. sizeof(data) == 3 * width * height;
	int width = 0, height = 0;
	lxImageRGB() = default;
	lxImageRGB(int width, int height) : data(3UL * width * height), width(width), height(height) {}
};

/* Compress image into JPEG, and save it to disk
   quality must be in range 0-100 */
bool lxWrite_JPEG_file (const char * filename, int quality, lxImageRGB img);

/* Load and decompress JPEG image from disk */
lxImageRGB lxRead_JPEG_file (const char * filename, FILE * infile = NULL);

#endif


