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
#include "thcs.h"
#ifdef THMSVC
#define hypot _hypot
#endif


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
  this->gridcs = TTCS_LOCAL;
  
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
  this->total_sdx = thnan;
  this->total_sdy = thnan;
  this->total_sdz = thnan;
  this->adj_dx = thnan;
  this->adj_dy = thnan;
  this->adj_dz = thnan;
  
  this->fxx = thnan;
  this->txx = thnan;
  this->extend = TT_EXTENDFLAG_NORMAL;
    
  this->length_sd = thnan;
  this->counter_sd = thnan;
  this->depth_sd = thnan;
  this->bearing_sd = thnan;
  this->gradient_sd = thnan;
  this->dx_sd = thnan;
  this->dy_sd = thnan;
  this->dz_sd = thnan;
  this->x_sd = thnan;
  this->y_sd = thnan;
  this->z_sd = thnan;
  this->declination = thnan;
  this->implicit_declination = 0.0;
  
  this->from_up = thnan;
  this->from_down = thnan;
  this->from_left = thnan;
  this->from_right = thnan;
  
  this->to_up = thnan;
  this->to_down = thnan;
  this->to_left = thnan;
  this->to_right = thnan;
  
  this->walls = TT_AUTO;
  this->shape = TT_DATALEG_SHAPE_UNKNOWN;
  this->vtresh = thnan;
    
  this->infer_plumbs = false;
  this->infer_equates = false;
  this->direction = true;
  this->adjusted = false;
  this->to_be_adjusted = false;
  this->topofil = false;
  this->plumbed = false;

  this->loop = NULL;
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
  this->explored = thnan;
}


thdataequate::thdataequate()
{
  this->station.clear();
  this->eqid = 0;
  this->psurvey = NULL;
}

void thdataleg::calc_total_stds()
{

//  this->total_sdx = this->total_length;
//  this->total_sdy = this->total_length;
//  this->total_sdz = this->total_length;
//
//  if (this->data_type == TT_DATATYPE_NORMAL) {
//    if ((thisinf(this->gradient) == -1) || (thisinf(this->gradient) == 1)) {
//      this->total_sdx = 0.0;
//      this->total_sdy = 0.0;
//    }
//  }
  
  double dx, dy, dz, dL, dl, dT, dC, ddx, ddy, ddz, dZ;
#define setd(a,b,v) if (thisnan(b)) a = v; else a = b;
  setd(dx, this->x_sd, 0.057735026919);
  setd(dy, this->y_sd, 0.057735026919);
  setd(dz, this->z_sd, 0.057735026919);
  setd(dL, this->length_sd, 0.1);
  setd(dl, this->counter_sd, 0.1);
  setd(dZ, this->depth_sd, 0.1);
  setd(dT, this->bearing_sd, 1.0);
    dT = dT / 180.0 * 3.14159265359;
  setd(dC, this->gradient_sd, 1.0);
    dC = dC / 180.0 * 3.14159265359;
  setd(ddx, this->dx_sd, 0.057735026919);
  setd(ddy, this->dy_sd, 0.057735026919);
  setd(ddz, this->dz_sd, 0.057735026919);

  this->total_sdx = 0.003333333333;
  this->total_sdy = 0.003333333333;
  this->total_sdz = 0.003333333333;
  
  double x,y,z,T,C,L,h;
  x = this->total_dx;
  y = this->total_dy;
  z = this->total_dz;
  T = this->total_bearing / 180.0 * 3.14159265359;
  C = this->total_gradient / 180.0 * 3.14159265359;
  L = this->total_length;
  h = hypot(this->total_dx, this->total_dy);

  if (this->topofil) {
    dL = 1.41421356237 * dl;
  }

#define pow2(x) ((x) * (x))

  switch (this->data_type) {
  
    case TT_DATATYPE_NORMAL:
      if ((thisinf(this->gradient) == -1) || (thisinf(this->gradient) == 1)) {
        this->total_sdx = 0.0;
        this->total_sdy = 0.0;
      } else {
        if (L > 0.0) {
          this->total_sdx = pow2(dx) + pow2(x * dL / L) +
            pow2(y * dT) + (0.5 + pow2(sin(T) * cos(C))) 
            * pow2(z * dC);
          this->total_sdy = pow2(dy) + pow2(y * dL / L) +
            pow2(x * dT) + (0.5 + pow2(cos(T) * cos(C))) 
            * pow2(z * dC);
        }
      }
      if (L > 0.0)
        this->total_sdz = pow2(dz) + pow2(z * dL / L) + pow2(L * cos(C) * dC);
      break;
      
    case TT_DATATYPE_DIVING:
      if (pow2(h) <= (2 * pow2(dZ) + pow2(dL))) {
        this->total_sdx = 2 * pow2(dZ) + pow2(dL);
        this->total_sdy = this->total_sdx;
      } else {
        if (h > 0.0) {
          this->total_sdx = pow2(dx) + pow2(x * dL / L) + pow2(y * dT) +
            pow2(dZ * z * sin(T) / h);
          this->total_sdy = pow2(dy) + pow2(y * dL / L) + pow2(x * dT) +
            pow2(dZ * z * cos(T) / h);
        }
      }
      this->total_sdz = 2 * pow2(dZ);
      break;
      
    case TT_DATATYPE_CYLPOLAR:
      if (h > 0.0) {
        this->total_sdx = pow2(dx) + pow2(x * dL / h) + pow2(y * dT);
        this->total_sdy = pow2(dy) + pow2(y * dL / h) + pow2(x * dT);
      }
      this->total_sdz = pow2(dz) + pow2(dZ);
      break;
      
    case TT_DATATYPE_CARTESIAN:
    default:
      this->total_sdx = pow2(dx) + pow2(ddx);
      this->total_sdy = pow2(dy) + pow2(ddy);
      this->total_sdz = pow2(dz) + pow2(ddz);
      break;
  }
}

void thdataleg::export_mp_flags(FILE * out)
{
	fprintf(out, "ATTR__shotflag_splay := %s;\n", (((this->flags & TT_LEGFLAG_SPLAY) != 0) ? "true" : "false"));
	fprintf(out, "ATTR__shotflag_duplicate := %s;\n", (((this->flags & TT_LEGFLAG_DUPLICATE) != 0) ? "true" : "false"));
	fprintf(out, "ATTR__shotflag_approx := %s;\n", (((this->flags & TT_LEGFLAG_APPROXIMATE) != 0) ? "true" : "false"));
}


