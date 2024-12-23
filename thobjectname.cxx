/**
 * @file thobjectname.cxx
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "thobjectname.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thdataobject.h"
#include "thsurvey.h"

#include <fmt/printf.h>

thobjectname::thobjectname()
{
  this->clear();
}

void thobjectname::clear()
{
  this->name = NULL;
  this->survey = NULL;
  this->psurvey = NULL;
  this->id = 0;
}

bool thobjectname::is_empty()
{
  return (this->name == NULL);
}

void thparse_objectname(thobjectname & ds, thmbuffer * sstore, char * src, thdataobject * psobj)
{
  if ((psobj != NULL) && ((psobj->stnsuff != NULL) || (psobj->stnpref != NULL))) {
    if (psobj->stnpref != NULL)
      thdb.buff_tmp = psobj->stnpref;
    else
      thdb.buff_tmp = "";
    thdb.buff_tmp += src;
    if (psobj->stnsuff != NULL)
      thdb.buff_tmp += psobj->stnsuff;
    src = thdb.buff_tmp.get_buffer();
  }
  ds.name = src;
  size_t snl = strlen(src), sni;
  for(sni = 0; sni < snl; sni++, src++)
    if (*src == '@') {
      *src = 0;
      ds.survey = src + 1;
      break;
    }
  if (!th_is_extkeyword(ds.name))
    ththrow("invalid object name -- {}", ds.name);
  if (ds.survey != NULL)
    if (!th_is_keyword_list(ds.survey, '.'))
      ththrow("invalid survey name -- {}", ds.survey);
  ds.name = sstore->append(ds.name);
  if (ds.survey != NULL)
    ds.survey = sstore->append(ds.survey);
  ds.psurvey = thdb.get_current_survey();
}


void fprintf(FILE * fh, thobjectname & ds)
{
  if (ds.name != NULL)
    fprintf(fh, "%s", ds.name);
  else
    fprintf(fh, "-");
  if (ds.survey != NULL)
    fprintf(fh, "@%s", ds.survey);
  if (ds.id > 0)
    fprintf(fh, ":%ld", ds.id);
}


std::string thobjectname::print_name()
{
  std::string name_str, survey_str;
  if (this->name) {
    name_str = this->name;
  }
  if (this->survey) {
    survey_str = this->survey;
  }
  if (!name_str.empty() && !survey_str.empty()) {
    return fmt::sprintf("%s@%s", name_str, survey_str);
  }
  if (!name_str.empty()) {
    return name_str;
  }
  return survey_str;
}
  

std::string thobjectname_print_full_name(const char * oname_ptr, thsurvey * psrv, int slevel)
{
  std::string_view sname;
  std::string_view oname;
  if (psrv && psrv->get_full_name()) {
    sname = psrv->get_full_name();
  }
  if (oname_ptr) {
    oname = oname_ptr;
  }

  std::string rv;
  size_t start = 0;
  if (!oname.empty() && !sname.empty() && (slevel != 0)) {
    const char sep = (oname.find('@') == oname.npos) ? '@' : '.';
    rv = fmt::sprintf("%s%c%s", oname, sep, sname);
    start = oname.size() + 1;
  } else if (!oname.empty()) {
    rv = oname;
  } else if (!sname.empty() && (slevel != 0)) {
    rv = sname;
  }
  if (!sname.empty() && (slevel > 0)) {
    int clevel = 0;
    for(size_t cx = start; cx < rv.size(); cx++) {
      if (rv[cx] == '.') {
        clevel++;
        if (clevel == slevel)  {
          rv = rv.substr(0, cx);
          break;
        }
      }
    }
  }
  return rv;
}

std::string thobjectname::print_full_name(int slevel)
{
  return thobjectname_print_full_name(this->print_name().c_str(), this->psurvey, slevel);
}

