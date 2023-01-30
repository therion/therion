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
 

#ifndef thtexfonts_h
#define thtexfonts_h

#include <string>
#include <list>
#include <vector>
#include <map>

std::string select_lang(std::string s, std::string lang);

struct fontrecord {
  int id;
  std::string rm,it,bf,ss,si;
  bool opt;
};

extern std::list <fontrecord> FONTS;

int get_enc_id(const char* enc);
void print_tex_encodings (void);
void print_fonts_setup(void);

void init_encodings();

std::string utf2texoctal(std::string s);
std::string utf2texhex(std::string s);
std::string utf2tex (std::string s, bool b=false);
int tex2uni(std::string font, int ch);
std::string replace_all(std::string s, std::string f, std::string r);

struct encodings_new {
 private:
  std::vector<int> v_fon;  // tex - unicode
  std::map<int,int> m_fon; // unicode - tex
  int i_fon; // last position
  
 public:
  int NFSS, t1_convert;
  std::string otf_file[5]; 
  
  encodings_new();
  
  int get_enc_pos(int ch);
  int get_enc_count();
  int get_uni (int f, int ch);
  void write_enc_files();
};

extern encodings_new ENC_NEW;

#endif
