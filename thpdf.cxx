/**
 * @file thpdf.cxx
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
 
// #include <iomanip>
#include <iostream>
#include <strstream>
#include <fstream>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <string>

// #include <cstring>
#include <cstdio>
#include <cfloat>
#include <cmath>
#include <unistd.h>

#include "thconvert.h"
#include "thpdf.h"
#include "thpdfdbg.h"

using namespace std;



///////////

extern list<scraprecord> SCRAPLIST;
extern map<int,layerrecord> LAYERHASH;
extern set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;

layout LAYOUT;

int mode;

//////////

const int ATLAS = 0, MAP = 1;
double MINX=DBL_MAX, MINY=DBL_MAX, MAXX=-DBL_MAX, MAXY=-DBL_MAX;
float HS,VS;
//////////


void read_settings() {
  ifstream F("scraps.dat");
  if(!F) therror(("???"));
  char buf[101],ctok[100];
  string tok;
  int context = 0;   // 0=scrap, 1=layer, 2=map preview, 3=legend
  int i;
  float llx,lly,urx,ury;
  scraprecord SCR = {"","","","","","","","","",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0,0,0,0,0,0};
  layerrecord L;
  list<scraprecord>::iterator I;
  map<int,layerrecord>::iterator J;

  while(F.getline(buf,100,'\n')) {
    istrstream S(buf);
    S >> ctok;
    tok = ctok;
    if      (tok == "[SCRAP]")  context = 0;
    else if (tok == "[LAYER]")  context = 1;
    else if (tok == "[MAP]")    context = 2;
    else if (tok == "[LEGEND]") context = 3;
    else if (tok == "N") {
      switch (context) {
        case 0:
          S >> ctok;
          SCRAPLIST.push_front(SCR);
          I = SCRAPLIST.begin();
          I->name = ctok;
          break;
        case 1:
          S >> ctok; ((*J).second).N = ctok;
          while(S >> ctok) {
            ((*J).second).N = ((*J).second).N + " " + string(ctok);
          }
          break;
      }
    }
    else if (tok == "F") {
      S >> llx >> lly >> urx >> ury;
      I->F = " "; I->F1 = llx; I->F2 = lly; I->F3 = urx; I->F4 = ury;
    }
    else if (tok == "G") {
      S >> llx >> lly >> urx >> ury;
      I->G = " "; I->G1 = llx; I->G2 = lly; I->G3 = urx; I->G4 = ury;
    }
    else if (tok == "B") {
      S >> llx >> lly >> urx >> ury;
      I->B = " "; I->B1 = llx; I->B2 = lly; I->B3 = urx; I->B4 = ury;
    }
    else if (tok == "I") {
      S >> llx >> lly >> urx >> ury;
      I->I = " "; I->I1 = llx; I->I2 = lly; I->I3 = urx; I->I4 = ury;
    }
    else if (tok == "E") {
      S >> llx >> lly >> urx >> ury;
      I->E = " "; I->E1 = llx; I->E2 = lly; I->E3 = urx; I->E4 = ury;
    }
    else if (tok == "X") {
      S >> llx >> lly >> urx >> ury;
      I->X = " "; I->X1 = llx; I->X2 = lly; I->X3 = urx; I->X4 = ury;
    }
    else if (tok == "P") {
      S >> ctok;  I->P = ctok;  
      S >> llx >> lly;  I->S1 = llx; I->S2 = lly;
    }
    else if (tok == "Y") {
      S >> i;  I->layer = i;
    }
    else if (tok == "V") {
      S >> i;  I->level = i;
    }
    else if (tok == "R") {
      S >> i;
      LAYERHASH.insert(make_pair(i,L));
      J = LAYERHASH.find(i);
      ((*J).second).Z = 0;
    }
    else if (tok == "U") {
      switch (context) {
        case 1:
          while(S >> i) {
            (((*J).second).U).insert(i);
          }
          break;
        case 2:
          while(S >> i) {
            MAP_PREVIEW_UP.insert(i);
          }
          break;
      }
    }
    else if (tok == "D") {
      switch (context) {
        case 1:
          while(S >> i) {
            (((*J).second).D).insert(i);
          }
          break;
        case 2:
          while(S >> i) {
            MAP_PREVIEW_DOWN.insert(i);
          }
          break;
      }
    }
    else if (tok == "T") {
      S >> ctok; ((*J).second).T = ctok;
      while(S >> ctok) {
        ((*J).second).T = ((*J).second).T + " " + string(ctok);
      }
    }
    else if (tok == "Z") {
      switch (context) {
        case 0:
          S >> i;  I->sect = i;
          break;
        case 1:
          ((*J).second).Z = 1;
          break;
      }
    }
  }
  F.close();
  SCRAPLIST.reverse();
}

void make_sheets() {
  double llx = 0, lly = 0, urx = 0, ury = 0;
  for (list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    llx = DBL_MAX; lly = DBL_MAX; urx = -DBL_MAX; ury = -DBL_MAX;
    if (I->F != "") {
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
    
    if (mode == ATLAS) {
    }
    else {
      map<int,layerrecord>::iterator J = LAYERHASH.find(I->layer);
      if (J == LAYERHASH.end()) therror (("This can't happen!"));
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
//  cout << "MINMAX " << MINX << " " << MINY << " " << MAXX << " " << MAXY << endl;
}

void print_pdf_queue(deque<string>& thstack, float llx, float lly, 
                string command, ofstream& TEX) {
  char x[20],y[20];
  for(unsigned i=0; i<thstack.size(); i=i+2) {
//    TEX << setprecision(1) << fixed;  // doesn't work on my compiler
    sprintf(x, "%.1f", atof(thstack[i].c_str())-llx);
    sprintf(y, "%.1f", atof(thstack[i+1].c_str())-lly);
    TEX << x << " " << y << " ";
  }
  TEX << command;
}

void print_preview(int up,ofstream& PAGEDEF,double HSHIFT,double VSHIFT) {
  set<int> used_layers;
  set<string> used_scraps;
  double xc = 0, yc = 0;
  
  PAGEDEF << (up ? "\\PL{q .1 w}%" : "\\PL{q .8 g}%") << endl;
  if (mode == ATLAS) {
    
  }
  else {
    used_layers = (up ? MAP_PREVIEW_UP : MAP_PREVIEW_DOWN);
  }
  for (set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
    if (mode == ATLAS) {
    
    }
    else {
      map<int,layerrecord>::iterator J = LAYERHASH.find(*I);
      if (J == LAYERHASH.end()) therror(("This can't happen!"));
      used_scraps = J->second.allscraps;
    }
    if (!used_scraps.empty()) {
      for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
        if (used_scraps.find(K->name) != used_scraps.end()) {
          if (up) {
            if (K->B != "" && K->sect == 0) {
              xc = K->B1; yc = K->B2;
              xc -= HSHIFT; yc -= VSHIFT;
              PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                      tex_Xname("B"+(K->name)) << "}%" << endl;
            }
          }
          else {
            if (K->I != "" && K->sect == 0) {
              xc = K->I1; yc = K->I2;
              xc -= HSHIFT; yc -= VSHIFT;
              PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                      tex_Xname("I"+(K->name)) << "}%" << endl;
            }
          }
        }
      }
    }
  }
  PAGEDEF << "\\PL{Q}%" << endl;
}


void print_map(int layer, ofstream& PAGEDEF, ofstream& PAGE){
  double HSHIFT=0, VSHIFT=0, xc = 0, yc = 0;
  map < int,set<string> > LEVEL;
  set <string> page_text_scraps,used_scraps;
  string buffer;
  deque<string> thstack;

  if (mode == ATLAS) {
  
  }
  else {
    HSHIFT = MINX;
    VSHIFT = MINY;
    LEVEL = ((*(LAYERHASH.find(layer))).second).scraps;
  }

  for (map < int,set<string> >::iterator I = LEVEL.begin();
                                         I != LEVEL.end(); I++) {
    used_scraps = (*I).second;
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
       if (used_scraps.find(K->name) != used_scraps.end() && K->P != "") 
          page_text_scraps.insert(K->name);
    }
  }
  
  //pr. down
  
  for (map < int,set<string> >::iterator I = LEVEL.begin();
                                         I != LEVEL.end(); I++) {
    used_scraps = (*I).second;

    if (LAYOUT.transparency) {                 // transparency group beginning
      PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{%" << endl;
      PAGEDEF << "\\PL{/GS1 gs}%" << endl;     // beginning of transparency
    }

    PAGEDEF << "\\PL{q 1 g}%" << endl;         // white background of the scrap
    
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.find(K->name) != used_scraps.end() && K->I != "") {
        xc = K->I1; yc = K->I2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname("I"+(K->name)) << "}%" << endl;
      }
    }
    PAGEDEF << "\\PL{Q}%" << endl;            // end of white color for filled bg

    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.find(K->name) != used_scraps.end() && K->G != "") {
        xc = K->G1; yc = K->G2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname("G"+(K->name)) << "}%" << endl;
      };
    }

    if (LAYOUT.transparency) {
      PAGEDEF << "\\PL{/GS0 gs}%" << endl;      // end of default transparency
    }

    PAGEDEF << "\\PL{q 0 0 m " << HS << " 0 l " << HS << " " << 
                                  VS << " l 0 " << VS << " l 0 0 l}";
                               // beginning of the text clipping path definition

    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (page_text_scraps.find(K->name) != page_text_scraps.end() && 
          K->P != "" && K->level >= ((*I).first)) {
        xc = HSHIFT - K->S1; yc = VSHIFT - K->S2;
        ifstream G((K->P).c_str());
        if(!G) therror(("Can't open file"));
        while(G >> buffer) {
          if ((buffer == "m") || (buffer == "l") || (buffer == "c")) {
            PAGEDEF << "\\PL{"; 
            print_pdf_queue(thstack,xc,yc,buffer,PAGEDEF);
            PAGEDEF << "}%" << endl;
            thstack.clear();
          }
          else {
            thstack.push_back(buffer);
          }
        }
        G.close();
      };
    }
  
    PAGEDEF << "\\PL{h W n}";  // end of text clipping path definition
    
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.find(K->name) != used_scraps.end() && K->F != "") {
        xc = K->F1; yc = K->F2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname(K->name) << "}%" << endl;
      };
    }
   
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.find(K->name) != used_scraps.end() && K->E != "") {
        xc = K->E1; yc = K->E2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname("E"+(K->name)) << "}%" << endl;
      };
    }
   
    PAGEDEF << "\\PL{Q}";   // end of clipping by text

    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.find(K->name) != used_scraps.end() && K->X != "") {
        xc = K->X1; yc = K->X2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname("X"+(K->name)) << "}%" << endl;
      };
    }
    if (LAYOUT.transparency) {
      PAGEDEF << "\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
      PAGEDEF << "\\immediate\\pdfxform ";
      PAGEDEF << "attr{/Group \\the\\attrid\\space 0 R} ";
      PAGEDEF << "resources{/ExtGState \\the\\resid\\space 0 R}";
      PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;
    }
  }
  
  // pr,up
}

void build_pages() {
  
  ofstream PAGEDEF("th_pagedef.tex");
  if(!PAGEDEF) therror(("Can't write file"));
  ofstream PAGE("th_pages.tex");
  if(!PAGE) therror(("Can't write file"));

  ofstream PDFRES("th_resources.tex");
  if(!PDFRES) therror(("Can't write file"));
  if (LAYOUT.transparency) {
    PDFRES << "\\ifnum\\pdftexversion<110\\pdfcatalog{/Version /1.4}\\fi" << endl;
    PDFRES << "\\immediate\\pdfobj{ << /GS0 " <<
                 "<< /Type /ExtGState /ca 1 /BM /Normal >> " <<
           " /GS1 << /Type /ExtGState /ca \\the\\opacity\\space /BM /Normal >> >> }" << endl;
    PDFRES << "\\newcount\\resid\\resid=\\pdflastobj" << endl;
    PDFRES << "\\immediate\\pdfobj{ << /S /Transparency /K true >> }" << endl;
    PDFRES << "\\newcount\\attrid\\attrid=\\pdflastobj" << endl;
  }
  else {
    PDFRES << "\\immediate\\pdfobj{ << /GS0 " <<
                 "<< /Type /ExtGState >> " <<
           " /GS1 << /Type /ExtGState >> >> }" << endl;
    PDFRES << "\\newcount\\resid\\resid=\\pdflastobj" << endl;
  }
  PDFRES.close();

  if (mode == ATLAS) {
  }
  else {
    if (LAYOUT.map_grid) {
      MINX = LAYOUT.hsize * floor (MINX/LAYOUT.hsize);
      MINY = LAYOUT.vsize * floor (MINY/LAYOUT.vsize);
      MAXX = LAYOUT.hsize * ceil (MAXX/LAYOUT.hsize);
      MAXY = LAYOUT.vsize * ceil (MAXY/LAYOUT.vsize);
    }
    HS = MAXX - MINX;
    VS = MAXY - MINY;
    if (HS>14000 || VS>14000) 
      therror(("Map is too large for PDF format. Try smaller scale!"));
    PAGEDEF << "\\eject" << endl;
    PAGEDEF << "\\hsize=" << HS << "bp" << endl;
    PAGEDEF << "\\vsize=" << VS << "bp" << endl;
    PAGEDEF << "\\pdfpagewidth=\\hsize\\advance\\pdfpagewidth by " <<
                                             2*LAYOUT.overlap << "bp" << endl;
    PAGEDEF << "\\pdfpageheight=\\vsize\\advance\\pdfpageheight by " <<
                                             2*LAYOUT.overlap << "bp" << endl;
    PAGEDEF << "\\hoffset=0cm" << endl;
    PAGEDEF << "\\voffset=0cm" << endl;
    PAGEDEF << "\\pdfhorigin=" << LAYOUT.overlap << "bp" << endl;
    PAGEDEF << "\\pdfvorigin=" << LAYOUT.overlap << "bp" << endl;
  }
  
  if (mode == ATLAS) {

  }
  else {
    PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{%" << endl;
    if (!MAP_PREVIEW_DOWN.empty()) {
      print_preview(0,PAGEDEF,MINX,MINY);
    }
    for (map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                        I != LAYERHASH.end(); I++) {
      if (((*I).second).Z == 0) print_map((*I).first,PAGEDEF,PAGE);
    }
    if (!MAP_PREVIEW_UP.empty()) {
      print_preview(1,PAGEDEF,MINX,MINY);
    }
    if (LAYOUT.map_grid) {
      PAGEDEF << "\\PL{q .4 w}%" << endl;
      PAGEDEF << "\\PL{0 0 " << HS << " " << VS << " re S}%" << endl;
      for (double i=0; i <= HS; i += LAYOUT.hsize) {
        PAGEDEF << "\\PL{" << i << " 0 m " << i << " " << VS << " l S}%" << endl;
      }
      for (double i=0; i <= VS; i += LAYOUT.vsize) {
        PAGEDEF << "\\PL{0 " << i << " m " << HS << " " << i << " l S}%" << endl;
      }
      PAGEDEF << "\\PL{Q}%" << endl;

    }
    PAGEDEF << "\\setbox\\xxx=\\hbox{";                     // map legend
    PAGEDEF << "\\vbox to " << VS << "bp{\\maplegend\\vfill}}" << endl;
    PAGEDEF << "\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}" << endl;
    PAGEDEF << "\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
    PAGEDEF << "\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;
  }

  PAGEDEF.close();
  PAGE.close();
}


int thpdf(int m, char * wdir) {
  mode = m;

#ifdef NOTHERION
  cout << "making " << ((mode == ATLAS) ? "atlas" : "map") << "... " << flush;
#else
  thprintf("making %s... ", (mode == ATLAS) ? "atlas" : "map");
#endif

#ifdef NOTHERION
  read_settings();   // change to the quick mode only
#endif    
  
  if (mode == ATLAS) {
    cout << "(using thpdf) " << flush;
    int retcode = EXIT_SUCCESS;
    retcode = system("thpdf data.pl");
#ifndef NOTHERION
  if (retcode != EXIT_SUCCESS) {
    chdir(wdir);
    ththrow(("thpdf exit code -- %d", retcode))
  }
#endif
  }
  else {                  // map mode
    make_sheets();
    build_pages();
  }
#ifdef NOTHERION
  cout << "done." << endl;
#else
  thprintf("done.\n");
#endif
  return(0);
}

#ifdef NOTHERION
int main() {
  LAYOUT.transparency = true;
  LAYOUT.overlap = 20;
  LAYOUT.hsize = 400;
  LAYOUT.vsize = 600;
  LAYOUT.map_grid = false;
//  LAYOUT. = ;

  thpdf(1,".");
}
#endif


