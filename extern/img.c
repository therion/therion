/* img.c
 * Routines for reading and writing Survex ".3d" image files
 * Copyright (C) 1993-2003 Olly Betts
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "img.h"

#ifdef IMG_HOSTED
# include "debug.h"
# include "filelist.h"
# include "filename.h"
# include "message.h"
# include "useful.h"
# define TIMENA msg(/*Date and time not available.*/108)
# define TIMEFMT msg(/*%a,%Y.%m.%d %H:%M:%S %Z*/107)
#else
# define INT32_T int
# define TIMENA "Time not available."
# define TIMEFMT "%a,%Y.%m.%d %H:%M:%S %Z"
# define EXT_SVX_3D "3d"
# define EXT_SVX_POS "pos"
# define FNM_SEP_EXT '.'
# define METRES_PER_FOOT 0.3048 /* exact value */
# define xosmalloc(L) malloc((L))
# define xosrealloc(L,S) realloc((L),(S))
# define osfree(P) free((P))
# define ossizeof(T) sizeof(T)
/* in IMG_HOSTED mode, this tests if a filename refers to a directory */
# define fDirectory(X) 0
/* open file FNM with mode MODE, maybe using path PTH and/or extension EXT */
/* path isn't used in img.c, but EXT is */
# define fopenWithPthAndExt(PTH,FNM,EXT,MODE,X) fopen(FNM,MODE)
# define fFalse 0
# define fTrue 1
# define bool int
/* dummy do {...} while(0) hack to permit stuff like
 * if (s) fputsnl(s,fh); else exit(1);
 * to work as intended
 */
# define fputsnl(S, FH) do {fputs((S), (FH)); putc('\n', (FH));} while(0)
# define SVX_ASSERT(X)

static INT32_T
get32(FILE *fh)
{
   INT32_T w = getc(fh);
   w |= (INT32_T)getc(fh) << 8l;
   w |= (INT32_T)getc(fh) << 16l;
   w |= (INT32_T)getc(fh) << 24l;
   return w;
}

static void
put32(long w, FILE *fh)
{
   putc((char)(w), fh);
   putc((char)(w >> 8l), fh);
   putc((char)(w >> 16l), fh);
   putc((char)(w >> 24l), fh);
}

static short
get16(FILE *fh)
{
   short w = getc(fh);
   w |= (long)getc(fh) << 8l;
   return w;
}

static void
put16(short w, FILE *fh)
{
   putc((char)(w), fh);
   putc((char)(w >> 8l), fh);
}

static char *
baseleaf_from_fnm(const char *fnm)
{
   const char *p;
   char *q;
   size_t len;

   p = fnm;
   q = strrchr(p, '/');
   if (q) p = q + 1;
   q = strrchr(p, '\\');
   if (q) p = q + 1;

   q = strrchr(p, FNM_SEP_EXT);
   if (q) len = (const char *)q - p; else len = strlen(p);

   q = xosmalloc(len + 1);
   if (!q) return NULL;
   memcpy(q, p, len);
   q[len] = '\0';
   return q;
}
#endif

#include <math.h>
#ifdef HAVE_ROUND
extern double round(double); /* prototype is often missing... */
# define my_round round
#else
static double
my_round(double x) {
   if (x >= 0.0) return floor(x + 0.5);
   return ceil(x - 0.5);
}
#endif

/* portable case insensitive string compare */
#if defined(strcasecmp) || defined(HAVE_STRCASECMP)
# define my_strcasecmp strcasecmp
#else
/* What about top bit set chars? */
int my_strcasecmp(const char *s1, const char *s2) {
   register int c1, c2;
   do {
      c1 = *s1++;
      c2 = *s2++;
   } while (c1 && toupper(c1) == toupper(c2));
   /* now calculate real difference */
   return c1 - c2;
}
#endif

unsigned int img_output_version = 3;

#ifdef IMG_HOSTED
static enum {
   IMG_NONE = 0,
   IMG_FILENOTFOUND = /*Couldn't open data file `%s'*/24,
   IMG_OUTOFMEMORY  = /*Out of memory %.0s*/38,
   IMG_DIRECTORY    = /*Filename `%s' refers to directory*/44,
   IMG_CANTOPENOUT  = /*Failed to open output file `%s'*/47,
   IMG_BADFORMAT    = /*Bad 3d image file `%s'*/106,
   IMG_READERROR    = /*Error reading from file `%s'*/109,
   IMG_WRITEERROR   = /*Error writing to file `%s'*/110,
   IMG_TOONEW       = /*File `%s' has a newer format than this program can understand*/114
} img_errno = IMG_NONE;
#else
static img_errcode img_errno = IMG_NONE;
#endif

#define FILEID "Survex 3D Image File"

#define EXT_PLT "plt"
#define EXT_PLF "plf"
#define EXT_XYZ "xyz"

/* Attempt to string paste to ensure we are passed a literal string */
#define LITLEN(S) (sizeof(S"") - 1)

static char *
my_strdup(const char *str)
{
   char *p;
   size_t len = strlen(str) + 1;
   p = xosmalloc(len);
   if (p) memcpy(p, str, len);
   return p;
}

static char *
getline_alloc(FILE *fh)
{
   int ch;
   size_t i = 0;
   size_t len = 16;
   char *buf = xosmalloc(len);
   if (!buf) return NULL;

   ch = getc(fh);
   while (ch != '\n' && ch != '\r' && ch != EOF) {
      buf[i++] = ch;
      if (i == len - 1) {
	 char *p;
	 len += len;
	 p = xosrealloc(buf, len);
	 if (!p) {
	    osfree(buf);
	    return NULL;
	 }
	 buf = p;
      }
      ch = getc(fh);
   }
   if (ch == '\n' || ch == '\r') {
      int otherone = ch ^ ('\n' ^ '\r');
      ch = getc(fh);
      /* if it's not the other eol character, put it back */
      if (ch != otherone) ungetc(ch, fh);
   }
   buf[i++] = '\0';
   return buf;
}

#ifndef IMG_HOSTED
img_errcode
img_error(void)
{
   return img_errno;
}
#else
int
img_error(void)
{
   return (int)img_errno;
}
#endif

static bool
check_label_space(img *pimg, size_t len)
{
   if (len > pimg->buf_len) {
      char *b = xosrealloc(pimg->label_buf, len);
      if (!b) return fFalse;
      pimg->label = (pimg->label - pimg->label_buf) + b;
      pimg->label_buf = b;
      pimg->buf_len = len;
   }
   return fTrue;
}

#define has_ext(F,L,E) ((L) > LITLEN(E) + 1 &&\
			(F)[(L) - LITLEN(E) - 1] == FNM_SEP_EXT &&\
			my_strcasecmp((F) + (L) - LITLEN(E), E) == 0)

img *
img_open_survey(const char *fnm, const char *survey)
{
   img *pimg;
   size_t len;
   char buf[LITLEN(FILEID) + 9];
   int ch;

   if (fDirectory(fnm)) {
      img_errno = IMG_DIRECTORY;
      return NULL;
   }

   pimg = (img *)xosmalloc(ossizeof(img));
   if (pimg == NULL) {
      img_errno = IMG_OUTOFMEMORY;
      return NULL;
   }

   pimg->buf_len = 257;
   pimg->label_buf = xosmalloc(pimg->buf_len);
   if (!pimg->label_buf) {
      osfree(pimg);
      img_errno = IMG_OUTOFMEMORY;
      return NULL;
   }

   pimg->fh = fopenWithPthAndExt("", fnm, EXT_SVX_3D, "rb", NULL);
   if (pimg->fh == NULL) {
      osfree(pimg->label_buf);
      osfree(pimg);
      img_errno = IMG_FILENOTFOUND;
      return NULL;
   }

   pimg->fRead = fTrue; /* reading from this file */
   img_errno = IMG_NONE;

   pimg->flags = 0;

   /* for version 3 we use label_buf to store the prefix for reuse */
   /* for version -2, 0 value indicates we haven't entered a survey yet */
   /* for version -4, we store the last station here to detect whether
    * we MOVE or LINE */
   pimg->label_len = 0;
   pimg->label_buf[0] = '\0';

   pimg->survey = NULL;
   pimg->survey_len = 0;
   pimg->separator = '.';

   pimg->title = pimg->datestamp = NULL;
   if (survey) {
      len = strlen(survey);
      if (len) {
	 if (survey[len - 1] == '.') len--;
	 if (len) {
	    char *p;
	    pimg->survey = xosmalloc(len + 2);
	    if (!pimg->survey) {
	       img_errno = IMG_OUTOFMEMORY;
	       goto error;
	    }
	    memcpy(pimg->survey, survey, len);
	    /* Set title to leaf survey name */
	    pimg->survey[len] = '\0';
	    p = strchr(pimg->survey, '.');
	    if (p) p++; else p = pimg->survey;
	    pimg->title = my_strdup(p);
	    if (!pimg->title) {
	       img_errno = IMG_OUTOFMEMORY;
	       goto error;
	    }
	    pimg->survey[len] = '.';
	    pimg->survey[len + 1] = '\0';
	 }
      }
      pimg->survey_len = len;
   }

   /* [version -2, -3, -4] pending IMG_LINE or IMG_MOVE - both have 4 added
    * [version -1] already skipped heading line, or there wasn't one
    * [version 0] not in the middle of a 'LINE' command
    * [version 3] not in the middle of turning a LINE into a MOVE
    */
   pimg->pending = 0;

   len = strlen(fnm);
   if (has_ext(fnm, len, EXT_SVX_POS)) {
pos_file:
      pimg->version = -1;
      if (!pimg->survey) pimg->title = baseleaf_from_fnm(fnm);
      pimg->datestamp = my_strdup(TIMENA);
      if (!pimg->datestamp) {
	 img_errno = IMG_OUTOFMEMORY;
	 goto error;
      }
      pimg->start = 0;
      return pimg;
   }

   if (has_ext(fnm, len, EXT_PLT) || has_ext(fnm, len, EXT_PLF)) {
      long fpos;
plt_file:
      pimg->version = -2;
      /* Spaces aren't legal in Compass station names, but dots are, so
       * use space as the level separator */
      pimg->separator = ' ';
      pimg->start = 0;
      if (!pimg->survey) pimg->title = baseleaf_from_fnm(fnm);
      pimg->datestamp = my_strdup(TIMENA);
      if (!pimg->datestamp) {
	 img_errno = IMG_OUTOFMEMORY;
	 goto error;
      }
      while (1) {
	 ch = getc(pimg->fh);
	 switch (ch) {
	  case '\x1a':
	    fseek(pimg->fh, -1, SEEK_CUR);
	    /* FALL THRU */
	  case EOF:
	    pimg->start = ftell(pimg->fh);
	    return pimg;
	  case 'N': {
	    char *line, *q;
	    fpos = ftell(pimg->fh) - 1;
	    if (!pimg->survey) {
	       /* FIXME : if there's only one survey in the file, it'd be nice
		* to use its description as the title here...
		*/
	       ungetc('N', pimg->fh);
	       pimg->start = fpos;
	       return pimg;
	    }
	    line = getline_alloc(pimg->fh);
	    if (!line) {
	       img_errno = IMG_OUTOFMEMORY;
	       goto error;
	    }
	    len = 0;
	    while (line[len] > 32) ++len;
	    if (pimg->survey_len != len ||
		memcmp(line, pimg->survey, len) != 0) {
	       osfree(line);
	       continue;
	    }
	    q = strchr(line + len, 'C');
	    if (q && q[1]) {
		osfree(pimg->title);
		pimg->title = my_strdup(q + 1);
	    } else if (!pimg->title) {
		pimg->title = my_strdup(pimg->label);
	    }
	    osfree(line);
	    if (!pimg->title) {
		img_errno = IMG_OUTOFMEMORY;
		goto error;
	    }
	    if (!pimg->start) pimg->start = fpos;
	    fseek(pimg->fh, pimg->start, SEEK_SET);
	    return pimg;
	  }
	  case 'M': case 'D':
	    pimg->start = ftell(pimg->fh) - 1;
	    break;
	 }
	 while (ch != '\n' && ch != '\r') {
	    ch = getc(pimg->fh);
	 }
      }
   }

   if (has_ext(fnm, len, EXT_XYZ)) {
      char *line;
xyz_file:
      /* Spaces aren't legal in CMAP station names, but dots are, so
       * use space as the level separator */
      pimg->separator = ' ';
      line = getline_alloc(pimg->fh);
      if (!line) {
	 img_errno = IMG_OUTOFMEMORY;
	 goto error;
      }
      /* FIXME: reparse date? */
      len = strlen(line);
      if (len > 59) line[59] = '\0';
      if (len > 45) {
	 pimg->datestamp = my_strdup(line + 45);
      } else {
	 pimg->datestamp = my_strdup(TIMENA);
      }
      if (strncmp(line, "  Cave Survey Data Processed by CMAP ",
		  LITLEN("  Cave Survey Data Processed by CMAP ")) == 0) {
	 len = 0;
      } else {
	 if (len > 45) {
	    line[45] = '\0';
	    len = 45;
	 }
	 while (len > 2 && line[len - 1] == ' ') --len;
	 if (len > 2) {
	    line[len] = '\0';
	    pimg->title = my_strdup(line + 2);
	 }
      }
      if (len <= 2) pimg->title = baseleaf_from_fnm(fnm);
      osfree(line);
      if (!pimg->datestamp || !pimg->title) {
	 img_errno = IMG_OUTOFMEMORY;
	 goto error;
      }
      line = getline_alloc(pimg->fh);
      if (!line) {
	 img_errno = IMG_OUTOFMEMORY;
	 goto error;
      }
      if (line[0] != ' ' || (line[1] != 'S' && line[1] != 'O')) {
	 img_errno = IMG_BADFORMAT;
	 goto error;
      }
      if (line[1] == 'S') {
	 pimg->version = -3; /* Station format */
      } else {
	 pimg->version = -4; /* Shot format */
      }
      osfree(line);
      line = getline_alloc(pimg->fh);
      if (!line) {
	 img_errno = IMG_OUTOFMEMORY;
	 goto error;
      }
      if (line[0] != ' ' || line[1] != '-') {
	 img_errno = IMG_BADFORMAT;
	 goto error;
      }
      osfree(line);
      pimg->start = ftell(pimg->fh);
      return pimg;
   }

   if (fread(buf, LITLEN(FILEID) + 1, 1, pimg->fh) != 1 ||
       memcmp(buf, FILEID"\n", LITLEN(FILEID) + 1) != 0) {
      if (fread(buf + LITLEN(FILEID) + 1, 8, 1, pimg->fh) == 1 &&
	  memcmp(buf, FILEID"\r\nv0.01\r\n", LITLEN(FILEID) + 9) == 0) {
	 /* v0 3d file with DOS EOLs */
	 pimg->version = 0;
	 goto v03d;
      }
      rewind(pimg->fh);
      if (buf[1] == ' ') {
	 if (buf[0] == ' ') {
	    /* Looks like a CMAP .xyz file ... */
	    goto xyz_file;
	 } else if (strchr("ZSNF", buf[0])) {
	    /* Looks like a Compass .plt file ... */
	    /* Almost certainly it'll start "Z " */
	    goto plt_file;
	 }
      }
      if (buf[0] == '(') {
	 /* Looks like a Survex .pos file ... */
	 goto pos_file;
      }
      img_errno = IMG_BADFORMAT;
      goto error;
   }

   /* check file format version */
   ch = getc(pimg->fh);
   pimg->version = 0;
   if (tolower(ch) == 'b') {
      /* binary file iff B/b prefix */
      pimg->version = 1;
      ch = getc(pimg->fh);
   }
   if (ch != 'v') {
      img_errno = IMG_BADFORMAT;
      goto error;
   }
   ch = getc(pimg->fh);
   if (ch == '0') {
      if (fread(buf, 4, 1, pimg->fh) != 1 || memcmp(buf, ".01\n", 4) != 0) {
	 img_errno = IMG_BADFORMAT;
	 goto error;
      }
      /* nothing special to do */
   } else if (pimg->version == 0) {
      if (ch < '2' || ch > '3' || getc(pimg->fh) != '\n') {
	 img_errno = IMG_TOONEW;
	 goto error;
      }
      pimg->version = ch - '0';
   } else {
      img_errno = IMG_BADFORMAT;
      goto error;
   }

v03d:
   if (!pimg->title)
       pimg->title = getline_alloc(pimg->fh);
   else
       osfree(getline_alloc(pimg->fh));
   pimg->datestamp = getline_alloc(pimg->fh);
   if (!pimg->title || !pimg->datestamp) {
      img_errno = IMG_OUTOFMEMORY;
      error:
      osfree(pimg->title);
      osfree(pimg->datestamp);
      fclose(pimg->fh);
      osfree(pimg);
      return NULL;
   }

   pimg->start = ftell(pimg->fh);

   return pimg;
}

int
img_rewind(img *pimg)
{
   if (!pimg->fRead) {
      img_errno = IMG_WRITEERROR;
      return 0;
   }
   if (fseek(pimg->fh, pimg->start, SEEK_SET) != 0) {
      img_errno = IMG_READERROR;
      return 0;
   }
   clearerr(pimg->fh);
   /* [version -1] already skipped heading line, or there wasn't one
    * [version 0] not in the middle of a 'LINE' command
    * [version 3] not in the middle of turning a LINE into a MOVE */
   pimg->pending = 0;

   img_errno = IMG_NONE;

   /* for version 3 we use label_buf to store the prefix for reuse */
   /* for version -2, 0 value indicates we haven't entered a survey yet */
   /* for version -4, we store the last station here to detect whether
    * we MOVE or LINE */
   pimg->label_len = 0;
   return 1;
}

img *
img_open_write(const char *fnm, char *title_buf, bool fBinary)
{
   time_t tm;
   img *pimg;

   fBinary = fBinary;

   if (fDirectory(fnm)) {
      img_errno = IMG_DIRECTORY;
      return NULL;
   }

   pimg = (img *)xosmalloc(ossizeof(img));
   if (pimg == NULL) {
      img_errno = IMG_OUTOFMEMORY;
      return NULL;
   }

   pimg->buf_len = 257;
   pimg->label_buf = xosmalloc(pimg->buf_len);
   if (!pimg->label_buf) {
      osfree(pimg);
      img_errno = IMG_OUTOFMEMORY;
      return NULL;
   }

   pimg->fh = fopen(fnm, "wb");
   if (!pimg->fh) {
      osfree(pimg->label_buf);
      osfree(pimg);
      img_errno = IMG_CANTOPENOUT;
      return NULL;
   }

   /* Output image file header */
   fputs("Survex 3D Image File\n", pimg->fh); /* file identifier string */
   if (img_output_version < 2) {
      pimg->version = 1;
      fputs("Bv0.01\n", pimg->fh); /* binary file format version number */
   } else {
      pimg->version = (img_output_version > 2) ? 3 : 2;
      fprintf(pimg->fh, "v%d\n", pimg->version); /* file format version no. */
   }
   fputsnl(title_buf, pimg->fh);
   tm = time(NULL);
   if (tm == (time_t)-1) {
      fputsnl(TIMENA, pimg->fh);
   } else {
      char date[256];
      /* output current date and time in format specified */
      strftime(date, 256, TIMEFMT, localtime(&tm));
      fputsnl(date, pimg->fh);
   }
   pimg->fRead = fFalse; /* writing to this file */
   img_errno = IMG_NONE;

   /* for version 3 we use label_buf to store the prefix for reuse */
   pimg->label_buf[0] = '\0';
   pimg->label_len = 0;

   /* Don't check for write errors now - let img_close() report them... */
   return pimg;
}

static void
read_xyz_station_coords(img_point *pt, const char *line)
{
   char num[12];
   memcpy(num, line + 6, 9);
   num[9] = '\0';
   pt->x = atof(num) / METRES_PER_FOOT;
   memcpy(num, line + 15, 9);
   pt->y = atof(num) / METRES_PER_FOOT;
   memcpy(num, line + 24, 8);
   num[8] = '\0';
   pt->z = atof(num) / METRES_PER_FOOT;
}

static void
read_xyz_shot_coords(img_point *pt, const char *line)
{
   char num[12];
   memcpy(num, line + 40, 10);
   num[10] = '\0';
   pt->x = atof(num) / METRES_PER_FOOT;
   memcpy(num, line + 50, 10);
   pt->y = atof(num) / METRES_PER_FOOT;
   memcpy(num, line + 60, 9);
   num[9] = '\0';
   pt->z = atof(num) / METRES_PER_FOOT;
}

static void
subtract_xyz_shot_deltas(img_point *pt, const char *line)
{
   char num[12];
   memcpy(num, line + 15, 9);
   num[9] = '\0';
   pt->x -= atof(num) / METRES_PER_FOOT;
   memcpy(num, line + 24, 8);
   num[8] = '\0';
   pt->y -= atof(num) / METRES_PER_FOOT;
   memcpy(num, line + 32, 8);
   pt->z -= atof(num) / METRES_PER_FOOT;
}

static int
read_coord(FILE *fh, img_point *pt)
{
   SVX_ASSERT(fh);
   SVX_ASSERT(pt);
   pt->x = get32(fh) / 100.0;
   pt->y = get32(fh) / 100.0;
   pt->z = get32(fh) / 100.0;
   if (ferror(fh) || feof(fh)) {
      img_errno = feof(fh) ? IMG_BADFORMAT : IMG_READERROR;
      return 0;
   }
   return 1;
}

static int
skip_coord(FILE *fh)
{
    return (fseek(fh, 12, SEEK_CUR) == 0);
}

int
img_read_item(img *pimg, img_point *p)
{
   int result;
   pimg->flags = 0;

   if (pimg->version == 3) {
      int opt;
      if (pimg->pending >= 0x80) {
	 *p = pimg->mv;
	 pimg->flags = (int)(pimg->pending) & 0x3f;
	 pimg->pending = 0;
	 return img_LINE;
      }
      again3: /* label to goto if we get a prefix */
      pimg->label = pimg->label_buf;
      opt = getc(pimg->fh);
      if (opt == EOF) {
	 img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	 return img_BAD;
      }
      switch (opt >> 6) {
       case 0:
	 if (opt == 0) {
	    if (!pimg->label_len) return img_STOP; /* end of data marker */
	    pimg->label_len = 0;
	    goto again3;
	 }
	 if (opt < 15) {
	    /* 1-14 mean trim that many levels from current prefix */
	    int c;
	    if (pimg->label_len <= 16) {
	       /* zero prefix using "0" */
	       img_errno = IMG_BADFORMAT;
	       return img_BAD;
	    }
	    c = pimg->label_len - 16 - 1;
	    while (pimg->label_buf[c] != '.' || --opt > 0) {
	       if (--c < 0) {
		  /* zero prefix using "0" */
		  img_errno = IMG_BADFORMAT;
		  return img_BAD;
	       }
	    }
	    c++;
	    pimg->label_len = c;
	    goto again3;
	 }
	 if (opt == 15) {
	    result = img_MOVE;
	    break;
	 }
	 /* 16-31 mean remove (n - 15) characters from the prefix */
	 /* zero prefix using 0 */
	 if (pimg->label_len <= (size_t)(opt - 15)) {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 pimg->label_len -= (opt - 15);
	 goto again3;
       case 1: case 2: {
	 char *q;
	 long len = getc(pimg->fh);
	 if (len == EOF) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 if (len == 0xfe) {
	    len += get16(pimg->fh);
	    if (feof(pimg->fh)) {
	       img_errno = IMG_BADFORMAT;
	       return img_BAD;
	    }
	    if (ferror(pimg->fh)) {
	       img_errno = IMG_READERROR;
	       return img_BAD;
	    }
	 } else if (len == 0xff) {
	    len = get32(pimg->fh);
	    if (ferror(pimg->fh)) {
	       img_errno = IMG_READERROR;
	       return img_BAD;
	    }
	    if (feof(pimg->fh) || len < 0xfe + 0xffff) {
	       img_errno = IMG_BADFORMAT;
	       return img_BAD;
	    }
	 }

	 if (!check_label_space(pimg, pimg->label_len + len + 1)) {
	    img_errno = IMG_OUTOFMEMORY;
	    return img_BAD;
	 }
	 q = pimg->label_buf + pimg->label_len;
	 pimg->label_len += len;
	 if (len && fread(q, len, 1, pimg->fh) != 1) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 q[len] = '\0';

	 result = opt & 0x40 ? img_LABEL : img_LINE;

	 if (pimg->survey_len) {
	    size_t l = pimg->survey_len;
	    const char *s = pimg->label_buf;
	    if (result == img_LINE) {
	       if (strncmp(pimg->survey, s, l) != 0 ||
		   !(s[l] == '.' || s[l] == '\0')) {
		  if (!read_coord(pimg->fh, &(pimg->mv))) return img_BAD;
		  pimg->pending = 15;
		  goto again3;
	       }
	    } else {
	       if (strncmp(pimg->survey, s, l + 1) != 0) {
		  if (!skip_coord(pimg->fh)) return img_BAD;
		  pimg->pending = 0;
		  goto again3;
	       }
	    }
	    pimg->label += l;
	    /* skip the dot if there */
	    if (*pimg->label) pimg->label++;
	 }

	 if (result == img_LINE && pimg->pending) {
	    *p = pimg->mv;
	    if (!read_coord(pimg->fh, &(pimg->mv))) return img_BAD;
	    pimg->pending = opt;
	    return img_MOVE;
	 }
	 pimg->flags = (int)opt & 0x3f;
	 break;
       }
       default:
	 img_errno = IMG_BADFORMAT;
	 return img_BAD;
      }
      if (!read_coord(pimg->fh, p)) return img_BAD;
      pimg->pending = 0;
      return result;
   }

   pimg->label = pimg->label_buf;

   if (pimg->version > 0) {
      static long opt_lookahead = 0;
      static img_point pt = { 0.0, 0.0, 0.0 };
      long opt;
      again: /* label to goto if we get a cross */
      pimg->label[0] = '\0';

      if (pimg->version == 1) {
	 if (opt_lookahead) {
	    opt = opt_lookahead;
	    opt_lookahead = 0;
	 } else {
	    opt = get32(pimg->fh);
	 }
      } else {
	 opt = getc(pimg->fh);
      }

      if (feof(pimg->fh)) {
	 img_errno = IMG_BADFORMAT;
	 return img_BAD;
      }
      if (ferror(pimg->fh)) {
	 img_errno = IMG_READERROR;
	 return img_BAD;
      }

      switch (opt) {
       case -1: case 0:
	 return img_STOP; /* end of data marker */
       case 1:
	 /* skip coordinates */
	 if (!skip_coord(pimg->fh)) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 goto again;
       case 2: case 3: {
	 char *q;
	 int ch;
	 result = img_LABEL;
	 ch = getc(pimg->fh);
	 if (ch == EOF) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 if (ch != '\\') ungetc(ch, pimg->fh);
	 fgets(pimg->label_buf, 257, pimg->fh);
	 if (feof(pimg->fh)) {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 if (ferror(pimg->fh)) {
	    img_errno = IMG_READERROR;
	    return img_BAD;
	 }
	 q = pimg->label_buf + strlen(pimg->label_buf) - 1;
	 if (*q != '\n') {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 /* Ignore empty labels in some .3d files (caused by a bug) */
	 if (q == pimg->label_buf) goto again;
	 *q = '\0';
	 pimg->flags = img_SFLAG_UNDERGROUND; /* no flags given... */
	 if (opt == 2) goto done;
	 break;
       }
       case 6: case 7: {
	 long len;
	 result = img_LABEL;

	 if (opt == 7)
	    pimg->flags = getc(pimg->fh);
	 else
	    pimg->flags = img_SFLAG_UNDERGROUND; /* no flags given... */

	 len = get32(pimg->fh);

	 if (feof(pimg->fh)) {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 if (ferror(pimg->fh)) {
	    img_errno = IMG_READERROR;
	    return img_BAD;
	 }

	 /* Ignore empty labels in some .3d files (caused by a bug) */
	 if (len == 0) goto again;
	 if (!check_label_space(pimg, len + 1)) {
	    img_errno = IMG_OUTOFMEMORY;
	    return img_BAD;
	 }
	 if (fread(pimg->label_buf, len, 1, pimg->fh) != 1) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 pimg->label_buf[len] = '\0';
	 break;
       }
       case 4:
	 result = img_MOVE;
	 break;
       case 5:
	 result = img_LINE;
	 break;
       default:
	 switch ((int)opt & 0xc0) {
	  case 0x80:
	    pimg->flags = (int)opt & 0x3f;
	    result = img_LINE;
	    break;
	  case 0x40: {
	    char *q;
	    pimg->flags = (int)opt & 0x3f;
	    result = img_LABEL;
	    if (!fgets(pimg->label_buf, 257, pimg->fh)) {
	       img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	       return img_BAD;
	    }
	    q = pimg->label_buf + strlen(pimg->label_buf) - 1;
	    /* Ignore empty-labels in some .3d files (caused by a bug) */
	    if (q == pimg->label_buf) goto again;
	    if (*q != '\n') {
	       img_errno = IMG_BADFORMAT;
	       return img_BAD;
	    }
	    *q = '\0';
	    break;
	  }
	  case 0xc0:
	    /* use this for an extra leg or station flag if we need it */
	  default:
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 break;
      }

      if (!read_coord(pimg->fh, &pt)) return img_BAD;

      if (result == img_LABEL && pimg->survey_len) {
	 if (strncmp(pimg->label_buf, pimg->survey, pimg->survey_len + 1) != 0)
	    goto again;
	 pimg->label += pimg->survey_len + 1;
      }

      done:
      *p = pt;

      if (result == img_MOVE && pimg->version == 1) {
	 /* peek at next code and see if it's an old-style label */
	 opt_lookahead = get32(pimg->fh);

	 if (feof(pimg->fh)) {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 if (ferror(pimg->fh)) {
	    img_errno = IMG_READERROR;
	    return img_BAD;
	 }

	 if (opt_lookahead == 2) return img_read_item(pimg, p);
      }

      return result;
   } else if (pimg->version == 0) {
      ascii_again:
      pimg->label[0] = '\0';
      if (feof(pimg->fh)) return img_STOP;
      if (pimg->pending) {
	 pimg->pending = 0;
	 result = img_LINE;
      } else {
	 char cmd[7];
	 /* Stop if nothing found */
	 if (fscanf(pimg->fh, "%6s", cmd) < 1) return img_STOP;
	 if (strcmp(cmd, "move") == 0)
	    result = img_MOVE;
	 else if (strcmp(cmd, "draw") == 0)
	    result = img_LINE;
	 else if (strcmp(cmd, "line") == 0) {
	    /* set flag to indicate to process second triplet as LINE */
	    pimg->pending = 1;
	    result = img_MOVE;
	 } else if (strcmp(cmd, "cross") == 0) {
	    if (fscanf(pimg->fh, "%lf%lf%lf", &p->x, &p->y, &p->z) < 3) {
	       img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	       return img_BAD;
	    }
	    goto ascii_again;
	 } else if (strcmp(cmd, "name") == 0) {
	    size_t off = 0;
	    int ch = getc(pimg->fh);
	    if (ch == ' ') ch = getc(pimg->fh);
	    while (ch != ' ') {
	       if (ch == '\n' || ch == EOF) {
		  img_errno = ferror(pimg->fh) ? IMG_READERROR : IMG_BADFORMAT;
		  return img_BAD;
	       }
	       if (off == pimg->buf_len) {
		  if (!check_label_space(pimg, pimg->buf_len * 2)) {
		     img_errno = IMG_OUTOFMEMORY;
		     return img_BAD;
		  }
	       }
	       pimg->label_buf[off++] = ch;
	       ch = getc(pimg->fh);
	    }
	    pimg->label_buf[off] = '\0';

	    pimg->label = pimg->label_buf;
	    if (pimg->label[0] == '\\') pimg->label++;

	    result = img_LABEL;
	 } else {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD; /* unknown keyword */
	 }
      }

      if (fscanf(pimg->fh, "%lf%lf%lf", &p->x, &p->y, &p->z) < 3) {
	 img_errno = ferror(pimg->fh) ? IMG_READERROR : IMG_BADFORMAT;
	 return img_BAD;
      }

      if (result == img_LABEL && pimg->survey_len) {
	 if (strncmp(pimg->label, pimg->survey, pimg->survey_len + 1) != 0)
	    goto ascii_again;
	 pimg->label += pimg->survey_len + 1;
      }

      return result;
   } else if (pimg->version == -1) {
      /* version -1: .pos file */
      size_t off;
      pimg->flags = img_SFLAG_UNDERGROUND; /* default flags */
      againpos:
      off = 0;
      while (fscanf(pimg->fh, "(%lf,%lf,%lf ) ", &p->x, &p->y, &p->z) != 3) {
	 int ch;
	 if (ferror(pimg->fh)) {
	    img_errno = IMG_READERROR;
	    return img_BAD;
	 }
	 if (feof(pimg->fh)) return img_STOP;
	 if (pimg->pending) {
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 pimg->pending = 1;
	 /* ignore rest of line */
	 do {
	    ch = getc(pimg->fh);
	 } while (ch != '\n' && ch != '\r' && ch != EOF);
      }

      pimg->label_buf[0] = '\0';
      while (!feof(pimg->fh)) {
	 if (!fgets(pimg->label_buf + off, pimg->buf_len - off, pimg->fh)) {
	    img_errno = IMG_READERROR;
	    return img_BAD;
	 }

	 off += strlen(pimg->label_buf + off);
	 if (off && pimg->label_buf[off - 1] == '\n') {
	    pimg->label_buf[off - 1] = '\0';
	    break;
	 }
	 if (!check_label_space(pimg, pimg->buf_len * 2)) {
	    img_errno = IMG_OUTOFMEMORY;
	    return img_BAD;
	 }
      }

      pimg->label = pimg->label_buf;

      if (pimg->label[0] == '\\') pimg->label++;

      if (pimg->survey_len) {
	 size_t l = pimg->survey_len + 1;
	 if (strncmp(pimg->survey, pimg->label, l) != 0) goto againpos;
	 pimg->label += l;
      }

      return img_LABEL;
   } else if (pimg->version == -2) {
      /* version -2: Compass .plt file */
      if (pimg->pending > 0) {
	 /* -1 signals we've entered the first survey we want to
	  * read, and need to fudge lots if the first action is 'D'...
	  */
	 /* pending MOVE or LINE */
	 int r = pimg->pending - 4;
	 pimg->pending = 0;
	 pimg->flags = 0;
	 pimg->label[pimg->label_len] = '\0';
	 return r;
      }

      while (1) {
	 char *line;
	 char *q;
	 size_t len = 0;
	 int ch = getc(pimg->fh);

	 switch (ch) {
	    case '\x1a': case EOF: /* Don't insist on ^Z at end of file */
	       return img_STOP;
	    case 'X': case 'F': case 'S':
	       /* bounding boX (marks end of survey), Feature survey, or
		* new Section - skip to next survey */
	       if (pimg->survey) return img_STOP;
skip_to_N:
	       while (1) {
		  do {
		     ch = getc(pimg->fh);
		  } while (ch != '\n' && ch != '\r' && ch != EOF);
		  while (ch == '\n' || ch == '\r') ch = getc(pimg->fh);
		  if (ch == 'N') break;
		  if (ch == '\x1a' || ch == EOF) return img_STOP;
	       }
	       /* FALLTHRU */
	    case 'N':
	       line = getline_alloc(pimg->fh);
	       if (!line) {
		  img_errno = IMG_OUTOFMEMORY;
		  return img_BAD;
	       }
	       while (line[len] > 32) ++len;
	       if (pimg->label_len == 0) pimg->pending = -1;
	       if (!check_label_space(pimg, len + 1)) {
		  osfree(line);
		  img_errno = IMG_OUTOFMEMORY;
		  return img_BAD;
	       }
	       pimg->label_len = len;
	       pimg->label = pimg->label_buf;
	       memcpy(pimg->label, line, len);
	       pimg->label[len] = '\0';
	       osfree(line);
	       break;
	    case 'M': case 'D': {
	       /* Move or Draw */
	       long fpos = -1;
	       if (pimg->survey && pimg->label_len == 0) {
		  /* We're only holding onto this line in case the first line
		   * of the 'N' is a 'D', so skip it for now...
		   */
		  goto skip_to_N;
	       }
	       if (ch == 'D' && pimg->pending == -1) {
		  if (pimg->survey) {
		     fpos = ftell(pimg->fh) - 1;
		     fseek(pimg->fh, pimg->start, SEEK_SET);
		     ch = getc(pimg->fh);
		     pimg->pending = 0;
		  } else {
		     /* If a file actually has a 'D' before any 'M', then
		      * pretend the 'D' is an 'M' - one of the examples
		      * in the docs was like this! */
		     ch = 'M';
		  }
	       }
	       line = getline_alloc(pimg->fh);
	       if (!line) {
		  img_errno = IMG_OUTOFMEMORY;
		  return img_BAD;
	       }
	       /* Compass store coordinates as North, East, Up = (y,x,z)! */
	       if (sscanf(line, "%lf%lf%lf", &p->y, &p->x, &p->z) != 3) {
		  osfree(line);
		  if (ferror(pimg->fh)) {
		     img_errno = IMG_READERROR;
		  } else {
		     img_errno = IMG_BADFORMAT;
		  }
		  return img_BAD;
	       }
	       p->x *= METRES_PER_FOOT;
	       p->y *= METRES_PER_FOOT;
	       p->z *= METRES_PER_FOOT;
	       q = strchr(line, 'S');
	       if (!q) {
		  osfree(line);
		  img_errno = IMG_BADFORMAT;
		  return img_BAD;
	       }
	       ++q;
	       len = 0;
	       while (q[len] > ' ') ++len;
	       q[len] = '\0';
	       len += 2; /* ' ' and '\0' */
	       if (!check_label_space(pimg, pimg->label_len + len)) {
		  img_errno = IMG_OUTOFMEMORY;
		  return img_BAD;
	       }
	       pimg->label = pimg->label_buf;
	       if (pimg->label_len) {
		   pimg->label[pimg->label_len] = ' ';
		   memcpy(pimg->label + pimg->label_len + 1, q, len - 1);
	       } else {
		   memcpy(pimg->label, q, len - 1);
	       }
	       osfree(line);
	       pimg->flags = img_SFLAG_UNDERGROUND; /* default flags */
	       if (fpos != -1) {
		  fseek(pimg->fh, fpos, SEEK_SET);
	       } else {
		  pimg->pending = (ch == 'M' ? img_MOVE : img_LINE) + 4;
	       }
	       return img_LABEL;
	    }
	    default:
	       img_errno = IMG_BADFORMAT;
	       return img_BAD;
	 }
      }
   } else {
      /* version -3 or -4: CMAP .xyz file */
      char *line = NULL;
      char *q;
      size_t len;

      if (pimg->pending) {
	 /* pending MOVE or LINE or LABEL or STOP */
	 int r = pimg->pending - 4;
	 /* Set label to empty - don't use "" as we adjust label relative
	  * to label_buf when label_buf is reallocated. */
	 pimg->label = pimg->label_buf + strlen(pimg->label_buf);
	 pimg->flags = 0;
	 if (r == img_LABEL) {
	    /* nasty magic */
	    read_xyz_shot_coords(p, pimg->label_buf + 16);
	    subtract_xyz_shot_deltas(p, pimg->label_buf + 16);
	    pimg->pending = img_STOP + 4;
	    return img_MOVE;
	 }
	
	 pimg->pending = 0;

	 if (r == img_STOP) {
	    /* nasty magic */
	    read_xyz_shot_coords(p, pimg->label_buf + 16);
	    return img_LINE;
	 }
	 
	 return r;
      }

      pimg->label = pimg->label_buf;
      do {
	 osfree(line);
	 if (feof(pimg->fh)) return img_STOP;
	 line = getline_alloc(pimg->fh);
	 if (!line) {
	    img_errno = IMG_OUTOFMEMORY;
	    return img_BAD;
	 }
      } while (line[0] == ' ' || line[0] == '\0');
      if (line[0] == '\x1a') return img_STOP;

      len = strlen(line);
      if (pimg->version == -3) {
	 /* station variant */
	 if (len < 37) {
	    osfree(line);
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 memcpy(pimg->label, line, 6);
	 q = memchr(pimg->label, ' ', 6);
	 if (!q) q = pimg->label + 6;
	 *q = '\0';

	 read_xyz_station_coords(p, line);
 
	 /* FIXME: look at prev for lines (line + 32, 5) */
	 /* FIXME: duplicate stations... */
	 return img_LABEL;
      } else {
	 /* Shot variant */
	 char old[8], new[8];
	 if (len < 61) {
	    osfree(line);
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
 
	 memcpy(old, line, 7);
	 q = memchr(old, ' ', 7);
	 if (!q) q = old + 7;
	 *q = '\0'; 

	 memcpy(new, line + 7, 7);
	 q = memchr(new, ' ', 7);
	 if (!q) q = new + 7;
	 *q = '\0'; 

	 pimg->flags = img_SFLAG_UNDERGROUND;

	 if (strcmp(old, new) == 0) {
	    pimg->pending = img_MOVE + 4;
	    read_xyz_shot_coords(p, line);
	    strcpy(pimg->label, new);
	    osfree(line);
	    return img_LABEL;
	 }
 
	 if (strcmp(old, pimg->label) == 0) {
	    pimg->pending = img_LINE + 4;
	    read_xyz_shot_coords(p, line);
	    strcpy(pimg->label, new);
	    osfree(line);
	    return img_LABEL;
	 }
	 
	 pimg->pending = img_LABEL + 4;
	 read_xyz_shot_coords(p, line);
	 strcpy(pimg->label, new);
	 memcpy(pimg->label + 16, line, 70);

	 osfree(line);
	 return img_LABEL;
      }
   }
}

static int
write_v3label(img *pimg, int opt, const char *s)
{
   size_t len, n, dot;

   /* find length of common prefix */
   dot = 0;
   for (len = 0; s[len] == pimg->label_buf[len] && s[len] != '\0'; len++) {
      if (s[len] == '.') dot = len + 1;
   }

   SVX_ASSERT(len <= pimg->label_len);
   n = pimg->label_len - len;
   if (len == 0) {
      if (pimg->label_len) putc(0, pimg->fh);
   } else if (n <= 16) {
      if (n) putc(n + 15, pimg->fh);
   } else if (dot == 0) {
      if (pimg->label_len) putc(0, pimg->fh);
      len = 0;
   } else {
      const char *p = pimg->label_buf + dot;
      n = 1;
      for (len = pimg->label_len - dot - 17; len; len--) {
	 if (*p++ == '.') n++;
      }
      if (n <= 14) {
	 putc(n, pimg->fh);
	 len = dot;
      } else {
	 if (pimg->label_len) putc(0, pimg->fh);
	 len = 0;
      }
   }

   n = strlen(s + len);
   putc(opt, pimg->fh);
   if (n < 0xfe) {
      putc(n, pimg->fh);
   } else if (n < 0xffff + 0xfe) {
      putc(0xfe, pimg->fh);
      put16((short)(n - 0xfe), pimg->fh);
   } else {
      putc(0xff, pimg->fh);
      put32(n, pimg->fh);
   }
   fwrite(s + len, n, 1, pimg->fh);

   n += len;
   pimg->label_len = n;
   if (!check_label_space(pimg, n + 1))
      return 0; /* FIXME: distinguish out of memory... */
   memcpy(pimg->label_buf + len, s + len, n - len + 1);

   return !ferror(pimg->fh);
}

void
img_write_item(img *pimg, int code, int flags, const char *s,
	       double x, double y, double z)
{
   if (!pimg) return;
   if (pimg->version == 3) {
      int opt = 0;
      switch (code) {
       case img_LABEL:
	 write_v3label(pimg, 0x40 | flags, s);
	 opt = 0;
	 break;
       case img_MOVE:
	 opt = 15;
	 break;
       case img_LINE:
	 write_v3label(pimg, 0x80 | flags, s ? s : "");
	 opt = 0;
	 break;
       default: /* ignore for now */
	 return;
      }
      if (opt) putc(opt, pimg->fh);
      /* Output in cm */
      put32((INT32_T)my_round(x * 100.0), pimg->fh);
      put32((INT32_T)my_round(y * 100.0), pimg->fh);
      put32((INT32_T)my_round(z * 100.0), pimg->fh);
   } else {
      size_t len;
      INT32_T opt = 0;
      SVX_ASSERT(pimg->version > 0);
      switch (code) {
       case img_LABEL:
	 if (pimg->version == 1) {
	    /* put a move before each label */
	    img_write_item(pimg, img_MOVE, 0, NULL, x, y, z);
	    put32(2, pimg->fh);
	    fputsnl(s, pimg->fh);
	    return;
	 }
	 len = strlen(s);
	 if (len > 255 || strchr(s, '\n')) {
	    /* long label - not in early incarnations of v2 format, but few
	     * 3d files will need these, so better not to force incompatibility
	     * with a new version I think... */
	    putc(7, pimg->fh);
	    putc(flags, pimg->fh);
	    put32(len, pimg->fh);
	    fputs(s, pimg->fh);
	 } else {
	    putc(0x40 | (flags & 0x3f), pimg->fh);
	    fputsnl(s, pimg->fh);
	 }
	 opt = 0;
	 break;
       case img_MOVE:
	 opt = 4;
	 break;
       case img_LINE:
	 if (pimg->version > 1) {
	    opt = 0x80 | (flags & 0x3f);
	    break;
	 }
	 opt = 5;
	 break;
       default: /* ignore for now */
	 return;
      }
      if (pimg->version == 1) {
	 put32(opt, pimg->fh);
      } else {
	 if (opt) putc(opt, pimg->fh);
      }
      /* Output in cm */
      put32((INT32_T)my_round(x * 100.0), pimg->fh);
      put32((INT32_T)my_round(y * 100.0), pimg->fh);
      put32((INT32_T)my_round(z * 100.0), pimg->fh);
   }
}

int
img_close(img *pimg)
{
   int result = 1;
   if (pimg) {
      if (pimg->fh) {
	 if (pimg->fRead) {
	    osfree(pimg->survey);
	    osfree(pimg->title);
	    osfree(pimg->datestamp);
	 } else {
	    /* write end of data marker */
	    switch (pimg->version) {
	     case 1:
	       put32((INT32_T)-1, pimg->fh);
	       break;
	     case 2:
	       putc(0, pimg->fh);
	       break;
	     case 3:
	       if (pimg->label_len) putc(0, pimg->fh);
	       putc(0, pimg->fh);
	       break;
	    }
	 }
	 if (ferror(pimg->fh)) result = 0;
	 if (fclose(pimg->fh)) result = 0;
	 if (!result) img_errno = pimg->fRead ? IMG_READERROR : IMG_WRITEERROR;
      }
      osfree(pimg->label_buf);
      osfree(pimg);
   }
   return result;
}
