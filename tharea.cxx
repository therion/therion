/*
 * @file tharea.cxx
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
 
#include "tharea.h"
#include "thexception.h"
#include "thobjectname.h"
#include "thchenc.h"
#include "thexpmap.h"
#include "thline.h"

tharea::tharea()
{
  this->type = TT_AREA_TYPE_UNKNOWN;
  this->place = TT_2DOBJ_PLACE_BOTTOM;
  this->first_line = NULL;
  this->last_line = NULL;
}


tharea::~tharea()
{
}


int tharea::get_class_id() 
{
  return TT_AREA_CMD;
}


bool tharea::is(int class_id)
{
  if (class_id == TT_AREA_CMD)
    return true;
  else
    return th2ddataobject::is(class_id);
}

int tharea::get_cmd_nargs() 
{
  return 1;
}


char * tharea::get_cmd_end()
{
  return "endarea";
}


char * tharea::get_cmd_name()
{
  return "area";
}


thcmd_option_desc tharea::get_cmd_option_desc(char * opts)
{
//  int id = thmatch_token(opts, thtt_area_opt);
//  if (id == TT_AREA_UNKNOWN)
    return th2ddataobject::get_cmd_option_desc(opts);
//  else
//    return thcmd_option_desc(id);
}


void tharea::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  
  if (cod.id == 1)
    cod.id = TT_AREA_TYPE;
    
  switch (cod.id) {
  
    case 0:
      thsplit_args(& this->db->db2d.mbf, *args);
      this->insert_border_line(this->db->db2d.mbf.get_size(), 
        this->db->db2d.mbf.get_buffer());
      break;
  
    case TT_AREA_TYPE:
      this->type = thmatch_token(*args, thtt_area_types);
      if (this->type == TT_AREA_TYPE_UNKNOWN)
        ththrow(("unknown area type -- %s", *args))
      break;
    
    default:
      th2ddataobject::set(cod, args, argenc, indataline);
      
  }
}


void tharea::self_delete()
{
  delete this;
}

void tharea::self_print_properties(FILE * outf)
{
  th2ddataobject::self_print_properties(outf);
  fprintf(outf,"tharea:\n");
  fprintf(outf,"\ttype: %d\n",this->type);
  if (this->first_line != NULL) {
    fprintf(outf,"\tborder lines:\n");
    thdb2dab * cbl = this->first_line;
    while (cbl != NULL) {
      fprintf(outf,"\t\t");
      fprintf(outf,cbl->name);
      fprintf(outf,"\n");
      cbl = cbl->next_line;
    }
  }
}


void tharea::insert_border_line(int npars, char ** pars)
{
  thdb2dab * bl;
  if (npars != 1)
    ththrow(("one line name per line allowed"))
  bl = this->db->db2d.insert_border_line();
  bl->source = this->db->csrc;
  thparse_objectname(bl->name,& this->db->buff_stations,*pars);
  if (this->last_line == NULL) {
    this->first_line = bl;
    this->last_line = bl;
    bl->next_line = NULL;
    bl->prev_line = NULL;
  } else {
    this->last_line->next_line = bl;
    bl->prev_line = this->last_line;
    this->last_line = bl;
  }
}


bool tharea::export_mp(class thexpmapmpxs * out)
{
  int macroid = SYMA_WATER;
#define tharea_type_export_mp(type,mid) case type: \
  macroid = mid; \
  break;
  switch (this->type) {
    tharea_type_export_mp(TT_AREA_TYPE_SAND, SYMA_SAND)
    tharea_type_export_mp(TT_AREA_TYPE_DEBRIS, SYMA_DEBRIS)
    tharea_type_export_mp(TT_AREA_TYPE_SUMP, SYMA_SUMP)
    tharea_type_export_mp(TT_AREA_TYPE_WATER, SYMA_WATER)
  }
  
  if (!out->symset->assigned[macroid])
    return(false);
  if (this->first_line == NULL)
    return(false);

  if (out->file == NULL)
    return(true);
    
  fprintf(out->file,"%s(buildcycle(",out->symset->get_mp_macro(macroid));

  this->first_line->line->export_path_mp(out);
  thdb2dab * bl = this->first_line->next_line;
  while (bl != NULL) {
    fprintf(out->file,",\n");
    bl->line->export_path_mp(out);
    bl = bl->next_line;
  }
  
  fprintf(out->file,"));\n");
  
  return(false);  
  
}




