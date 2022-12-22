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
#include <cstddef>
#include <list>
#include <vector>
#include <map>
#endif  
//LXDEPCHECK - standard libraries

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


struct lxTriGeomPoint {
  lxVec p, n;
  operator lxVec & () {return this->p;}
  const lxVec & operator = (const lxVec q) {this->p = q; return this->p;}
};


bool operator < ( const lxTriGeomPoint& p, const lxTriGeomPoint& q );


struct lxTriGeom3Angle {
  size_t v1, v2, v3;
  lxTriGeom3Angle() : v1(0), v2(0), v3(0) {}
  lxTriGeom3Angle(size_t vv1, size_t vv2, size_t vv3);
};


struct lxTriGeom {
  size_t m_nPoints;
  std::map<lxTriGeomPoint, size_t> m_points;
  std::vector<lxTriGeomPoint> m_positions;
  std::list<lxTriGeom3Angle> m_triangles;
  std::vector<lxTriGeom3Angle> m_geometry;
  void Clear();
  void Append(lxTriGeom * src);
  size_t InsertPoint(lxTriGeomPoint p);
  bool Insert3Angle(lxTriGeomPoint p1, lxTriGeomPoint p2, lxTriGeomPoint p3);
  bool Insert4Angle(lxTriGeomPoint p1, lxTriGeomPoint p2, lxTriGeomPoint p3, lxTriGeomPoint p4);
  size_t GetNPoints();
  lxTriGeomPoint GetPoint(size_t i);
  size_t GetNTriangles();
  lxTriGeom3Angle GetTriangle(size_t i);
  lxTriGeom() : m_nPoints(0) {}
};


struct lxPlane {
  lxVec m_point, m_normal;
  double m_delta = {};
  void Init(lxVec pt, lxVec norm);
  double CalcPosition(lxVec pt);
  bool CalcIntersection(lxVec fp, lxVec tp, lxVec & tgt);
};


#define lxVecXYZ(vec) (vec).x, (vec).y, (vec).z
#define lxMax(a,b) ((a) > (b) ? (a) : (b))
#define lxMin(a,b) ((a) < (b) ? (a) : (b))
#define lxShiftVecPXYZ(v,s) ((v)->x - (s).x), ((v)->y - (s).y), ((v)->z - (s).z)
#define lxShiftVecXYZ(v,s) ((v).x - (s).x), ((v).y - (s).y), ((v).z - (s).z)
#define lxShiftVecX3(v,s) (v[0] - (s).x), (v[1] - (s).y), (v[2] - (s).z)

#endif
