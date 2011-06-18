/** 
 * @file thscraplo.h
 * Scrap outline line class.
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
 
#ifndef thscraplo_h
#define thscraplo_h

#include <cstddef>
#include <list>

enum {
  TT_OUTLINE_NO,
  TT_OUTLINE_NORMAL,
  TT_OUTLINE_REVERSED,
};

/**
 * Scrap outline line class.
 */
 
class thscraplo {

  public:

  bool lfreefirst, lfreelast;
 
  thscraplo * next_line,
    * next_outline,
    * next_scrap_line;
    
  class thline * line;
  
  int mode;
  
  thscraplo(); ///< Default constructor
};


typedef std::list <thscraplo> thscraplo_list;  ///< Points list.


#endif


