/** 
 * @file thdb2dpt.h
 * 2D map item class.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thdb2dmi_h
#define thdb2dmi_h

#include <list>
#include "thobjectname.h"
#include "thobjectsrc.h"
#include "thstok.h"


/**
 * Type of map items.
 *
 * Note order is important.
 */

enum {
  TT_MAPITEM_UNKNOWN = 0,
  TT_MAPITEM_BELOW = 1,
  TT_MAPITEM_ABOVE = 2,
  TT_MAPITEM_BESIDE = 3,
  TT_MAPITEM_NORMAL = 4,
  TT_MAPITEM_NONE = 5,
};


/**
 * Projection parsing table.
 */

static const thstok thtt_2dmi[] = {
  {"above", TT_MAPITEM_ABOVE},
  {"below", TT_MAPITEM_BELOW},
  {"none", TT_MAPITEM_NONE},
//  {"beside", TT_MAPITEM_BESIDE},
  {NULL, TT_MAPITEM_UNKNOWN}
};



struct thdb2dmi_shift {
  double m_x, m_y, m_prev_x, m_prev_y;
  int m_preview;
  thdb2dmi_shift() : m_x(0.0), m_y(0.0), m_prev_x(0.0), m_prev_y(0.0), m_preview(TT_MAPITEM_UNKNOWN) {}
  bool is_active() const;
  thdb2dmi_shift add(const thdb2dmi_shift & s);
};

bool operator == (const thdb2dmi_shift & s1, const thdb2dmi_shift & s2);

bool operator < (const thdb2dmi_shift & s1, const thdb2dmi_shift & s2);


/**
 * 2D map item class.
 */
 
class thdb2dmi {

  public:
  
  thobjectsrc source; ///< ???
  
  int type;  ///< Type of map item.
  
  thdb2dmi * next_item,  ///< Next map item.
    * prev_item;  ///< Prev map item.
    
  unsigned itm_level;  ///< Item level.
  
  thobjectname name;  ///< Item name.
  class thsurvey * psurvey;  ///< Item parent survey.
  
  class thdataobject * object;  ///< Parsed object pointer.

  thdb2dmi_shift m_shift; ///< Map shift specification.
  
  thdb2dmi();  ///< Default constructor.
};


typedef std::list <thdb2dmi> thdb2dmi_list;  ///< Items list.


#endif


