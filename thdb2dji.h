/** 
 * @file thdb2dpt.h
 * 2D join item class.
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
 
#ifndef thdb2dji_h
#define thdb2dji_h

#include <list>
#include "thobjectname.h"

/**
 * 2D join item class.
 */
 
class thdb2dji {

  public:
  
  thdb2dji * next_item,  ///< Next join item.
    * prev_item,  ///< Prev join item.
    * prev_list,  ///< Previous join list.
    * next_list,  ///< Next join list.
    * prev_list_item,  ///< Prev list item.
    * next_list_item;  ///< Next list item.
  
  thobjectname name;  ///< Item name.
  char * mark;  ///< Point mark.
  bool is_active; ///< Whether join item is active.
  
  class thdb2dpt * point, * cp1, * cp2;
  class thdb2dlp * line_point;
  class thdataobject * object;
  
  thdb2dji();  ///< Default constructor.
  
  void parse_item(char * istr);  ///< Parse join item.
  
};


typedef std::list <thdb2dji> thdb2dji_list;  ///< Joins list.


#endif


