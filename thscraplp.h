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
  
  thscraplp(); ///< Default constructor
  
  double stx, sty, lnx1, lnx2, lny1, lny2;
  bool lnio;
  
};


typedef std::list <thscraplp> thscraplp_list;  ///< Points list.


#endif

