/**
 * @file thsketch.h
 * sketch module.
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
 
#ifndef thsketch_h
#define thsketch_h

#include <list>
#include <map>
#include <memory>

#include "thpic.h"
#include "thwarp.h"

enum {
  THSKETCH_WARP_UNKNOWN,
  THSKETCH_WARP_LINEAR,
  THSKETCH_WARP_IDPOINT,
  THSKETCH_WARP_IDLINE,
  THSKETCH_WARP_PLAQUETTE,
};

/**
 * Sketch class.
 */

struct thsketch {

  thpic m_pic;  ///< Picture.
  double m_x;  //!< sketch points X offset
  double m_y;  //!< sketch points Y offset
  std::unique_ptr<thwarp> m_warp;  ///< Warping class.
  class thscrap * m_scrap = nullptr;  ///< Sketch scrap.

  thsketch();

  thpic * morph(double scale);
  
};


typedef std::list<thsketch> thsketch_list;


#endif
