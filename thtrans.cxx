/**
 * @file thtrans.cxx
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
#include <math.h>
#include <map>

#include "thtrans.h"
#include "thdatabase.h"

void thvec2::reset() {
  this->m_x = 0.0;
  this->m_y = 0.0;
}


void thmat2::reset() {
  this->m_xx = 1.0;
  this->m_xy = 0.0;
  this->m_yx = 0.0;
  this->m_yy = 1.0;
}


thlintrans::thlintrans() {
  this->reset();
}


void thlintrans::reset() {
  this->m_scale = 1.0;
  this->m_rot = 0.0;
  this->m_initpts.clear();
  this->m_shift.reset();
  this->m_bmat.reset();
  this->m_fmat.reset();
}


thvec2 thvec2::operator += (const thvec2 & v)
{
  this->m_x += v.m_x;
  this->m_y += v.m_y;
  return (*this);
}


thvec2 thvec2::operator -= (const thvec2 & v)
{
  this->m_x -= v.m_x;
  this->m_y -= v.m_y;
  return (*this);
}


thvec2 thvec2::operator *= (const double & c)
{
  this->m_x *= c;
  this->m_y *= c;
  return (*this);
}


thvec2 thvec2::operator /= (const double & c)
{
  this->m_x /= c;
  this->m_y /= c;
  return (*this);
}


void thvec2::minimize(thvec2 v)
{
  if (this->m_x > v.m_x) this->m_x = v.m_x;
  if (this->m_y > v.m_y) this->m_y = v.m_y;
}


void thvec2::maximize(thvec2 v)
{
  if (this->m_x < v.m_x) this->m_x = v.m_x;
  if (this->m_y < v.m_y) this->m_y = v.m_y;
}



thmat2 thmat2::operator *= (const double & c)
{
  this->m_xx *= c;
  this->m_xy *= c;
  this->m_yx *= c;
  this->m_yy *= c;
  return (*this);
}


thmat2 thmat2::operator /= (const double & c)
{
  this->m_xx /= c;
  this->m_xy /= c;
  this->m_yx /= c;
  this->m_yy /= c;
  return (*this);
}

thmat2 thmat2::inverse()
{
  double d;
  d = (this->m_xx * this->m_yy - this->m_xy * this->m_yx);
  return thmat2(this->m_yy / d, -1.0 * this->m_xy / d, -1.0 * this->m_yx / d, this->m_xx / d);
}

thvec2 operator - (const thvec2 & v)
{
  return thvec2( - v.m_x, - v.m_y);
}

thvec2 operator + (const thvec2 & v1, const thvec2 & v2)
{
  return thvec2(v1.m_x + v2.m_x, v1.m_y + v2.m_y);
}


thvec2 operator - (const thvec2 & v1, const thvec2 & v2)
{
  return thvec2(v1.m_x - v2.m_x, v1.m_y - v2.m_y);
}


double operator * (const thvec2 & v1, const thvec2 & v2)
{
  return (v1.m_x * v2.m_x + v1.m_y * v2.m_y);
}

double operator ^ (const thvec2 & v1, const thvec2 & v2)
{
  return (v1.m_x * v2.m_y - v1.m_y * v2.m_x);
}


thvec2 operator * (const double & c, const thvec2 & v)
{
  return thvec2(c * v.m_x, c * v.m_y);
}

thvec2 operator * (const thvec2 & v, const double & c)
{
  return thvec2(v.m_x * c, v.m_y * c);
}

thvec2 operator / (const thvec2 & v, const double & c)
{
  return thvec2(v.m_x / c, v.m_y / c);
}


thvec2 operator * (const thmat2 & m, const thvec2 & v)
{
  return thvec2(m.m_xx * v.m_x + m.m_xy * v.m_y, m.m_yx * v.m_x + m.m_yy * v.m_y);
}


thmat2 operator * (const double & c, const thmat2 & m)
{
  return thmat2(c * m.m_xx, c * m.m_xy, c * m.m_yx, c * m.m_yy);
}

thmat2 operator * (const thmat2 & m, const double & c)
{
  return thmat2(c * m.m_xx, c * m.m_xy, c * m.m_yx, c * m.m_yy);
}

thmat2 operator / (const thmat2 & m, const double & c)
{
  return thmat2(m.m_xx / c, m.m_xy / c, m.m_yx / c, m.m_yy / c);
}


void thvec2::normalize()
{
  double l = this->length();
  if (l > 0.0)
    *this /= this->length();
}


thline2::thline2(thvec2 from, thvec2 to)
{
  thvec2 norm;
  norm = to - from;
  norm.normalize();
  this->m_a = - norm.m_y;
  this->m_b = norm.m_x;
  this->m_c = - (this->m_a * from.m_x + this->m_b * from.m_y);
}

thvec2::thvec2( thline2 & l1, thline2 & l2 )
{
  //      | m_xx    m_xy | * | x | = | -c1 |
  //      | m_yx    m_yy |   | y |   | -c2 |
  thmat2 m( l1.m_a, l1.m_b, l2.m_a, l2.m_b );
  thmat2 m_1 = m.inverse();
  m_x = - ( m_1.m_xx * l1.m_c + m_1.m_xy * l2.m_c );
  m_y = - ( m_1.m_yx * l1.m_c + m_1.m_yy * l2.m_c );
}


double thline2::eval(thvec2 p)
{
  return (p.m_x * this->m_a + p.m_y * this->m_b + this->m_c);
}

void thmat3::reset()
{
  m_xx = 1.0;   m_xy = 0.0;   m_xz = 0.0;
  m_yx = 0.0;   m_yy = 1.0;   m_yz = 0.0;
  m_zx = 0.0;   m_zy = 0.0;   m_zz = 1.0;
}

thmat3 thmat3::inverse()
{
  thmat3 minor;
  minor.m_xx = m_yy * m_zz - m_yz * m_zy;
  minor.m_xy = m_yx * m_zz - m_yz * m_zx;
  minor.m_xz = m_yx * m_zy - m_yy * m_zx;
  minor.m_yx = m_xy * m_zz - m_xz * m_zy;
  minor.m_yy = m_xx * m_zz - m_xz * m_zx;
  minor.m_yz = m_xx * m_zy - m_xy * m_zx;
  minor.m_zx = m_xy * m_yz - m_xz * m_yy;
  minor.m_zy = m_xx * m_yz - m_xz * m_yx;
  minor.m_zz = m_xx * m_yy - m_xy * m_yx;

  double det = m_xx * minor.m_xx - m_xy * minor.m_xy + m_xz * minor.m_xz;


  return thmat3( minor.m_xx/det, -minor.m_yx/det,  minor.m_zx/det,
                -minor.m_xy/det,  minor.m_yy/det, -minor.m_zy/det,
                 minor.m_xz/det, -minor.m_yz/det,  minor.m_zz/det );
}



void thlintrans::init(thmat2 A, thvec2 b)
{
  this->m_fmat = A;
  this->m_shift = b;
}


void thlintrans::insert_point(thvec2 src, thvec2 dst)
{
  this->m_initpts.push_back(thlintrans_pt(src, dst));
}


void thlintrans::init_points()
{
  // Initialize transformation using least squares.
  this->m_fmat.reset();
  this->m_bmat.reset();
  this->m_shift.reset();
  this->m_scale = 1.0;
  this->m_rot = 0.0;

  size_t npt = this->m_initpts.size();
  thlintrans_pt_list::iterator ipt;

  if (npt == 0)
    return;

  if (npt == 1) {
    ipt = this->m_initpts.begin();
    this->m_shift = ipt->m_tgt - ipt->m_src;
    return;
  }

  // 1. najdeme transformaciu targetov
  for (ipt = this->m_initpts.begin(); ipt != this->m_initpts.end(); ipt++) {
    this->m_shift += ipt->m_tgt;
  }
  this->m_shift /= double(npt);

  double sumXx, sumYy, sumYx, sumXy, sumxx, sumyy, X, Y, a, b;
  sumXx = 0.0; sumYy = 0.0; 
  sumYx = 0.0; sumXy = 0.0;
  sumxx = 0.0; sumyy = 0.0;
  for (ipt = this->m_initpts.begin(); ipt != this->m_initpts.end(); ipt++) {
    X = ipt->m_tgt.m_x - this->m_shift.m_x;
    Y = ipt->m_tgt.m_y - this->m_shift.m_y;
    sumXx += X * ipt->m_src.m_x;
    sumYy += Y * ipt->m_src.m_y;
    sumXy += X * ipt->m_src.m_y;
    sumYx += Y * ipt->m_src.m_x;
    sumxx += ipt->m_src.m_x * ipt->m_src.m_x;
    sumyy += ipt->m_src.m_y * ipt->m_src.m_y;
  }

  a = (sumXx + sumYy) / (sumxx + sumyy);
  b = (sumYx - sumXy) / (sumxx + sumyy);
        
  if (((sumxx + sumyy) > 0) && (std::hypot(a,b) > 0.0)) {
    this->m_fmat.m_xx = a;
    this->m_fmat.m_xy = -b;
    this->m_fmat.m_yx = b;
    this->m_fmat.m_yy = a;
    this->m_scale = std::hypot(a,b);
    this->m_rot = - atan2(b,a) / 3.14159265359 * 180.0;
  }

}

double thvec2::orientation()
{
  double o = atan2(this->m_x, this->m_y) / 3.14159265359 * 180.0;
  if (o < 0.0)
    o += 360.0;
  return o;
}

void thlintrans::init(thvec2 src, thvec2 dst) {
  this->m_fmat.reset();
  this->m_scale = 1.0;
  this->m_rot = 0.0;
  this->m_shift = dst - src;
}


void thlintrans::init(thvec2 srcF, thvec2 srcT, thvec2 dstF, thvec2 dstT)
{
  thvec2 srcV, dstV;
  srcV = srcT - srcF;
  dstV = dstT - dstF;
  double r1, r2, rr, ss;
  r1 = atan2(srcV.m_y, srcV.m_x);
  r2 = atan2(dstV.m_y, dstV.m_x);
  rr = r2 - r1;
  ss = dstV.length() / srcV.length();
  this->m_fmat = ss * thmat2(cos(rr), -sin(rr), sin(rr), cos(rr));
  this->m_shift = dstF - this->m_fmat * srcF;
  this->m_scale = ss;
  this->m_rot = rr / 3.14159265358 * 180.0;
}


void thlintrans::init_backward()
{
  this->m_bmat = this->m_fmat.inverse();
}


thvec2 thlintrans::forward(thvec2 src)
{
  return (this->m_fmat * src + this->m_shift);
}

thvec2 thlintrans::backward(thvec2 dst)
{
  return (this->m_bmat * (dst - this->m_shift));
}


double thvec2::length() {
  return std::hypot(this->m_x, this->m_y);
}


void thmorphtrans::insert_point(thvec2 src, thvec2 dst)
{
  this->m_initpts.push_back(thlintrans_pt(src, dst - src));
}


thvec2 thmorphtrans::forward(thvec2 src)
{
  thlintrans_pt_list::iterator i;
  thvec2 sumv;
  double sumw(0.0), d, w;
  for(i = this->m_initpts.begin(); i != this->m_initpts.end(); i++) {
    d = (i->m_src - src).length();
    if (d > 0.0) {
      w = 1.0 / (d * d);
      sumw += w;
      sumv += w * i->m_tgt;
    } else {
      return (src + i->m_tgt);
    }
  }
  sumv /= sumw;
  sumv += src;
  return sumv;
}


thvec2 thmorphtrans::backward(thvec2 dst, thvec2 ini)
{
#define EPS 0.01
#define FF(v) (dst - this->forward(v)).length()
  double F, a, b, d;
  thvec2 x(ini), dd;
  size_t counter(0);
  do {
    F = FF(x);
    if (F < EPS) break;
    a = (FF(x + thvec2(EPS, 0.0)) - F) / EPS;
    b = (FF(x + thvec2(0.0, EPS)) - F) / EPS;
    d = a * a + b * b;
    if (d == 0.0) break;
    dd = thvec2(a * F / d, b * F / d);
    x -= dd;
  } while (counter++ < 10);
  return x;
}




struct thm2t_point {
  thvec2 m_src, m_dst;
  bool m_used;
  long m_id;
  double m_value;
  thm2t_point() : m_used(false), m_id(-1), m_value(thnan) {}
  thm2t_point(thvec2 src, thvec2 dst, long id) : m_src(src), m_dst(dst), m_used(false), m_id(id), m_value(thnan) {}
  thm2t_point(thvec2 src, thvec2 dst, long id, double value) : m_src(src), m_dst(dst), m_used(false), m_id(id), m_value(value) {}
};

typedef thm2t_point * thm2t_point_ptr;

typedef std::map<long, thm2t_point_ptr> thm2t_point_ptr_map;

typedef std::list<thm2t_point> thm2t_point_list;

struct thm2t_line {
  long m_f, m_t;
  thm2t_line(long f, long t) {
    if (f < t) {
      this->m_f = f;
      this->m_t = t;
    } else {
      this->m_t = f;
      this->m_f = t;
    }
  }
};

typedef thm2t_line * thm2t_line_ptr;

bool operator < (const struct thm2t_line & l1, const struct thm2t_line & l2)
{
  if (l1.m_f < l2.m_f)
    return true;
  if (l1.m_f > l2.m_f)
    return false;
  if (l1.m_t < l2.m_t)
    return true;
  return false;
}

typedef std::list<thm2t_line> thm2t_line_list;
typedef std::map<thm2t_line, thm2t_line_ptr> thm2t_line_map;

struct thm2t_zoom_point {
  thvec2 m_src;
  double m_dst;
  long m_id;
  thm2t_zoom_point(thvec2 src, double dst, long id) : m_src(src), m_dst(dst), m_id(id) {}
};

typedef std::list<thm2t_zoom_point> thm2t_zoom_point_list;


struct thm2t_feature {
  bool m_single = {};
  thm2t_point_ptr m_fp = {}, m_tp = {};
  thline2 m_ln, m_lnf, m_lnt;
  thlintrans m_lintrans;
  thlinzoomtrans m_zoomtrans;
  double calc_dist(thvec2 src);
  double calc_dist_interpolate(thvec2 src, double & value);
};


double thm2t_feature::calc_dist(thvec2 src)
{
  double lnfd, lntd;
  if (this->m_single) {
    return (src - this->m_fp->m_src).length();
  } else {
    lnfd = this->m_lnf.eval(src);
    lntd = this->m_lnt.eval(src);
    if (lnfd < 0)
      return (src - this->m_fp->m_src).length();
    else if (lntd < 0)
      return (src - this->m_tp->m_src).length();
    else
      return fabs(this->m_ln.eval(src));
  }
}


double thm2t_feature::calc_dist_interpolate(thvec2 src, double & value)
{
  double lnfd, lntd;
  if (this->m_single) {
    value = this->m_fp->m_value;
    return (src - this->m_fp->m_src).length();
  } else {
    lnfd = this->m_lnf.eval(src);
    lntd = this->m_lnt.eval(src);
    if (lnfd < 0) {
      value = this->m_fp->m_value;
      return (src - this->m_fp->m_src).length();      
    } else if (lntd < 0) {
      value = this->m_tp->m_value;
      return (src - this->m_tp->m_src).length();
    } else {
      if ((lnfd + lntd) > 0)
    	  value = (lntd * this->m_fp->m_value + lnfd * this->m_tp->m_value) / (lnfd + lntd);
      else
    	  value = this->m_fp->m_value;
      return fabs(this->m_ln.eval(src));
    }
  }
}



typedef std::list<thm2t_feature> thm2t_feature_list;

struct thmorph2trans_members {
  thm2t_point_list m_points;
  thm2t_point_list m_extra_points;
  thm2t_point_ptr_map m_point_map;
  thm2t_point_ptr_map m_extra_point_map;
  thm2t_line_list m_lines;
  thm2t_line_map m_line_map;
  thm2t_point_ptr get_point(long id);
  thm2t_point_ptr get_extra_point(long id);
  thm2t_zoom_point_list m_zoom_points;
  thm2t_feature_list m_features;
};

thm2t_point_ptr thmorph2trans_members::get_point(long id)
{
  thm2t_point_ptr_map::iterator ii;
  ii = this->m_point_map.find(id);
  if (ii == this->m_point_map.end())
    return NULL;
  else
    return ii->second;
}



thm2t_point_ptr thmorph2trans_members::get_extra_point(long id)
{
  thm2t_point_ptr_map::iterator ii;
  ii = this->m_extra_point_map.find(id);
  if (ii == this->m_extra_point_map.end())
    return NULL;
  else
    return ii->second;
}


thmorph2trans::thmorph2trans()
{
  this->m = std::make_unique<thmorph2trans_members>();
  this->reset();
}

// thmorph2trans_members is forward-declared in the header,
// so the destructor of thmorph2trans must be defined in the cpp file
thmorph2trans::~thmorph2trans() = default;

void thmorph2trans::reset()
{
  this->m_eps = 0.01;
  this->m->m_lines.clear();
  this->m->m_points.clear();
  this->m->m_point_map.clear();
  this->m->m_zoom_points.clear();
  this->m->m_features.clear();
}


void thmorph2trans::insert_point(thvec2 src, thvec2 dst, long id, double value)
{
  thm2t_point_list::iterator i;
  i = this->m->m_points.insert(this->m->m_points.end(), thm2t_point(src, dst, id, value));
  this->m->m_point_map[id] = &(*i);
}


void thmorph2trans::insert_extra_point(thvec2 src, thvec2 dst, long id, double value)
{
  thm2t_point_list::iterator i;
  i = this->m->m_extra_points.insert(this->m->m_extra_points.end(), thm2t_point(src, dst, id, value));
  this->m->m_extra_point_map[id] = &(*i);
}


void thmorph2trans::insert_zoom_point(thvec2 src, double dst, long id)
{
  this->m->m_zoom_points.push_back(thm2t_zoom_point(src, dst, id));
}

void thmorph2trans::insert_line(long from, long to)
{
  thm2t_line tmp(from, to);
  thm2t_line_map::iterator mi;
  if (from == to)
    return;
  if ((this->m->get_point(from) == NULL) || (this->m->get_point(to) == NULL)) {
	if ((this->m->get_point(from) != NULL) && (this->m->get_extra_point(to) != NULL))
		this->m->get_extra_point(to)->m_used = true;
	else if ((this->m->get_extra_point(from) != NULL) && (this->m->get_point(to) != NULL))
		this->m->get_extra_point(from)->m_used = true;
	else
	  return;
  }
  mi = this->m->m_line_map.find(tmp);
  if (mi == this->m->m_line_map.end()) {
    thm2t_line_ptr lp = &(*this->m->m_lines.insert(this->m->m_lines.end(), thm2t_line(from, to)));
    this->m->m_line_map[tmp] = lp;
  }
}


void thmorph2trans::init(double eps)
{
  this->m->m_features.clear();
  this->m_eps = eps;
  
  for(auto xpi = this->m->m_extra_points.begin(); xpi != this->m->m_extra_points.end(); xpi++) {
	  if (xpi->m_used) {
		  this->insert_point(xpi->m_src, xpi->m_dst, xpi->m_id, xpi->m_value);
	  }
  }

  // initialize features
  thm2t_line_list::iterator iln;
  for(iln = this->m->m_lines.begin(); iln != this->m->m_lines.end(); iln++) {
    thm2t_feature tmpf;
    tmpf.m_fp = this->m->get_point(iln->m_f);
    tmpf.m_tp = this->m->get_point(iln->m_t);
    if ((tmpf.m_fp != NULL) && (tmpf.m_tp != NULL)) {
      if (((tmpf.m_fp->m_src - tmpf.m_tp->m_src).length() > 0.0) && 
        ((tmpf.m_fp->m_dst - tmpf.m_tp->m_dst).length() > 0.0)) {
          tmpf.m_single = false;
          tmpf.m_ln = thline2(tmpf.m_fp->m_src, tmpf.m_tp->m_src);
          tmpf.m_lnf = thline2(tmpf.m_fp->m_src, tmpf.m_fp->m_src + thvec2(-tmpf.m_ln.m_a, -tmpf.m_ln.m_b));
          tmpf.m_lnt = thline2(tmpf.m_tp->m_src, tmpf.m_tp->m_src + thvec2(tmpf.m_ln.m_a, tmpf.m_ln.m_b));
          tmpf.m_lintrans.init(tmpf.m_fp->m_src, tmpf.m_tp->m_src, tmpf.m_fp->m_dst, tmpf.m_tp->m_dst);
          tmpf.m_fp->m_used = true;
          tmpf.m_tp->m_used = true;
          this->m->m_features.push_back(tmpf);
      }
    }
  }

  thm2t_point_list::iterator ipt, ipt2;
  for(ipt = this->m->m_points.begin(); ipt != this->m->m_points.end(); ipt++) {
    thm2t_feature tmpf;
    tmpf.m_fp = &(*ipt);
    tmpf.m_tp = NULL;
    if (!tmpf.m_fp->m_used) {
      // find nearest TP
      double md, cd;
      md = -1.0;
      for(ipt2 = this->m->m_points.begin(); ipt2 != this->m->m_points.end(); ipt2++) {
        if (ipt->m_id != ipt2->m_id) {
          cd = (ipt->m_dst - ipt2->m_dst).length();
          if ((cd > 0.0) && ((ipt->m_src - ipt2->m_src).length() > 0.0)) {
            if (md < 0.0) {
              md = cd;
              tmpf.m_tp = &(*ipt2);
            } else if (cd < md) {
              md = cd;
              tmpf.m_tp = &(*ipt2);
            }
          }
        }
      }
      if (tmpf.m_tp == NULL) {
        tmpf.m_single = true;
        tmpf.m_lintrans.init(tmpf.m_fp->m_src, tmpf.m_fp->m_dst);
      } else {
        tmpf.m_single = false;
        tmpf.m_ln = thline2(tmpf.m_fp->m_src, tmpf.m_tp->m_src);
        tmpf.m_lnf = thline2(tmpf.m_fp->m_src, tmpf.m_fp->m_src + thvec2(-tmpf.m_ln.m_a, -tmpf.m_ln.m_b));
        tmpf.m_lnt = thline2(tmpf.m_tp->m_src, tmpf.m_tp->m_src + thvec2(tmpf.m_ln.m_a, tmpf.m_ln.m_b));
        tmpf.m_lintrans.init(tmpf.m_fp->m_src, tmpf.m_tp->m_src, tmpf.m_fp->m_dst, tmpf.m_tp->m_dst);
        tmpf.m_fp->m_used = true;
        tmpf.m_tp->m_used = true;
      }
      this->m->m_features.push_back(tmpf);
    }
  }

  // initialize zoom transformations
  thm2t_feature_list::iterator i;
  thm2t_zoom_point_list::iterator izp;
  for(i = this->m->m_features.begin(); i != this->m->m_features.end(); i++) {
    if (i->m_tp != NULL)
      i->m_zoomtrans.init_points(i->m_fp->m_dst, i->m_tp->m_dst);
    else
      i->m_zoomtrans.init_points(i->m_fp->m_dst, i->m_fp->m_dst);
  }

  for(izp = this->m->m_zoom_points.begin(); izp != this->m->m_zoom_points.end(); izp++) {
    for(i = this->m->m_features.begin(); i != this->m->m_features.end(); i++) {
      if (izp->m_id == i->m_fp->m_id)
        i->m_zoomtrans.init_from(this->forward(izp->m_src), izp->m_dst);
      if (i->m_tp != NULL) {
        if (izp->m_id == i->m_tp->m_id)
          i->m_zoomtrans.init_to(this->forward(izp->m_src), izp->m_dst);
      }
    }

  }

  for(i = this->m->m_features.begin(); i != this->m->m_features.end(); i++) {
    i->m_zoomtrans.init();
  }

}


thvec2 thmorph2trans::forward(thvec2 src)
{
  thm2t_feature_list::iterator i;
  thvec2 sumv, cdst;
  double sumw(0.0), d, w;
  for(i = this->m->m_features.begin(); i != this->m->m_features.end(); i++) {
    d = i->calc_dist(src);
    cdst = i->m_zoomtrans.forward(i->m_lintrans.forward(src));
    if (d > 0.0) {
      w = 1.0 / (d * d * d * d);
      sumw += w;
      sumv += w * cdst;
    } else {
      return cdst;
    }
  }
  sumv /= sumw;
  return sumv;
}


thvec2 thmorph2trans::backward(thvec2 dst, thvec2 ini)
{
#define EPS 0.01
#define FF(v) (dst - this->forward(v)).length()
  double F, a, b, d;
  thvec2 x(ini), dd;
  size_t counter(0);
  do {
    F = FF(x);
    if (F < EPS) break;
    a = (FF(x + thvec2(EPS, 0.0)) - F) / EPS;
    b = (FF(x + thvec2(0.0, EPS)) - F) / EPS;
    d = a * a + b * b;
    if (d == 0.0) break;
    dd = thvec2(a * F / d, b * F / d);
    x -= dd;
  } while (counter++ < 10);
  return x;
}


double thmorph2trans::interpolate(thvec2 src)
{
  thm2t_feature_list::iterator i;
  double sumv(0.0), sumw(0.0), d, v, w;
  for(i = this->m->m_features.begin(); i != this->m->m_features.end(); i++) {
    d = i->calc_dist_interpolate(src, v);
    if (d > 0.0) {
      w = 1.0 / (d * d * d * d);
      sumw += w;
      sumv += w * v;
    } else {
      return v;
    }
  }
  if (sumw > 0.0) sumv /= sumw;
  return sumv;
}






void thmorph2trans::insert_lines_from_db()
{
  long nst = (long) thdb.db1d.station_vec.size();
  thdb1d_tree_node * nodes = thdb.db1d.get_tree_nodes(), * cnode;
  thdb1d_tree_arrow * carrow;


  // station by station and insert shots
  thm2t_point_list::iterator ipt;
  thdataleg * lg;
  long fid, tid;
  for(ipt = this->m->m_points.begin(); ipt != this->m->m_points.end(); ipt++) {
    if ((ipt->m_id > 0) && (ipt->m_id <= nst)) {
      cnode = &(nodes[ipt->m_id - 1]);
      carrow = cnode->first_arrow;
      while (carrow != NULL) {
        lg = carrow->leg->leg;
        fid = thdb.db1d.station_vec[lg->from.id - 1].uid;
        tid = thdb.db1d.station_vec[lg->to.id - 1].uid;
        this->insert_line(fid, tid);
        carrow = carrow->next_arrow;
      }
    }
  }

}



thlinzoomtrans::thlinzoomtrans() {

  this->m_valid = false;
  this->m_single = true;

  this->m_fl = 0.0;
  this->m_fr = 0.0;
  this->m_tl = 0.0;
  this->m_tr = 0.0;

  this->m_flc = 0;
  this->m_frc = 0;
  this->m_tlc = 0;
  this->m_trc = 0;
}


void thlinzoomtrans::init_points(thvec2 from, thvec2 to) {
  this->m_from = from;
  this->m_to = to;
  thvec2 vec;
  vec = to - from;
  this->m_line_l = vec.length();
  if (this->m_line_l > 0.0) {
    this->m_line = thline2(from, to);
    this->m_line_from = thline2(from, from + thvec2(vec.m_y, -vec.m_x));
    this->m_orient_from = thvec2(vec.m_y, -vec.m_x).orientation();
    this->m_line_to = thline2(to, to + thvec2(-vec.m_y, vec.m_x));
    this->m_orient_to = thvec2(-vec.m_y, vec.m_x).orientation();
    this->m_single = false;
  } else {
    this->m_single = true;
  }
}


void thlinzoomtrans::init_from(thvec2 src, double dst) {
  double l;
  l = (src - this->m_from).length();
  if (l > 0.0) {
    if (this->m_single) {
      this->m_flc++;
      this->m_fl += (dst / l);
    } else {
      if (this->m_line.eval(src) > 0) {
        this->m_flc++;
        this->m_fl += (dst / l);
      } else {
        this->m_frc++;
        this->m_fr += (dst / l);
      }
    }
  }
}


void thlinzoomtrans::init_to(thvec2 src, double dst) {
  double l;
  l = (src - this->m_to).length();
  if (l > 0.0) {
    if (this->m_single) {
      this->m_flc++;
      this->m_fl += (dst / l);
    } else {
      if (this->m_line.eval(src) > 0) {
        this->m_tlc++;
        this->m_tl += (dst / l);
      } else {
        this->m_trc++;
        this->m_tr += (dst / l);
      }
    }
  }
}


void thlinzoomtrans::init() {
  if ((this->m_flc > 0) || (this->m_frc > 0) || (this->m_tlc > 0) || (this->m_trc > 0)) {
    
    this->m_valid = true;

    if (this->m_flc > 0) this->m_fl /= double(this->m_flc); else this->m_fl = 1.0;
    if (this->m_frc > 0) this->m_fr /= double(this->m_frc); else this->m_fr = 1.0;
    if (this->m_tlc > 0) this->m_tl /= double(this->m_tlc); else this->m_tl = 1.0;
    if (this->m_trc > 0) this->m_tr /= double(this->m_trc); else this->m_tr = 1.0;
    
    //if ((this->m_flc == 0) && (this->m_frc == 0)) {
    //  this->m_fl = 1.0;
    //  this->m_fr = 1.0;
    //} else if ((this->m_flc == 0) && (this->m_frc > 0)) {
    //  this->m_fl = this->m_fr;
    //} else if ((this->m_flc > 0) && (this->m_frc == 0)) {
    //  this->m_fr = this->m_fl;
    //} 
    //if ((this->m_tlc == 0) && (this->m_trc == 0)) {
    //  this->m_tl = 1.0;
    //  this->m_tr = 1.0;
    //} else if ((this->m_tlc == 0) && (this->m_trc > 0)) {
    //  this->m_tl = this->m_tr;
    //} else if ((this->m_tlc > 0) && (this->m_trc == 0)) {
    //  this->m_tr = this->m_tl;
    //} 
  }
}


thvec2 thlinzoomtrans::forward(thvec2 src) {
  if (this->m_valid) {
    thvec2 vec;
    double vo;
    if (this->m_single) {
      vec = src - this->m_from;
      vec *= this->m_fl;
      return (src + vec);
    } else {
      double dl, dlf, dlt;
      dl = this->m_line.eval(src);
      dlf = this->m_line_from.eval(src);
      dlt = this->m_line_to.eval(src);
      if (dlf < 0.0) {
        vec = src - this->m_from;
        vo = atan2(sin((vec.orientation() - this->m_orient_from) / 180.0 * THPI), cos((vec.orientation() - this->m_orient_from) / 180.0 * THPI)) / THPI;
        if (vo < 0.0) {
        	if (vo > -.5)
        		vo = 0.0;
        	else
        		vo = 1.0;
        }
        vec *= (vo * this->m_fl + (1.0 - vo) * this->m_fr);
        //thprintf("VF: %.8f (%.8f, %.8f)\n", vo, (this->m_from + vec).m_x, (this->m_from + vec).m_y);
        return this->m_from + vec;
      } else if (dlt < 0.0) {
        vec = src - this->m_to;
        vo = atan2(sin((this->m_orient_to - vec.orientation() - 180.0) / 180.0 * THPI), cos((this->m_orient_to - vec.orientation() - 180.0) / 180.0 * THPI)) / THPI;
        if (vo < 0.0) {
        	if (vo > -.5)
        		vo = 0.0;
        	else
        		vo = 1.0;
        }
        vec *= (vo * this->m_tl + (1.0 - vo) * this->m_tr);
        //thprintf("VT: %.8f (%.8f, %.8f)\n", vo, (this->m_to + vec).m_x, (this->m_to + vec).m_y);
        return this->m_to + vec;
      } else {
        thvec2 lp;
        double ln;
        ln = this->m_line.m_a * this->m_line.m_a + this->m_line.m_b * this->m_line.m_b;
        lp = thvec2(src.m_x - this->m_line.m_a * dl / ln, src.m_y - this->m_line.m_b * dl / ln);
        vo = this->m_line.eval(lp);
        vec = src - lp;
        vo = (lp - this->m_from).length() / this->m_line_l;
        if (dl > 0) {
          vec *= (vo * this->m_tl + (1.0 - vo) * this->m_fl);
        } else {
          vec *= (vo * this->m_tr + (1.0 - vo) * this->m_fr);
        }
        //thprintf("VI: %.8f (%.8f, %.8f)\n", vo, (lp + vec).m_x, (lp + vec).m_y);
        return lp + vec;
      }
    }
  } else {
    return src;
  }
}


void thbbox2::update(thvec2 pt) {
	if (!this->m_valid) {
		this->m_min = pt;
		this->m_max = pt;
		this->m_valid = true;
	}
	if (this->m_min.m_x > pt.m_x) this->m_min.m_x = pt.m_x;  
	if (this->m_min.m_y > pt.m_y) this->m_min.m_y = pt.m_y;  
	if (this->m_max.m_x < pt.m_x) this->m_max.m_x = pt.m_x;  
	if (this->m_max.m_y < pt.m_y) this->m_max.m_y = pt.m_y;  
}

bool thbbox2::is_valid() {
	return (this->m_min.m_x < this->m_max.m_x) && (this->m_min.m_y < this->m_max.m_y);
}


