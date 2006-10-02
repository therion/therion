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


#include "thtrans.h"
#include <map>
#include <math.h>


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


thvec2 operator * (const double & c, const thvec2 & v)
{
  return thvec2(c * v.m_x, c * v.m_y);
}


thvec2 operator * (const thmat2 & m, const thvec2 & v)
{
  return thvec2(m.m_xx * v.m_x + m.m_xy * v.m_y, m.m_yx * v.m_x + m.m_yy * v.m_y);
}


thmat2 operator * (const double & c, const thmat2 & m)
{
  return thmat2(c * m.m_xx, c * m.m_xy, c * m.m_yx, c * m.m_yy);
}


thmat2 operator / (const thmat2 & m, const double & c)
{
  return thmat2(m.m_xx / c, m.m_xy / c, m.m_yx / c, m.m_yy / c);
}


void thvec2::normalize()
{
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


double thline2::eval(thvec2 p)
{
  return (p.m_x * this->m_a + p.m_y * this->m_b + this->m_c);
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
        
  if (((sumxx + sumyy) > 0) && (hypot(a,b) > 0.0)) {
    this->m_fmat.m_xx = a;
    this->m_fmat.m_xy = -b;
    this->m_fmat.m_yx = b;
    this->m_fmat.m_yy = a;
    this->m_scale = hypot(a,b);
    this->m_rot = - atan2(b,a) / 3.14159265358 * 180.0;
  }

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
  return hypot(this->m_x, this->m_y);
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
  return (src + sumv);
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
  thm2t_point() : m_used(false), m_id(-1) {}
  thm2t_point(thvec2 src, thvec2 dst, long id) : m_src(src), m_dst(dst), m_used(false), m_id(id) {}
};

typedef thm2t_point * thm2t_point_ptr;

typedef std::map<long, thm2t_point_ptr> thm2t_point_ptr_map;

typedef std::list<thm2t_point> thm2t_point_list;

struct thm2t_line {
  long m_f, m_t;
  thm2t_point_ptr m_fp, m_tp;
  bool m_used, m_single;
  struct thmorph2trans * m_trans_ptr;
  thm2t_line() : m_f(-1), m_t(-1), m_used(false), m_single(false), m_trans_ptr(NULL) {}
  thm2t_line(long f, long t, struct thmorph2trans * tptr) : m_f(f), m_t(t), m_used(false), m_single(false), m_trans_ptr(tptr) {}
  void init();
  bool calc_v2d(thvec2 src, thvec2 & v, double & d);
};

bool calc_v2d(thvec2 src, thvec2 & v, double & d)
{
  return false;
}

typedef std::list<thm2t_line> thm2t_line_list;

struct thmorph2trans_members {
  thm2t_point_list m_points;
  thm2t_point_ptr_map m_point_map;
  thm2t_line_list m_lines;
  thm2t_point_ptr get_point(long id);
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


thmorph2trans::thmorph2trans()
{
  this->m = new thmorph2trans_members;
  this->reset();
}

thmorph2trans::~thmorph2trans()
{
  delete this->m;
}


void thmorph2trans::reset()
{
  this->m_eps = 0.01;
  this->m->m_lines.clear();
  this->m->m_points.clear();
  this->m->m_point_map.clear();
}


void thmorph2trans::insert_point(thvec2 src, thvec2 dst, long id)
{
  thm2t_point_list::iterator i;
  i = this->m->m_points.insert(this->m->m_points.end(), thm2t_point(src, dst - src, id));
  this->m->m_point_map[id] = &(*i);
}

void thm2t_line::init()
{
  this->m_used = false;
  if (this->m_trans_ptr == NULL)
    return;
  if ((m_f < 0) || (m_t < 0))
    return;
  this->m_fp = this->m_trans_ptr->m->get_point(this->m_f);
  this->m_tp = this->m_trans_ptr->m->get_point(this->m_t);
  if ((this->m_fp == NULL) || (this->m_tp == NULL))
    return;
  // CONTINUE: Inicializacia line2 struktur!
}

void thmorph2trans::insert_line(long from, long to)
{
  this->m->m_lines.push_back(thm2t_line(from, to, this));
}


void thmorph2trans::init(double eps)
{
  this->m_eps = eps;
}


thvec2 thmorph2trans::forward(thvec2 src)
{
  return thvec2();
}


thvec2 thmorph2trans::backward(thvec2 dst, thvec2 ini)
{
  return thvec2();
}


