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

#include "lxMath.h"

#include <cmath>

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


void lxVecLimits::Add(lxVec v)
{
  this->Add(lxVecXYZ(v));
}


lxVec lxVecAbs(lxVec v)
{
  return lxVec(fabs(v.x), fabs(v.y), fabs(v.z));
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


