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

using namespace std;

string select_lang(string s, string lang);

struct fontrecord {
  int id;
  string rm,it,bf,ss,si;
};

extern list <fontrecord> FONTS;

int get_enc_id(const char* enc);
void print_tex_encodings (void);
void print_fonts_setup(void);

void init_encodings();

string utf2texoctal(string s);
string utf2texhex(string s);
string utf2tex (string s, bool b=false);
int tex2uni(string font, int ch);

struct encodings_new {
 private:
  vector<int> v_fon;  // tex - unicode
  map<int,int> m_fon; // unicode - tex
  int i_fon; // last position
  
 public:
  int NFSS, t1_convert;
  string otf_file[5]; 
  
  encodings_new();
  
  int get_enc_pos(int ch);
  int get_enc_count();
  int get_uni (int f, int ch);
  void write_enc_files();
};

extern encodings_new ENC_NEW;


#ifndef NOTHERION
const char * utf2tex (const char * s, bool b = false);
#endif

#endif
