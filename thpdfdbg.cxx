/**
 * @file thpdfdbg.cxx
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
 
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <iomanip>
#include <fmt/core.h>

#include <iostream>

#include "thpdfdbg.h"
#include "thpdfdata.h"
#include "thexception.h"

std::string tex_Xname(std::string s) {return("THX"+s);}
std::string tex_Wname(std::string s) {return("THW"+s);}   // special = northarrow &c.
std::string tex_Fname(std::string s) {return("THF"+s);}
std::string tex_Pname(std::string s) {return("THP"+s);}
std::string tex_Lname(std::string s) {return("THL"+s);}

bool tex_refs_registers = true;

std::string tex_set_ref(std::string s, std::string t) {
  if (tex_refs_registers)
    return("\\newcount\\" + s + "\\" + s + "=" + t);   // use registers, legacy mode
  else
    return("\\setref{" + s + "}{\\the" + t + "}");     // use macros to store references
}

std::string tex_get_ref(std::string s) {
  if (tex_refs_registers)
    return("\\" + s);
  else
    return("\\getref{" + s + "}");
}

std::string u2str(unsigned u) {
  unsigned i=u;
  char c;
  std::string s="";
  while (i>0) {
    c = 'a' + ((i-1) % 26);
    s = c + s;
    i = (i-1) / 26;
  };
  return (s);
}

std::string rgb2svg(double r, double g, double b) {
  return fmt::format("#{:02x}{:02x}{:02x}",int(255*r) % 256,
                                           int(255*g) % 256,
                                           int(255*b) % 256);
}

std::string icc2pdfresources() {
  std::string s;
  if (!LAYOUT.icc_profile_cmyk.empty()) s += " /DefaultCMYK \\the\\iccobjcmyk\\space 0 R ";
  if (!LAYOUT.icc_profile_rgb.empty())  s += " /DefaultRGB \\the\\iccobjrgb\\space 0 R ";
  if (!LAYOUT.icc_profile_gray.empty()) s += " /DefaultGray \\the\\iccobjgray\\space 0 R ";
  return s;
}

bool icc_used() {
  return (!LAYOUT.icc_profile_cmyk.empty() || !LAYOUT.icc_profile_rgb.empty() || !LAYOUT.icc_profile_gray.empty());
}

