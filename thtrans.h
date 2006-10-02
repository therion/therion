/**
 * @file thtrans.h
 * Transformation structures.
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
 
#ifndef thtrans_h
#define thtrans_h

#include <list>

struct thvec2 {
  double m_x, m_y;
  thvec2() : m_x(0.0), m_y(0.0) {}
  thvec2(double x, double y) : m_x(x), m_y(y) {}
  thvec2 operator += (const thvec2 & v);
  thvec2 operator -= (const thvec2 & v);
  thvec2 operator *= (const double & c);
  thvec2 operator /= (const double & c);
  void minimize(thvec2 v);
  void maximize(thvec2 v);
  double length();
  void reset();
  void normalize();
};

struct thmat2 {
  double m_xx, m_xy, m_yx, m_yy;
  thmat2() : m_xx(1.0), m_xy(0.0), m_yx(0.0), m_yy(1.0) {}
  thmat2(double xx, double xy, double yx, double yy) : m_xx(xx), m_xy(xy), m_yx(yx), m_yy(yy) {}
  thmat2 operator *= (const double & c);
  thmat2 operator /= (const double & c);
  thmat2 inverse();
  void reset();
};

struct thline2 {
  double m_a, m_b, m_c;
  thline2() : m_a(0.0), m_b(0.0), m_c(0.0) {}
  thline2(double a, double b, double c) : m_a(a), m_b(b), m_c(c) {}
  thline2(thvec2 from, thvec2 to);
  double eval(thvec2 p);
};

thvec2 operator + (const thvec2 & v1, const thvec2 & v2);

thvec2 operator - (const thvec2 & v1, const thvec2 & v2);

double operator * (const thvec2 & v1, const thvec2 & v2);

thvec2 operator * (const double & c, const thvec2 & v);

thvec2 operator * (const thmat2 & m, const thvec2 & v);

thmat2 operator * (const double & c, const thmat2 & m);

thmat2 operator / (const thmat2 & m, const double & c);

struct thlintrans_pt {
  thvec2 m_src, m_tgt;
  thlintrans_pt(thvec2 src, thvec2 tgt) : m_src(src), m_tgt(tgt) {}
};

typedef std::list<thlintrans_pt> thlintrans_pt_list;

/**
 * Linear transformation. Rotate, zoom, shift.
 */

struct thlintrans {
  
  thmat2 m_fmat, m_bmat;
  thvec2 m_shift;
  double m_rot, m_scale;

  thlintrans_pt_list m_initpts;

  thlintrans();
  void reset();
  void init(thmat2 A, thvec2 b);
  void insert_point(thvec2 src, thvec2 dst);
  void init_points();
  void init_backward();
  thvec2 forward(thvec2 src);
  thvec2 backward(thvec2 dst);

};


/**
 * Morphing transformation.
 */

struct thmorphtrans {
  
  thlintrans_pt_list m_initpts;

  void reset();
  void insert_point(thvec2 src, thvec2 dst);
  thvec2 forward(thvec2 src);
  thvec2 backward(thvec2 dst, thvec2 ini = thvec2(0,0));

};


/**
 * Advanced morphing transformation.
 */

struct thmorph2trans {
  
  struct thmorph2trans_members * m;
  double m_eps;

  thmorph2trans();
  ~thmorph2trans();
  void reset();
  void insert_point(thvec2 src, thvec2 dst, long id);
  void insert_line(long from, long to);
  void init(double eps = 0.01);
  thvec2 forward(thvec2 src);
  thvec2 backward(thvec2 dst, thvec2 ini = thvec2(0,0));

};


#endif


