/**
 * @file thdb2dprj.h
 * 2D projection.
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
 
#ifndef thdb2dprj_h
#define thdb2dprj_h


#include "thparse.h"
#include <map>
#include <list>

/**
 * Projection types.
 */
 
enum {
  TT_2DPROJ_UNKNOWN = -1,  ///< ???
  TT_2DPROJ_NONE = 0,  ///< None projection, for sections.
  TT_2DPROJ_PLAN = 1,  ///< Basic plan projection
  TT_2DPROJ_ELEV = 2,  ///< Elevation
  TT_2DPROJ_EXTEND = 3,  ///< Extended elevation
};


/**
 * Projection parsing table.
 */

static const thstok thtt_2dproj[] = {
  {"elevation", TT_2DPROJ_ELEV},
  {"extended", TT_2DPROJ_EXTEND},
  {"none", TT_2DPROJ_NONE},
  {"plan", TT_2DPROJ_PLAN},
  {NULL, TT_2DPROJ_UNKNOWN}
};


/**
 * 2D projection class.
 */
 
class thdb2dprj {


  public:
  
  char type;  ///< Projection type.
  int id;  ///< ID.
  char * index;  ///< Projection index.
  double pp1;  ///< Projection parameter 1.
  bool processed; ///< Check whether projection is processed.
  
  class thscrap * first_scrap,  ///< First scrap in projection.
    * last_scrap;  ///< Last scrap in projection.
    
  class thjoin * first_join,  ///< Joins in given projection.
    * last_join;  ///< Joins in given projection.
    
  class thdb2dji * first_join_list, ///< 1. Join list.
    * last_join_list;  ///< Last join list.

  double shift_x, shift_y, shift_z; ///< Shift.


  /**
   * Standard constructor.
   */
  
  thdb2dprj();


  /**
   * Constructor with arguments.
   */
  
  thdb2dprj(char, char *);
  
  
};


class thdb2dprjid {

  public:
  
  char type;  ///< Projetion type.
  char * index;  ///< Projection index.
  
  thdb2dprjid();  ///< Default constructor.
  
  thdb2dprjid(thdb2dprj * proj);  ///< Constructor from projection.
  
  thdb2dprjid(int tt, char * ii) : type(tt), index(ii) {}  ///< ???
  
};


bool operator < (const thdb2dprjid & p1, const thdb2dprjid & p2);

typedef std::list <thdb2dprj> thdb2dprj_list;

typedef std::map <thdb2dprjid, thdb2dprj *> thdb2dprjid_map;

#endif


