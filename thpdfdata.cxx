/**
 * @file thpdfdata.cxx
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
 
#include <list>
#include <map>
#include <set>
#include <string>
#include <sstream>


#include "thexception.h"
#include "thpdfdata.h"
#include "thlang.h"

using namespace std;

list<scraprecord> SCRAPLIST;
list<legendrecord> LEGENDLIST;
list<colorlegendrecord> COLORLEGENDLIST;
map<int,layerrecord> LAYERHASH;
set<int> MAP_PREVIEW_UP, MAP_PREVIEW_DOWN;
layout LAYOUT;

list<surfpictrecord> SURFPICTLIST;

scraprecord::scraprecord() {
  S1=0.0; S2=0.0;
  layer=0; level=0; sect=0;
  F1=0.0; F2=0.0; F3=0.0; F4=0.0; 
  G1=0.0; G2=0.0; G3=0.0; G4=0.0; 
  B1=0.0; B2=0.0; B3=0.0; B4=0.0; 
  I1=0.0; I2=0.0; I3=0.0; I4=0.0; 
  E1=0.0; E2=0.0; E3=0.0; E4=0.0; 
  X1=0.0; X2=0.0; X3=0.0; X4=0.0; 
  r=-1; g=-1; b=-1;
}

layerrecord::layerrecord() {
  Z=0;
  AltJump = 0;
  minx=0; maxx=0; miny=0; maxy=0;
  bookmark = false;
}

color::color() {
  model = colormodel::no;
  a = b = c = d = -1;
}

void color::set(double a) {
  model = colormodel::grey;
  this->a = a;
}

void color::set(double a, double b, double c) {
  model = colormodel::rgb;
  this->a = a;
  this->b = b;
  this->c = c;
}

void color::set(double a, double b, double c, double d) {
  model = colormodel::cmyk;
  this->a = a;
  this->b = b;
  this->c = c;
  this->d = d;
}

bool color::is_white() {
  if ((model == colormodel::grey && a == 1.0) ||
      (model == colormodel::rgb && (a == 1.0 && b == 1.0 && c == 1.0)) ||
      (model == colormodel::cmyk && (a == 0 && b == 0 && c == 0 && d == 0))) return true;
  else return false;
}

string color::to_svg() {
  double r = 0, g = 0, b = 0;
  if (model == colormodel::grey)
    r = g = b = this->a;
  else if (model == colormodel::rgb) {
    r = this->a;
    g = this->b;
    b = this->c;
  }
  else if (model == colormodel::cmyk) {
    r = 1.0 - min(1.0, this->a + this->d);
    g = 1.0 - min(1.0, this->b + this->d);
    b = 1.0 - min(1.0, this->c + this->d);
  }
  else therror(("undefined color used"));
  char ch[8];
  sprintf(ch,"#%02x%02x%02x",int(255*r) % 256,
                             int(255*g) % 256,
                             int(255*b) % 256);
  return (string) ch;
}

string color::to_pdfliteral(fillstroke fs) {
  ostringstream s;
  if (model == colormodel::grey) {
    if (fs == fillstroke::fill || fs == fillstroke::fillstroke)
      s << this->a << " g";
    if (fs == fillstroke::fillstroke)
      s << " ";
    if (fs == fillstroke::stroke || fs == fillstroke::fillstroke)
      s << this->a << " G";
  }
  else if (model == colormodel::rgb) {
    if (fs == fillstroke::fill || fs == fillstroke::fillstroke)
      s << this->a << " " << this->b << " " << this->c << " rg";
    if (fs == fillstroke::fillstroke)
      s << " ";
    if (fs == fillstroke::stroke || fs == fillstroke::fillstroke)
      s << this->a << " " << this->b << " " << this->c << " RG";
  }
  else if (model == colormodel::cmyk) {
    if (fs == fillstroke::fill || fs == fillstroke::fillstroke)
      s << this->a << " " << this->b << " " << this->c <<  " " << this->d << " k";
    if (fs == fillstroke::fillstroke)
      s << " ";
    if (fs == fillstroke::stroke || fs == fillstroke::fillstroke)
      s << this->a << " " << this->b << " " << this->c <<  " " << this->d << " K";
  }
  return s.str();
}

layout::layout() {
  transparency = true;
  overlap = 20;
  hsize = 400;
  vsize = 600;
  map_grid = false;
  hoffset = 0;
  voffset = 0;
  excl_pages = false;
  labelx = "0";
  labely = "0";
  page_numbering = true;
  nav_right = 2;
  nav_up = 2;
  nav_factor = 30;
  title_pages = false;
  col_background.set(1,1,1);
  col_foreground.set(1,1,1);
  col_preview_below.set(.8,.8,.8);
  col_preview_above.set(0,0,0);
  own_pages = 0;
  OCG = true;
  lang = THLANG_UNKNOWN;
  legend_width = 200;
  legend_columns = 2;
  surface = 0;
  surface_opacity = 0.7;
  hgridorigin = 0;
  vgridorigin = 0;
  map_header_bg = false;
  transparent_map_bg = false;

  colored_text = true;
  grid_coord_freq = 2;
}

paired::paired() {
  x = 0;
  y = 0;
}
