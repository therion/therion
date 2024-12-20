/**
 * @file thdb2dlp.h
 * 2D line point.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thdb2dlp_h
#define thdb2dlp_h

#include "thdb2dpt.h"
#include <list>


/**
 * Line point tags.
 */
 
enum {
  TT_LINEPT_TAG_NONE = 0,
  TT_LINEPT_TAG_ORIENT = 1,
  TT_LINEPT_TAG_SIZE = 2,
  TT_LINEPT_TAG_LSIZE = 4,
  TT_LINEPT_TAG_RSIZE = 8,
  TT_LINEPT_TAG_ALTITUDE = 16,
  TT_LINEPT_TAG_GRADIENT = 16,
  TT_LINEPT_TAG_DIRECTION = 16,
};

/**
 * 2D point class.
 */
 
class thdb2dlp {

  public:
  
  thdb2dlp * nextlp, ///< Next line point.
    * prevlp; ///< Previous line point.
    
  thdb2dpt * point, ///< The point it self.
    * cp1, ///< Control point 1.
    * cp2; ///< Control point 2.
    
  int subtype,  ///< Line subtype.
    smooth, smooth_orig,  ///< Whether line is smooth in given point.
    adjust;  ///< line point adjustment
    
  const char * mark;  ///< Line point mark.
  unsigned tags;  ///< Line point tags.
  
  double orient,rsize,lsize; ///< ???
    
  thdb2dlp(); ///< Standard constructor.

  void export_prevcp_mp(class thexpmapmpxs * out);
  void export_nextcp_mp(class thexpmapmpxs * out);
  bool get_nextcp(double & dx, double & dy);
  bool get_prevcp(double & dx, double & dy);
  double get_rotation();
  
};


typedef std::list <thdb2dlp> thdb2dlp_list;  ///< Line points list.


#endif


