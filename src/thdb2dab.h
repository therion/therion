/** 
 * @file thdb2dab.h
 * Area border class.
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
 
#ifndef thdb2dab_h
#define thdb2dab_h

#include <list>
#include "thobjectname.h"
#include "thobjectsrc.h"

/**
 * Area border line.
 */
 
class thdb2dab {
  
  public:
  
  thobjectname name;  ///< Line name.
  thobjectsrc source;  ///< Line source.
  class thline * line;  ///< Line pointer.
  thdb2dab(); ///< Default constructor.
  
  thdb2dab * next_line, ///< Next line.
    * prev_line;   ///< Prev line.
  
};


typedef std::list <thdb2dab> thdb2dab_list;


#endif


