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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thpdfdata_h
#define thpdfdata_h

#include <string>
#include <list>
#include <map>
#include <set>
#include "thepsparse.h"

enum class shading_mode {off, quick};  // add precise

struct surfpictrecord {
  const char * filename = {}, * type = {};
  double dx = {}, dy = {}, xx = {}, xy = {}, yx = {}, yy = {}, width = {}, height = {};
};

struct scraprecord {
  std::string name,F,B,I,E,X,G,C,P;      // name + files
    // E: scrap content that is not clipped  (walls, surveys, clip off)
    // F: scrap content that will be clipped (most of map features)
    // B: scrap outline, stroked             (produced by MP draw_upscrap macro)
    // I: scrap outline, filled              (produced by MP draw_downscrap macro)     extension: bg
    // C: clipping path                      (produced by MP draw_downscrap macro)     extension: clip
    // X: labels
    // P: text bboxes used for clipping      (produced by MP process_[*]label macros)   extension: bbox
    // [G: scrap background, UNUSED]
  converted_data Fc, Bc, Ic, Ec, Xc, Gc;
  double S1,S2;                      // shift
  int layer,level,sect;              // layer; level; sect: 1 for cross-sections, 0 otherwise
  double F1,F2,F3,F4,                // bounding boxes
        G1,G2,G3,G4,
        B1,B2,B3,B4,
        I1,I2,I3,I4,
        E1,E2,E3,E4,
        X1,X2,X3,X4;

  color col_scrap;

  // Gouraud shading data (BitsPerCoordinate = 8, BitsPerComponent = 8)
  int gour_n;
  double gour_xmin, gour_xmax, gour_ymin, gour_ymax;
  std::string gour_stream;

  // Scrap identifiers
  std::string src_name, src_survey;

  std::list<surfpictrecord> SKETCHLIST;
  scraprecord();
};

struct layerrecord {
  std::set<int> U,D;  // U: preview above references, D: preview below references
  std::string N,T;    // N: layer name, T: title (creates a title page in the atlas)
  int Z;         // 0 for maps of maps (expanded), 1 for maps of scraps (basic)
  int AltJump;   // reference to the expanded layer with U/D previews in basic layers
  int minx, maxx, miny, maxy;
  bool bookmark;
  std::map< int,std::set<std::string> > scraps;
  std::set<std::string> allscraps;
  
  layerrecord();
};

struct legendrecord {
  std::string name, fname, descr;
  converted_data ldata;
  unsigned idsym = {}, idfig = {}, idnum = {};
};

struct colorlegendrecord {
  color col_legend;
  std::string texname, name;
};

struct paired {
  double x,y;
  
  paired();
};

paired rotatedaround(paired x,paired o, double th);

struct layout {
  std::string excl_list,labelx,labely,
         doc_author,doc_keywords,doc_subject,doc_title,doc_comment,
         northarrow, scalebar, altitudebar, langstr,
         icc_profile_cmyk, icc_profile_rgb, icc_profile_gray;
  bool  excl_pages,title_pages,page_numbering,
        transparency,map_grid,OCG,map_header_bg,colored_text,transparent_map_bg,geospatial;
  double hsize,vsize,overlap,
        hgridsize = 0.0, vgridsize = 0.0,
        hgridorigin, vgridorigin, gridrot = 0.0,
        nav_factor, XS = 0.0, YS = 0.0, XO = 0.0, YO = 0.0;
  int nav_right,nav_up,own_pages,lang,legend_columns;
  int alpha_step;   // alpha increment in percent
  shading_mode smooth_shading;
  double hoffset, voffset, opacity = 0.0, legend_width;
  color col_background, col_foreground, col_preview_below, col_preview_above;
  colormodel output_colormodel = {};
  
  int surface, grid = 0, proj = 0, grid_coord_freq; // freq 0 no, 1 border, 2 all
  std::string gridAA, gridAB, gridAC, 
         gridBA, gridBB, gridBC, 
         gridCA, gridCB, gridCC;
  paired gridcell[9];
  double surface_opacity;
  paired calibration_local[9], calibration_latlong[9];
  double calibration_hdist = 0.0;
  
  layout();
};

extern std::map<int,layerrecord> LAYERHASH;
extern std::set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;
extern std::list<scraprecord> SCRAPLIST;
extern std::list<legendrecord> LEGENDLIST;
extern std::list<colorlegendrecord> COLORLEGENDLIST;
extern layout LAYOUT;
extern std::list<pattern> PATTERNLIST;
extern std::list<converted_data> GRIDLIST;
extern converted_data NArrow, ScBar, AltBar;
extern std::map<std::string,gradient> GRADIENTS;



extern std::list<surfpictrecord> SURFPICTLIST;

#endif
