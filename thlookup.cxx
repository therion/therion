/**
 * @file thlookup.cxx
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
 
#include "thlookup.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdata.h"
#include "thparse.h"
#include "thinfnan.h"
#include "thpdfdata.h"
#include "thsymbolset.h"
#include "thtflength.h"
#include "thlang.h"
#include "thcsdata.h"
#include "thconfig.h"
#include <string.h>
#ifdef THMSVC
#include <direct.h>
#define getcwd _getcwd
#endif


enum {
  TTLDBG_JOINS = 1,
  TTLDBG_STATIONS = 2,
  TTLDBG_SCRAPNAMES = 4,
  TTLDBG_STATIONNAMES = 8,
};



thlookup::thlookup()
{
  this->m_type = TT_LOOKUP_TYPE_UNKNOWN;
  this->m_scale = TT_LOOKUP_SCALE_UNKNOWN;
  this->m_title = "";
}


thlookup::~thlookup()
{
}


int thlookup::get_class_id() 
{
  return TT_LOOKUP_CMD;
}


bool thlookup::is(int class_id)
{
  if (class_id == TT_LOOKUP_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thlookup::get_cmd_nargs() 
{
  return 1;
}


const char * thlookup::get_cmd_end()
{
  return "endlookup";
}


const char * thlookup::get_cmd_name()
{
  // insert command name here
  return "lookup";
}


thcmd_option_desc thlookup::get_default_cod(int id) {
  return thcmd_option_desc(id);
}


thcmd_option_desc thlookup::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_lookup_opt);
  if (id == TT_LOOKUP_UNKNOWN) 
    return thdataobject::get_cmd_option_desc(opts);
  else 
    return this->get_default_cod(id);
}


void thlookup::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  double dum;
  int sv, sv2, dum_int;

  thcmd_option_desc defcod = this->get_default_cod(cod.id);
  switch (cod.id) {
    case TT_DATAOBJECT_AUTHOR:
    case TT_DATAOBJECT_COPYRIGHT:
      defcod.nargs = 2;
    default:
      if (cod.nargs > defcod.nargs)
        ththrow(("too many arguments -- %s", args[defcod.nargs]))
  }
  
  switch (cod.id) {

    case 0:
	  // add table row
      break;
    
	case TT_LOOKUP_TITLE:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->m_title = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->m_title = "";
      break;
    
    case 1:
      cod.id = TT_DATAOBJECT_NAME;
    default:
      thdataobject::set(cod, args, argenc, indataline);
      break;
  }
}


void thlookup::self_delete()
{
  delete this;
}


int thlookup::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}




void thlookup_table_row::parse(char ** args) {
}



