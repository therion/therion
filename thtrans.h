/**
 * @file thtrans.h
 * Transformation structures.
 */
  
/* Copyright (C) 2006-2007 Stacho Mudrak, Marco Corvi
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
#ifndef thtrans_2_h
#define thtrans_2_h

#include <math.h> // sqrt

#include "thinfnan.h" // NaN


#include <string>
#include <vector>
#include <list>
#include <memory>

struct thline2; // forward declaration

/** 2D vector
 *
 */
struct thvec2 {
  double m_x, m_y;                                     //!< coordinates

  thvec2() : m_x(0.0), m_y(0.0) {}                     //!< default cstr
  thvec2(double x, double y) : m_x(x), m_y(y) {}       //!< cstr
  thvec2( thline2 & l1, thline2 & l2 );                //!< intersection of two lines

  thvec2 operator += (const thvec2 & v);               //!< vector addition   
  thvec2 operator -= (const thvec2 & v);               //!< vector difference
  thvec2 operator *= (const double & c);               //!< multiplication by a scalar
  thvec2 operator /= (const double & c);               //!< division by a scalar
  void minimize(thvec2 v);                             //!< min( this, v )
  void maximize(thvec2 v);                             //!< max( this, v )
  double length();                                     //!< |V| = sqrt( V * V )
  double length2() const { return m_x*m_x + m_y*m_y; } //!< V * V
  double orientation();                                //!< orientation in degrees
  void reset();                                        //!< V = 0
  void normalize();                                    //!< V / |V|
  thvec2 orthogonal() { return thvec2( m_y, -m_x); }   //!< V^ (anticlockwise orthogonal vector)

  bool is_nan() const { return thisnan(m_x) || thisnan(m_y); }
};

/** 2 x 2 matrix with real coefficient
 * <PRE>
 *       m_xx  m_xy
 *       m_yx  m_yy 
 * </PRE>
 */
struct thmat2 {
  double m_xx, m_xy, m_yx, m_yy;                       //!< matrix coefficients

  thmat2() : m_xx(1.0), m_xy(0.0), m_yx(0.0), m_yy(1.0) {}  //!< cstr
  thmat2(double xx, double xy, double yx, double yy) : m_xx(xx), m_xy(xy), m_yx(yx), m_yy(yy) {}

  thmat2 operator *= (const double & c);                 //!< multiplication by a scalar
  thmat2 operator /= (const double & c);                 //!< division by a scalar
  thmat2 inverse();                                      //!< inverse matrix
  double determinant() { return m_xx*m_yy - m_xy*m_yx; } //!< determinant
  double trace() { return m_xx + m_yy; }                 //!< trace
  void reset();                                          //!< M = Identity matrix
};

/** line in the 2D plane, with equation
 * <PRE>
 *     m_a * X + m_b * Y + m_c = 0
 * </PRE> 
 */
struct thline2 {
  double m_a, m_b, m_c;                                //!< coefficients of the line
  thline2() : m_a(0.0), m_b(0.0), m_c(0.0) {}          //!< cstr
  thline2(double a, double b, double c) : m_a(a), m_b(b), m_c(c) {}

  thline2(thvec2 from, thvec2 to);                     //!< cstr: line joining two vectors
  double eval(thvec2 p);                               //!< evaluate the line eq. on the vector
};

/** linear algebra operations
 */
thvec2 operator - (const thvec2 & v);                      // -V

thvec2 operator + (const thvec2 & v1, const thvec2 & v2);  // V1 + V2

thvec2 operator - (const thvec2 & v1, const thvec2 & v2);  // V1 - V2

double operator * (const thvec2 & v1, const thvec2 & v2);  // V1 * V2

double operator ^ (const thvec2 & v1, const thvec2 & v2);  // V1 ^ V2 = V1 * (V2^)

thvec2 operator * (const double & c, const thvec2 & v);    // c * V1

thvec2 operator * (const thvec2 & v, const double & c);    // V1 * c

thvec2 operator / (const thvec2 & v, const double & c);    // V1 / c

thvec2 operator * (const thmat2 & m, const thvec2 & v);    // M * V1

thmat2 operator * (const double & c, const thmat2 & m);    // c * M

thmat2 operator * (const thmat2 & m, const double & c);    // M * c

thmat2 operator / (const thmat2 & m, const double & c);    // M / c


/**
 * 2D bouding box.
 */

struct thbbox2 {
	bool m_valid;
	thvec2 m_min, m_max;
	thbbox2() : m_valid(false) {}
	bool is_valid();
	void update(thvec2 pt);
};


/** 
 * 3x3 matrix
 */
struct thmat3
{
  double m_xx, m_xy, m_xz;  //!< first row
  double m_yx, m_yy, m_yz;  //!< second row
  double m_zx, m_zy, m_zz;  //!< second row

  /** default cstr: set the matrix to the identity
   */
  thmat3() : m_xx(1.0), m_xy(0.0), m_xz(0.0)
           , m_yx(0.0), m_yy(1.0), m_yz(0.0) 
           , m_zx(0.0), m_zy(0.0), m_zz(1.0) { }

  /** cstr with the nine matrix elements, row-wise
   */
  thmat3(double xx, double xy, double xz,
         double yx, double yy, double yz,
         double zx, double zy, double zz)
    : m_xx(xx), m_xy(xy), m_xz(xz)
    , m_yx(yx), m_yy(yy), m_yz(yz)
    , m_zx(zx), m_zy(zy), m_zz(zz) { }

  /** set the matrix to the identity
   */
  void reset();

  /** compute the inverse matrix
   * @return the inverse matrix
   */
  thmat3 inverse();
};

/** linear transformation point
 */
struct thlintrans_pt {
  thvec2 m_src;        //!< point in the source domain
  thvec2 m_tgt;        //!< point in the target domain

  thlintrans_pt() {}

  /** cstr
   */
  thlintrans_pt(thvec2 src, thvec2 tgt) 
    : m_src(src)
    , m_tgt(tgt) 
  { }
};

/** list of linear-transform points
 */
typedef std::list<thlintrans_pt> thlintrans_pt_list;

/**
 * Linear transformation. Rotate, zoom, shift.
 */
struct thlintrans {
  
  thmat2 m_fmat;           //!< forward matrix
  thmat2 m_bmat;           //!< backward matrix
  thvec2 m_shift;
  double m_rot = {}, m_scale = {};

  thlintrans_pt_list m_initpts;

  thlintrans();
  void reset();
  void init(thmat2 A, thvec2 b);
  void init(thvec2 src, thvec2 dst);
  void init(thvec2 srcX, thvec2 srcY, thvec2 dstX, thvec2 dstY);
  void insert_point(thvec2 src, thvec2 dst);
  void init_points();
  void init_backward();
  thvec2 forward(thvec2 src);
  thvec2 backward(thvec2 dst);

};


/**
 * Linear zooming transformation.
 */

struct thlinzoomtrans {
  
  bool m_valid, m_single;
  thvec2 m_from, m_to;
  double m_fl, m_fr, m_tl, m_tr;
  int m_flc, m_frc, m_tlc, m_trc;

  thline2 m_line_from, m_line_to, m_line;
  double m_orient_from = 0.0, m_orient_to = 0.0, m_line_l = 0.0;

  thlinzoomtrans();

  void init_points(thvec2 from, thvec2 to);
  void init_from(thvec2 src, double dst);
  void init_to(thvec2 src, double dst);
  void init();

  thvec2 forward(thvec2 src);
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
  
  std::unique_ptr<struct thmorph2trans_members> m;
  double m_eps = {};

  thmorph2trans();
  ~thmorph2trans();
  thmorph2trans(const thmorph2trans&) = delete;
  thmorph2trans(thmorph2trans&&) = delete;
  thmorph2trans& operator=(const thmorph2trans&) = delete; 
  thmorph2trans& operator=(thmorph2trans&&) = delete; 
  void reset();
  void insert_point(thvec2 src, thvec2 dst, long id, double value = thnan);
  void insert_extra_point(thvec2 src, thvec2 dst, long id, double value = thnan);
  void insert_line(long from, long to);
  void insert_lines_from_db();
  void insert_zoom_point(thvec2 src, double dst, long id);
  void init(double eps = 0.01);
  thvec2 forward(thvec2 src);
  thvec2 backward(thvec2 dst, thvec2 ini = thvec2(0,0));
  double interpolate(thvec2 src);

};

#endif
