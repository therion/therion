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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#include "thobjectname.h"
#include "thexception.h"
#include "thparse.h"
#include "thdatabase.h"
#include "thdataobject.h"
#include "thsurvey.h"

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


char * thobjectname::print_name()
{
  static thbuffer pname;
  size_t plen, slen;
  char * rv;
  plen = (this->name != NULL ? strlen(this->name) : 0);
  slen = (this->survey != NULL ? strlen(this->survey) : 0);
  pname.guarantee(plen + slen + 1);
  rv = pname.get_buffer();
  rv[0] = 0;
  if ((plen > 0) && (slen > 0)) {
    snprintf(rv, pname.size, "%s@%s", this->name, this->survey);
  } else if (plen > 0) {
    snprintf(rv, pname.size, "%s", this->name);
  } else if (slen > 0) {
    snprintf(rv, pname.size, "%s", this->survey);
  }
  return rv;
}
  

char * thobjectname_print_full_name(const char * oname, thsurvey * psrv, int slevel)
{
  static thbuffer pname;
  size_t plen, slen, start, cx, tx;
  int clevel;
  char * rv;
  const char * sname;
  sname = NULL;
  if (psrv != NULL) 
    sname = psrv->get_full_name();
  plen = (oname != NULL ? strlen(oname) : 0);
  slen = (sname != NULL ? strlen(sname) : 0);
  pname.guarantee(plen + slen + 1);
  rv = pname.get_buffer();
  rv[0] = 0;
  if ((plen > 0) && (slen > 0) && (slevel != 0)) {
    snprintf(rv, pname.size, "%s@%s", oname, sname);
  } else if (plen > 0) {
    snprintf(rv, pname.size, "%s", oname);
  } else if ((slen > 0) && (slevel != 0)) {
    snprintf(rv, pname.size, "%s", sname);
  }
  if ((slen > 0) && (slevel > 0)) {
    start = (plen > 0 ? plen + 1 : 0);
    clevel = 0;
    tx = strlen(rv);
    for(cx = start; cx < tx; cx++) {
      if (rv[cx] == '.') {
        clevel++;
        if (clevel == slevel)  {
          rv[cx] = 0;
          break;
        }
      }
    }
  }
  return rv;
}

char * thobjectname::print_full_name(int slevel)
{
  return thobjectname_print_full_name(this->name, this->psurvey, slevel);
}

