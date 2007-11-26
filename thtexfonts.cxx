/**
 * @file thtexfonts.cxx
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

#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>

#include <cstring>
#include <cstdio>
#include <cassert>

#include "thtexfonts.h"
#include "thtexenc.cxx"
#include "thpdfdbg.h"

#ifndef NOTHERION
#include "thbuffer.h"
thbuffer thtexfontsbuff;
#endif

using namespace std;

list<fontrecord> FONTS;
typedef list<int> unistr;

// returns the index of given encoding (as used in predefined encoding arrays)
// or -1 if such an encoding is not known

int get_enc_id(const char * enc) {
  for (int i=0; i<max_enc; i++) if (strcmp(encodings[i],enc) == 0) return i;
  return -1;
}

void print_tex_encodings (void) {
  for (int i=0; i<max_enc; i++) cout << encodings[i] << endl;
}

void init_encodings() {
  fontrecord F;
  F.id = get_enc_id("raw");
  F.rm = "cmr10";
  F.it = "cmti10";
  F.bf = "cmbx10";
  F.ss = "cmss10";
  F.si = "cmssi10";
  FONTS.push_back(F);
  F.id = get_enc_id("xl2");
  F.rm = "csr10";
  F.it = "csti10";
  F.bf = "csbx10";
  F.ss = "csss10";
  F.si = "csssi10";
  FONTS.push_back(F);
  F.id = get_enc_id("cmcyr");
  F.rm = "cmcyr10";
  F.it = "cmcti10";
  F.bf = "cmcbx10";
  F.ss = "cmcss10";
  F.si = "cmcssi10";
  FONTS.push_back(F);
}

unistr utf2uni(string s) {
  unsigned char c;
  unistr t;
  int j;

  for (unsigned i=0; i<s.size(); i++) {
    c = s[i];
    if (c<128) {
      t.push_back(c);
    }
    else if (c>=192 && c<=223) {
      j = 64*(c-192); 
      c = s[++i];
      j += c-128;
      t.push_back(j);
    }
    else if (c>=224 && c<=239) {
      j = 4096*(c-224);
      c = s[++i];
      j += 64*(c-128); 
      c = s[++i];
      j += c-128;
      t.push_back(j);
    }
    else therror (("Invalid utf-8 string!")); // we don't support higher
                                              // unicode characters
  }
  return t;
}

// converts utf8 to 2B Unicode in a special format for pdfTeX

string utf2texoctal(string str) {
  unistr s = utf2uni(str);
  string t;
  char ch[10];
//  t = "\\ne\\376\\ne\\377";
  unsigned char c;
  for (unistr::iterator I = s.begin(); I != s.end(); I++) {
    c = (*I) / 256;
    sprintf(ch,"%o",c);
    t = t + "\\ne\\" + (string) ch;
    c = (*I) % 256;
    sprintf(ch,"%o",c);
    t = t + "\\ne\\" + (string) ch;
  }
  return t;
}

// converts utf8 to 2B Unicode in a special format for pdfTeX

string utf2texhex(string str) {
  unistr s = utf2uni(str);
  string t;
  char ch[10];
  unsigned char c;
  for (unistr::iterator I = s.begin(); I != s.end(); I++) {
    c = (*I) / 256;
    sprintf(ch,"%02x",c);
    t += (string) ch;
    c = (*I) % 256;
    sprintf(ch,"%02x",c);
    t += (string) ch;
  }
  return t;
}

string replace_all(string s, string f, string r) {
  size_t found = s.find(f);
  while(found != string::npos) {
    s.replace(found, f.length(), r);
    found = s.find(f);
  }
  return s;
}

// main task is done here

#define SELFONT if (lastenc!=-1) T << "\\thf" << u2str(lastenc+1)

string utf2tex(string str, bool remove_kerning) {
  ostringstream T;
  string tmp;
  int wc;  //wide char
  int lastenc = -1;
  int laststyle = -1;
  int align = 0;
  bool is_multiline = false;

  if (str.find("<center>") != string::npos) align = 1;
  else if (str.find("<centre>") != string::npos) align = 1;
  else if (str.find("<left>") != string::npos) align = 0;
  else if (str.find("<right>") != string::npos) align = 2;

  if (str.find("<br>") != string::npos) is_multiline = true;
 
  str = replace_all(str,"<center>","");
  str = replace_all(str,"<centre>","");
  str = replace_all(str,"<left>","");
  str = replace_all(str,"<right>","");
  str = replace_all(str,"<br>","\033\1");   // \e not accepted in VC++
  str = replace_all(str,"<thsp>","\033\2");
  str = replace_all(str,"<rm>","\033\3");
  str = replace_all(str,"<it>","\033\4");
  str = replace_all(str,"<bf>","\033\5");
  str = replace_all(str,"<ss>","\033\6");
  str = replace_all(str,"<si>","\033\7");

  if (is_multiline) {
    T << "\\vbox{\\halign{";
    if (align > 0) T << "\\hfil";
    T << "#";
    if (align < 2) T << "\\hfil";
    T << "\\cr";
  }
  
  unistr s = utf2uni(str);

  for (unistr::iterator I = s.begin(); I != s.end(); I++) {
    wc = *I;
    if (wc == 32) {                     // space requires special treatment 
      T << "\\ ";                        // (it's not included in TeX fonts)
      continue;                         // so encodings search doesn't help
    }
    else if (wc == 27) {                // escaped chars (special formatting)
      wc = *(++I);
      switch (wc) {
        case 1: 
          T << "\\cr";
          switch (laststyle) {
            case 1: T << "\\rm"; break;
            case 2: T << "\\it"; break;
            case 3: T << "\\bf"; break;
            case 4: T << "\\ss"; break;
            case 5: T << "\\si"; break;
          }
          SELFONT; T << " ";
          break;
        case 2: T << "\\thinspace "; break;
        case 3: T << "\\rm "; laststyle = 1; SELFONT; break;
        case 4: T << "\\it "; laststyle = 2; SELFONT; break;
        case 5: T << "\\bf "; laststyle = 3; SELFONT; break;
        case 6: T << "\\ss "; laststyle = 4; SELFONT; break;
        case 7: T << "\\si "; laststyle = 5; SELFONT; break;
      }
      continue;
    }
    
    if (wc==39) wc = 8217; // apostrophe -> quoteright;
    
    bool local_exit = false;
    bool local_repeat = true;
    bool is_accent = false;
    bool bot_accent = false;
    int bc = 0, alt = 0;  // base char, alternative char

    while(local_repeat) {
      local_repeat = false;
      for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++) {
        int j = J->id;
        for (int i=0; i<256; i++) {
          if (texenc[i][j] == wc || (is_accent && (texenc[i][j] == alt))) {
            if (j != lastenc) {           // do a better optimization
                                          // of the font changes in a string
              T << "\\thf" << u2str(j+1);//<< "{}";
              lastenc = j;
            }
            if (!is_accent) {
                           // some characters require special treatment in TeX
//              if (i==0 || i==127 || i==123 || i==125 || i==36 || i==38 || 
//                  i==10 || i==35 || i==95 || i==126 || i==13 || i==94 || 
//                  i==92 || i==37 || i < 32) {

//original                T << "\\char" << i << " ";
                T << "\\char" << i << " " << (remove_kerning ? "{}" : "");
//              }
//              else {
//                T << char(i);
////                if (bot_accent) T << " ";
//              }
            }
            else
              if (!bot_accent) {
                T << "\\accent" << i << " ";
              }
              else {
                T << "\\fixaccent{" << i << "}";
              }
            local_exit = true;
            local_repeat = false;
          }
          if (local_exit) break;
        }
        if (local_exit) break;
      }
      if (!local_exit && !is_accent) {

        // current character is not included in any of the used encodings;
        // let's try to find an approximation based on Unicode
        // decomposition table

        int a = 0, b = max_base - 1, i, j;
        while (a<=b) {
          i = (int) (a+b)/2;
          j = unibase[i][0];
          if (j == wc) {
            bc = unibase[i][1];
            wc = unibase[i][2];
            alt = -1;
            bot_accent = false;
            switch (wc) {
              case 768: alt =  96; break; // grave
              case 769: alt = 180; break; // acute
              case 770: alt = 710; break; // circumflex
              case 771: alt = 732; break; // tilde
              case 772: alt = 175; break; // macron
              case 774: alt = 728; break; // breve
              case 775: alt = 729; break; // dot above
              case 776: alt = 168; break; // diaeresis
              case 778: alt = 730; break; // ring above
              case 779: alt = 733; break; // double acute -- hungarian
              case 780: alt = 711; break; // caron
              case 807: alt = 184; bot_accent = true; break; // cedilla
              case 808: alt = 731; bot_accent = true; break; // ogonek
            }
            if (!bot_accent && bc == 105) bc = 305;   // dotless i correction
            if (!bot_accent && bc == 106) bc = 63166; // dotless j correction
            is_accent = true;
            local_repeat = true;
            break;
          }
          if (j > wc) 
            b = i - 1;
          else
            a = i + 1;
        } 
      }
      else if (is_accent) {
        wc = bc;
        is_accent = false;
        local_repeat = true;
        local_exit = false;
      }
    }
    if (!local_exit)       // we didn't find anything to display
    
      // it would be possible to add here various characters mapped
      // by plain TeX macros to mathematical fonts (backslash &c.)
      // This would require to make math fonts scalable with the \size[.] macro
    
      T << ".";
  }
  
  if (is_multiline) {
    T << "\\cr}}";
  }
  T << "\\mainfont{}";
  return T.str();
}

int tex2uni(string font, int ch) {
  int id = -1;
  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    if (J->rm == font || J->it == font || J->ss == font || J->si == font || J->bf == font) {
      id = J->id;
      break;
    }
  assert(id != -1);
  ch %= 256;
  if (ch < 0) ch += 256;  // if string is based on signed char
  return texenc[ch][id];
}


//const char * utf2tex (char * s) {
//  string t = utf2tex(string(s));
//  return t.c_str();
//}

#ifndef NOTHERION
const char * utf2tex (const char * s, bool b) {
  string t = utf2tex(string(s),b);
  thtexfontsbuff.strcpy(t.c_str());
  return thtexfontsbuff.get_buffer();
}
#endif

// For simlicity we suppose that all characters which are set by TeX macros
// are included in the first encoding specified. This concerns especially 
// page numbers, labels, and scalebar legend. Therefore we switch to \thfa
// after each size/style change

void print_fonts_setup() {
  ofstream P("th_enc.tex");
  if(!P) therror(("Can't write file"));
  P << "\\def\\rms{\\rm}" << endl;
  P << "\\def\\its{\\it}" << endl;
  P << "\\def\\bfs{\\bf}" << endl;
  P << "\\def\\sss{\\ss}" << endl;
  P << "\\def\\sis{\\si}" << endl;
  P << "\\def\\fixaccent#1#2 {{\\setbox0\\hbox{#2}\\ifdim\\ht0=1ex\\accent#1 #2%" << endl;
  P << "  \\else\\ooalign{\\unhbox0\\crcr\\hidewidth\\char#1\\hidewidth}\\fi}}" << endl;

  P << "\\def\\size[#1]{%" << endl;
  P << "  \\let\\prevstyle\\laststyle" << endl;
  P << "  \\baselineskip#1pt" << endl;
  P << "  \\baselineskip=1.2\\baselineskip" << endl;

  string firstfont = "\\thf" + u2str(FONTS.begin()->id+1);

  P << "  \\def\\rm{";
  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->rm << " at#1pt";
  P << "\\let\\laststyle\\rms" << firstfont << "}%" << endl;

  P << "  \\def\\it{";
  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->it << " at#1pt";
  P << "\\let\\laststyle\\its" << firstfont << "}%" << endl;

  P << "  \\def\\bf{";
  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->bf << " at#1pt";
  P << "\\let\\laststyle\\bfs" << firstfont << "}%" << endl;

  P << "  \\def\\ss{";
  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->ss << " at#1pt";
  P << "\\let\\laststyle\\sss" << firstfont << "}%" << endl;

  P << "  \\def\\si{";
  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->si << " at#1pt";
  P << "\\let\\laststyle\\sis" << firstfont << "}%" << endl;
  
  P << "  \\prevstyle" << endl;
  P << "}";
  P << "\\let\\laststyle\\rms" << endl;
  P << "\\size[10]\\ss" << endl;
  P << "\\def\\mainfont{" << firstfont << "}" << endl;
  P.close();
}


#ifdef NOTHERION
#ifndef NOMAIN
int main () {
//cout << "xl2 " << get_enc_index("xl2") << endl;
  return(0);
}
#endif
#endif

// obsolete:

//    else if (wc == 60) {                // special <.> string formatting
//      tmp = "";
//      I++;
//      while (I != s.end() && (wc = *I) != 62 && wc < 128) {
//        tmp += char(wc);
//        I++;
//      }
//      if (tmp == "br") T << "\\cr ";
//      else if (tmp == "center" || tmp == "left" || tmp == "right") ;
//      else if (tmp == "thsp") T << "\\thinspace ";
//      else T << "?";
//      if (I == s.end()) break;   // incorrect input (no closing `>')
//      continue;
//    }

