/**
 * @file thconvert.cxx
 */
  
/* Copyright (C) 2005 Martin Budaj
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
 
// #include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
//#include <deque>
#include <map>
#include <set>
//#include <vector>
#include <string>

#include <cstring>
#include <cstdio>
#include <cfloat>
#include <cassert>
#include <cmath>

#include "thepsparse.h"
#include "thpdfdbg.h"
#include "thpdfdata.h"
//#include "thtexfonts.h"
#include "therion.h"
#include "thversion.h"
#include "thlegenddata.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

static const char base64_tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_encode(char * fname, ofstream & fout) {
  int llength = 0;
  char in_buffer[3];
  unsigned char out_buffer[4];

  ifstream fin(fname, ios::binary);

  do {
      for (int i = 0; i < 3; i++) in_buffer[i] = '\x0';
      for (int i = 0; i < 4; i++) out_buffer[i] = '=';

      fin.read(in_buffer, 3);
      unsigned long value = 
        ((unsigned char)in_buffer[0]) << 16 |
        ((unsigned char)in_buffer[1]) << 8 |
	((unsigned char)in_buffer[2]) << 0;

      out_buffer[0] = base64_tab[(value >> 18) & 0x3F];
      out_buffer[1] = base64_tab[(value >> 12) & 0x3F];
      if (fin.gcount() > 1) out_buffer[2] = base64_tab[(value >> 6) & 0x3F];
      if (fin.gcount() > 2) out_buffer[3] = base64_tab[(value >> 0) & 0x3F];

      if (llength >= 76) {
        fout << endl;
        llength = 0;
      }
      fout << out_buffer[0] << out_buffer[1] << out_buffer[2] << out_buffer[3];
      llength += 4;
  } while (fin);

  fin.close();
}

void find_dimensions(double & MINX,double & MINY,double & MAXX,double & MAXY) {
  double llx = 0, lly = 0, urx = 0, ury = 0;
  MINX=DBL_MAX, MINY=DBL_MAX, MAXX=-DBL_MAX, MAXY=-DBL_MAX;
  for (list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    llx = DBL_MAX; lly = DBL_MAX; urx = -DBL_MAX; ury = -DBL_MAX;
    if (I->F != "" && I->E == "" && I->G == "" && 
        I->B == "" && I->X == "") { // clipped symbols shouldn't affect map size
      if (I->F1 < llx) llx = I->F1;
      if (I->F2 < lly) lly = I->F2;
      if (I->F3 > urx) urx = I->F3;
      if (I->F4 > ury) ury = I->F4;
    }
    if (I->E != "") {
      if (I->E1 < llx) llx = I->E1;
      if (I->E2 < lly) lly = I->E2;
      if (I->E3 > urx) urx = I->E3;
      if (I->E4 > ury) ury = I->E4;
    }
    if (I->G != "") {
      if (I->G1 < llx) llx = I->G1;
      if (I->G2 < lly) lly = I->G2;
      if (I->G3 > urx) urx = I->G3;
      if (I->G4 > ury) ury = I->G4;
    }
    if (I->B != "") {
      if (I->B1 < llx) llx = I->B1;
      if (I->B2 < lly) lly = I->B2;
      if (I->B3 > urx) urx = I->B3;
      if (I->B4 > ury) ury = I->B4;
    }
    if (I->I != "") {
      if (I->I1 < llx) llx = I->I1;
      if (I->I2 < lly) lly = I->I2;
      if (I->I3 > urx) urx = I->I3;
      if (I->I4 > ury) ury = I->I4;
    }
    if (I->X != "") {
      if (I->X1 < llx) llx = I->X1;
      if (I->X2 < lly) lly = I->X2;
      if (I->X3 > urx) urx = I->X3;
      if (I->X4 > ury) ury = I->X4;
    }

    if (llx == DBL_MAX || lly == DBL_MAX || urx == -DBL_MAX || ury == -DBL_MAX) 
      therror(("This can't happen -- no data for a scrap!"));
    
    map<int,layerrecord>::iterator J = LAYERHASH.find(I->layer);
    assert(J != LAYERHASH.end());

    map<int,set<string> >::iterator K = (((*J).second).scraps).find(I->level);
    if (K == (((*J).second).scraps).end()) {
      set<string> SCRP;
      (((*J).second).scraps).insert(make_pair(I->level,SCRP));
      K = (((*J).second).scraps).find(I->level);
    }
    ((*K).second).insert(I->name);
    
    (((*J).second).allscraps).insert(I->name); 
      
    if (((*J).second).Z == 0) {
      if (MINX > llx) MINX = llx;
      if (MINY > lly) MINY = lly;
      if (MAXX < urx) MAXX = urx;
      if (MAXY < ury) MAXY = ury;
    }
  }
}


void print_preview(int up,ofstream& F) {
  set<int> used_layers;
  set<string> used_scraps;
  
  if (up) { 
    F << "<g fill=\"none\" stroke=\"" << 
       rgb2svg(LAYOUT.preview_above_r,
               LAYOUT.preview_above_g,
               LAYOUT.preview_above_b) << "\" stroke-width=\"0.1\">" << endl;
  }
  else { 
    F << "<g stroke=\"none\" fill=\"" << 
       rgb2svg(LAYOUT.preview_below_r,
               LAYOUT.preview_below_g,
               LAYOUT.preview_below_b) << "\">" << endl;
  }
  used_layers = (up ? MAP_PREVIEW_UP : MAP_PREVIEW_DOWN);
  for (set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
    map<int,layerrecord>::iterator J = LAYERHASH.find(*I);
    assert (J != LAYERHASH.end());
    used_scraps = J->second.allscraps;
    if (!used_scraps.empty()) {
      for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
        if (used_scraps.count(K->name) > 0) {
          if (up) {
            if (K->B != "" && K->sect == 0) {
              F << "<use x=\"" << K->B1 << "\" y=\"" << K->B2 << "\" xlink:href=\"#B_" << K->name << "\" />" << endl;
            }
          }
          else {
            if (K->I != "" && K->sect == 0) {
              F << "<use x=\"" << K->I1 << "\" y=\"" << K->I2 << "\" xlink:href=\"#I_" << K->name << "\" />" << endl;
            }
          }
        }
      }
    }
  }
  F << "</g>" << endl;
}


void print_grid(ofstream& PAGEDEF, double LLX,double LLY,double URX,double URY) {
  if (LAYOUT.grid == 0) return;
  PAGEDEF << "<g>" << endl;
  
  paired ll, ur, lr, ul, llrot, urrot, ulrot, lrrot, llnew, urnew, origin;
  ll.x = LLX;
  ll.y = LLY; 
  ur.x = URX;
  ur.y = URY; 
  lr.x = ur.x;
  lr.y = ll.y;
  ul.x = ll.x;
  ul.y = ur.y;
  origin.x = LAYOUT.hgridorigin;
  origin.y = LAYOUT.vgridorigin;
  
  llrot = rotatedaround(ll,origin,-LAYOUT.gridrot);
  urrot = rotatedaround(ur,origin,-LAYOUT.gridrot);
  lrrot = rotatedaround(lr,origin,-LAYOUT.gridrot);
  ulrot = rotatedaround(ul,origin,-LAYOUT.gridrot);
  
  llnew.x = min(min(llrot.x, urrot.x), min(lrrot.x, ulrot.x));
  llnew.y = min(min(llrot.y, urrot.y), min(lrrot.y, ulrot.y));
  urnew.x = max(max(llrot.x, urrot.x), max(lrrot.x, ulrot.x));
  urnew.y = max(max(llrot.y, urrot.y), max(lrrot.y, ulrot.y));
  
  double grid_init_x = LAYOUT.hgridsize * floor ((llnew.x-origin.x)/LAYOUT.hgridsize) + origin.x;
  double grid_init_y = LAYOUT.vgridsize * floor ((llnew.y-origin.y)/LAYOUT.vgridsize) + origin.y;
  
  double cosr = cos(-LAYOUT.gridrot * 3.14159265 / 180);
  double sinr = sin(-LAYOUT.gridrot * 3.14159265 / 180);

  int row, col, elem;

  if (LAYOUT.proj == 0) {
    paired out,tmp;
    for (double i = grid_init_x; i < urnew.x + LAYOUT.hgridsize - 0.05; i += LAYOUT.hgridsize) {
      for (double j = grid_init_y; j < urnew.y + LAYOUT.vgridsize - 0.05; j += LAYOUT.vgridsize) {
        row = (i == grid_init_x ? 0 : (i >= urnew.x ? 2 : 1));
        col = (j == grid_init_y ? 0 : (j >= urnew.y ? 2 : 1));
	elem = row + 3*col;
        tmp.x = i+LAYOUT.gridcell[elem].x;
        tmp.y = j+LAYOUT.gridcell[elem].y;
        out = rotatedaround(tmp,origin,LAYOUT.gridrot);
//        out.x -= LLX;
//        out.y -= LLY;
        PAGEDEF << "<g transform=\"matrix(" << 
                   cosr << " " << sinr << " " << -sinr << " " << cosr << " " << 
                   out.x << " " << out.y << ")\">";
	PAGEDEF << "<use xlink:href=\"#grid_" << u2str(elem+1) << "\" />";
        PAGEDEF << "</g>" << endl;
      }
    }
  }
  else {
    grid_init_x = LLX;
    for (double j = grid_init_y; j < urnew.y + LAYOUT.vgridsize - 0.05; j += LAYOUT.vgridsize) {
      for (double i = grid_init_x; i < urnew.x + LAYOUT.hgridsize - 0.05; i += LAYOUT.hgridsize) {
        row = (i == grid_init_x ? 0 : (i >= urnew.x ? 2 : 1));
        col = (j == grid_init_y ? 0 : (j >= urnew.y ? 2 : 1));
	elem = row + 3*col;
	PAGEDEF << "<use x=\"" << i/*-LLX*/+LAYOUT.gridcell[elem].x << "\" y=\"" << 
	                      j/*-LLY*/+LAYOUT.gridcell[elem].y << "\" xlink:href=\"#grid_" << 
			      u2str(elem+1) << "\" />" << endl;
      }
    }
  } 

  PAGEDEF << "</g>" << endl;
}

void print_surface_bitmaps (ofstream &F) {
  if (LAYOUT.transparency) 
    F << "<g opacity=\"" << LAYOUT.surface_opacity << "\">" << endl;
  for (list<surfpictrecord>::iterator I = SURFPICTLIST.begin();
                                      I != SURFPICTLIST.end(); I++) {
    F << "<g transform=\"matrix(";
    F << I->xx << " " << I->yx << " " << -I->xy << " " << -I->yy << " " << 
         I->dx << " " << I->dy << ")\">";
    F << "<image x=\"0\" y=\"" << -I->height << "\" width=\"" << I->width << 
         "\" height=\"" << I->height << "\" xlink:href=\"data:image/" << 
         I->type << ";base64," << endl;
    base64_encode(I->filename, F);
    F << "\" />";
    F << "</g>" << endl;
  };
  if (LAYOUT.transparency) 
    F << "</g>" << endl;
}



#define ginit(ID) F << "<g id=\"" << ID << "\">" << endl;
#define gend  F << "</g>" << endl;



void thsvg(char * fname, int fmt, legenddata ldata = legenddata::legenddata()) {
  if (fmt == 0)
    thprintf("making svg map ... ");
  else
    thprintf("making svg (xhtml) map ... ");
  string bgcol;

  ofstream F(fname);
  //F.setf(ios::fixed, ios::floatfield);
  //F.precision(2);
  
  double llx=0, lly=0, urx=0, ury=0;
  find_dimensions(llx, lly, urx, ury);

  if (LAYOUT.proj > 0 && LAYOUT.grid > 0) {  // natiahnutie vysky aby sa zobrazil grid pod aj nad jaskynou
    lly = LAYOUT.vgridsize * floor ((lly-LAYOUT.vgridorigin)/LAYOUT.vgridsize) + LAYOUT.vgridorigin;
    ury = LAYOUT.vgridsize * ceil  ((ury-LAYOUT.vgridorigin)/LAYOUT.vgridsize) + LAYOUT.vgridorigin;
  }
  double llxo(0.0), llyo(0.0), urxo(0.0), uryo(0.0);  // rozmery mapy bez overlapu
  if (LAYOUT.map_grid) {
    llxo = llx = LAYOUT.hsize * floor (llx/LAYOUT.hsize);
    llyo = lly = LAYOUT.vsize * floor (lly/LAYOUT.vsize);
    urxo = urx = LAYOUT.hsize * ceil (urx/LAYOUT.hsize);
    uryo = ury = LAYOUT.vsize * ceil (ury/LAYOUT.vsize);
  }

  llx -= LAYOUT.overlap;
  urx += LAYOUT.overlap;
  lly -= LAYOUT.overlap;
  ury += LAYOUT.overlap;

  F << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" << endl;
  
  F << "<!-- Generated by Therion " << THVERSION << " -->" << endl;
  if (LAYOUT.doc_author != "") 
    F << "<!-- Author: " << LAYOUT.doc_author << " -->" << endl;
  if (LAYOUT.doc_title != "") 
    F << "<!-- Title: " << LAYOUT.doc_title << " -->" << endl;
  if (LAYOUT.doc_subject != "") 
    F << "<!-- Subject: " << LAYOUT.doc_subject << " -->" << endl;
  if (LAYOUT.doc_keywords != "") 
    F << "<!-- Keywords: " << LAYOUT.doc_keywords << " -->" << endl;

  if (fmt == 0)
    F << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;
  else {
    F << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << endl;
    F << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
    F << "<body>" << endl;
    // title
    if (!ldata.cavename.empty()) F << "<h2>" << ldata.cavename << "</h2>" << endl;
    if (!ldata.comment.empty()) F << "<p>" << ldata.comment << "</p>" << endl;
    // north, scale
    // ...
    if (!ldata.cavelength.empty()) F << "<p><i>" << ldata.cavelengthtitle << ":</i> " << ldata.cavelength << "</p>" << endl;
    if (!ldata.cavedepth.empty()) F << "<p><i>" << ldata.cavedepthtitle << ":</i> " << ldata.cavedepth << "</p>" << endl;
    if (!ldata.copyrights.empty()) F << "<p>" << ldata.copyrights << "</p>" << endl;
    if (LAYOUT.scalebar != "") {
      F << "<p>" << endl;
      ScBar.print_svg(F);
      F << "</p>" << endl;
    }
    if (LAYOUT.northarrow != "") {
      F << "<p>" << endl;
      NArrow.print_svg(F);
      F << "</p>" << endl;
    }
  }

  F << "<svg width=\"" << 2.54/72*(urx-llx) << 
        "cm\" height=\"" << 2.54/72*(ury-lly) << 
        "cm\" viewBox=\"" << llx << " " << -ury << 
        " " << urx-llx << " " << ury-lly << 
        "\" xmlns=\"http://www.w3.org/2000/svg\" " << // pridane pre xhtml
        "xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << endl;

  F << "<defs>" << endl;
  // patterns:
  // pattern is clipped according to width and height attributes;
  // it's not possible to specify clipping area independently
  // like using BBox in PDF -- may produce some strange results
  // if the pattern definition exceeds the BBox in MetaPost
  for (list<pattern>::iterator I = PATTERNLIST.begin();
                               I != PATTERNLIST.end(); I++) {
    F << "<pattern id=\"patt_" << I->name << 
         "\" patternUnits=\"userSpaceOnUse\"" << 
         " width=\"" << I->xstep <<   
         "\" height=\"" << I->ystep << 
         "\" patternTransform=\"matrix(" << I->xx << " " << I->xy << " " 
                                         << I->yx << " " << I->yy << " " 
                                         << I->x <<  " " << I->y  << 
         ")\">" << endl;
    F << "<g transform=\"translate(" 
                  << I->llx1-I->llx << " " << I->lly1-I->lly << ")\">" << endl;
    I->data.MP.print_svg(F);
    F << "</g>" << endl;
    F << "</pattern>" << endl;
  }
  // scraps:
  for(list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    ginit("F_" + I->name); I->Fc.MP.print_svg(F); gend;
    ginit("G_" + I->name); I->Gc.MP.print_svg(F); gend;
    ginit("B_" + I->name); I->Bc.MP.print_svg(F); gend;
    ginit("I_" + I->name); I->Ic.MP.print_svg(F); gend;
    ginit("E_" + I->name); I->Ec.MP.print_svg(F); gend;
    ginit("X_" + I->name); I->Xc.MP.print_svg(F); gend;
  }
  // grid:
  int i=0;
  for (list<converted_data>::iterator I = GRIDLIST.begin();
                                      I != GRIDLIST.end(); I++) {
    ginit("grid_" + u2str(++i)); I->MP.print_svg(F); gend;
  }
  // clip to initial viewBox
  // (browsers mostly ignore clip="auto" overflow="hidden" root svg attributes)
  F << "<clipPath id=\"clip_viewBox\">" << endl;
    F << "<path d=\"M" << llx << " " << lly << 
         "L" << urx << " " << lly << 
         "L" << urx << " " << ury << 
         "L" << llx << " " << ury << "z\" />" << endl;
  F << "</clipPath>" << endl;
  
  F << "</defs>" << endl;
  // --- end of definitions ---

  F << "<g transform=\"scale(1,-1)\" fill=\"#000000\" stroke=\"#000000\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-miterlimit=\"10\" fill-rule=\"evenodd\" clip-rule=\"evenodd\" clip-path=\"url(#clip_viewBox)\">" << endl;
  // page background:
  if ((LAYOUT.background_r != 1) || 
      (LAYOUT.background_g != 1) || 
      (LAYOUT.background_b != 1)) {
    bgcol=rgb2svg(LAYOUT.background_r, LAYOUT.background_g, LAYOUT.background_b);
    F << "<rect x=\"" << llx << "\" y=\"" << lly << 
         "\" width=\"" << urx-llx << "\" height=\"" << ury-lly << 
         "\" stroke=\"none\" fill=\"" << bgcol << "\" />" << endl;
  }
    
  // surface:
  if (LAYOUT.surface == 1) print_surface_bitmaps(F);

  // white scrap backgrounds (when transparency added):
  // grid:
  if (LAYOUT.grid == 1) print_grid(F,llx,lly,urx,ury);

  // preview down:
  if (!MAP_PREVIEW_DOWN.empty()) print_preview(0,F);
  
  // map export:
  for (map<int,layerrecord>::iterator J = LAYERHASH.begin();
                                      J != LAYERHASH.end(); J++) {
    if (J->second.Z == 0) {
      map < int,set<string> > LEVEL;
      set <string> page_text_scraps,used_scraps;
      LEVEL = J->second.scraps;
      /*for (map < int,set<string> >::iterator I_l = LEVEL.begin();
                                             I_l != LEVEL.end(); I_l++) {
        used_scraps = I_l->second;
        for (list<scraprecord>::iterator K = SCRAPLIST.begin(); 
                                         K != SCRAPLIST.end(); K++) {
           if (used_scraps.count(K->name) > 0 && K->P != "") 
              page_text_scraps.insert(K->name);
        }
      }*/
      for (map < int,set<string> >::iterator I = LEVEL.begin();
                                             I != LEVEL.end(); I++) {
        used_scraps = I->second;
        
        // background
        for (list<scraprecord>::iterator K = SCRAPLIST.begin(); 
                                         K != SCRAPLIST.end(); K++) {
          if (K->r < 0 || K->g < 0 || K->b < 0) {
            bgcol=rgb2svg(LAYOUT.foreground_r, LAYOUT.foreground_g, LAYOUT.foreground_b);
          }
          else {
            bgcol=rgb2svg(K->r, K->g, K->b);
          }
          if (used_scraps.count(K->name) > 0 && K->I != "") {
            F << "<g fill=\"" << bgcol << "\">" << endl;
            F << "<use x=\"" << K->I1 << "\" y=\"" << K->I2 << "\" xlink:href=\"#I_" << K->name << "\" />" << endl;
            F << "</g>" << endl;
          }
        }

//    F << "<use x=\"" << I->G1 << "\" y=\"" << -I->G2 << "\" xlink:href=\"#G_" << I->name << "\" />" << endl;

        for (list<scraprecord>::iterator K = SCRAPLIST.begin(); 
                                         K != SCRAPLIST.end(); K++) {
          if (used_scraps.count(K->name) > 0 && K->F != "") {
            F << "<use x=\"" << K->F1 << "\" y=\"" << K->F2 << "\" xlink:href=\"#F_" << K->name << "\" />" << endl;
          }
        }

        for (list<scraprecord>::iterator K = SCRAPLIST.begin(); 
                                         K != SCRAPLIST.end(); K++) {
          if (used_scraps.count(K->name) > 0 && K->E != "") {
            F << "<use x=\"" << K->E1 << "\" y=\"" << K->E2 << "\" xlink:href=\"#E_" << K->name << "\" />" << endl;
          }
        }

        for (list<scraprecord>::iterator K = SCRAPLIST.begin(); 
                                         K != SCRAPLIST.end(); K++) {
          if (used_scraps.count(K->name) > 0 && K->X != "") {
            F << "<use x=\"" << K->X1 << "\" y=\"" << K->X2 << "\" xlink:href=\"#X_" << K->name << "\" />" << endl;
          }
        }

      }
    }
  }

  // preview up:
  if (!MAP_PREVIEW_UP.empty()) print_preview(1,F);
  
  // surface:
  if (LAYOUT.surface == 2) print_surface_bitmaps(F);

  // grid:
  if (LAYOUT.grid == 2) print_grid(F,llx,lly,urx,ury);

  // map grid:
  if (LAYOUT.map_grid) {
    F << "<g stroke=\"#000000\" fill=\"none\" stroke-width=\"0.4\">" << endl;
    F << "<rect x=\"" << llxo << "\" y=\"" << llyo << 
         "\" width=\"" << urxo-llxo << "\" height=\"" << uryo-llyo << 
         "\" />" << endl;
    for (double i=llxo; i <= urxo; i += LAYOUT.hsize) {
      F << "<line x1=\"" << i << "\" y1=\"" << llyo << "\" x2=\"" << i << "\" y2=\"" << uryo << "\" />" << endl;
    }
    for (double i=llyo; i <= uryo; i += LAYOUT.vsize) {
      F << "<line x1=\"" << llxo << "\" y1=\"" << i << "\" x2=\"" << urxo << "\" y2=\"" << i << "\" />" << endl;
    }
    F << "</g>" << endl;
  }

  F << "</g>" << endl;
  F << "</svg>" << endl;
  // end of main SVG data block

  if (fmt > 0) {  // legend in xhtml
    // title
    if (!ldata.exploteam.empty()) F << "<p><i>" << ldata.explotitle << ":</i> " << ldata.exploteam << 
          " <i>" << ldata.explodate << "</i></p>" << endl;
    if (!ldata.topoteam.empty()) F << "<p><i>" << ldata.topotitle << ":</i> " << ldata.topoteam << 
          " <i>" << ldata.topodate << "</i></p>" << endl;
    if (!ldata.cartoteam.empty()) F << "<p><i>" << ldata.cartotitle << ":</i> " << ldata.cartoteam << 
          " <i>" << ldata.cartodate << "</i></p>" << endl;

    // color legend
    if (!COLORLEGENDLIST.empty()) {
      F << "<h3>" << ldata.colorlegendtitle << "</h3>" << endl;
      F << "<table cellspacing=\"5\">" << endl;
      for(list<colorlegendrecord>::iterator I = COLORLEGENDLIST.begin(); I != COLORLEGENDLIST.end(); I++) {
        F << "<tr>" << endl;
        F << "<td style=\"background-color: " << rgb2svg(I->R,I->G,I->B) <<
             "; height: 24px; width: 36px;\">" << endl;
        F << "</td><td>" << I->name << "</td>" << endl;
        F << "</tr>" << endl;
      }
      F << "</table>" << endl;
    }

    // map symbols
    if (!LEGENDLIST.empty()) {
      F << "<h3>" << ldata.legendtitle << "</h3>" << endl;

      vector<legendrecord> L;
      for(list<legendrecord>::iterator I = LEGENDLIST.begin(); 
          I != LEGENDLIST.end(); I++) {
            L.push_back(*I);
      }
      int legendbox_num = LEGENDLIST.size();
      int columns = LAYOUT.legend_columns; 
      int rows = (int) ceil(double(legendbox_num) / columns);
      int pos = 0;

      int i_patt = 0;
      F << "<table cellspacing=\"5\">" << endl;
      for (int i = 0; i < rows; i++) {
        F << "<tr>" << endl;
        for (int j = 0; j < columns; j++) {
          F << "<td>" << endl;
          pos = i + j * rows;
          if (pos < legendbox_num) {
            L[pos].ldata.print_svg(F,++i_patt);
            F << L[pos].descr;
            // F << "</p>" << endl;
          }
          F << "</td>" << endl;
        }
        F << "</tr>" << endl;
      }
      F << "</table>" << endl;
    }

    F << "</body>" << endl << "</html>" << endl;
  }

  F.close();
  thprintf("done\n");
}


