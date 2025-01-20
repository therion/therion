/**
 * @file thdb2dcp.cxx
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

#include "thdb2dcp.h"

thdb2dcp::thdb2dcp() {

  this->point = NULL;
  this->pt = NULL;
  this->prevcp  = NULL;
  this->nextcp = NULL;
  this->st = NULL;
//  this->xst = NULL;
  this->is_attached = false;
  this->used_in_attachement = false;
  this->is_sticky = 0;
  
  this->tx = 0.0;
  this->ty = 0.0;
  this->dx = 0.0;
  this->dy = 0.0;
  this->oxt = 0.0;
  this->oyt = 0.0;
  this->tz = 0.0;
  this->ta = 0.0;

}


