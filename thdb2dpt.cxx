/**
 * @file thdb2dpt.cxx
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
 
#include "thdb2dpt.h"
#include "thexpmap.h"

thdb2dpt::thdb2dpt() {

  this->x = 0.0;
  this->y = 0.0;
  this->xt = 0.0;
  this->yt = 0.0;
  this->zt = 0.0;
  this->at = 0.0;
    
  this->pscrap = NULL;
  this->join_item = NULL;

}

void thdb2dpt::export_mp(class thexpmapmpxs * out, int dbglevel) {
  switch (dbglevel) {
    case 1:
      fprintf(out->file,"(%.2f,%.2f)", thxmmxst(out, this->dbgx1, this->dbgy1));
      break;
    case 0:
      fprintf(out->file,"(%.2f,%.2f)", thxmmxst(out, this->dbgx0, this->dbgy0));
      break;
    default:
      fprintf(out->file,"(%.2f,%.2f)", thxmmxst(out, this->xt, this->yt));
  }
}
  
