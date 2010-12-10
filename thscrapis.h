/** 
 * @file thscrapis.h
 * Scrap interpolation structure.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
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
 
#ifndef thscrapis_h
#define thscrapis_h

#include <map>
#include <list>
#include "thdb3d.h"
#include "extern/lxMath.h"
#define THSCRAPISRES 0.001
//#define THSCRAPIS_NEW3D 1

#ifndef THSCRAPIS_NEW3D
struct thscrapisloc {
  double x, y;
  thscrapisloc(double xx, double yy) : x(xx), y(yy) {}
};

bool operator < (const struct thscrapisloc & l1, 
    const struct thscrapisloc & l2);

struct thscrapisbp {
  double x, y, z, ///< 3D coordinates
    dd, dx, dy,  ///< direction unit vector
    sumd,  ///< sum of directions
    sums,  ///< sum of direction weights.
    suml,
    sumsl,
    up,
    down;
  thscrapisbp * next;
  thscrapisbp() : x(0.0), y(0.0), z(0.0), dd(false), dx(0.0), dy(0.0), 
    sumd(0.0), sums(-1.0), 
    suml(0.0), sumsl(0.0), up(-1.6), down(-1.0), 
    next(NULL) {};
};

struct thscrapisdim {
  double x, y, z,
    up, down, zup, zdown, dd, dx, dy;
  thscrapisdim * next;
  thscrapisdim() : x(0.0), y(0.0), z(0.0), up(0.0), down(0.0),
    zup(0.0), zdown(0.0), dd(0.0), dx(0.0), dy(0.0), next(NULL) {}
};

#endif


struct thscrapisolpt {
  double x, y, w;
#ifndef THSCRAPIS_NEW3D
  double z, zd, zu;
#else
  lxVec pt, dir;
  double d_up, d_dn;
#endif
  bool visible, deletable, outer;
  thscrapisolpt * next, * prev, * next_segment;
  thdb3dvx * vx3dup, * vx3ddn;
  thscrapisolpt() : x(0.0), y(0.0), w(0.0),
#ifndef THSCRAPIS_NEW3D
    z(0.0), zd(0.0), zu(0.0), 
#else
    d_up(0.0), d_dn(0.0),
#endif
    visible(true), deletable(true), outer(true), 
    next(NULL), prev(NULL),
    next_segment(NULL), vx3dup(NULL), vx3ddn(NULL) {}
};

typedef thscrapisolpt * pthscrapisolpt;

/**
 * Scrap polygon outline class.
 */
 
struct thscrapis {

#ifndef THSCRAPIS_NEW3D

  thscrapisbp * firstbp, * firstbbp;

  std::map <thscrapisloc, thscrapisbp> bp_map;
  
  thscrapisdim * firstdim;
  
  std::list <thscrapisdim> dim_list;

  void insert_bp(double x, double y, double z);
  
  void end_bp();
  
  void insert_bp_direction(double x, double y, double z, double tx, double ty, double tz, class thscraplp * slp);
    
  void end_bp_direction();

  void insert_bp_shot(double fx, double fy, double fz, double tx, double ty, double tz);
  
  void bp_interpolate(double x, double y, double & iz, double & id, double & idx, double & idy);
  
  void insert_dim(double x, double y, double up, double down);
  
  void insert_bp_dim();

  void outline_interpolate_dims();

  void dim_interpolate(double x, double y, double z, double & zu, double & zd);

#endif

  pthscrapisolpt (*tri_triangles) [3];
  
  int tri_num;

  class thscrap * m_scrap;

  thscrapisolpt * firstolseg, * lastolseg, * lastolcseg;
  
  std::list <thscrapisolpt> ol_list;  
  
  thscrapis(class thscrap * scrap); ///< Default constructor
  ~thscrapis(); ///< Default destructor

#ifdef THSCRAPIS_NEW3D

  void int3d();

#endif

  void insert_outline_lnsegment(bool newsegment, class thline * line, 
    bool outer, bool reverse, bool visible, long from, long to);

  void outline_scan(class thscraplo * outln);
  
};

#endif

