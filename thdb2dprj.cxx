/**
 * @file thdb2dprj.cxx
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
 
#include "thdb2dprj.h"
#include "thinfnan.h"
#include <string.h>


thdb2dprj::thdb2dprj() {
  this->type = TT_2DPROJ_PLAN;
  this->index = "";
  this->pp1 = thnan;
  this->id = 0;
  this->processed = false;
  
  this->first_scrap = NULL;
  this->last_scrap = NULL;
  
  this->first_join = NULL;
  this->last_join = NULL;

  this->first_join_list = NULL;
  this->last_join_list = NULL;
  
  this->shift_x = 0.0;
  this->shift_y = 0.0;
  this->shift_z = 0.0;
}

thdb2dprj::thdb2dprj(char t, char * i) {
  this->type = t;
  this->index = i;
  this->pp1 = thnan;
  this->id = 0;
  this->processed = false;
  
  this->first_scrap = NULL;
  this->last_scrap = NULL;
  
  this->first_join = NULL;
  this->last_join = NULL;
  
  this->shift_x = 0.0;
  this->shift_y = 0.0;
  this->shift_z = 0.0;
}


thdb2dprjid::thdb2dprjid() {
  this->type = TT_2DPROJ_PLAN;
  this->index = "";
}


thdb2dprjid::thdb2dprjid(thdb2dprj * proj) {
  this->type = proj->type;
  this->index = proj->index;
}


bool operator < (const thdb2dprjid & p1, const thdb2dprjid & p2) {
  if (p1.type < p2.type)
    return true;
  if (p1.type == p2.type) {
    if (strcmp(p1.index,p2.index) < 0)
      return true;
  }
  return false;
}









