/**
 * @file lxTriGeom.h
 */
  
/* Copyright (C) 2004 Stacho Mudrak
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

#pragma once

#include "lxMath.h"

#include <cstddef>
#include <list>
#include <map>
#include <vector>

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
