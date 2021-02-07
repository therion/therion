/** 
 * @file thlayoutln.h
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
 
#ifndef thlayoutln_h
#define thlayoutln_h

#include <list>
#include "thlayoutclr.h"

/**
 * Layout line class.
 */
 
class thlayoutln {

  public:
  
  char code;
 
  thlayoutln * next_line;
    
  const char * line;

  thlayout_color sclr; ///< Symbol color.
  
  int smid;  ///< Symbol macro ID
  
  const char * path;  ///< Source file.
  
  thlayoutln(); ///< Default constructor
};


typedef std::list <thlayoutln> thlayoutln_list;  ///< Points list.


#endif


