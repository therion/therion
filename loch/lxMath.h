/**
 * @file lxMath.h
 * Loch mathematics module.
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

#ifndef lxMath_h
#define lxMath_h

#define lxPI 3.1415926535898

struct lxVec {

  double x, y, z;

  lxVec() : x(0.0), y(0.0), z(0.0) {};

  lxVec(double a, double b, double c) : x(a), y(b), z(c) {};

  double Length();
  double Azimuth();
  double Inclination();

  double Norm();

  void Normalize();  

  lxVec operator += ( const lxVec& v);
  lxVec operator -= ( const lxVec& v);
  lxVec operator *= ( const double& c);
  lxVec operator /= ( const double& c);

  lxVec Rotated(double a, double i);
  
};


lxVec operator + ( const lxVec& p, const lxVec& q );
lxVec operator - ( const lxVec& p, const lxVec& q );
double operator * ( const lxVec& p, const lxVec& q );
lxVec operator * ( const double& c, const lxVec& q );
lxVec operator / ( const lxVec& p, const double& c);
lxVec operator ^ ( const lxVec& p, const lxVec& q );
bool operator < ( const lxVec& p, const lxVec& q );
bool operator == ( const lxVec& p, const lxVec& q );
bool operator != ( const lxVec& p, const lxVec& q );
lxVec lxPol2Vec(const double l, const double a, const double i);
lxVec lxVecAbs(lxVec v);
lxVec lxCalcNormal4(lxVec v1, lxVec v2, lxVec v3, lxVec vN);
lxVec lxCalcNormal5(lxVec v1, lxVec v2, lxVec v3, lxVec v4, lxVec vN);

struct lxVecLimits {
  lxVec min, max, xmin, xmax, ymin, ymax, zmin, zmax;
  bool valid;
  lxVecLimits() : valid(false) {};
  void Add(double a, double b, double c);
  void Add(lxVec v);
  lxVecLimits Rotate(double a, double i, lxVec c);
  lxVec Vertex(int v);
};

#define lxVecXYZ(vec) (vec).x, (vec).y, (vec).z
#define lxMax(a,b) ((a) > (b) ? (a) : (b))
#define lxMin(a,b) ((a) < (b) ? (a) : (b))
#define lxShiftVecPXYZ(v,s) ((v)->x - (s).x), ((v)->y - (s).y), ((v)->z - (s).z)
#define lxShiftVecXYZ(v,s) ((v).x - (s).x), ((v).y - (s).y), ((v).z - (s).z)
#define lxShiftVecX3(v,s) (v[0] - (s).x), (v[1] - (s).y), (v[2] - (s).z)

#endif
