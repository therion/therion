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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "thdb2dlp.h"
#include "thdb2dpt.h"
#include "thline.h"
#include "thparse.h"
#include "thinfnan.h"
#include "thexpmap.h"

thdb2dlp::thdb2dlp () {

  this->nextlp = NULL;
  this->prevlp = NULL;

  this->point = NULL;
  this->cp1 = NULL;
  this->cp2 = NULL;  

  this->subtype = TT_LINE_SUBTYPE_UNKNOWN;
  this->smooth = TT_AUTO;
  this->smooth_orig = TT_AUTO;
  this->mark = NULL;
  
  this->orient = thnan;
  this->rsize = thnan;
  this->lsize = thnan;
  this->tags = TT_LINEPT_TAG_NONE;
  this->adjust = TT_LINE_ADJUST_NONE;
}

void thdb2dlp::export_prevcp_mp(class thexpmapmpxs * out) {
  double dx, dy;
  this->get_prevcp(dx,dy);
  fprintf(out->file,"(%g,%g)", thxmmxst(out, (this->point->xt + dx), (this->point->yt + dy)));
}


void thdb2dlp::export_nextcp_mp(class thexpmapmpxs * out) {
  double dx, dy;
  this->get_nextcp(dx,dy);
  fprintf(out->file,"(%g,%g)", thxmmxst(out, (this->point->xt + dx), (this->point->yt + dy)));
}


bool thdb2dlp::get_prevcp(double & dx, double & dy) {
  bool postprocess = false;
  dx = 0.0;
  dy = -1.0;
  if ((this->cp2 != NULL) &&
      ((this->cp2->xt != this->point->xt) || 
      (this->cp2->yt != this->point->yt))) {
      dx = this->cp2->xt - this->point->xt;
      dy = this->cp2->yt - this->point->yt;
      postprocess = true;
    }
  else if ((this->cp1 != NULL) &&
      ((this->cp1->xt != this->point->xt) || 
      (this->cp1->yt != this->point->yt))) {
      dx = this->cp1->xt - this->point->xt;
      dy = this->cp1->yt - this->point->yt;
      postprocess = true;
    }
  else if ((this->prevlp != NULL) &&
      ((this->prevlp->point->xt != this->point->xt) || 
      (this->prevlp->point->yt != this->point->yt))){
      dx = this->prevlp->point->xt - this->point->xt;
      dy = this->prevlp->point->yt - this->point->yt;
      postprocess = true;
    }
  else if (this->nextlp != NULL) {
    if ((this->nextlp->cp1 != NULL) &&
        ((this->nextlp->cp1->xt != this->point->xt) || 
        (this->nextlp->cp1->yt != this->point->yt))) {
      dx = this->point->xt - this->nextlp->cp1->xt;
      dy = this->point->yt - this->nextlp->cp1->yt;
      postprocess = true;
    }
    else if ((this->nextlp->cp2 != NULL) &&
        ((this->nextlp->cp2->xt != this->point->xt) || 
        (this->nextlp->cp2->yt != this->point->yt))) {
      dx = this->point->xt - this->nextlp->cp2->xt;
      dy = this->point->yt - this->nextlp->cp2->yt;
      postprocess = true;
    }
    else {
      dx = this->point->xt - this->nextlp->point->xt;
      dy = this->point->yt - this->nextlp->point->yt;
      postprocess = true;
    }
  }
  return postprocess;
}

bool thdb2dlp::get_nextcp(double & dx, double & dy) {
  bool postprocess = false;
  dx = 0.0;
  dy = 1.0;
  if (this->nextlp != NULL) {
    if ((this->nextlp->cp1 != NULL) &&
        ((this->nextlp->cp1->xt != this->point->xt) || 
        (this->nextlp->cp1->yt != this->point->yt))) {
        dx = this->nextlp->cp1->xt - this->point->xt;
        dy = this->nextlp->cp1->yt - this->point->yt;
        postprocess = true;
      }
    else if ((this->nextlp->cp2 != NULL) &&
        ((this->nextlp->cp2->xt != this->point->xt) || 
        (this->nextlp->cp2->yt != this->point->yt))) {
        dx = this->nextlp->cp2->xt - this->point->xt;
        dy = this->nextlp->cp2->yt - this->point->yt;
        postprocess = true;
      }
    else {
      dx = this->nextlp->point->xt - this->point->xt;
      dy = this->nextlp->point->yt - this->point->yt;
      postprocess = true;
    }
  } else if ((this->cp2 != NULL) &&
      ((this->cp2->xt != this->point->xt) || 
      (this->cp2->yt != this->point->yt))) {
    dx = this->point->xt - this->cp2->xt;
    dy = this->point->yt - this->cp2->yt;
    postprocess = true;
  }
  else if ((this->cp1 != NULL) &&
      ((this->cp1->xt != this->point->xt) || 
      (this->cp1->yt != this->point->yt))) {
    dx = this->point->xt - this->cp1->xt;
    dy = this->point->yt - this->cp1->yt;
    postprocess = true;
  }
  else if ((this->prevlp != NULL) &&
      ((this->prevlp->point->xt != this->point->xt) || 
      (this->prevlp->point->yt != this->point->yt))) {
    dx = this->point->xt - this->prevlp->point->xt;
    dy = this->point->yt - this->prevlp->point->yt;
    postprocess = true;
  }
  return postprocess;
}

double thdb2dlp::get_rotation() {
  double dx,dy,x,y;
  this->get_nextcp(x,y);
  this->get_prevcp(dx,dy);
  x -= dx;
  y -= dy;
  return (atan2(y,x) / 3.14159265358 * 180);
}

