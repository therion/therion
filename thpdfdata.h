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
#include "thepsparse.h"

using namespace std;

struct scraprecord {
  string name,F,B,I,E,X,G,C,P;      // name + files
  converted_data Fc, Bc, Ic, Ec, Xc, Gc;
  double S1,S2;                      // shift
  int layer,level,sect;             // Y, V, Z
  double F1,F2,F3,F4,                // bounding boxes
        G1,G2,G3,G4,
        B1,B2,B3,B4,
        I1,I2,I3,I4,
        E1,E2,E3,E4,
        X1,X2,X3,X4;
        
  double r,g,b;
  scraprecord();
};

struct layerrecord {
  set<int> U,D;
  string N,T;
  int Z;
  int AltJump;
  int minx, maxx, miny, maxy;
  bool bookmark;
  map< int,set<string> > scraps;
  set<string> allscraps;
  
  layerrecord();
};

struct legendrecord {
  string name, fname, descr;
  converted_data ldata;
  unsigned idsym, idfig, idnum;
};

struct colorlegendrecord {
  double R, G, B;
  string texname, name;
};

struct paired {
  double x,y;
  
  paired();
};

paired rotatedaround(paired x,paired o, double th);

struct layout {
  string excl_list,labelx,labely,
         doc_author,doc_keywords,doc_subject,doc_title,doc_comment,
         northarrow, scalebar,langstr;
  bool  excl_pages,title_pages,page_numbering,
        transparency,map_grid,OCG,map_header_bg,colored_text; 
  double hsize,vsize,overlap,
        hgridsize, vgridsize,
        hgridorigin, vgridorigin, gridrot,
        nav_factor, XS,YS,XO,YO;
  int nav_right,nav_up,own_pages,lang,legend_columns;
  double hoffset, voffset, opacity, legend_width;
  double background_r, background_g, background_b;
  double foreground_r, foreground_g, foreground_b;
  double preview_below_r, preview_below_g, preview_below_b;
  double preview_above_r, preview_above_g, preview_above_b;
  
  int surface, grid, proj, grid_coord_freq; // freq 0 no, 1 border, 2 all
  string gridAA, gridAB, gridAC, 
         gridBA, gridBB, gridBC, 
         gridCA, gridCB, gridCC;
  paired gridcell[9];
  double surface_opacity;
  paired calibration_local[9], calibration_latlong[9];
  double calibration_hdist;
  
  layout();
};

extern map<int,layerrecord> LAYERHASH;
extern set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;
extern list<scraprecord> SCRAPLIST;
extern list<legendrecord> LEGENDLIST;
extern list<colorlegendrecord> COLORLEGENDLIST;
extern layout LAYOUT;
extern list<pattern> PATTERNLIST;
extern list<converted_data> GRIDLIST;
extern converted_data NArrow, ScBar;


struct surfpictrecord {
  const char * filename, * type;
  double dx, dy, xx, xy, yx, yy, width, height;
};


extern list<surfpictrecord> SURFPICTLIST;

#endif
