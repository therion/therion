#include <X11/Xlib.h>
#include <Imlib2.h>
#include <stdio.h>
#include <string.h>

#define FLAG_RGB 0x1
#define FLAG_ASCII 0x2
#define FLAG_INFO 0x4

int parse(int argc, char **argv, int *flags) {
  int f;

  if (argc < 2) return -1;

  for (f = 1; f < argc; f++) {
    if (strcmp(argv[f], "-ascii") == 0) *flags |= FLAG_ASCII;
    else if (strcmp(argv[f], "-binary") == 0) *flags &= ~FLAG_ASCII;
    else if (strcmp(argv[f], "-rgb") == 0) *flags |= FLAG_RGB;
    else if (strcmp(argv[f], "-rgba") == 0) *flags &= ~FLAG_RGB;
    else if (strcmp(argv[f], "-info") == 0) *flags |= FLAG_INFO;
    else break;
  }

  return (f == argc)? -1 : f;
}

int main(int argc, char **argv) {
  Imlib_Image image;
  DATA32 *data;
  int f, flags = 0;
  int x, y, width, height, offset;
  const char *fmt;

  if ((f = parse(argc, argv, &flags)) == -1) exit(-1);
  image = imlib_load_image(argv[f]);
  if (image != 0) {
    if (flags & FLAG_ASCII) fmt = (flags & FLAG_RGB)? "%d %d %d\n" : "%d %d %d %d\n";
    else fmt = (flags & FLAG_RGB)? "%c%c%c" : "%c%c%c%c";
    imlib_context_set_image(image);
    width = imlib_image_get_width();
    height = imlib_image_get_height();
    if (flags & FLAG_INFO) {
      printf("%d %d %d", width, height, ((flags & FLAG_RGB) == 0));
    } else {
      data = imlib_image_get_data_for_reading_only();
      for (y = height - 1; y >= 0; y--) {
	offset = y * width;
	for (x = 0; x < width; x++) {
	  printf(fmt,
		 ((*(data + offset)) >> 16) & 0xff,
		 ((*(data + offset)) >> 8) & 0xff,
		 ((*(data + offset)) >> 0) & 0xff,
		 ((*(data + offset)) >> 24) & 0xff);
	  offset++;
	}
      }
      imlib_free_image();
    }
  }
  
  return 0;
}
