/*
 * Copyright (C) 2003 Martin Budaj
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
 

#ifndef thpdf_h
#define thpdf_h

#ifndef NOTHERION
#include "thexception.h"
#endif

#include <string>
#include <map>
#include <set>

using namespace std;

typedef struct {
  set<int> U,D;
  string N,T;
  int Z;
  float minx, maxx, miny, maxy;
  map< int,set<string> > scraps;
  set<string> allscraps;
} layerrecord;

typedef struct {
  string excl_list,labelx,labely;
  bool  excl_pages,background,title_pages,page_numbering,
        transparency,map_grid; 
  float hsize,vsize,overlap,
        hgrid,vgrid,hgridoffset,vgridoffset,
	nav_factor;
  int nav_right,nav_up,own_pages;
} layout;

extern map<int,layerrecord> LAYERHASH;
extern set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;

int thpdf(int mode, char * wdir);   // 0=atlas, 1=mapa

#endif
