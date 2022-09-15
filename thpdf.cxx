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
#include <regex>
#include <fmt/core.h>

#include "thpdfdbg.h"
#include "thconfig.h"
#include "thpdfdata.h"
#include "thtexfonts.h"
#include "thlang.h"
#include "thversion.h"
#include "thdouble.h"

#ifdef THMSVC
#define round(x) floor((x) + 0.5)
#endif

#include "thchenc.h"
#include "thbuffer.h"

#define IOerr(F) ((std::string)"Can't open file "+F+"!\n").c_str()



///////////

// extern list<scraprecord> SCRAPLIST;
// extern map<int,layerrecord> LAYERHASH;
// extern set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;


///////////

const int prec_matrix = 6;
const int prec_xy = 2;

typedef struct {
  int id;
  int namex,namey,layer;
  bool dest,bookmark,title_before;
  std::map< int,std::set<std::string> > scraps;
  int jumpE,jumpW,jumpN,jumpS;
  std::set<int> jumpU,jumpD;
//  map< int,set<string> > preview;
//  set<string> preview;
} sheetrecord;

std::map<std::string,std::set<std::string> > preview;
std::map<std::string,std::list<sheetrecord>::iterator> SHEET_JMP;


bool operator < (const sheetrecord& a, const sheetrecord& b) {
  return a.id < b.id;
}

bool operator == (const sheetrecord& a, const sheetrecord& b) {
  return a.id == b.id;
}

std::list<sheetrecord> SHEET;


//////////

int mode;
const int ATLAS = 0, MAP = 1;
double MINX, MINY, MAXX, MAXY;
double HS,VS;
//////////

std::string black2pdf(double shade, fillstroke fs) {    // shade: 0 = white, 1 = black
  if (shade < 0 || shade > 1) therror((fmt::format("shade {} out of range <0,1>",shade).c_str()));
  color c;
  if (LAYOUT.output_colormodel == colormodel::grey) c.set(1-shade);
  else if (LAYOUT.output_colormodel == colormodel::rgb) c.set(1-shade, 1-shade, 1-shade);
  else if (LAYOUT.output_colormodel == colormodel::cmyk) c.set(0, 0, 0, shade);
  else therror (("invalid color model"));
  return c.to_pdfliteral(fs);
}

std::string tex_Sname(std::string s) {return("THS"+s);}
std::string tex_Nname(std::string s) {return("THN"+s);}
std::string tex_BMPname(std::string s) {return("THBMP"+s);} // bitmap

std::string xyz2str(int x, int y, int z) {
  return fmt::format("{}.{}.{}",x,y,z);
}

std::list<sheetrecord>::iterator find_sheet(int x, int y, int z) {
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
  double a,b,w,h;
  sheetrecord SHREC;

  if (mode == ATLAS) {
    for (std::map<int,layerrecord>::iterator I = LAYERHASH.begin(); 
                                        I != LAYERHASH.end(); I++) {
      I->second.minx = INT_MAX;
      I->second.miny = INT_MAX;
      I->second.maxx = INT_MIN;
      I->second.maxy = INT_MIN;
    }
  }
  for (std::list<scraprecord>::iterator I = SCRAPLIST.begin(); 
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
    for (std::list<surfpictrecord>::iterator I_sk = I->SKETCHLIST.begin();
                                          I_sk != I->SKETCHLIST.end(); I_sk++) {
      for (int i = 0; i<=1; i++) {
        for (int j = 0; j<=1; j++) {
          w = i * I_sk->width;
          h = j * I_sk->height;
          a = I_sk->xx*w + I_sk->xy*h + I_sk->dx;
          b = I_sk->yx*w + I_sk->yy*h + I_sk->dy;
          if (a < llx) llx = a;
          if (b < lly) lly = b;
          if (a > urx) urx = a;
          if (b > ury) ury = b;
        }
      }
    };

    if (llx == DBL_MAX || lly == DBL_MAX || urx == -DBL_MAX || ury == -DBL_MAX) 
      therror(("This can't happen -- no data for a scrap!"));
    
    std::map<int,layerrecord>::iterator J = LAYERHASH.find(I->layer);
    if (J == LAYERHASH.end()) therror (("This can't happen!"));

    if (mode == ATLAS) {
      int Llx = (int) floor((llx-LAYOUT.overlap-LAYOUT.hoffset) / LAYOUT.hsize);
      int Lly = (int) floor((lly-LAYOUT.overlap-LAYOUT.voffset) / LAYOUT.vsize);
      int Urx = (int) floor((urx+LAYOUT.overlap-LAYOUT.hoffset) / LAYOUT.hsize);
      int Ury = (int) floor((ury+LAYOUT.overlap-LAYOUT.voffset) / LAYOUT.vsize);

      for (int i = Llx; i <= Urx; i++) {
        for (int j = Lly; j <= Ury; j++) {
          if (J->second.Z == 0) {    // Z layers don't create new sheets
            std::list<sheetrecord>::iterator sheet_it = find_sheet(I->layer,i,j);
            if (sheet_it == SHEET.end()) {
              sheet_it = SHEET.insert(SHEET.end(),SHREC);
              SHEET_JMP.insert(make_pair(xyz2str(I->layer,i,j),sheet_it));
            }
            sheet_it->layer = I->layer;
            sheet_it->namex = i;
            sheet_it->namey = j;

            std::map<int,std::set<std::string> >::iterator K = sheet_it->scraps.find(I->level);
            if (K == sheet_it->scraps.end()) {
              std::set<std::string> SCRP;
              sheet_it->scraps.insert(std::make_pair(I->level,SCRP));
              K = sheet_it->scraps.find(I->level);
            }
            ((*K).second).insert(I->name);
        
            if (J->second.minx > Llx) J->second.minx = Llx;
            if (J->second.miny > Lly) J->second.miny = Lly;
            if (J->second.maxx < Urx) J->second.maxx = Urx;
            if (J->second.maxy < Ury) J->second.maxy = Ury;
          } 
          // we add scrap to preview
          std::set<std::string> tmpset;
          std::string tmpstr;
          tmpstr = xyz2str(I->layer,i,j);
          std::map<std::string,std::set<std::string> >::iterator pr_it = preview.find(tmpstr);
          if (pr_it == preview.end()) {
            preview.insert(make_pair(tmpstr,tmpset));
            pr_it = preview.find(tmpstr);
          }
          pr_it->second.insert(I->name);
        }
      }
    }
    else {
      std::map<int,std::set<std::string> >::iterator K = (((*J).second).scraps).find(I->level);
      if (K == (((*J).second).scraps).end()) {
        std::set<std::string> SCRP;
        (((*J).second).scraps).insert(std::make_pair(I->level,SCRP));
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
//  cout << "MINMAX " << MINX << " " << MINY << " " << MAXX << " " << MAXY << std::endl;
}

void find_jumps() {
  for (std::list<sheetrecord>::iterator sheet_it = SHEET.begin(); 
                                   sheet_it != SHEET.end(); sheet_it++) {
    sheet_it->jumpW = 0;
    sheet_it->jumpE = 0;
    sheet_it->jumpN = 0;
    sheet_it->jumpS = 0;

    std::list<sheetrecord>::iterator I;
    
    int jump;    
                               
    std::string W = xyz2str(sheet_it->layer,sheet_it->namex-1,sheet_it->namey);
    std::string E = xyz2str(sheet_it->layer,sheet_it->namex+1,sheet_it->namey);
    std::string N = xyz2str(sheet_it->layer,sheet_it->namex,sheet_it->namey+1);
    std::string S = xyz2str(sheet_it->layer,sheet_it->namex,sheet_it->namey-1);
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

    std::map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
    if (lay_it == LAYERHASH.end()) therror (("This can't happen!"));

    if (!lay_it->second.U.empty()) {
      for (std::set<int>::iterator l_it = lay_it->second.U.begin(); 
                              l_it != lay_it->second.U.end(); l_it++) {
        std::map<int,layerrecord>::iterator alt_lay_it = LAYERHASH.find(*l_it);
        if (alt_lay_it == LAYERHASH.end()) therror(("This can't happen!"));
        jump = (alt_lay_it->second.Z == 0) ? *l_it : alt_lay_it->second.AltJump;
        std::string U = xyz2str(jump,sheet_it->namex,sheet_it->namey);
        if (SHEET_JMP.count(U) > 0) {
          I = SHEET_JMP.find(U)->second;
          sheet_it->jumpU.insert(jump);
          I->dest = true;
        }
      }
    }
    if (!lay_it->second.D.empty()) {
      for (std::set<int>::iterator l_it = lay_it->second.D.begin(); 
                              l_it != lay_it->second.D.end(); l_it++) {
        std::map<int,layerrecord>::iterator alt_lay_it = LAYERHASH.find(*l_it);
        if (alt_lay_it == LAYERHASH.end()) therror(("This can't happen!"));
        jump = (alt_lay_it->second.Z == 0) ? *l_it : alt_lay_it->second.AltJump;
        std::string D = xyz2str(jump,sheet_it->namex,sheet_it->namey);
        if (SHEET_JMP.count(D) > 0) {
          I = SHEET_JMP.find(D)->second;
          sheet_it->jumpD.insert(jump);
          I->dest = true;
        }
      }
    }
  }
}

int grid_str2int(std::string s, char init = 'A') {
  int res = 0;
  for (size_t i = 0; i < s.length(); i++) res = 26*res + s[i] - init + 1;
  return res;
}

std::string grid_int2str(int i, char init = 'A') {
  if (i <= 0) return "?";
  std::string s;
  int rem;
  while (i > 0) {
    rem = i % 26;
    if (rem == 0) {     // Z or z
      s = (char) (init + 26 - 1) + s;
      i = i/26 - 1;
    } else {
      s = (char) (init + rem - 1) + s;
      i = i/26;
    }
  }
  return s;
}

std::regex reg_num(R"(^\d+$)");
std::regex reg_s_lower(R"(^[a-z]+$)");
std::regex reg_s_upper(R"(^[A-Z]+$)");

std::string grid_name(std::string s, int offset) {
  if (std::regex_match(s,reg_num)) return fmt::format("{:d}", atoi(s.c_str())+offset);
  else if (std::regex_match(s,reg_s_lower)) return grid_int2str(grid_str2int(s,'a')+offset,'a');
  else if (std::regex_match(s,reg_s_upper)) return grid_int2str(grid_str2int(s,'A')+offset,'A');
  else return "?";
}

std::set<int> find_excluded_pages(std::string s) {
  std::set<int> excl;
  int i,j;
  char c;
//  istrstream S(s.c_str());
  std::istringstream S(s);
  
  c = ',';
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
//cout << std::endl;
//cout << "Excl.list: " << s << std::endl;  
//cout << "Excl. set: ";  
//for (set<int>::iterator I = excl.begin(); I != excl.end(); I++) 
//  cout << *I << " ";
//cout << std::endl;
  return excl;
}

void sort_sheets() {
  int pageid = 1 + LAYOUT.own_pages, tmppagenum = 1;
  std::set<int> excluded;
  bool wait_for_title;
  
  if (LAYOUT.excl_pages) excluded = find_excluded_pages(LAYOUT.excl_list);

  for (std::map<int,layerrecord>::reverse_iterator I = LAYERHASH.rbegin(); 
                                      I != LAYERHASH.rend(); I++) {
//    I->second.minid = pageid;
    I->second.bookmark = false;
    wait_for_title = (I->second.T !="" && LAYOUT.title_pages) ? true : false;

    for (int j = I->second.maxy; j >= I->second.miny; j--) {
      for (int i = I->second.minx; i <= I->second.maxx; i++) {
        std::list<sheetrecord>::iterator sheet_it = find_sheet(I->first,i,j);
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
            SHEET_JMP.erase(xyz2str(sheet_it->layer,
                                    sheet_it->namex,sheet_it->namey));
            SHEET.erase(sheet_it);
//cout << "Should erase sheet " << tmppagenum << std::endl;
          }
          tmppagenum++;
        }
      }
    }
//    I->second.maxid = pageid - 1;
  }
  SHEET.sort();
//cout << "sheets: " << SHEET.size() << std::endl;  
}


void print_preview(int up,std::ofstream& PAGEDEF,double HSHIFT,double VSHIFT,
                   std::list<sheetrecord>::iterator sheet_it = std::list<sheetrecord>::iterator()) {
  std::set<int> used_layers;
  std::set<std::string> used_scraps;
  double xc = 0, yc = 0;
  
  if (LAYOUT.OCG) {
    if (mode == MAP)
      PAGEDEF << "\\setbox\\xxx=\\hbox to \\adjustedHS{%" << std::endl;
    else 
      PAGEDEF << "\\setbox\\xxx=\\hbox to " << fmt::format("{}",thdouble(HS,prec_xy)) << "bp{%" << std::endl;
  }

//  PAGEDEF << (up ? "\\PL{q .1 w}%" : "\\PL{q .8 g}%") << std::endl;
  
  if (up) { 
    PAGEDEF << "\\PL{q " << LAYOUT.col_preview_above.to_pdfliteral(fillstroke::stroke) <<
               "}%" << std::endl;
  }
  else { 
    PAGEDEF << "\\PL{q " << LAYOUT.col_preview_below.to_pdfliteral(fillstroke::fill) <<
               "}%" << std::endl;
  }

  if (mode == ATLAS) {
    std::map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
    if (lay_it == LAYERHASH.end()) therror(("This can't happen!"));
    used_layers = (up ? lay_it->second.U : lay_it->second.D);
  }
  else {
    used_layers = (up ? MAP_PREVIEW_UP : MAP_PREVIEW_DOWN);
  }
  for (std::set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
    if (mode == ATLAS) {
        std::map<std::string,std::set<std::string> >::iterator pr_it = 
          preview.find(xyz2str(*I,sheet_it->namex,sheet_it->namey));
        if (pr_it != preview.end()) used_scraps = pr_it->second;
    }
    else {
      std::map<int,layerrecord>::iterator J = LAYERHASH.find(*I);
      if (J == LAYERHASH.end()) therror(("This can't happen!"));
      used_scraps = J->second.allscraps;
    }
    if (!used_scraps.empty()) {
      for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
        if (used_scraps.count(K->name) > 0) {
          if (up) {
            if (K->B != "" && K->sect == 0) {
              xc = K->B1; yc = K->B2;
              xc -= HSHIFT; yc -= VSHIFT;
              PAGEDEF << (mode == MAP && LAYOUT.OCG ? "\\PBcorr{" : "\\PB{") << 
                      fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                      tex_get_ref(tex_Xname("B"+(K->name))) << "}%" << std::endl;
            }
          }
          else {
            if (K->I != "" && K->sect == 0) {
              xc = K->I1; yc = K->I2;
              xc -= HSHIFT; yc -= VSHIFT;
              PAGEDEF << (mode == MAP && LAYOUT.OCG ? "\\PBcorr{" : "\\PB{") << 
                      fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                      tex_get_ref(tex_Xname("I"+(K->name))) << "}%" << std::endl;
            }
          }
        }
      }
    }
  }
  PAGEDEF << "\\PL{Q}%" << std::endl;
  if (LAYOUT.OCG) {
    if (mode==MAP)
      PAGEDEF << "\\hfill}\\ht\\xxx=\\adjustedVS\\dp\\xxx=0bp" << std::endl;
    else 
      PAGEDEF << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
    PAGEDEF << "\\immediate\\pdfxform ";
    PAGEDEF << "attr{/OC \\the\\" << (up ? "ocU" : "ocD") << "\\space 0 R} ";
    if (icc_used())  PAGEDEF << "resources{/ColorSpace <<" << icc2pdfresources() << ">>} ";
    if (mode == MAP)
      PAGEDEF << "\\xxx\\PB{-\\adjustedX}{-\\adjustedY}{\\pdflastxform}%" << std::endl;
    else 
      PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;
  }
}



void compose_page(std::list<sheetrecord>::iterator sheet_it, std::ofstream& PAGE) {
  std::map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
  if (lay_it == LAYERHASH.end()) therror (("This can't happen!"));

  if (sheet_it->title_before) {
    PAGE << "\\TITLE{" << utf2tex(lay_it->second.T) << "}\n";
  }

  PAGE << "%\n% Page: " << u2str(sheet_it->id) << std::endl << "%\n";
  if (sheet_it->dest) PAGE << "\\pdfdest name {" << u2str(sheet_it->id) << 
                              "} xyz" << std::endl;

  if (sheet_it->bookmark) {
    PAGE << "\\pdfoutline goto name {" << u2str(sheet_it->id) << 
                  "} count 0 {\\ne\\376\\ne\\377" << 
                  utf2texoctal(lay_it->second.N) << "}%" << std::endl;
  }
  //PAGE.precision(6);
  PAGE << "\\setbox\\mapbox=\\hbox to " << fmt::format("{}",thdouble(HS,prec_matrix)) << "bp{%" << std::endl;
  //PAGE.precision(2);
  PAGE << "\\rlap{\\pdfrefxform\\" << tex_Sname(u2str(sheet_it->id)) << 
          "}%" << std::endl;

  // map hyperlinks
  int lw = 25;
  double lhy = VS - 2*lw;
  double lhx = HS - 2*lw;
  if (sheet_it->jumpW) PAGE << "\\flatlink{0}{" << lw << "}{" << lw << 
          "}{" << fmt::format("{}",thdouble(lhy,prec_xy)) << "}{" << u2str(sheet_it->jumpW) << "}%\n";
  if (sheet_it->jumpE) PAGE << "\\flatlink{" << HS-lw << "}{" << lw <<
          "}{" << lw << "}{" << fmt::format("{}",thdouble(lhy,prec_xy)) << "}{" << u2str(sheet_it->jumpE) << "}%\n";
  if (sheet_it->jumpN) PAGE << "\\flatlink{" << lw << "}{" << fmt::format("{}",thdouble(VS-lw,prec_xy)) <<
          "}{" << fmt::format("{}",thdouble(lhx,prec_xy)) << "}{" << lw << "}{" << u2str(sheet_it->jumpN)<< "}%\n";
  if (sheet_it->jumpS) PAGE << "\\flatlink{" << lw << "}{0}{" << fmt::format("{}",thdouble(lhx,prec_xy)) <<
          "}{" << lw << "}{" << u2str(sheet_it->jumpS) << "}%\n";
  

  //PAGE.precision(6);
  PAGE << "\\hfil}\\ht\\mapbox=" << fmt::format("{}",thdouble(VS,prec_matrix)) << "bp%" << std::endl;
  //PAGE.precision(2);

  PAGE << "\\pagelabel={" << grid_name(LAYOUT.labely,-sheet_it->namey) << 
                        " " << grid_name(LAYOUT.labelx,sheet_it->namex) <<
                        "}%" << std::endl;
  if (LAYOUT.page_numbering) PAGE << "\\pagenum=" << 
                             sheet_it->id << "%" << std::endl;
    
//    up and down links

  if (!sheet_it->jumpU.empty()) {
    PAGE << "\\pointerU={%\n";
    for (std::set<int>::reverse_iterator l_it = sheet_it->jumpU.rbegin();
                                    l_it != sheet_it->jumpU.rend(); l_it++) {
      std::list<sheetrecord>::iterator s_it = 
        find_sheet(*l_it,sheet_it->namex,sheet_it->namey);
      if (s_it == SHEET.end()) therror (("This can't happen!"));
      PAGE << utf2tex(LAYERHASH.find(*l_it)->second.N) << "|" <<
           s_it->id << "|" << u2str(s_it->id) << "||%" << std::endl;
    }
    PAGE << "}%\n";
  }
  else PAGE << "\\pointerU={notdef}%" << std::endl;

  if (!sheet_it->jumpD.empty()) {
    PAGE << "\\pointerD={%\n";
    for (std::set<int>::reverse_iterator l_it = sheet_it->jumpD.rbegin();
                                    l_it != sheet_it->jumpD.rend(); l_it++) {
      std::list<sheetrecord>::iterator s_it = 
        find_sheet(*l_it,sheet_it->namex,sheet_it->namey);
      if (s_it == SHEET.end()) therror (("This can't happen!"));
      PAGE << utf2tex(LAYERHASH.find(*l_it)->second.N) << "|" <<
           s_it->id << "|" << u2str(s_it->id) << "||%" << std::endl;
    }
    PAGE << "}%\n";
  }
  else PAGE << "\\pointerD={notdef}%" << std::endl;

  PAGE << "\\pagename={" << utf2tex(lay_it->second.N) << "}%" << std::endl;

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

  PAGE << "\\setbox\\navbox=\\hbox{%" << std::endl;
  // navigator hyperlinks
  int nav_x = 2*LAYOUT.nav_right+1;
  int nav_y = 2*LAYOUT.nav_up+1;
  double HSN = LAYOUT.hsize / LAYOUT.nav_factor * nav_x;
  double VSN = LAYOUT.vsize / LAYOUT.nav_factor * nav_y;
  for (int i=-LAYOUT.nav_right; i <= LAYOUT.nav_right; i++) {
    for (int j=-LAYOUT.nav_up; j <= LAYOUT.nav_up; j++) {
      if (i!=0 || j!=0) {
        std::string tmp = xyz2str(sheet_it->layer,
                             sheet_it->namex+i,sheet_it->namey+j);
        if (SHEET_JMP.count(tmp) > 0) {
          PAGE << "\\flatlink{" << fmt::format("{}",thdouble(HSN*(i+LAYOUT.nav_right)/nav_x,prec_xy)) <<
            "}{" << fmt::format("{}",thdouble(VSN*(j+LAYOUT.nav_up)/nav_y,prec_xy)) << "}{" <<
            fmt::format("{}",thdouble(HSN/nav_x,prec_xy)) << "}{" << fmt::format("{}",thdouble(VSN/nav_y,prec_xy)) << "}{" <<
            u2str(SHEET_JMP.find(tmp)->second->id) << "}%\n";
        }
      }
    }
  }
  
  PAGE << "\\pdfrefxform\\" << tex_Nname(u2str(sheet_it->id)) << "}%" << std::endl;

  PAGE << "\\dopage\\eject" << std::endl;
}

void print_page_bg(std::ofstream& PAGEDEF) {
  if (! LAYOUT.transparent_map_bg) {

    // bg rectangle
    PAGEDEF << "\\PL{q " << LAYOUT.col_background.to_pdfliteral(fillstroke::fill) <<
                     " 0 0 " << fmt::format("{}",thdouble(HS,prec_xy)) << " " << fmt::format("{}",thdouble(VS,prec_xy)) << " re f Q}%" << std::endl;
  }
}

void print_page_bg_scraps(int layer, std::ofstream& PAGEDEF, 
               std::list<sheetrecord>::iterator sheet_it = std::list<sheetrecord>::iterator()) {
  // if transparency is used, all scraps should be filled white 
  // on the coloured background, just before preview_down is displayed
  // and transparency is turned on
/*  if (LAYOUT.transparency && 
      ((LAYOUT.background_r != 1) || 
       (LAYOUT.background_g != 1) || 
       (LAYOUT.background_b != 1))) { */  // even if bg is white, pdf might
                                          // be included in other pdf with
                                          // non-white bg; we should avoid 
                                          // overoptimization
  if (LAYOUT.transparency ) {

    double HSHIFT=0, VSHIFT=0, xc = 0, yc = 0;
    std::map < int,std::set<std::string> > LEVEL;
    std::set <std::string> used_scraps;
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

    PAGEDEF << "\\PL{q " << black2pdf(0, fillstroke::fill) << "}%" << std::endl;      // white background of the scrap
    for (std::map < int,std::set<std::string> >::iterator I = LEVEL.begin();
                                       I != LEVEL.end(); I++) {
      used_scraps = (*I).second;
      // scrap backgrounds
      for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
        if (used_scraps.count(K->name) > 0 && K->I != "") {
          xc = K->I1; yc = K->I2;
          xc -= HSHIFT; yc -= VSHIFT;
          PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                  tex_get_ref(tex_Xname("I"+(K->name))) << "}%" << std::endl;
        }
      }
    }
    PAGEDEF << "\\PL{Q}%" << std::endl;         // end of white color for filled bg
  }
}

void print_surface_bitmaps (std::ofstream &PAGEDEF, double shiftx, double shifty) {
  if (LAYOUT.transparency || LAYOUT.OCG) {
    if (mode == MAP)
      PAGEDEF << "\\setbox\\xxx=\\hbox to\\adjustedHS{%" << std::endl;
    else
      PAGEDEF << "\\setbox\\xxx=\\hbox to" << fmt::format("{}",thdouble(HS,prec_xy)) << "bp{%" << std::endl;
    PAGEDEF << "\\PL{/GS2 gs}%" << std::endl;
  }
  int i = 1;
  //PAGEDEF.precision(6);
  for (std::list<surfpictrecord>::iterator I = SURFPICTLIST.begin();
                                      I != SURFPICTLIST.end(); I++) {
    if (mode == MAP && (LAYOUT.transparency || LAYOUT.OCG)) {
      PAGEDEF << "\\bitmapcorr{";
    } else {
      PAGEDEF << "\\bitmap{";
    }
    PAGEDEF << 
          fmt::format("{}",thdouble(I->xx,prec_matrix)) << "}{" << fmt::format("{}",thdouble(I->yx,prec_matrix)) << "}{" <<
          fmt::format("{}",thdouble(I->xy,prec_matrix)) << "}{" << fmt::format("{}",thdouble(I->yy,prec_matrix)) << "}{" <<
          fmt::format("{}",thdouble(I->dx - shiftx,prec_matrix)) << "}{" << fmt::format("{}",thdouble(I->dy - shifty,prec_matrix)) <<
          "}{\\" << tex_BMPname(u2str(i)) << "}%" << std::endl;
    i++;
  };
  //PAGEDEF.precision(2);
  if (LAYOUT.transparency || LAYOUT.OCG) {
    if (mode == MAP)
      PAGEDEF << "\\hfill}\\ht\\xxx=\\adjustedVS\\dp\\xxx=0bp" << std::endl;
    else
      PAGEDEF << "\\hfill}\\ht\\xxx="<< fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
    PAGEDEF << "\\immediate\\pdfxform ";
    PAGEDEF << "attr{";
    if (LAYOUT.transparency) PAGEDEF << "/Group \\the\\attrid\\space 0 R ";
    if (LAYOUT.OCG) PAGEDEF << "/OC \\the\\ocSUR\\space 0 R ";
    PAGEDEF << "} ";
    PAGEDEF << "resources{/ExtGState \\the\\resid\\space 0 R}";
    if (mode == MAP)
      PAGEDEF << "\\xxx\\PB{-\\adjustedX}{-\\adjustedY}{\\pdflastxform}%" << std::endl;
    else
      PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;
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


void print_grid_pdf(std::ofstream& PAGEDEF, double LLX, double LLY, double origMINX,double origMINY,double origMAXX, double origMAXY) {
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
  
  llnew.x = std::min(std::min(llrot.x, urrot.x), std::min(lrrot.x, ulrot.x));
  llnew.y = std::min(std::min(llrot.y, urrot.y), std::min(lrrot.y, ulrot.y));
  urnew.x = std::max(std::max(llrot.x, urrot.x), std::max(lrrot.x, ulrot.x));
  urnew.y = std::max(std::max(llrot.y, urrot.y), std::max(lrrot.y, ulrot.y));
  
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
  
//cout << "***" << LAYOUT.hgridsize << std::endl;

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
	PAGEDEF << "\\PL{" << fmt::format("{}",thdouble(cosr,prec_matrix)) << " " << fmt::format("{}",thdouble(sinr,prec_matrix)) << " "
                           << fmt::format("{}",thdouble(-sinr,prec_matrix)) << " " << fmt::format("{}",thdouble(cosr,prec_matrix)) << " "
                           << fmt::format("{}",thdouble(out.x,prec_matrix)) << " " << fmt::format("{}",thdouble(out.y,prec_matrix)) << " cm}";
	PAGEDEF << "\\PB{0}{0}{" << tex_get_ref(tex_Wname("grid") + u2str(elem+1)) << "}";
        PAGEDEF << "\\PL{Q}%" << std::endl;

        if (LAYOUT.grid_coord_freq==2 || (LAYOUT.grid_coord_freq==1 && elem!=4)) {
          tmp.x = i;
          tmp.y = j;
          out = rotatedaround(tmp,origin,LAYOUT.gridrot);
          out.x -= LLX;
          out.y -= LLY;
          PAGEDEF << "\\PL{q}";
          PAGEDEF << "\\PL{" << fmt::format("{}",thdouble(cosr,prec_matrix)) << " " << fmt::format("{}",thdouble(sinr,prec_matrix)) << " "
                             << fmt::format("{}",thdouble(-sinr,prec_matrix)) << " " << fmt::format("{}",thdouble(cosr,prec_matrix)) << " "
                             << fmt::format("{}",thdouble(out.x,prec_matrix)) << " " << fmt::format("{}",thdouble(out.y,prec_matrix)) << " cm}";
	  PAGEDEF << "\\gridcoord{" << (row == 2 ? (col == 2 ? 1 : 3) : (col == 2 ? 7 : 9)) << 
	      "}{$(" << std::setprecision(0) << 
	      fmt::format("{}",thdouble(G_real_init_x+ii*LAYOUT.XS,0)) << "," <<
              fmt::format("{}",thdouble(G_real_init_y+jj*LAYOUT.YS,0)) << std::setprecision(2) << ")$}%" << std::endl;
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
	PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(i-LLX+LAYOUT.gridcell[elem].x,prec_xy)) << "}{" <<
			      fmt::format("{}",thdouble(j-LLY+LAYOUT.gridcell[elem].y,prec_xy)) << "}{" <<
			      tex_get_ref(tex_Wname("grid") + u2str(elem+1)) << "}%" << std::endl;

        if (col == 0 && LAYOUT.grid_coord_freq > 0) {
          PAGEDEF << "\\PL{q}";
          PAGEDEF << "\\PL{1 0 0 1 " << fmt::format("{}",thdouble(i-LLX,prec_xy)) << " " << fmt::format("{}",thdouble(j-LLY,prec_xy)) << " cm}";
          PAGEDEF << "\\gridcoord{" << (row==2?3:9) << "}{$" << 
	      std::setprecision(0) << fmt::format("{}",thdouble(G_real_init_y+jj*LAYOUT.YS,0)) <<
	      std::setprecision(2)<< "$}";
          PAGEDEF << "\\PL{Q}%" << std::endl;
	}
        if (col == 2 && LAYOUT.grid_coord_freq == 2) {
          PAGEDEF << "\\PL{q}";
          PAGEDEF << "\\PL{1 0 0 1 " << fmt::format("{}",thdouble(i-LLX,prec_xy)) << " " << fmt::format("{}",thdouble(j-LLY,prec_xy)) << " cm}";
          PAGEDEF << "\\gridcoord{" << (row==2?1:7) << "}{$" << 
	      std::setprecision(0) << fmt::format("{}",thdouble(G_real_init_y+jj*LAYOUT.YS,0)) <<
	      std::setprecision(2)<< "$}";
          PAGEDEF << "\\PL{Q}%" << std::endl;
        }

      }
    }
  } 

  PAGEDEF << "\\PL{Q}%" << std::endl;
}



void print_map(int layer, std::ofstream& PAGEDEF, 
               std::list<sheetrecord>::iterator sheet_it = std::list<sheetrecord>::iterator()){
  double HSHIFT=0, VSHIFT=0, xc = 0, yc = 0;
  std::map < int,std::set<std::string> > LEVEL;
  std::set <std::string> page_text_scraps,used_scraps;
  std::string buffer;
  std::deque<std::string> thstack;

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

  for (std::map < int,std::set<std::string> >::iterator I = LEVEL.begin();
                                         I != LEVEL.end(); I++) {
    used_scraps = (*I).second;
    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
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
  
  for (std::map < int,std::set<std::string> >::iterator I = LEVEL.begin();
                                         I != LEVEL.end(); I++) {
    used_scraps = (*I).second;

    if (LAYOUT.transparency) {                 // transparency group beginning
      PAGEDEF << "\\setbox\\xxx=\\hbox to " << fmt::format("{}",thdouble(HS,prec_xy)) << "bp{%" << std::endl;
      PAGEDEF << "\\PL{/GS1 gs}%" << std::endl;     // beginning of transparency
    }

    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->I != "") {
        xc = K->I1; yc = K->I2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                tex_get_ref(tex_Xname("I"+(K->name)+"COLORED")) << "}%" << std::endl;
      }
    }   // end of the colored background

    // sketches
    //PAGEDEF.precision(6);
    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0) {
        for (std::list<surfpictrecord>::iterator I_sk = K->SKETCHLIST.begin();
                                            I_sk != K->SKETCHLIST.end(); I_sk++) {
          PAGEDEF << "\\pdfximage{" << (std::string) I_sk->filename << "}%" << std::endl;
          PAGEDEF << "\\bitmap{" <<
              fmt::format("{}",thdouble(I_sk->xx,prec_matrix)) << "}{" << fmt::format("{}",thdouble(I_sk->yx,prec_matrix)) << "}{" <<
              fmt::format("{}",thdouble(I_sk->xy,prec_matrix)) << "}{" << fmt::format("{}",thdouble(I_sk->yy,prec_matrix)) << "}{" <<
              fmt::format("{}",thdouble(I_sk->dx - HSHIFT,prec_matrix)) << "}{" << fmt::format("{}",thdouble(I_sk->dy - VSHIFT,prec_matrix))  <<
              "}{\\pdflastximage}%" << std::endl;
        };
      };
    }
    //PAGEDEF.precision(2);
    // end of sketches

    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->G != "") {
        xc = K->G1; yc = K->G2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                tex_get_ref(tex_Xname("G"+(K->name))) << "}%" << std::endl;
      };
    }

    if (LAYOUT.transparency) {
      PAGEDEF << "\\PL{/GS0 gs}%" << std::endl;      // end of default transparency
      if (LAYOUT.smooth_shading == shading_mode::quick) {
        PAGEDEF << "\\setbox\\xxx=\\hbox to " << fmt::format("{}",thdouble(HS,prec_xy)) << "bp{%\n";  // add another knockout group
      }
    }

    PAGEDEF << "\\PL{q 0 0 m " << fmt::format("{}",thdouble(HS,prec_xy)) << " 0 l " << fmt::format("{}",thdouble(HS,prec_xy)) << " " <<
                                  fmt::format("{}",thdouble(VS,prec_xy)) << " l 0 " << fmt::format("{}",thdouble(VS,prec_xy)) << " l 0 0 l}";
                               // beginning of the text clipping path definition

    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (page_text_scraps.count(K->name) > 0 && 
          K->P != "" && K->level >= (I->first)) {
        xc = HSHIFT - K->S1; yc = VSHIFT - K->S2;
        std::ifstream G((K->P).c_str());
        if(!G) therror((IOerr(K->P)));
        while(G >> buffer) {
          if ((buffer == "m") || (buffer == "l") || (buffer == "c")) {
            PAGEDEF << "\\PL{"; 
            for(unsigned i=0; i<thstack.size(); i=i+2) {
              PAGEDEF << fmt::format("{}", thdouble(std::stod(thstack[i])-xc,prec_xy)) << " " <<
                         fmt::format("{}", thdouble(std::stod(thstack[i+1])-yc,prec_xy)) << " ";
            }
            PAGEDEF << buffer << "}%" << std::endl;
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
    
    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->F != "") {
        xc = K->F1; yc = K->F2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                tex_get_ref(tex_Xname(K->name)) << "}%" << std::endl;
      };
    }
   
    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->E != "") {
        xc = K->E1; yc = K->E2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                tex_get_ref(tex_Xname("E"+(K->name))) << "}%" << std::endl;
      };
    }
   
    PAGEDEF << "\\PL{Q}";   // end of clipping by text

    for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
      if (used_scraps.count(K->name) > 0 && K->X != "") {
        xc = K->X1; yc = K->X2;
        xc -= HSHIFT; yc -= VSHIFT;
        PAGEDEF << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                tex_get_ref(tex_Xname("X"+(K->name))) << "}%" << std::endl;
      };
    }
    if (LAYOUT.transparency) {
      if (LAYOUT.smooth_shading == shading_mode::quick) {
        PAGEDEF << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
        PAGEDEF << "\\immediate\\pdfxform ";
        PAGEDEF << "attr{/Group \\the\\attrid\\space 0 R} ";
        PAGEDEF << "resources{/ExtGState \\the\\resid\\space 0 R";
        if (icc_used()) PAGEDEF << " /ColorSpace <<" << icc2pdfresources() << ">> ";
        PAGEDEF << "}";
        PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;
      }
      PAGEDEF << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
      PAGEDEF << "\\immediate\\pdfxform ";
      // the external group can't be a knockout group in the quick mode
      if (LAYOUT.smooth_shading != shading_mode::quick) PAGEDEF << "attr{/Group \\the\\attrid\\space 0 R} ";
      PAGEDEF << "resources{/ExtGState \\the\\resid\\space 0 R";
      if (icc_used()) PAGEDEF << " /ColorSpace <<" << icc2pdfresources() << ">> ";
      PAGEDEF << "}";
      PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;
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

void print_navigator(std::ofstream& P, std::list<sheetrecord>::iterator sheet_it) {
  std::set<std::string> NAV_SCRAPS;
  std::set<int> used_layers;
  std::set<std::string> used_scraps;
  int nav_x = 2*LAYOUT.nav_right+1;
  int nav_y = 2*LAYOUT.nav_up+1;
  double HSN = LAYOUT.hsize / LAYOUT.nav_factor * nav_x;
  double VSN = LAYOUT.vsize / LAYOUT.nav_factor * nav_y;
  double xc = 0, yc = 0;

  P << "%\n\\setbox\\xxx=\\hbox to " << fmt::format("{}",thdouble(HSN,prec_xy)) << "bp{%\n\\PL{q ";
  //P.precision(6);
  P << fmt::format("{}",thdouble(1/LAYOUT.nav_factor,prec_matrix)) << " 0 0 " <<
       fmt::format("{}",thdouble(1/LAYOUT.nav_factor,prec_matrix)) << " 0 0 cm}%\n";
  //P.precision(2);

  std::map<int,layerrecord>::iterator lay_it = LAYERHASH.find(sheet_it->layer);
  if (lay_it == LAYERHASH.end()) therror (("This can't happen!"));

  NAV_SCRAPS.clear();
  if (!lay_it->second.D.empty()) {
    P << "\\PL{" << black2pdf(0.2, fillstroke::fill) << "}%\n";
    used_layers = lay_it->second.D;
    for (std::set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
      for (int i = sheet_it->namex-LAYOUT.nav_right; 
               i <= sheet_it->namex+LAYOUT.nav_right; i++) {
        for (int j = sheet_it->namey-LAYOUT.nav_up; 
                 j <= sheet_it->namey+LAYOUT.nav_up; j++) {
          used_scraps.clear();
          std::map<std::string,std::set<std::string> >::iterator pr_it = 
            preview.find(xyz2str(*I,i,j));
          if (pr_it != preview.end()) used_scraps = pr_it->second;
          if (!used_scraps.empty()) {
            for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
              if (used_scraps.count(K->name) > 0 && 
                           NAV_SCRAPS.count(K->name) == 0 &&
                           K->I != "" && K->sect == 0) {
                xc = K->I1; yc = K->I2;
                xc -= LAYOUT.hsize * (sheet_it->namex - LAYOUT.nav_right) + 
                      LAYOUT.hoffset; 
                yc -= LAYOUT.vsize * (sheet_it->namey - LAYOUT.nav_up) + 
                      LAYOUT.voffset; 
                P << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                        tex_get_ref(tex_Xname("I"+(K->name))) << "}%" << std::endl;
                NAV_SCRAPS.insert(K->name);
              }
            }
          }
        }
      }
    }
  }
  NAV_SCRAPS.clear();
  P << "\\PL{" << black2pdf(1, fillstroke::fill) << "}%\n";
  for (int i = sheet_it->namex-LAYOUT.nav_right; 
           i <= sheet_it->namex+LAYOUT.nav_right; i++) {
    for (int j = sheet_it->namey-LAYOUT.nav_up; 
             j <= sheet_it->namey+LAYOUT.nav_up; j++) {
      used_scraps.clear();
      std::map<std::string,std::set<std::string> >::iterator pr_it = 
        preview.find(xyz2str(sheet_it->layer,i,j));
      if (pr_it != preview.end()) used_scraps = pr_it->second;
      if (!used_scraps.empty()) {
        for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
          if (used_scraps.count(K->name) > 0 && 
                       NAV_SCRAPS.count(K->name) == 0 &&
                       K->I != "" && K->sect == 0) {
            xc = K->I1; yc = K->I2;
            xc -= LAYOUT.hsize * (sheet_it->namex - LAYOUT.nav_right) + 
                  LAYOUT.hoffset; 
            yc -= LAYOUT.vsize * (sheet_it->namey - LAYOUT.nav_up) + 
                  LAYOUT.voffset; 
            P << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                    tex_get_ref(tex_Xname("I"+(K->name))) << "}%" << std::endl;
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
    for (std::set<int>::iterator I=used_layers.begin(); I != used_layers.end(); I++) {
      for (int i = sheet_it->namex-LAYOUT.nav_right; 
               i <= sheet_it->namex+LAYOUT.nav_right; i++) {
        for (int j = sheet_it->namey-LAYOUT.nav_up; 
                 j <= sheet_it->namey+LAYOUT.nav_up; j++) {
          used_scraps.clear();
          std::map<std::string,std::set<std::string> >::iterator pr_it = 
            preview.find(xyz2str(*I,i,j));
          if (pr_it != preview.end()) used_scraps = pr_it->second;
          if (!used_scraps.empty()) {
            for (std::list<scraprecord>::iterator K = SCRAPLIST.begin(); K != SCRAPLIST.end(); K++) {
              if (used_scraps.count(K->name) > 0 && 
                           NAV_SCRAPS.count(K->name) == 0 &&
                           K->B != "" && K->sect == 0) {
                xc = K->B1; yc = K->B2;
                xc -= LAYOUT.hsize * (sheet_it->namex - LAYOUT.nav_right) + 
                      LAYOUT.hoffset; 
                yc -= LAYOUT.vsize * (sheet_it->namey - LAYOUT.nav_up) + 
                      LAYOUT.voffset; 
                P << "\\PB{" << fmt::format("{}",thdouble(xc,prec_xy)) << "}{" << fmt::format("{}",thdouble(yc,prec_xy)) << "}{" <<
                        tex_get_ref(tex_Xname("B"+(K->name))) << "}%" << std::endl;
                NAV_SCRAPS.insert(K->name);
              }
            }
          }
        }
      }
    }
  }

  // navigator grid
  P << "\\PL{Q}" << "\\PL{q " << black2pdf(1, fillstroke::stroke) << " 0 0 " <<
       fmt::format("{}",thdouble(HSN,prec_xy)) << " " << fmt::format("{}",thdouble(VSN,prec_xy)) << " re S 0.1 w}";
  for (int i = 1; i < nav_x; i++)      
    P << "\\PL{" << fmt::format("{}",thdouble(HSN*i/nav_x,prec_xy)) << " 0 m " << fmt::format("{}",thdouble(HSN*i/nav_x,prec_xy)) << " " <<
         fmt::format("{}",thdouble(VSN,prec_xy)) << " l S}%\n";
  for (int i = 1; i < nav_y; i++)
    P << "\\PL{0 " << fmt::format("{}",thdouble(VSN*i/nav_y,prec_xy)) << " m " << fmt::format("{}",thdouble(HSN,prec_xy)) << " " <<
         fmt::format("{}",thdouble(VSN*i/nav_y,prec_xy)) << " l S}%\n";
  P << "\\PL{0.4 w " <<
    fmt::format("{}",thdouble(HSN*LAYOUT.nav_right/nav_x,prec_xy)) << " " << fmt::format("{}",thdouble(VSN*LAYOUT.nav_up/nav_y,prec_xy)) << " " <<
    fmt::format("{}",thdouble(HSN/nav_x,prec_xy)) << " " << fmt::format("{}",thdouble(VSN/nav_y,prec_xy)) << " " << " re S Q}";
  // XObject definition
  P << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VSN,prec_xy)) << "bp\\dp\\xxx=0bp\n";
  P << "\\immediate\\pdfxform";
  if (icc_used())  P << " resources{/ColorSpace <<" << icc2pdfresources() << ">>} ";
  P << "\\xxx\\newcount\\" <<
       tex_Nname(u2str(sheet_it->id)) << " \\" <<
       tex_Nname(u2str(sheet_it->id)) << "=\\pdflastxform" << std::endl;
}


void print_margins(std::ofstream& PAGEDEF) {
  PAGEDEF << "\\PL{q}";
//  PAGEDEF << "\\PL{3 w 0 0 " << HS << " " << VS << " re S}";
  if (LAYOUT.overlap > 0) {
    double i = LAYOUT.hsize + LAYOUT.overlap; 
    double j = LAYOUT.vsize + LAYOUT.overlap;
    PAGEDEF << "\\PL{" << black2pdf(1, fillstroke::stroke) << "}";
    PAGEDEF << "\\PL{0.5 w}";
    PAGEDEF << "\\PL{0 " << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << " m " << fmt::format("{}",thdouble(HS,prec_xy)) << " " <<
                            fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << " l S}";
    PAGEDEF << "\\PL{0 " << fmt::format("{}",thdouble(j,prec_xy)) << " m " << fmt::format("{}",thdouble(HS,prec_xy)) << " "
                         << fmt::format("{}",thdouble(j,prec_xy)) << " l S}";
    PAGEDEF << "\\PL{" << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << " 0 m " << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) <<
               " " << fmt::format("{}",thdouble(VS,prec_xy)) << " l S}";
    PAGEDEF << "\\PL{" << fmt::format("{}",thdouble(i,prec_xy)) << " 0 m " << fmt::format("{}",thdouble(i,prec_xy)) << " " <<
                          fmt::format("{}",thdouble(VS,prec_xy)) << " l S}";
  }
  PAGEDEF << "\\PL{Q}%" << std::endl;
}

void icc_check_file(std::string fname, std::string type) {
  std::ifstream iccfile(fname, std::ios::binary);
  if(!iccfile) therror((IOerr(fname)));
  char buffer[5];
  iccfile.seekg(16);
  iccfile.read(buffer,4);
  if (type != std::string(buffer)) therror(((std::string("Invalid ICC profile type: expected ")+type+", got "+buffer).c_str()));
}

void build_pages() {
  
  std::ofstream PAGEDEF("th_pagedef.tex");
  if(!PAGEDEF) therror(("Can't write file th_pagedef.tex"));
  PAGEDEF.setf(std::ios::fixed, std::ios::floatfield);
  PAGEDEF.precision(2);

  std::ofstream PAGE("th_pages.tex");
  if(!PAGE) therror(("Can't write file th_pages.tex"));
  PAGE.setf(std::ios::fixed, std::ios::floatfield);
  PAGE.precision(2);

  std::ofstream PDFRES("th_resources.tex");
  if(!PDFRES) therror(("Can't write file th_resources.tex"));

  PDFRES << "\\pdfminorversion=5%\n";

  if (thcfg.reproducible_output) {
    PDFRES <<
R"(\pdfcompresslevel=9%
\pdfobjcompresslevel=2%
\pdfdecimaldigits=3%
\ifx\directlua\undefined
  \pdfsuppressptexinfo=-1\pdftrailerid{}\pdfinfoomitdate=1%
  \pdfinfo{/Creator (Therion, MetaPost, TeX) /Producer (pdfTeX)}%
\else
  \pdfvariable suppressoptionalinfo\numexpr1+2+4+8+32+64+512%
  \pdfinfo{/Creator (Therion, MetaPost, TeX) /Producer (LuaTeX)}%
\fi%
)";
//    // this is one way to fix the font IDs for CM fonts to values matching their order when they are preloaded by plain TeX
//    // samples should then use just CM fonts (no diacritics)
//    // [another option is to install exactly the same set of CM/CS/CMCYR fonts on all platforms]
//    // sadly, this fails in pre-2018 pdftex, see also https://mailman.ntg.nl/pipermail/ntg-pdftex/2018-January/004209.html
//    // pdftex: ../../../texk/web2c/pdftexdir/writefont.c:607: create_fontdictionary: Assertion `fo->last_char >= fo->first_char' failed.
//    // Aborted (core dumped)
//
//    PDFRES << "% the following hack initializes all fonts at the base size to avoid getting a different \\pdffontname\n";
//    for (auto & s: {"rm","it","bf","ss","si"}) {
//      PDFRES << "  \\" << s;
//      for (auto & f: FONTS) PDFRES << "\\edef\\tmp{\\pdffontname\\thf" << u2str(f.id+1) << "}";
//      PDFRES << "%\n";
//    }
  } else {
    PDFRES << "\\pdfinfo{/Creator (Therion " << THVERSION << ", MetaPost, TeX)}%" << std::endl;
  }
  PDFRES << "\\pdfcatalog{ /ViewerPreferences << /DisplayDocTitle true /PrintScaling /None >> }" << std::endl;

  if(ENC_NEW.NFSS != 0) PDFRES << "\\input thfonts.map" << std::endl;

  PDFRES << "\\ifnum\\pdftexversion>139" << std::endl;
  PDFRES << "  \\newread\\testin" << std::endl;
  PDFRES << "  \\openin\\testin=glyphtounicode.tex" << std::endl;
  PDFRES << "  \\ifeof\\testin\\message{No glyph to unicode mapping found!}\\else\\closein\\testin\\input glyphtounicode.tex\\pdfgentounicode=1\\fi" << std::endl;
  PDFRES << "\\fi" << std::endl;

  if (LAYOUT.transparency) {
    PDFRES << "\\opacity{" << LAYOUT.opacity << "}%" << std::endl;
    PDFRES << "\\surfaceopacity{" << LAYOUT.surface_opacity << "}%" << std::endl;
    PDFRES << "\\immediate\\pdfobj{ <<\n" <<
           " /GS0 << /Type /ExtGState /ca 1 /BM /Normal >>\n" <<
           " /GS1 << /Type /ExtGState /ca \\the\\opacity\\space /BM /Normal >>\n" <<
           " /GS2 << /Type /ExtGState /ca \\the\\surfaceopacity\\space /BM /Normal >>\n";
    for (int i = 0; i <= 100; i+=LAYOUT.alpha_step)
      PDFRES << fmt::format(" /GSa{:d} << /Type /ExtGState /ca {} /BM /Normal >>\n", i, thdouble(i/100.0, 2));
    PDFRES << ">> }" << std::endl;
    PDFRES << "\\newcount\\resid\\resid=\\pdflastobj" << std::endl;
    PDFRES << "\\immediate\\pdfobj{ << /S /Transparency /K true >> }" << std::endl;
    PDFRES << "\\newcount\\attrid\\attrid=\\pdflastobj" << std::endl;
  }
  else {
    PDFRES << "\\immediate\\pdfobj{ <<\n" <<
           " /GS0 << /Type /ExtGState >>\n" <<
           " /GS1 << /Type /ExtGState >>\n" <<
           " /GS2 << /Type /ExtGState >>\n";
    for (int i = 0; i <= 100; i+=LAYOUT.alpha_step)
      PDFRES << fmt::format(" /GSa{:d} << /Type /ExtGState >>\n", i);
    PDFRES << ">> }" << std::endl;
    PDFRES << "\\newcount\\resid\\resid=\\pdflastobj" << std::endl;
  }

  if (LAYOUT.OCG) {
    PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" << 
      utf2texhex(std::string(thT("title preview above",LAYOUT.lang))) << 
      "> >> }" << std::endl;
    PDFRES << "\\newcount\\ocU\\ocU=\\pdflastobj" << std::endl;
    PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" <<
      utf2texhex(std::string(thT("title preview below",LAYOUT.lang))) << 
      "> >> }" << std::endl;
    PDFRES << "\\newcount\\ocD\\ocD=\\pdflastobj" << std::endl;
    PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" <<
      utf2texhex(std::string(thT("title surface bitmap",LAYOUT.lang))) << 
      "> >> }" << std::endl;
    PDFRES << "\\newcount\\ocSUR\\ocSUR=\\pdflastobj" << std::endl;
    if (mode == MAP) {
      for (std::map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                          I != LAYERHASH.end(); I++) {
        if (I->second.Z == 0) {
          PDFRES << "\\immediate\\pdfobj{ << /Type /OCG /Name <feff" <<
            utf2texhex(I->second.N) << "> >> }" << std::endl;
          PDFRES << "\\newcount\\oc" << u2str(I->first) << "\\oc" << 
                     u2str(I->first) << "=\\pdflastobj" << std::endl;
        }
      }
    }
    PDFRES << "\\pdfcatalog{ /OCProperties <<" << std::endl <<
              "  /OCGs [";
    if (LAYOUT.surface == 2) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "\\the\\ocU\\space0 R ";
    if (mode == MAP) {
      for (std::map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                          I != LAYERHASH.end(); I++) {
        if (I->second.Z == 0) 
          PDFRES << "\\the\\oc" << u2str(I->first) << "\\space 0 R ";
      }
    }
    PDFRES << "\\the\\ocD\\space0 R ";
    if (LAYOUT.surface == 1) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "]" << std::endl <<
              "  /D << /Name (Map layers) /ListMode /VisiblePages" << 
                     " /Order [";
    if (LAYOUT.surface == 2) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "\\the\\ocU\\space0 R ";
    if (mode == MAP) {
      for (std::map<int,layerrecord>::reverse_iterator I = LAYERHASH.rbegin();
                                          I != LAYERHASH.rend(); I++) {
        if (I->second.Z == 0) 
          PDFRES << "\\the\\oc" << u2str(I->first) << "\\space 0 R ";
      }
    }
    PDFRES << "\\the\\ocD\\space0 R ";
    if (LAYOUT.surface == 1) PDFRES << "\\the\\ocSUR\\space0 R "; 
    PDFRES << "] >>" << std::endl << ">> }" << std::endl;
  }

  if (LAYOUT.doc_author != "") 
    PDFRES << "\\pdfinfo{ /Author <feff" << utf2texhex(LAYOUT.doc_author) << ">}" << std::endl;
  if (LAYOUT.doc_subject != "") 
    PDFRES << "\\pdfinfo{ /Subject <feff" << utf2texhex(LAYOUT.doc_subject) << ">}" << std::endl;
  if (LAYOUT.doc_keywords != "") 
    PDFRES << "\\pdfinfo{ /Keywords <feff" << utf2texhex(LAYOUT.doc_keywords) << ">}" << std::endl;
  if (LAYOUT.doc_title != "") {
    PDFRES << "\\pdfinfo{ /Title <feff" << utf2texhex(LAYOUT.doc_title) << ">}" << std::endl;
//    PDFRES << "\\legendcavename={" << utf2tex(LAYOUT.doc_title) << "}" << std::endl;
  }
//  if (LAYOUT.doc_comment != "") {
//    PDFRES << "\\legendcomment={" << utf2tex(LAYOUT.doc_comment) << "}" << std::endl;
//  }

  if (!LEGENDLIST.empty()) {  // zmenit test na LAYOUT.legend???
    PDFRES << "\\legendtrue" << std::endl;
  }
  else {
    PDFRES << "\\legendfalse" << std::endl;
  }
  
  if (!COLORLEGENDLIST.empty()) {  
    PDFRES << "\\colorlegendtrue" << std::endl;
  }
  else {
    PDFRES << "\\colorlegendfalse" << std::endl;
  }

  if (LAYOUT.altitudebar != "") PDFRES << "\\altitudebartrue\n";
  else PDFRES << "\\altitudebarfalse\n";

  PDFRES << "\\legendwidth=" << LAYOUT.legend_width << "bp" << std::endl;

  if (LAYOUT.map_header_bg) {
    PDFRES << "\\bgcolor={" << LAYOUT.col_background.to_pdfliteral(fillstroke::fill) << "}" << std::endl;
    PDFRES << "\\legendbgfilltrue" << std::endl;
  } 
  else PDFRES << "\\legendbgfillfalse" << std::endl;

  if (LAYOUT.icc_profile_cmyk != "") {
    icc_check_file(LAYOUT.icc_profile_cmyk, "CMYK");
    PDFRES << "\\immediate\\pdfobj stream attr {/N 4 /Alternate /DeviceCMYK} file {" << LAYOUT.icc_profile_cmyk << "}" << std::endl;
    PDFRES << "\\immediate\\pdfobj{[/ICCBased \\the\\pdflastobj\\space 0 R]}" << std::endl;
    PDFRES << "\\newcount\\iccobjcmyk\\iccobjcmyk=\\the\\pdflastobj" << std::endl;
  }
  if (LAYOUT.icc_profile_rgb != "") {
    icc_check_file(LAYOUT.icc_profile_rgb, "RGB ");
    PDFRES << "\\immediate\\pdfobj stream attr {/N 3 /Alternate /DeviceRGB} file {" << LAYOUT.icc_profile_rgb << "}" << std::endl;
    PDFRES << "\\immediate\\pdfobj{[/ICCBased \\the\\pdflastobj\\space 0 R]}" << std::endl;
    PDFRES << "\\newcount\\iccobjrgb\\iccobjrgb=\\the\\pdflastobj" << std::endl;
  }
  if (LAYOUT.icc_profile_gray != "") {
    icc_check_file(LAYOUT.icc_profile_gray, "GRAY");
    PDFRES << "\\immediate\\pdfobj stream attr {/N 1 /Alternate /DeviceGray} file {" << LAYOUT.icc_profile_gray << "}" << std::endl;
    PDFRES << "\\immediate\\pdfobj{[/ICCBased \\the\\pdflastobj\\space 0 R]}" << std::endl;
    PDFRES << "\\newcount\\iccobjgray\\iccobjgray=\\the\\pdflastobj" << std::endl;
  }
  PDFRES << "\\edef\\colorres{";
  if (icc_used()) PDFRES << "/ColorSpace <<" << icc2pdfresources() << ">>";
  PDFRES << "}" << std::endl;

  // jednorazove vlozenie povrchovych obrazkov
  int i = 1;
  for (std::list<surfpictrecord>::iterator I = SURFPICTLIST.begin();
                                      I != SURFPICTLIST.end(); I++) {
    PAGEDEF << "\\pdfximage{" << (std::string) I->filename << "}%" << std::endl;
    PAGEDEF << "\\newcount\\" << tex_BMPname(u2str(i)) << "\\" <<
               tex_BMPname(u2str(i)) << "=\\pdflastximage%" << std::endl;
    i++;
  }

  double origMINX=0, origMINY=0, origMAXX=0, origMAXY=0;

  if (mode == ATLAS) {
    HS = LAYOUT.hsize + 2*LAYOUT.overlap;
    VS = LAYOUT.vsize + 2*LAYOUT.overlap;
    if (LAYOUT.page_numbering) {
      PAGE << "\\pagenumberingtrue" << std::endl;
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
    PAGEDEF << "\\eject" << std::endl;
    PAGEDEF << "\\hsize=" << fmt::format("{}",thdouble(HS,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\vsize=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\pdfpagewidth=\\hsize\\advance\\pdfpagewidth by " <<
                                             fmt::format("{}",thdouble(2*LAYOUT.overlap,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\pdfpageheight=\\vsize\\advance\\pdfpageheight by " <<
                                             fmt::format("{}",thdouble(2*LAYOUT.overlap,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\hoffset=0cm" << std::endl;
    PAGEDEF << "\\voffset=0cm" << std::endl;
    PAGEDEF << "\\pdfhorigin=" << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\pdfvorigin=" << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << "bp" << std::endl;
  }
  
  if (mode == ATLAS) {
    PAGEDEF << "\\newdimen\\overlap\\overlap=" << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << "bp" << std::endl;
    for (std::list<sheetrecord>::iterator I = SHEET.begin(); 
                                     I != SHEET.end(); I++) {

//      cout << "ID: " << I->id << " Layer: " << I->layer << " X: " << 
//           I->namex << " Y: " << I->namey << std::endl;

// cout << "*" << flush; 

      PAGEDEF << "\\setbox\\xxx=\\hbox to "<< fmt::format("{}",thdouble(HS,prec_xy)) << "bp{%" << std::endl;

      print_map(I->layer, PAGEDEF, I);
      print_margins(PAGEDEF);

      PAGEDEF << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
      PAGEDEF << "\\immediate\\pdfxform";
      if (icc_used())  PAGEDEF << " resources{/ColorSpace <<" << icc2pdfresources() << ">>} ";
      PAGEDEF << "\\xxx\\newcount\\" << tex_Sname(u2str(I->id)) <<
             " \\" << tex_Sname(u2str(I->id)) << "=\\pdflastxform" << std::endl;

      print_navigator(PAGEDEF,I);
      compose_page(I, PAGE);
    }
    if (icc_used()) {
      PDFRES << "\\edef\\thpdfpageres {/ColorSpace <<" << icc2pdfresources() << ">>}";
      PDFRES << "\\pdfpageresources\\expandafter{\\thpdfpageres}" << std::endl;
    }
  }
  else {
    PAGEDEF << "\\newdimen\\x \\x=" << fmt::format("{}",thdouble(HS,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\newdimen\\y \\y=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp" << std::endl;
    PAGEDEF << "\\setbox\\xxx=\\hbox to \\x{\\maplayout\\hfill}%\\dp\\xxx=0bp" << std::endl;
    PAGEDEF << "\\advance\\x by \\extraE" << std::endl;
    PAGEDEF << "\\advance\\x by \\extraW" << std::endl;
    PAGEDEF << "\\advance\\y by \\extraN" << std::endl;
    PAGEDEF << "\\advance\\y by \\extraS" << std::endl;
    PAGEDEF << "\\newbox\\xxxx\\setbox\\xxxx=\\hbox to \\x{\\kern\\extraW\\raise\\extraS\\box\\xxx\\hss}%\\dp\\xxx=0bp" << std::endl;
    PAGEDEF << "\\wd\\xxxx=\\x" << std::endl;
    PAGEDEF << "\\ht\\xxxx=\\y" << std::endl;
    PAGEDEF << "\\immediate\\pdfxform";
    if (icc_used())  PAGEDEF << " resources{/ColorSpace <<" << icc2pdfresources() << ">>} ";
    PAGEDEF << "\\xxxx" << std::endl;
    PAGEDEF << "\\newcount\\THmaplegend\\THmaplegend=\\pdflastxform" << std::endl;

    PAGEDEF << "\\advance\\pdfhorigin by \\extraW" << std::endl;
    PAGEDEF << "\\advance\\pdfvorigin by \\extraN" << std::endl;
    PAGEDEF << "\\advance\\pdfpagewidth by \\extraW" << std::endl;
    PAGEDEF << "\\advance\\pdfpagewidth by \\extraE" << std::endl;
    PAGEDEF << "\\advance\\pdfpageheight by \\extraN" << std::endl;
    PAGEDEF << "\\advance\\pdfpageheight by \\extraS" << std::endl;

    PAGEDEF << "\\newdimen\\overlap\\overlap=" << fmt::format("{}",thdouble(LAYOUT.overlap,prec_xy)) << "bp" << std::endl;

    PAGEDEF << "\\dimtobp{\\the\\pdfpagewidth}\\edef\\xsize{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\dimtobp{\\the\\pdfpageheight}\\edef\\ysize{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\advance\\y by -\\extraN\\advance\\y by \\overlap" << std::endl;
    PAGEDEF << "\\dimtobp{\\the\\y}\\edef\\nsize{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\x=\\extraW\\advance\\x by \\overlap" << std::endl;
    PAGEDEF << "\\dimtobp{\\the\\x}\\edef\\wsize{\\tmpdef}%" << std::endl;

    PAGEDEF << "\\newdimen\\overlaphalf\\overlaphalf=\\overlap\\divide\\overlaphalf by 2%" << std::endl;
    PAGEDEF << "\\newdimen\\framew\\framew=\\pdfpagewidth\\advance\\framew by -\\overlap" << std::endl;
    PAGEDEF << "\\newdimen\\frameh\\frameh=\\pdfpageheight\\advance\\frameh by -\\overlap" << std::endl;
    PAGEDEF << "\\newdimen\\framex\\framex=\\extraW\\advance\\framex by \\overlaphalf" << std::endl;
    PAGEDEF << "\\newdimen\\framey\\framey=\\extraN\\advance\\framey by \\overlaphalf" << std::endl;

    PAGEDEF << "\\dimtobp{\\framew}\\edef\\Framew{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\dimtobp{\\frameh}\\edef\\Frameh{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\dimtobp{\\framex}\\edef\\Framex{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\dimtobp{\\framey}\\edef\\Framey{\\tmpdef}%" << std::endl;


    PAGEDEF << "\\adjustedHS=" << fmt::format("{}",thdouble(HS,prec_xy)) << "bp" <<
      "\\advance\\adjustedHS by \\extraE" << 
      "\\advance\\adjustedHS by \\extraW" <<
      "\\advance\\adjustedHS by \\overlap" << 
      "\\advance\\adjustedHS by \\overlap" <<  std::endl;

    PAGEDEF << "\\adjustedVS=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp" <<
      "\\advance\\adjustedVS by \\extraN" << 
      "\\advance\\adjustedVS by \\extraS" << 
      "\\advance\\adjustedVS by \\overlap" << 
      "\\advance\\adjustedVS by \\overlap" << std::endl;

    //calibration
    //PAGEDEF.precision(10);
    for (int i=0; i<9; i++) {
      PAGEDEF << "\\calibrX{" << fmt::format("{}",thdouble(LAYOUT.calibration_local[i].x - MINX,10)) << "bp}\\tmpdimenX=\\tmpdimen" << std::endl;
      PAGEDEF << "\\calibrY{" << fmt::format("{}",thdouble(LAYOUT.calibration_local[i].y - MINY,10)) << "bp}\\tmpdimenY=\\tmpdimen" << std::endl;
    
      PAGEDEF << "\\pdfcatalog { /thCalibrate" << i << 
                 " (X=\\the\\tmpdimenX, Y=\\the\\tmpdimenY, L=" << fmt::format("{}",thdouble(LAYOUT.calibration_latlong[i].x,10)) <<
                 ", F=" << fmt::format("{}",thdouble(LAYOUT.calibration_latlong[i].y,10)) << ")}" << std::endl;
    }
    PAGEDEF << "\\pdfcatalog { /thCalibrate (HS=\\the\\adjustedHS, VS=\\the\\adjustedVS, HD=" <<
               fmt::format("{}",thdouble(LAYOUT.calibration_hdist,10)) << ")}";
    //PAGEDEF.precision(2);

    PAGEDEF << "\\tmpdimen=\\extraW\\advance\\tmpdimen by \\overlap" << std::endl;
    PAGEDEF << "\\dimtobp{\\tmpdimen}\\edef\\adjustedX{\\tmpdef}%" << std::endl;
    PAGEDEF << "\\tmpdimen=\\extraS\\advance\\tmpdimen by \\overlap" << std::endl;
    PAGEDEF << "\\dimtobp{\\tmpdimen}\\edef\\adjustedY{\\tmpdef}%" << std::endl;

    if (! LAYOUT.transparent_map_bg) {
        PAGEDEF << "\\PL{q " << LAYOUT.col_background.to_pdfliteral(fillstroke::fill) << " -" <<
			    "\\wsize\\space"  << "-" << 
			    "\\nsize\\space" << 
			    "\\xsize\\space" << 
			    "\\ysize\\space" << 
                            " re f Q}%" << std::endl;
    }
    PAGEDEF << "\\ifdim\\framethickness>0mm\\dimtobp{\\framethickness}\\edef\\Framethickness{\\tmpdef}" <<
               "\\PL{q " << black2pdf(1, fillstroke::stroke) << " 1 J 1 j \\Framethickness\\space w " << 
               "-\\Framex\\space\\Framey\\space\\Framew\\space-\\Frameh\\space" << 
               " re s Q}\\fi" << std::endl;


//    PAGEDEF << "\\leavevmode\\setbox\\xxx=\\hbox to " << HS << "bp{%" << std::endl;
    PAGEDEF << "\\leavevmode\\setbox\\xxx=\\hbox to 0bp{%" << std::endl;

    if (LAYOUT.surface == 1) print_surface_bitmaps(PAGEDEF,MINX,MINY);

//    print_page_bg(PAGEDEF);
    for (std::map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                        I != LAYERHASH.end(); I++) {
      if (I->second.Z == 0) {
        if (LAYOUT.OCG && LAYOUT.transparency) {
          PAGEDEF << "\\PL{/OC /oc\\the\\oc" << u2str(I->first) << "\\space BDC}%" << std::endl;
        }
        print_page_bg_scraps(I->first,PAGEDEF);
        if (LAYOUT.OCG && LAYOUT.transparency) {
          PAGEDEF << "\\PL{EMC}%" << std::endl;
        }
      }
    }

    if (LAYOUT.grid == 1) print_grid_pdf(PAGEDEF,MINX,MINY,origMINX,origMINY,origMAXX,origMAXY);

    if (!MAP_PREVIEW_DOWN.empty()) print_preview(0,PAGEDEF,MINX,MINY);
    for (std::map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                        I != LAYERHASH.end(); I++) {
      if (I->second.Z == 0) {
        PAGEDEF << "\\setbox\\xxx=\\hbox to " << fmt::format("{}",thdouble(HS,prec_xy)) << "bp{%" << std::endl;
                            // we need flush layer data using XObject 
                            // (the text clipping path may become too large)

        print_map((*I).first,PAGEDEF);

        PAGEDEF << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
        PAGEDEF << "\\immediate\\pdfxform ";
        if (LAYOUT.OCG) {
          PAGEDEF << "attr{/OC \\the\\oc" << u2str(I->first) << "\\space 0 R} ";
        }
        if (icc_used())  PAGEDEF << "resources{/ColorSpace <<" << icc2pdfresources() << ">>} ";
        PAGEDEF << "\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;
      }
    }
    if (!MAP_PREVIEW_UP.empty()) print_preview(1,PAGEDEF,MINX,MINY);

    if (LAYOUT.surface == 2) print_surface_bitmaps(PAGEDEF,MINX,MINY);
    if (LAYOUT.grid == 2) print_grid_pdf(PAGEDEF,MINX,MINY,origMINX,origMINY,origMAXX,origMAXY);

    if (LAYOUT.map_grid) {
      PAGEDEF << "\\PL{q .4 w " << black2pdf(0.4, fillstroke::fillstroke) << " }%" << std::endl;
      PAGEDEF << "\\PL{0 0 " << fmt::format("{}",thdouble(HS,prec_xy)) << " " << fmt::format("{}",thdouble(VS,prec_xy)) << " re S}%" << std::endl;
      for (double i=0; i <= HS; i += LAYOUT.hsize) {
        PAGEDEF << "\\PL{" << fmt::format("{}",thdouble(i,prec_xy)) << " 0 m " << fmt::format("{}",thdouble(i,prec_xy)) << " " <<
                   fmt::format("{}",thdouble(VS,prec_xy)) << " l S}%" << std::endl;
	if (i<HS) {
          PAGEDEF << "\\PL{q 1 0 0 1 " << fmt::format("{}",thdouble(i+LAYOUT.hsize/2,prec_xy)) <<
	    " 0 cm}\\gridcoord{8}{\\size[24]" << grid_name(LAYOUT.labelx,round((MINX+i)/LAYOUT.hsize)) <<
	    "}\\PL{Q}%" << std::endl;
          PAGEDEF << "\\PL{q 1 0 0 1 " << fmt::format("{}",thdouble(i+LAYOUT.hsize/2,prec_xy)) << " " << fmt::format("{}",thdouble(VS,prec_xy)) <<
	    " cm}\\gridcoord{2}{\\size[24]" << grid_name(LAYOUT.labelx,round((MINX+i)/LAYOUT.hsize)) <<
	    "}\\PL{Q}%" << std::endl;
	}
      }
      for (double i=0; i <= VS; i += LAYOUT.vsize) {
        PAGEDEF << "\\PL{0 " << fmt::format("{}",thdouble(i,prec_xy)) << " m " << fmt::format("{}",thdouble(HS,prec_xy)) << " " <<
                   fmt::format("{}",thdouble(i,prec_xy)) << " l S}%" << std::endl;
	if (i<VS) {
          PAGEDEF << "\\PL{q 1 0 0 1 0 " << fmt::format("{}",thdouble(i+LAYOUT.vsize/2,prec_xy)) <<
    	    " cm}\\gridcoord{6}{\\size[24]" << grid_name(LAYOUT.labely,round((MINY+VS-i)/LAYOUT.vsize)-1) <<
	    "}\\PL{Q}%" << std::endl;
          PAGEDEF << "\\PL{q 1 0 0 1 " << fmt::format("{}",thdouble(HS,prec_xy)) << " " << fmt::format("{}",thdouble(i+LAYOUT.vsize/2,prec_xy)) <<
	    " cm}\\gridcoord{4}{\\size[24]" << grid_name(LAYOUT.labely,round((MINY+VS-i)/LAYOUT.vsize)-1) <<
	    "}\\PL{Q}%" << std::endl;
	}
      }
      PAGEDEF << "\\PL{Q}%" << std::endl;
    }
////    PAGEDEF << "\\setbox\\xxx=\\hbox to " << HS << "bp{";      // map legend
////    PAGEDEF << "\\maplayout\\hfill}\\ht\\xxx=" << VS << "bp\\dp\\xxx=0bp" << std::endl;
////    PAGEDEF << "\\immediate\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;

    PAGEDEF << "\\hfill}\\ht\\xxx=" << fmt::format("{}",thdouble(VS,prec_xy)) << "bp\\dp\\xxx=0bp" << std::endl;
//    PAGEDEF << "\\immediate\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}%" << std::endl;
    PAGEDEF << "\\box\\xxx%" << std::endl;
    
    PAGEDEF << "\\smash{\\rlap{\\kern-\\extraW\\raise-\\extraS" << 
               "\\hbox{\\pdfrefxform\\THmaplegend}}}" << std::endl;

    if ((LAYOUT.OCG && LAYOUT.transparency) || icc_used()) {
      PAGEDEF << "\\edef\\thpdfpageres {";
      if (LAYOUT.OCG && LAYOUT.transparency) {
        PAGEDEF << " /Properties << ";
        for (std::map<int,layerrecord>::iterator I = LAYERHASH.begin();
                                            I != LAYERHASH.end(); I++) {
          if (I->second.Z == 0)
            PAGEDEF << "/oc\\the\\oc" << u2str(I->first) << "\\space\\the\\oc" <<
                       u2str(I->first) << "\\space0 R ";
        }
        PAGEDEF << " >> ";
      }
      if (icc_used()) {
        PAGEDEF << " /ColorSpace <<" << icc2pdfresources() << ">> ";
      }
      PAGEDEF << "}" << std::endl << "\\pdfpageresources\\expandafter{\\thpdfpageres}" << std::endl;
    }
  }

  PDFRES.close();
  PAGEDEF.close();
  PAGE.close();
}


int thpdf(int m) {
  mode = m;

  thprintf("making %s ... ", (mode == ATLAS) ? "atlas" : "map");

  SHEET.clear();
  SHEET_JMP.clear();
  MINX=DBL_MAX, MINY=DBL_MAX, MAXX=-DBL_MAX, MAXY=-DBL_MAX;

  make_sheets();
  if (mode == ATLAS) {
    sort_sheets();
    find_jumps();
  }
  build_pages();

  thprintf("done\n");

  return(0);
}

