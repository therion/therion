/** 
 * @file thdb2dcp.h
 * 2D control point class.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
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
 
#ifndef thdb2dcp_h
#define thdb2dcp_h

#include <list>
#include "thdb2dpt.h"

/**
 * 2D point class.
 */
 
class thdb2dcp {

  public:
  
  friend class thscrap;
  friend class thdb2d;

  class thpoint * point;  ///< Point.
  
  class thdb1ds * st; ///< Station.
  
//  class thdb2dxstation * xst;  ///< Station for extended projection.

  thdb2dpt * pt;  ///< Real control point.
  
  thdb2dcp * prevcp, * nextcp;  ///< Previous and next control point.
  
  double tx,ty,tz,ta; ///< Target control point coordinates.

  double dx,dy, oxt, oyt; ///< Drifts.

  
  bool is_attached, ///< ???
    used_in_attachement;
  char is_sticky; ///< ???
  
  thdb2dcp(); ///< Default constructor.
  
};


typedef std::list <thdb2dcp> thdb2dcp_list;  ///< Points list.


#endif


