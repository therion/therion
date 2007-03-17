/**
 * @file thbezier.h
 * Bezier curve manipulation class.
 */
  
/* Copyright (C) 2007 Stacho Mudrak
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
 
#ifndef thbezier_h
#define thbezier_h

#include <list>
#include <vector>


struct thbezier_point {
  double m_x, m_y, m_z;
  bool m_valid;
  thbezier_point() : m_x(0.0), m_y(0.0), m_z(0.0), m_valid(false) {}
  thbezier_point(double x, double y, double z = 0.0) : m_x(x), m_y(y), m_z(z), m_valid(true) {}
};


struct thbezier_segment {
  thbezier_point m_cp1, m_cp2, m_p;
};


struct thbezier_curve {
  std::list<thbezier_segment> m_segments;
  std::list<thbezier_segment>::iterator m_current_segment;
  void clear();
  size_t get_length() {return this->m_segments.size();}
  thbezier_segment * insert_segment();
  thbezier_segment * get_first_segment();
  thbezier_segment * get_current_segment();
  thbezier_segment * get_next_segment();
  void copy_polyline(struct thbezier_polyline * line, double err);
};


struct thbezier_polyline {
  std::list<thbezier_point> m_points;
  std::list<thbezier_point>::iterator m_current_point;
  void clear();
  size_t get_length() {return this->m_points.size();}
  thbezier_point * insert_point();
  thbezier_point * get_first_point();
  thbezier_point * get_current_point();
  thbezier_point * get_next_point();
  void copy_curve(struct thbezier_curve * curve, double err);
};


#endif