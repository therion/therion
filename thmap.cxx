/**
 * @file thmap.cxx
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
 
#include "thmap.h"
#include "thexception.h"
#include "thdb2dmi.h"
#include "thchenc.h"
#include "thscrap.h"
#include "thobjectname.h"
#include "thdb2dmi.h"

thmap::thmap()
{
  this->first_item = NULL;
  this->last_item = NULL;
  
  this->projection_id = 0;
  this->last_level = 1;
  this->is_basic = true;
  
  this->selection_mode = TT_MAPITEM_UNKNOWN;
  this->selection_xs = NULL;
  
  this->z = 0.0;
  this->nz = -1;
}


thmap::~thmap()
{
}


int thmap::get_class_id() 
{
  return TT_MAP_CMD;
}


bool thmap::is(int class_id)
{
  if (class_id == TT_MAP_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thmap::get_cmd_nargs() 
{
  return 1;
}


char * thmap::get_cmd_end()
{
  return "endmap";
}


char * thmap::get_cmd_name()
{
  return "map";
}


thcmd_option_desc thmap::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_map_opt);
  if (id == TT_MAP_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else {
    switch (id) {
      case TT_MAP_PREVIEW:
        return thcmd_option_desc(id,2);
      case TT_MAP_BREAK:
        return thcmd_option_desc(id,0);
      default:
        return thcmd_option_desc(id);
    }
  }
}


void thmap::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  if (cod.id == 1)
    cod.id = TT_DATAOBJECT_NAME;
    
  switch (cod.id) {
  
    case 0:
      thsplit_args(& this->db->db2d.mbf, *args);
      this->parse_item(this->db->db2d.mbf.get_size(), 
        this->db->db2d.mbf.get_buffer());
      break;
    
    
    case TT_MAP_BREAK:
      this->last_level++;
      break;

    case TT_MAP_PREVIEW:
      this->parse_preview(args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
  
}


void thmap::self_delete()
{
  delete this;
}

void thmap::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thmap:\n");
  fprintf(outf,"\tz: %f\n",this->z);
  fprintf(outf,"\tprojection id: %d\n",this->projection_id);
  if (this->first_item != NULL) {
    fprintf(outf,"\tmap items:\n");
    thdb2dmi * cbl = this->first_item;
    while (cbl != NULL) {
      fprintf(outf,"\t\t");
      fprintf(outf,cbl->name);
      fprintf(outf," type:%d\n",cbl->type);
      cbl = cbl->next_item;
    }
  }
}


void thmap::parse_item(int npar, char ** pars)
{
  if (npar != 1)
    ththrow(("one map or scrap name per line allowed"))
  thdb2dmi * citem = this->db->db2d.insert_map_item();
  citem->itm_level = this->last_level;
  citem->source = this->db->csrc;
  citem->psurvey = this->db->get_current_survey();
  citem->type = TT_MAPITEM_NORMAL;
  thparse_objectname(citem->name,& this->db->buff_stations, *pars);
  if (this->last_item == NULL) {
    this->first_item = citem;
    this->last_item = citem;
  } else {
    this->last_item->next_item = citem;
    citem->prev_item = this->last_item;
    this->last_item = citem;
  }
}


void thmap::parse_preview(char ** pars)
{
  thdb2dmi * citem = this->db->db2d.insert_map_item();
  citem->source = this->db->csrc;
  citem->psurvey = this->db->get_current_survey();
  citem->type = thmatch_token(*pars,thtt_2dmi);
  if (citem->type == TT_MAPITEM_UNKNOWN)
    ththrow(("invalid preview type -- %s", *pars))
  thparse_objectname(citem->name,& this->db->buff_stations, pars[1]);
  if (this->last_item == NULL) {
    this->first_item = citem;
    this->last_item = citem;
  } else {
    this->last_item->next_item = citem;
    citem->prev_item = this->last_item;
    this->last_item = citem;
  }
}


void thmap::calc_z() {
  thdb2dmi * mi;
  if (this->nz > -1)
    return;
  this->z = 0.0;
  this->nz = 0;
  mi = this->first_item;
  while (mi != NULL) {
    if (mi->type == TT_MAPITEM_NORMAL) {
      switch (mi->object->get_class_id()) {
        case TT_MAP_CMD:
          ((thmap*)mi->object)->calc_z();
          this->z += ((thmap*)mi->object)->z;
          this->nz++;
          break;
        case TT_SCRAP_CMD:
          this->z += ((thscrap*)mi->object)->z;
          this->nz++;
          break;
      }
    }
    mi = mi->next_item;
  }
  if (this->nz > 0) {
    this->z /= double(this->nz);
  }
}


int thmap::get_context()
{
  return (THCTX_SURVEY);
}
