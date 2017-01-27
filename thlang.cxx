/**
 * @file thlang.cxx
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
 
#include "thlang.h"
#include "thparse.h"
#include "thlangdata.cxx"
#include "thinit.h"
#include "thconfig.h"
#include "thdatabase.h"
#include "thexception.h"
#include <map>


struct thlang_str {

  int m_lang;

  const char * m_text;

  thlang_str(int lang, const char * text) : m_lang(lang), m_text(text) {}

};

bool operator < (const struct thlang_str & s1, const struct thlang_str & s2)
{
  if (s1.m_lang < s2.m_lang) return true;
  if (s1.m_lang > s2.m_lang) return false;
  if (strcmp(s1.m_text, s2.m_text) < 0) return true;
  return false;
}

bool operator == (const struct thlang_str & s1, const struct thlang_str & s2)
{
  if ((s1.m_lang == s2.m_lang) && (strcmp(s1.m_text, s2.m_text) == 0)) return true;
  return false;
}

typedef std::map<thlang_str, const char *> thlang_str_map;

thlang_str_map ulang_map;

int thlang_parse(const char * str) {
  // Therion used to use the incorrect code "en_UK" for British English, so
  // map this to the correct code for backward compatibility.
  if (strcmp(str, "en_UK") == 0) {
    str = "en_GB";
  }
  return thmatch_token(str, thtt_lang);
}

const char * thlang_getid(int id) {
  if (id < 0) {
    return "en";
  }
  const char * rv = thmatch_string(thlang_getlang(id), thtt_lang);
  return rv;
}

const char * thlang_getcxxid(int id) {
  if (id < 0)
    return "THLANG_UNKNOWN";
  else
    return thlang__cxxids[id];
}

const thstok * thlang_get_text_table()
{
  return &(thtt__texts[0]);
}

void thlang_set_translation(char * lang, char * text, char * translation) {
  
  int lang_id;
  lang_id = thlang_parse(lang);
  if (lang_id == THLANG_UNKNOWN)
    ththrow(("unknown language -- %s", lang));
  int text_id;
  text_id = thmatch_token(text, thtt__texts);
  if (text_id == -1) {
    if (
      (strncmp(text,"point u:",8) == 0) ||
      (strncmp(text,"line u:",7) == 0) ||
      (strncmp(text,"area u:",7) == 0)) {
        ulang_map[thlang_str(lang_id, thdb.strstore(text, true))] = thdb.strstore(translation);
    } else
      ththrow(("unknown text -- %s", text))
  } else { 
    thlang__translations[text_id][lang_id] = thdb.strstore(translation);
  }
}

const char * thT(const char * txt, int lng) {
  if (lng == THLANG_SYSTEM)
    return txt;
  int sv = thmatch_token(txt,thtt__texts);
  const char * trans;
  lng = thlang_getlang(lng);
  // najde ci ho mame v danom jazyku
  if (sv == -1) {
    thlang_str_map::iterator it = ulang_map.find(thlang_str(lng, txt));
    if ((it == ulang_map.end()) && (thlang__alternatives[lng] != THLANG_UNKNOWN))
      it = ulang_map.find(thlang_str(thlang__alternatives[lng], txt));
    if (it == ulang_map.end())
      return txt;
    else
      return it->second;
  }
  trans = thlang__translations[sv][lng];
  if ((trans == NULL) && (thlang__alternatives[lng] != THLANG_UNKNOWN)) {
    trans = thlang__translations[sv][thlang__alternatives[lng]];
  }
  if (trans != NULL)
    return trans;
  else
    return txt;
}

int thlang_getlang(int lng) {
  if (lng == THLANG_UNKNOWN)
    lng = thcfg.lang;
  if (lng == THLANG_UNKNOWN)
    lng = thini.lang;
  if (lng == THLANG_UNKNOWN)
    lng = THLANG_EN;
  return lng;
}


