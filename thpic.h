/**
 * @file thpic.h
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
 
#ifndef thpic_h
#define thpic_h

struct thpic {

  const char * fname, * texfname, * rgbafn;
  
  char * rgba;

  long width, height;

  double x, y, ///< Coordinates of upper left corner
    scale;  ///< Image scale.

  thpic();

  bool exists();

  void init(const char * fname, const char * incfnm);

  const char * convert(const char * type, const char * ext, const char * optfmt, ...);

  void rgba_load();

  void rgba_free();

  void rgba_init(long width, long height);

  void rgba_save(const char * type, const char * ext);

  char * rgba_get_pixel(long x, long y);

  char * rgba_interpolate_pixel(double x, double y);

  void rgba_set_pixel(long x, long y, char * data);
  
};

#endif

