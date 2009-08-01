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

/**
 * layout color class.
 */

struct thlayout_color {
  double R, G, B;
  int defined;
  void parse(char * str);
  thlayout_color() : R(1.0), G(1.0), B(1.0), defined(false) {}
  thlayout_color(double v) : R(v), G(v), B(v), defined(false) {}
  thlayout_color(double r, double g, double b) : R(r), G(g), B(b), defined(false) {}
};

#endif


