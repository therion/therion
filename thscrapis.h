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
#define THSCRAPISRES 0.001

struct thscrapisloc {
  double x, y;
  thscrapisloc(double xx, double yy) : x(xx), y(yy) {}
};

bool operator < (const struct thscrapisloc & l1, 
    const struct thscrapisloc & l2);

struct thscrapis1ip {
  bool newone;
  double x, y, z;
  struct thscrapis1ip * next;
  struct thscrapis1ila * firsta, * lasta;
  thscrapis1ip() : newone(true), x(0.0), y(0.0), z(0.0),
    next(NULL), firsta(NULL), lasta(NULL) {}
};


struct thscrapis1ila {
  struct thscrapis1ip * sp, * tp;
  double dd, lnd, lnx, lny, rnd, rnx, rny;
  thscrapis1ila * next;
  thscrapis1ila() : lnd(-1.0), rnd(-1.0), next(NULL) {}
};

struct thscrapis1il {
  thscrapis1ila a1, a2;
  thscrapis1il * next;
  thscrapis1il() : a1(), a2(), next(NULL) {}
};



struct thscrapis2ip {
  bool local;
  double x, y, up, dn,
    orient, sx, sy;
};


// REMOVE

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

// ENDREMOVE



struct thscrapisolpt {
  double x, y, z, zd, zu;
  bool visible, deletable, outer;
  thscrapisolpt * next, * nnext, * prev, * pprev, * next_segment;
  thdb3dvx * vx3dup, * vx3ddn;
  thscrapisolpt() : x(0.0), y(0.0), z(0.0), zd(0.0), zu(0.0), 
    visible(true), deletable(true), outer(true), 
    next(NULL), nnext(NULL), prev(NULL), pprev(NULL),
    next_segment(NULL), vx3dup(NULL), vx3ddn(NULL) {}
};

/**
 * Scrap polygon line class.
 */
 
struct thscrapis {

  thscrapisolpt ** tri_opts;
  
  int (*tri_triangles) [3];
  
  int tri_num;

  bool track_bottom;  ///< Whether bottom is tracked.
  
  void insert_bp(double x, double y, double z);
  
  void end_bp();
  
  void insert_bp_direction(double x, double y, double z, double tx, double ty, double tz);
    
  void end_bp_direction();

  void insert_bp_shot(double fx, double fy, double fz, double tx, double ty, double tz);
  
  void bp_interpolate(double x, double y, double & iz, double & id, double & idx, double & idy);

  void insert_outline_lnsegment(bool newsegment, class thline * line, 
    bool outer, bool reverse, bool visible, long from, long to);
    
  void outline_scan(class thscraplo * outln);

  thscrapisbp * firstbp, * firstbbp;
  
  thscrapisolpt * firstolseg, * lastolseg, * lastolcseg, * maxol;
  
  thscrapis(); ///< Default constructor
  ~thscrapis(); ///< Default destructor
  
  std::map <thscrapisloc, thscrapisbp> bp_map;

  std::list <thscrapisolpt> ol_list;  
  
  thscrapisdim * firstdim;
  
  std::list <thscrapisdim> dim_list;
  
  void insert_dim(double x, double y, double up, double down);
  
  void insert_bp_dim();

  void outline_interpolate_dims();

  void dim_interpolate(double x, double y, double z, double & zu, double & zd);
  
  thscrapis1ip * first1ip, * last1ip;
  std::map <thscrapisloc, thscrapis1ip> i1pt_map;
  std::list <thscrapis1il> i1_legs;
  thscrapis1il * first1il, * last1il;
  std::list <thscrapis2ip> i2_points;
  thscrapis2ip * first2ip, * last2ip;
  
  thscrapis1ip * i1insertpt (double x, double y, double z);
  void i1insert(double x1, double y1, double z1, double x2, double y2, double z2);
  void i1interp();
  void i2insert(thscrapis2ip ipt);
  void i2interp();
  
};

#endif

