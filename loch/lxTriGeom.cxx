/**
 * @file lxTriGeom.cxx
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

#include "lxTriGeom.h"

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
