/**
 * @file lxMath.cxx
 * Loch math implementation.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
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

// Standard libraries
#ifndef LXDEPCHECK
#include <math.h>
#endif  
//LXDEPCHECK - standard libraries

#include "lxMath.h"


void lxVec::Normalize() 
{
  double n = this->Length();
  if (n > 0.0) {
    this->x /= n;
    this->y /= n;
    this->z /= n;
  }
}


double lxVec::Norm() {
  return (this->x * this->x + this->y * this->y + this->z * this->z);
}


double lxVec::Length() {
  return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}


double lxVec::Azimuth()
{
  return atan2(this->x, this->y) / lxPI * 180.0;
}


double lxVec::Inclination()
{
  return atan2(this->z, sqrt(this->x * this->x + this->y * this->y)) / lxPI * 180.0;
}


lxVec operator + ( const lxVec& p, const lxVec& q )
{
  return lxVec(p.x + q.x, p.y + q.y, p.z + q.z);
}


lxVec operator - ( const lxVec& p, const lxVec& q )
{
  return lxVec(p.x - q.x, p.y - q.y, p.z - q.z);
}


double operator * ( const lxVec& p, const lxVec& q )
{
  return (p.x * q.x + p.y * q.y + p.z * q.z);
}


lxVec operator * ( const double& c, const lxVec& q )
{
  return lxVec(c * q.x, c * q.y, c * q.z);
}


lxVec operator / ( const lxVec& p, const double& c)
{
  return (c != 0.0 ? lxVec(p.x / c, p.y / c, p.z / c) : lxVec());
}


lxVec operator ^ ( const lxVec& p, const lxVec& q )
{
  return lxVec(p.y * q.z - p.z * q.y, 
    p.z * q.x - p.x * q.z, p.x * q.y - p.y * q.x);
}


void lxVecLimits::Add(double a, double b, double c) 
{

#define lxVecLimAddSet(par) this->par = lxVec(a, b, c)

  if (this->valid) {

    if (this->min.x > a) {
      this->min.x = a;      
      lxVecLimAddSet(xmin);
    }
    if (this->max.x < a) {
      this->max.x = a;
      lxVecLimAddSet(xmax);
    }

    if (this->min.y > b) {
      this->min.y = b;
      lxVecLimAddSet(ymin);
    }
    if (this->max.y < b) {
      this->max.y = b;
      lxVecLimAddSet(xmax);
    }

    if (this->min.z > c) {
      this->min.z = c;
      lxVecLimAddSet(zmin);
    }
    if (this->max.z < c) {
      this->max.z = c;
      lxVecLimAddSet(zmax);
    }
      
  } else {

    lxVecLimAddSet(min);
    lxVecLimAddSet(max);
    lxVecLimAddSet(xmin);
    lxVecLimAddSet(xmax);
    lxVecLimAddSet(ymin);
    lxVecLimAddSet(ymax);
    lxVecLimAddSet(zmin);
    lxVecLimAddSet(zmax);
    this->valid = true;

  }
}


lxVec lxPol2Vec(const double l, const double a, const double i)
{
  return lxVec(
    l * cos(i / 180.0 * lxPI) * sin(a / 180.0 * lxPI),
    l * cos(i / 180.0 * lxPI) * cos(a / 180.0 * lxPI),
    l * sin(i / 180.0 * lxPI)
    );
}


lxVec lxVec::Rotated(double a, double i) {
  double sa = sin(a / 180.0 * lxPI), 
         ca = cos(a / 180.0 * lxPI), 
         si = sin(i / 180.0 * lxPI), 
         ci = cos(i / 180.0 * lxPI);
  
  return lxVec(
    this->x * ca      - this->y * sa                    ,      
    this->x * ci * sa + this->y * ca * ci - this->z * si,
    this->x * si * sa + this->y * si * ca + this->z * ci
  );
}

lxVec lxVecLimits::Vertex(int v) {
  switch (v) {
    case 0: return this->min;
    case 1: return lxVec(this->max.x, this->min.y, this->min.z);
    case 2: return lxVec(this->max.x, this->max.y, this->min.z);
    case 3: return lxVec(this->min.x, this->max.y, this->min.z);
    case 4: return lxVec(this->min.x, this->min.y, this->max.z);
    case 5: return lxVec(this->max.x, this->min.y, this->max.z);
    case 6: return this->max;
    default: return lxVec(this->min.x, this->max.y, this->max.z);
  }
}

lxVecLimits lxVecLimits::Rotate(double a, double i, lxVec c)
{
  lxVecLimits result;
  lxVec mymin = this->min - c;
  lxVec mymax = this->max - c;
  result.Add(mymin.Rotated(a,i));
  result.Add(lxVec(mymin.x, mymin.y, mymax.z).Rotated(a,i));
  result.Add(lxVec(mymin.x, mymax.y, mymin.z).Rotated(a,i));
  result.Add(lxVec(mymax.x, mymin.y, mymin.z).Rotated(a,i));
  result.Add(lxVec(mymax.x, mymin.y, mymax.z).Rotated(a,i));
  result.Add(lxVec(mymax.x, mymax.y, mymin.z).Rotated(a,i));
  result.Add(lxVec(mymin.x, mymax.y, mymax.z).Rotated(a,i));
  result.Add(mymax.Rotated(a,i));
  return result;
}


lxVec lxVec::operator += ( const lxVec& v)
{
  *this = lxVec(this->x + v.x, this->y + v.y, this->z + v.z);
  return *this;
}

lxVec lxVec::operator -= ( const lxVec& v)
{
  *this = lxVec(this->x - v.x, this->y - v.y, this->z - v.z);
  return *this;
}

lxVec lxVec::operator *= ( const double& c)
{
  *this = lxVec(this->x * c, this->y * c, this->z * c);
  return *this;
}

lxVec lxVec::operator /= ( const double& c)
{
  *this = lxVec(this->x / c, this->y / c, this->z / c); 
  return *this;
}

bool operator < (const struct lxVec & p, const struct lxVec & q)
{
  if (p.x < q.x)
    return true;
  if (p.x > q.x)
    return false;
  if (p.y < q.y)
    return true;
  if (p.y > q.y)
    return false;
  if (p.z < q.z)
    return true;
  return false;
}


bool operator == ( const lxVec& p, const lxVec& q )
{
  if ((!(p < q)) && (!(q < p)))
    return true;
  else
    return false;
}


bool operator != ( const lxVec& p, const lxVec& q )
{
  return !(p == q);
}



bool operator < ( const lxTriGeomPoint& p, const lxTriGeomPoint& q )
{
  if (p.p < q.p)
    return true;
  if (q.p < p.p)
    return false;
  if (p.n < q.n)
    return true;
  else
    return false;
}


void lxVecLimits::Add(lxVec v)
{
  this->Add(lxVecXYZ(v));
}


lxVec lxVecAbs(lxVec v)
{
  return lxVec(fabs(v.x), fabs(v.y), fabs(v.z));
}


size_t lxTriGeom::InsertPoint(lxTriGeomPoint p)
{
  size_t rv;
  std::map<lxTriGeomPoint, size_t>::iterator i;
  i = this->m_points.find(p);
  if (i == this->m_points.end()) {
    rv = this->m_nPoints;
    this->m_positions.push_back(p);
    this->m_nPoints++;
    this->m_points[p] = rv;
  } else {
    rv = i->second;
  }
  return rv;
}


void lxTriGeom::Clear()
{
  this->m_nPoints = 0;
  this->m_points.clear();
  this->m_positions.clear();
  this->m_triangles.clear();
  this->m_geometry.clear();
}


lxTriGeom3Angle::lxTriGeom3Angle(size_t vv1, size_t vv2, size_t vv3)
{
  this->v1 = vv1;
  this->v2 = vv2;
  this->v3 = vv3;
}


bool lxTriGeom::Insert3Angle(lxTriGeomPoint p1, lxTriGeomPoint p2, lxTriGeomPoint p3)
{
  size_t v1, v2, v3;
  v1 = this->InsertPoint(p1);
  v2 = this->InsertPoint(p2);
  v3 = this->InsertPoint(p3);
  if ((p1.p != p2.p) && (p2.p != p3.p) && (p1.p != p3.p)) {
    this->m_triangles.push_back(lxTriGeom3Angle(v1, v2, v3));
    return true;
  } else {
    return false;
  }
}


bool lxTriGeom::Insert4Angle(lxTriGeomPoint p1, lxTriGeomPoint p2, lxTriGeomPoint p3, lxTriGeomPoint p4)
{
  bool i1, i2;
  i1 = this->Insert3Angle(p1, p2, p3);
  i2 = this->Insert3Angle(p4, p3, p2);
  return (i1 && i2);
}


size_t lxTriGeom::GetNPoints()
{
  return this->m_nPoints;
}


lxTriGeomPoint lxTriGeom::GetPoint(size_t i)
{
  return this->m_positions[i];
}


size_t lxTriGeom::GetNTriangles()
{
  this->m_geometry.clear();
  std::list<lxTriGeom3Angle>::iterator i;
  for(i = this->m_triangles.begin(); i != this->m_triangles.end(); i++) {
    this->m_geometry.push_back(*i);
  }
  return this->m_geometry.size();
}


lxTriGeom3Angle lxTriGeom::GetTriangle(size_t i)
{
  return this->m_geometry[i];
}


void lxPlane::Init(lxVec pt, lxVec norm)
{
  norm.Normalize();
  this->m_normal = norm;
  this->m_point = pt;
  this->m_delta = -1.0 * (this->m_normal * this->m_point); 
}


double lxPlane::CalcPosition(lxVec pt)
{
  return (pt * this->m_normal + this->m_delta);
}


bool lxPlane::CalcIntersection(lxVec fp, lxVec tp, lxVec & tgt)
{
  lxVec d = (tp - fp);
  double y = (this->m_normal * d);
  if (y == 0.0)
    return false;
  else {
    y = ((this->m_normal * fp + this->m_delta) / y);
    tgt = fp - y * d;
    if (y > 0.0)
      return false;
    else
      return true;
  }
}


void lxTriGeom::Append(lxTriGeom * src)
{

  lxTriGeomPoint pp;
  lxTriGeom3Angle tt;
  size_t sn, sx, i;
  sn = src->GetNPoints();
  sx = this->m_nPoints;

  for(i = 0; i < sn; i++) {
    pp = src->GetPoint(i);
    this->m_nPoints++;
    this->m_positions.push_back(pp);
    this->m_points[pp] = sx + i;
  }
  
  sn = src->GetNTriangles();
  for(i = 0; i < sn; i++) {
    tt = src->GetTriangle(i);
    tt.v1 += sx;
    tt.v2 += sx;
    tt.v3 += sx;
    this->m_triangles.push_back(tt);
  }

}


lxVec lxCalcNormal4(lxVec v1, lxVec v2, lxVec v3, lxVec vN)
{
  v1 = vN - v1;
  v2 = vN - v2;
  v3 = vN - v3;
  v1 = v1 ^ (-1.0 * v2);
  v3 = v3 ^ v2;
  v1.Normalize();
  v3.Normalize();
  vN = v1 + v3;
  vN.Normalize();
  return vN;
}


lxVec lxCalcNormal5(lxVec v1, lxVec v2, lxVec v3, lxVec v4, lxVec vN)
{
  v2 = lxCalcNormal4(v1, v2, v3, vN);
  v4 = lxCalcNormal4(v3, v4, v1, vN);
  vN = v2 + v4;
  vN.Normalize();
  return vN;
}


