/**
 * @file thlayoutclr.cxx
 */
  
/* Copyright (C) 2000 Stacho Mudrak
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
 
#include "thlayoutclr.h"
#include "thparse.h"
#include "thdatabase.h"
#include "thexception.h"
#include <cmath>
#include <fmt/printf.h>


bool thlayout_color::is_defined() {
  return (this->defined > 0);
}


void thlayout_color::set_color(int output_model, color & clr) {
	this->fill_missing_color_models();
	if ((output_model & TT_LAYOUTCLRMODEL_CMYK) > 0)
		clr.set(this->C, this->M, this->Y, this->K);
	else if ((output_model & TT_LAYOUTCLRMODEL_GRAY) > 0)
		clr.set(this->W);
	else
		clr.set(this->R, this->G, this->B);
}


void thlayout_color::fill_missing_color_models() {
	if ((this->model & TT_LAYOUTCLRMODEL_RGB) > 0) {
		if ((this->model & TT_LAYOUTCLRMODEL_CMYK) == 0)
			this->RGBtoCMYK();
		if ((this->model & TT_LAYOUTCLRMODEL_GRAY) == 0)
			this->RGBtoGRAYSCALE();
	}
	if ((this->model & TT_LAYOUTCLRMODEL_CMYK) > 0) {
		if ((this->model & TT_LAYOUTCLRMODEL_RGB) == 0)
			this->CMYKtoRGB();
		if ((this->model & TT_LAYOUTCLRMODEL_GRAY) == 0)
			this->RGBtoGRAYSCALE();
	}
	if ((this->model & TT_LAYOUTCLRMODEL_GRAY) > 0) {
		if ((this->model & TT_LAYOUTCLRMODEL_RGB) == 0)
			this->GRAYSCALEtoRGB();
		if ((this->model & TT_LAYOUTCLRMODEL_CMYK) == 0)
			this->RGBtoCMYK();
	}
}


void thlayout_color::print_to_file(int output_model, FILE * f) {
	this->fill_missing_color_models();
	if ((output_model & TT_LAYOUTCLRMODEL_CMYK) > 0)
		fprintf(f, "(%.5f,%.5f,%.5f,%.5f)", this->C, this->M, this->Y, this->K);
	else if ((output_model & TT_LAYOUTCLRMODEL_GRAY) > 0)
		fprintf(f, "(%.5f)", this->W);
	else
		fprintf(f, "(%.5f,%.5f,%.5f)", this->R, this->G, this->B);
}


char dbl1_to_char(double dbl) {
	int x = int(255.0 * dbl);
	if (x > 255) x = 255;
	if (x < 0) x = 0;
	return char(x);
}

void thlayout_color::encode_to_str(int output_model, std::string & str) {
	this->fill_missing_color_models();
	if ((output_model & TT_LAYOUTCLRMODEL_CMYK) > 0) {
		str += dbl1_to_char(this->C);
		str += dbl1_to_char(this->M);
		str += dbl1_to_char(this->Y);
		str += dbl1_to_char(this->K);
	} else if ((output_model & TT_LAYOUTCLRMODEL_GRAY) > 0) {
		str += dbl1_to_char(this->W);
	} else {
		str += dbl1_to_char(this->R);
		str += dbl1_to_char(this->G);
		str += dbl1_to_char(this->B);
	}
}


std::string thlayout_color::print_to_str(int output_model) {
	this->fill_missing_color_models();
	if ((output_model & TT_LAYOUTCLRMODEL_CMYK) > 0)
		return fmt::sprintf("(%.5f,%.5f,%.5f,%.5f)", this->C, this->M, this->Y, this->K);
	else if ((output_model & TT_LAYOUTCLRMODEL_GRAY) > 0)
		return fmt::sprintf("(%.5f)", this->W);
	else
		return fmt::sprintf("(%.5f,%.5f,%.5f)", this->R, this->G, this->B);
}

void thlayout_color::RGBtoGRAYSCALE() {
	this->W = 0.2126 * this->R + 0.7152 * this->G + 0.0722 * this->B;
	if (this->W <= 0.0031308) {
		this->W *= 12.92;
	} else {
		this->W = 1.055 * pow(this->W, 1/2.4) - 0.055;
	}
}

void thlayout_color::GRAYSCALEtoRGB() {
	this->R = this->G = this->B = this->W;
}


void thlayout_color::RGBtoCMYK() {
    this->C = 1.0 - this->R;
    this->M = 1.0 - this->G;
    this->Y = 1.0 - this->B;
    this->K = this->C;
    if (this->K > this->M) this->K = this->M;
    if (this->K > this->Y) this->K = this->Y;
    if (this->K == 1.0) {
    	this->C = 0.0;
    	this->M = 0.0;
    	this->Y = 0.0;
    } else {
    	this->C = (1.0 - this->R - this->K) / (1.0 - this->K);
    	this->M = (1.0 - this->G - this->K) / (1.0 - this->K);
    	this->Y = (1.0 - this->B - this->K) / (1.0 - this->K);
    }
}

void thlayout_color::CMYKtoRGB() {
  	this->R = (1.0 - this->C) * (1.0 - this->K);
  	this->G = (1.0 - this->M) * (1.0 - this->K);
  	this->B = (1.0 - this->Y) * (1.0 - this->K);
}

#define invalid_color_spec ththrow("invalid color specification -- {}", str);

double clrhex2num(char * str, char p) {
	char src[3];
	char *endptr = NULL;
	int iv;
	src[0] = str[2*p];
	src[1] = str[1 + 2*p];
	src[2] = 0;
	errno = 0;
	iv = (int) strtol(src, &endptr, 16);
	if ((errno != 0) || (*endptr != 0)) invalid_color_spec;
	return (double(iv) / 256.0);
}

void thlayout_color::parse(char * str, bool aalpha) {
	// 1 arg = W (grayscale)
	// 3 arg = RGB
	// 4 arg = CMYK
	// 8 arg = RGBCMYKW
  thsplit_words(&(thdb.mbuff_tmp), str);
  int nargs = thdb.mbuff_tmp.get_size(), sv;
  char ** args = thdb.mbuff_tmp.get_buffer();
  switch (nargs) {
    case 8:
			thparse_double(sv,this->C,args[4]);
			if ((sv != TT_SV_NUMBER) || (this->C < 0.0) || (this->C > 100.0))
				invalid_color_spec;
			thparse_double(sv,this->M,args[5]);
			if ((sv != TT_SV_NUMBER) || (this->M < 0.0) || (this->M > 100.0))
				invalid_color_spec;
			thparse_double(sv,this->Y,args[6]);
			if ((sv != TT_SV_NUMBER) || (this->Y < 0.0) || (this->Y > 100.0))
				invalid_color_spec;
			thparse_double(sv,this->K,args[7]);
			if ((sv != TT_SV_NUMBER) || (this->K < 0.0) || (this->K > 100.0))
				invalid_color_spec;
			this->C /= 100.0;
			this->M /= 100.0;
			this->Y /= 100.0;
			this->K /= 100.0;
      		[[fallthrough]];
  	case 4:
			thparse_double(sv,this->W,args[3]);
			if ((sv != TT_SV_NUMBER) || (this->W < 0.0) || (this->W > 100.0))
				invalid_color_spec;
			this->W /= 100.0;
      		[[fallthrough]];
    case 3:
      thparse_double(sv,this->B,args[2]);        
      if ((sv != TT_SV_NUMBER) || (this->B < 0.0) || (this->B > 100.0))
        invalid_color_spec;
      this->B /= 100.0;
      thparse_double(sv,this->G,args[1]);        
      if ((sv != TT_SV_NUMBER) || (this->G < 0.0) || (this->G > 100.0))
        invalid_color_spec;
      this->G /= 100.0;
      [[fallthrough]];
    case 1:
      if (aalpha && (strcmp(args[0],"transparent") == 0)) {
        this->A = 0.0;
        this->defined = 2;
        break;
      }
      thparse_double(sv,this->R,args[0]);
      if ((nargs == 1) && (sv != TT_SV_NUMBER) && (strlen(args[0]) == 6)) {
    	  // CSS color specification starting with #
    	  this->R = clrhex2num(args[0], 0) * 100.0;
    	  this->G = clrhex2num(args[0], 1);
    	  this->B = clrhex2num(args[0], 2);
    	  sv = TT_SV_NUMBER;
    	  nargs = 3;
      }
      if ((sv != TT_SV_NUMBER) || (this->R < 0.0) || (this->R > 100.0))
        invalid_color_spec;
      this->R /= 100.0;
      if (nargs == 1) {
        this->B = this->R;
        this->G = this->R;
        this->W = this->R;
        this->K = 1.0 - this->R;
        this->C = 0.0;
        this->M = 0.0;
        this->Y = 0.0;
        this->model = TT_LAYOUTCLRMODEL_GRAY;
      }
      if (nargs == 3) {
        this->RGBtoCMYK();
        this->RGBtoGRAYSCALE();
        this->model = TT_LAYOUTCLRMODEL_RGB;
      }
      if (nargs == 4) {
      	this->C = this->R;
      	this->M = this->G;
      	this->Y = this->B;
      	this->K = this->W;
      	this->CMYKtoRGB();
      	this->RGBtoGRAYSCALE();
        this->model = TT_LAYOUTCLRMODEL_CMYK;
      }
      if (nargs == 8) {
      	double tmp;
      	tmp = this->K;
      	this->K = this->Y;
      	this->Y = this->M;
      	this->M = this->C;
      	this->C = this->W;
      	this->W = tmp;
      	this->model = TT_LAYOUTCLRMODEL_CMYK | TT_LAYOUTCLRMODEL_GRAY | TT_LAYOUTCLRMODEL_RGB;
      }
      this->defined = 2;
      break;
    default:
      invalid_color_spec;
  }
}

void thlayout_color::alpha_correct(double alpha) {
	this->R = alpha * this->R + (1 - alpha) * 1.0;
	this->G = alpha * this->G + (1 - alpha) * 1.0;
	this->B = alpha * this->B + (1 - alpha) * 1.0;
	this->W = alpha * this->W + (1 - alpha) * 1.0;
	this->C = alpha * this->C + (1 - alpha) * 0.0;
	this->M = alpha * this->M + (1 - alpha) * 0.0;
	this->Y = alpha * this->Y + (1 - alpha) * 0.0;
	this->K = alpha * this->K + (1 - alpha) * 0.0;
}

/**
 * Mix color with ratio * other_clr.
 */

void thlayout_color::mix_with_color(double ratio, thlayout_color other_clr) {
	this->R = (1 - ratio) * this->R + ratio * other_clr.R;
	this->G = (1 - ratio) * this->G + ratio * other_clr.G;
	this->B = (1 - ratio) * this->B + ratio * other_clr.B;
	this->W = (1 - ratio) * this->W + ratio * other_clr.W;
	this->C = (1 - ratio) * this->C + ratio * other_clr.C;
	this->M = (1 - ratio) * this->M + ratio * other_clr.M;
	this->Y = (1 - ratio) * this->Y + ratio * other_clr.Y;
	this->K = (1 - ratio) * this->K + ratio * other_clr.K;
}

void thlayout_color::copy_color(const thlayout_color& src) {
    if (this == &src)
        return;
    this->R = src.R;
    this->G = src.G;
    this->B = src.B;
    this->W = src.W;
    this->C = src.C;
    this->M = src.M;
    this->Y = src.Y;
    this->K = src.K;
    this->A = src.A;
    this->model = src.model;
}

