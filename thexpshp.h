/**
 * @file thexpshp.h
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thexpshp_h
#define thexpshp_h

#include <list>
#include "thscrap.h"
#include "thpoint.h"
#include "thline.h"
#include "tharea.h"
#include "thattr.h"
#include "thdb2d.h"


struct thexpshpf_data {
  double m_x, m_y, m_z, m_m;
  thexpshpf_data(double x, double y, double z, double m) : m_x(x), m_y(y), m_z(z), m_m(m) {}
};

struct thexpshpf_part {
  int m_type = {}, m_start = {};
};


// Shapefile module
struct thexpshpf {

  const char * m_fnm, * m_fpath = nullptr;
  struct thexpshp * m_xshp;
  bool m_is_open = false;
  int m_type;
  size_t m_num_objects = 0;
  SHPHandle m_hndl = nullptr;

  thattr m_attributes;

  thexpshpf(struct thexpshp * xshp, const char * fnm, int type);
  bool open();
  void close();

  std::list<thexpshpf_data> m_point_list;
  std::list<thexpshpf_part> m_part_list;
  int m_object_id = -1;
  void object_clear();
  void object_insert();

  bool m_ring_outer = false;
  std::list<thexpshpf_data> m_ring_list;

  void point_insert(double x, double y, double z, double m = 0.0);

  void polygon_start_ring(bool outer);
  void polygon_insert_line(thline * ln, bool reverse);
  void polygon_close_ring();

  void tristrip_start();

};


// Shapefile export structure.
struct thexpshp {

  const char * m_dirname = nullptr;
  thdb2dprj * m_xproj = nullptr;
  thexpshpf m_fscrap, m_fpoints, m_flines, m_fareas;
  thexpshpf m_fstations3D, m_fshots3D, m_fwalls3D;
  class thexpmap * m_expmap = nullptr;
  class thexpmodel * m_expmodel = nullptr;

  thexpshp();
  bool open(const char * dirname);
  void xscrap2d(thscrap * scrap, thdb2dxm * xmap, thdb2dxs * xbasic);
  void close();

};

#endif




