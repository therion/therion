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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------------- 
 */

#ifndef lxMath_h
#define lxMath_h

// Standard libraries
#ifndef LXDEPCHECK
#endif  
//LXDEPCHECK - standart libraries

#define lxPI 3.1415926535898

struct lxVec {

  double x, y, z;

  lxVec() : x(0.0), y(0.0), z(0.0) {};

  lxVec(double a, double b, double c) : x(a), y(b), z(c) {};

  double Length();

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
lxVec lxPol2Vec(const double l, const double a, const double i);
lxVec lxVecAbs(lxVec v);

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
#define lxShiftVecXYZ(v,s) ((v).x - (s).x), ((v).y - (s).y), ((v).z - (s).z)

#endif
