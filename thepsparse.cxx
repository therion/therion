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
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>

#include <cstring>
#include <cstdio>
#include <cmath>
#include <cassert>

#include "thepsparse.h"
#include "thpdfdbg.h"
#include "thpdfdata.h"
#include "thtexfonts.h"
#include "thconvert.h"

using namespace std;


extern map<string,string> RGB, ALL_FONTS, ALL_PATTERNS;
typedef set<unsigned char> FONTCHARS;
extern map<string,FONTCHARS> USED_CHARS;

extern unsigned font_id, patt_id;

////////////////////////////////////////////////////////////////////////////////

CGS::CGS () {
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
  
//  clippathID = random();
//  clippath = false;
}

int CGS::clippathID = 0;

string CGS::svg_color() {
  char ch[8];
  sprintf(ch,"#%02x%02x%02x",int(255*color[0]) % 256,
                             int(255*color[1]) % 256,
                             int(255*color[2]) % 256);
  return (string) ch;
}

MP_data::MP_data () {
  idx = 0;
}

void MP_transform::clear () {
  command = MP_notransf;
}

MP_transform::MP_transform () {
  clear();
}

void MP_transform::set(int T, string s1, string s2,double dx, double dy) {
  command = T;
  transf[0] = atof(s1.c_str())-dx;
  transf[1] = atof(s2.c_str())-dy;
}

void MP_transform::set(int T, string s1, string s2, string s3,
                              string s4, string s5, string s6,
                              double dx, double dy) {
  command = T;
  transf[0] = atof(s1.c_str());
  transf[1] = atof(s2.c_str());
  transf[2] = atof(s3.c_str());
  transf[3] = atof(s4.c_str());
  transf[4] = atof(s5.c_str())-dx;
  transf[5] = atof(s6.c_str())-dy;
}

void MP_path::clear() {
  segments.clear();
  closed=false;
  fillstroke = -1;
  transformation.clear();
}

MP_path::MP_path() {
  clear();
}

void MP_path::add(int command, string s1, string s2, double dx, double dy) {
  MP_path_segment seg;
  seg.command = command;
  seg.coord[0] = atof(s1.c_str())-dx;
  seg.coord[1] = atof(s2.c_str())-dy;
  segments.push_back(seg);
}

void MP_path::add(int command, string s1, string s2, string s3, 
                               string s4, string s5, string s6,
                               double dx, double dy) {
  MP_path_segment seg;
  seg.command = command;
  seg.coord[0] = atof(s1.c_str())-dx;
  seg.coord[1] = atof(s2.c_str())-dy;
  seg.coord[2] = atof(s3.c_str())-dx;
  seg.coord[3] = atof(s4.c_str())-dy;
  seg.coord[4] = atof(s5.c_str())-dx;
  seg.coord[5] = atof(s6.c_str())-dy;
  segments.push_back(seg);
}

void MP_path::print_svg(ofstream & F, CGS & gstate) {
  if (fillstroke == MP_clip) {
    CGS::clippathID++;
    gstate.clippathdepth.insert(make_pair(CGS::clippathID,0));
    F << "<clipPath id=\"clip_" << CGS::clippathID << "\">" << endl << "  ";
  }
  F << "<path ";
  if (fillstroke != MP_clip) {
    F << "fill=\"" << (fillstroke==MP_fill || fillstroke==MP_fillstroke ? 
           gstate.svg_color() : "none") <<  
         "\" stroke=\"" << (fillstroke==MP_stroke || fillstroke==MP_fillstroke ? 
           gstate.svg_color() : "none") <<  "\" ";
    if (fillstroke==MP_stroke || fillstroke==MP_fillstroke) {
      F << "stroke-width=\"" << gstate.linewidth << "\" ";
      if (gstate.linecap != MP_rounded) {
        F << "stroke-linecap=";
        if (gstate.linecap == MP_butt) F << "\"butt\" ";
        else F << "\"square\" ";
      }
      if (gstate.linejoin != MP_rounded) {
        F << "stroke-linejoin=";
        if (gstate.linecap == MP_mitered) F << "\"miter\" ";
        else F << "\"bevel\" ";
      }
      if (gstate.miterlimit != 10) F << "stroke-miterlimit=\"" << 
          gstate.miterlimit << "\" ";
    }
  }
  F << "d=\"";
  for (unsigned i=0; i < segments.size(); i++) {
    switch (segments[i].command) {
      case MP_moveto:
        F << "M" << segments[i].coord[0] << " " << -segments[i].coord[1];
        break;
      case MP_lineto:
        F << "L" << segments[i].coord[0] << " " << -segments[i].coord[1];
        break;
      case MP_rlineto:
        F << "l" << segments[i].coord[0] << " " << -segments[i].coord[1];
        break;
      case MP_curveto:
        F << "C" << segments[i].coord[0] << " " << -segments[i].coord[1] << " "
                 << segments[i].coord[2] << " " << -segments[i].coord[3] << " " 
                 << segments[i].coord[4] << " " << -segments[i].coord[5];
        break;
    }
  }
  if (closed) F << "Z";
  F << "\" />" << endl;
  if (fillstroke == MP_clip) F << "</clipPath>" << endl << 
     "<g clip-path=\"url(#clip_" << CGS::clippathID << ")\">" << endl;
}

void MP_data::add(int i) {
  MP_index ind;
  ind.vector = I_gsave;
  ind.idx = i;
  index.push_back(ind);
}

void MP_data::add(int i, string s) {
  MP_index ind;
  MP_setting set;
  ind.vector = I_setting;
  ind.idx = settings.size();
  
  if (i == MP_dash) {
    set.str = s;
  }
  else if (i == MP_rgb) {
    istringstream ss(s); // prerobit!!!
    ss >> set.data[0];
    ss >> set.data[1];
    ss >> set.data[2];
  }
  else {
    set.data[0] = atof(s.c_str());
  }
  set.command = i;
  settings.push_back(set);
  index.push_back(ind);
}

void MP_data::add(MP_path P) {
  MP_index ind;
  ind.vector = I_path;
  ind.idx = paths.size();
  paths.push_back(P);
  index.push_back(ind);
}

void MP_data::add(MP_transform T) {
  MP_index ind;
  ind.vector = I_transform;
  ind.idx = transforms.size();
  transforms.push_back(T);
  index.push_back(ind);
}

void MP_data::clear() {
  index.clear();
  paths.clear();
  texts.clear();
  settings.clear();
  transforms.clear();
  idx = 0;
}

void MP_setting::print_svg (ofstream & F, CGS & gstate) {
  switch (command) {
    case MP_rgb:
      for (int i=0; i<3; i++) gstate.color[i] = data[i];
      break;
    case MP_gray:
      for (int i=0; i<3; i++) gstate.color[i] = data[0];
      break;
    case MP_linejoin:
      gstate.linejoin = int(data[0]);
      break;
    case MP_linecap:
      gstate.linecap = int(data[0]);
      break;
    case MP_miterlimit:
      gstate.miterlimit = data[0];
      break;
    case MP_linewidth:
      gstate.linewidth = data[0];
      break;
  }
}

void MP_data::print_svg (ofstream & F, string ID) {
  F << "<g id=\"" << ID <<  // plain MP settings follow
       "\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-miterlimit=\"10\">" << endl;
  for (unsigned int i=0; i<index.size(); i++) {
    switch (index[i].vector) {
      case I_path:
        paths[index[i].idx].print_svg(F,gstate);
        break;
      case I_setting:
        settings[index[i].idx].print_svg(F,gstate);
        break;
      case I_gsave:
        switch (index[i].idx) {
          case MP_gsave:
            for (map<int,int>::iterator I = gstate.clippathdepth.begin();
                                        I!= gstate.clippathdepth.end(); I++) 
              I->second++;
            F << "<g>" << endl;
            break;
          case MP_grestore:
            for (map<int,int>::iterator I = gstate.clippathdepth.begin();
                                        I!= gstate.clippathdepth.end(); I++) {
              I->second--;
              if (I->second < 0) F << "</g>" << endl;
            }
            // nemoze ist do predch. cyklu, lebo zmazanie smernika
            // urobi chaos
            for (map<int,int>::iterator I = gstate.clippathdepth.begin();
                                        I!= gstate.clippathdepth.end(); I++) {
              if (I->second < 0) gstate.clippathdepth.erase(I);
            }
            F << "</g>" << endl;
            break;
          case MP_transp_on:
            
            break;
          case MP_transp_off:
            
            break;
        }
        break;
    }
  }
  F << "</g>" << endl;
  
  assert(gstate.clippathdepth.empty());
}

void converted_data::clear() {
  MP.clear();
  fonts.clear();
}

converted_data::converted_data() {
  clear();
}

void parse_eps(string fname, string cname, double dx, double dy, 
               double & c1, double & c2, double & c3, double & c4, 
               converted_data & data) {
  string tok, buffer;
  string font, patt;
  bool comment = true, concat = false, 
       already_transp = false, transp_used = false;
  double llx = 0, lly = 0, urx = 0, ury = 0, HS = 0.0, VS = 0.0;
  deque<string> thbuffer;
  set<string> FORM_FONTS, FORM_PATTERNS;
  bool inpath = false, gsaveinpath = false;
  
  MP_path mp_path;
  MP_transform mp_trans, fntmatr;

  ifstream F(fname.c_str());
  if(!F) therror(("Can't open file for reading"));
  while(F >> tok) {
    if (comment) {                      // File header
      if (tok == "%%BoundingBox:") {
        F >> llx >> lly >> urx >> ury;

        c1 = llx+dx;
	c2 = lly+dy;
	c3 = urx+dx;
	c4 = ury+dy;	
                
	HS = urx - llx;
	VS = ury - lly;
	if (cname != "") { // beginning of boundary cl.path definition
                           // for F and G scraps
          data.MP.add(MP_gsave);
          ifstream G(cname.c_str());
          if(!G) therror(("Can't open file"));
          mp_path.clear();
          while(G >> buffer) {
            if (buffer == "m") {
              mp_path.add(MP_moveto, thbuffer[0],thbuffer[1],llx,lly);
              thbuffer.clear();
            }
            else if (buffer == "c") {
              mp_path.add(MP_curveto, thbuffer[0], thbuffer[1], thbuffer[2], 
                          thbuffer[3], thbuffer[4], thbuffer[5],llx,lly);
              thbuffer.clear();
            }
            else if (buffer == "l") {
              mp_path.add(MP_lineto, thbuffer[0],thbuffer[1],llx,lly);
              thbuffer.clear();
            }
            else {
              thbuffer.push_back(buffer);
            }
          }
          mp_path.fillstroke = MP_clip;
          data.MP.add(mp_path);
          thbuffer.clear();
          G.close();
	}
      }
      else if (tok == "%%Page:") {
        F >> tok; F >> tok;
        comment = false;
      }
    }
    else {                              // PostScript commands
      if (tok == "showpage") {
        break;
      }
      else if (tok == "moveto") {
        mp_path.add(MP_moveto, thbuffer[0],thbuffer[1],llx,lly);
        thbuffer.clear();
      }
      else if (tok == "curveto") {
        mp_path.add(MP_curveto, thbuffer[0], thbuffer[1], thbuffer[2], 
                    thbuffer[3], thbuffer[4], thbuffer[5],llx,lly);
        thbuffer.clear();
      }
      else if (tok == "lineto") {
        mp_path.add(MP_lineto, thbuffer[0],thbuffer[1],llx,lly);
        thbuffer.clear();
      }
      else if (tok == "rlineto") {
        mp_path.add(MP_rlineto, thbuffer[0],thbuffer[1],0,0);
        thbuffer.clear();
      }
      else if (tok == "newpath") {
        inpath = true;
        mp_path.clear();
        thbuffer.clear();
      }
      else if (tok == "closepath") {
        mp_path.closed = true;
        thbuffer.clear();
      }
      else if (tok == "fill") {
        if (!gsaveinpath) {
          mp_path.fillstroke = MP_fill;
          data.MP.add(mp_path);
          inpath=false;
        }
        thbuffer.clear();
      }
      else if (tok == "stroke") {
        if (!gsaveinpath) mp_path.fillstroke = MP_stroke;
        else {
          mp_path.fillstroke = MP_fillstroke;
          gsaveinpath = false;
        }
        data.MP.add(mp_path);
        inpath=false;
        thbuffer.clear();
      }
      else if (tok == "clip") {
        mp_path.fillstroke = MP_clip;
        data.MP.add(mp_path);
        inpath=false;
        thbuffer.clear();
      }
      else if (tok == "setlinejoin") {
        data.MP.add(MP_linejoin, thbuffer[0]);
        thbuffer.clear();
      }
      else if (tok == "setlinecap") {
        data.MP.add(MP_linecap, thbuffer[0]);
        thbuffer.clear();
      }
      else if (tok == "setmiterlimit") {
        data.MP.add(MP_miterlimit, thbuffer[0]);
        thbuffer.clear();
      }
      else if (tok == "setgray") {
        if (already_transp) {  // transp off
          data.MP.add(MP_transp_off);
          already_transp = false;
        }
        data.MP.add(MP_gray, thbuffer[0]);
        thbuffer.clear();
      }
      else if (tok == "setrgbcolor") {
        if ((!((thbuffer[0] == "0.00002") && (thbuffer[1] == "0.00018"))) 
              && already_transp) {           // transp off
          data.MP.add(MP_transp_off);
          already_transp = false;
        };
        if (thbuffer[0] == "0.00002") {        // special commands
          if (thbuffer[1] == "0.00015") {          // patterns
            patt = thbuffer[2];
            if (FORM_PATTERNS.find(patt) == FORM_PATTERNS.end()) {
              FORM_PATTERNS.insert(patt);
            }
            if (ALL_PATTERNS.find(patt) == ALL_PATTERNS.end()) {
              ALL_PATTERNS.insert(make_pair(patt,u2str(patt_id)));
              patt_id++;
            }
            data.MP.add(MP_pattern, patt);
          }
          else if (thbuffer[1] == "0.00018") {     // transparency
            transp_used = true;
            if (!already_transp) {
              data.MP.add(MP_transp_on);
              already_transp = true;
            }
            map<string,string>::iterator I = RGB.find(thbuffer[2]);
            if (I != RGB.end()) {
              data.MP.add(MP_rgb, I->second);
            } else cerr << "Unknown color!" << endl;
          }
          else cerr << "Unknown special!" << endl;
	}
	else {                               // regular RGB color
          data.MP.add(MP_rgb, thbuffer[0]+" "+thbuffer[1]+" "+thbuffer[2]);
	}
        thbuffer.clear();
      }
      else if (tok == "setdash") {
        buffer = "";
        for(unsigned i=0; i<thbuffer.size(); i++) {
	  buffer = buffer + thbuffer[i] + " ";
	}
        data.MP.add(MP_dash, buffer);
        thbuffer.clear();
      }
      else if (tok == "setlinewidth") {
        if(thbuffer[0] != "0") {
	  buffer = thbuffer[0];
	}
	else {
	  buffer = thbuffer[1];
	  F >> tok; // redundant pop
	}
        data.MP.add(MP_linewidth, buffer);
        thbuffer.clear();
      }
      else if (tok == "gsave") {
        if (!inpath) data.MP.add(MP_gsave);
        else gsaveinpath = true;
        thbuffer.clear();
      }
      else if (tok == "grestore") {
        if (!inpath) data.MP.add(MP_grestore);
        thbuffer.clear();
      }
      else if (tok == "translate") {
        mp_trans.set(MP_translate,thbuffer[0],thbuffer[1],llx,lly);
        data.MP.add(mp_trans);
        thbuffer.clear();
      }
      else if (tok == "scale") {
        mp_trans.set(MP_scale,thbuffer[0],thbuffer[1],0,0);
        if (!inpath) data.MP.add(mp_trans);
        else mp_path.transformation = mp_trans;
        thbuffer.clear();
      }
      else if (tok == "concat") {     
        if (thbuffer[0] != "[") {  // opening bracket
          thbuffer[0].erase(0,1);
        }
        else {
          thbuffer.pop_front();
        }
        mp_trans.set(MP_concat,thbuffer[0],thbuffer[1],thbuffer[2],
                     thbuffer[3],thbuffer[4],thbuffer[5],llx,lly);
        if (!inpath) {
          fntmatr = mp_trans;
          concat = true;
        }
        else mp_path.transformation = mp_trans;
        thbuffer.clear();
      }
      
      // text conversion should be
      // A B moveto (C) D E fshow
      // -> 
      // BT /Fiii E Tf 1 0 0 1 A B Tm (C) Tj ET
      // or
      // gsave [A1 A2 A3 A4 A5 A6 ] concat 0 0 moveto (C) D E fshow grestore
      // ->
      // BT /Fiii E Tf A1 A2 A3 A4 A5 A6 Tm (C) Tj ET
      // 
      // currently we leave moveto, gsave, grestore unchanged;
      // path started with moveto is terminated with the `n' operator
      
      else if (tok == "fshow") {            // font changes should be optimized
/*        unsigned i = thbuffer.size();
        font = thbuffer[i-2];
        if (FORM_FONTS.find(font) == FORM_FONTS.end()) {
          FORM_FONTS.insert(font);
        }
        if (ALL_FONTS.find(font) == ALL_FONTS.end()) {
          ALL_FONTS.insert(make_pair(font,u2str(font_id)));
          font_id++;
        }
//        font = tex_Fname(ALL_FONTS[font]);
        if (USED_CHARS.find(font) == USED_CHARS.end()) {
          FONTCHARS FCH;
          USED_CHARS.insert(make_pair(font,FCH));
        }
        buffer = "";
        for (unsigned j=0; j<i-2; j++) {
          buffer = buffer + thbuffer[j] + " ";
        }
        buffer = process_pdf_string(buffer,font);
        print_str("n BT",TEX); // we end the path started with `x y moveto'
                               // should be done more cleanly
        print_str("/F\\pdffontname\\"+font+"\\space "+thbuffer[i-1]+" Tf",TEX);
        if (concat) {
          print_str(fntmatr+" Tm",TEX);
        }
        else {
          sprintf(x, "%.1f", atof(lastmovex.c_str())-llx);  // modify this part
          sprintf(y, "%.1f", atof(lastmovey.c_str())-lly);
          print_str("1 0 0 1 "+string(x)+" "+string(y)+" Tm",TEX);
        }
        TEX << "\\PL{" << buffer << " Tj}%" << endl;
        print_str("ET",TEX);
        concat = false;
      */
        thbuffer.clear();
      }
      else {
        thbuffer.push_back(tok);
      }
    }
  }  // end of while loop
  F.close();
  if (cname != "") { // end of boundary cl.path
    data.MP.add(MP_grestore);
  }
  data.fonts = FORM_FONTS;
  data.patterns = FORM_PATTERNS;
  if (transp_used) data.transparency = true;
  else data.transparency = false;
}

void convert_scraps_new() {
/*  converted_data dataconv;
  list<converted_data> convlist;
  double llx,lly,urx,ury;
convlist.push_back(dataconv);
dataconv.clear();*/
  
  for(list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    if (I->F != "") parse_eps(I->F, I->C, I->S1, I->S2, I->F1, I->F2, I->F3, I->F4, I->Fc);
    if (I->G != "") parse_eps(I->G, I->C, I->S1, I->S2, I->G1, I->G2, I->G3, I->G4, I->Gc);
    if (I->B != "") parse_eps(I->B, "", I->S1, I->S2, I->B1, I->B2, I->G3, I->B4, I->Bc);
    if (I->I != "") parse_eps(I->I, "", I->S1, I->S2, I->I1, I->I2, I->I3, I->I4, I->Ic);
    if (I->E != "") parse_eps(I->E, "", I->S1, I->S2, I->E1, I->E2, I->E3, I->E4, I->Ec);
    if (I->X != "") parse_eps(I->X, "", I->S1, I->S2, I->X1, I->X2, I->X3, I->X4, I->Xc);
  }


}



int thconvert_new() { 

  thprintf("converting scraps 2 ... ");
  
  RGB.clear();
  ALL_FONTS.clear();
  ALL_PATTERNS.clear();
  USED_CHARS.clear();
  font_id = 1;
  patt_id = 1;

  read_rgb();
  convert_scraps_new();

//  thpdfdbg();  // in the debugging mode only
  
  thprintf("done\n");
  return(0);
}


