/**
 * @file thendscrap.cxx
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
 
#include "thendscrap.h"
#include "thexception.h"
#include "thdatabase.h"

thendscrap::thendscrap()
{
}


thendscrap::~thendscrap()
{
}


int thendscrap::get_class_id() 
{
  return TT_ENDSCRAP_CMD;
}


bool thendscrap::is(int class_id)
{
  if (class_id == TT_ENDSCRAP_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thendscrap::get_cmd_nargs() 
{
  return 0;
}


const char * thendscrap::get_cmd_end()
{
  return NULL;
}


const char * thendscrap::get_cmd_name()
{
  return "endscrap";
}


thcmd_option_desc thendscrap::get_cmd_option_desc(const char * /*opts*/)
{
  return TT_DATAOBJECT_UNKNOWN;
}


void thendscrap::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  if (cod.id == 1)
    cod.id = TT_DATAOBJECT_NAME;
    
  switch (cod.id) {

    case TT_DATAOBJECT_NAME:
      if (th_is_keyword(*args))
        this->name = this->db->strstore(*args);
      else 
        throw thexception(fmt::format("invalid keyword -- {}", *args));
      break;
      
    default:
      thdataobject::set(cod, args, argenc, indataline);
      
  }
}


void thendscrap::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thendscrap:\n");
}


int thendscrap::get_context()
{
  return THCTX_SCRAP;
}












