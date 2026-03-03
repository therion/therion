/**
 * @file thdb2dji.cxx
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
 
#include "thdb2dji.h"
#include "thdatabase.h"
#include "thexception.h"

#include <fmt/core.h>

thdb2dji::thdb2dji()
{
  this->prev_item = NULL;
  this->next_item = NULL;
  this->prev_list = NULL;
  this->next_list = NULL;
  this->prev_list_item = NULL;
  this->next_list_item = NULL;

  this->name.clear();
  this->mark = NULL;

  this->is_active = false;  
  this->point = NULL;
  this->cp1 = NULL;
  this->cp2 = NULL;
  this->line_point = NULL;
  this->object = NULL;
}

  
void thdb2dji::parse_item(char * istr)
{
/*
  thdb.buff_tmp.strcpy(istr);
  char * p_name = thdb.buff_tmp.get_buffer(), * s_name = "", * tmpch;
  tmpch = p_name;
  size_t snl = strlen(istr), sni;
  for(sni = 0; sni < snl; sni++, tmpch++)
    if (*tmpch == '@') {
      *tmpch = 0;
      s_name = tmpch + 1;
      break;
    }
  thsplit_strings(& thdb.db2d.mbf, p_name, ':');
*/
  thsplit_strings(& thdb.db2d.mbf, istr, ':');
  int npar = thdb.db2d.mbf.get_size();
  char ** pars = thdb.db2d.mbf.get_buffer();
  if (npar == 2) {
    this->mark = thdb.strstore(pars[1]);
    if (!th_is_keyword(this->mark))
      throw thexception(fmt::format("line mark not a keyword -- {}",istr));
  }
  /*
  thdb.buff_enc.strcpy(pars[0]);
  if (strlen(s_name) > 0) {
    thdb.buff_enc += "@";
    thdb.buff_enc += s_name;
  }*/
//  thparse_objectname(this->name, & thdb.buff_stations, thdb.buff_enc.get_buffer());
  thparse_objectname(this->name, & thdb.buff_stations, pars[0]);
}



