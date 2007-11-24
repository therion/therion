/**
 * @file thjoin.cxx
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
 
#include "thjoin.h"
#include "thexception.h"
#include "thchenc.h"
#include "thparse.h"

thjoin::thjoin()
{
  this->smooth = TT_TRUE;
  this->count = 1;
  
  this->proj = NULL;
  this->proj_next_join = NULL;
  this->proj_prev_join = NULL;
    
  this->first_item = NULL;
  this->last_item = NULL;
}


thjoin::~thjoin()
{
}


int thjoin::get_class_id() 
{
  return TT_JOIN_CMD;
}


bool thjoin::is(int class_id)
{
  if (class_id == TT_JOIN_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thjoin::get_cmd_nargs() 
{
  return 2;
}


const char * thjoin::get_cmd_end()
{
  return NULL;
}


const char * thjoin::get_cmd_name()
{
  return "join";
}


thcmd_option_desc thjoin::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_join_opt);
  if (id == TT_JOIN_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thjoin::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  double dv;
  int sv;
  switch (cod.id) {

    // replace this by real properties setting
    case TT_JOIN_SMOOTH:
      this->smooth = thmatch_token(*args, thtt_bool);
      if (this->smooth == TT_UNKNOWN_BOOL)
        ththrow(("invalid switch -- %s",*args))
      break;
    
    case TT_JOIN_COUNT:
      thparse_double(sv,dv,*args);
      if ((sv != TT_SV_NUMBER) || (dv <= 1.0) || (dv != double(int(dv))))
        ththrow(("invalid join count -- %s",*args))
      this->count = int(dv);
      break;
      
    // if not found, try to set fathers properties  
    default:
      if (cod.id < 1000)
        this->parse_item(*args);
      else
        thdataobject::set(cod, args, argenc, indataline);
  }
}


void thjoin::self_delete()
{
  delete this;
}

void thjoin::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thjoin:\n");
  fprintf(outf,"\tsmooth: %d\n",this->smooth);
  if (this->first_item != NULL) {
    fprintf(outf,"\tjoined items:\n");
    thdb2dji * cbl = this->first_item;
    while (cbl != NULL) {
      fprintf(outf,"\t\t");
      fprintf(outf,cbl->name);
      if (cbl->mark != NULL)
        fprintf(outf,":%s",cbl->mark);
      fprintf(outf,"\n");
      cbl = cbl->next_item;
    }
  }
  // insert intended print of object properties here
}

void thjoin::parse_item(char * istr)
{
  thdb2dji * citem = this->db->db2d.insert_join_item();
  citem->parse_item(istr);
  if (this->last_item == NULL) {
    this->first_item = citem;
    this->last_item = citem;
  } else {
    this->last_item->next_item = citem;
    citem->prev_item = this->last_item;
    this->last_item = citem;
  }
}


int thjoin::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}





