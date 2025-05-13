/* img.c
 * Routines for reading and writing processed survey data files
 *
 * Copyright (C) 1993-2024 Olly Betts
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "img.h"

#define TIMENA "?"
#ifdef IMG_HOSTED
# define INT32_T int32_t
# define UINT32_T uint32_t
# define SNPRINTF snprintf
# include "debug.h"
# include "filelist.h"
# include "filename.h"
# include "message.h"
# include "useful.h"
# define TIMEFMT msg(/*%a,%Y.%m.%d %H:%M:%S %Z*/107)
#else
# if defined HAVE_STDINT_H || \
     (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) || \
     (defined __cplusplus && __cplusplus >= 201103L)
#  include <stdint.h>
#  define INT32_T int32_t
#  define UINT32_T uint32_t
# else
#  include <limits.h>
#  if INT_MAX >= 2147483647
#   define INT32_T int
#   define UINT32_T unsigned
#  else
#   define INT32_T long
#   define UINT32_T unsigned long
#  endif
# endif
# if defined HAVE_SNPRINTF || \
     (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) || \
     (defined __cplusplus && __cplusplus >= 201103L)
#  define SNPRINTF snprintf
# else
#  define SNPRINTF my_snprintf
static int my_snprintf(char *s, size_t size, const char *format, ...) {
    int result;
    va_list ap;
    va_start(ap, format);
    (void)size; /* The buffer passed should always be large enough. */
    result = vsprintf(s, format, ap);
    va_end(ap);
    return result;
}
# endif
# define TIMEFMT "%a,%Y.%m.%d %H:%M:%S %Z"
# define EXT_SVX_3D "3d"
# define FNM_SEP_EXT '.'
# define METRES_PER_FOOT 0.3048 /* exact value */
# define xosmalloc(L) malloc((L))
# define xosrealloc(L,S) realloc((L),(S))
# define osfree(P) free((P))
# define osnew(T) (T*)malloc(sizeof(T))

/* in IMG_HOSTED mode, this tests if a filename refers to a directory */
# define fDirectory(X) 0
/* open file FNM with mode MODE, maybe using path PTH and/or extension EXT */
/* path isn't used in img.c, but EXT is */
# define fopenWithPthAndExt(PTH,FNM,EXT,MODE,X) \
    ((*(X) = NULL), fopen(FNM,MODE))
# ifndef PUTC
#  define PUTC(C, FH) putc(C, FH)
# endif
# ifndef GETC
#  define GETC(FH) getc(FH)
# endif
# define fputsnl(S, FH) (fputs((S), (FH)) == EOF ? EOF : putc('\n', (FH)))
# define SVX_ASSERT(X)

#ifdef __cplusplus
# include <algorithm>
using std::max;
using std::min;
#else
/* Return max/min of two numbers. */
/* May be defined already (e.g. by Borland C in stdlib.h) */
/* NB Bad news if X or Y has side-effects... */
# ifndef max
#  define max(X, Y) ((X) > (Y) ? (X) : (Y))
# endif
# ifndef min
#  define min(X, Y) ((X) < (Y) ? (X) : (Y))
# endif
#endif

static INT32_T
get32(FILE *fh)
{
   UINT32_T w = GETC(fh);
   w |= (UINT32_T)GETC(fh) << 8l;
   w |= (UINT32_T)GETC(fh) << 16l;
   w |= (UINT32_T)GETC(fh) << 24l;
   return (INT32_T)w;
}

static void
put32(UINT32_T w, FILE *fh)
{
   PUTC((char)(w), fh);
   PUTC((char)(w >> 8l), fh);
   PUTC((char)(w >> 16l), fh);
   PUTC((char)(w >> 24l), fh);
}

static short
get16(FILE *fh)
{
   UINT32_T w = GETC(fh);
   w |= (UINT32_T)GETC(fh) << 8l;
   return (short)w;
}

static void
put16(short word, FILE *fh)
{
   unsigned short w = (unsigned short)word;
   PUTC((char)(w), fh);
   PUTC((char)(w >> 8l), fh);
}

static char *
baseleaf_from_fnm(const char *fnm)
{
   const char *p;
   const char *q;
   char * res;
   size_t len;

   p = fnm;
   q = strrchr(p, '/');
   if (q) p = q + 1;
   q = strrchr(p, '\\');
   if (q) p = q + 1;

   q = strrchr(p, FNM_SEP_EXT);
   if (q) len = (const char *)q - p; else len = strlen(p);

   res = (char *)xosmalloc(len + 1);
   if (!res) return NULL;
   memcpy(res, p, len);
   res[len] = '\0';
   return res;
}
#endif

static char * my_strdup(const char *str);

static time_t
mktime_with_tz(struct tm * tm, const char * tz)
{
    time_t r;
    char * old_tz = getenv("TZ");
#ifdef _MSC_VER
    if (old_tz) {
	old_tz = my_strdup(old_tz);
	if (!old_tz)
	    return (time_t)-1;
    }
    if (_putenv_s("TZ", tz) != 0) {
	osfree(old_tz);
	return (time_t)-1;
    }
#elif defined HAVE_SETENV
    if (old_tz) {
	old_tz = my_strdup(old_tz);
	if (!old_tz)
	    return (time_t)-1;
    }
    if (setenv("TZ", tz, 1) < 0) {
	osfree(old_tz);
	return (time_t)-1;
    }
#else
    char * p;
    if (old_tz) {
	size_t len = strlen(old_tz) + 1;
	p = (char *)xosmalloc(len + 3);
	if (!p)
	    return (time_t)-1;
	memcpy(p, "TZ=", 3);
	memcpy(p + 3, tz, len);
	old_tz = p;
    }
    p = (char *)xosmalloc(strlen(tz) + 4);
    if (!p) {
	osfree(old_tz);
	return (time_t)-1;
    }
    memcpy(p, "TZ=", 3);
    strcpy(p + 3, tz);
    if (putenv(p) != 0) {
	osfree(p);
	osfree(old_tz);
	return (time_t)-1;
    }
#define CLEANUP() osfree(p)
#endif
    tzset();
    r = mktime(tm);
    if (old_tz) {
#ifdef _MSC_VER
	_putenv_s("TZ", old_tz);
#elif !defined HAVE_SETENV
	putenv(old_tz);
#else
	setenv("TZ", old_tz, 1);
#endif
	osfree(old_tz);
    } else {
#ifdef _MSC_VER
	_putenv_s("TZ", "");
#elif !defined HAVE_UNSETENV
	putenv((char*)"TZ");
#else
	unsetenv("TZ");
#endif
    }
#ifdef CLEANUP
    CLEANUP();
#undef CLEANUP
#endif
    return r;
}

static unsigned short
getu16(FILE *fh)
{
   return (unsigned short)get16(fh);
}

#include <math.h>

#if !defined HAVE_LROUND && !defined HAVE_DECL_LROUND
/* The autoconf tests are not in use, but C99 and C++11 both added lround(),
 * so set HAVE_LROUND and HAVE_DECL_LROUND conservatively based on the language
 * standard version the compiler claims to support. */
# if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) || \
     (defined __cplusplus && __cplusplus >= 201103L)
#  define HAVE_LROUND 1
#  define HAVE_DECL_LROUND 1
# endif
#endif

#ifdef HAVE_LROUND
# if defined HAVE_DECL_LROUND && !HAVE_DECL_LROUND
/* On older systems, the prototype may be missing. */
extern long lround(double);
# endif
# define my_lround lround
#else
static long
my_lround(double x) {
   return (x >= 0.0) ? (long)(x + 0.5) : -(long)(0.5 - x);
}
#endif

unsigned int img_output_version = IMG_VERSION_MAX;

static img_errcode img_errno = IMG_NONE;

#define FILEID "Survex 3D Image File"

/* Encode extension into integer for fast testing. */
#define EXT3(C1, C2, C3) (((C3) << 16) | ((C2) << 8) | (C1))

/* Attempt to string paste to ensure we are passed a literal string */
#define LITLEN(S) (sizeof(S"") - 1)

/* Fake "version numbers" for non-3d formats we can read. */
#define VERSION_CMAP_SHOT	-4
#define VERSION_CMAP_STATION	-3
#define VERSION_COMPASS_PLT	-2
#define VERSION_SURVEX_POS	-1

/* Flags bitwise-or-ed into pending to track XSECTs. */
#define PENDING_XSECT_END	0x100
#define PENDING_HAD_XSECT	0x001 /* Only for VERSION_COMPASS_PLT */
#define PENDING_MOVE		0x002 /* Only for VERSION_COMPASS_PLT */
#define PENDING_LINE		0x004 /* Only for VERSION_COMPASS_PLT */
#define PENDING_XSECT		0x008 /* Only for VERSION_COMPASS_PLT */
#define PENDING_FLAGS_SHIFT	9 /* Only for VERSION_COMPASS_PLT */

/* Days from start of 1900 to start of 1970. */
#define DAYS_1900 25567

/* Start of 1900 in time_t with standard Unix epoch of start of 1970. */
#define TIME_T_1900 -2208988800L

/* Seconds in a day. */
#define SECS_PER_DAY 86400L

static unsigned
hash_data(const char *s, unsigned len)
{
    /* djb2 hash but with an initial value of zero. */
    unsigned h = 0;
    while (len) {
	unsigned char c = (unsigned char)*s++;
	h = ((h << 5) + h) + c;
	--len;
    }
    return h;
}

struct compass_station {
    struct compass_station *next;
    unsigned char flags;
    unsigned char len;
    char name[1];
};

/* On the first pass, at the start of each survey we run through all the
 * hash table entries that exist and set this flag.
 *
 * If this flag is set when we add flags to an existing station we
 * know it appears in multiple surveys and can infer img_SFLAG_EXPORTED.
 */
#define COMPASS_SFLAG_DIFFERENT_SURVEY 0x80

#define COMPASS_SFLAG_MASK 0x7f

/* How many hash buckets to use (must be a power of 2).
 *
 * Each bucket is a linked list so this doesn't limit how many entries we can
 * store, but should be sized based on a plausible estimate of how many
 * different stations we're likely to see in a single PLT file.
 */
#define HASH_BUCKETS 0x2000U

static void*
compass_plt_allocate_hash(void)
{
    struct compass_station_name** htab = xosmalloc(HASH_BUCKETS * sizeof(struct compass_station_name*));
    if (htab) {
	unsigned i;
	for (i = 0; i < HASH_BUCKETS; ++i)
	    htab[i] = NULL;
    }
    return htab;
}

static int
compass_plt_update_station(img *pimg, const char *name, int name_len,
			   unsigned flags)
{
    struct compass_station *p;
    struct compass_station **htab = (struct compass_station**)pimg->data;
    htab += hash_data(name, name_len) & (HASH_BUCKETS - 1U);
    for (p = *htab; p; p = p->next) {
	if (p->len == name_len) {
	    if (memcmp(name, p->name, name_len) == 0) {
		p->flags |= flags;
		if (p->flags & COMPASS_SFLAG_DIFFERENT_SURVEY)
		    p->flags |= img_SFLAG_EXPORTED;
		return 0;
	    }
	}
    }
    p = malloc(offsetof(struct compass_station, name) + name_len);
    if (!p) return -1;
    p->flags = flags;
    p->len = name_len;
    memcpy(p->name, name, name_len);
    p->next = *htab;
    *htab = p;
    return 0;
}

static void
compass_plt_new_survey(img *pimg)
{
    struct compass_station **htab = (struct compass_station**)pimg->data;
    int i = HASH_BUCKETS;
    while (--i) {
	struct compass_station *p;
	for (p = *htab; p; p = p->next) {
	    p->flags |= COMPASS_SFLAG_DIFFERENT_SURVEY;
	}
	++htab;
    }
}

static void
compass_plt_free_data(img *pimg)
{
    struct compass_station **htab = (struct compass_station**)pimg->data;
    int i = HASH_BUCKETS;
    while (--i) {
	struct compass_station *p = *htab;
	while (p) {
	    struct compass_station *next = p->next;
	    osfree(p);
	    p = next;
	}
	++htab;
    }
    osfree(pimg->data);
    pimg->data = NULL;
}

static int
compass_plt_get_station_flags(img *pimg, const char *name, int name_len)
{
    struct compass_station *p;
    struct compass_station **htab = (struct compass_station**)pimg->data;
    htab += hash_data(name, name_len) & (HASH_BUCKETS - 1U);
    for (p = *htab; p; p = p->next) {
	if (p->len == name_len) {
	    if (memcmp(name, p->name, name_len) == 0) {
		if (p->flags & COMPASS_SFLAG_DIFFERENT_SURVEY) {
		    p->flags &= ~COMPASS_SFLAG_DIFFERENT_SURVEY;
		    return p->flags;
		}
		return p->flags | INT_MIN;
	    }
	}
    }
    return -1;
}

static char *
my_strdup(const char *str)
{
   char *p;
   size_t len = strlen(str) + 1;
   p = (char *)xosmalloc(len);
   if (p) memcpy(p, str, len);
   return p;
}

#define getline_alloc(FH) getline_alloc_len(FH, NULL)

static char *
getline_alloc_len(FILE *fh, size_t * p_len)
{
   int ch;
   size_t i = 0;
   size_t len = 16;
   char *buf = (char *)xosmalloc(len);
   if (!buf) return NULL;

   ch = GETC(fh);
   while (ch != '\n' && ch != '\r' && ch != EOF) {
      buf[i++] = ch;
      if (i == len - 1) {
	 char *p;
	 len += len;
	 p = (char *)xosrealloc(buf, len);
	 if (!p) {
	    osfree(buf);
	    return NULL;
	 }
	 buf = p;
      }
      ch = GETC(fh);
   }
   if (ch == '\n' || ch == '\r') {
      int otherone = ch ^ ('\n' ^ '\r');
      ch = GETC(fh);
      /* if it's not the other eol character, put it back */
      if (ch != otherone) ungetc(ch, fh);
   }
   buf[i] = '\0';
   if (p_len) *p_len = i;
   return buf;
}

img_errcode
img_error(void)
{
   return img_errno;
}

static int
check_label_space(img *pimg, size_t len)
{
   if (len > pimg->buf_len) {
      size_t label_offset = pimg->label - pimg->label_buf;
      char *b = (char *)xosrealloc(pimg->label_buf, len);
      if (!b) return 0;
      pimg->label_buf = b;
      pimg->label = b + label_offset;
      pimg->buf_len = len;
   }
   return 1;
}

/* Check if a station name should be included. */
static int
stn_included(img *pimg)
{
    if (!pimg->survey_len) return 1;
    size_t l = pimg->survey_len;
    const char *s = pimg->label_buf;
    if (strncmp(pimg->survey, s, l + 1) != 0) {
	return 0;
    }
    pimg->label += l + 1;
    return 1;
}

/* Check if a survey name should be included. */
static int
survey_included(img *pimg)
{
    if (!pimg->survey_len) return 1;
    size_t l = pimg->survey_len;
    const char *s = pimg->label_buf;
    if (strncmp(pimg->survey, s, l) != 0 ||
	!(s[l] == '.' || s[l] == '\0')) {
       return 0;
    }
    pimg->label += l;
    /* skip the dot if there */
    if (*pimg->label) pimg->label++;
    return 1;
}

/* Check if a survey name in a buffer should be included.
 *
 * For "foreign" formats which just have one level of surveys.
 */
static int
buf_included(img *pimg, const char *buf, size_t len)
{
    return pimg->survey_len == len && memcmp(buf, pimg->survey, len) == 0;
}

img *
img_open_survey(const char *fnm, const char *survey)
{
   img *pimg;
   FILE *fh;
   char* filename_opened = NULL;

   if (fDirectory(fnm)) {
      img_errno = IMG_DIRECTORY;
      return NULL;
   }

   fh = fopenWithPthAndExt("", fnm, EXT_SVX_3D, "rb", &filename_opened);
   pimg = img_read_stream_survey(fh, fclose,
				 filename_opened ? filename_opened : fnm,
				 survey);
   if (pimg) {
       pimg->filename_opened = filename_opened;
   } else {
       osfree(filename_opened);
   }
   return pimg;
}

static int
compass_plt_open(img *pimg)
{
    int utm_zone = 0;
    int datum = img_DATUM_UNKNOWN;
    long fpos;
    char *from = NULL;
    int from_len = 0;

    pimg->version = VERSION_COMPASS_PLT;
    /* Spaces aren't legal in Compass station names, but dots are, so
     * use space as the level separator */
    pimg->separator = ' ';
    pimg->start = -1;
    pimg->datestamp = my_strdup(TIMENA);
    if (!pimg->datestamp) {
	return IMG_OUTOFMEMORY;
    }
    pimg->data = compass_plt_allocate_hash();
    if (!pimg->data) {
	return IMG_OUTOFMEMORY;
    }

    /* Read through the whole file first, recording any station flags
     * (pimg->data), finding where to start reading data from (pimg->start),
     * and deciding what to report for "title".
     */
    while (1) {
	int ch = GETC(pimg->fh);
	switch (ch) {
	  case '\x1a':
	    fseek(pimg->fh, -1, SEEK_CUR);
	    /* FALL THRU */
	  case EOF:
	    if (pimg->start < 0) {
		pimg->start = ftell(pimg->fh);
	    } else {
		fseek(pimg->fh, pimg->start, SEEK_SET);
	    }

	    if (datum && utm_zone && abs(utm_zone) <= 60) {
		/* Map to an EPSG code where we can. */
		const char* template = "EPSG:%d";
		int value = 0;
		switch (datum) {
		  case img_DATUM_NAD27:
		    if (utm_zone < 0) {
			template = "+proj=utm +zone=%d +datum=NAD27 +south +units=m +no_defs +type=crs";
			value = -utm_zone;
		    } else if (utm_zone <= 23) {
			value = 26700 + utm_zone;
		    } else if (utm_zone < 59) {
			template = "+proj=utm +zone=%d +datum=NAD27 +units=m +no_defs +type=crs";
			value = utm_zone;
		    } else {
			value = 3311 + utm_zone;
		    }
		    break;
		  case img_DATUM_NAD83:
		    if (utm_zone < 0) {
			template = "+proj=utm +zone=%d +datum=NAD83 +south +units=m +no_defs +type=crs";
			value = -utm_zone;
		    } else if (utm_zone <= 23) {
			value = 26900 + utm_zone;
		    } else if (utm_zone == 24) {
			value = 9712;
		    } else if (utm_zone < 59) {
			template = "+proj=utm +zone=%d +datum=NAD83 +units=m +no_defs +type=crs";
			value = utm_zone;
		    } else {
			value = 3313 + utm_zone;
		    }
		    break;
		  case img_DATUM_WGS84:
		    if (utm_zone > 0) {
			value = 32600 + utm_zone;
		    } else {
			value = 32700 - utm_zone;
		    }
		    break;
		}
		if (value) {
		    size_t len = strlen(template) + 4;
		    pimg->cs = (char*)xosmalloc(len);
		    if (!pimg->cs) {
			goto out_of_memory_error;
		    }
		    SNPRINTF(pimg->cs, len, template, value);
		}
	    }

	    osfree(from);
	    return 0;
	  case 'S':
	    /* "Section" - in the case where we aren't filtering by survey
	     * (i.e. pimg->survey == NULL): if there's only one non-empty
	     * section name specified, we use it as the title.
	     */
	    if (pimg->survey == NULL && (!pimg->title || pimg->title[0])) {
		char *line = getline_alloc(pimg->fh);
		if (!line) {
		    goto out_of_memory_error;
		}
		if (line[0]) {
		    if (pimg->title) {
			if (strcmp(pimg->title, line) != 0) {
			    /* Two different non-empty section names found. */
			    pimg->title[0] = '\0';
			}
			osfree(line);
		    } else {
			pimg->title = line;
		    }
		} else {
		    osfree(line);
		}
		continue;
	    }
	    break;
	  case 'N': {
	      char *line, *q;
	      size_t len;
	      compass_plt_new_survey(pimg);
	      if (pimg->start >= 0) break;
	      fpos = ftell(pimg->fh) - 1;
	      if (!pimg->survey) {
		  /* We're not filtering by survey so just note down the file
		   * offset for the first N command. */
		  pimg->start = fpos;
		  break;
	      }
	      line = getline_alloc(pimg->fh);
	      if (!line) {
		  goto out_of_memory_error;
	      }
	      len = 0;
	      while (line[len] > 32) ++len;
	      if (!buf_included(pimg, line, len)) {
		  /* Not the survey we are looking for. */
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
		  goto out_of_memory_error;
	      }
	      pimg->start = fpos;
	      continue;
	  }
	  case 'M':
	  case 'D':
	  case 'd': {
	      /* Move or Draw */
	      int command = ch;
	      char *q, *name;
	      unsigned station_flags = 0;
	      int name_len;
	      int not_plotted = (command == 'd');

	      /* Find station name. */
	      do { ch = GETC(pimg->fh); } while (ch >= ' ' && ch != 'S');

	      if (ch != 'S') {
		  /* Leave reporting error to second pass for consistency. */
		  break;
	      }

	      name = getline_alloc(pimg->fh);
	      if (!name) {
		  goto out_of_memory_error;
	      }

	      name_len = 0;
	      while (name[name_len] > ' ') ++name_len;
	      if (name_len > 255) {
		  /* The spec says "up to 12 characters", we allow up to 255. */
		  osfree(name);
		  osfree(from);
		  return IMG_BADFORMAT;
	      }

	      /* Check for the "distance from entrance" field. */
	      q = strchr(name + name_len, 'I');
	      if (q) {
		  double distance_from_entrance;
		  int bytes_used = 0;
		  ++q;
		  if (sscanf(q, "%lf%n",
			     &distance_from_entrance, &bytes_used) == 1 &&
		      distance_from_entrance == 0.0) {
		      /* Infer an entrance. */
		      station_flags |= img_SFLAG_ENTRANCE;
		  }
		  q += bytes_used;
		  while (*q && *q <= ' ') q++;
	      } else {
		  q = strchr(name + name_len, 'F');
	      }

	      if (q && *q == 'F') {
		  /* "Shot Flags". */
		  while (isalpha((unsigned char)*++q)) {
		      switch (*q) {
			case 'S':
			  /* The format specification says «The shot is a "splay"
			   * shot, which is a shot from a station to the wall to
			   * define the passage shape.» so we set the wall flag
			   * for the to station.
			   */
			  station_flags |= img_SFLAG_WALL;
			  break;
			case 'P':
			  not_plotted = 1;
			  break;
		      }
		  }
	      }

	      /* Shot flag P (which is also implied by command d) is "Exclude
	       * this shot from plotting", but the use suggested in the Compass
	       * docs is for surface data, and they "[do] not support passage
	       * modeling".
	       *
	       * Even if it's actually being used for a different purpose,
	       * Survex programs don't show surface legs by default so the end
	       * effect is at least to not plot as intended.
	       */
	      if (command != 'M') {
		  int surface_or_not = not_plotted ? img_SFLAG_SURFACE
						   : img_SFLAG_UNDERGROUND;
		  station_flags |= surface_or_not;
		  if (compass_plt_update_station(pimg, from, from_len,
						 surface_or_not) < 0) {
		      goto out_of_memory_error;
		  }
	      }

	      if (compass_plt_update_station(pimg, name, name_len,
					     station_flags) < 0) {
		  goto out_of_memory_error;
	      }
	      osfree(from);
	      from = name;
	      from_len = name_len;
	      continue;
	  }
	  case 'P': {
	      /* Fixed point. */
	      char *line, *q, *name;
	      int name_len;

	      line = getline_alloc(pimg->fh);
	      if (!line) {
		  goto out_of_memory_error;
	      }
	      q = line;
	      while (*q && *q <= ' ') q++;
	      name = q;
	      name_len = 0;
	      while (name[name_len] > ' ') ++name_len;

	      if (name_len > 255) {
		  /* The spec says "up to 12 characters", we allow up to 255. */
		  osfree(line);
		  osfree(from);
		  return IMG_BADFORMAT;
	      }

	      if (compass_plt_update_station(pimg, name, name_len,
					     img_SFLAG_FIXED) < 0) {
		  goto out_of_memory_error;
	      }

	      osfree(line);
	      continue;
	  }
	  case 'G': {
	      /* UTM Zone - 1 to 60 for North, -1 to -60 for South. */
	      char *line = getline_alloc(pimg->fh);
	      char *p = line;
	      long v = strtol(p, &p, 10);
	      if (v < -60 || v > 60 || v == 0 || *p > ' ') {
		  osfree(line);
		  continue;
	      }
	      if (utm_zone && utm_zone != v) {
		  /* More than one UTM zone specified. */
		  /* FIXME: We could handle this by reprojecting, but then we'd
		   * need access to PROJ from img.
		   */
		  utm_zone = 99;
	      } else {
		  utm_zone = v;
	      }
	      osfree(line);
	      continue;
	  }
	  case 'O': {
	      /* Datum. */
	      int new_datum;
	      char *line = getline_alloc(pimg->fh);
	      if (!line) {
		  goto out_of_memory_error;
	      }
	      if (utm_zone == 99) {
		  osfree(line);
		  continue;
	      }

	      new_datum = img_parse_compass_datum_string(line, strlen(line));
	      if (new_datum == img_DATUM_UNKNOWN) {
		  utm_zone = 99;
	      } else if (datum == img_DATUM_UNKNOWN) {
		  datum = new_datum;
	      } else if (datum != new_datum) {
		  utm_zone = 99;
	      }

	      osfree(line);
	      continue;
	  }
	}
	while (ch != '\n' && ch != '\r') {
	    ch = GETC(pimg->fh);
	}
    }
out_of_memory_error:
    osfree(from);
    return IMG_OUTOFMEMORY;
}

static int
cmap_xyz_open(img *pimg)
{
    size_t len;
    char *line = getline_alloc(pimg->fh);
    if (!line) {
	return IMG_OUTOFMEMORY;
    }

    /* Spaces aren't legal in CMAP station names, but dots are, so
     * use space as the level separator. */
    pimg->separator = ' ';

    /* There doesn't seem to be a spec for what happens after 1999 with cmap
     * files, so this code allows for:
     *  * 21xx -> xx (up to 2150)
     *  * 21xx -> 1xx (up to 2199)
     *  * full year being specified instead of 2 digits
     */
    len = strlen(line);
    if (len > 59) {
	/* Don't just truncate at column 59, allow for a > 2 digit year. */
	char * p = strstr(line + len, "Page");
	if (p) {
	    while (p > line && p[-1] == ' ')
		--p;
	    *p = '\0';
	    len = p - line;
	} else {
	    line[59] = '\0';
	}
    }
    if (len > 45) {
	/* YY/MM/DD HH:MM */
	struct tm tm;
	unsigned long v;
	char * p;
	pimg->datestamp = my_strdup(line + 45);
	p = pimg->datestamp;
	v = strtoul(p, &p, 10);
	if (v <= 50) {
	    /* In the absence of a spec for cmap files, assume <= 50 means 21st
	     * century. */
	    v += 2000;
	} else if (v < 200) {
	    /* Map 100-199 to 21st century. */
	    v += 1900;
	}
	if (v == ULONG_MAX || *p++ != '/')
	    goto bad_cmap_date;
	tm.tm_year = v - 1900;
	v = strtoul(p, &p, 10);
	if (v < 1 || v > 12 || *p++ != '/')
	    goto bad_cmap_date;
	tm.tm_mon = v - 1;
	v = strtoul(p, &p, 10);
	if (v < 1 || v > 31 || *p++ != ' ')
	    goto bad_cmap_date;
	tm.tm_mday = v;
	v = strtoul(p, &p, 10);
	if (v >= 24 || *p++ != ':')
	    goto bad_cmap_date;
	tm.tm_hour = v;
	v = strtoul(p, &p, 10);
	if (v >= 60)
	    goto bad_cmap_date;
	tm.tm_min = v;
	if (*p == ':') {
	    v = strtoul(p + 1, &p, 10);
	    if (v > 60)
		goto bad_cmap_date;
	    tm.tm_sec = v;
	} else {
	    tm.tm_sec = 0;
	}
	tm.tm_isdst = 0;
	/* We have no indication of what timezone this timestamp is in.  It's
	 * probably local time for whoever processed the data, so just assume
	 * UTC, which is at least fairly central in the possibilities.
	 */
	pimg->datestamp_numeric = mktime_with_tz(&tm, "");
    } else {
	pimg->datestamp = my_strdup(TIMENA);
    }
bad_cmap_date:
    if (strncmp(line, "  Cave Survey Data Processed by CMAP ",
		LITLEN("  Cave Survey Data Processed by CMAP ")) != 0) {
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
    osfree(line);
    if (!pimg->datestamp || !pimg->title) {
	return IMG_OUTOFMEMORY;
    }
    line = getline_alloc(pimg->fh);
    if (!line) {
	return IMG_OUTOFMEMORY;
    }
    if (line[0] != ' ' || (line[1] != 'S' && line[1] != 'O')) {
	return IMG_BADFORMAT;
    }
    if (line[1] == 'S') {
	pimg->version = VERSION_CMAP_STATION;
    } else {
	pimg->version = VERSION_CMAP_SHOT;
    }
    osfree(line);
    line = getline_alloc(pimg->fh);
    if (!line) {
	return IMG_OUTOFMEMORY;
    }
    if (line[0] != ' ' || line[1] != '-') {
	return IMG_BADFORMAT;
    }
    osfree(line);
    pimg->start = ftell(pimg->fh);
    return 0;
}

img *
img_read_stream_survey(FILE *stream, int (*close_func)(FILE*),
		       const char *fnm,
		       const char *survey)
{
   img *pimg;
   size_t len;
   char buf[LITLEN(FILEID) + 9];
   int ch;
   UINT32_T ext;

   if (stream == NULL) {
      img_errno = IMG_FILENOTFOUND;
      return NULL;
   }

   pimg = osnew(img);
   if (pimg == NULL) {
      img_errno = IMG_OUTOFMEMORY;
      if (close_func) close_func(stream);
      return NULL;
   }

   pimg->fh = stream;
   pimg->close_func = close_func;

   pimg->buf_len = 257;
   pimg->label_buf = (char *)xosmalloc(pimg->buf_len);
   if (!pimg->label_buf) {
      if (pimg->close_func) pimg->close_func(pimg->fh);
      osfree(pimg);
      img_errno = IMG_OUTOFMEMORY;
      return NULL;
   }

   pimg->fRead = 1; /* reading from this file */
   img_errno = IMG_NONE;

   pimg->flags = 0;
   pimg->filename_opened = NULL;
   pimg->data = NULL;

   /* for version >= 3 we use label_buf to store the prefix for reuse */
   /* for VERSION_COMPASS_PLT, 0 value indicates we haven't
    * entered a survey yet */
   /* for VERSION_CMAP_SHOT, we store the last station here
    * to detect whether we MOVE or LINE */
   pimg->label_len = 0;
   pimg->label_buf[0] = '\0';

   pimg->survey = NULL;
   pimg->survey_len = 0;
   pimg->separator = '.';
#if IMG_API_VERSION == 0
   pimg->date1 = pimg->date2 = 0;
#else /* IMG_API_VERSION == 1 */
   pimg->days1 = pimg->days2 = -1;
#endif
   pimg->is_extended_elevation = 0;

   pimg->style = pimg->oldstyle = img_STYLE_UNKNOWN;

   pimg->l = pimg->r = pimg->u = pimg->d = -1.0;

   pimg->title = pimg->datestamp = pimg->cs = NULL;
   pimg->datestamp_numeric = (time_t)-1;

   if (survey) {
      len = strlen(survey);
      if (len) {
	 if (survey[len - 1] == '.') len--;
	 if (len) {
	    char *p;
	    pimg->survey = (char *)xosmalloc(len + 2);
	    if (!pimg->survey) {
	       goto out_of_memory_error;
	    }
	    memcpy(pimg->survey, survey, len);
	    /* Set title to leaf survey name */
	    pimg->survey[len] = '\0';
	    p = strrchr(pimg->survey, '.');
	    if (p) p++; else p = pimg->survey;
	    pimg->title = my_strdup(p);
	    if (!pimg->title) {
	       goto out_of_memory_error;
	    }
	    pimg->survey[len] = '.';
	    pimg->survey[len + 1] = '\0';
	 }
      }
      pimg->survey_len = len;
   }

   /* [VERSION_COMPASS_PLT] bitwise-or of PENDING_* values, or -1.
    * [VERSION_CMAP_STATION, VERSION_CMAP_SHOT] pending IMG_LINE or IMG_MOVE -
    * both have 4 added.
    * [VERSION_SURVEX_POS] already skipped heading line, or there wasn't one
    * [version 0] not in the middle of a 'LINE' command
    * [version >= 3] not in the middle of turning a LINE into a MOVE
    */
   pimg->pending = 0;

   len = strlen(fnm);
   /* Currently only 3 character extensions are tested below. */
   ext = 0;
   if (len > 4 && fnm[len - 4] == '.') {
       /* Read extension and pack into ext. */
       int i;
       for (i = 1; i < 4; ++i) {
	   unsigned char ext_ch = fnm[len - i];
	   ext = (ext << 8) | tolower(ext_ch);
       }
   }
   switch (ext) {
     case EXT3('p', 'o', 's'): /* Survex .pos */
pos_file:
       pimg->version = VERSION_SURVEX_POS;
       pimg->datestamp = my_strdup(TIMENA);
       if (!pimg->datestamp) {
	   goto out_of_memory_error;
       }
       pimg->start = 0;
       goto successful_return;

     case EXT3('p', 'l', 't'): /* Compass .plt */
     case EXT3('p', 'l', 'f'): /* Compass .plf */ {
       int result;
plt_file:
       result = compass_plt_open(pimg);
       if (result) {
	   img_errno = result;
	   goto error;
       }
       goto successful_return;
     }

     /* Although these are often referred to as "CMAP .XYZ files", it seems
      * that actually, the extension .XYZ isn't used, rather .SHT (shot
      * variant, produced by CMAP v16 and later), .UNA (unadjusted) and .ADJ
      * (adjusted) extensions are.  Since img has long checked for .XYZ, we
      * continue to do so in case anyone is relying on it.
      */
     case EXT3('s', 'h', 't'): /* CMAP .sht */
     case EXT3('a', 'd', 'j'): /* CMAP .adj */
     case EXT3('u', 'n', 'a'): /* CMAP .una */
     case EXT3('x', 'y', 'z'): /* CMAP .xyz */ {
       int result;
xyz_file:
       result = cmap_xyz_open(pimg);
       if (result) {
	   img_errno = result;
	   goto error;
       }
       goto successful_return;
     }
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
   ch = GETC(pimg->fh);
   pimg->version = 0;
   if (tolower(ch) == 'b') {
      /* binary file iff B/b prefix */
      pimg->version = 1;
      ch = GETC(pimg->fh);
   }
   if (ch != 'v') {
      img_errno = IMG_BADFORMAT;
      goto error;
   }
   ch = GETC(pimg->fh);
   if (ch == '0') {
      if (fread(buf, 4, 1, pimg->fh) != 1 || memcmp(buf, ".01\n", 4) != 0) {
	 img_errno = IMG_BADFORMAT;
	 goto error;
      }
      /* nothing special to do */
   } else if (pimg->version == 0) {
      if (ch < '2' || ch > '0' + IMG_VERSION_MAX || GETC(pimg->fh) != '\n') {
	 img_errno = IMG_TOONEW;
	 goto error;
      }
      pimg->version = ch - '0';
   } else {
      img_errno = IMG_BADFORMAT;
      goto error;
   }

v03d:
   {
       size_t title_len;
       char * title = getline_alloc_len(pimg->fh, &title_len);
       if (!title) goto out_of_memory_error;
       if (pimg->version == 8) {
	   /* We sneak in extra fields after a zero byte here, containing the
	    * specified coordinate system (if any) and the level separator
	    * character.  Older readers will just not see these fields (which
	    * is OK), and this trick avoids us having to bump the 3d format
	    * version.
	    */
	   size_t real_len = strlen(title);
	   if (real_len != title_len) {
	       char * cs = title + real_len + 1;
	       real_len += strlen(cs) + 1;
	       if (memcmp(cs, "+init=", 6) == 0) {
		   /* PROJ 5 and later don't handle +init=esri:<number> but
		    * that's what cavern used to put in .3d files for
		    * coordinate systems specified using ESRI codes.  We parse
		    * and convert the strings cavern used to generate and
		    * convert to the form ESRI:<number> which is still
		    * understood.
		    *
		    * PROJ 6 and later don't recognise +init=epsg:<number>
		    * by default and don't apply datum shift terms in some
		    * cases, so we also convert these to the form
		    * EPSG:<number>.
		    */
		   char * p = cs + 6;
		   if (p[4] == ':' && isdigit((unsigned char)p[5]) &&
		       ((memcmp(p, "epsg", 4) == 0 || memcmp(p, "esri", 4) == 0))) {
		       p = p + 6;
		       while (isdigit((unsigned char)*p)) {
			   ++p;
		       }
		       /* Allow +no_defs to be omitted as it seems to not
			* actually do anything with recent PROJ - cavern always
			* included it, but other software generating 3d files
			* may not.
			*/
		       if (*p == '\0' || strcmp(p, " +no_defs") == 0) {
			   int i;
			   cs = cs + 6;
			   for (i = 0; i < 4; ++i) {
			       cs[i] = toupper(cs[i]);
			   }
			   *p = '\0';
		       }
		   }
	       } else if (memcmp(cs, "+proj=", 6) == 0) {
		   /* Convert S_MERC and UTM proj strings which cavern used
		    * to generate to their corresponding EPSG:<number> codes.
		    */
		   char * p = cs + 6;
		   if (memcmp(p, "utm +ellps=WGS84 +datum=WGS84 +units=m +zone=", 45) == 0) {
		       int n = 0;
		       p += 45;
		       while (isdigit((unsigned char)*p)) {
			   n = n * 10 + (*p - '0');
			   ++p;
		       }
		       if (memcmp(p, " +south", 7) == 0) {
			   p += 7;
			   n += 32700;
		       } else {
			   n += 32600;
		       }
		       /* Allow +no_defs to be omitted as it seems to not
			* actually do anything with recent PROJ - cavern always
			* included it, but other software generating 3d files
			* may not have.
			*/
		       if (*p == '\0' || strcmp(p, " +no_defs") == 0) {
			   /* There are at least 45 bytes (see memcmp above)
			    * which is ample for EPSG: plus an integer.
			    */
			   SNPRINTF(cs, 45, "EPSG:%d", n);
		       }
		   } else if (memcmp(p, "merc +lat_ts=0 +lon_0=0 +k=1 +x_0=0 +y_0=0 +a=6378137 +b=6378137 +units=m +nadgrids=@null", 89) == 0) {
		       p = p + 89;
		       /* Allow +no_defs to be omitted as it seems to not
			* actually do anything with recent PROJ - cavern always
			* included it, but other software generating 3d files
			* may not have.
			*/
		       if (*p == '\0' || strcmp(p, " +no_defs") == 0) {
			   strcpy(cs, "EPSG:3857");
		       }
		   }
	       }
	       if (cs[0]) pimg->cs = my_strdup(cs);
	   }

	   if (real_len != title_len) {
	       pimg->separator = title[real_len + 1];
	   }
       }
       if (!pimg->title) {
	   pimg->title = title;
       } else {
	   osfree(title);
       }
   }
   pimg->datestamp = getline_alloc(pimg->fh);
   if (!pimg->datestamp) {
out_of_memory_error:
      img_errno = IMG_OUTOFMEMORY;
error:
      osfree(pimg->title);
      osfree(pimg->cs);
      osfree(pimg->datestamp);
      osfree(pimg->filename_opened);
      if (pimg->close_func) pimg->close_func(pimg->fh);
      osfree(pimg);
      return NULL;
   }

   if (pimg->version >= 8) {
      int flags = GETC(pimg->fh);
      if (flags & img_FFLAG_EXTENDED) pimg->is_extended_elevation = 1;
   } else if (pimg->title) {
      len = strlen(pimg->title);
      if (len > 11 && strcmp(pimg->title + len - 11, " (extended)") == 0) {
	  pimg->title[len - 11] = '\0';
	  pimg->is_extended_elevation = 1;
      }
   }

   if (pimg->datestamp[0] == '@') {
      unsigned long v;
      char * p;
      errno = 0;
      v = strtoul(pimg->datestamp + 1, &p, 10);
      if (errno == 0 && *p == '\0')
	 pimg->datestamp_numeric = v;
      /* FIXME: We're assuming here that the C time_t epoch is 1970, which is
       * true for Unix-like systems, macOS and Windows, but isn't guaranteed
       * by ISO C.
       */
   } else {
      /* %a,%Y.%m.%d %H:%M:%S %Z */
      struct tm tm;
      unsigned long v;
      char * p = pimg->datestamp;
      while (isalpha((unsigned char)*p)) ++p;
      if (*p == ',') ++p;
      while (isspace((unsigned char)*p)) ++p;
      v = strtoul(p, &p, 10);
      if (v == ULONG_MAX || *p++ != '.')
	 goto bad_3d_date;
      tm.tm_year = v - 1900;
      v = strtoul(p, &p, 10);
      if (v < 1 || v > 12 || *p++ != '.')
	 goto bad_3d_date;
      tm.tm_mon = v - 1;
      v = strtoul(p, &p, 10);
      if (v < 1 || v > 31 || *p++ != ' ')
	 goto bad_3d_date;
      tm.tm_mday = v;
      v = strtoul(p, &p, 10);
      if (v >= 24 || *p++ != ':')
	 goto bad_3d_date;
      tm.tm_hour = v;
      v = strtoul(p, &p, 10);
      if (v >= 60 || *p++ != ':')
	 goto bad_3d_date;
      tm.tm_min = v;
      v = strtoul(p, &p, 10);
      if (v > 60)
	 goto bad_3d_date;
      tm.tm_sec = v;
      tm.tm_isdst = 0;
      while (isspace((unsigned char)*p)) ++p;
      /* p now points to the timezone string.
       *
       * However, it's likely to be a string like "BST", and such strings can
       * be ambiguous (BST could be UTC+1 or UTC+6), so it is impossible to
       * reliably convert in all cases.  Just pass what we have to tzset() - if
       * it doesn't handle it, UTC will be used.
       */
      pimg->datestamp_numeric = mktime_with_tz(&tm, p);
   }
bad_3d_date:

   pimg->start = ftell(pimg->fh);

successful_return:
   /* If no title from another source, default to the base leafname. */
   if (!pimg->title || !pimg->title[0]) {
       osfree(pimg->title);
       pimg->title = baseleaf_from_fnm(fnm);
   }
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
   /* [VERSION_SURVEX_POS] already skipped heading line, or there wasn't one
    * [version 0] not in the middle of a 'LINE' command
    * [version >= 3] not in the middle of turning a LINE into a MOVE */
   pimg->pending = 0;

   img_errno = IMG_NONE;

   /* for version >= 3 we use label_buf to store the prefix for reuse */
   /* for VERSION_COMPASS_PLT, 0 value indicates we haven't entered a survey
    * yet */
   /* for VERSION_CMAP_SHOT, we store the last station here to detect whether
    * we MOVE or LINE */
   pimg->label_len = 0;
   pimg->style = img_STYLE_UNKNOWN;
   return 1;
}

img *
img_open_write_cs(const char *fnm, const char *title, const char *cs, int flags)
{
   if (fDirectory(fnm)) {
      img_errno = IMG_DIRECTORY;
      return NULL;
   }

   return img_write_stream(fopen(fnm, "wb"), fclose, title, cs, flags);
}

img *
img_write_stream(FILE *stream, int (*close_func)(FILE*),
		 const char *title, const char *cs, int flags)
{
   time_t tm;
   img *pimg;

   if (stream == NULL) {
      img_errno = IMG_FILENOTFOUND;
      return NULL;
   }

   pimg = osnew(img);
   if (pimg == NULL) {
      img_errno = IMG_OUTOFMEMORY;
      if (close_func) close_func(stream);
      return NULL;
   }

   pimg->fh = stream;
   pimg->close_func = close_func;
   pimg->buf_len = 257;
   pimg->label_buf = (char *)xosmalloc(pimg->buf_len);
   if (!pimg->label_buf) {
      if (pimg->close_func) pimg->close_func(pimg->fh);
      osfree(pimg);
      img_errno = IMG_OUTOFMEMORY;
      return NULL;
   }

   pimg->filename_opened = NULL;
   pimg->data = NULL;

   pimg->separator = (flags & 0x100) ? (flags >> 9) : '.';

   /* Output image file header */
   fputs("Survex 3D Image File\n", pimg->fh); /* file identifier string */
   if (img_output_version < 2) {
      pimg->version = 1;
      fputs("Bv0.01\n", pimg->fh); /* binary file format version number */
   } else {
      pimg->version = (img_output_version > IMG_VERSION_MAX) ? IMG_VERSION_MAX : img_output_version;
      fprintf(pimg->fh, "v%d\n", pimg->version); /* file format version no. */
   }

   fputs(title, pimg->fh);
   if (pimg->version < 8 && (flags & img_FFLAG_EXTENDED)) {
      /* Older format versions append " (extended)" to the title to mark
       * extended elevations. */
      size_t len = strlen(title);
      if (len < 11 || strcmp(title + len - 11, " (extended)") != 0)
	 fputs(" (extended)", pimg->fh);
   }
   if (pimg->version == 8 && ((cs && *cs) || pimg->separator != '.')) {
      /* We sneak in extra fields after a zero byte here, containing the
       * specified coordinate system (if any) and the separator character
       * if it isn't the default of '.'.  Older readers will just not see
       * these (which is fine for the coordinate system, and not very
       * problematic for the separator), and this trick avoids us having to
       * bump the 3d format version.
       */
      PUTC('\0', pimg->fh);
      if (cs && *cs) fputs(cs, pimg->fh);
      if (pimg->separator != '.') {
	  PUTC('\0', pimg->fh);
	  PUTC(pimg->separator, pimg->fh);
      }
   }
   PUTC('\n', pimg->fh);

   if (getenv("SOURCE_DATE_EPOCH")) {
      /* Support reproducible builds which create .3d files by not embedding a
       * timestamp if SOURCE_DATE_EPOCH is set.  We don't bother trying to
       * parse the timestamp as it is simpler and seems cleaner to just not
       * embed a timestamp at all given the 3d file format already provides
       * a way not to.
       *
       * See https://reproducible-builds.org/docs/source-date-epoch/
       */
      tm = (time_t)-1;
   } else {
      tm = time(NULL);
   }

   if (tm == (time_t)-1) {
      fputsnl(TIMENA, pimg->fh);
   } else if (pimg->version <= 7) {
      char date[256];
      /* output current date and time in format specified */
      strftime(date, 256, TIMEFMT, localtime(&tm));
      fputsnl(date, pimg->fh);
   } else {
      fprintf(pimg->fh, "@%ld\n", (long)tm);
   }

   if (pimg->version >= 8) {
      /* Clear bit one in case anyone has been passing true for fBinary. */
      flags &=~ 1;
      PUTC(flags, pimg->fh);
   }

#if 0
   if (img_output_version >= 5) {
       static const unsigned char codelengths[32] = {
	   4,  8,  8,  16, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
       };
       fwrite(codelengths, 32, 1, pimg->fh);
   }
#endif
   pimg->fRead = 0; /* writing to this file */
   img_errno = IMG_NONE;

   /* for version >= 3 we use label_buf to store the prefix for reuse */
   pimg->label_buf[0] = '\0';
   pimg->label_len = 0;

#if IMG_API_VERSION == 0
   pimg->date1 = pimg->date2 = 0;
   pimg->olddate1 = pimg->olddate2 = 0;
#else /* IMG_API_VERSION == 1 */
   pimg->days1 = pimg->days2 = -1;
   pimg->olddays1 = pimg->olddays2 = -1;
#endif
   pimg->style = pimg->oldstyle = img_STYLE_UNKNOWN;

   pimg->l = pimg->r = pimg->u = pimg->d = -1.0;

   pimg->n_legs = 0;
   pimg->length = 0.0;
   pimg->E = pimg->H = pimg->V = 0.0;

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

static int
read_v3label(img *pimg)
{
   char *q;
   long len = GETC(pimg->fh);
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
   return 0;
}

static int
read_v8label(img *pimg, int common_flag, size_t common_val)
{
   char *q;
   size_t del, add;
   if (common_flag) {
      if (common_val == 0) return 0;
      add = del = common_val;
   } else {
      int ch = GETC(pimg->fh);
      if (ch == EOF) {
	 img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	 return img_BAD;
      }
      if (ch != 0x00) {
	 del = ch >> 4;
	 add = ch & 0x0f;
      } else {
	 ch = GETC(pimg->fh);
	 if (ch == EOF) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 if (ch != 0xff) {
	    del = ch;
	 } else {
	    del = get32(pimg->fh);
	    if (ferror(pimg->fh)) {
	       img_errno = IMG_READERROR;
	       return img_BAD;
	    }
	 }
	 ch = GETC(pimg->fh);
	 if (ch == EOF) {
	    img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	    return img_BAD;
	 }
	 if (ch != 0xff) {
	    add = ch;
	 } else {
	    add = get32(pimg->fh);
	    if (ferror(pimg->fh)) {
	       img_errno = IMG_READERROR;
	       return img_BAD;
	    }
	 }
      }

      if (add > del && !check_label_space(pimg, pimg->label_len + add - del + 1)) {
	 img_errno = IMG_OUTOFMEMORY;
	 return img_BAD;
      }
   }
   if (del > pimg->label_len) {
      img_errno = IMG_BADFORMAT;
      return img_BAD;
   }
   pimg->label_len -= del;
   q = pimg->label_buf + pimg->label_len;
   pimg->label_len += add;
   if (add && fread(q, add, 1, pimg->fh) != 1) {
      img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
      return img_BAD;
   }
   q[add] = '\0';
   return 0;
}

static int img_read_item_new(img *pimg, img_point *p);
static int img_read_item_v3to7(img *pimg, img_point *p);
static int img_read_item_ancient(img *pimg, img_point *p);
static int img_read_item_ascii_wrapper(img *pimg, img_point *p);
static int img_read_item_ascii(img *pimg, img_point *p);

int
img_read_item(img *pimg, img_point *p)
{
   pimg->flags = 0;

   if (pimg->version >= 8) {
      return img_read_item_new(pimg, p);
   } else if (pimg->version >= 3) {
      return img_read_item_v3to7(pimg, p);
   } else if (pimg->version >= 1) {
      return img_read_item_ancient(pimg, p);
   } else {
      return img_read_item_ascii_wrapper(pimg, p);
   }
}

static int
img_read_item_new(img *pimg, img_point *p)
{
   int result;
   int opt;
   pimg->l = pimg->r = pimg->u = pimg->d = -1.0;
   if (pimg->pending >= 0x40) {
      if (pimg->pending == PENDING_XSECT_END) {
	 pimg->pending = 0;
	 return img_XSECT_END;
      }
      *p = pimg->mv;
      pimg->flags = (int)(pimg->pending) & 0x3f;
      pimg->pending = 0;
      return img_LINE;
   }
   again3: /* label to goto if we get a prefix, date, or lrud */
   pimg->label = pimg->label_buf;
   opt = GETC(pimg->fh);
   if (opt == EOF) {
      img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
      return img_BAD;
   }
   if (opt >> 6 == 0) {
      if (opt <= 4) {
	 if (opt == 0 && pimg->style == 0)
	    return img_STOP; /* end of data marker */
	 /* STYLE */
	 pimg->style = opt;
	 goto again3;
      }
      if (opt >= 0x10) {
	  switch (opt) {
	      case 0x10: { /* No date info */
#if IMG_API_VERSION == 0
		  pimg->date1 = pimg->date2 = 0;
#else /* IMG_API_VERSION == 1 */
		  pimg->days1 = pimg->days2 = -1;
#endif
		  break;
	      }
	      case 0x11: { /* Single date */
		  int days1 = (int)getu16(pimg->fh);
#if IMG_API_VERSION == 0
		  pimg->date2 = pimg->date1 = (days1 - DAYS_1900) * SECS_PER_DAY;
#else /* IMG_API_VERSION == 1 */
		  pimg->days2 = pimg->days1 = days1;
#endif
		  break;
	      }
	      case 0x12: { /* Date range (short) */
		  int days1 = (int)getu16(pimg->fh);
		  int days2 = days1 + GETC(pimg->fh) + 1;
#if IMG_API_VERSION == 0
		  pimg->date1 = (days1 - DAYS_1900) * SECS_PER_DAY;
		  pimg->date2 = (days2 - DAYS_1900) * SECS_PER_DAY;
#else /* IMG_API_VERSION == 1 */
		  pimg->days1 = days1;
		  pimg->days2 = days2;
#endif
		  break;
	      }
	      case 0x13: { /* Date range (long) */
		  int days1 = (int)getu16(pimg->fh);
		  int days2 = (int)getu16(pimg->fh);
#if IMG_API_VERSION == 0
		  pimg->date1 = (days1 - DAYS_1900) * SECS_PER_DAY;
		  pimg->date2 = (days2 - DAYS_1900) * SECS_PER_DAY;
#else /* IMG_API_VERSION == 1 */
		  pimg->days1 = days1;
		  pimg->days2 = days2;
#endif
		  break;
	      }
	      case 0x1f: /* Error info */
		  pimg->n_legs = get32(pimg->fh);
		  pimg->length = get32(pimg->fh) / 100.0;
		  pimg->E = get32(pimg->fh) / 100.0;
		  pimg->H = get32(pimg->fh) / 100.0;
		  pimg->V = get32(pimg->fh) / 100.0;
		  return img_ERROR_INFO;
	      case 0x30: case 0x31: /* LRUD */
	      case 0x32: case 0x33: /* Big LRUD! */
		  if (read_v8label(pimg, 0, 0) == img_BAD) return img_BAD;
		  pimg->flags = (int)opt & 0x01;
		  if (opt < 0x32) {
		      pimg->l = get16(pimg->fh) / 100.0;
		      pimg->r = get16(pimg->fh) / 100.0;
		      pimg->u = get16(pimg->fh) / 100.0;
		      pimg->d = get16(pimg->fh) / 100.0;
		  } else {
		      pimg->l = get32(pimg->fh) / 100.0;
		      pimg->r = get32(pimg->fh) / 100.0;
		      pimg->u = get32(pimg->fh) / 100.0;
		      pimg->d = get32(pimg->fh) / 100.0;
		  }
		  if (!stn_included(pimg)) {
		      return img_XSECT_END;
		  }
		  /* If this is the last cross-section in this passage, set
		   * pending so we return img_XSECT_END next time. */
		  if (pimg->flags & 0x01) {
		      pimg->pending = PENDING_XSECT_END;
		      pimg->flags &= ~0x01;
		  }
		  return img_XSECT;
	      default: /* 0x25 - 0x2f and 0x34 - 0x3f are currently unallocated. */
		  img_errno = IMG_BADFORMAT;
		  return img_BAD;
	  }
	  goto again3;
      }
      if (opt != 15) {
	 /* 1-14 and 16-31 reserved */
	 img_errno = IMG_BADFORMAT;
	 return img_BAD;
      }
      result = img_MOVE;
   } else if (opt >= 0x80) {
      if (read_v8label(pimg, 0, 0) == img_BAD) return img_BAD;

      result = img_LABEL;

      if (!stn_included(pimg)) {
	 if (!skip_coord(pimg->fh)) return img_BAD;
	 pimg->pending = 0;
	 goto again3;
      }

      pimg->flags = (int)opt & 0x7f;
   } else if ((opt >> 6) == 1) {
      if (read_v8label(pimg, opt & 0x20, 0) == img_BAD) return img_BAD;

      result = img_LINE;

      if (!survey_included(pimg)) {
	 if (!read_coord(pimg->fh, &(pimg->mv))) return img_BAD;
	 pimg->pending = 15;
	 goto again3;
      }

      if (pimg->pending) {
	 *p = pimg->mv;
	 if (!read_coord(pimg->fh, &(pimg->mv))) return img_BAD;
	 pimg->pending = opt;
	 return img_MOVE;
      }
      pimg->flags = (int)opt & 0x1f;
   } else {
      img_errno = IMG_BADFORMAT;
      return img_BAD;
   }
   if (!read_coord(pimg->fh, p)) return img_BAD;
   pimg->pending = 0;
   return result;
}

static int
img_read_item_v3to7(img *pimg, img_point *p)
{
   int result;
   int opt;
   pimg->l = pimg->r = pimg->u = pimg->d = -1.0;
   if (pimg->pending == PENDING_XSECT_END) {
      pimg->pending = 0;
      return img_XSECT_END;
   }
   if (pimg->pending >= 0x80) {
      *p = pimg->mv;
      pimg->flags = (int)(pimg->pending) & 0x3f;
      pimg->pending = 0;
      return img_LINE;
   }
   again3: /* label to goto if we get a prefix, date, or lrud */
   pimg->label = pimg->label_buf;
   opt = GETC(pimg->fh);
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
	 if (pimg->label_len <= 17) {
	    /* zero prefix using "0" */
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 /* extra - 1 because label_len points to one past the end */
	 c = pimg->label_len - 17 - 1;
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
      if (opt >= 0x20) {
	  switch (opt) {
	      case 0x20: /* Single date */
		  if (pimg->version < 7) {
		      int date1 = get32(pimg->fh);
#if IMG_API_VERSION == 0
		      pimg->date2 = pimg->date1 = date1;
#else /* IMG_API_VERSION == 1 */
		      if (date1 != 0) {
			  pimg->days1 = (date1 - TIME_T_1900) / SECS_PER_DAY;
			  pimg->days2 = pimg->days1;
		      } else {
			  pimg->days2 = pimg->days1 = -1;
		      }
#endif
		  } else {
		      int days1 = (int)getu16(pimg->fh);
#if IMG_API_VERSION == 0
		      pimg->date1 = (days1 - DAYS_1900) * SECS_PER_DAY;
		      pimg->date2 = pimg->date1;
#else /* IMG_API_VERSION == 1 */
		      pimg->days2 = pimg->days1 = days1;
#endif
		  }
		  break;
	      case 0x21: /* Date range (short for v7+) */
		  if (pimg->version < 7) {
		      INT32_T date1 = get32(pimg->fh);
		      INT32_T date2 = get32(pimg->fh);
#if IMG_API_VERSION == 0
		      pimg->date1 = date1;
		      pimg->date2 = date2;
#else /* IMG_API_VERSION == 1 */
		      pimg->days1 = (date1 - TIME_T_1900) / SECS_PER_DAY;
		      pimg->days2 = (date2 - TIME_T_1900) / SECS_PER_DAY;
#endif
		  } else {
		      int days1 = (int)getu16(pimg->fh);
		      int days2 = days1 + GETC(pimg->fh) + 1;
#if IMG_API_VERSION == 0
		      pimg->date1 = (days1 - DAYS_1900) * SECS_PER_DAY;
		      pimg->date2 = (days2 - DAYS_1900) * SECS_PER_DAY;
#else /* IMG_API_VERSION == 1 */
		      pimg->days1 = days1;
		      pimg->days2 = days2;
#endif
		  }
		  break;
	      case 0x22: /* Error info */
		  pimg->n_legs = get32(pimg->fh);
		  pimg->length = get32(pimg->fh) / 100.0;
		  pimg->E = get32(pimg->fh) / 100.0;
		  pimg->H = get32(pimg->fh) / 100.0;
		  pimg->V = get32(pimg->fh) / 100.0;
		  if (feof(pimg->fh)) {
		      img_errno = IMG_BADFORMAT;
		      return img_BAD;
		  }
		  if (ferror(pimg->fh)) {
		      img_errno = IMG_READERROR;
		      return img_BAD;
		  }
		  return img_ERROR_INFO;
	      case 0x23: { /* v7+: Date range (long) */
		  if (pimg->version < 7) {
		      img_errno = IMG_BADFORMAT;
		      return img_BAD;
		  }
		  int days1 = (int)getu16(pimg->fh);
		  int days2 = (int)getu16(pimg->fh);
		  if (feof(pimg->fh)) {
		      img_errno = IMG_BADFORMAT;
		      return img_BAD;
		  }
		  if (ferror(pimg->fh)) {
		      img_errno = IMG_READERROR;
		      return img_BAD;
		  }
#if IMG_API_VERSION == 0
		  pimg->date1 = (days1 - DAYS_1900) * SECS_PER_DAY;
		  pimg->date2 = (days2 - DAYS_1900) * SECS_PER_DAY;
#else /* IMG_API_VERSION == 1 */
		  pimg->days1 = days1;
		  pimg->days2 = days2;
#endif
		  break;
	      }
	      case 0x24: { /* v7+: No date info */
#if IMG_API_VERSION == 0
		  pimg->date1 = pimg->date2 = 0;
#else /* IMG_API_VERSION == 1 */
		  pimg->days1 = pimg->days2 = -1;
#endif
		  break;
	      }
	      case 0x30: case 0x31: /* LRUD */
	      case 0x32: case 0x33: /* Big LRUD! */
		  if (read_v3label(pimg) == img_BAD) return img_BAD;
		  pimg->flags = (int)opt & 0x01;
		  if (opt < 0x32) {
		      pimg->l = get16(pimg->fh) / 100.0;
		      pimg->r = get16(pimg->fh) / 100.0;
		      pimg->u = get16(pimg->fh) / 100.0;
		      pimg->d = get16(pimg->fh) / 100.0;
		  } else {
		      pimg->l = get32(pimg->fh) / 100.0;
		      pimg->r = get32(pimg->fh) / 100.0;
		      pimg->u = get32(pimg->fh) / 100.0;
		      pimg->d = get32(pimg->fh) / 100.0;
		  }
		  if (feof(pimg->fh)) {
		      img_errno = IMG_BADFORMAT;
		      return img_BAD;
		  }
		  if (ferror(pimg->fh)) {
		      img_errno = IMG_READERROR;
		      return img_BAD;
		  }
		  if (!stn_included(pimg)) {
		      return img_XSECT_END;
		  }
		  /* If this is the last cross-section in this passage, set
		   * pending so we return img_XSECT_END next time. */
		  if (pimg->flags & 0x01) {
		      pimg->pending = PENDING_XSECT_END;
		      pimg->flags &= ~0x01;
		  }
		  return img_XSECT;
	      default: /* 0x25 - 0x2f and 0x34 - 0x3f are currently unallocated. */
		  img_errno = IMG_BADFORMAT;
		  return img_BAD;
	  }
	  if (feof(pimg->fh)) {
	      img_errno = IMG_BADFORMAT;
	      return img_BAD;
	  }
	  if (ferror(pimg->fh)) {
	      img_errno = IMG_READERROR;
	      return img_BAD;
	  }
	  goto again3;
      }
      /* 16-31 mean remove (n - 15) characters from the prefix */
      /* zero prefix using 0 */
      if (pimg->label_len <= (size_t)(opt - 15)) {
	 img_errno = IMG_BADFORMAT;
	 return img_BAD;
      }
      pimg->label_len -= (opt - 15);
      goto again3;
    case 1:
      if (read_v3label(pimg) == img_BAD) return img_BAD;

      result = img_LABEL;

      if (!stn_included(pimg)) {
	  if (!skip_coord(pimg->fh)) return img_BAD;
	  pimg->pending = 0;
	  goto again3;
      }

      pimg->flags = (int)opt & 0x3f;
      break;
    case 2:
      if (read_v3label(pimg) == img_BAD) return img_BAD;

      result = img_LINE;

      if (!survey_included(pimg)) {
	  if (!read_coord(pimg->fh, &(pimg->mv))) return img_BAD;
	  pimg->pending = 15;
	  goto again3;
      }

      if (pimg->pending) {
	 *p = pimg->mv;
	 if (!read_coord(pimg->fh, &(pimg->mv))) return img_BAD;
	 pimg->pending = opt;
	 return img_MOVE;
      }
      pimg->flags = (int)opt & 0x3f;
      break;
    default:
      img_errno = IMG_BADFORMAT;
      return img_BAD;
   }
   if (!read_coord(pimg->fh, p)) return img_BAD;
   pimg->pending = 0;
   return result;
}

static int
img_read_item_ancient(img *pimg, img_point *p)
{
   int result;
   static long opt_lookahead = 0;
   static img_point pt = { 0.0, 0.0, 0.0 };
   long opt;

   again: /* label to goto if we get a cross */
   pimg->label = pimg->label_buf;
   pimg->label[0] = '\0';

   if (pimg->version == 1) {
      if (opt_lookahead) {
	 opt = opt_lookahead;
	 opt_lookahead = 0;
      } else {
	 opt = get32(pimg->fh);
      }
   } else {
      opt = GETC(pimg->fh);
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
      size_t len;
      result = img_LABEL;
      if (!fgets(pimg->label_buf, pimg->buf_len, pimg->fh)) {
	 img_errno = feof(pimg->fh) ? IMG_BADFORMAT : IMG_READERROR;
	 return img_BAD;
      }
      if (pimg->label[0] == '\\') pimg->label++;
      len = strlen(pimg->label);
      if (len == 0 || pimg->label[len - 1] != '\n') {
	 img_errno = IMG_BADFORMAT;
	 return img_BAD;
      }
      /* Ignore empty labels in some .3d files (caused by a bug) */
      if (len == 1) goto again;
      pimg->label[len - 1] = '\0';
      pimg->flags = img_SFLAG_UNDERGROUND; /* no flags given... */
      if (opt == 2) goto done;
      break;
    }
    case 6: case 7: {
      long len;
      result = img_LABEL;

      if (opt == 7)
	 pimg->flags = GETC(pimg->fh);
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
	 if (!fgets(pimg->label_buf, pimg->buf_len, pimg->fh)) {
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
       default:
	 img_errno = IMG_BADFORMAT;
	 return img_BAD;
      }
      break;
   }

   if (!read_coord(pimg->fh, &pt)) return img_BAD;

   if (result == img_LABEL && !stn_included(pimg)) {
       goto again;
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

      if (opt_lookahead == 2) return img_read_item_ancient(pimg, p);
   }

   return result;
}

static int
img_read_item_ascii_wrapper(img *pimg, img_point *p)
{
   /* We need to set the default locale for fscanf() to work on
    * numbers with "." as decimal point. */
   int result;
   char * current_locale = my_strdup(setlocale(LC_NUMERIC, NULL));
   setlocale(LC_NUMERIC, "C");
   result = img_read_item_ascii(pimg, p);
   setlocale(LC_NUMERIC, current_locale);
   free(current_locale);
   return result;
}

/* Handle all ASCII formats. */
static int
img_read_item_ascii(img *pimg, img_point *p)
{
   int result;
   pimg->label = pimg->label_buf;
   if (pimg->version == 0) {
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
	    int ch = GETC(pimg->fh);
	    if (ch == ' ') ch = GETC(pimg->fh);
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
	       ch = GETC(pimg->fh);
	    }
	    pimg->label_buf[off] = '\0';

	    pimg->label = pimg->label_buf;
	    if (pimg->label[0] == '\\') pimg->label++;

	    pimg->flags = img_SFLAG_UNDERGROUND; /* default flags */

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

      if (result == img_LABEL && !stn_included(pimg)) {
	  goto ascii_again;
      }

      return result;
   } else if (pimg->version == VERSION_SURVEX_POS) {
      /* Survex .pos file */
      int ch;
      size_t off;
      pimg->flags = img_SFLAG_UNDERGROUND; /* default flags */
      againpos:
      while (fscanf(pimg->fh, "(%lf,%lf,%lf )", &p->x, &p->y, &p->z) != 3) {
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
	    ch = GETC(pimg->fh);
	 } while (ch != '\n' && ch != '\r' && ch != EOF);
      }

      pimg->label_buf[0] = '\0';
      do {
	  ch = GETC(pimg->fh);
      } while (ch == ' ' || ch == '\t');
      if (ch == '\n' || ch == EOF) {
	  /* If there's no label, set img_SFLAG_ANON. */
	  pimg->flags |= img_SFLAG_ANON;
	  return img_LABEL;
      }
      pimg->label_buf[0] = ch;
      off = 1;
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

      if (!stn_included(pimg)) goto againpos;

      return img_LABEL;
   } else if (pimg->version == VERSION_COMPASS_PLT) {
      /* Compass .plt file */
      if ((pimg->pending & ~PENDING_HAD_XSECT) > 0) {
	 /* -1 signals we've entered the first survey we want to read, and
	  * need to fudge lots if the first action is 'D' or 'd'...
	  */
	 pimg->flags = 0;
	 if (pimg->pending & PENDING_XSECT_END) {
	     /* pending XSECT_END */
	     pimg->pending &= ~PENDING_XSECT_END;
	     return img_XSECT_END;
	 }
	 if (pimg->pending & PENDING_XSECT) {
	     /* pending XSECT */
	     pimg->pending &= ~PENDING_XSECT;
	     return img_XSECT;
	 }
	 pimg->label[pimg->label_len] = '\0';
	 if (pimg->pending & PENDING_LINE) {
	     pimg->flags = (pimg->pending >> PENDING_FLAGS_SHIFT);
	     pimg->pending &= ((1 << PENDING_FLAGS_SHIFT) - 1) & ~PENDING_LINE;
	     return img_LINE;
	 }
	 pimg->pending &= ~PENDING_MOVE;
	 return img_MOVE;
      }

      while (1) {
	 char *line;
	 char *q;
	 size_t len = 0;
	 int ch = GETC(pimg->fh);

	 switch (ch) {
	    case '\x1a': case EOF: /* Don't insist on ^Z at end of file */
	       if (pimg->pending == PENDING_HAD_XSECT) {
		   ungetc('\x1a', pimg->fh);
		   pimg->pending = 0;
		   return img_XSECT_END;
	       }
	       return img_STOP;
	    case 'X': case 'F': case 'S':
	       /* bounding boX (marks end of survey), Feature survey, or
		* new Section - skip to next survey */
	       if (pimg->pending == PENDING_HAD_XSECT) {
		   ungetc(ch, pimg->fh);
		   pimg->pending = 0;
		   return img_XSECT_END;
	       }
	       if (pimg->survey) return img_STOP;
skip_to_N:
	       while (1) {
		  do {
		     ch = GETC(pimg->fh);
		  } while (ch != '\n' && ch != '\r' && ch != EOF);
		  while (ch == '\n' || ch == '\r') ch = GETC(pimg->fh);
		  if (ch == 'N') break;
		  if (ch == '\x1a' || ch == EOF) return img_STOP;
	       }
	       /* FALLTHRU */
	    case 'N':
	       compass_plt_new_survey(pimg);
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
	       /* Handle the survey date. */
	       while (line[len] && line[len] <= 32) ++len;
	       if (line[len] == 'D') {
		  struct tm tm;
		  memset(&tm, 0, sizeof(tm));
		  unsigned long v;
		  q = line + len + 1;
		  /* NB Order is Month Day Year order. */
		  v = strtoul(q, &q, 10);
		  if (v < 1 || v > 12)
		     goto bad_plt_date;
		  tm.tm_mon = v - 1;

		  v = strtoul(q, &q, 10);
		  if (v < 1 || v > 31)
		     goto bad_plt_date;
		  tm.tm_mday = v;

		  v = strtoul(q, &q, 10);
		  if (v == ULONG_MAX)
		     goto bad_plt_date;
		  if (v < 1900) {
		     /* "The Year is expected to be the full year like 1994 not
		      * 94", but "expected to" != "must" so treat a two digit
		      * year as 19xx.
		      */
		     v += 1900;
		  }
		  if (v == 1901 && tm.tm_mday == 1 && tm.tm_mon == 0) {
		     /* Compass uses 1/1/1 or 1/1/1901 for "date unknown". */
		     goto bad_plt_date;
		  }
		  tm.tm_year = v - 1900;
		  /* We have no indication of what timezone this date is
		   * in.  It's probably local time for whoever processed the
		   * data, so just assume noon in UTC, which is at least fairly
		   * central in the possibilities.
		   */
		  tm.tm_hour = 12;
		  {
		     time_t datestamp = mktime_with_tz(&tm, "");
#if IMG_API_VERSION == 0
		     pimg->date1 = pimg->date2 = datestamp;
#else /* IMG_API_VERSION == 1 */
		     pimg->days1 = (datestamp - TIME_T_1900) / SECS_PER_DAY;
		     pimg->days2 = pimg->days1;
#endif
		  }
	       } else {
bad_plt_date:
#if IMG_API_VERSION == 0
		  pimg->date1 = pimg->date2 = 0;
#else /* IMG_API_VERSION == 1 */
		  pimg->days1 = pimg->days2 = -1;
#endif
	       }
	       osfree(line);
	       break;
	    case 'M':
	       if (pimg->pending == PENDING_HAD_XSECT) {
		   pimg->pending = PENDING_XSECT_END;
	       }
	       /* FALLTHRU */
	    case 'D':
	    case 'd': {
	       /* Move or Draw */
	       unsigned shot_flags = (ch == 'd' ? img_FLAG_SURFACE : 0);
	       long fpos = -1;
	       if (pimg->survey && pimg->label_len == 0) {
		  /* We're only holding onto this line in case the first line
		   * of the 'N' is a 'D', so skip it for now...
		   */
		  goto skip_to_N;
	       }
	       if (pimg->pending == -1) {
		   pimg->pending = 0;
		   if (ch != 'M') {
		       if (pimg->survey) {
			   fpos = ftell(pimg->fh) - 1;
			   fseek(pimg->fh, pimg->start, SEEK_SET);
			   ch = GETC(pimg->fh);
		       } else {
			   /* If a file actually has a 'D' or 'd' before any
			    * 'M', then pretend the action is 'M' - one of the
			    * examples in the docs was like this!
			    */
			   ch = 'M';
		       }
		   }
	       }
	       line = getline_alloc(pimg->fh);
	       if (!line) {
		  img_errno = IMG_OUTOFMEMORY;
		  return img_BAD;
	       }
	       /* Compass stores coordinates as North, East, Up = (y,x,z)! */
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
	       /* Add 2 for ' ' before and terminating '\0'. */
	       if (!check_label_space(pimg, pimg->label_len + len + 2)) {
		  img_errno = IMG_OUTOFMEMORY;
		  return img_BAD;
	       }
	       pimg->flags = compass_plt_get_station_flags(pimg, q, len);
	       pimg->label = pimg->label_buf;
	       if (pimg->label_len) {
		   pimg->label[pimg->label_len] = ' ';
		   memcpy(pimg->label + pimg->label_len + 1, q, len);
		   pimg->label[pimg->label_len + 1 + len] = '\0';
	       } else {
		   memcpy(pimg->label, q, len);
		   pimg->label[len] = '\0';
	       }
	       q += len;

	       /* Now read LRUD.  Technically, this is optional but virtually
		* all PLT files have it (with dummy negative values if no LRUD
		* was recorded) and some versions of Compass can't read PLT
		* files without it!
		*/
	       while (*q && *q <= ' ') q++;
	       if (*q == 'P') {
		   double dim[4];
		   int bytes_used;
		   ++q;
		   if (sscanf(q, "%lf%lf%lf%lf%n",
			      &dim[0], &dim[1], &dim[2], &dim[3],
			      &bytes_used) != 4) {
		       osfree(line);
		       if (ferror(pimg->fh)) {
			   img_errno = IMG_READERROR;
		       } else {
			   img_errno = IMG_BADFORMAT;
		       }
		       return img_BAD;
		   }
		   q += bytes_used;

		   // No cross-sections for surface data.
		   if ((pimg->flags & img_SFLAG_UNDERGROUND)) {
		       int have_xsect = 0;
		       int i;
		       for (i = 0; i < 4; ++i) {
			   // The PLT format specification says 'Values less
			   // than zero are considered to be missing or
			   // “Passage.”' but Compass has an (apparently
			   // undocumented) extra check here for compatibility
			   // with data that was originally entered in Karst
			   // which uses 999 instead.
			   //
			   // Larry Fish says the check Compass actually uses
			   // when processing PLT files is:
			   //
			   // if (Left<0) or (Left>900)
			   if (dim[i] < 0.0 || dim[i] > 900.0) {
			       dim[i] = -1.0;
			   } else {
			       dim[i] *= METRES_PER_FOOT;
			       have_xsect = 1;
			   }
		       }
		       if (!have_xsect) goto no_xsect;
		       pimg->l = dim[0];
		       pimg->r = dim[1];
		       pimg->u = dim[2];
		       pimg->d = dim[3];
		       pimg->pending |= PENDING_XSECT | PENDING_HAD_XSECT;
		   } else {
		       goto no_xsect;
		   }
	       } else {
no_xsect:
		   pimg->l = pimg->r = pimg->u = pimg->d = -1.0;
		   if (pimg->pending == PENDING_HAD_XSECT) {
		       pimg->pending = PENDING_XSECT_END;
		   }
	       }
	       while (*q && *q <= ' ') q++;
	       if (*q == 'I') {
		   /* Skip distance from entrance. */
		   do ++q; while (*q && *q <= ' ');
		   while (*q > ' ') q++;
		   while (*q && *q <= ' ') q++;
	       }
	       if (*q == 'F') {
		   /* "Shot Flags".  Defined flags we currently ignore here:
		    * C: "Do not adjust this shot when closing loops."
		    * X: "you will never see this flag in a plot file."
		    */
		   while (isalpha((unsigned char)*++q)) {
		       switch (*q) {
			 case 'L':
			   shot_flags |= img_FLAG_DUPLICATE;
			   break;
			 case 'S':
			   shot_flags |= img_FLAG_SPLAY;
			   break;
			 case 'P':
			   /* P is "Exclude this shot from plotting", but the
			    * use suggested in the Compass docs is for surface
			    * data, and they "[do] not support passage
			    * modeling".
			    *
			    * Even if it's actually being used for a different
			    * purpose, Survex programs don't show surface legs
			    * by default so img_FLAG_SURFACE matches fairly
			    * well.
			    */
			   shot_flags |= img_FLAG_SURFACE;
			   break;
		       }
		   }
	       }
	       if (shot_flags & img_FLAG_SURFACE) {
		   /* Suppress passage? */
	       }
	       osfree(line);
	       if (fpos != -1) {
		   fseek(pimg->fh, fpos, SEEK_SET);
	       }

	       if (pimg->flags < 0) {
		   pimg->flags = shot_flags;
		   /* We've already emitted img_LABEL for this station. */
		   if (ch == 'M') {
		       return img_MOVE;
		   }
		   return img_LINE;
	       }
	       if (fpos == -1) {
		   if (ch == 'M') {
		       pimg->pending |= PENDING_MOVE;
		   } else {
		       pimg->pending |= PENDING_LINE | (shot_flags << PENDING_FLAGS_SHIFT);
		   }
	       }

	       return img_LABEL;
	    }
	    default:
	       img_errno = IMG_BADFORMAT;
	       return img_BAD;
	 }
      }
   } else {
      /* CMAP .xyz file */
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
      if (pimg->version == VERSION_CMAP_STATION) {
	 /* station variant */
	 if (len < 37) {
	    osfree(line);
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }
	 memcpy(pimg->label, line, 6);
	 q = (char *)memchr(pimg->label, ' ', 6);
	 if (!q) q = pimg->label + 6;
	 *q = '\0';

	 read_xyz_station_coords(p, line);

	 /* FIXME: look at prev for lines (line + 32, 5) */
	 /* FIXME: duplicate stations... */
	 return img_LABEL;
      } else {
	 /* Shot variant (VERSION_CMAP_SHOT) */
	 char old[8], new_[8];
	 if (len < 61) {
	    osfree(line);
	    img_errno = IMG_BADFORMAT;
	    return img_BAD;
	 }

	 memcpy(old, line, 7);
	 q = (char *)memchr(old, ' ', 7);
	 if (!q) q = old + 7;
	 *q = '\0';

	 memcpy(new_, line + 7, 7);
	 q = (char *)memchr(new_, ' ', 7);
	 if (!q) q = new_ + 7;
	 *q = '\0';

	 pimg->flags = img_SFLAG_UNDERGROUND;

	 if (strcmp(old, new_) == 0) {
	    pimg->pending = img_MOVE + 4;
	    read_xyz_shot_coords(p, line);
	    strcpy(pimg->label, new_);
	    osfree(line);
	    return img_LABEL;
	 }

	 if (strcmp(old, pimg->label) == 0) {
	    pimg->pending = img_LINE + 4;
	    read_xyz_shot_coords(p, line);
	    strcpy(pimg->label, new_);
	    osfree(line);
	    return img_LABEL;
	 }

	 pimg->pending = img_LABEL + 4;
	 read_xyz_shot_coords(p, line);
	 strcpy(pimg->label, new_);
	 memcpy(pimg->label + 16, line, 70);

	 osfree(line);
	 return img_LABEL;
      }
   }
}

static void
write_coord(FILE *fh, double x, double y, double z)
{
   SVX_ASSERT(fh);
   /* Output in cm */
   INT32_T X = my_lround(x * 100.0);
   INT32_T Y = my_lround(y * 100.0);
   INT32_T Z = my_lround(z * 100.0);

   put32(X, fh);
   put32(Y, fh);
   put32(Z, fh);
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
      if (pimg->label_len) PUTC(0, pimg->fh);
   } else if (n <= 16) {
      if (n) PUTC(n + 15, pimg->fh);
   } else if (dot == 0) {
      if (pimg->label_len) PUTC(0, pimg->fh);
      len = 0;
   } else {
      const char *p = pimg->label_buf + dot;
      n = 1;
      for (len = pimg->label_len - dot - 17; len; len--) {
	 if (*p++ == '.') n++;
      }
      if (n <= 14) {
	 PUTC(n, pimg->fh);
	 len = dot;
      } else {
	 if (pimg->label_len) PUTC(0, pimg->fh);
	 len = 0;
      }
   }

   n = strlen(s + len);
   PUTC(opt, pimg->fh);
   if (n < 0xfe) {
      PUTC(n, pimg->fh);
   } else if (n < 0xffff + 0xfe) {
      PUTC(0xfe, pimg->fh);
      put16((short)(n - 0xfe), pimg->fh);
   } else {
      PUTC(0xff, pimg->fh);
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

static int
write_v8label(img *pimg, int opt, int common_flag, size_t common_val,
	      const char *s)
{
   size_t len, del, add;

   /* find length of common prefix */
   for (len = 0; s[len] == pimg->label_buf[len] && s[len] != '\0'; len++) {
   }

   SVX_ASSERT(len <= pimg->label_len);
   del = pimg->label_len - len;
   add = strlen(s + len);

   if (add == common_val && del == common_val) {
      PUTC(opt | common_flag, pimg->fh);
   } else {
      PUTC(opt, pimg->fh);
      if (del <= 15 && add <= 15 && (del || add)) {
	 PUTC((del << 4) | add, pimg->fh);
      } else {
	 PUTC(0x00, pimg->fh);
	 if (del < 0xff) {
	    PUTC(del, pimg->fh);
	 } else {
	    PUTC(0xff, pimg->fh);
	    put32(del, pimg->fh);
	 }
	 if (add < 0xff) {
	    PUTC(add, pimg->fh);
	 } else {
	    PUTC(0xff, pimg->fh);
	    put32(add, pimg->fh);
	 }
      }
   }

   if (add)
      fwrite(s + len, add, 1, pimg->fh);

   pimg->label_len = len + add;
   if (add > del && !check_label_space(pimg, pimg->label_len + 1))
      return 0; /* FIXME: distinguish out of memory... */

   memcpy(pimg->label_buf + len, s + len, add + 1);

   return !ferror(pimg->fh);
}

static void
img_write_item_date_new(img *pimg)
{
    int same, unset;
    /* Only write dates when they've changed. */
#if IMG_API_VERSION == 0
    if (pimg->date1 == pimg->olddate1 && pimg->date2 == pimg->olddate2)
	return;

    same = (pimg->date1 == pimg->date2);
    unset = (pimg->date1 == 0);
#else /* IMG_API_VERSION == 1 */
    if (pimg->days1 == pimg->olddays1 && pimg->days2 == pimg->olddays2)
	return;

    same = (pimg->days1 == pimg->days2);
    unset = (pimg->days1 == -1);
#endif

    if (same) {
	if (unset) {
	    PUTC(0x10, pimg->fh);
	} else {
	    PUTC(0x11, pimg->fh);
#if IMG_API_VERSION == 0
	    put16((pimg->date1 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
#else /* IMG_API_VERSION == 1 */
	    put16(pimg->days1, pimg->fh);
#endif
	}
    } else {
#if IMG_API_VERSION == 0
	int diff = (pimg->date2 - pimg->date1) / SECS_PER_DAY;
	if (diff > 0 && diff <= 256) {
	    PUTC(0x12, pimg->fh);
	    put16((pimg->date1 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
	    PUTC(diff - 1, pimg->fh);
	} else {
	    PUTC(0x13, pimg->fh);
	    put16((pimg->date1 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
	    put16((pimg->date2 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
	}
#else /* IMG_API_VERSION == 1 */
	int diff = pimg->days2 - pimg->days1;
	if (diff > 0 && diff <= 256) {
	    PUTC(0x12, pimg->fh);
	    put16(pimg->days1, pimg->fh);
	    PUTC(diff - 1, pimg->fh);
	} else {
	    PUTC(0x13, pimg->fh);
	    put16(pimg->days1, pimg->fh);
	    put16(pimg->days2, pimg->fh);
	}
#endif
    }
#if IMG_API_VERSION == 0
    pimg->olddate1 = pimg->date1;
    pimg->olddate2 = pimg->date2;
#else /* IMG_API_VERSION == 1 */
    pimg->olddays1 = pimg->days1;
    pimg->olddays2 = pimg->days2;
#endif
}

static void
img_write_item_date(img *pimg)
{
    int same, unset;
    /* Only write dates when they've changed. */
#if IMG_API_VERSION == 0
    if (pimg->date1 == pimg->olddate1 && pimg->date2 == pimg->olddate2)
	return;

    same = (pimg->date1 == pimg->date2);
    unset = (pimg->date1 == 0);
#else /* IMG_API_VERSION == 1 */
    if (pimg->days1 == pimg->olddays1 && pimg->days2 == pimg->olddays2)
	return;

    same = (pimg->days1 == pimg->days2);
    unset = (pimg->days1 == -1);
#endif

    if (same) {
	if (img_output_version < 7) {
	    PUTC(0x20, pimg->fh);
#if IMG_API_VERSION == 0
	    put32(pimg->date1, pimg->fh);
#else /* IMG_API_VERSION == 1 */
	    put32((pimg->days1 - DAYS_1900) * SECS_PER_DAY, pimg->fh);
#endif
	} else {
	    if (unset) {
		PUTC(0x24, pimg->fh);
	    } else {
		PUTC(0x20, pimg->fh);
#if IMG_API_VERSION == 0
		put16((pimg->date1 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
#else /* IMG_API_VERSION == 1 */
		put16(pimg->days1, pimg->fh);
#endif
	    }
	}
    } else {
	if (img_output_version < 7) {
	    PUTC(0x21, pimg->fh);
#if IMG_API_VERSION == 0
	    put32(pimg->date1, pimg->fh);
	    put32(pimg->date2, pimg->fh);
#else /* IMG_API_VERSION == 1 */
	    put32((pimg->days1 - DAYS_1900) * SECS_PER_DAY, pimg->fh);
	    put32((pimg->days2 - DAYS_1900) * SECS_PER_DAY, pimg->fh);
#endif
	} else {
#if IMG_API_VERSION == 0
	    int diff = (pimg->date2 - pimg->date1) / SECS_PER_DAY;
	    if (diff > 0 && diff <= 256) {
		PUTC(0x21, pimg->fh);
		put16((pimg->date1 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
		PUTC(diff - 1, pimg->fh);
	    } else {
		PUTC(0x23, pimg->fh);
		put16((pimg->date1 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
		put16((pimg->date2 - TIME_T_1900) / SECS_PER_DAY, pimg->fh);
	    }
#else /* IMG_API_VERSION == 1 */
	    int diff = pimg->days2 - pimg->days1;
	    if (diff > 0 && diff <= 256) {
		PUTC(0x21, pimg->fh);
		put16(pimg->days1, pimg->fh);
		PUTC(diff - 1, pimg->fh);
	    } else {
		PUTC(0x23, pimg->fh);
		put16(pimg->days1, pimg->fh);
		put16(pimg->days2, pimg->fh);
	    }
#endif
	}
    }
#if IMG_API_VERSION == 0
    pimg->olddate1 = pimg->date1;
    pimg->olddate2 = pimg->date2;
#else /* IMG_API_VERSION == 1 */
    pimg->olddays1 = pimg->days1;
    pimg->olddays2 = pimg->days2;
#endif
}

static void
img_write_item_new(img *pimg, int code, int flags, const char *s,
		   double x, double y, double z);
static void
img_write_item_v3to7(img *pimg, int code, int flags, const char *s,
		     double x, double y, double z);
static void
img_write_item_ancient(img *pimg, int code, int flags, const char *s,
		       double x, double y, double z);

void
img_write_item(img *pimg, int code, int flags, const char *s,
	       double x, double y, double z)
{
   if (!pimg) return;
   if (pimg->version >= 8) {
      img_write_item_new(pimg, code, flags, s, x, y, z);
   } else if (pimg->version >= 3) {
      img_write_item_v3to7(pimg, code, flags, s, x, y, z);
   } else {
      img_write_item_ancient(pimg, code, flags, s, x, y, z);
   }
}

static void
img_write_item_new(img *pimg, int code, int flags, const char *s,
		   double x, double y, double z)
{
   switch (code) {
    case img_LABEL:
      write_v8label(pimg, 0x80 | flags, 0, -1, s);
      break;
    case img_XSECT: {
      INT32_T l, r, u, d, max_dim;
      img_write_item_date_new(pimg);
      l = (INT32_T)my_lround(pimg->l * 100.0);
      r = (INT32_T)my_lround(pimg->r * 100.0);
      u = (INT32_T)my_lround(pimg->u * 100.0);
      d = (INT32_T)my_lround(pimg->d * 100.0);
      if (l < 0) l = -1;
      if (r < 0) r = -1;
      if (u < 0) u = -1;
      if (d < 0) d = -1;
      max_dim = max(max(l, r), max(u, d));
      flags = (flags & img_XFLAG_END) ? 1 : 0;
      if (max_dim >= 32768) flags |= 2;
      write_v8label(pimg, 0x30 | flags, 0, -1, s);
      if (flags & 2) {
	 /* Big passage!  Need to use 4 bytes. */
	 put32(l, pimg->fh);
	 put32(r, pimg->fh);
	 put32(u, pimg->fh);
	 put32(d, pimg->fh);
      } else {
	 put16(l, pimg->fh);
	 put16(r, pimg->fh);
	 put16(u, pimg->fh);
	 put16(d, pimg->fh);
      }
      return;
    }
    case img_MOVE:
      PUTC(15, pimg->fh);
      break;
    case img_LINE:
      img_write_item_date_new(pimg);
      if (pimg->style != pimg->oldstyle) {
	  switch (pimg->style) {
	    case img_STYLE_NORMAL:
	    case img_STYLE_DIVING:
	    case img_STYLE_CARTESIAN:
	    case img_STYLE_CYLPOLAR:
	    case img_STYLE_NOSURVEY:
	       PUTC(pimg->style, pimg->fh);
	       break;
	  }
	  pimg->oldstyle = pimg->style;
      }
      write_v8label(pimg, 0x40 | flags, 0x20, 0x00, s ? s : "");
      break;
    default: /* ignore for now */
      return;
   }
   write_coord(pimg->fh, x, y, z);
}

static void
img_write_item_v3to7(img *pimg, int code, int flags, const char *s,
		     double x, double y, double z)
{
   switch (code) {
    case img_LABEL:
      write_v3label(pimg, 0x40 | flags, s);
      break;
    case img_XSECT: {
      INT32_T l, r, u, d, max_dim;
      /* Need at least version 5 for img_XSECT. */
      if (pimg->version < 5) return;
      img_write_item_date(pimg);
      l = (INT32_T)my_lround(pimg->l * 100.0);
      r = (INT32_T)my_lround(pimg->r * 100.0);
      u = (INT32_T)my_lround(pimg->u * 100.0);
      d = (INT32_T)my_lround(pimg->d * 100.0);
      if (l < 0) l = -1;
      if (r < 0) r = -1;
      if (u < 0) u = -1;
      if (d < 0) d = -1;
      max_dim = max(max(l, r), max(u, d));
      flags = (flags & img_XFLAG_END) ? 1 : 0;
      if (max_dim >= 32768) flags |= 2;
      write_v3label(pimg, 0x30 | flags, s);
      if (flags & 2) {
	 /* Big passage!  Need to use 4 bytes. */
	 put32(l, pimg->fh);
	 put32(r, pimg->fh);
	 put32(u, pimg->fh);
	 put32(d, pimg->fh);
      } else {
	 put16(l, pimg->fh);
	 put16(r, pimg->fh);
	 put16(u, pimg->fh);
	 put16(d, pimg->fh);
      }
      return;
    }
    case img_MOVE:
      PUTC(15, pimg->fh);
      break;
    case img_LINE:
      if (pimg->version >= 4) {
	  img_write_item_date(pimg);
      }
      write_v3label(pimg, 0x80 | flags, s ? s : "");
      break;
    default: /* ignore for now */
      return;
   }
   write_coord(pimg->fh, x, y, z);
}

static void
img_write_item_ancient(img *pimg, int code, int flags, const char *s,
		       double x, double y, double z)
{
   size_t len;
   INT32_T opt = 0;
   SVX_ASSERT(pimg->version > 0);
   switch (code) {
    case img_LABEL:
      if (pimg->version == 1) {
	 /* put a move before each label */
	 img_write_item_ancient(pimg, img_MOVE, 0, NULL, x, y, z);
	 put32(2, pimg->fh);
	 fputsnl(s, pimg->fh);
	 return;
      }
      len = strlen(s);
      if (len > 255 || strchr(s, '\n')) {
	 /* long label - not in early incarnations of v2 format, but few
	  * 3d files will need these, so better not to force incompatibility
	  * with a new version I think... */
	 PUTC(7, pimg->fh);
	 PUTC(flags, pimg->fh);
	 put32(len, pimg->fh);
	 fputs(s, pimg->fh);
      } else {
	 PUTC(0x40 | (flags & 0x3f), pimg->fh);
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
      if (opt) PUTC(opt, pimg->fh);
   }
   write_coord(pimg->fh, x, y, z);
}

/* Write error information for the current traverse
 * n_legs is the number of legs in the traverse
 * length is the traverse length (in m)
 * E is the ratio of the observed misclosure to the theoretical one
 * H is the ratio of the observed horizontal misclosure to the theoretical one
 * V is the ratio of the observed vertical misclosure to the theoretical one
 */
void
img_write_errors(img *pimg, int n_legs, double length,
		 double E, double H, double V)
{
    PUTC((pimg->version >= 8 ? 0x1f : 0x22), pimg->fh);
    put32(n_legs, pimg->fh);
    put32((INT32_T)my_lround(length * 100.0), pimg->fh);
    put32((INT32_T)my_lround(E * 100.0), pimg->fh);
    put32((INT32_T)my_lround(H * 100.0), pimg->fh);
    put32((INT32_T)my_lround(V * 100.0), pimg->fh);
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
	    osfree(pimg->cs);
	    osfree(pimg->datestamp);
	 } else {
	    /* write end of data marker */
	    switch (pimg->version) {
	     case 1:
	       put32((INT32_T)-1, pimg->fh);
	       break;
	     default:
	       if (pimg->version <= 7 ?
		   (pimg->label_len != 0) :
		   (pimg->style != img_STYLE_NORMAL)) {
		  PUTC(0, pimg->fh);
	       }
	       /* FALL THROUGH */
	     case 2:
	       PUTC(0, pimg->fh);
	       break;
	    }
	 }
	 if (ferror(pimg->fh)) result = 0;
	 if (pimg->close_func && pimg->close_func(pimg->fh))
	     result = 0;
	 if (!result) img_errno = pimg->fRead ? IMG_READERROR : IMG_WRITEERROR;
      }
      if (pimg->data) {
	  switch (pimg->version) {
	    case VERSION_COMPASS_PLT:
	      compass_plt_free_data(pimg);
	      break;
	    default:
	      osfree(pimg->data);
	  }
      }
      osfree(pimg->label_buf);
      osfree(pimg->filename_opened);
      osfree(pimg);
   }
   return result;
}

img_datum
img_parse_compass_datum_string(const char *s, size_t len)
{
#define EQ(S) len == LITLEN(S) && memcmp(s, S, LITLEN(S)) == 0
    /* First check the three which seem to be commonly used in Compass data. */
    if (EQ("WGS 1984"))
	return img_DATUM_WGS84;
    if (EQ("North American 1927"))
	return img_DATUM_NAD27;
    if (EQ("North American 1983"))
	return img_DATUM_NAD83;

    if (EQ("Adindan"))
	return img_DATUM_ADINDAN;
    if (EQ("Arc 1950"))
	return img_DATUM_ARC1950;
    if (EQ("Arc 1960"))
	return img_DATUM_ARC1960;
    if (EQ("Cape"))
	return img_DATUM_CAPE;
    if (EQ("European 1950"))
	return img_DATUM_EUROPEAN1950;
    if (EQ("Geodetic 1949"))
	return img_DATUM_NZGD49;
    if (EQ("Hu Tzu Shan"))
	return img_DATUM_HUTZUSHAN1950;
    if (EQ("Indian"))
	return img_DATUM_INDIAN1960;
    if (EQ("Tokyo"))
	return img_DATUM_TOKYO;
    if (EQ("WGS 1972"))
	return img_DATUM_WGS72;

    return img_DATUM_UNKNOWN;
}

char *
img_compass_utm_proj_str(img_datum datum, int utm_zone)
{
    int epsg_code = 0;
    const char* proj4_datum = NULL;

    if (utm_zone < -60 || utm_zone > 60 || utm_zone == 0)
	return NULL;

    switch (datum) {
      case img_DATUM_UNKNOWN:
	break;
      case img_DATUM_ADINDAN:
	if (utm_zone >= 35 && utm_zone <= 38)
	    epsg_code = 20100 + utm_zone;
	break;
      case img_DATUM_ARC1950:
	if (utm_zone >= -36 && utm_zone <= -34)
	    epsg_code = 20900 - utm_zone;
	break;
      case img_DATUM_ARC1960:
	if (utm_zone >= -37 && utm_zone <= -35)
	    epsg_code = 21000 - utm_zone;
	break;
      case img_DATUM_CAPE:
	if (utm_zone >= -36 && utm_zone <= -34)
	    epsg_code = 22200 - utm_zone;
	break;
      case img_DATUM_EUROPEAN1950:
	if (utm_zone >= 28 && utm_zone <= 38)
	    epsg_code = 23000 + utm_zone;
	break;
      case img_DATUM_NZGD49:
	if (utm_zone >= 58)
	    epsg_code = 27200 + utm_zone;
	break;
      case img_DATUM_HUTZUSHAN1950:
	if (utm_zone == 51)
	    epsg_code = 3829;
	break;
      case img_DATUM_INDIAN1960:
	if (utm_zone >= 48 && utm_zone <= 49)
	    epsg_code = 3100 + utm_zone;
	break;
      case img_DATUM_NAD27:
	if (utm_zone > 0 && utm_zone <= 23)
	    epsg_code = 26700 + utm_zone;
	else if (utm_zone >= 59)
	    epsg_code = 3311 + utm_zone;
	else
	    proj4_datum = "NAD27";
	break;
      case img_DATUM_NAD83:
	if (utm_zone > 0 && utm_zone <= 23)
	    epsg_code = 26900 + utm_zone;
	else if (utm_zone == 24)
	    epsg_code = 9712;
	else if (utm_zone >= 59)
	    epsg_code = 3313 + utm_zone;
	else
	    proj4_datum = "NAD83";
	break;
      case img_DATUM_TOKYO:
	if (utm_zone >= 51 && utm_zone <= 55)
	    epsg_code = 3041 + utm_zone;
	break;
      case img_DATUM_WGS72:
	if (utm_zone > 0)
	    epsg_code = 32200 + utm_zone;
	else
	    epsg_code = 32300 - utm_zone;
	break;
      case img_DATUM_WGS84:
	if (utm_zone > 0)
	    epsg_code = 32600 + utm_zone;
	else
	    epsg_code = 32700 - utm_zone;
	break;
    }

    if (epsg_code) {
	char *proj_str = xosmalloc(11);
	if (!proj_str) {
	    img_errno = IMG_OUTOFMEMORY;
	    return NULL;
	}
	SNPRINTF(proj_str, 11, "EPSG:%d", epsg_code);
	return proj_str;
    }

    if (proj4_datum) {
	char *proj_str;
	size_t len = strlen(proj4_datum) + 52 + 2 + 1;
	const char *south = "";
	if (utm_zone < 0) {
	    utm_zone = -utm_zone;
	    south = "+south ";
	    len += 7;
	}
	proj_str = xosmalloc(len);
	if (!proj_str) {
	    img_errno = IMG_OUTOFMEMORY;
	    return NULL;
	}
	SNPRINTF(proj_str, len,
		 "+proj=utm +zone=%d %s+datum=%s +units=m +no_defs +type=crs",
		 utm_zone, south, proj4_datum);
	return proj_str;
    }

    return NULL;
}
