/**
 * @file thepsparse.cxx
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
#include <iomanip>

#include <cstring>
#include <cstdio>
#include <cmath>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "thepsparse.h"
#include "thpdfdbg.h"
#include "thpdfdata.h"
#include "thtexfonts.h"
#include "therion.h"
#include "thdouble.h"

#define IOerr(F) ((std::string)"Can't open file "+F+"!\n").c_str()

extern std::map<std::string,std::string> ALL_FONTS, ALL_PATTERNS;
typedef std::set<unsigned char> FONTCHARS;
extern std::map<std::string,FONTCHARS> USED_CHARS;
std::list<pattern> PATTERNLIST;
std::map<std::string,gradient> GRADIENTS;
std::list<converted_data> GRIDLIST;
converted_data NArrow, ScBar, AltBar;

extern unsigned font_id, patt_id;
static int conv_mode;

const int prec_col = 5;
const int prec_matr = 6;
const int prec_mp = 5;
const int prec_xy = 2;

////////////////////////////////////////////////////////////////////////////////

void color::reset() {
  model = colormodel::no;
  a = b = c = d = -1;
}

void color::set(double a) {
  model = colormodel::grey;
  this->a = a;
}

void color::set(double a, double b, double c) {
  if (LAYOUT.output_colormodel != colormodel::grey) {
    model = colormodel::rgb;
    this->a = a;
    this->b = b;
    this->c = c;
  } else {  // convert to gray in grayscale output
    model = colormodel::grey;
    this->a = 0.3*a + 0.59*b + 0.11*c;   // see PDF Reference, section 6.2
  }
}

void color::set(double a, double b, double c, double d) {
  if (LAYOUT.output_colormodel != colormodel::grey) {
    model = colormodel::cmyk;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
  } else {  // convert to gray in grayscale output
    model = colormodel::grey;
    this->a = 1.0 - std::min(1.0, 0.3*a + 0.59*b + 0.11*c + d);   // see PDF Reference, section 6.2
  }
}

bool color::is_white() const {
  if ((model == colormodel::grey && a == 1.0) ||
      (model == colormodel::rgb && (a == 1.0 && b == 1.0 && c == 1.0)) ||
      (model == colormodel::cmyk && (a == 0 && b == 0 && c == 0 && d == 0))) return true;
  else return false;
}

bool color::is_defined() const {
  return model != colormodel::no;
}

std::string color::to_svg() const {
  double r = 0, g = 0, b = 0;
  if (model == colormodel::grey)
    r = g = b = this->a;
  else if (model == colormodel::rgb) {
    r = this->a;
    g = this->b;
    b = this->c;
  }
  else if (model == colormodel::cmyk) {
    r = 1.0 - std::min(1.0, this->a + this->d);
    g = 1.0 - std::min(1.0, this->b + this->d);
    b = 1.0 - std::min(1.0, this->c + this->d);
  }
  else therror(("undefined color used"));
  return fmt::format("#{:02x}{:02x}{:02x}",int(255*r) % 256,
                                           int(255*g) % 256,
                                           int(255*b) % 256);
}

std::string color::to_pdfliteral(fillstroke fs) const {
  std::ostringstream s;
  if (model == colormodel::grey) {
    if (fs == fillstroke::fill || fs == fillstroke::fillstroke)
      s << fmt::format("{}", thdouble(this->a,prec_col)) << " g";
    if (fs == fillstroke::fillstroke)
      s << " ";
    if (fs == fillstroke::stroke || fs == fillstroke::fillstroke)
      s << fmt::format("{}", thdouble(this->a,prec_col)) << " G";
  }
  else if (model == colormodel::rgb) {
    if (fs == fillstroke::fill || fs == fillstroke::fillstroke)
      s << fmt::format("{}", thdouble(this->a,prec_col)) << " " << fmt::format("{}", thdouble(this->b,prec_col)) << " " << fmt::format("{}", thdouble(this->c,prec_col)) << " rg";
    if (fs == fillstroke::fillstroke)
      s << " ";
    if (fs == fillstroke::stroke || fs == fillstroke::fillstroke)
      s << fmt::format("{}", thdouble(this->a,prec_col)) << " " << fmt::format("{}", thdouble(this->b,prec_col)) << " " << fmt::format("{}", thdouble(this->c,prec_col)) << " RG";
  }
  else if (model == colormodel::cmyk) {
    if (fs == fillstroke::fill || fs == fillstroke::fillstroke)
      s << fmt::format("{}", thdouble(this->a,prec_col)) << " " << fmt::format("{}", thdouble(this->b,prec_col)) << " " << fmt::format("{}", thdouble(this->c,prec_col)) <<  " " << fmt::format("{}", thdouble(this->d,prec_col)) << " k";
    if (fs == fillstroke::fillstroke)
      s << " ";
    if (fs == fillstroke::stroke || fs == fillstroke::fillstroke)
      s << fmt::format("{}", thdouble(this->a,prec_col)) << " " << fmt::format("{}", thdouble(this->b,prec_col)) << " " << fmt::format("{}", thdouble(this->c,prec_col)) <<  " " << fmt::format("{}", thdouble(this->d,prec_col)) << " K";
  }
  return s.str();
}


std::string color::to_elements() const {
  std::string s;
  if (model == colormodel::grey) {
    s = fmt::format("{}", thdouble(a,prec_col));
  }
  else if (model == colormodel::rgb) {
    s = fmt::format("{} {} {}", thdouble(a,prec_col), thdouble(b,prec_col), thdouble(c,prec_col));
  }
  else if (model == colormodel::cmyk) {
    s = fmt::format("{} {} {} {}", thdouble(a,prec_col), thdouble(b,prec_col), thdouble(c,prec_col), thdouble(d,prec_col));
  }
  return s;
}


std::string color::to_pdfpatterncolor() const {
  std::string s;
  if (model == colormodel::grey) {
    s = "/CS3 cs";
  }
  else if (model == colormodel::rgb) {
    s = "/CS1 cs";
  }
  else if (model == colormodel::cmyk) {
    s = "/CS2 cs";
  }
  return s + " " + to_elements();
}


CGS::CGS () {

  linejoin = linecap = 1;
//  clippathID = random();
//  clippath = false;
}

int CGS::clippathID = 0;

std::string CGS::svg_color() {
  if (!col.is_defined()) return "inherit";
  return col.to_svg();
}

std::string str2pdfhex(std::string s) {
  std::string t;
  for (unsigned char c: s) {
    t += fmt::format("{:02x}", c);
  }
  return "<" + t + ">";
}

std::string PL(std::string s) {
  std::string t;
  if (conv_mode > 0) t = "\\PL{";
  t += s;
  if (conv_mode > 0) t += "}%";
  return  t + "\n";
}

void MP_text::clear() {
  x = y = 0;
  xx = yy = 1;
  xy = yx = 0;
  size = 0;
  col.reset();
  transformed = false;
}

void MP_text::print_svg(std::ofstream & F, CGS & gstate) {
  F << "<text font-family=\"" << font << "\" font-size=\"" << size << "\" ";
  if (LAYOUT.colored_text && col.is_defined()) F << 
    "fill=\"" << col.to_svg() << "\" " <<   // col = the scrap color
    "stroke=\"black\" stroke-width=\"0.1\" ";
  else F << "fill=\"" << (gstate.col.is_defined() ? gstate.col.to_svg() : "black") <<
         "\" stroke=\"none\" ";
  F << "transform=\"matrix(" << xx << " " << xy << " " << -yx << " " << -yy <<
       " " << x << " " << y << ")\">";
  for (unsigned int i = 0; i < text.size(); i++)
    F << "&#x" << std::hex << tex2uni(font, int(text[i])) << std::dec << ";";
  F << "</text>" << std::endl;
}

void MP_text::print_pdf(std::ofstream & F) const {
  F << PL("BT");
  F << PL(fmt::format("/F\\pdffontname\\{:s}\\space {} Tf", tex_Fname(ALL_FONTS[font]), thdouble(size,prec_mp)));
  F << PL(fmt::format("{} {} {} {} {:.1f} {:.1f} Tm", thdouble(xx,prec_mp), thdouble(xy,prec_mp), thdouble(yx,prec_mp), thdouble(yy,prec_mp), x, y));
  if (LAYOUT.colored_text && col.is_defined()) {    // use the scrap color
    F << PL("0.1 w " + col.to_pdfliteral(fillstroke::fill) + " 2 Tr ");
  };
  F << PL(fmt::format("{:s} Tj", str2pdfhex(text)));
  F << PL("ET");
}

MP_text::MP_text() {
  clear();
}

void MP_transform::clear () {
  command = MP_notransf;
  transf[0] = 1;
  transf[1] = 0;
  transf[2] = 0;
  transf[3] = 1;
  transf[4] = 0;
  transf[5] = 0;
}

MP_transform::MP_transform () {
  clear();
}

void MP_transform::set(int T, std::string s1, std::string s2,double dx, double dy) {
  command = T;
  transf[0] = atof(s1.c_str())-dx;
  transf[1] = atof(s2.c_str())-dy;
}

void MP_transform::set(int T, std::string s1, std::string s2, std::string s3,
                              std::string s4, std::string s5, std::string s6,
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

void MP_path::add(int command, std::string s1, std::string s2, double dx, double dy) {
  MP_path_segment seg;
  seg.command = command;
  seg.coord[0] = atof(s1.c_str())-dx;
  seg.coord[1] = atof(s2.c_str())-dy;
  segments.push_back(seg);
}

void MP_path::add(int command, std::string s1, std::string s2, std::string s3, 
                               std::string s4, std::string s5, std::string s6,
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

void MP_path::print_svg(std::ofstream & F, CGS & gstate, std::string unique_prefix) {
  if (fillstroke == MP_clip) {
    CGS::clippathID++;
    gstate.clippathdepth.insert(std::make_pair(CGS::clippathID,0));
    F << "<clipPath id=\"clip_" << CGS::clippathID << "_" << unique_prefix << "\">" << std::endl << "  ";
  }
  F << "<path ";
  if (fillstroke != MP_clip) {
    F << "fill=\"" << (fillstroke==MP_fill || fillstroke==MP_fillstroke ? 
           (gstate.pattern == "" ? (gstate.gradient=="" ? gstate.svg_color() : "url(#grad_" + gstate.gradient + "_" + unique_prefix + ")") :
              "url(#patt_" + gstate.pattern + "_" + unique_prefix + ")") : "none") <<  
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
        if (gstate.linejoin == MP_mitered) F << "\"miter\" ";
        else F << "\"bevel\" ";
      }
      if (gstate.miterlimit != 10) F << "stroke-miterlimit=\"" << 
          gstate.miterlimit << "\" ";
      if (!gstate.dasharray.empty()) {
        F << "stroke-dasharray=\"";
        unsigned int i = 1;
        for(std::list<float>::iterator I = gstate.dasharray.begin();
                                  I != gstate.dasharray.end(); I++) {
          F << *I;
          if (i++ < gstate.dasharray.size()) F << ",";
        }
        F << "\" stroke-dashoffset=\"" << gstate.dashoffset << "\" ";
      }
    }
  }
  F << "d=\"";
  for (unsigned i=0; i < segments.size(); i++) {
    switch (segments[i].command) {
      case MP_moveto:
        F << "M" << segments[i].coord[0] << " " << segments[i].coord[1];
        break;
      case MP_lineto:
        F << "L" << segments[i].coord[0] << " " << segments[i].coord[1];
        break;
      case MP_rlineto:
        if (segments[i].coord[0] == 0 && segments[i].coord[1] == 0)
          F << "h0.001";  // rlineto 0 0 used for dot rendering doesn't work in SVG
	else
          F << "l" << segments[i].coord[0] << " " << segments[i].coord[1];
        break;
      case MP_curveto:
        F << "C" << segments[i].coord[0] << " " << segments[i].coord[1] << " "
                 << segments[i].coord[2] << " " << segments[i].coord[3] << " " 
                 << segments[i].coord[4] << " " << segments[i].coord[5];
        break;
    }
  }
  if (closed) F << "Z";
  F << "\" />" << std::endl;
  if (fillstroke == MP_clip) F << "</clipPath>" << std::endl << 
     "<g clip-path=\"url(#clip_" << CGS::clippathID << "_" << unique_prefix << ")\">" << std::endl;
}

void MP_path::print_pdf(std::ofstream & F) const {
  double last_x = 0, last_y = 0;
  for (unsigned i=0; i < segments.size(); i++) {
    switch (segments[i].command) {
      case MP_moveto:
        F << PL(fmt::format("{:.2f} {:.2f} m", segments[i].coord[0], segments[i].coord[1]));
        last_x = segments[i].coord[0];
        last_y = segments[i].coord[1];
        break;
      case MP_lineto:
        F << PL(fmt::format("{:.2f} {:.2f} l", segments[i].coord[0], segments[i].coord[1]));
        break;
      case MP_rlineto:
        F << PL(fmt::format("{:.2f} {:.2f} l", segments[i].coord[0]+last_x, segments[i].coord[1]+last_y));
        break;
      case MP_curveto:
        F << PL(fmt::format("{:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} c",
                  segments[i].coord[0], segments[i].coord[1],
                  segments[i].coord[2], segments[i].coord[3],
                  segments[i].coord[4], segments[i].coord[5]));
        break;
    }
  }
  if (closed) F << PL("h");

  if (fillstroke == MP_fill) F << PL("f*");
  else if (fillstroke == MP_stroke) F << PL("S");
  else if (fillstroke == MP_fillstroke) F << PL("B*");
  else if (fillstroke == MP_clip) F << PL("W* n");
  else therror(("invalid path drawing operation"));
}

void MP_data::add(int i) {
  MP_index ind;
  ind.vector = I_gsave;
  ind.idx = i;
  index.push_back(ind);
}

void MP_data::add(int i, std::string s, color col) {
  MP_index ind;
  MP_setting sett{};
  ind.vector = I_setting;
  ind.idx = settings.size();
  float fl;
  double a, b, c, d;
  
  if (i == MP_dash) {
    s.replace(s.find("["),1,"");
    s.replace(s.find("]"),1,"");
    sett.dasharray.clear();
    std::istringstream ss(s);
    while (ss >> fl) sett.dasharray.push_back(fl);
    sett.dasharray.pop_back();
    sett.dashoffset = fl;
    //set.str = s;
  }
  else if (i == MP_gray) {
    std::istringstream ss(s);
    ss >> a;
    sett.col.set(a);
  }
  else if (i == MP_rgb) {
    std::istringstream ss(s);
    ss >> a >> b >> c;
    sett.col.set(a,b,c);
  }
  else if (i == MP_cmyk) {
    std::istringstream ss(s);
    ss >> a >> b >> c >> d;
    sett.col.set(a,b,c,d);
  }
  else if (i == MP_pattern) {
    sett.pattern = s;
    sett.col = col;
  }
  else if (i == MP_gradient) {
    sett.pattern = s;
  }
  else if (i == MP_transp_on) {
    sett.alpha = std::min(std::stod(s),1.0);  // alpha clipped to 1.0 max; negative values have a special meaning (bg transparency)
  }
  else {
    sett.data = std::stod(s);
  }
  sett.command = i;
  settings.push_back(sett);
  index.push_back(ind);
}

void MP_data::add(const MP_path& P) {
  MP_index ind;
  ind.vector = I_path;
  ind.idx = paths.size();
  paths.push_back(P);
  index.push_back(ind);
}

void MP_data::add(const MP_transform& T) {
  MP_index ind;
  ind.vector = I_transform;
  ind.idx = transforms.size();
  transforms.push_back(T);
  index.push_back(ind);
}

void MP_data::add(const MP_text& T) {
  MP_index ind;
  ind.vector = I_text;
  ind.idx = texts.size();
  texts.push_back(T);
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

void MP_setting::print_svg (std::ofstream & F, CGS & gstate) {
  switch (command) {
    case MP_rgb:
    case MP_gray:
    case MP_cmyk:
      gstate.col = col;
      gstate.pattern = "";
      gstate.gradient = "";
      break;
    case MP_pattern:
      gstate.pattern = pattern;
      gstate.gradient = "";
      break;
    case MP_gradient:
      gstate.gradient = pattern;
      gstate.pattern = "";
      break;
    case MP_linejoin:
      gstate.linejoin = int(data);
      break;
    case MP_linecap:
      gstate.linecap = int(data);
      break;
    case MP_miterlimit:
      gstate.miterlimit = data;
      break;
    case MP_linewidth:
      gstate.linewidth = data;
      break;
    case MP_dash:
      gstate.dasharray = dasharray;
      gstate.dashoffset = dashoffset;
      break;
    case MP_transp_on:
      break;
  }
}

void MP_setting::print_pdf (std::ofstream & F) const {
  switch (command) {
    case MP_rgb:
    case MP_gray:
    case MP_cmyk:
      F << PL(col.to_pdfliteral());
      break;
    case MP_pattern:
      F << PL(fmt::format("{:s} /{:s} scn", col.to_pdfpatterncolor(), pattern));
      break;
    case MP_gradient:
      F << PL(fmt::format("/Pattern cs /{:s} scn", tex_Pname(pattern)));
      break;
    case MP_linejoin:
      F << PL(fmt::format("{:.0f} j", data));
      break;
    case MP_linecap:
      F << PL(fmt::format("{:.0f} J", data));
      break;
    case MP_miterlimit:
      F << PL(fmt::format("{} M", thdouble(data,prec_mp)));
      break;
    case MP_linewidth:
      F << PL(fmt::format("{} w", thdouble(data,prec_mp)));
      break;
    case MP_dash:
      {
      std::string t;
      t = "[";
      for (double d: dasharray) t += fmt::format("{} ", thdouble(d,prec_mp));
      t+= fmt::format("] {} d", thdouble(dashoffset,prec_mp));
      F << PL(t);
      }
      break;
    case MP_transp_on:
      if (alpha < 0) {
        if (LAYOUT.smooth_shading == shading_mode::quick) {
          if (alpha < -1.5) F << PL("/GS1 gs"); // -2: use the opacity from the layout, intended for s_altitudebar
          else F << PL("/GSa0 gs");             // -1: use zero alpha to "cut" the transparent areas from the scrap to show the shaded background
        } else
          F << PL("/GS1 gs");                   // use the opacity from the layout inside the knockout group
      } else {
        F << PL(fmt::format("/GSa{:.0f} gs", LAYOUT.alpha_step*round(100*alpha/LAYOUT.alpha_step)));
      }
      break;
  }
}

std::map<int,int> tmpclip;

void MP_data::print_svg (std::ofstream & F, std::string unique_prefix) {
//  F << "<g id=\"" << ID <<  // plain MP settings follow
//       "\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-miterlimit=\"10\">" << endl;
  for (unsigned int i=0; i<index.size(); i++) {
    switch (index[i].vector) {
      case I_path:
        paths[index[i].idx].print_svg(F,gstate,unique_prefix);
        break;
      case I_setting:
        settings[index[i].idx].print_svg(F,gstate);
        break;
      case I_text:
        texts[index[i].idx].print_svg(F,gstate);
        break;
      case I_gsave:
        switch (index[i].idx) {
          case MP_gsave:
            for (std::map<int,int>::iterator I = gstate.clippathdepth.begin();
                                        I!= gstate.clippathdepth.end(); I++) 
              I->second++;
            F << "<g>" << std::endl;
            GSTATE_stack.push_back(gstate);
            break;
          case MP_grestore:
            for (std::map<int,int>::iterator I = gstate.clippathdepth.begin();
                                        I!= gstate.clippathdepth.end(); I++) {
              I->second--;
              if (I->second < 0) F << "</g>" << std::endl;
            }
            // nemoze ist do predch. cyklu, lebo zmazanie smernika
            // urobi chaos
            {auto I = gstate.clippathdepth.begin();
              while (I!= gstate.clippathdepth.end()) {
                if (I->second < 0) I = gstate.clippathdepth.erase(I);
                else I++;
              }
            }
            tmpclip = gstate.clippathdepth;
            gstate = GSTATE_stack.back();
            gstate.clippathdepth = tmpclip;
            GSTATE_stack.pop_back();
            F << "</g>" << std::endl;
            break;
//          case MP_transp_on:
//            
//            break;
          case MP_transp_off:
            
            break;
        }
        break;
    }
  }
//  F << "</g>" << endl;
  
  thassert(gstate.clippathdepth.empty());
}

void MP_data::print_pdf (std::ofstream & F) const {
  for (unsigned int i=0; i<index.size(); i++) {
    switch (index[i].vector) {
      case I_path:
        paths[index[i].idx].print_pdf(F);
        break;
      case I_setting:
        settings[index[i].idx].print_pdf(F);
        break;
      case I_text:
        texts[index[i].idx].print_pdf(F);
        break;
      case I_gsave:
        switch (index[i].idx) {
          case MP_gsave:
            F << PL("q");
            break;
          case MP_grestore:
            F << PL("Q");
            break;
//          case MP_transp_on:
//            F << PL("/GS1 gs");
//            break;
          case MP_transp_off:
            F << PL("/GS0 gs");
            break;
        }
        break;
    }
  }

}

// print a single picture to SVG
// used for embedding the legend symbols into an XHTML file
void converted_data::print_svg (std::ofstream & F, std::string unique_prefix) { 
  std::ostringstream s;
  static long i_patt_def(10000);
  s << ++i_patt_def;   // i_patt maju byt rozne
  unique_prefix += "_";
  unique_prefix += s.str();

  F << "<svg width=\"" << 2.54/72*(urx - llx) << 
      "cm\" height=\"" << 2.54/72*(ury - lly) << 
      "cm\" viewBox=\"" << llx << " " << -ury << 
      " " << urx-llx << " " << ury-lly << 
      "\" xmlns=\"http://www.w3.org/2000/svg\" " << 
      "xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
  F << "<defs>" << std::endl;
  //patterns
  if (!patterns.empty()) {
    for (std::list<pattern>::iterator J = PATTERNLIST.begin();
                                J != PATTERNLIST.end(); J++) {
    if (patterns.count(J->name) > 0) {
        F << "<pattern id=\"patt_" << J->name <<  "_" << unique_prefix <<
            "\" patternUnits=\"userSpaceOnUse\"" << 
            " width=\"" << J->xstep <<   
            "\" height=\"" << J->ystep << 
            "\" patternTransform=\"matrix(" << J->xx << " " << J->xy << " " 
                                            << J->yx << " " << J->yy << " " 
                                            << J->x <<  " " << J->y  << 
            ")\">" << std::endl;
        F << "<g transform=\"translate(" 
                      << J->llx1-J->llx << " " << J->lly1-J->lly << ")\"" <<
                       " fill=\"black\" stroke=\"black\">\n";
                       // currentColor doesn't work to inherit the color of the symbol;
                       // context-fill/stroke doesn't work in patterns
        J->data.MP.print_svg(F,unique_prefix);
        F << "</g>" << std::endl;
        F << "</pattern>" << std::endl;
      }
    }
  }
  // gradients
  for (auto &g: GRADIENTS) {
    if (gradients.count(g.first) == 0) continue;
    if (g.second.type == gradient_lin) {
      fmt::print(F, "<linearGradient id=\"grad_{:s}_{:s}\" gradientUnits=\"userSpaceOnUse\" x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\">\n",
                 g.first, unique_prefix, thdouble(g.second.x0,prec_xy), thdouble(g.second.y0,prec_xy), thdouble(g.second.x1,prec_xy), thdouble(g.second.y1,prec_xy));
    } else {
      fmt::print(F, "<radialGradient id=\"grad_{:s}_{:s}\" gradientUnits=\"userSpaceOnUse\" fx=\"{}\" fy=\"{}\" fr=\"{}\" cx=\"{}\" cy=\"{}\" r=\"{}\">\n",
                 g.first, unique_prefix, thdouble(g.second.x0,prec_xy), thdouble(g.second.y0,prec_xy), thdouble(g.second.r0,prec_xy),
                                         thdouble(g.second.x1,prec_xy), thdouble(g.second.y1,prec_xy), thdouble(g.second.r1,prec_xy));
    }
    F << "<stop offset=\"0%\" stop-color=\"" << g.second.c0.to_svg() << "\"/>\n";
    F << "<stop offset=\"100%\" stop-color=\"" << g.second.c1.to_svg() << "\"/>\n";
    if (g.second.type == gradient_lin) {
      F << "</linearGradient>\n";
    } else {
      F << "</radialGradient>\n";
    }
  }
  // clip to initial viewBox
  F << "<clipPath id=\"clip_viewBox_" << unique_prefix << "\">" << std::endl;
  F << "<path d=\"M" << llx << " " << lly << 
      "L" << urx << " " << lly << 
      "L" << urx << " " << ury << 
      "L" << llx << " " << ury << "z\" />" << std::endl;
  F << "</clipPath>" << std::endl;
  
  F << "</defs>" << std::endl;
  // --- end of definitions ---
  F << "<g transform=\"scale(1,-1)\" fill=\"#000000\" stroke=\"#000000\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-miterlimit=\"10\" fill-rule=\"evenodd\" clip-rule=\"evenodd\" clip-path=\"url(#clip_viewBox_" << unique_prefix << ")\">" << std::endl;
  MP.print_svg(F,unique_prefix);
  F << "</g>" << std::endl;
  F << "</svg>" << std::endl;
}

void converted_data::print_pdf(std::ofstream & F, std::string name) const {
//  if (MP.index.empty()) return;  // can't skip the empty XObject definition, as it might be referenced somewhere
  conv_mode = mode;

  double HS = urx - llx;
  double VS = ury - lly;
  std::string font;

  if (mode > 0)
    F << "%\n\\setbox\\xxx=\\hbox{\\vbox to" << fmt::format("{:.2f}",VS) << "bp{\\vfill\n";

  MP.print_pdf(F);

  if (mode > 0) {
    F << "}}\\wd\\xxx=" << fmt::format("{:.2f}",HS) << "bp" << std::endl;
    F << "\\immediate\\pdfxform";
    if (mode == 32 && LAYOUT.transparency) {  // transparency group for the altitude bar
      F << " attr{/Group \\the\\attrid\\space 0 R}";
    }
    if (transparency || !fonts.empty() || !patterns.empty() || !gradients.empty() || icc_used()) {
      F << " resources { /ProcSet [/PDF /Text] ";
      if (transparency) {
        F << "/ExtGState \\the\\resid\\space 0 R ";
      }
      if (!fonts.empty()) {
        F << "/Font << ";
        for(std::set<std::string>::iterator I = fonts.begin();
                                  I != fonts.end(); I++) {
          font = tex_Fname(ALL_FONTS[*I]);
          F << "/F\\pdffontname\\" << font <<
                 "\\space\\pdffontobjnum\\" << font << "\\space 0 R ";
        }
        F << ">> ";
      }
      if (!patterns.empty() || !gradients.empty()) {
        F << "/Pattern << ";
        for(std::set<std::string>::iterator I = patterns.begin();
                                    I != patterns.end(); I++) {
          F << "/" << *I << " " << tex_get_ref(tex_Pname(ALL_PATTERNS[*I])) <<
                 "\\space 0 R ";
        }
        for(const auto & g: gradients) {
          F << "/" << tex_Pname(g) << " " << tex_get_ref(tex_Pname(g)) <<
                 "\\space 0 R ";
        }
        F << ">> ";
      }
      if (!patterns.empty() || icc_used()) {
        F << "/ColorSpace <<";
        if (!patterns.empty())
          F << " /CS1 [/Pattern /DeviceRGB] /CS2 [/Pattern /DeviceCMYK] /CS3 [/Pattern /DeviceGray] ";
        if (icc_used())
          F << icc2pdfresources();
        F << ">> ";
      }
      F << "} ";
    }
    std::string outname;
    if (mode < 30) outname = tex_Xname(name);
    else if (mode == 30) outname = tex_Lname(name);
    else if (31 <= mode && mode <= 32) outname = tex_Wname(name);
    else if (mode > 100 && mode < 110) outname = tex_Wname(name);
    else therror(("invalid conversion mode"));
    F << "\\xxx\n" << tex_set_ref(outname,"\\pdflastxform") << std::endl;
  }
}

void converted_data::clear() {
  MP.clear();
  fonts.clear();
}


std::string process_eps_string(std::string s, std::string font) {
  std::string r,t;
  unsigned char c;
  char *err;
  unsigned j;
  std::map<std::string,FONTCHARS>::iterator I;

  I = USED_CHARS.find(font);
  thassert (I != USED_CHARS.end());
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
    if (((*I).second).find(c) == ((*I).second).end()) {
      ((*I).second).insert(c);
    }
  }
  return r;
}



void parse_eps(std::string fname, std::string cname, double dx, double dy, 
               double & c1, double & c2, double & c3, double & c4, 
               converted_data & data, int mode, color col) {
  std::string tok, buffer;
  std::string font, patt;
  color pattcolor;
  gradient grad;
  bool comment = true, concat = false, 
       already_transp = false, transp_used = false, before_group_transp = false, cancel_transp = true;
  double llx = 0, lly = 0, urx = 0, ury = 0, HS = 0.0, VS = 0.0;
  std::string prev_alpha;
  std::deque<std::string> thbuffer;
  std::set<std::string> FORM_FONTS, FORM_PATTERNS, FORM_GRADIENTS;
  bool inpath = false, gsaveinpath = false;
  
  MP_path mp_path;
  MP_transform mp_trans, fntmatr;
  MP_text text;

  data.clear();
  data.mode = mode;

  std::ifstream F(fname.c_str());
  if(!F) therror((IOerr(fname)));
  while(F >> tok) {
    if (comment) {                      // File header
      if (tok == "%%BoundingBox:") {
        F >> llx >> lly >> urx >> ury;

        if (mode == 0) llx = lly = urx = ury = 0.0;   // no content shifting in patterns

        c1 = llx+dx;  // bbox pre absolutnu polohu 
        c2 = lly+dy;
        c3 = urx+dx;
        c4 = ury+dy;

	HS = urx - llx;
	VS = ury - lly;

        data.llx = 0;  // skutocny bbox 
        data.lly = 0;
        data.urx = HS;
        data.ury = VS;

        if (cname != "" && mode > 0 && mode <= 11) { // beginning of boundary cl.path definition
                           // for F and G scraps
          data.MP.add(MP_gsave);
          std::ifstream G(cname.c_str());
          if(!G) therror((IOerr(cname)));
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
        if (inpath)
          mp_path.add(MP_moveto, thbuffer[0],thbuffer[1],llx,lly);
        else if (!concat) {
          fntmatr.transf[0] = 1;
          fntmatr.transf[1] = 0;        
          fntmatr.transf[2] = 0;          
          fntmatr.transf[3] = 1;          
          fntmatr.transf[4] = std::stod(thbuffer[0])-llx;
          fntmatr.transf[5] = std::stod(thbuffer[1])-lly;
        }
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
          if (already_transp && cancel_transp) {  // transp off
            data.MP.add(MP_transp_off);
            already_transp = false;
          }
          cancel_transp = true;
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
        if (already_transp && cancel_transp) {  // transp off
          data.MP.add(MP_transp_off);
          already_transp = false;
        }
        cancel_transp = true;
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
        if (mode == 0) {
          thbuffer.clear();
          continue;
        }
        data.MP.add(MP_gray, thbuffer[0]);
        thbuffer.clear();
      }
      else if (tok == "setrgbcolor") {
        if (mode == 0) {
          thbuffer.clear();
          continue;
        }
        data.MP.add(MP_rgb, thbuffer[0]+" "+thbuffer[1]+" "+thbuffer[2]);
        thbuffer.clear();
      }
      else if (tok == "setcmykcolor") {
        if (mode == 0) {
          thbuffer.clear();
          continue;
        }
        data.MP.add(MP_cmyk, thbuffer[0]+" "+thbuffer[1]+" "+thbuffer[2]+" "+thbuffer[3]);
        thbuffer.clear();
      }
      else if (tok == "THsetpattern") {
        patt = thbuffer[0];
        if (FORM_PATTERNS.find(patt) == FORM_PATTERNS.end()) {
          FORM_PATTERNS.insert(patt);
        }
        if (ALL_PATTERNS.find(patt) == ALL_PATTERNS.end()) {
          ALL_PATTERNS.insert(make_pair(patt,u2str(patt_id)));
          patt_id++;
        }
        data.MP.add(MP_pattern, patt, pattcolor);
        thbuffer.clear();
      }
      else if (tok == "THgradient") {
        int addon;
        if (thbuffer[0] == "L") {
          grad.type = gradient_lin;
          addon = 0;
        } else {
          grad.type = gradient_rad;
          addon = 2;
        }
        if (10 <= mode && mode <= 20) grad.used_in_map = true; else grad.used_in_map = false;  // used in xhtml to check whether the gradient was used in the main map or the legend symbol
        try {
          grad.x0 = std::stod(thbuffer[1])-llx;
          grad.y0 = std::stod(thbuffer[2])-lly;
          grad.x1 = std::stod(thbuffer[3])-llx;
          grad.y1 = std::stod(thbuffer[4])-lly;
          if (thbuffer.size() == 7+addon) {
            grad.c0.set(std::stod(thbuffer[5]));
            grad.c1.set(std::stod(thbuffer[6]));
            if (grad.type == gradient_rad) {
              grad.r0 = std::stod(thbuffer[7]);
              grad.r1 = std::stod(thbuffer[8]);
            }
          } else if (thbuffer.size() == 11+addon) {
            grad.c0.set(std::stod(thbuffer[5]),std::stod(thbuffer[6]),std::stod(thbuffer[7]));
            grad.c1.set(std::stod(thbuffer[8]),std::stod(thbuffer[9]),std::stod(thbuffer[10]));
            if (grad.type == gradient_rad) {
              grad.r0 = std::stod(thbuffer[11]);
              grad.r1 = std::stod(thbuffer[12]);
            }
          } else if (thbuffer.size() == 13+addon) {
            grad.c0.set(std::stod(thbuffer[5]),std::stod(thbuffer[6]),std::stod(thbuffer[7]),std::stod(thbuffer[8]));
            grad.c1.set(std::stod(thbuffer[9]),std::stod(thbuffer[10]),std::stod(thbuffer[11]),std::stod(thbuffer[12]));
            if (grad.type == gradient_rad) {
              grad.r0 = std::stod(thbuffer[13]);
              grad.r1 = std::stod(thbuffer[14]);
            }
          } else ththrow("invalid buffer size");
        } catch (const std::exception& e) {
          therror((e.what()));
        }
        if (FORM_GRADIENTS.find(u2str(patt_id)) == FORM_GRADIENTS.end()) {
          FORM_GRADIENTS.insert(u2str(patt_id));
        }
        GRADIENTS.insert({u2str(patt_id),grad});
        data.MP.add(MP_gradient, u2str(patt_id));
        patt_id++;
        thbuffer.clear();
      }
      else if (tok == "THsetalpha") {
        transp_used = true;
        if (!already_transp || prev_alpha != thbuffer[0]) {
          data.MP.add(MP_transp_on, thbuffer[0]);
          already_transp = true;
          prev_alpha = thbuffer[0];
        }
        cancel_transp = false;
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
        if (already_transp) before_group_transp = true;
        else before_group_transp = false;
      }
      else if (tok == "grestore") {
        if (!inpath) data.MP.add(MP_grestore);
        thbuffer.clear();
        if (before_group_transp) already_transp = true;
        else already_transp = false;
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
      // currently we leave gsave, grestore unchanged;
      // path started with moveto is omitted
      
      else if (tok == "fshow") {            // font changes should be optimized
        if (already_transp && cancel_transp) {  // transp off
          data.MP.add(MP_transp_off);
          already_transp = false;
        }
        cancel_transp = true;
        text.clear();
        unsigned i = thbuffer.size();
        font = thbuffer[i-2];
        text.font = font;
        text.size = atof(thbuffer[i-1].c_str());
        if (FORM_FONTS.count(font) == 0) {
          FORM_FONTS.insert(font);
        }
        if (ALL_FONTS.count(font) == 0) {
          ALL_FONTS.insert(make_pair(font,u2str(font_id)));
          font_id++;
        }
//        font = tex_Fname(ALL_FONTS[font]);
        if (USED_CHARS.count(font) == 0) {
          FONTCHARS FCH;
          USED_CHARS.insert(make_pair(font,FCH));
        }
        buffer = "";
        for (unsigned j=0; j<i-2; j++) {
          buffer = buffer + thbuffer[j] + " ";
        }
        text.text = process_eps_string(buffer,font);
        text.xx = fntmatr.transf[0];
        text.xy = fntmatr.transf[1];
        text.yx = fntmatr.transf[2];
        text.yy = fntmatr.transf[3];
        text.x = fntmatr.transf[4];
        text.y = fntmatr.transf[5];
        text.col = col;
        concat = false;
        data.MP.add(text);
        thbuffer.clear();
      }
      else if (tok == "THsetpatterncolor") {  // currently unused in SVG as it is not completely trivial to implement uncolored patterns in SVG
        if (thbuffer.size() == 1)
          pattcolor.set(std::stod(thbuffer[0]));
        else if (thbuffer.size() == 3)
          pattcolor.set(std::stod(thbuffer[0]), std::stod(thbuffer[1]), std::stod(thbuffer[2]));
        else if (thbuffer.size() == 4)
          pattcolor.set(std::stod(thbuffer[0]), std::stod(thbuffer[1]), std::stod(thbuffer[2]), std::stod(thbuffer[3]));
        thbuffer.clear();
      }
      else {
        thbuffer.push_back(tok);
      }
    }
  }  // end of while loop
  F.close();
  if (mode > 0) {
    if (cname != "" && mode <= 11) { // end of boundary cl.path
      data.MP.add(MP_grestore);
    }
    data.fonts = FORM_FONTS;
    data.patterns = FORM_PATTERNS;
    data.gradients = FORM_GRADIENTS;
    if (transp_used) data.transparency = true;
    else data.transparency = false;
  }
}

void convert_scraps_new() {
  
  for(std::list<scraprecord>::iterator I = SCRAPLIST.begin(); 
                                  I != SCRAPLIST.end(); I++) {
    if (I->F != "") parse_eps(I->F, I->C, I->S1, I->S2, I->F1, I->F2, I->F3, I->F4, I->Fc, 10, I->col_scrap);
    if (I->G != "") parse_eps(I->G, I->C, I->S1, I->S2, I->G1, I->G2, I->G3, I->G4, I->Gc, 11);
    if (I->B != "") parse_eps(I->B, "", I->S1, I->S2, I->B1, I->B2, I->B3, I->B4, I->Bc, 12);
    if (I->I != "") parse_eps(I->I, "", I->S1, I->S2, I->I1, I->I2, I->I3, I->I4, I->Ic, 13);
    if (I->E != "") parse_eps(I->E, "", I->S1, I->S2, I->E1, I->E2, I->E3, I->E4, I->Ec, 14, I->col_scrap);
    if (I->X != "") parse_eps(I->X, "", I->S1, I->S2, I->X1, I->X2, I->X3, I->X4, I->Xc, 20, I->col_scrap);
  }

  for(std::list<legendrecord>::iterator I = LEGENDLIST.begin(); 
                                   I != LEGENDLIST.end(); I++) {
    double a,b,c,d;
    if (I->fname != "") parse_eps(I->fname, "",0,0,a,b,c,d,I->ldata, 30);
  }
  
  if (LAYOUT.northarrow != "") {
    double a, b, c, d;
    parse_eps(LAYOUT.northarrow, "",0,0,a,b,c,d,NArrow, 31);
  }
  if (LAYOUT.scalebar != "") {
    double a, b, c, d;
    parse_eps(LAYOUT.scalebar, "",0,0,a,b,c,d,ScBar, 31);
  }
  if (LAYOUT.altitudebar != "") {
    double a, b, c, d;
    parse_eps(LAYOUT.altitudebar, "",0,0,a,b,c,d,AltBar, 32);
  }

  GRIDLIST.clear();
  if (LAYOUT.grid > 0) {
    converted_data scr;
    double a,b,c,d;
    parse_eps(LAYOUT.gridAA, "",0,0,a,b,c,d,scr,101); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[0].x = a;
    LAYOUT.gridcell[0].y = b;
    parse_eps(LAYOUT.gridAB, "",0,0,a,b,c,d,scr,102); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[1].x = a;
    LAYOUT.gridcell[1].y = b;
    parse_eps(LAYOUT.gridAC, "",0,0,a,b,c,d,scr,103); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[2].x = a;
    LAYOUT.gridcell[2].y = b;
    parse_eps(LAYOUT.gridBA, "",0,0,a,b,c,d,scr,104); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[3].x = a;
    LAYOUT.gridcell[3].y = b;
    parse_eps(LAYOUT.gridBB, "",0,0,a,b,c,d,scr,105); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[4].x = a;
    LAYOUT.gridcell[4].y = b;
    parse_eps(LAYOUT.gridBC, "",0,0,a,b,c,d,scr,106); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[5].x = a;
    LAYOUT.gridcell[5].y = b;
    parse_eps(LAYOUT.gridCA, "",0,0,a,b,c,d,scr,107); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[6].x = a;
    LAYOUT.gridcell[6].y = b;
    parse_eps(LAYOUT.gridCB, "",0,0,a,b,c,d,scr,108); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[7].x = a;
    LAYOUT.gridcell[7].y = b;
    parse_eps(LAYOUT.gridCC, "",0,0,a,b,c,d,scr,109); GRIDLIST.push_back(scr);scr.clear();
    LAYOUT.gridcell[8].x = a;
    LAYOUT.gridcell[8].y = b;
  }

  PATTERNLIST.clear();

  std::ifstream P("patterns.dat");
  if(!P) therror(("Can't open patterns definition file!"));
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
//    if (ALL_PATTERNS.count(num) > 0) {  // changed to patt.used flag
                                          // because thsymbolset.cxx 
                                          // calls eps_parse after
                                          // this function is called
                                          // and patterns referenced
                                          // there would be missing in this list
      pattern patt;
      patt.used = (ALL_PATTERNS.count(num) > 0);
      patt.name = num;

      matr.replace(matr.find("["),1,"");
      matr.replace(matr.find("]"),1,"");
      std::istringstream s1(matr);
      s1 >> patt.xx >> patt.xy >> patt.yx >> patt.yy >> patt.x >> patt.y;
      bbox.replace(bbox.find("["),1,"");
      bbox.replace(bbox.find("]"),1,"");
      std::istringstream s2(bbox);
      s2 >> patt.llx >> patt.lly >> patt.urx >> patt.ury;
//      F << "/Matrix " << matr << endl;
//      F << "/BBox " << bbox << endl;
      patt.xstep = atof(xstep.c_str());
      patt.ystep = atof(ystep.c_str());

      parse_eps(pfile , "", 0,0, patt.llx1,patt.lly1,patt.urx1,patt.ury1,patt.data,0);
      PATTERNLIST.push_back(patt);
//    }  // patt.used
  }
  P.close();
 
  
}



int thconvert_eps() {

  thprintf("converting scraps ... ");

  ALL_FONTS.clear();
  ALL_PATTERNS.clear();
  USED_CHARS.clear();
  PATTERNLIST.clear();
  GRIDLIST.clear();
  GRADIENTS.clear();
  font_id = 1;
  patt_id = 1;

  convert_scraps_new();

  thprintf("done\n");
  return(0);
}

void thgraphics2pdf() {
  std::ofstream TEX("th_formdef.tex");
  if(!TEX) therror((IOerr("th_formdef.tex")));

  for(const auto &I: SCRAPLIST) {
    I.Fc.print_pdf(TEX, I.name);
    I.Gc.print_pdf(TEX, "G"+I.name);
    I.Bc.print_pdf(TEX, "B"+I.name);
    I.Ic.print_pdf(TEX, "I"+I.name);
    I.Ec.print_pdf(TEX, "E"+I.name);
    I.Xc.print_pdf(TEX, "X"+I.name);

    // make a special XObject form containing the colored background;
    // this is necessary for gouraud shading definitions which need
    // to be in the same coordinate system as the background outline of the scrap;
    // the following doesn't work (with the exception of Okular):
    //   q
    //   1 0 0 1 <dx> <dy> cm
    //   /Pattern cs /P1 scn
    //   /Fm1 Do
    //   Q
    if (I.I != "") {
      std::string s, res;
      if (!I.col_scrap.is_defined()) {
        s = LAYOUT.col_foreground.to_pdfliteral(fillstroke::fill);
      } else {
        if (LAYOUT.smooth_shading == shading_mode::off || I.gour_stream == "") {
          s = I.col_scrap.to_pdfliteral(fillstroke::fill);
        } else {
          s = "/Pattern cs /" + tex_Gname(I.name) + " scn";
          res = " /Pattern << /" + tex_Gname(I.name) + " " + tex_get_ref(tex_Gname(I.name)) + "\\space 0 R >> ";
        }
      }
      TEX << "\\setbox\\xxx=\\hbox to" << fmt::format("{:.2f}",I.Ic.urx-I.Ic.llx) << "bp{%\n";
      TEX << "\\PL{" << s <<  "}%\n";
      TEX << "\\pdfrefxform" << tex_get_ref(tex_Xname("I" + I.name)) + "%\n";
      TEX << "}\\ht\\xxx="  << fmt::format("{:.2f}",I.Ic.ury-I.Ic.lly) << "bp\\dp\\xxx=0bp%\n";
      TEX << "\\immediate\\pdfxform";
      TEX << " resources { " + res;
      if (icc_used()) {
        TEX << "/ColorSpace <<";
        TEX << icc2pdfresources();
        TEX << ">> ";
      }
      TEX << "}\\xxx" + tex_set_ref(tex_Xname("I" + I.name + "COLORED"), "\\pdflastxform") + "%\n";
    }
  }

  for(const auto &I: LEGENDLIST) {
    I.ldata.print_pdf(TEX, I.name);
  }

  NArrow.print_pdf(TEX, "northarrow");
  ScBar.print_pdf(TEX, "scalebar");
  if (LAYOUT.altitudebar != "") AltBar.print_pdf(TEX, "altitudebar");

  unsigned char c = 'a';
  for(const auto &I: GRIDLIST) {
    I.print_pdf(TEX, fmt::format("grid{:c}",c));
    c++;
  }

  TEX.close();


  std::ofstream F("th_fontdef.tex");
  if(!F) therror((IOerr("th_fontdef.tex")));
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
  std::set<std::string> fontset;
  std::string s;
  for (std::map<std::string,FONTCHARS>::iterator I = USED_CHARS.begin();
                                       I != USED_CHARS.end(); I++) {
    s = "\\includechars\\" + tex_Fname(ALL_FONTS[(*I).first]) + ":";
    for (FONTCHARS::iterator J = ((*I).second).begin();
                             J != ((*I).second).end(); J++) {
      c = *J;
      if (c > 31 && c < 128) {
//        if (c==37) F << "\\";    // % remains a comment
        s += c;
        if (c==92) s += " ";     // \ has to be followed by space
      } else {
        s += "^^" + fmt::format("{:02x}", c);
      }
    }
    s += "\\endinclude";
    fontset.insert(s);
  }
  for (auto & i: fontset) F << i;   // print the fonts sorted
  F << "\\endgroup" << std::endl;

  F << "% PATTERNS:" << std::endl;
  for (auto &patt: PATTERNLIST) {
      auto I = ALL_PATTERNS.find(patt.name);
      if (I == ALL_PATTERNS.end()) continue;

      F << "\\immediate\\pdfobj stream attr {/Type /Pattern\n";
      F << "/PaintType 2 /PatternType 1 /TilingType 1\n";
      F << fmt::format("/Matrix [{} {} {} {} {} {}]\n", thdouble(patt.xx,prec_mp), thdouble(patt.xy,prec_mp), thdouble(patt.yx,prec_mp), thdouble(patt.yy,prec_mp), thdouble(patt.x,prec_mp), thdouble(patt.y,prec_mp));
      F << fmt::format("/BBox [{} {} {} {}]\n", thdouble(patt.llx,prec_mp), thdouble(patt.lly,prec_mp), thdouble(patt.urx,prec_mp), thdouble(patt.ury,prec_mp));
      F << fmt::format("/XStep {}\n", thdouble(patt.xstep,prec_mp));
      F << fmt::format("/YStep {}\n", thdouble(patt.ystep,prec_mp));
      F << "/Resources << /ProcSet [/PDF ] ";
      if (icc_used()) F << " /ColorSpace <<" << icc2pdfresources() << ">> ";
      F << ">>} {\n";
      patt.data.print_pdf(F,patt.name);
      F << "}" << tex_set_ref(tex_Pname(ALL_PATTERNS[patt.name]), "\\pdflastobj") << "\n";
  }
  if (GRADIENTS.size() > 0) F << "% GRADIENTS:" << std::endl;
  for (auto &g: GRADIENTS) {
      F << "\\immediate\\pdfobj {<< /Type /Pattern /PatternType 2 /Shading <<\n";
      F << fmt::format("/ShadingType {:d} /ColorSpace /Device{:s}\n", g.second.type == gradient_lin ? 2 : 3, g.second.c0.model == colormodel::grey ? "Gray" : (g.second.c0.model == colormodel::cmyk ? "CMYK" : "RGB"));
      if (g.second.type == gradient_lin) {
        F << fmt::format("/Coords [{} {} {} {}] ", thdouble(g.second.x0,prec_xy), thdouble(g.second.y0,prec_xy), thdouble(g.second.x1,prec_xy), thdouble(g.second.y1,prec_xy));
      } else {
        F << fmt::format("/Coords [{} {} {} {} {} {}] ", thdouble(g.second.x0,prec_xy), thdouble(g.second.y0,prec_xy), thdouble(g.second.r0,prec_xy),
                                                         thdouble(g.second.x1,prec_xy), thdouble(g.second.y1,prec_xy), thdouble(g.second.r1,prec_xy));
      }
      F << "/Extend [true true]\n";
      F << fmt::format("/Function << /FunctionType 2 /Domain [0 1] /C0 [{}] /C1 [{}] /N 1 >>\n", g.second.c0.to_elements(), g.second.c1.to_elements());
      F << ">> >>}" << tex_set_ref(tex_Pname(g.first), "\\pdflastobj") << "\n";
  }
  // gouraud shadings
  for (auto &i: SCRAPLIST) {
      if (LAYOUT.smooth_shading == shading_mode::off || i.gour_stream == "") continue;
      std::ofstream F_g("th_gour_"+i.name+".dat", std::ios::out | std::ios::binary);
      if(!F_g) therror((IOerr("th_gour_"+i.name)));
      F_g << i.gour_stream;
      F_g.close();
      F << "\\immediate\\pdfobj stream attr {";
      F << fmt::format("/ShadingType 5 /ColorSpace /Device{:s}\n", LAYOUT.output_colormodel == colormodel::grey ? "Gray" : (LAYOUT.output_colormodel == colormodel::cmyk ? "CMYK" : "RGB"));
      F << fmt::format("/BitsPerCoordinate 8 /BitsPerComponent 8 /VerticesPerRow {:d} ", i.gour_n);
      F << fmt::format("/Decode [{} {} {} {} {:s}]", thdouble(i.gour_xmin-i.I1+i.S1,prec_matr), thdouble(i.gour_xmax-i.I1+i.S1,prec_matr),
                                                     thdouble(i.gour_ymin-i.I2+i.S2,prec_matr), thdouble(i.gour_ymax-i.I2+i.S2,prec_matr),
                                                     LAYOUT.output_colormodel == colormodel::grey ? "0 1" : (LAYOUT.output_colormodel == colormodel::cmyk ? "0 1 0 1 0 1 0 1" : "0 1 0 1 0 1"));
      F << "} file {th_gour_" << i.name << ".dat}\n";
      F << "\\immediate\\pdfobj {<< /Type /Pattern /PatternType 2 /Shading \\the\\pdflastobj\\space 0 R >>}\n";
      F << tex_set_ref(tex_Gname(i.name), "\\pdflastobj") << "\n";
  }
  F.close();

  std::vector<std::string> legend_arr_n, legend_arr_d;
  for(std::list<legendrecord>::iterator I = LEGENDLIST.begin();
                                   I != LEGENDLIST.end(); I++) {
    legend_arr_n.push_back(I->name);
    legend_arr_d.push_back(I->descr);
  }
  std::ofstream LEG("th_legend.tex");
  if(!LEG) therror((IOerr("th_legend.tex")));
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
  std::ofstream LEGCOLOR("th_legendcolor.tex");
  if(!LEGCOLOR) therror((IOerr("th_legendcolor.tex")));

  if (LAYOUT.altitudebar == "") {
    legendbox_num = COLORLEGENDLIST.size();
    columns = 1;
    rows = (int) ceil(double(legendbox_num) / columns);

    LEGCOLOR << "\\legendcolumns" << columns << "\n";

    for (int i = 0; i < rows; i++) {
      LEGCOLOR << "\\line{%\n";
      for (int j = 0; j < columns; j++) {
        pos = i + j * rows;
        if (pos < legendbox_num) {
          LEGCOLOR << "  \\colorlegendbox{";
          if (LAYOUT.transparency) LEGCOLOR << "/GS1 gs ";   // colorlegendbox argument is enclosed in q ... Q
          LEGCOLOR << legend_color[pos].col_legend.to_pdfliteral(fillstroke::fill) << "}%\n";
          LEGCOLOR << "  \\colorlegendsymbolbox{\\pdflastxform}{" <<
          legend_color[pos].texname << "}\\hskip10pt\n";
        }
      }
      LEGCOLOR << "\\hss}\n";
    }
  } else {
    LEGCOLOR << "\\ifaltitudebar\\vskip2mm\\altitudebar\\fi\n";
  }

  LEGCOLOR.close();


}
