/*
 * Copyright (C) 2005 Martin Budaj
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
 
#ifndef thepsparse_h
#define thepsparse_h

#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>

enum class fillstroke {none, fill, stroke, fillstroke, fill2, clip, mask};
enum class colormodel {no, grey, rgb, cmyk};

struct color{
  double a = -1, b = -1, c = -1, d = -1;
  colormodel model = colormodel::no;

  color() {
    model = colormodel::no;
    a = b = c = d = -1;
  }

  void reset();
  void set(double);
  void set(double,double,double);
  void set(double,double,double,double);
  bool is_white();
  bool is_defined();
  std::string to_svg();
  std::string to_pdfliteral(fillstroke = fillstroke::fillstroke);
};

struct CGS {  // current graphics state
  color col;
  int linejoin, linecap;
  float miterlimit, linewidth;
  std::list<float> dasharray;
  float dashoffset;
  std::string pattern;
  
  std::map<int,int> clippathdepth;
  static int clippathID;
   
  CGS();
  std::string svg_color();  
};

struct MP_transform {
  int command;
  double transf[6];
  
  MP_transform();
  void clear();
  void set(int, std::string, std::string, double, double);
  void set(int, std::string, std::string, std::string, std::string, std::string, std::string, double, double);
};

struct MP_path_segment {
  int command;
  double coord[6];
};

struct MP_path {
  std::vector<MP_path_segment> segments;
  bool closed;
//  bool clip;  mp nevie orezat aj vykreslit
  int fillstroke;
  MP_transform transformation;
//  bool transform;
  
  MP_path();
  void clear();
  void add(int, std::string, std::string, std::string, std::string, std::string, std::string, double, double);
  void add(int, std::string, std::string, double, double);

  void print_svg(std::ofstream & F, CGS & gstate, std::string prefix);
};

struct MP_index {
  int vector, idx;
};

struct MP_text {
  std::string text, font;
  double size, x, y, xx, xy, yx, yy;
  color col;
  bool transformed;
  
  MP_text();
  void clear();
  void print_svg(std::ofstream & F, CGS & gstate);
};

struct MP_setting {
  int command;
  double data[4];
//  string str;
  std::list<float> dasharray;
  float dashoffset;
  std::string pattern;
  
  void print_svg(std::ofstream & F, CGS & gstate);
};

enum {MP_lineto, MP_moveto, MP_curveto, MP_rlineto};
enum {MP_fill, MP_stroke, MP_fillstroke, MP_clip};

enum {MP_linejoin, MP_linecap, MP_miterlimit, MP_gray, MP_rgb, MP_cmyk,
      MP_pattern, MP_transp, MP_dash, MP_linewidth};

enum {MP_notransf, MP_scale, MP_translate, MP_concat};
enum {MP_gsave, MP_grestore, MP_transp_on, MP_transp_off};
enum {I_path, I_text, I_setting, I_gsave, I_transform};

enum {MP_mitered = 0, MP_rounded, MP_beveled};
enum {MP_butt=0, MP_squared=2};

struct MP_data {
  std::vector<MP_index> index;
  std::vector<MP_path> paths;
  std::vector<MP_text> texts;
  std::vector<MP_setting> settings;
  std::vector<MP_transform> transforms;
  
  int idx;
  
  CGS gstate;
  
  std::list<CGS> GSTATE_stack;
  
  void add(MP_path);
  void add(MP_text);
  void add(MP_transform);
  void add(int);
  void add(int,std::string);
  void get();
  void pop();
  
  MP_data();
  void clear();
  
  void print_svg(std::ofstream & F, std::string prefix);
};

struct converted_data {
  MP_data MP;
  std::set<std::string> fonts, patterns;
  bool transparency = false;
//  double hsize, vsize;
  double llx, lly, urx, ury;
  
  void clear();
  converted_data();
  void print_svg(std::ofstream & F, std::string prefix="");
};

struct pattern {
  converted_data data;
  float llx, lly, urx, ury, xstep, ystep;
  double llx1,lly1,urx1,ury1;
  double xx, xy, yx, yy, x, y;
  std::string name;
  bool used;
};

int thconvert_new();
void parse_eps(std::string fname, std::string cname, double dx, double dy, 
               double & c1, double & c2, double & c3, double & c4, 
               converted_data & data, color=color());

#endif
