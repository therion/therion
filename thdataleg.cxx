/**
 * @file thdataleg.cxx
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
 
#include "thdataleg.h"
#include "thinfnan.h"


thdataleg::thdataleg()
{
  this->clear();
}


thdataleg::~thdataleg()
{
}


void thdataleg::clear()
{
  this->is_valid = false;
  this->data_type = TT_DATATYPE_UNKNOWN;
  this->psurvey = NULL;
  this->s_mark = TT_DATAMARK_UNKNOWN;
  this->flags = TT_LEGFLAG_NONE;
  
  this->station.clear();
  this->from.clear();
  this->to.clear();
  this->length = thnan;
  this->counter = thnan;
  this->fromcounter = thnan;
  this->tocounter = thnan;
  this->depth = thnan;
  this->fromdepth = thnan;
  this->todepth = thnan;
  this->depthchange = thnan;
  this->bearing = thnan;
  this->gradient = thnan;
  this->backbearing = thnan;
  this->backgradient = thnan;
  this->dx = thnan;
  this->dy = thnan;
  this->dz = thnan;
  this->total_length = thnan;
  this->total_bearing = thnan;
  this->total_gradient = thnan;
  this->total_dx = thnan;
  this->total_dy = thnan;
  this->total_dz = thnan;
    
  this->length_sd = thnan;
  this->counter_sd = thnan;
  this->depth_sd = thnan;
  this->bearing_sd = thnan;
  this->gradient_sd = thnan;
  this->dx_sd = thnan;
  this->dy_sd = thnan;
  this->dz_sd = thnan;
  this->declination = thnan;
    
  this->infer_plumbs = false;
  this->infer_equates = false;
  this->direction = true;
}


thdatafix::thdatafix()
{
  this->station.clear();
  this->psurvey = NULL;
  this->x = thnan;
  this->y = thnan;
  this->z = thnan;
  this->sdx = thnan;
  this->sdy = thnan;
  this->sdz = thnan;
  this->cxy = thnan;
  this->cyz = thnan;
  this->cxz = thnan;
}


thdatass::thdatass()
{
  this->station.clear();
  this->comment = NULL;
  this->flags = TT_STATIONFLAG_NONE;
  this->psurvey = NULL;
}


thdataequate::thdataequate()
{
  this->station.clear();
  this->eqid = 0;
  this->psurvey = NULL;
}


