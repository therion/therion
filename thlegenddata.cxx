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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include <string>


#include "thlegenddata.h"

legenddata::legenddata() {
  legendtitle = "Legenda";
  colorlegendtitle = "Legenda hlbok";
  cavename = "Jaskyna MN";
  comment = "komentar";
  copyrights = "(c) ja";
  cavelengthtitle = "Dlzka";
  cavelength = "124324";
  cavedepthtitle = "Hlbka";
  cavedepth = "3243";
  explotitle = "Objavili";
  explodate = "2004";
  exploteam = "A B C";
  topotitle = "Zamerali";
  topodate = "2005";
  topoteam = "D E F";
  cartotitle = "Kreslil";
  cartodate = "2006";
  cartoteam = "G";
  
  northarrow = true;
  scalebar = true;
  
}

//void legenddata::print_tex() {
//}

//void legenddata::print_svg() {
//}
