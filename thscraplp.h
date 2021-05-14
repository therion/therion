/** 
 * @file thscraplp.h
 * Scrap polygon line class.
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
 
#ifndef thscraplp_h
#define thscraplp_h

#include <cstddef>
#include "thdb1d.h"
#include <list>

/**
 * Scrap polygon line class.
 */
 
class thscraplp {

  public:
  
  thscraplp * next_item,
    * prev_item;
    
  thobjectname station_name;
  thdb1ds * station, * ustation;
  class thdb1d_tree_arrow * arrow;
  
  thscraplp(); ///< Default constructor
  
  double stx = 0.0, sty = 0.0, stz = 0.0, lnx1 = 0.0, lnx2 = 0.0, lny1 = 0.0, lny2 = 0.0, lnz1 = 0.0, lnz2 = 0.0;
  bool lnio;
  int type;

  void export_mp(class thexpmapmpxs * out, class thscrap * scrap, bool background);
  
};


typedef std::list <thscraplp> thscraplp_list;  ///< Points list.


#endif

