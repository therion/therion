/*
 * Copyright (C) 2003 Martin Budaj
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
 

#ifndef thpdfdbg_h
#define thpdfdbg_h

#include <string>

std::string u2str(unsigned u);
std::string tex_Xname(std::string s);
std::string tex_Wname(std::string s);
std::string tex_Fname(std::string s);
std::string tex_Pname(std::string s);
std::string tex_Lname(std::string s);
std::string tex_Gname(std::string s);
std::string tex_set_ref(std::string s, std::string t);
std::string tex_get_ref(std::string s);
std::string pdf_info(void);
std::string rgb2svg(double r, double g, double b);
std::string icc2pdfresources();
bool icc_used();

extern bool tex_refs_registers;

#endif
