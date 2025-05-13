/**
 * @file thcomment.cxx
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
 
#include "thcomment.h"
#include "thdatabase.h"

thcomment::thcomment()
{
}


thcomment::~thcomment()
{
}


int thcomment::get_class_id() 
{
  return TT_COMMENT_CMD;
}


bool thcomment::is(int class_id)
{
  if (class_id == TT_COMMENT_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thcomment::get_cmd_nargs() 
{
  return 0;
}


const char * thcomment::get_cmd_end()
{
  return "endcomment";
}


const char * thcomment::get_cmd_name()
{
  // insert command name here
  return "comment";
}


thcmd_option_desc thcomment::get_cmd_option_desc(const char * opts)
{
  return thdataobject::get_cmd_option_desc(opts);
}


void thcomment::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    case 0:
      // ignore everything inside
      break;
    
    default:
      thdataobject::set(cod, args, argenc, indataline);      
  }
}


void thcomment::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
}


int thcomment::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}


