/**
 * @file thexpuni.h
 * Shapefile export classes.
 */
  
/* Copyright (C) 2007 Stacho Mudrak
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
 
#ifndef thexpuni_h
#define thexpuni_h

#include <list>
#include "thscrap.h"
#include "thpoint.h"
#include "thline.h"
#include "tharea.h"
#include "thattr.h"
#include "thdb2d.h"


struct thexpuni_data {
  double m_x, m_y, m_z, m_a;
  thexpuni_data(double x, double y, double z, double a) : m_x(x), m_y(y), m_z(z), m_a(a) {}
};

struct thexpuni_part {
  bool m_outer;
  thdb2dlp * m_lp;
  thscraplo * m_lo;
  std::list<thexpuni_data> m_point_list;
  thexpuni_part() : m_outer(true), m_lp(NULL), m_lo(NULL) {}
};


// Universal module
struct thexpuni {

  std::list<thexpuni_part> m_part_list;

  thexpuni();
  void clear();

  thexpuni_part * m_cpart;
  std::list<thexpuni_data> m_ring_list;
  void polygon_start_ring(bool outer);
  void polygon_insert_line(thline * ln, bool reverse);
  void polygon_close_ring();

  void parse_line(thline * line);
  void parse_scrap(thscrap * scrap);

};

#endif




