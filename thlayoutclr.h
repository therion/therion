/** 
 * @file thlayoutclr.h
 * Scrap outline line class.
 */
  
/* Copyright (C) 2009 Stacho Mudrak
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
 
#ifndef thlayoutclr_h
#define thlayoutclr_h

#include "thparse.h"
#include "thpdfdata.h"
#include <stdio.h>

enum {
  TT_LAYOUTCLRMODEL_UNKNOWN = 0,
  TT_LAYOUTCLRMODEL_GRAY = 1,
  TT_LAYOUTCLRMODEL_RGB = 2,
  TT_LAYOUTCLRMODEL_CMYK = 4,
};

static const thstok thtt_layoutclr_model[] = {
  {"cmyk", TT_LAYOUTCLRMODEL_CMYK},
  {"grayscale", TT_LAYOUTCLRMODEL_GRAY},
  {"rgb", TT_LAYOUTCLRMODEL_RGB},
  {NULL, TT_LAYOUTCLRMODEL_UNKNOWN}
};



/**
 * layout color class.
 */

struct thlayout_color {
  double R, G, B, A, C, M, Y, K, W;
  int defined;
  int model;
  void parse(char * str, bool aalpha = false);
  thlayout_color() : R(1.0), G(1.0), B(1.0), A(1.0), C(0.0), M(0.0), Y(0.0), K(0.0), W(1.0), defined(0), model(TT_LAYOUTCLRMODEL_RGB) {}
  thlayout_color(double v) : R(v), G(v), B(v), A(1.0), C(0.0), M(0.0), Y(0.0), K(1.0 - v), W(v), defined(0), model(TT_LAYOUTCLRMODEL_GRAY) {}
  thlayout_color(double r, double g, double b) : R(r), G(g), B(b), A(1.0), C(0.0), M(0.0), Y(0.0), K(0.0), W(0.0), defined(0), model(TT_LAYOUTCLRMODEL_RGB) {}
  thlayout_color(double c, double m, double y, double k) : R(0.0), G(0.0), B(0.0), A(1.0), C(c), M(m), Y(y), K(k), W(1-k), defined(0), model(TT_LAYOUTCLRMODEL_CMYK) {}
  void copy_color(const thlayout_color& src);
  bool is_defined();
  void set_color(int output_model, color & clr);
  void print_to_file(int output_model, FILE * f);
  std::string print_to_str(int output_model);
  void encode_to_str(int output_model, std::string & str);  
  void RGBtoGRAYSCALE();
  void GRAYSCALEtoRGB();
  void RGBtoCMYK();
  void CMYKtoRGB();
  void alpha_correct(double alpha);
  void mix_with_color(double ratio, thlayout_color other_clr);
  void fill_missing_color_models();
};

#endif


