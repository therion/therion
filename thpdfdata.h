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
 
#ifndef thpdfdata_h
#define thpdfdata_h

#include <string>
#include <list>
#include <map>
#include <set>

using namespace std;

struct scraprecord {
  string name,F,B,I,E,X,G,C,P;      // name + files
  double S1,S2;                      // shift
  int layer,level,sect;             // Y, V, Z
  float F1,F2,F3,F4,                // bounding boxes
        G1,G2,G3,G4,
        B1,B2,B3,B4,
        I1,I2,I3,I4,
        E1,E2,E3,E4,
        X1,X2,X3,X4;
        
  scraprecord();
};

struct layerrecord {
  set<int> U,D;
  string N,T;
  int Z;
  int minx, maxx, miny, maxy;
  bool bookmark;
  map< int,set<string> > scraps;
  set<string> allscraps;
  
  layerrecord();
};

struct layout {
  string excl_list,labelx,labely,
         doc_author,doc_keywords,doc_subject,doc_title;
  bool  excl_pages,background,title_pages,page_numbering,
        transparency,map_grid,OCG; 
  double hsize,vsize,overlap,
        hgrid,vgrid,hgridoffset,vgridoffset,
	nav_factor;
  int nav_right,nav_up,own_pages;
  double hoffset, voffset,opacity;
  
  layout();
};

extern map<int,layerrecord> LAYERHASH;
extern set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;
extern list<scraprecord> SCRAPLIST;
extern layout LAYOUT;

#endif
