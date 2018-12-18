/**
 * @file proj.cxx
 */
  
/* Copyright (C) 2006 Martin Budaj
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

#include "thcs.h"
#include "thparse.h"
#include "thcsdata.h"
#include "thexception.h"
#include <proj_api.h>
#include "thdatabase.h"
#include <map>
#include <string>
#include <list>

std::map<std::string, int> thcs_custom_name2int;
std::map<int, std::string> thcs_custom_int2name;
std::map<int, thcsdata*> thcs_custom_int2data;
std::list<thcsdata> thcs_custom_data;
int thcs_custom_id = TTCS_UNKNOWN;


int thcs_parse(const char * name)
{
  int sv = thcasematch_token(name, thtt_cs);
  if (sv == TTCS_UNKNOWN) {
    std::map<std::string, int>::iterator it = thcs_custom_name2int.find(std::string(name));
    if (it != thcs_custom_name2int.end()) return it->second;
  }
  return sv;
}


const char * thcs_get_name(int cs)
{
  size_t tab_size = ((sizeof(thtt_cs)/sizeof(thstok)) - 1);
  const char * csstr;
  long i, ii;
  i = 0;
  ii = -1;
  while (i < (long)tab_size) {
    if (thtt_cs[i].tok == cs) {
      ii = i;
      if (strlen(thtt_cs[ii].s) < 6)
        break;
      if (!(((thtt_cs[ii].s[0] == 'e' || thtt_cs[ii].s[0] == 'E') && 
        (thtt_cs[ii].s[1] == 's' || thtt_cs[ii].s[1] == 'S') && 
        (thtt_cs[ii].s[2] == 'r' || thtt_cs[ii].s[2] == 'R') && 
        (thtt_cs[ii].s[3] == 'i' || thtt_cs[ii].s[3] == 'I')) || 
        ((thtt_cs[ii].s[0] == 'e' || thtt_cs[ii].s[0] == 'E') && 
        (thtt_cs[ii].s[1] == 'p' || thtt_cs[ii].s[1] == 'P') && 
        (thtt_cs[ii].s[2] == 's' || thtt_cs[ii].s[2] == 'S') && 
        (thtt_cs[ii].s[3] == 'g' || thtt_cs[ii].s[3] == 'G'))))
        break;
    }
    i++;
  }
  if (ii > -1)
    csstr = thtt_cs[ii].s;
  else if (cs < TTCS_UNKNOWN)
    return thcs_custom_int2name[cs].c_str();
  else
    csstr = "unknown";
  return csstr;
}

const thcsdata * thcs_get_data(int cs) {
  if (cs >= 0) return &(thcsdata_table[cs]);
  if (cs < TTCS_UNKNOWN) return thcs_custom_int2data[cs];
  return NULL;
}

void thcs_add_cs(char * id, char * proj4id, size_t nargs, char ** args)
{
  if (thcs_parse(id) != TTCS_UNKNOWN) ththrow(("cs already exists -- %s", id));
  if (!th_is_extkeyword(id)) ththrow(("invalid cs identifier -- %s", id));
  projPJ P1;
  if ((P1 = pj_init_plus(proj4id))==NULL) 
    ththrow(("invalid proj4 identifier -- %s", proj4id));
  pj_free(P1);
  thcsdata * pd = &(*thcs_custom_data.insert(thcs_custom_data.end(), thcsdata()));
  pd->prjspec = "";
  pd->params = thdb.strstore(proj4id);
  pd->swap = false;
  pd->output = true;
  pd->dms = false;
  thcs_custom_id--;
  thcs_custom_name2int[std::string(id)] = thcs_custom_id;
  thcs_custom_int2name[thcs_custom_id] = std::string(id);
  thcs_custom_int2data[thcs_custom_id] = pd;
}
