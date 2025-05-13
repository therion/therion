/**
 * @file thtexfonts.cxx
 */
  
/* Copyright (C) 2003-2008 Martin Budaj
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
#include <list>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <regex>

#include <cstring>
#include <cstdio>
#include <cmath>
#include <fmt/core.h>

#include "thtexfonts.h"
#include "thtexenc.h"
#include "thpdfdbg.h"
#include "thinit.h"
#include "therion.h"

std::list<fontrecord> FONTS;
typedef std::list<int> unistr;

encodings_new::encodings_new () {
  v_fon.resize(134);
//  v_fon.resize(127);
  for (int i = 0; i < 33; i++) {
    v_fon[i] = 0;
  }
  for (int i = 33; i < 127; i++) {  // printable ASCII
    v_fon[i] = i;
    m_fon[i] = i;
  }
  v_fon[127] = 0xFB00; m_fon[0xFB00] = 127;  // ff
  v_fon[128] = 0xFB01; m_fon[0xFB01] = 128;  // fi
  v_fon[129] = 0xFB02; m_fon[0xFB02] = 129;  // fl
  v_fon[130] = 0xFB03; m_fon[0xFB03] = 130;  // ffi
  v_fon[131] = 0xFB04; m_fon[0xFB04] = 131;  // ffl
  v_fon[132] = 0x2013; m_fon[0x2013] = 132;  // en dash
  v_fon[133] = 0x2014; m_fon[0x2014] = 133;  // em dash

  i_fon = 0;
  
  NFSS = 0;
  t1_convert = 1;
}

int encodings_new::get_enc_pos (int ch) {
  if (m_fon.find(ch) == m_fon.end()) {
    if (i_fon < 33) {
      v_fon[i_fon] = ch;
      m_fon[ch] = i_fon;
      i_fon++;
    } else {
      v_fon.push_back(ch);
      m_fon[ch] = v_fon.size()-1;
    }
  }
//cout << "==FP== " << m_fon[style][ch] << '\n';
  return m_fon[ch];
}

void encodings_new::write_enc_files() {
  if (NFSS==0) return;

  thprintf("generating TeX metrics ... ");
  std::string style[5] = {"rm", "it", "bf", "ss", "si"};
  std::string s, fc;

  std::ofstream H ("thfonts.map"); // delete previous file, we will append to it below
  if (!H) therror(("could not write font mapping data for pdfTeX\n"));
  H.close();

  int fcount = get_enc_count();
  for (int j = 0; j < fcount; j++) {
    fc = fmt::format("{:02d}", j);
    std::string fname_enc = std::string("th_enc")+fc+".enc";
    
    std::ofstream F(fname_enc.c_str());
    if (!F) therror(("could not write encoding file\n"));
    F << "% LIGKERN uni002D uni002D =: uni2013 ; uni2013 uni002D =: uni2014 ;\n";
    F << "% LIGKERN uni0066 uni0066 =: uniFB00 ; uni0066 uni006C =: uniFB02 ; uni0066 uni0069 =: uniFB01 ; uniFB00 uni0069 =: uniFB03 ; uniFB00 uni006C =: uniFB04 ;\n";
    F << "/" << fname_enc << "[\n";
    for (int k=0; k < 256; k++) {
//cout << ccount << "** i:" << i << " j:" << j << " k:" << k << " "  << v_fon[i][k] << '\n';
      if ((v_fon.size() <= ((unsigned) 256*j + k)) || (v_fon[256*j + k] == 0)) 
        F << "/.notdef\n";
      else
        F << "/uni" << std::setw(4) << std::setfill('0') << std::hex << std::noshowbase << std::uppercase << v_fon[256*j + k] << '\n';
    }
    F << "] def\n";
    F.close();

    for (int i=0; i<5; i++) {
      std::string fname_tfm = "th"+style[i]+fc;  // convention used also in tex2uni
      
      // we don't use -fliga to turn ligatures on, because between
      // subsequent runs (metapost, pdftex) the meaning of ligatures 
      // positioned at the end of the encoding would change
      // if more characters are present at the second run
      // -- a few ligatures are initialised in the constructor
      
      std::string type1 = (t1_convert==1) ? " " : " --no-type1 ";

      if (system(("\"" + std::string(thini.get_path_otftotfm()) + "\" -e " + fname_enc +
        " -fkern --no-default-ligkern --no-virtual --name " + fname_tfm +
//        " -fkern --no-default-ligkern --name " + fname_tfm +
//        type1 + " --warn-missing "+otf_file[i]+" > thotftfm.tmp").c_str()) > 0)
        type1 + otf_file[i]+" > thotftfm.tmp").c_str()) != 0)
          therror((("can't generate TFM file from "+otf_file[i]+" (LCDF typetools not installed?)").c_str()));
      std::ifstream G ("thotftfm.tmp");
      if (!G) therror(("could not read font mapping data\n"));
      while (G) {
        std::getline(G,s);
        if (s.find("<") != std::string::npos) break;
      }
      if (s.size() < 10) therror(("no usable otftotfm output"));
      if (s.substr(s.size()-3,3)=="otf" || s.substr(s.size()-3,3)=="OTF") {
        s.replace(s.rfind("<"), 1, "<<");  // OTF fonts must be fully embedded
      }
      G.close();
      std::ofstream H ("thfonts.map", std::ios::app); 
      if (!H) therror(("could not write font mapping data for pdfTeX\n"));
      H << "\\pdfmapline{+" << s << "}\n";
      H.close();
    }
  }
  thprintf("done\n");
}

int encodings_new::get_enc_count () {
  return v_fon.size() / 256 + 1;
}

int encodings_new::get_uni (int f, int ch) {
  return v_fon[256*f + ch];
}

encodings_new ENC_NEW;

// returns the index of given encoding (as used in predefined encoding arrays)
// or -1 if such an encoding is not known

int get_enc_id(const char * enc) {
  for (int i=0; i<max_enc; i++) if (strcmp(encodings[i],enc) == 0) return i;
  return -1;
}

void print_tex_encodings (void) {
  for (int i=0; i<max_enc; i++) std::cout << encodings[i] << '\n';
  std::cout << std::flush;
}

void init_encodings() {
  fontrecord F;
  F.id = get_enc_id("raw");
  F.rm = "cmr10";
  F.it = "cmti10";
  F.bf = "cmbx10";
  F.ss = "cmss10";
  F.si = "cmssi10";
  F.opt = false;
  FONTS.push_back(F);
  F.id = get_enc_id("xl2");
  F.rm = "csr10";
  F.it = "csti10";
  F.bf = "csbx10";
  F.ss = "csss10";
  F.si = "csssi10";
#ifdef THWIN32
  F.opt = false;
#else
  F.opt = true;
#endif
  FONTS.push_back(F);
  F.id = get_enc_id("cmcyr");
  F.rm = "cmcyr10";
  F.it = "cmcti10";
  F.bf = "cmcbx10";
  F.ss = "cmcss10";
  F.si = "cmcssi10";
#ifdef THWIN32
  F.opt = false;
#else
  F.opt = true;
#endif
  FONTS.push_back(F);
}

unistr utf2uni(std::string s) {
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

std::string utf2texoctal(std::string str) {
  unistr s = utf2uni(str);
  std::string t;
//  t = "\\ne\\376\\ne\\377";
  unsigned char c;
  for (unistr::iterator I = s.begin(); I != s.end(); I++) {
    c = (*I) / 256;
    t = t + "\\ne\\" + fmt::format("{:o}",c);
    c = (*I) % 256;
    t = t + "\\ne\\" + fmt::format("{:o}",c);
  }
  return t;
}

// converts utf8 to 2B Unicode in a special format for pdfTeX

std::string utf2texhex(std::string str) {
  unistr s = utf2uni(str);
  std::string t;
  unsigned char c;
  for (unistr::iterator I = s.begin(); I != s.end(); I++) {
    c = (*I) / 256;
    t += fmt::format("{:02x}",c);
    c = (*I) % 256;
    t += fmt::format("{:02x}",c);
  }
  return t;
}

std::string replace_all(std::string s, std::string f, std::string r) {
  size_t found = s.find(f);
  while(found != std::string::npos) {
    s.replace(found, f.length(), r);
    found = s.find(f);
  }
  return s;
}

// easier to use brute force than to link regex on all platforms :(
std::string select_lang(std::string s, std::string lang) {
  size_t i,j;
  if (s.find("<lang:") != std::string::npos) {
    i = s.find("<lang:"+lang+">");
    if (i != std::string::npos) {  // precise match
      i = s.find(">",i);
      j = s.find("<lang:",i);
      return s.substr(i+1,(j==std::string::npos? std::string::npos : j-i-1));
    }
    if (lang.length()==5) {
      lang = lang.substr(0,2);
      i = s.find("<lang:"+lang+">");
      if (i != std::string::npos) {  // match main language part
        i = s.find(">",i);
        j = s.find("<lang:",i);
        return s.substr(i+1,(j==std::string::npos? std::string::npos : j-i-1));
      }
    }
    i = s.find("<lang:"+lang);
    if (i != std::string::npos) {  // match any of dialects if no main part is present
      i = s.find(">",i);
      j = s.find("<lang:",i);
      return s.substr(i+1,(j==std::string::npos? std::string::npos : j-i-1));
    }
    i = s.find("<lang:");  // no match, use everything before first lang as a default
    return s.substr(0,i);
  }
  return s;
}

// main task is done here

std::regex reg_fontsize(R"(<size:((\d{1,3})(\%)?|x?s|x?l|m)>)");

#define SELFONT if (ENC_NEW.NFSS == 0 && lastenc!=-1) T << "\\thf" << u2str(lastenc+1)

std::string utf2tex(std::string str, bool remove_kerning) {
  if (str.empty()) return str;

  std::ostringstream T;
  int wc;  //wide char
  int lastenc = -1;
  int laststyle = -1;
  std::string setsize;
  int align = 0;
  int tmpint;
  bool rtl = false;
  // bool link_active = false;
  bool is_multiline = false;

//  str = select_lang(str, LAYOUT.langstr);

  if (str.find("<center>") != std::string::npos) align = 1;
  else if (str.find("<centre>") != std::string::npos) align = 1;
  else if (str.find("<left>") != std::string::npos) align = 0;
  else if (str.find("<right>") != std::string::npos) align = 2;

  if (str.find("<br>") != std::string::npos) is_multiline = true;
 
  str = replace_all(str,"<center>","");
  str = replace_all(str,"<centre>","");
  str = replace_all(str,"<left>","");
  str = replace_all(str,"<right>","");
  str = replace_all(str,"<br>","\x1B\x1");   // \e not accepted in VC++
  str = replace_all(str,"<thsp>","\x1B\x2");
  str = replace_all(str,"<rm>","\x1B\x3");
  str = replace_all(str,"<it>","\x1B\x4");
  str = replace_all(str,"<bf>","\x1B\x5");
  str = replace_all(str,"<ss>","\x1B\x6");
  str = replace_all(str,"<si>","\x1B\x7");
  str = replace_all(str,"<rtl>","\x1B\x8");
  str = replace_all(str,"</rtl>","\x1B\x9");

  // An attempt to implement hyperlinks, unsuccessful though:
  // ! pdfTeX error (ext4): link annotations cannot be inside an XForm.
  // <recently read> \xxxx 
  //// links
//  size_t i=-1,j;
//  while((i = str.find("<link:",i+1)) != string::npos) {
//      j = str.find(">",i);
//      if (j == string::npos) therror(("No closing '>' in <link:...> definition"));
//      str = str.replace(j,1,"\x1B\x0B");
//      str = str.replace(i,6,"\x1B\x0A");
//  }
//  str = replace_all(str,"</link>","\x1B\x0C");
  //// endlinks

  // font size; see https://stackoverflow.com/a/48371552
  std::string out;
  std::string::const_iterator it = str.cbegin(), end = str.cend();
  for (std::smatch match; std::regex_search(it, end, match, reg_fontsize); it = match[0].second) {
    if (match.str(1) == "xs")
      out += match.prefix().str() + "\x1B\xE" + (char) 101;
    else if (match.str(1) == "s")
      out += match.prefix().str() + "\x1B\xE" + (char) 102;
    else if (match.str(1) == "m")
      out += match.prefix().str() + "\x1B\xE" + (char) 103;
    else if (match.str(1) == "l")
      out += match.prefix().str() + "\x1B\xE" + (char) 104;
    else if (match.str(1) == "xl")
      out += match.prefix().str() + "\x1B\xE" + (char) 105;
    else if (match.str(3) == "%")  // font size in percents; 10 % increments to fit the values into <1,100> range
      out += match.prefix().str() + "\x1B\xE" + (char) (std::max((int) std::round(std::stod(match.str(2)) / 10), 1));
    else if (match.str(1) == match.str(2))   // font size in points; limited to <1,127>
      out += match.prefix().str() + "\x1B\xD" + (char) (std::min(std::max(std::stoi(match.str(2)),1),127));
    else therror(("invalid font size specification"));
  }
  out.append(it, end);
  str = out;

  if (is_multiline) {
    T << "\\vtop{\\halign{";
    if (align > 0) T << "\\hfil";
    T << "#";
    if (align < 2) T << "\\hfil";
    T << "\\cr";
  }
  
  unistr s = utf2uni(str);

  for (unistr::iterator I = s.begin(); I != s.end(); I++) {
    wc = *I;
    if (wc == 32 || wc == 9) {          // space requires special treatment 
      T << "\\ ";                       // (it's not included in TeX fonts)
      continue;                         // so encodings search doesn't help
    }
    else if (wc == 27) {                // escaped chars (special formatting)
      wc = *(++I);
      switch (wc) {
        case 0x1: 
          if (rtl) T << "\\endR";
          T << "\\cr";
          if (rtl) T << "\\beginR";
          switch (laststyle) {
            case 1: T << "\\rm"; break;
            case 2: T << "\\it"; break;
            case 3: T << "\\bf"; break;
            case 4: T << "\\ss"; break;
            case 5: T << "\\si"; break;
          }
          if (setsize != "") T << setsize;
          SELFONT; T << " ";
          break;
        case 0x2: T << "\\thinspace "; break;
        case 0x3: T << "\\rm "; laststyle = 1; SELFONT; break;
        case 0x4: T << "\\it "; laststyle = 2; SELFONT; break;
        case 0x5: T << "\\bf "; laststyle = 3; SELFONT; break;
        case 0x6: T << "\\ss "; laststyle = 4; SELFONT; break;
        case 0x7: T << "\\si "; laststyle = 5; SELFONT; break;
        case 0x8: T << "\\global\\TeXXeTstate=1\\beginR "; rtl = true; break;
        case 0x9: T << "\\endR "; rtl = false; break;
        //// links
//        case 0xA: if (link_active) T << "\\pdfendlink ";
//                 T << "\\pdfstartlink attr {/Border [0 0 0]} goto name {"; 
//                 link_active = true; 
//                 break;
//        case 0xB: T << "}"; break;
//        case 0xC: if (link_active) {
//                   T << "\\pdfendlink "; 
//                   link_active = false; 
//                 }
//                 break;
        //// endlinks
        case 0xD: setsize = fmt::format("\\size[{}]", (int) *(++I)); T << setsize; SELFONT; break;
        case 0xE:
          tmpint = (int) *(++I);
          switch (tmpint) {
            case 101: setsize = "\\thtinysize "; break;
            case 102: setsize = "\\thsmallsize "; break;
            case 103: setsize = "\\thnormalsize "; break;
            case 104: setsize = "\\thlargesize "; break;
            case 105: setsize = "\\thhugesize "; break;
            // 1-100: size in percents
            default: setsize = fmt::format("\\size[\\numexpr {}*\\basefontsize/10\\relax]", tmpint); break;
          }
          T << setsize;
          SELFONT;
          break;
      }
      continue;
    }
    
    
if (ENC_NEW.NFSS==0) {    
    
    if (wc==39) wc = 8217; // apostrophe -> quoteright;
    
    bool local_exit = false;
    bool local_repeat = true;
    bool is_accent = false;
    bool bot_accent = false;
    int bc = 0, alt = 0;  // base char, alternative char

    while(local_repeat) {
      local_repeat = false;
      for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++) {
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

      switch (wc) {
        case  160: T << "~";   break;  // no-break space
        case 8722: T << "--";  break;  // unicode minus
        default:   T << ".";   break;
      }

} else { // NFSS == 1
      int k = ENC_NEW.get_enc_pos(wc);
      int f_ind = k / 256;
      if (f_ind != lastenc) {
        T << "\\thf" << u2str(f_ind+1);
        lastenc = f_ind;
      }
      T << "\\char" << k % 256 << " " << (remove_kerning ? "{}" : "");
}  


  } // endfor
  
  
  if (is_multiline) {
    T << "\\cr}}";
  }
  T << "\\mainfont{}";
  if (rtl) T << "\\endR ";  // change from the links experiment
  return T.str();
}

int tex2uni(std::string font, int ch) {
  if (ENC_NEW.NFSS==0) {
    int id = -1;
    for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
      if (J->rm == font || J->it == font || J->ss == font || J->si == font || J->bf == font) {
        id = J->id;
        break;
      }
    //thassert(id != -1);
    ch %= 256;
    if (ch < 0) ch += 256;  // if string is based on signed char
    if (id == -1) {
      std::ostringstream s;
      s << "can't map character 0x" << std::uppercase << std::setfill('0') << 
           std::setw(2) << std::hex << ch << 
           " in font '" << font << "' to unicode";
      therror((s.str().c_str()));
    }
    return texenc[ch][id];
  } else {  // NFSS
    std::string f_ind = font.substr(4,2);
    // basic check that we have a number
    thassert(f_ind[0] >= '0' && f_ind[0] <= '9' && f_ind[1] >= '0' && f_ind[1] <= '9');
    if (ch < 0) ch += 256;  // if string is based on signed char
    return ENC_NEW.get_uni(atoi(f_ind.c_str()),ch);
  }
}

// For simplicity we suppose that all characters which are set by TeX macros
// are included in the first encoding specified. This concerns especially 
// page numbers, labels, and scalebar legend. Therefore we switch to \thfa
// after each size/style change

void print_fonts_setup() {
  std::ofstream P("th_enc.tex");  // included also in MetaPost
  if(!P) therror(("Can't write file th_enc.tex"));
  P << "\\def\\rms{\\rm}\n";
  P << "\\def\\its{\\it}\n";
  P << "\\def\\bfs{\\bf}\n";
  P << "\\def\\sss{\\ss}\n";
  P << "\\def\\sis{\\si}\n";

if (ENC_NEW.NFSS==0) {
  P << "\\def\\fixaccent#1#2 {{\\setbox0\\hbox{#2}\\ifdim\\ht0=1ex\\accent#1 #2%\n";
  P << "  \\else\\ooalign{\\unhbox0\\crcr\\hidewidth\\char#1\\hidewidth}\\fi}}\n";

  P << "\\def\\size[#1]{%\n";
  P << "  \\let\\prevstyle\\laststyle\n";
  P << "  \\baselineskip#1pt\n";
  P << "  \\baselineskip=1.2\\baselineskip\n";

  std::string firstfont = "\\thf" + u2str(FONTS.begin()->id+1);

  P << "  \\def\\rm{";
  for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->rm << " at#1pt";
  P << "\\let\\laststyle\\rms" << firstfont << "}%\n";

  P << "  \\def\\it{";
  for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->it << " at#1pt";
  P << "\\let\\laststyle\\its" << firstfont << "}%\n";

  P << "  \\def\\bf{";
  for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->bf << " at#1pt";
  P << "\\let\\laststyle\\bfs" << firstfont << "}%\n";

  P << "  \\def\\ss{";
  for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->ss << " at#1pt";
  P << "\\let\\laststyle\\sss" << firstfont << "}%\n";

  P << "  \\def\\si{";
  for (std::list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++)
    P << "\\font\\thf" << u2str(J->id+1) << "=" << J->si << " at#1pt";
  P << "\\let\\laststyle\\sis" << firstfont << "}%\n";
  
  P << "  \\prevstyle\n";
  P << "}";
  P << "\\let\\laststyle\\rms\n";
  P << "\\size[10]\\ss\n";
  P << "\\def\\mainfont{" << firstfont << "}\n";
} else {
  std::string styledef[5] = {"rm", "it", "bf", "ss", "si"};
  P << "\\def\\size[#1]{%\n";
  P << "  \\let\\prevstyle\\laststyle\n";
  P << "  \\baselineskip#1pt\n";
  P << "  \\baselineskip=1.2\\baselineskip\n";
  
  for (int j=0; j<5; j++) {
    P << "  \\def\\" << styledef[j] << "{";
    for (int i = 0; i < ENC_NEW.get_enc_count(); i++) 
      P << "\\font\\thf" << u2str(i+1) << "=th" << styledef[j] << 
      std::setw(2) << std::setfill('0') << i << " at#1pt";
    P << "\\let\\laststyle\\" << styledef[j] << "s\\thfa}%\n";
    }
  P << "  \\prevstyle\n";
  P << "}";
  P << "\\let\\laststyle\\rms\n";
  P << "\\size[10]\\ss\n";
  P << "\\def\\mainfont{\\thfa}\n";
}  

  // needs to be defined here as this file (th_enc.tex) is included by MP before processing of the labels
  P << R"(
\ifx\TeXXeTstate\undefined
  \ifx\directlua\undefined
    \let\beginR\relax
    \let\endR\relax
  \else
    \def\beginR{\textdir TRT}
    \def\endR{\textdir TLT}
  \fi
  \def\TeXXeTstate=#1{\def\blbost{}}  % for using \global\TeXXeTstate
\fi
)";

  P.close();
}

