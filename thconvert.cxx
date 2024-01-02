/**
 * @file thconvert.cxx
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
#include <fstream>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <fmt/core.h>

#include <cstring>
#include <cstdio>
#include <cmath>

#include "therion.h"
#include "thpdfdbg.h"
#include "thpdfdata.h"
#include "thtexfonts.h"

// extern list<scraprecord> SCRAPLIST;

#define IOerr(F) fmt::format("Can't open file {}!\n", (F)).c_str()

std::map<std::string,std::string> ALL_FONTS, ALL_PATTERNS;
typedef std::set<unsigned char> FONTCHARS;
std::map<std::string,FONTCHARS> USED_CHARS;

unsigned font_id, patt_id;
int convert_mode;

////////////////////////////////////////////////////////////////////////////////

std::list<scraprecord>::iterator find_scrap(std::string name) {
    std::list<scraprecord>::iterator I;
    for (I = SCRAPLIST.begin(); I != SCRAPLIST.end(); I++) {
      if (I->name == name) break;
    }
    if (I == SCRAPLIST.end()) {
      std::cerr << "This can't happen!" << std::endl;
    }
    return (I);
}

void print_queue(std::deque<std::string>& thstack, double llx, double lly, 
                std::string command, std::ofstream& TEX) {
  if (convert_mode>0) {TEX << "\\PL{";}
  for(unsigned i=0; i<thstack.size(); i=i+2) {
    TEX << atof(thstack[i].c_str())-llx << " " << 
           atof(thstack[i+1].c_str())-lly << " ";
  }
  TEX << command;
  if (convert_mode>0) {TEX << "}%";}
  TEX << "\n";
}

void print_str(std::string str, std::ofstream& TEX) {
  if (convert_mode>0) {TEX << "\\PL{";}
  TEX << str;
  if (convert_mode>0) {TEX << "}%";}
  TEX << "\n";
}


std::string process_pdf_string(std::string s, std::string font) {
  std::string r,t;
  unsigned char c;
  char *err;
  unsigned j;

  auto& I = USED_CHARS.at(font); // must be there
  s = s.substr(1,s.length()-3);  // delete surrounding parentheses and final space
  for (unsigned i=0; i<s.size(); i++) {
    c = s[i];
    if (c == 92) {
      i++;
      c = s[i];
      if (c == 92 || c == 40 || c == 41) {     // escape sequences \\, \(, \)
        r += c;
      }
      else if (c>=48 && c<=57) {
        j = i+1;
        t = c;
        while((c=s[j])>=48 && c<=57 && j<i+3) {   // octal numbers
          t += s[j];
          j++;
        }
        i = j-1;
        c = strtol(t.c_str(),&err,8);
        r += c;
      }
      else i--;                  // otherwise backslash is ignored
    }
    else {
      r += c;
    }
  }
  t = "";
  for (unsigned i=0; i<r.size(); i++) {
    c = r[i];
    I.insert(c);
    t += fmt::format("{:02x}",c);
  }
  return "<" + t + ">";
}


// convert modes:  0 -- patterns
//                10 -- scrap content which shoul be clipped
//                11 -- background
//                12 -- outline
//                13 -- filled outline
//                20 -- nonclipped scrap data
//                30 -- legend
//                31 -- northarrow, scalebar

void distill_eps(std::string name, std::string fname, std::string cname, int mode, std::ofstream& TEX, color col = color()) {
  std::string form_id;
  std::string tok, lastmovex, lastmovey, buffer;
  std::string font, patt, fntmatr;
  std::string pattcolor = "/CS2 cs 0 0 0 1";
  bool comment = true, concat = false, 
       already_transp = false, transp_used = false, before_group_transp = false, cancel_transp = true;
  double llx = 0, lly = 0, urx = 0, ury = 0, HS = 0.0, VS = 0.0;
  double dx, dy;
  std::deque<std::string> thstack;
  std::set<std::string> FORM_FONTS, FORM_PATTERNS;
  std::list<scraprecord>::iterator J;
  
  convert_mode = mode;
  
  std::ostringstream text_attr;
  if (LAYOUT.colored_text && col.is_defined()) {
    text_attr << "0.1 w " << col.to_pdfliteral() << " 2 Tr ";
  };

  std::ifstream F(fname.c_str());
  if(!F) therror((IOerr(fname)));
  while(F >> tok) {
    if (comment) {                      // File header
      if ((tok == "%%BoundingBox:") && (mode > 0)) {
        F >> llx >> lly >> urx >> ury;
	
        if ((mode>0) && (mode<30)) {
          J = find_scrap(name);
          dx = J->S1;
          dy = J->S2;
          if (J->name == "") J->name = name;
          if (mode == 10) {
            J->F1 = llx+dx;
            J->F2 = lly+dy;
            J->F3 = urx+dx;
            J->F4 = ury+dy;
            form_id = tex_Xname(name);
          }
          else if (mode == 11) {
            J->G1 = llx+dx;
            J->G2 = lly+dy;
            J->G3 = urx+dx;
            J->G4 = ury+dy;
            form_id = tex_Xname("G"+name);
          }
          else if (mode == 12) {
            J->B1 = llx+dx;
            J->B2 = lly+dy;
            J->B3 = urx+dx;
            J->B4 = ury+dy;
            form_id = tex_Xname("B"+name);
          }
          else if (mode == 13) {
            J->I1 = llx+dx;
            J->I2 = lly+dy;
            J->I3 = urx+dx;
            J->I4 = ury+dy;
            form_id = tex_Xname("I"+name);
          }
          else if (mode == 14) {
            J->E1 = llx+dx;
            J->E2 = lly+dy;
            J->E3 = urx+dx;
            J->E4 = ury+dy;
            form_id = tex_Xname("E"+name);
          }
          else if (mode == 20) {
            J->X1 = llx+dx;
            J->X2 = lly+dy;
            J->X3 = urx+dx;
            J->X4 = ury+dy;
            form_id = tex_Xname("X"+name);
          }
          else std::cerr << "Unknown mode!" << std::endl; 
        }
        else if (mode == 30) {
          form_id = tex_Lname(name);
        }
        else if (mode == 31) {
          form_id = tex_Wname(name);
        }
        else if (mode > 100 && mode < 110) {
          form_id = tex_Wname(name);
	  LAYOUT.gridcell[mode - 101].x = llx;
	  LAYOUT.gridcell[mode - 101].y = lly;
        }
        
	HS = urx - llx;
	VS = ury - lly;
        TEX << "%\n\\setbox\\xxx=\\hbox{\\vbox to" << VS << "bp{\\vfill" << std::endl;
	if ((mode <= 11) && (cname != "")) { // beginning of boundary cl.path definition
	  TEX << "\\PL{q}%\n";          // for F and G scraps
          std::ifstream G(cname.c_str());
          if(!G) therror((IOerr(cname)));
          while(G >> buffer) {
            if ((buffer == "m") || (buffer == "l") || (buffer == "c")) {
              print_queue(thstack,llx,lly,buffer,TEX);
              thstack.clear();
            }
            else {
              thstack.push_back(buffer);
            }
          }
          G.close();
          thstack.clear();
          TEX << "\\PL{W* n}%\n";  // end of boundary clipping path definition
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
        lastmovex = thstack[0];
        lastmovey = thstack[1];
        print_queue(thstack,llx,lly,"m",TEX);
        thstack.clear();
      }
      else if (tok == "curveto") {
        print_queue(thstack,llx,lly,"c",TEX);
        thstack.clear();
      }
      else if (tok == "lineto") {
        print_queue(thstack,llx,lly,"l",TEX);
        thstack.clear();
      }
      else if (tok == "rlineto") {
        thstack.clear();
        thstack.push_back(lastmovex);
        thstack.push_back(lastmovey);
        print_queue(thstack,llx,lly,"l",TEX);
        thstack.clear();
      }
      else if (tok == "newpath") {
        thstack.clear();
      }
      else if (tok == "closepath") {
        print_str("h",TEX);
        thstack.clear();
      }
      else if (tok == "fill") {
        print_str("f*",TEX);
        thstack.clear();
      }
      else if (tok == "stroke") {
        print_str("S",TEX);
        thstack.clear();
      }
      else if (tok == "clip") {
        print_str("W* n",TEX);
        thstack.clear();
      }
      else if (tok == "setlinejoin") {
        print_str(thstack[0]+" j",TEX);
        thstack.clear();
      }
      else if (tok == "setlinecap") {
        print_str(thstack[0]+" J",TEX);
        thstack.clear();
      }
      else if (tok == "setmiterlimit") {
        print_str(thstack[0]+" M",TEX);
        thstack.clear();
      }
      else if (tok == "setgray") {
        if (mode == 0) {
          thstack.clear();
          continue;            // ignore color for uncolored patterns
        }
        if (already_transp && cancel_transp) {  // transp off
          print_str("/GS0 gs",TEX);
          already_transp = false;
        }
        cancel_transp = true;
        print_str(thstack[0]+" g "+thstack[0]+" G",TEX);
        thstack.clear();
      }
      else if (tok == "setrgbcolor") {
        if (mode == 0) {
          thstack.clear();
          continue;            // ignore color for uncolored patterns
        }
        if (already_transp && cancel_transp) {           // transp off
          print_str("/GS0 gs",TEX);
          already_transp = false;
        };
        cancel_transp = true;
        print_str(thstack[0]+" "+thstack[1]+" "+thstack[2]+" rg "
                 +thstack[0]+" "+thstack[1]+" "+thstack[2]+" RG",TEX);
        thstack.clear();
      }
      else if (tok == "setcmykcolor") {
        if (mode == 0) {
          thstack.clear();
          continue;            // ignore color for uncolored patterns
        }
        if (already_transp && cancel_transp) {           // transp off
          print_str("/GS0 gs",TEX);
          already_transp = false;
        }
        cancel_transp = true;
        print_str(thstack[0]+" "+thstack[1]+" "+thstack[2]+" "+thstack[3]+" k "
                 +thstack[0]+" "+thstack[1]+" "+thstack[2]+" "+thstack[3]+" K",TEX);
        thstack.clear();
      }
      else if (tok == "THsetpattern") {
        if (already_transp && cancel_transp) {  // transp off
          print_str("/GS0 gs",TEX);
          already_transp = false;
        }
        cancel_transp = true;
        patt = thstack[0];
        if (FORM_PATTERNS.find(patt) == FORM_PATTERNS.end()) {
          FORM_PATTERNS.insert(patt);
        }
        if (ALL_PATTERNS.find(patt) == ALL_PATTERNS.end()) {
          ALL_PATTERNS.insert(make_pair(patt,u2str(patt_id)));
          patt_id++;
        }
        print_str(pattcolor+" /"+patt+" scn",TEX);
        thstack.clear();
      }
      else if (tok == "THsetalpha") {
        transp_used = true;
        if (!already_transp) {
          print_str("/GS1 gs",TEX);
          already_transp = true;
          cancel_transp = false;
        }
        thstack.clear();
      }
      else if (tok == "setdash") {
        buffer = "";
        for(unsigned i=0; i<thstack.size(); i++) {
	  buffer = buffer + thstack[i] + " ";
	}
	print_str(buffer+"d",TEX);
        thstack.clear();
      }
      else if (tok == "setlinewidth") {
        if(thstack[0] != "0") {
	  buffer = thstack[0];
	}
	else {
	  buffer = thstack[1];
	  F >> tok; // redundant pop
	}
	print_str(buffer+" w",TEX);
        thstack.clear();
      }
      else if (tok == "gsave") {
        print_str("q",TEX);
        thstack.clear();
        if (already_transp) before_group_transp = true;
        else before_group_transp = false;
      }
      else if (tok == "grestore") {
        print_str("Q",TEX);
        thstack.clear();
        if (before_group_transp) already_transp = true;
        else already_transp = false;
      }
      else if (tok == "translate") {
        print_str("1 0 0 1 "+thstack[0]+" "+thstack[1]+" cm",TEX);
        thstack.clear();
      }
      else if (tok == "scale") {
        print_str(thstack[0]+" 0 0 "+thstack[1]+" 0 0 cm",TEX);
        thstack.clear();
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
        unsigned i = thstack.size();
        font = thstack[i-2];
        if (FORM_FONTS.find(font) == FORM_FONTS.end()) {
          FORM_FONTS.insert(font);
        }
        if (ALL_FONTS.find(font) == ALL_FONTS.end()) {
          ALL_FONTS.insert(make_pair(font,u2str(font_id)));
          font_id++;
        }
        font = tex_Fname(ALL_FONTS[font]);
        if (USED_CHARS.find(font) == USED_CHARS.end()) {
          FONTCHARS FCH;
          USED_CHARS.insert(make_pair(font,FCH));
        }
        buffer = "";
        for (unsigned j=0; j<i-2; j++) {
          buffer = buffer + thstack[j] + " ";
        }
        buffer = process_pdf_string(buffer,font);
        print_str("n BT",TEX); // we end the path started with `x y moveto'
                               // should be done more cleanly
        print_str("/F\\pdffontname\\"+font+"\\space "+thstack[i-1]+" Tf",TEX);
        if (concat) {
          print_str(fntmatr+" Tm",TEX);
        }
        else {
          // modify this part
          print_str("1 0 0 1 " +
                    fmt::format("{:.1f}", atof(lastmovex.c_str())-llx) + " " +
                    fmt::format("{:.1f}", atof(lastmovey.c_str())-lly) + " Tm", TEX);
        }
	print_str(text_attr.str(),TEX);
        TEX << "\\PL{" << buffer << " Tj}%" << std::endl;
        print_str("ET",TEX);
        concat = false;
        thstack.clear();
      }
      else if (tok == "concat") {      // only when applied to texts
        if (thstack[0] != "[") {  // opening bracket
          thstack[0].erase(0,1);
        }
        else {
          thstack.pop_front();
        }
        // modify this part
        fntmatr = thstack[0] + " " + thstack[1] + " " + thstack[2] +
                  " " + thstack[3] + " " +
                  fmt::format("{:.1f}", atof(thstack[4].c_str())-llx) + " " +
                  fmt::format("{:.1f}", atof(thstack[5].c_str())-lly);
        concat = true;
        thstack.clear();
      }
      else if (tok == "THsetpatterncolor") {
        if (thstack.size() == 1)
          pattcolor = "/CS3 cs " + thstack[0];
        else if (thstack.size() == 3)
          pattcolor = "/CS1 cs " + thstack[0] + " " + thstack[1] + " " + thstack[2];
        else if (thstack.size() == 4)
          pattcolor = "/CS2 cs " + thstack[0] + " " + thstack[1] + " " + thstack[2] + " " + thstack[3];
        thstack.clear();
      }
      else {
        thstack.push_back(tok);
      }
    }
  }  // end of while loop
  F.close();
  if (mode>0) {
    if ((mode <= 11) && (cname != "")) { // end of boundary cl.path
      TEX << "\\PL{Q}%" << std::endl;
    }
    TEX << "}}\\wd\\xxx=" << HS << "bp" << std::endl;
    TEX << "\\immediate\\pdfxform";

//    if (mode == 12 || mode == 13) {
//      TEX << " attr {";
//      if (mode == 12) {
//        TEX << "/OC \\the\\ocU\\space 0 R ";
//      }
//      else if (mode == 13) {
//        TEX << "/OC \\the\\ocD\\space 0 R ";
//      }
//      TEX << "} ";
//    }

    if (transp_used || !FORM_FONTS.empty() || !FORM_PATTERNS.empty() || icc_used()) {
      TEX << " resources { /ProcSet [/PDF /Text] ";
      if (transp_used) {
        TEX << "/ExtGState \\the\\resid\\space 0 R ";
      }
      if (!FORM_FONTS.empty()) {
        TEX << "/Font << ";
        for(std::set<std::string>::iterator I = FORM_FONTS.begin(); 
                                  I != FORM_FONTS.end(); I++) {
          font = tex_Fname(ALL_FONTS[*I]);
          TEX << "/F\\pdffontname\\" << font << 
                 "\\space\\pdffontobjnum\\" << font << "\\space 0 R ";
        }
        TEX << ">> ";
      }
      if (!FORM_PATTERNS.empty()) {
        TEX << "/Pattern << ";
        for(std::set<std::string>::iterator I = FORM_PATTERNS.begin(); 
                                    I != FORM_PATTERNS.end(); I++) {
          TEX << "/" << *I << " \\the\\" << tex_Pname(ALL_PATTERNS[*I]) << 
                 "\\space 0 R ";
        }
        TEX << ">> ";
      }
      if (!FORM_PATTERNS.empty() || icc_used()) {
        TEX << "/ColorSpace <<";
        if (!FORM_PATTERNS.empty())
          TEX << " /CS1 [/Pattern /DeviceRGB] /CS2 [/Pattern /DeviceCMYK] /CS3 [/Pattern /DeviceGray] ";
        if (icc_used())
          TEX << icc2pdfresources();
        TEX << ">> ";
      }
      TEX << "} ";
    }
    TEX << "\\xxx\n" << tex_set_ref(form_id,"\\pdflastxform") << std::endl;
  }
}


void convert_scraps() {
  unsigned char c;
 
  std::ofstream TEX("th_formdef_.tex");
  if(!TEX) therror((IOerr("th_formdef_.tex")));
  TEX.setf(std::ios::fixed, std::ios::floatfield);
  TEX.precision(2);
  
  for(std::list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
//    cout << "*" << flush;
    if (I->F != "") distill_eps(I->name, I->F, I->C, 10, TEX, I->col_scrap);
    if (I->G != "") distill_eps(I->name, I->G, I->C, 11, TEX);
    if (I->B != "") distill_eps(I->name, I->B, "", 12, TEX);
    if (I->I != "") distill_eps(I->name, I->I, "", 13, TEX);
    if (I->E != "") distill_eps(I->name, I->E, "", 14, TEX, I->col_scrap);
    if (I->X != "") distill_eps(I->name, I->X, "", 20, TEX, I->col_scrap);
  }

  // similarly with legend (distill_eps( , , , 30, TEX))
  for(std::list<legendrecord>::iterator I = LEGENDLIST.begin(); 
                                  I != LEGENDLIST.end(); I++) {
    if (I->fname != "") distill_eps(I->name, I->fname, "", 30, TEX);
  }

  // north arrow &c.
  if (LAYOUT.northarrow != "") 
             distill_eps("northarrow", LAYOUT.northarrow, "", 31, TEX);
  if (LAYOUT.scalebar != "") 
             distill_eps("scalebar", LAYOUT.scalebar, "", 31, TEX);

  if (LAYOUT.grid > 0) {
    distill_eps("grida", LAYOUT.gridAA, "", 101, TEX);
    distill_eps("gridb", LAYOUT.gridAB, "", 102, TEX);
    distill_eps("gridc", LAYOUT.gridAC, "", 103, TEX);
    distill_eps("gridd", LAYOUT.gridBA, "", 104, TEX);
    distill_eps("gride", LAYOUT.gridBB, "", 105, TEX);
    distill_eps("gridf", LAYOUT.gridBC, "", 106, TEX);
    distill_eps("gridg", LAYOUT.gridCA, "", 107, TEX);
    distill_eps("gridh", LAYOUT.gridCB, "", 108, TEX);
    distill_eps("gridi", LAYOUT.gridCC, "", 109, TEX);
  }

  TEX.close();

  std::ofstream F("th_fontdef_.tex");
  if(!F) therror((IOerr("th_fontdef_.tex")));
  F << "% FONTS:" << std::endl;
  F.setf(std::ios::fixed, std::ios::floatfield);
  F.precision(2);
  for (std::map<std::string,std::string>::iterator I = ALL_FONTS.begin(); 
                                    I != ALL_FONTS.end(); I++) {
    F << "\\font\\" << tex_Fname((*I).second) << "=" << (*I).first << std::endl;

  }
  F << "\\begingroup" << std::endl           // make special characters normal
    << "\\catcode`\\^^@=12\\catcode`\\^^?=12\\catcode`\\{=12" << std::endl
    << "\\catcode`\\}=12\\catcode`\\$=12\\catcode`\\&=12" << std::endl
    << "\\catcode`\\#=12\\catcode`\\_=12\\catcode`\\~=12" << std::endl
    << "\\catcode`\\%=12" << std::endl
    << "\\catcode`\\^^L=12\\catcode`\\^^A=12\\catcode`\\^^K=12\\catcode`\\^^I=12" << std::endl
    << "\\catcode`\\^^M=12" << std::endl;   // na tomto riadku ma tex este stare catcode konca riadku,
                                       // vsetko nasledovne musi byt v jednom riadku
  for (std::map<std::string,FONTCHARS>::iterator I = USED_CHARS.begin(); 
                                       I != USED_CHARS.end(); I++) {
    F << "\\includechars\\" << (*I).first << ":";
    for (FONTCHARS::iterator J = ((*I).second).begin();
                             J != ((*I).second).end(); J++) {
      c = *J;
      if (c > 31 && c < 128) {
//        if (c==37) F << "\\";    // % remains a comment
        F << c;
        if (c==92) F << " ";     // \ has to be followed by space
      } else {
        F << "^^" << fmt::format("{:02x}", c);
      }
    }
    F << "\\endinclude";
  }
  F << "\\endgroup" << std::endl;
  F << "% PATTERNS:" << std::endl;
  std::ifstream P("patterns.dat");
  if(!P) therror((IOerr("patterns.dat")));
  char buf[5000];
  char delim[] = ":";
  std::string line,num,pfile,bbox,xstep,ystep,matr;
  while(P.getline(buf,5000,'\n')) {
    num = strtok(buf,delim);
    pfile = strtok(NULL,delim);
    bbox = strtok(NULL,delim);
    xstep = strtok(NULL,delim);
    ystep = strtok(NULL,delim);
    matr = strtok(NULL,delim);
    std::map<std::string,std::string>::iterator I = ALL_PATTERNS.find(num);
    if (I != ALL_PATTERNS.end()) {
      F << "\\immediate\\pdfobj stream attr {/Type /Pattern" << std::endl;
      F << "/PaintType 2 /PatternType 1 /TilingType 1" << std::endl;
      F << "/Matrix " << matr << std::endl;
      F << "/BBox " << bbox << std::endl;
      F << "/XStep " << xstep << std::endl;
      F << "/YStep " << ystep << std::endl;
      F << "/Resources << /ProcSet [/PDF ] ";
      if (icc_used()) F << " /ColorSpace <<" << icc2pdfresources() << ">> ";
      F << ">>} {" << std::endl;
      distill_eps("", pfile , "", 0, F);
      F << "} \\newcount \\" << tex_Pname((*I).second) << 
           "\\" << tex_Pname((*I).second) << "=\\pdflastobj" << std::endl;
    }
  }
  P.close();
  F.close();

  std::vector<std::string> legend_arr_n, legend_arr_d;
  for(std::list<legendrecord>::iterator I = LEGENDLIST.begin(); 
                                   I != LEGENDLIST.end(); I++) {
    legend_arr_n.push_back(I->name);
    legend_arr_d.push_back(I->descr);
  }
  std::ofstream LEG("th_legend_.tex");
  if(!LEG) therror(("Can't write a file!"));
/*  for(list<legendrecord>::iterator I = LEGENDLIST.begin(); 
                                   I != LEGENDLIST.end(); I++) {
    LEG << "\\legendsymbolbox{" << tex_get_ref(tex_Lname(I->name)) << "}{" <<
                               utf2tex(I->descr) << "}" << endl;
  } */
  int legendbox_num = LEGENDLIST.size();
  int columns = LAYOUT.legend_columns; 
  int rows = (int) ceil(double(legendbox_num) / columns);
  int pos = 0;
  
  LEG << "\\legendcolumns" << columns << std::endl;

  for (int i = 0; i < rows; i++) {
    LEG << "\\line{%" << std::endl;
    for (int j = 0; j < columns; j++) {
      pos = i + j * rows;
      if (pos < legendbox_num) 
        LEG << "  \\legendsymbolbox{" << tex_get_ref(tex_Lname(legend_arr_n[pos])) <<
               "}{" << utf2tex(legend_arr_d[pos]) << "}\\hskip10pt" << std::endl;
    }
    LEG << "\\hss}" << std::endl;
  }

  LEG.close();

  std::vector<colorlegendrecord> legend_color;
  colorlegendrecord lcr;
  for(std::list<colorlegendrecord>::iterator I = COLORLEGENDLIST.begin(); 
                                   I != COLORLEGENDLIST.end(); I++) {
    lcr.col_legend = I->col_legend;
    lcr.texname = I->texname;
    legend_color.push_back(lcr);
  }
  std::ofstream LEGCOLOR("th_legendcolor_.tex");
  if(!LEGCOLOR) therror(("Can't write a file!"));

  legendbox_num = COLORLEGENDLIST.size();
  columns = 1;
  rows = (int) ceil(double(legendbox_num) / columns);
//  pos = 0;
  
  LEGCOLOR << "\\legendcolumns" << columns << std::endl;

  for (int i = 0; i < rows; i++) {
    LEGCOLOR << "\\line{%" << std::endl;
    for (int j = 0; j < columns; j++) {
      pos = i + j * rows;
      if (pos < legendbox_num) {
        LEGCOLOR << "  \\colorlegendbox{";
        if (LAYOUT.transparency) LEGCOLOR << "/GS1 gs ";   // colorlegendbox argument is enclosed in q ... Q
        LEGCOLOR << legend_color[pos].col_legend.to_pdfliteral(fillstroke::fill) << "}%" << std::endl;
        LEGCOLOR << "  \\legendsymbolbox{\\pdflastxform}{" <<
	legend_color[pos].texname << "}\\hskip10pt" << std::endl;
      }
    }
    LEGCOLOR << "\\hss}" << std::endl;
  }

  LEGCOLOR.close();

}

int thconvert_old() {

  thprintf("converting scraps* ... ");

  ALL_FONTS.clear();
  ALL_PATTERNS.clear();
  USED_CHARS.clear();
  font_id = 1;
  patt_id = 1;

  convert_scraps();

  thprintf("done\n");

  return(0);
}

