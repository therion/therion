/** 
 * @file thdb2dpt.h
 * 2D point class.
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
 
#ifndef thdb2dpt_h
#define thdb2dpt_h

#include <list>

/**
 * 2D point class.
 */
 
class thdb2dpt {

  public:
  
  double x,y,xt,yt,zt,at; ///< Coordinates.

  class thscrap * pscrap; ///< Parent scrap pointer.
  
  class thdb2dji * join_item; ///< Join item.

  thdb2dpt(); ///< Default constructor
  
  void export_mp(class thexpmapmpxs * out);
  
};


typedef std::list <thdb2dpt> thdb2dpt_list;  ///< Points list.


#endif


