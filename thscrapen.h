/** 
 * @file thscrapen.h
 * Scrap end class.
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
 
#ifndef thscrapen_h
#define thscrapen_h

#include <cstddef>
#include <list>

/**
 * Scrap end class.
 */
 
class thscrapen {

  public:
 
  thscrapen * next_end;
    
  class thline * l1;
  class thdb2dlp * lp1;
  class thline * l2;
  class thdb2dlp * lp2;
  
  double cxt = 0.0, cyt = 0.0;
  bool active = false;
  
  thscrapen(); ///< Default constructor
};


typedef std::list <thscrapen> thscrapen_list;  ///< Points list.


#endif


