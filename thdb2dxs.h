/** 
 * @file thdb2dxs.h
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
 
#ifndef thdb2dxs_h
#define thdb2dxs_h

#include <list>

/**
 * 2D export scrap class.
 */
 
class thdb2dxs {

  public:
  
  thdb2dxs * next_item,  ///< Next join item.
    * prev_item;  ///< Prev join item.
    
  class thmap * bm;
  class thdb2dxm * fmap;
  int mode;
  bool previewed;
  unsigned long preview_output_number;
  
  thdb2dxs();  ///< Default constructor.
  
};


typedef std::list <thdb2dxs> thdb2dxs_list;  ///< Joins list.


#endif


