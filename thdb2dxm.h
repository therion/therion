/** 
 * @file thdb2dxm.h
 * 2D map export class.
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
 
#ifndef thdb2dxm_h
#define thdb2dxm_h

#include <list>

/**
 * 2D export map class.
 */
 
class thdb2dxm {

  public:
  
  thdb2dxm * next_item,  ///< Next map item.
    * prev_item;  ///< Prev map item.
    
  class thdb2dxs * first_bm,  ///< First scrap in the map.
    * last_bm;  ///< Last scrap in the map.
    
  class thmap * map;

  unsigned long selection_level, output_number;    
  bool title, expand;
  
  thdb2dxm();  ///< Default constructor.
  
};


typedef std::list <thdb2dxm> thdb2dxm_list;  ///< Joins list.


#endif


