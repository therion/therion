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
 
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <climits>

// #include <cstring>
#include <cstdio>
#include <cfloat>
#include <cmath>

// #ifdef NOTHERION  
// #include <unistd.h>  // I don't know why
// #endif

#include "thpdfdbg.h"
#include "thpdfdata.h"
#include "thtexfonts.h"
#include "thlang.h"
#include "thversion.h"

#ifndef NOTHERION
#include "thchenc.h"
#include "thbuffer.h"
#endif

using namespace std;



///////////

// extern list<scraprecord> SCRAPLIST;
// extern map<int,layerrecord> LAYERHASH;
// extern set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;


///////////

typedef struct {
  int id;
  int namex,namey,layer;
  bool dest,bookmark,title_before;
  map< int,set<string> > scraps;
  int jumpE,jumpW,jumpN,jumpS;
  set<int> jumpU,jumpD;
//  map< int,set<string> > preview;
//  set<string> preview;
} sheetrecord;

map<string,set<string> > preview;
map<string,list<sheetrecord>::iterator> SHEET_JMP;


bool operator < (sheetrecord a, sheetrecord b) {
  return a.id < b.id;
}

bool operator == (sheetrecord a, sheetrecord b) {
  return a.id == b.id;
}

list<sheetrecord> SHEET;


//////////

int mode;
const int ATLAS = 0, MAP = 1;
double MINX, MINY, MAXX, MAXY;
double HS,VS;
//////////

string tex_Sname(string s) {return("THS"+s);}
string tex_Nname(string s) {return("THN"+s);}
string tex_BMPname(string s) {return("THBMP"+s);} // bitmap

void read_settings() {
  ifstream F("scraps.dat");
  if(!F) therror(("Can't open file `scraps.dat'!"));
  char buf[101],ctok[100];
  string tok;
  int context = 0;   // 0=scrap, 1=layer, 2=map preview, 3=legend
  int i;
  double llx,lly,urx,ury;
  scraprecord SCR;
  layerrecord LH;
  list<scraprecord>::iterator I;
  map<int,layerrecord>::iterator J;

  while(F.getline(buf,100,'\n')) {
//    istrstream S(buf);
    istringstream S(buf);
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
      LAYERHASH.insert(make_pair(i,LH));
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

string xyz2str(int x, int y, int z) {
  char buf[50];
  sprintf(buf,"%d.%d.%d",x,y,z);
  return (string) buf;
}

list<sheetrecord>::iterator find_sheet(int x, int y, int z) {
//  list<sheetrecord>::iterator I;
//  for (I = SHEET.begin(); I != SHEET.end(); I++) {
//      if (I->layer == x && I->namex == y && I->namey == z) break;
//    }
//    return (I);
  if (SHEET_JMP.count(xyz2str(x,y,z)) > 0) 
    return SHEET_JMP.find(xyz2str(x,y,z))->second;
  else return SHEET.end();
}

void make_sheets() {
  double llx = 0, lly = 0, urx = 0, ury = 0;
  sheetrecord SHREC;

  if (mode == ATLAS) {
    for (map<int,layerrecord>::iterator I = LAYERHASH.begin(); 
                                        I != LAYERHASH.end(); I++) {
      I->second.minx = INT_MAX;
      I->second.miny = INT_MAX;
      I->second.maxx = INT_MIN;
      I->second.maxy = INT_MIN;
    }
  }
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
    if (J == LAYERHASH.end()) therror (("This can't happen!"));

    if (mode == ATLAS) {
      int Llx = (int) floor((llx-LAYOUT.overlap-LAYOUT.hoffset) / LAYOUT.hsize);
      int Lly = (int) floor((lly-LAYOUT.overlap-LAYOUT.voffset) / LAYOUT.vsize);
      int Urx = (int) floor((urx+LAYOUT.overlap-LAYOUT.hoffset) / LAYOUT.hsize);
      int Ury = (int) floor((ury+LAYOUT.overlap-LAYOUT.voffset) / LAYOUT.vsize);

      for (int i = Llx; i <= Urx; i++) {
        for (int j = Lly; j <= Ury; j++) {
          if (J->second.Z == 0) {    // Z layers don't create new sheets
            list<sheetrecord>::iterator sheet_it = find_sheet(I->layer,i,j);
            if (sheet_it == SHEET.end()) {
              sheet_it = SHEET.insert(SHEET.end(),SHREC);
              SHEET_JMP.insert(make_pair(xyz2str(I->layer,i,j),sheet_it));
            }
            sheet_it->layer = I->layer;
            sheet_it->namex = i;
            sheet_it->namey = j;

            map<int,set<string> >::iterator K = sheet_it->scraps.find(I->level);
            if (K == sheet_it->scraps.end()) {
              set<string> SCRP;
              sheet_it->scraps.insert(make_pair(I->level,SCRP));
              K = sheet_it->scraps.find(I->level);
            }
            ((*K).second).insert(I->name);
        
            if (J->second.minx > Llx) J->second.minx = Llx;
            if (J->second.miny > Lly) J->second.miny = Lly;
            if (J->second.maxx < Urx) J->second.maxx = Urx;
            if (J->second.maxy < Ury) J->second.maxy = Ury;
          } 
          // we add scrap to preview
          set<string> tmpset;
          string tmpstr;
          tmpstr = xyz2str(I->layer,i,j);
          map<string,set<string> >::iterator pr_it = preview.find(tmpstr);
          if (pr_it == preview.end()) {
            preview.insert(make_pair(tmpstr,tmpset));
            pr_it = preview.find(tmpstr);
          }
          pr_it->second.insert(I->name);
        }
      }
    }
    else {
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

void find_jumps() {
  for (list<sheetrecord>::iterator sheet_it = SHEET.begin(); 
                                   sheet_it != SHEET.end(); sheet_it++) {
    sheet_it->jumpW = 0;
    sheet_it->jumpE = 0;
    sheet_it->jumpN = 0;
    sheet_it->jumpS = 0;

    list<sheetrecord>::iterator I;
    
    int jump;    
                               
    string W = xyz2str(sheet_it->layer,sheet_it->namex-1,sheet_it->namey);
    string E = xyz2str(sheet_it->layer,sheet_it->namex+1,sheet_it->namey);
    string N = xyz2str(sheet_it->layer,sheet_it->namex,sheet_it->namey+1);
    string S = xyz2str(sheet_it->layer,sheet_it->namex,sheet_it->namey-1);
    if (SHEET_JMP.count(W) > 0) {
       I = SHEET_JMP.find(W)->second;
       sheet_it->jumpW = I->id;
       I->dest = true;
    }
    if (SHEET_JMP.count(E) > 0) {
       I = SHEET_JMP.find(E)->second;
       sheet_it->jumpE = I->id;
       I->dest = true;
    }
    if (SHEET_JMP.count(N) > 0) {
       I = SHEET_JMP.find(N)->second;
       sheet_it->jumpN = I->id;
       I->dest = true;
    }
    if (SHEET_JMP.count(S) > 0) {
       I = SHEET_JMP.find(S)->second;
       sheet_it->jumpS = I->id;
       I->dest = true;
    }

    map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
    if (lay_it == LAYERHASH.end()) therror (("This can't happen!"));

    if (!lay_it->second.U.empty()) {
      for (set<int>::iterator l_it = lay_it->second.U.begin(); 
                              l_it != lay_it->second.U.end(); l_it++) {
        map<int,layerrecord>::iterator alt_lay_it = LAYERHASH.find(*l_it);
        if (alt_lay_it == LAYERHASH.end()) therror(("This can't happen!"));
        jump = (alt_lay_it->second.Z == 0) ? *l_it : alt_lay_it->second.AltJump;
        string U = xyz2str(jump,sheet_it->namex,sheet_it->namey);
        if (SHEET_JMP.count(U) > 0) {
          I = SHEET_JMP.find(U)->second;
          sheet_it->jumpU.insert(jump);
          I->dest = true;
        }
      }
    }
    if (!lay_it->second.D.empty()) {
      for (set<int>::iterator l_it = lay_it->second.D.begin(); 
                              l_it != lay_it->second.D.end(); l_it++) {
        map<int,layerrecord>::iterator alt_lay_it = LAYERHASH.find(*l_it);
        if (alt_lay_it == LAYERHASH.end()) therror(("This can't happen!"));
        jump = (alt_lay_it->second.Z == 0) ? *l_it : alt_lay_it->second.AltJump;
        string D = xyz2str(jump,sheet_it->namex,sheet_it->namey);
        if (SHEET_JMP.count(D) > 0) {
          I = SHEET_JMP.find(D)->second;
          sheet_it->jumpD.insert(jump);
          I->dest = true;
        }
      }
    }
  }
}




string grid_name(string s, int offset) {
  unsigned char c;
  bool is_num = true;

  for (unsigned i=0; i<s.size(); i++) {
    c=s[i];
    if (!isdigit(c)) {
      is_num = false;
      break;
    }
  }
  if (is_num) {
    char buf[10];
    sprintf(buf,"%d",atoi(s.c_str())+offset);
    return (string) buf;
  }
  else if (s.size()==1) {
    c=s[0];
    if ((c >= 65 && c <= 90 && (c+offset) >= 65 && (c+offset) <= 90) ||
        (c >= 97 && c <=122 && (c+offset) >= 97 && (c+offset) <=122)) {
      char buf[10];
      sprintf(buf,"%c",c+offset);
      return (string) buf;
    }
    else return "?";
  }
  else return "?";
}

set<int> find_excluded_pages(string s) {
  set<int> excl;
  int i,j;
  char c;
//  istrstream S(s.c_str());
  istringstream S(s);
  
  while (S >> i) {
    S >> c;
    if (c == ',') excl.insert(i);
    else if (c == '-') {
      S >> j;
      for (int k=i; k<=j; k++) excl.insert(k);
      S >> c;   // punctuation character
    }
    else therror(("Invalid character in the exclusion list!"));
  }
//cout << endl;
//cout << "Excl.list: " << s << endl;  
//cout << "Excl. set: ";  
//for (set<int>::iterator I = excl.begin(); I != excl.end(); I++) 
//  cout << *I << " ";
//cout << endl;
  return excl;
}

void sort_sheets() {
  int pageid = 1 + LAYOUT.own_pages, tmppagenum = 1;
  set<int> excluded;
  bool wait_for_title;
  
  if (LAYOUT.excl_pages) excluded = find_excluded_pages(LAYOUT.excl_list);

  for (map<int,layerrecord>::reverse_iterator I = LAYERHASH.rbegin(); 
                                      I != LAYERHASH.rend(); I++) {
//    I->second.minid = pageid;
    I->second.bookmark = false;
    wait_for_title = (I->second.T !="" && LAYOUT.title_pages) ? true : false;

    for (int j = I->second.maxy; j >= I->second.miny; j--) {
      for (int i = I->second.minx; i <= I->second.maxx; i++) {
        list<sheetrecord>::iterator sheet_it = find_sheet(I->first,i,j);
        if (sheet_it != SHEET.end()) {
//          if (wait_for_title && excluded.count(tmppagenum) > 0) {
//            wait_for_title = false;
//          }
          if (excluded.count(tmppagenum) == 0) {
            sheet_it->dest = false;
            sheet_it->title_before = false;
            sheet_it->bookmark = false;
            if (!I->second.bookmark) {
              sheet_it->bookmark = true;
              sheet_it->dest = true;
              I->second.bookmark = true;
            }
            if (wait_for_title) {
              sheet_it->title_before = true;
              pageid++;
              wait_for_title = false;
            }
            sheet_it->id = pageid;
            pageid++;
          }
          else {
            SHEET.erase(sheet_it);
            SHEET_JMP.erase(xyz2str(sheet_it->layer,
                                    sheet_it->namex,sheet_it->namey));
//cout << "Should erase sheet " << tmppagenum << endl;
          }
          tmppagenum++;
        }
      }
    }
//    I->second.maxid = pageid - 1;
  }
  SHEET.sort();
//cout << "sheets: " << SHEET.size() << endl;  
}


void print_preview(int up,ofstream& PAGEDEF,double HSHIFT,double VSHIFT,
                   list<sheetrecord>::iterator sheet_it = list<sheetrecord>::iterator()) {
  set<int> used_layers;
  set<string> used_scraps;
  double xc = 0, yc = 0;
  
  if (LAYOUT.OCG) {
    if (mode == MAP)
      PAGEDEF << "\\setbox\\xxx=\\hbox to \\adjustedHS{%" << endl;
    else 
      PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{%" << endl;
  }

//  PAGEDEF << (up ? "\\PL{q .1 w}%" : "\\PL{q .8 g}%") << endl;
  
  if (up) { 
    PAGEDEF << "\\PL{q .1 w " << LAYOUT.preview_above_r << " " <<
                                 LAYOUT.preview_above_g << " " <<
                                 LAYOUT.preview_above_b << " " <<
                                                           " RG}%" << endl;
  }
  else { 
    PAGEDEF << "\\PL{q " << LAYOUT.preview_below_r << " " <<
                            LAYOUT.preview_below_g << " " <<
                            LAYOUT.preview_below_b << " " <<
                                                      " rg}%" << endl;
  }

  if (mode == ATLAS) {
    map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
    if (lay_it == LAYERHASH.end()) therror(("This can't happen!"));
    used_layers = (up ? lay_it->second.U : lay_it->second.D);
  }
  else {
    used_layers = (up ? MAP_PREVIEW_UP : MAP_PREVIEW_DOWN);
  }
  for (set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
    if (mode == ATLAS) {
        map<string,set<string> >::iterator pr_it = 
          preview.find(xyz2str(*I,sheet_it->namex,sheet_it->namey));
        if (pr_it != preview.end()) used_scraps = pr_it->second;
    }
    else {
      map<int,layerrecord>::iterator J = LAYERHASH.find(*I);
      if (J == LAYERHASH.end()) therror(("This can't happen!"));
      used_scraps = J->second.allscraps;
    }
    if (!used_scraps.empty()) {
      for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
        if (used_scraps.count(K->name) > 0) {
          if (up) {
            if (K->B != "" && K->sect == 0) {
              xc = K->B1; yc = K->B2;
              xc -= HSHIFT; yc -= VSHIFT;
              PAGEDEF << (mode == MAP && LAYOUT.OCG ? "\\PBcorr{" : "\\PB{") << 
                      xc << "}{" << yc << "}{\\" << 
                      tex_Xname("B"+(K->name)) << "}%" << endl;
            }
          }
          else {
            if (K->I != "" && K->sect == 0) {
              xc = K->I1; yc = K->I2;
              xc -= HSHIFT; yc -= VSHIFT;
              PAGEDEF << (mode == MAP && LAYOUT.OCG ? "\\PBcorr{" : "\\PB{") << 
                      xc << "}{" << yc << "}{\\" << 
                      tex_Xname("I"+(K->name)) << "}%" << endl;
            }
          }
        }
      }
    }
  }
  PAGEDEF << "\\PL{Q}%" << endl;
  if (LAYOUT.OCG) {
    if (mode==MAP)
      PAGEDEF << "\\hfill}\\ht\\xxx=\\adjustedVS\\dp\\xxx=0bp" << endl;
    else 
      PAGEDEF << "\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
    PAGEDEF << "\\immediate\\pdfxform ";
    PAGEDEF << "attr{/OC \\the\\" << (up ? "ocU" : "ocD") << "\\space 0 R} ";
    if (mode == MAP)
      PAGEDEF << "\\xxx\\PB{-\\adjustedX}{-\\adjustedY}{\\pdflastxform}%" << endl;
    else 
      PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;
  }
}



void compose_page(list<sheetrecord>::iterator sheet_it, ofstream& PAGE) {
  map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
  if (lay_it == LAYERHASH.end()) therror (("This can't happen!"));

  if (sheet_it->title_before) {
    PAGE << "\\TITLE{" << utf2tex(lay_it->second.T) << "}\n";
  }

  PAGE << "%\n% Page: " << u2str(sheet_it->id) << endl << "%\n";
  if (sheet_it->dest) PAGE << "\\pdfdest name {" << u2str(sheet_it->id) << 
                              "} xyz" << endl;

  if (sheet_it->bookmark) {
    PAGE << "\\pdfoutline goto name {" << u2str(sheet_it->id) << 
                  "} count 0 {\\ne\\376\\ne\\377" << 
                  utf2texoctal(lay_it->second.N) << "}%" << endl;
  }
  PAGE.precision(6);
  PAGE << "\\setbox\\mapbox=\\hbox to " << HS << "bp{%" << endl;
  PAGE.precision(2);
  PAGE << "\\rlap{\\pdfrefxform\\" << tex_Sname(u2str(sheet_it->id)) << 
          "}%" << endl;

  // map hyperlinks
  int lw = 25;
  double lhy = VS - 2*lw;
  double lhx = HS - 2*lw;
  if (sheet_it->jumpW) PAGE << "\\flatlink{0}{" << lw << "}{" << lw << 
          "}{" << lhy << "}{" << u2str(sheet_it->jumpW) << "}%\n";
  if (sheet_it->jumpE) PAGE << "\\flatlink{" << HS-lw << "}{" << lw <<
          "}{" << lw << "}{" << lhy << "}{" << u2str(sheet_it->jumpE) << "}%\n";
  if (sheet_it->jumpN) PAGE << "\\flatlink{" << lw << "}{" << VS-lw << 
          "}{" << lhx << "}{" << lw << "}{" << u2str(sheet_it->jumpN)<< "}%\n";
  if (sheet_it->jumpS) PAGE << "\\flatlink{" << lw << "}{0}{" << lhx << 
          "}{" << lw << "}{" << u2str(sheet_it->jumpS) << "}%\n";
  

  PAGE.precision(6);
  PAGE << "\\hfil}\\ht\\mapbox=" << VS << "bp%" << endl;
  PAGE.precision(2);

  PAGE << "\\pagelabel={" << grid_name(LAYOUT.labely,-sheet_it->namey) << 
                        " " << grid_name(LAYOUT.labelx,sheet_it->namex) <<
                        "}%" << endl;
  if (LAYOUT.page_numbering) PAGE << "\\pagenum=" << 
                             sheet_it->id << "%" << endl;
    
//    up and down links

  if (!sheet_it->jumpU.empty()) {
    PAGE << "\\pointerU={%\n";
    for (set<int>::reverse_iterator l_it = sheet_it->jumpU.rbegin();
                                    l_it != sheet_it->jumpU.rend(); l_it++) {
      list<sheetrecord>::iterator s_it = 
        find_sheet(*l_it,sheet_it->namex,sheet_it->namey);
      if (s_it == SHEET.end()) therror (("This can't happen!"));
      PAGE << utf2tex(LAYERHASH.find(*l_it)->second.N) << "|" <<
           s_it->id << "|" << u2str(s_it->id) << "||%" << endl;
    }
    PAGE << "}%\n";
  }
  else PAGE << "\\pointerU={notdef}%" << endl;

  if (!sheet_it->jumpD.empty()) {
    PAGE << "\\pointerD={%\n";
    for (set<int>::reverse_iterator l_it = sheet_it->jumpD.rbegin();
                                    l_it != sheet_it->jumpD.rend(); l_it++) {
      list<sheetrecord>::iterator s_it = 
        find_sheet(*l_it,sheet_it->namex,sheet_it->namey);
      if (s_it == SHEET.end()) therror (("This can't happen!"));
      PAGE << utf2tex(LAYERHASH.find(*l_it)->second.N) << "|" <<
           s_it->id << "|" << u2str(s_it->id) << "||%" << endl;
    }
    PAGE << "}%\n";
  }
  else PAGE << "\\pointerD={notdef}%" << endl;

  PAGE << "\\pagename={" << utf2tex(lay_it->second.N) << "}%" << endl;

  // pointers to neighbouring pages
  if (LAYOUT.page_numbering) {
    if (sheet_it->jumpW) PAGE << "\\pointerW=" << sheet_it->jumpW << "%\n";
    else PAGE << "\\pointerW=0%\n";
    if (sheet_it->jumpE) PAGE << "\\pointerE=" << sheet_it->jumpE << "%\n";
    else PAGE << "\\pointerE=0%\n";
    if (sheet_it->jumpN) PAGE << "\\pointerN=" << sheet_it->jumpN << "%\n";
    else PAGE << "\\pointerN=0%\n";
    if (sheet_it->jumpS) PAGE << "\\pointerS=" << sheet_it->jumpS << "%\n";
    else PAGE << "\\pointerS=0%\n";
  }

  PAGE << "\\setbox\\navbox=\\hbox{%" << endl;
  // navigator hyperlinks
  int nav_x = 2*LAYOUT.nav_right+1;
  int nav_y = 2*LAYOUT.nav_up+1;
  double HSN = LAYOUT.hsize / LAYOUT.nav_factor * nav_x;
  double VSN = LAYOUT.vsize / LAYOUT.nav_factor * nav_y;
  for (int i=-LAYOUT.nav_right; i <= LAYOUT.nav_right; i++) {
    for (int j=-LAYOUT.nav_up; j <= LAYOUT.nav_up; j++) {
      if (i!=0 || j!=0) {
        string tmp = xyz2str(sheet_it->layer,
                             sheet_it->namex+i,sheet_it->namey+j);
        if (SHEET_JMP.count(tmp) > 0) {
          PAGE << "\\flatlink{" << HSN*(i+LAYOUT.nav_right)/nav_x <<
            "}{" << VSN*(j+LAYOUT.nav_up)/nav_y << "}{" <<
            HSN/nav_x << "}{" << VSN/nav_y << "}{" << 
            u2str(SHEET_JMP.find(tmp)->second->id) << "}%\n";
        }
      }
    }
  }
  
  PAGE << "\\pdfrefxform\\" << tex_Nname(u2str(sheet_it->id)) << "}%" << endl;

  PAGE << "\\dopage\\eject" << endl;
}

void print_page_bg(ofstream& PAGEDEF) {
  if ((LAYOUT.background_r != 1) || 
      (LAYOUT.background_g != 1) || 
      (LAYOUT.background_b != 1)) {

    // bg rectangle
    PAGEDEF << "\\PL{q " << LAYOUT.background_r << " " << 
                            LAYOUT.background_g << " " << 
                            LAYOUT.background_b << " rg 0 0 " << 
                            HS << " " << VS << " re f Q}%" << endl;
  }
}

void print_page_bg_scraps(int layer, ofstream& PAGEDEF, 
               list<sheetrecord>::iterator sheet_it = list<sheetrecord>::iterator()) {
  // if transparency is used, all scraps should be filled white 
  // on the coloured background, just before preview_down is displayed
  // and transparency is turned on
  if (LAYOUT.transparency && 
      ((LAYOUT.background_r != 1) || 
       (LAYOUT.background_g != 1) || 
       (LAYOUT.background_b != 1))) {

    double HSHIFT=0, VSHIFT=0, xc = 0, yc = 0;
    map < int,set<string> > LEVEL;
    set <string> used_scraps;
    if (mode == ATLAS) {
      HSHIFT = LAYOUT.hsize * sheet_it->namex + LAYOUT.hoffset - LAYOUT.overlap;
      VSHIFT = LAYOUT.vsize * sheet_it->namey + LAYOUT.voffset - LAYOUT.overlap; 
      LEVEL = sheet_it->scraps;
    }
    else {
      HSHIFT = MINX;
      VSHIFT = MINY;
      LEVEL = ((*(LAYERHASH.find(layer))).second).scraps;
    }

    PAGEDEF << "\\PL{q 1 g}%" << endl;      // white background of the scrap
    for (map < int,set<string> >::iterator I = LEVEL.begin();
                                       I != LEVEL.end(); I++) {
      used_scraps = (*I).second;
      // scrap backgrounds
      for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
        if (used_scraps.count(K->name) > 0 && K->I != "") {
          xc = K->I1; yc = K->I2;
          xc -= HSHIFT; yc -= VSHIFT;
          PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                  tex_Xname("I"+(K->name)) << "}%" << endl;
        }
      }
    }
    PAGEDEF << "\\PL{Q}%" << endl;         // end of white color for filled bg
  }
}

void print_surface_bitmaps (ofstream &PAGEDEF, double shiftx, double shifty) {
  if (LAYOUT.transparency || LAYOUT.OCG) {
    if (mode == MAP)
      PAGEDEF << "\\setbox\\xxx=\\hbox to\\adjustedHS{%" << endl;
    else
      PAGEDEF << "\\setbox\\xxx=\\hbox to" << HS << "bp{%" << endl;
    PAGEDEF << "\\PL{/GS2 gs}%" << endl;
  }
  int i = 1;
  PAGEDEF.precision(6);
  for (list<surfpictrecord>::iterator I = SURFPICTLIST.begin();
                                      I != SURFPICTLIST.end(); I++) {
    if (mode == MAP && (LAYOUT.transparency || LAYOUT.OCG)) {
      PAGEDEF << "\\bitmapcorr{";
    } else {
      PAGEDEF << "\\bitmap{";
    }
    PAGEDEF << 
          I->xx << "}{" << I->yx << "}{" << I->xy << "}{" << I->yy << "}{" << 
          I->dx - shiftx << "}{" << I->dy - shifty << 
          "}{\\" << tex_BMPname(u2str(i)) << "}%" << endl;
    i++;
  };
  PAGEDEF.precision(2);
  if (LAYOUT.transparency || LAYOUT.OCG) {
    if (mode == MAP)
      PAGEDEF << "\\hfill}\\ht\\xxx=\\adjustedVS\\dp\\xxx=0bp" << endl;
    else
      PAGEDEF << "\\hfill}\\ht\\xxx="<< VS << "bp\\dp\\xxx=0bp" << endl;
    PAGEDEF << "\\immediate\\pdfxform ";
    PAGEDEF << "attr{";
    if (LAYOUT.transparency) PAGEDEF << "/Group \\the\\attrid\\space 0 R ";
    if (LAYOUT.OCG) PAGEDEF << "/OC \\the\\ocSUR\\space 0 R ";
    PAGEDEF << "} ";
    PAGEDEF << "resources{/ExtGState \\the\\resid\\space 0 R}";
    if (mode == MAP)
      PAGEDEF << "\\xxx\\PB{-\\adjustedX}{-\\adjustedY}{\\pdflastxform}%" << endl;
    else
      PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;
  }
}

paired rotatedaround(paired x,paired o, double th) {
  double theta = -th * 3.14159265358979 / 180;
  paired z, tmp;
  tmp.x = x.x - o.x;
  tmp.y = x.y - o.y;
  z.x = tmp.x * cos(theta) - tmp.y * sin(theta) + o.x;
  z.y = tmp.x * sin(theta) + tmp.y * cos(theta) + o.y;
  return z;
}


void print_grid_pdf(ofstream& PAGEDEF, double LLX, double LLY, double origMINX,double origMINY,double origMAXX, double origMAXY) {
  if (LAYOUT.grid == 0) return;
  PAGEDEF << "\\PL{q}";
  
  paired ll, ur, lr, ul, llrot, urrot, ulrot, lrrot, llnew, urnew, origin;
/*  ll.x = LLX;
  ll.y = LLY; 
  ur.x = LLX + HS;
  ur.y = LLY + VS;  */

  ll.x = origMINX;
  ll.y = origMINY; 
  ur.x = origMAXX;
  ur.y = origMAXY; 

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
  
/*  na odladenie
PAGEDEF << "\\PL{ " << llrot.x-LLX << " " << llrot.y-LLY << " m " << lrrot.x-LLX << " " << lrrot.y-LLY << " l S}";
PAGEDEF << "\\PL{ " << ulrot.x-LLX << " " << ulrot.y-LLY << " m " << urrot.x-LLX << " " << urrot.y-LLY << " l S}";
PAGEDEF << "\\PL{2 w " << ll.x-LLX << " " << ll.y-LLY << " m " << lr.x-LLX << " " << lr.y-LLY << " l S}";
PAGEDEF << "\\PL{ " << ul.x-LLX << " " << ul.y-LLY << " m " << ur.x-LLX << " " << ur.y-LLY << " l S}";
PAGEDEF << "\\PL{10 w 1 J " << llnew.x-LLX << " " << llnew.y-LLY << " m " << llnew.x-LLX << " " << llnew.y-LLY << " l S}";
PAGEDEF << "\\PL{ " << urnew.x-LLX << " " << urnew.y-LLY << " m " << urnew.x-LLX << " " << urnew.y-LLY << " l S}";
PAGEDEF << "\\PL{20 w 1 J " << origin.x-LLX << " " << origin.y-LLY << " m " << origin.x-LLX << " " << origin.y-LLY << " l S}";
PAGEDEF << "\\PL{15 w 1 J " << rotatedaround(llnew,origin,LAYOUT.gridrot).x-LLX << " " 
        << rotatedaround(llnew,origin,LAYOUT.gridrot).y-LLY << " m " << 
	   rotatedaround(llnew,origin,LAYOUT.gridrot).x-LLX << " " << 
	   rotatedaround(llnew,origin,LAYOUT.gridrot).y-LLY << " l S}";
PAGEDEF << "\\PL{ " << rotatedaround(urnew,origin,LAYOUT.gridrot).x-LLX << " " << 
       rotatedaround(urnew,origin,LAYOUT.gridrot).y-LLY << " m " << 
       rotatedaround(urnew,origin,LAYOUT.gridrot).x-LLX << " " << 
       rotatedaround(urnew,origin,LAYOUT.gridrot).y-LLY << " l S}";
*/
  
//cout << "***" << LAYOUT.hgridsize << endl;

  double grid_init_x = LAYOUT.hgridsize * floor ((llnew.x-origin.x)/LAYOUT.hgridsize) + origin.x;
  double grid_init_y = LAYOUT.vgridsize * floor ((llnew.y-origin.y)/LAYOUT.vgridsize) + origin.y;

  double G_real_init_x = LAYOUT.XO + LAYOUT.XS * floor ((llnew.x-origin.x)/LAYOUT.hgridsize);
  double G_real_init_y = LAYOUT.YO + LAYOUT.YS * floor ((llnew.y-origin.y)/LAYOUT.vgridsize);
  
  double cosr = cos(-LAYOUT.gridrot * 3.14159265 / 180);
  double sinr = sin(-LAYOUT.gridrot * 3.14159265 / 180);

  int row, col, elem;

  if (LAYOUT.proj == 0) {
    paired out,tmp;
    int ii,jj;
    double i, j;
    for (i = grid_init_x, ii=0; i < urnew.x + LAYOUT.hgridsize - 0.05; i += LAYOUT.hgridsize, ii++) {
      for (j = grid_init_y, jj=0; j < urnew.y + LAYOUT.vgridsize - 0.05; j += LAYOUT.vgridsize, jj++) {
        col = (i == grid_init_x ? 0 : (i >= urnew.x ? 2 : 1));
        row = (j == grid_init_y ? 0 : (j >= urnew.y ? 2 : 1));
	elem = col + 3*row;
/*        tmp.x = i;
        tmp.y = j;
        out = rotatedaround(tmp,origin,LAYOUT.gridrot);
        out.x -= LLX;
        out.y -= LLY;
        PAGEDEF << "\\PL{q 5 w 1 J 0 0 1 RG " << out.x << " " << out.y << " m " << out.x << " " << out.y << " l S Q}";
*/
        tmp.x = i+LAYOUT.gridcell[elem].x;
        tmp.y = j+LAYOUT.gridcell[elem].y;
        out = rotatedaround(tmp,origin,LAYOUT.gridrot);
        out.x -= LLX;
        out.y -= LLY;
        PAGEDEF << "\\PL{q}";
	PAGEDEF << "\\PL{" << cosr << " " << sinr << " " << -sinr << " " << cosr << " " << out.x << " " << out.y << " cm}";
	PAGEDEF << "\\PB{0}{0}{\\" << tex_Wname("grid") << u2str(elem+1) << "}";
        PAGEDEF << "\\PL{Q}%" << endl;

        if (LAYOUT.grid_coord_freq==2 || (LAYOUT.grid_coord_freq==1 && elem!=4)) {
          tmp.x = i;
          tmp.y = j;
          out = rotatedaround(tmp,origin,LAYOUT.gridrot);
          out.x -= LLX;
          out.y -= LLY;
          PAGEDEF << "\\PL{q}";
          PAGEDEF << "\\PL{" << cosr << " " << sinr << " " << -sinr << " " << cosr << " " << out.x << " " << out.y << " cm}";
	  PAGEDEF << "\\gridcoord{" << (row == 2 ? (col == 2 ? 1 : 3) : (col == 2 ? 7 : 9)) << 
	      "}{$(" << setprecision(0) << 
	      G_real_init_x+ii*LAYOUT.XS << "," << 
              G_real_init_y+jj*LAYOUT.YS << setprecision(2) << ")$}%" << endl;
          PAGEDEF << "\\PL{Q}";
        }

      }
    }
  }
  else {
    grid_init_x = LLX;
    int jj;
    double j;
    for (j = grid_init_y,jj=0; j < urnew.y + LAYOUT.vgridsize - 0.05; j += LAYOUT.vgridsize,jj++) {
//      PAGEDEF << "\\PL{q 3 w 0 0 1 RG 0 " << j-LLY << "  m " << HS << " " << j-LLY << " l S Q}";
      for (double i = grid_init_x; i < urnew.x + LAYOUT.hgridsize - 0.05; i += LAYOUT.hgridsize) {
        col = (i == grid_init_x ? 0 : (i >= urnew.x ? 2 : 1));
        row = (j == grid_init_y ? 0 : (j >= urnew.y ? 2 : 1));
	elem = col + 3*row;
	PAGEDEF << "\\PB{" << i-LLX+LAYOUT.gridcell[elem].x << "}{" << 
	                      j-LLY+LAYOUT.gridcell[elem].y << "}{\\" << 
			      tex_Wname("grid") << u2str(elem+1) << "}%" << endl;

        if (col == 0 && LAYOUT.grid_coord_freq > 0) {
          PAGEDEF << "\\PL{q}";
          PAGEDEF << "\\PL{1 0 0 1 " << i-LLX << " " << j-LLY << " cm}";
          PAGEDEF << "\\gridcoord{" << (row==2?3:9) << "}{$" << 
	      setprecision(0) << G_real_init_y+jj*LAYOUT.YS << 
	      setprecision(2)<< "$}";
          PAGEDEF << "\\PL{Q}%" << endl;
	}
        if (col == 2 && LAYOUT.grid_coord_freq == 2) {
          PAGEDEF << "\\PL{q}";
          PAGEDEF << "\\PL{1 0 0 1 " << i-LLX << " " << j-LLY << " cm}";
          PAGEDEF << "\\gridcoord{" << (row==2?1:7) << "}{$" << 
	      setprecision(0) << G_real_init_y+jj*LAYOUT.YS <<
	      setprecision(2)<< "$}";
          PAGEDEF << "\\PL{Q}%" << endl;
        }

      }
    }
  } 

  PAGEDEF << "\\PL{Q}%" << endl;
}



void print_map(int layer, ofstream& PAGEDEF, 
               list<sheetrecord>::iterator sheet_it = list<sheetrecord>::iterator()){
  double HSHIFT=0, VSHIFT=0, xc = 0, yc = 0;
  map < int,set<string> > LEVEL;
  set <string> page_text_scraps,used_scraps;
  string buffer;
  deque<string> thstack;

  if (mode == ATLAS) {
    HSHIFT = LAYOUT.hsize * sheet_it->namex + LAYOUT.hoffset - LAYOUT.overlap;
    VSHIFT = LAYOUT.vsize * sheet_it->namey + LAYOUT.voffset - LAYOUT.overlap; 
    LEVEL = sheet_it->scraps;
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
       if (used_scraps.count(K->name) > 0 && K->P != "") 
          page_text_scraps.insert(K->name);
    }
  }
  
  if (mode == ATLAS) {
    print_page_bg(PAGEDEF);
    if (LAYOUT.surface == 1) print_surface_bitmaps(PAGEDEF,HSHIFT,VSHIFT);
    print_page_bg_scraps(layer, PAGEDEF, sheet_it);
    if (LAYOUT.grid == 1) print_grid_pdf(PAGEDEF,HSHIFT,VSHIFT,HSHIFT,VSHIFT,HSHIFT+HS,VSHIFT+VS);
  }

  if (mode == ATLAS && !LAYERHASH.find(layer)->second.D.empty()) {
    print_preview(0,PAGEDEF,HSHIFT,VSHIFT,sheet_it);
  }
  
  for (map < int,set<string> >::iterator I = LEVEL.begin();
                                         I != LEVEL.end(); I++) {
    used_scraps = (*I).second;

    if (LAYOUT.transparency) {                 // transparency group beginning
      PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{%" << endl;
      PAGEDEF << "\\PL{/GS1 gs}%" << endl;     // beginning of transparency
    }

//    PAGEDEF << "\\PL{q 1 g}%" << endl;         // white background of the scrap
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->I != "") {
        PAGEDEF << "\\PL{q ";
        if (K->r < 0 || K->g < 0 || K->b < 0) {
          PAGEDEF << LAYOUT.foreground_r << " " <<   // background of the scrap
                     LAYOUT.foreground_g << " " << 
                     LAYOUT.foreground_b << " rg}%" << endl;
        }
        else {
          PAGEDEF << K->r << " " <<   // background of the scrap
                     K->g << " " << 
                     K->b << " rg}%" << endl;
        }
        xc = K->I1; yc = K->I2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname("I"+(K->name)) << "}%" << endl;

        PAGEDEF << "\\PL{Q}%" << endl;            // end of white color for filled bg
      }
    }

    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->G != "") {
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
      if (page_text_scraps.count(K->name) > 0 && 
          K->P != "" && K->level >= (I->first)) {
        xc = HSHIFT - K->S1; yc = VSHIFT - K->S2;
        ifstream G((K->P).c_str());
        if(!G) therror(("Can't open file"));
        while(G >> buffer) {
          if ((buffer == "m") || (buffer == "l") || (buffer == "c")) {
            PAGEDEF << "\\PL{"; 
            for(unsigned i=0; i<thstack.size(); i=i+2) {
              PAGEDEF << atof(thstack[i].c_str())-xc << " " << 
                         atof(thstack[i+1].c_str())-yc << " ";
            }
            PAGEDEF << buffer << "}%" << endl;
            thstack.clear();
          }
          else {
            thstack.push_back(buffer);
          }
        }
        G.close();
        if (!thstack.empty()) therror(("This can't happen -- bad text clipping path!"));
      };
    }
  
    PAGEDEF << "\\PL{h W n}";  // end of text clipping path definition
    
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->F != "") {
        xc = K->F1; yc = K->F2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname(K->name) << "}%" << endl;
      };
    }
   
    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->E != "") {
        xc = K->E1; yc = K->E2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                tex_Xname("E"+(K->name)) << "}%" << endl;
      };
    }
   
    PAGEDEF << "\\PL{Q}";   // end of clipping by text

    for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->X != "") {
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
  
  if (mode == ATLAS && !LAYERHASH.find(layer)->second.U.empty()) {
    print_preview(1,PAGEDEF,HSHIFT,VSHIFT,sheet_it);
  }

  if (mode == ATLAS) {
    if (LAYOUT.surface == 2) print_surface_bitmaps(PAGEDEF,HSHIFT,VSHIFT);
    if (LAYOUT.grid == 2) print_grid_pdf(PAGEDEF,HSHIFT,VSHIFT,HSHIFT,VSHIFT,HSHIFT+HS,VSHIFT+VS);
  }
}

void print_navigator(ofstream& P, list<sheetrecord>::iterator sheet_it) {
  set<string> NAV_SCRAPS;
  set<int> used_layers;
  set<string> used_scraps;
  int nav_x = 2*LAYOUT.nav_right+1;
  int nav_y = 2*LAYOUT.nav_up+1;
  double HSN = LAYOUT.hsize / LAYOUT.nav_factor * nav_x;
  double VSN = LAYOUT.vsize / LAYOUT.nav_factor * nav_y;
  double xc = 0, yc = 0;

  P << "%\n\\setbox\\xxx=\\hbox to " << HSN << "bp{%\n\\PL{q ";
  P.precision(6);
  P << 1/LAYOUT.nav_factor << " 0 0 " << 1/LAYOUT.nav_factor << " 0 0 cm}%\n";
  P.precision(2);

  map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
  if (lay_it == LAYERHASH.end()) therror (("This can't happen!"));

  NAV_SCRAPS.clear();
  if (!lay_it->second.D.empty()) {
    P << "\\PL{.8 g}%\n";
    used_layers = lay_it->second.D;
    for (set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
      for (int i = sheet_it->namex-LAYOUT.nav_right; 
               i <= sheet_it->namex+LAYOUT.nav_right; i++) {
        for (int j = sheet_it->namey-LAYOUT.nav_up; 
                 j <= sheet_it->namey+LAYOUT.nav_up; j++) {
          used_scraps.clear();
          map<string,set<string> >::iterator pr_it = 
            preview.find(xyz2str(*I,i,j));
          if (pr_it != preview.end()) used_scraps = pr_it->second;
          if (!used_scraps.empty()) {
            for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
              if (used_scraps.count(K->name) > 0 && 
                           NAV_SCRAPS.count(K->name) == 0 &&
                           K->I != "" && K->sect == 0) {
                xc = K->I1; yc = K->I2;
                xc -= LAYOUT.hsize * (sheet_it->namex - LAYOUT.nav_right) + 
                      LAYOUT.hoffset; 
                yc -= LAYOUT.vsize * (sheet_it->namey - LAYOUT.nav_up) + 
                      LAYOUT.voffset; 
                P << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                        tex_Xname("I"+(K->name)) << "}%" << endl;
                NAV_SCRAPS.insert(K->name);
              }
            }
          }
        }
      }
    }
  }
  NAV_SCRAPS.clear();
  P << "\\PL{0 g}%\n";
  for (int i = sheet_it->namex-LAYOUT.nav_right; 
           i <= sheet_it->namex+LAYOUT.nav_right; i++) {
    for (int j = sheet_it->namey-LAYOUT.nav_up; 
             j <= sheet_it->namey+LAYOUT.nav_up; j++) {
      used_scraps.clear();
      map<string,set<string> >::iterator pr_it = 
        preview.find(xyz2str(sheet_it->layer,i,j));
      if (pr_it != preview.end()) used_scraps = pr_it->second;
      if (!used_scraps.empty()) {
        for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
          if (used_scraps.count(K->name) > 0 && 
                       NAV_SCRAPS.count(K->name) == 0 &&
                       K->I != "" && K->sect == 0) {
            xc = K->I1; yc = K->I2;
            xc -= LAYOUT.hsize * (sheet_it->namex - LAYOUT.nav_right) + 
                  LAYOUT.hoffset; 
            yc -= LAYOUT.vsize * (sheet_it->namey - LAYOUT.nav_up) + 
                  LAYOUT.voffset; 
            P << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                    tex_Xname("I"+(K->name)) << "}%" << endl;
            NAV_SCRAPS.insert(K->name);
          }
        }
      }
    }
  }
  NAV_SCRAPS.clear();
  if (!lay_it->second.U.empty()) {
    P << "\\PL{0.2 w}%\n";
    used_layers = lay_it->second.U;
    for (set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
      for (int i = sheet_it->namex-LAYOUT.nav_right; 
               i <= sheet_it->namex+LAYOUT.nav_right; i++) {
        for (int j = sheet_it->namey-LAYOUT.nav_up; 
                 j <= sheet_it->namey+LAYOUT.nav_up; j++) {
          used_scraps.clear();
          map<string,set<string> >::iterator pr_it = 
            preview.find(xyz2str(*I,i,j));
          if (pr_it != preview.end()) used_scraps = pr_it->second;
          if (!used_scraps.empty()) {
            for (list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
              if (used_scraps.count(K->name) > 0 && 
                           NAV_SCRAPS.count(K->name) == 0 &&
                           K->B != "" && K->sect == 0) {
                xc = K->B1; yc = K->B2;
                xc -= LAYOUT.hsize * (sheet_it->namex - LAYOUT.nav_right) + 
                      LAYOUT.hoffset; 
                yc -= LAYOUT.vsize * (sheet_it->namey - LAYOUT.nav_up) + 
                      LAYOUT.voffset; 
                P << "\\PB{" << xc << "}{" << yc << "}{\\" << 
                        tex_Xname("B"+(K->name)) << "}%" << endl;
                NAV_SCRAPS.insert(K->name);
              }
            }
          }
        }
      }
    }
  }

  // navigator grid
  P << "\\PL{Q}" << "\\PL{0 0 " << HSN << " " << VSN << " re S 0.1 w}";
  for (int i = 1; i < nav_x; i++)      
    P << "\\PL{" << HSN*i/nav_x << " 0 m " << HSN*i/nav_x << " " << VSN << " l S}%\n";
  for (int i = 1; i < nav_y; i++)
    P << "\\PL{0 " << VSN*i/nav_y << " m " << HSN << " " << VSN*i/nav_y << " l S}%\n";
  P << "\\PL{0.4 w " <<
    HSN*LAYOUT.nav_right/nav_x << " " << VSN*LAYOUT.nav_up/nav_y << " " <<
    HSN/nav_x << " " << VSN/nav_y << " " << " re S}";
  // XObject definition
  P << "\\hfill}\\ht\\xxx=" << VSN << "bp\\dp\\xxx=0bp\n";
  P << "\\immediate\\pdfxform\\xxx\\newcount\\" << 
       tex_Nname(u2str(sheet_it->id)) << " \\" <<
       tex_Nname(u2str(sheet_it->id)) << "=\\pdflastxform" << endl;
}


void print_margins(ofstream& PAGEDEF) {
  PAGEDEF << "\\PL{q}";
//  PAGEDEF << "\\PL{3 w 0 0 " << HS << " " << VS << " re S}";
  if (LAYOUT.overlap > 0) {
    double i = LAYOUT.hsize + LAYOUT.overlap; 
    double j = LAYOUT.vsize + LAYOUT.overlap;
    PAGEDEF << "\\PL{0.5 w}";
    PAGEDEF << "\\PL{0 " << LAYOUT.overlap << " m " << HS << " " << 
                            LAYOUT.overlap << " l S}";
    PAGEDEF << "\\PL{0 " << j << " m " << HS << " " << j << " l S}";
    PAGEDEF << "\\PL{" << LAYOUT.overlap << " 0 m " << LAYOUT.overlap << 
               " " << VS << " l S}";
    PAGEDEF << "\\PL{" << i << " 0 m " << i << " " << VS << " l S}";
  }
  PAGEDEF << "\\PL{Q}%" << endl;
}



void build_pages() {
  
  ofstream PAGEDEF("th_pagedef.tex");
  if(!PAGEDEF) therror(("Can't write file"));
  PAGEDEF.setf(ios::fixed, ios::floatfield);
  PAGEDEF.precision(2);

  ofstream PAGE("th_pages.tex");
  if(!PAGE) therror(("Can't write file"));
  PAGE.setf(ios::fixed, ios::floatfield);
  PAGE.precision(2);

  ofstream PDFRES("th_resources.tex");
  if(!PDFRES) therror(("Can't write file"));
  if (LAYOUT.transparency || LAYOUT.OCG) {
    PDFRES << "\\ifnum\\pdftexversion<110\\pdfcatalog{ /Version /" <<
      (LAYOUT.OCG ? "1.5" : "1.4") << " }" << 
      (LAYOUT.OCG ? "\\else\\pdfoptionpdfminorversion=5" : "") << "\\fi" << endl;
  }
  
  PDFRES << "\\pdfinfo{/Creator (Therion " << THVERSION << ", MetaPost, TeX)}%" << endl;
  PDFRES << "\\pdfcatalog{ /ViewerPreferences << /DisplayDocTitle true /PrintScaling /None >> }" << endl;
  

  if (LAYOUT.transparency) {
    PDFRES << "\\opacity{" << LAYOUT.opacity << "}%" << endl;
    PDFRES << "\\surfaceopacity{" << LAYOUT.surface_opacity << "}%" << endl;
    PDFRES << "\\immediate\\pdfobj{ << /GS0 " <<
                 "<< /Type /ExtGState /ca 1 /BM /Normal >> " <<
           " /GS1 << /Type /ExtGState /ca \\the\\opacity\\space /BM /Normal >> " <<
           " /GS2 << /Type /ExtGState /ca \\the\\surfaceopacity\\space /BM /Normal >> >> }" << endl;
    PDFRES << "\\newcount\\resid\\resid=\\pdflastobj" << endl;
    PDFRES << "\\immediate\\pdfobj{ << /S /Transparency /K true >> }" << endl;
    PDFRES << "\\newcount\\attrid\\attrid=\\pdflastobj" << endl;
  }
  else {
    PDFRES << "\\immediate\\pdfobj{ << /GS0 " <<
                 "<< /Type /ExtGState >> " <<
           " /GS1 << /Type /ExtGState >> " <<
           " /GS2 << /Type /ExtGState >> >> }" << endl;
    PDFRES << "\\newcount\\resid\\resid=\\pdflastobj" << endl;
  }

  if (LAYOUT.OCG) {
    PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" << 
      utf2texhex(string(thT("title preview above",LAYOUT.lang))) << 
      "> >> }" << endl;
    PDFRES << "\\newcount\\ocU\\ocU=\\pdflastobj" << endl;
    PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" <<
      utf2texhex(string(thT("title preview below",LAYOUT.lang))) << 
      "> >> }" << endl;
    PDFRES << "\\newcount\\ocD\\ocD=\\pdflastobj" << endl;
    PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" <<
      utf2texhex(string(thT("title surface bitmap",LAYOUT.lang))) << 
      "> >> }" << endl;
    PDFRES << "\\newcount\\ocSUR\\ocSUR=\\pdflastobj" << endl;
    if (mode == MAP) {
      for (map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                          I != LAYERHASH.end(); I++) {
        if (I->second.Z == 0) {
          PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" <<
            utf2texhex(I->second.N) << "> >> }" << endl;
          PDFRES << "\\newcount\\oc" << u2str(I->first) << "\\oc" << 
                     u2str(I->first) << "=\\pdflastobj" << endl;
        }
      }
    }
    PDFRES << "\\pdfcatalog{ /OCProperties <<" << endl <<
              "  /OCGs [";
    if (LAYOUT.surface == 2) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "\\the\\ocU\\space0 R ";
    if (mode == MAP) {
      for (map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                          I != LAYERHASH.end(); I++) {
        if (I->second.Z == 0) 
          PDFRES << "\\the\\oc" << u2str(I->first) << "\\space 0 R ";
      }
    }
    PDFRES << "\\the\\ocD\\space0 R ";
    if (LAYOUT.surface == 1) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "]" << endl <<
              "  /D << /Name (Map layers) /ListMode /VisiblePages" << 
                     " /Order [";
    if (LAYOUT.surface == 2) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "\\the\\ocU\\space0 R ";
    if (mode == MAP) {
      for (map<int,layerrecord>::reverse_iterator I = LAYERHASH.rbegin();
                                          I != LAYERHASH.rend(); I++) {
        if (I->second.Z == 0) 
          PDFRES << "\\the\\oc" << u2str(I->first) << "\\space 0 R ";
      }
    }
    PDFRES << "\\the\\ocD\\space0 R ";
    if (LAYOUT.surface == 1) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "] >>" << endl << ">> }" << endl;
  }

  if (LAYOUT.doc_author != "") 
    PDFRES << "\\pdfinfo{ /Author <feff" << utf2texhex(LAYOUT.doc_author) << ">}" << endl;
  if (LAYOUT.doc_subject != "") 
    PDFRES << "\\pdfinfo{ /Subject <feff" << utf2texhex(LAYOUT.doc_subject) << ">}" << endl;
  if (LAYOUT.doc_keywords != "") 
    PDFRES << "\\pdfinfo{ /Keywords <feff" << utf2texhex(LAYOUT.doc_keywords) << ">}" << endl;
  if (LAYOUT.doc_title != "") {
    PDFRES << "\\pdfinfo{ /Title <feff" << utf2texhex(LAYOUT.doc_title) << ">}" << endl;
//    PDFRES << "\\legendcavename={" << utf2tex(LAYOUT.doc_title) << "}" << endl;
  }
//  if (LAYOUT.doc_comment != "") {
//    PDFRES << "\\legendcomment={" << utf2tex(LAYOUT.doc_comment) << "}" << endl;
//  }

  if (!LEGENDLIST.empty()) {  // zmenit test na LAYOUT.legend???
    PDFRES << "\\legendtrue" << endl;
  }
  else {
    PDFRES << "\\legendfalse" << endl;
  }
  
  if (!COLORLEGENDLIST.empty()) {  
    PDFRES << "\\colorlegendtrue" << endl;
  }
  else {
    PDFRES << "\\colorlegendfalse" << endl;
  }
  
  PDFRES << "\\legendwidth=" << LAYOUT.legend_width << "bp" << endl;

  if (LAYOUT.map_header_bg) {
    PDFRES << "\\bgcolor={" << LAYOUT.background_r << " " << 
                               LAYOUT.background_g << " " <<
                               LAYOUT.background_b << "}" << endl;
    PDFRES << "\\legendbgfilltrue" << endl;
  } 
  else PDFRES << "\\legendbgfillfalse" << endl;

  PDFRES.close();

  // jednorazove vlozenie povrchovych obrazkov
  int i = 1;
  for (list<surfpictrecord>::iterator I = SURFPICTLIST.begin();
                                      I != SURFPICTLIST.end(); I++) {
    PAGEDEF << "\\pdfximage{" << (string) I->filename << "}%" << endl;
    PAGEDEF << "\\newcount\\" << tex_BMPname(u2str(i)) << "\\" <<
               tex_BMPname(u2str(i)) << "=\\pdflastximage%" << endl;
    i++;
  }

  double origMINX=0, origMINY=0, origMAXX=0, origMAXY=0;

  if (mode == ATLAS) {
    HS = LAYOUT.hsize + 2*LAYOUT.overlap;
    VS = LAYOUT.vsize + 2*LAYOUT.overlap;
    if (LAYOUT.page_numbering) {
      PAGE << "\\pagenumberingtrue" << endl;
    }
  }
  else {
    if (LAYOUT.proj > 0 && LAYOUT.grid > 0) {  // natiahnutie vysky aby sa zobrazil grid pod aj nad jaskynou
      MINY = LAYOUT.vgridsize * floor ((MINY-LAYOUT.vgridorigin)/LAYOUT.vgridsize) + LAYOUT.vgridorigin;
      MAXY = LAYOUT.vgridsize * ceil  ((MAXY-LAYOUT.vgridorigin)/LAYOUT.vgridsize) + LAYOUT.vgridorigin;
    }
    origMINX = MINX; origMINY = MINY; origMAXX = MAXX; origMAXY = MAXY;
    if (LAYOUT.map_grid) {
//      origMINX = MINX; origMINY = MINY; origMAXX = MAXX; origMAXY = MAXY;
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
    PAGEDEF << "\\newdimen\\overlap\\overlap=" << LAYOUT.overlap << "bp" << endl;
    for (list<sheetrecord>::iterator I = SHEET.begin(); 
                                     I != SHEET.end(); I++) {

//      cout << "ID: " << I->id << " Layer: " << I->layer << " X: " << 
//           I->namex << " Y: " << I->namey << endl;

// cout << "*" << flush; 

      PAGEDEF << "\\setbox\\xxx=\\hbox to "<< HS << "bp{%" << endl;

      print_map(I->layer, PAGEDEF, I);
      print_margins(PAGEDEF);

      PAGEDEF << "\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
      PAGEDEF << "\\immediate\\pdfxform";
      PAGEDEF << "\\xxx\\newcount\\" << tex_Sname(u2str(I->id)) <<
             " \\" << tex_Sname(u2str(I->id)) << "=\\pdflastxform" << endl;

      print_navigator(PAGEDEF,I);
      compose_page(I, PAGE);

    }
  }
  else {
    PAGEDEF << "\\newdimen\\x \\x=" << HS << "bp" << endl;
    PAGEDEF << "\\newdimen\\y \\y=" << VS << "bp" << endl;
    PAGEDEF << "\\setbox\\xxx=\\hbox to \\x{\\maplayout\\hfill}%\\dp\\xxx=0bp" << endl;
    PAGEDEF << "\\advance\\x by \\extraE" << endl;
    PAGEDEF << "\\advance\\x by \\extraW" << endl;
    PAGEDEF << "\\advance\\y by \\extraN" << endl;
    PAGEDEF << "\\advance\\y by \\extraS" << endl;
    PAGEDEF << "\\newbox\\xxxx\\setbox\\xxxx=\\hbox to \\x{\\kern\\extraW\\raise\\extraS\\box\\xxx\\hss}%\\dp\\xxx=0bp" << endl;
    PAGEDEF << "\\wd\\xxxx=\\x" << endl;
    PAGEDEF << "\\ht\\xxxx=\\y" << endl;
    PAGEDEF << "\\immediate\\pdfxform\\xxxx" << endl;
    PAGEDEF << "\\newcount\\THmaplegend\\THmaplegend=\\pdflastxform" << endl;

    PAGEDEF << "\\advance\\pdfhorigin by \\extraW" << endl;
    PAGEDEF << "\\advance\\pdfvorigin by \\extraN" << endl;
    PAGEDEF << "\\advance\\pdfpagewidth by \\extraW" << endl;
    PAGEDEF << "\\advance\\pdfpagewidth by \\extraE" << endl;
    PAGEDEF << "\\advance\\pdfpageheight by \\extraN" << endl;
    PAGEDEF << "\\advance\\pdfpageheight by \\extraS" << endl;

    PAGEDEF << "\\newdimen\\overlap\\overlap=" << LAYOUT.overlap << "bp" << endl;

    PAGEDEF << "\\dimtobp{\\the\\pdfpagewidth}\\edef\\xsize{\\tmpdef}%" << endl;
    PAGEDEF << "\\dimtobp{\\the\\pdfpageheight}\\edef\\ysize{\\tmpdef}%" << endl;
    PAGEDEF << "\\advance\\y by -\\extraN\\advance\\y by \\overlap" << endl;
    PAGEDEF << "\\dimtobp{\\the\\y}\\edef\\nsize{\\tmpdef}%" << endl;
    PAGEDEF << "\\x=\\extraW\\advance\\x by \\overlap" << endl;
    PAGEDEF << "\\dimtobp{\\the\\x}\\edef\\wsize{\\tmpdef}%" << endl;

    PAGEDEF << "\\newdimen\\overlaphalf\\overlaphalf=\\overlap\\divide\\overlaphalf by 2%" << endl;
    PAGEDEF << "\\newdimen\\framew\\framew=\\pdfpagewidth\\advance\\framew by -\\overlap" << endl;
    PAGEDEF << "\\newdimen\\frameh\\frameh=\\pdfpageheight\\advance\\frameh by -\\overlap" << endl;
    PAGEDEF << "\\newdimen\\framex\\framex=\\extraW\\advance\\framex by \\overlaphalf" << endl;
    PAGEDEF << "\\newdimen\\framey\\framey=\\extraN\\advance\\framey by \\overlaphalf" << endl;

    PAGEDEF << "\\dimtobp{\\framew}\\edef\\Framew{\\tmpdef}%" << endl;
    PAGEDEF << "\\dimtobp{\\frameh}\\edef\\Frameh{\\tmpdef}%" << endl;
    PAGEDEF << "\\dimtobp{\\framex}\\edef\\Framex{\\tmpdef}%" << endl;
    PAGEDEF << "\\dimtobp{\\framey}\\edef\\Framey{\\tmpdef}%" << endl;


    PAGEDEF << "\\adjustedHS=" << HS << "bp" <<
      "\\advance\\adjustedHS by \\extraE" << 
      "\\advance\\adjustedHS by \\extraW" <<
      "\\advance\\adjustedHS by \\overlap" << 
      "\\advance\\adjustedHS by \\overlap" <<  endl;

    PAGEDEF << "\\adjustedVS=" << VS << "bp" << 
      "\\advance\\adjustedVS by \\extraN" << 
      "\\advance\\adjustedVS by \\extraS" << 
      "\\advance\\adjustedVS by \\overlap" << 
      "\\advance\\adjustedVS by \\overlap" << endl;

    PAGEDEF << "\\tmpdimen=\\extraW\\advance\\tmpdimen by \\overlap" << endl;
    PAGEDEF << "\\dimtobp{\\tmpdimen}\\edef\\adjustedX{\\tmpdef}%" << endl;
    PAGEDEF << "\\tmpdimen=\\extraS\\advance\\tmpdimen by \\overlap" << endl;
    PAGEDEF << "\\dimtobp{\\tmpdimen}\\edef\\adjustedY{\\tmpdef}%" << endl;

    PAGEDEF << "\\PL{q " << LAYOUT.background_r << " " << 
                            LAYOUT.background_g << " " << 
                            LAYOUT.background_b << " rg -" << 
			    "\\wsize\\space"  << "-" << 
			    "\\nsize\\space" << 
			    "\\xsize\\space" << 
			    "\\ysize\\space" << 
                            " re f Q}%" << endl;

    PAGEDEF << "\\ifdim\\framethickness>0mm\\dimtobp{\\framethickness}\\edef\\Framethickness{\\tmpdef}" <<
               "\\PL{q 0 0 0 RG 1 J 1 j \\Framethickness\\space w " << 
               "-\\Framex\\space\\Framey\\space\\Framew\\space-\\Frameh\\space" << 
               " re s Q}\\fi" << endl;


//    PAGEDEF << "\\leavevmode\\setbox\\xxx=\\hbox to " << HS << "bp{%" << endl;
    PAGEDEF << "\\leavevmode\\setbox\\xxx=\\hbox to 0bp{%" << endl;

    if (LAYOUT.surface == 1) print_surface_bitmaps(PAGEDEF,MINX,MINY);

//    print_page_bg(PAGEDEF);
    for (map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                        I != LAYERHASH.end(); I++) {
      if (I->second.Z == 0) {
//        if (LAYOUT.OCG) {
//          PAGEDEF << "\\PL{/OC /oc\\the\\oc" << u2str(I->first) << "\\space BDC}%" << endl;
//        }
        print_page_bg_scraps(I->first,PAGEDEF);
//        if (LAYOUT.OCG) {
//          PAGEDEF << "\\PL{EMC}%" << endl;
//        }
      }
    }

    if (LAYOUT.grid == 1) print_grid_pdf(PAGEDEF,MINX,MINY,origMINX,origMINY,origMAXX,origMAXY);

    if (!MAP_PREVIEW_DOWN.empty()) print_preview(0,PAGEDEF,MINX,MINY);
    for (map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                        I != LAYERHASH.end(); I++) {
      if (I->second.Z == 0) {
        PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{%" << endl;
                            // we need flush layer data using XObject 
                            // (the text clipping path may become too large)

        print_map((*I).first,PAGEDEF);

        PAGEDEF << "\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
        PAGEDEF << "\\immediate\\pdfxform ";
        if (LAYOUT.OCG) {
          PAGEDEF << "attr{/OC \\the\\oc" << u2str(I->first) << "\\space 0 R} ";
        }
        PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;
      }
    }
    if (!MAP_PREVIEW_UP.empty()) print_preview(1,PAGEDEF,MINX,MINY);

    if (LAYOUT.surface == 2) print_surface_bitmaps(PAGEDEF,MINX,MINY);
    if (LAYOUT.grid == 2) print_grid_pdf(PAGEDEF,MINX,MINY,origMINX,origMINY,origMAXX,origMAXY);

    if (LAYOUT.map_grid) {
      PAGEDEF << "\\PL{q .4 w 0.6 g 0.6 G }%" << endl;
      PAGEDEF << "\\PL{0 0 " << HS << " " << VS << " re S}%" << endl;
      for (double i=0; i <= HS; i += LAYOUT.hsize) {
        PAGEDEF << "\\PL{" << i << " 0 m " << i << " " << VS << " l S}%" << endl;
	if (i<HS) {
          PAGEDEF << "\\PL{q 1 0 0 1 " << i+LAYOUT.hsize/2 << 
	    " 0 cm}\\gridcoord{8}{\\size[24]" << grid_name(LAYOUT.labelx,(MINX+i)/LAYOUT.hsize) << 
	    "}\\PL{Q}%" << endl;
          PAGEDEF << "\\PL{q 1 0 0 1 " << i+LAYOUT.hsize/2 << " " << VS <<
	    " cm}\\gridcoord{2}{\\size[24]" << grid_name(LAYOUT.labelx,(MINX+i)/LAYOUT.hsize) << 
	    "}\\PL{Q}%" << endl;
	}
      }
      for (double i=0; i <= VS; i += LAYOUT.vsize) {
        PAGEDEF << "\\PL{0 " << i << " m " << HS << " " << i << " l S}%" << endl;
	if (i<VS) {
          PAGEDEF << "\\PL{q 1 0 0 1 0 " << i+LAYOUT.vsize/2 << 
    	    " cm}\\gridcoord{6}{\\size[24]" << grid_name(LAYOUT.labely,(MINY+VS-i)/LAYOUT.vsize) << 
	    "}\\PL{Q}%" << endl;
          PAGEDEF << "\\PL{q 1 0 0 1 " << HS << " " << i+LAYOUT.vsize/2 << 
	    " cm}\\gridcoord{4}{\\size[24]" << grid_name(LAYOUT.labely,(MINY+VS-i)/LAYOUT.vsize) << 
	    "}\\PL{Q}%" << endl;
	}
      }
      PAGEDEF << "\\PL{Q}%" << endl;
    }
////    PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{";      // map legend
////    PAGEDEF << "\\maplayout\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
////    PAGEDEF << "\\immediate\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;

    PAGEDEF << "\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << endl;
//    PAGEDEF << "\\immediate\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}%" << endl;
    PAGEDEF << "\\box\\xxx%" << endl;
    
    PAGEDEF << "\\smash{\\rlap{\\kern-\\extraW\\raise-\\extraS" << 
               "\\hbox{\\pdfrefxform\\THmaplegend}}}" << endl;
  }

  PAGEDEF.close();
  PAGE.close();
}


int thpdf(int m) {
  mode = m;

#ifdef NOTHERION
  init_encodings();
  print_fonts_setup();
  cout << "making " << ((mode == ATLAS) ? "atlas" : "map") << " ... " << flush;
#else
  thprintf("making %s ... ", (mode == ATLAS) ? "atlas" : "map");
#endif

  SHEET.clear();
  SHEET_JMP.clear();
  MINX=DBL_MAX, MINY=DBL_MAX, MAXX=-DBL_MAX, MAXY=-DBL_MAX;
  
#ifdef NOTHERION
  read_settings();   // change to the quick mode only
#endif    

  make_sheets();
  if (mode == ATLAS) {
    sort_sheets();
    find_jumps();
  }
  build_pages();

#ifdef NOTHERION
  cout << "done" << endl;
#else
  thprintf("done\n");
#endif
  return(0);
}

#ifdef NOTHERION
int main() {
  thpdf(0);
}
#endif


