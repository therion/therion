/**
 * @file thpdfdata.cxx
 */
  
/* Copyright (C) 2003 Martin Budaj
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
 
#include <list>
#include <map>
#include <set>
#include <string>


#include "thpdfdata.h"

using namespace std;

list<scraprecord> SCRAPLIST;
map<int,layerrecord> LAYERHASH;
set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;
layout LAYOUT;

scraprecord::scraprecord() {
  S1=0.0; S2=0.0;
  layer=0; level=0; sect=0;
  F1=0.0; F2=0.0; F3=0.0; F4=0.0; 
  G1=0.0; G2=0.0; G3=0.0; G4=0.0; 
  B1=0.0; B2=0.0; B3=0.0; B4=0.0; 
  I1=0.0; I2=0.0; I3=0.0; I4=0.0; 
  E1=0.0; E2=0.0; E3=0.0; E4=0.0; 
  X1=0.0; X2=0.0; X3=0.0; X4=0.0; 
}

layerrecord::layerrecord() {
  Z=0;
  minx=0; maxx=0; miny=0; maxy=0;
  bookmark = false;
}

layout::layout() {
  transparency = true;
  overlap = 20;
  hsize = 400;
  vsize = 600;
  map_grid = false;
  hoffset = 0;
  voffset = 0;
  excl_pages = false;
  labelx = "0";
  labely = "0";
  page_numbering = true;
  nav_right = 2;
  nav_up = 2;
  nav_factor = 30;
  title_pages = false;
  background = false;
  own_pages = 0;
  OCG = true;
}
