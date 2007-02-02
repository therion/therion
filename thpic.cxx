/**
 * @file thpic.cxx
 * Picture manipulation structure.
 */
  
/* Copyright (C) 2006 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */

#include "thpic.h"
#include "thbuffer.h"
#include "thdatabase.h"
#include "thinit.h"
#include "thtmpdir.h"
#include "thexception.h"
#include <stdarg.h>
#ifdef THMSVC
#include <direct.h>
#define getcwd _getcwd
#endif

long thpic__convert_number(1);

char * thpic__tmp(NULL);

thpic::thpic() {
  this->fname = NULL;
  this->texfname = NULL;
  this->rgba = NULL;
  this->rgbafn = NULL;
  this->width = -1;
  this->height = -1;

  this->x = 0.0;
  this->y = 0.0;
  this->scale = 1.0;

  if (thpic__tmp == NULL) {
    thdb.buff_tmp = thtmp.get_file_name("pic0000.txt");
    thpic__tmp = thdb.strstore(thdb.buff_tmp);
  }
}


bool thpic::exists() {
  return ((this->width > 0) && (this->height > 0));
}


void thpic::init(const char * pfname, const char * incfnm)
{

  long i;
  thbuffer pict_path;
  pict_path.guarantee(1024);
  getcwd(pict_path.get_buffer(),1024);  

  if (strlen(pfname) == 0)
    ththrow(("picture file name not specified"));

  pict_path += "/";
  if (incfnm != NULL)
    pict_path += incfnm;
  char * pp = pict_path.get_buffer();
  for(i = (long)strlen(pp); i >= 0; i--) {
    if ((pp[i] == '/') || (pp[i] == '\\')) {
      break;
    } else
      pp[i] = 0;
  }
  if (strlen(pp) == 0)
    pict_path = "/";
  pict_path += pfname;
  pp = pict_path.get_buffer();
  for(i = (long)strlen(pp); i >= 0; i--) {
    if (pp[i] == '\\')
      pp[i] = '/';
  }

  this->fname = thdb.strstore(pp);
  // thprintf("\npict name: %s\n", this->fname);  

  thbuffer ccom;
  int retcode;
  bool isspc;

  // program path
  isspc = (strcspn(thini.get_path_identify()," \t") < strlen(thini.get_path_identify()));
  ccom = "";
  if (isspc) ccom += "\"";
  ccom += thini.get_path_identify();
  if (isspc) ccom += "\"";

  // format
  ccom += " -format \"%w\\n%h\\n\" ";

  // filename
  isspc = (strcspn(this->fname," \t") < strlen(this->fname));
  if (isspc) ccom += "\"";
  ccom += this->fname;
  if (isspc) ccom += "\"";

  // write into
  ccom += " > ";
  isspc = (strcspn(thpic__tmp," \t") < strlen(thpic__tmp));
  if (isspc) ccom += "\"";
  ccom += thpic__tmp;
  if (isspc) ccom += "\"";
  
#ifdef THDEBUG
  thprintf("running convert\n");
#endif

  retcode = system(ccom.get_buffer());
  if (retcode == EXIT_SUCCESS) {
    FILE * tmp;
    tmp = fopen(thpic__tmp,"r");
    if (tmp == NULL)
      retcode = EXIT_FAILURE;
    else {
      if (fscanf(tmp,"%ld",&this->width) != 1)
        retcode = EXIT_FAILURE;
      if (fscanf(tmp,"%ld",&this->height) != 1)
        retcode = EXIT_FAILURE;
      fclose(tmp);
    }
  }

  if ((retcode != EXIT_SUCCESS) || (!this->exists())) {
    thwarning(("unable to read \"%s\"", this->fname))
    this->height = -1;
    this->width = -1;
  }

}


char * thpic::convert(const char * type, const char * ext, const char * optfmt, ...)
{
  if (!this->exists())
    return NULL;

  thbuffer ccom;
  int retcode;
  bool isspc;
  char tmpfn[255];
  const char * tmpf;
  char options[1024];
  va_list args;
  va_start(args, optfmt);
  vsprintf(options, optfmt, args);
  va_end(args);
  sprintf(tmpfn, "pic%04ld.%s", thpic__convert_number++, ext);
  isspc = (strcspn(thini.get_path_convert()," \t") < strlen(thini.get_path_convert()));
  ccom = "";
  if (isspc) ccom += "\"";
  ccom += thini.get_path_convert();
  if (isspc) ccom += "\"";
  ccom += " ";
  ccom += options;
  ccom += " ";

  isspc = (strcspn(this->fname," \t") < strlen(this->fname));
  if (isspc) ccom += "\"";
  ccom += this->fname;
  if (isspc) ccom += "\"";
  ccom += " ";

  tmpf = thtmp.get_file_name(tmpfn);
  isspc = (strcspn(tmpf," \t") < strlen(tmpf));
  if (isspc) ccom += "\"";
  ccom += type;
  ccom += ":";
  ccom += tmpf;
  if (isspc) ccom += "\"";

  retcode = system(ccom.get_buffer());
  if (retcode == EXIT_SUCCESS) {
    ccom = thtmp.get_file_name(tmpfn);
    size_t x, l;
    l = strlen(ccom);
    for (x = 0; x < l; x++) if (ccom.get_buffer()[x] == '\\') ccom.get_buffer()[x] = '/';
    return (thdb.strstore(ccom));
  } else {
    return NULL;
  }

}


void thpic::rgba_load()
{

  if (!this->exists())
    return;

  if (this->rgbafn == NULL) {
    this->rgbafn = this->convert("RGBA","rgba","-depth 8");
  }

  if (this->rgbafn == NULL)
    return;

  FILE * f;
  f = fopen(this->rgbafn,"rb");
  if (f != NULL) {
    size_t rawsize, readsize;
    rawsize = (size_t)(this->width * this->height * 4);
    this->rgba = new char [rawsize];
    readsize = fread(this->rgba, 1, rawsize, f);
    if (readsize < rawsize) {
      this->rgba_free();
    }
    fclose(f);
  }

}


void thpic::rgba_free()
{
  if (this->rgba != NULL) {
    delete [] this->rgba;
    this->rgba = NULL;
  }
}



void thpic::rgba_init(long w, long h)
{
  this->width = w;
  this->height = h;
  long i, s;
  s = 4 * w * h;
  this->rgba = new char [s];
  for(i = 0; i < s; i++) this->rgba[i] = 0;
}

void thpic::rgba_save(const char * type, const char * ext)
{
  if (this->rgba == NULL) {
    this->width = -1;
    this->height = -1;
    this->fname = NULL;
    return;
  }
  thpic tmp;
  tmp.width = this->width;
  tmp.height = this->height;
  char tmpfn[255];
  sprintf(tmpfn, "pic%04ld.rgba", thpic__convert_number++);
  tmp.fname = thdb.strstore(thtmp.get_file_name(tmpfn));
  this->rgbafn = tmp.fname;
  FILE * f;
  f = fopen(tmp.fname,"wb");
  fwrite(this->rgba,1,4 * this->width * this->height,f);
  fclose(f);
  this->fname = tmp.convert(type, ext, "-depth 8 -size %dx%d -density 300", this->width, this->height);
  sprintf(tmpfn, "pic%04ld.%s", thpic__convert_number - 1, ext);
  this->texfname = thdb.strstore(tmpfn);
}


void thpic::rgba_set_pixel(long x, long y, char * data)
{
  char * dst;
  if ((this->rgba == NULL) || (x < 0) || (x >= this->width) || (y < 0) || (y >= this->height))
    return;
  dst = &(this->rgba[4 * ((this->height - y - 1) * this->width + x)]);
  dst[0] = data[0];
  dst[1] = data[1];
  dst[2] = data[2];
  dst[3] = data[3];
}

char * thpic::rgba_get_pixel(long x, long y)
{
  static unsigned char data[4], * src;
  if (this->rgba == NULL)
    return NULL;
  if ((x < 0) || (x >= this->width) || (y < 0) || (y >= this->height)) {
    data[0] = 255;
    data[1] = 255;
    data[2] = 255;
    data[3] = 0;
  } else {
    src = (unsigned char *) &(this->rgba[4 * ((this->height - y - 1) * this->width + x)]);
    data[0] = src[0];
    data[1] = src[1];
    data[2] = src[2];
    data[3] = src[3];
  }
  return (char *) data;
}


double R(double x) {
  double P2, P1, P0, P_1;
#define P(c) (x > -c ? x + c : 0.0);
#define P_(c) (x > c ? x - c : 0.0);
  P2 = P(2.0); P1 = P(1.0); P0 = P(0.0); P_1 = P_(1.0);
  return (0.16666666666666666666666666666667 * 
    (P2*P2*P2 - 4.0 * P1*P1*P1 + 6.0 * P0*P0*P0 - 4 * P_1*P_1*P_1));
}

char * thpic::rgba_interpolate_pixel(double x, double y)
{
  // nearest
#define PIC_INT_BSPLINE

#ifdef PIC_INT_NEAREST
  return this->rgba_get_pixel(long(x + 0.5), long(y + 0.5));
#endif

#ifdef PIC_INT_BILINEAR
  static unsigned char data[4];
  unsigned char * src00, * src10, * src01, * src11;
  int m, i, j;
  double dx, dy, w00, w10, w01, w11, d;
  i = int(x);
  j = int(y);
  if ((i < -2) || (i > this->width + 2) || (j < -2) || (j > this->height + 2)) {
    data[0] = 255; data[1] = 255; data[2] = 255; data[3] = 0;
    return (char *) data;
  }
  dx = x - double(i);
  dy = y - double(j);
  w00 = (1 - dx) * (1 - dy);
  w10 = dx * (1 - dy);
  w01 = (1 - dx) * dy;
  w11 = dx * dy;
  src00 = (unsigned char *) this->rgba_get_pixel(i,j);
  src10 = (unsigned char *) this->rgba_get_pixel(i+1,j);
  src01 = (unsigned char *) this->rgba_get_pixel(i,j+1);
  src11 = (unsigned char *) this->rgba_get_pixel(i+1,j+1);
  for (m = 0; m < 4; m++) {
    d = w00 * double(src00[m]) + w10 * double(src10[m]) + w01 * double(src01[m]) + w11 * double(src11[m]);
    i = int(d + 0.5); if (i < 0) i = 0; if (i > 255) i = 255; data[m] = (unsigned char) i;
  }
  return (char *) data;
#endif

#ifdef PIC_INT_BSPLINE
  static unsigned char data[4];
  unsigned char * src;
  int m, n, i, j;
  double dx, dy, w, ddata[4];
  i = int(x);
  j = int(y);
  if ((i < -2) || (i > this->width + 2) || (j < -2) || (j > this->height + 2)) {
    data[0] = 255; data[1] = 255; data[2] = 255; data[3] = 0;
    return (char *) data;
  }
  dx = x - double(i);
  dy = y - double(j);
  ddata[0] = 0.0; ddata[1] = 0.0;
  ddata[2] = 0.0; ddata[3] = 0.0;
  for (m = -1; m < 3; m++) {
    for (n = -1; n < 3; n++) {
      src = (unsigned char *) this->rgba_get_pixel(i + m, j + n);
      w = R(double(m) - dx) * R(dy - double(n));
      ddata[0] += w * double(src[0]);
      ddata[1] += w * double(src[1]);
      ddata[2] += w * double(src[2]);
      ddata[3] += w * double(src[3]);
    }
  }
  i = int(ddata[0] + 0.5); if (i < 0) i = 0; if (i > 255) i = 255; data[0] = (unsigned char) i;
  i = int(ddata[1] + 0.5); if (i < 0) i = 0; if (i > 255) i = 255; data[1] = (unsigned char) i;
  i = int(ddata[2] + 0.5); if (i < 0) i = 0; if (i > 255) i = 255; data[2] = (unsigned char) i;
  i = int(ddata[3] + 0.5); if (i < 0) i = 0; if (i > 255) i = 255; data[3] = (unsigned char) i;
  return (char *) data;
#endif

}

