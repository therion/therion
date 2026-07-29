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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */

#include "thpic.h"
#include "thdatabase.h"
#include "thtmpdir.h"
#include "thexception.h"
#include "thconfig.h"
#include "therion.h"

#include <Magick++/Image.h>

#include <fmt/format.h>

#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

long thpic_convert_number(1);

thpic::thpic() {
  this->fname = NULL;
  this->texfname = NULL;
  this->rgbafn = NULL;
  this->width = -1;
  this->height = -1;

  this->x = 0.0;
  this->y = 0.0;
  this->scale = 1.0;
}


bool thpic::exists() {
  return ((this->width > 0) && (this->height > 0));
}


void thpic::init(const char * pfname, const char * incfnm)
{
  if (strlen(pfname) == 0)
    throw thexception("picture file name not specified");

  std::error_code ec;
  auto pict_path = fs::current_path(ec);
  thassert(!ec);

  if (incfnm != NULL) {
    if (fs::path(incfnm).is_absolute())
    	pict_path = incfnm;
    else 
    	pict_path /= incfnm;
  }
  pict_path = pict_path.parent_path() / pfname;

  auto pict_path_str = pict_path.string();
  std::replace(pict_path_str.begin(), pict_path_str.end(), '\\', '/');
  this->fname = thdb.strstore(pict_path_str.c_str());

  try
  {
    const Magick::Image img(this->fname);
    this->width = img.columns();
    this->height = img.rows();
  }
  catch (const Magick::Exception& e)
  {
    thwarning(fmt::format("unable to read {} -- {}", this->fname, e.what()));
    this->height = -1;
    this->width = -1;
  }

}


void thpic::rgba_load()
{
  if (!this->exists())
    return;

  try
  {
    Magick::Image img(this->fname);
    this->rgba.resize(img.columns() * img.rows() * 4);
    img.write(0, 0, img.columns(), img.rows(), "RGBA", MagickCore::CharPixel, this->rgba.data());
  }
  catch (const Magick::Exception& e)
  {
    thwarning(fmt::format("unable to load {} -- {}", this->fname, e.what()));
    this->rgba_free();
  }
}


void thpic::rgba_free()
{
  this->rgba.clear();
}



void thpic::rgba_init(long w, long h)
{
  this->width = w;
  this->height = h;
  this->rgba.resize(4 * w * h);
  std::fill(this->rgba.begin(), this->rgba.end(), 0);
}

void thpic::rgba_save(const int colors)
{
  if (this->rgba.empty()) {
    this->width = -1;
    this->height = -1;
    this->fname = NULL;
    return;
  }

  const auto fileName = fmt::format("pic{:04d}.png", thpic_convert_number++);
  const std::string tmpFile = thtmp.get_file_name(fileName.c_str());
  try
  {
    Magick::Image image(this->width, this->height, "RGBA", Magick::CharPixel, this->rgba.data());
    image.defineValue("PNG", "exclude-chunks", "date,time");
    image.depth(8);
    image.density("300");
    if (colors > 1 && !thcfg.reproducible_output) {
      image.quantizeColors(colors);
      image.quantizeDither(true);
    }
    image.write(tmpFile);
    this->texfname = thdb.strstore(fileName.c_str());
  }
  catch (const Magick::Exception& e)
  {
    thwarning(fmt::format("unable to create {} -- {}", tmpFile, e.what()));
  }
}


void thpic::rgba_set_pixel(long x, long y, char * data)
{
  char * dst;
  if ((this->rgba.empty()) || (x < 0) || (x >= this->width) || (y < 0) || (y >= this->height))
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
  if (this->rgba.empty())
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

