/* img.h
 * Header file for routines to read and write Survex ".3d" image files
 * Copyright (C) Olly Betts 1993,1994,1997,2001,2002
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef IMG_H
# define IMG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

# ifdef IMG_HOSTED
#  include "useful.h"
# endif

# define img_BAD   -2
# define img_STOP  -1
# define img_MOVE   0
# define img_LINE   1
/* NB: img_CROSS is never output and ignored on input.
 * Put crosses where labels are. */
/* # define img_CROSS  2 */
# define img_LABEL  3

# define img_FLAG_SURFACE   0x01
# define img_FLAG_DUPLICATE 0x02
# define img_FLAG_SPLAY     0x04

# define img_SFLAG_SURFACE     0x01
# define img_SFLAG_UNDERGROUND 0x02
# define img_SFLAG_ENTRANCE    0x04
# define img_SFLAG_EXPORTED    0x08
# define img_SFLAG_FIXED       0x10

/* 3D coordinates (in metres) */
typedef struct {
   double x, y, z;
} img_point;

typedef struct {
   /* members you can access when reading (don't touch when writing) */
   char *label;
   int flags;
   char *title;
   char *datestamp;
   char separator; /* charactor used to separate survey levels ('.' usually) */
   /* all other members are for internal use only */
   FILE *fh;          /* file handle of image file */
   char *label_buf;
   size_t buf_len;
   size_t label_len;
# ifdef IMG_HOSTED
   bool fRead;        /* fTrue for reading, fFalse for writing */
# else
   int fRead;        /* fTrue for reading, fFalse for writing */
# endif
   long start;
   /* version of file format:
    *  -4 => CMAP .xyz file, shot format
    *  -3 => CMAP .xyz file, station format
    *  -2 => Compass .plt file
    *  -1 => .pos file
    *   0 => 0.01 ascii
    *   1 => 0.01 binary,
    *   2 => byte actions and flags
    *   3 => prefixes for legs; compressed prefixes
    */
   int version;
   char *survey;
   size_t survey_len;
   int pending; /* for old style text format files and survey filtering */
   img_point mv;
} img;

/* Which version of the file format to output (defaults to newest) */
extern unsigned int img_output_version;

/* Open a .3d file for reading
 * fnm is the filename
 * Returns pointer to an img struct or NULL
 */
#define img_open(F) img_open_survey((F), NULL)

/* Open a .3d file for reading
 * fnm is the filename
 * Returns pointer to an img struct or NULL
 * survey points to a survey name to restrict reading to (or NULL for all
 * survey data in the file)
 */
img *img_open_survey(const char *fnm, const char *survey);

/* Open a .3d file for output
 * fnm is the filename
 * title_buf is the title
 * fBinary is ignored (it used to select an ASCII variant of the earliest
 * version of the 3d file format)
 * Returns pointer to an img struct or NULL for error (check img_error()
 * for details)
 */
# ifdef IMG_HOSTED
img *img_open_write(const char *fnm, char *title_buf, bool fBinary);
# else
img *img_open_write(const char *fnm, char *title_buf, int fBinary);
# endif

/* Read an item from a .3d file
 * pimg is a pointer to an img struct returned by img_open()
 * coordinates are returned in p
 * flags and label name are returned in fields in pimg
 * Returns img_XXXX as #define-d above
 */
int img_read_item(img *pimg, img_point *p);

/* Write a item to a .3d file
 * pimg is a pointer to an img struct returned by img_open_write()
 * code is one of the img_XXXX #define-d above
 * flags is the leg or station flags (meaningful for img_LINE and img_LABEL
 * s is the label (only meaningful for img_LABEL)
 * x, y, z are the coordinates
 */
void img_write_item(img *pimg, int code, int flags, const char *s,
		    double x, double y, double z);

/* rewind a .3d file opened for reading so the data can be read in
 * several passes
 * pimg is a pointer to an img struct returned by img_open()
 * Returns: non-zero for success, zero for error (check img_error() for
 *   details)
 */
int img_rewind(img *pimg);

/* Close a .3d file
 * pimg is a pointer to an img struct returned by img_open() or
 *   img_open_write()
 * Returns: non-zero for success, zero for error (check img_error() for
 *   details)
 */
int img_close(img *pimg);

/* Codes returned by img_error */
# ifndef IMG_HOSTED
typedef enum {
   IMG_NONE = 0, IMG_FILENOTFOUND, IMG_OUTOFMEMORY,
   IMG_CANTOPENOUT, IMG_BADFORMAT, IMG_DIRECTORY,
   IMG_READERROR, IMG_WRITEERROR, IMG_TOONEW
} img_errcode;

/* Read the error code
 * if img_open() or img_open_write() returns NULL, you can call this
 * to discover why */
img_errcode img_error(void);
# else
int img_error(void);
# endif

#ifdef __cplusplus
}
#endif

#endif
