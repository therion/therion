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
#include "thtflength.h"

thmap::thmap()
{
  this->first_item = NULL;
  this->last_item = NULL;
  
  this->expl_projection = NULL;
  this->projection_id = 0;
  this->last_level = 1;
  this->is_basic = true;
  
  this->selection_mode = TT_MAPITEM_UNKNOWN;
  this->selection_xs = NULL;
  
  this->z = thnan;
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


const char * thmap::get_cmd_end()
{
  return "endmap";
}


const char * thmap::get_cmd_name()
{
  return "map";
}


thcmd_option_desc thmap::get_cmd_option_desc(const char * opts)
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
  thdb2dprjpr projection;

  if (cod.id == 1)
    cod.id = TT_DATAOBJECT_NAME;
    
  switch (cod.id) {
  
    case 0:
      thsplit_args(& this->db->db2d.mbf, *args);
      this->parse_item(this->db->db2d.mbf.get_size(), 
        this->db->db2d.mbf.get_buffer());
      break;
    
    case TT_MAP_PROJECTION:
      projection = this->db->db2d.parse_projection(*args);
      if (!projection.parok)
        ththrow(("invalid parameters of projection"));
      this->expl_projection = projection.prj;
      break;
    
    case TT_MAP_BREAK:
      this->last_level++;
      break;

	case TT_MAP_SURVEY:
	  thparse_objectname(this->asoc_survey, &(this->db->buff_stations), args[0], this);
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

thmbuffer mapitmmb;

void thmap::parse_item(int npar, char ** pars)
{
  if ((npar != 1) && (npar != 3))
    ththrow(("invalid map item"))
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
  if (npar == 3) {
    thsplit_words(&mapitmmb, pars[1]);
    char ** ss;
    size_t nw;
    int sv;
    nw = mapitmmb.get_size();
    ss = mapitmmb.get_buffer();
    if ((nw < 2) || (nw > 3))
      ththrow(("invalid map shift specification -- %s", pars[1]))

    thtflength l;
    thparse_double(sv, citem->m_shift.m_x, ss[0]);
    if (sv != TT_SV_NUMBER)
      ththrow(("not a number -- %s", ss[0]));
    thparse_double(sv, citem->m_shift.m_y, ss[1]);
    if (sv != TT_SV_NUMBER)
      ththrow(("not a number -- %s", ss[1]));
    if (nw > 2)
      l.parse_units(ss[2]);
    citem->m_shift.m_x = l.transform(citem->m_shift.m_x);
    citem->m_shift.m_y = l.transform(citem->m_shift.m_y);
    citem->m_shift.m_preview = thmatch_token(pars[2],thtt_2dmi);
    if ((citem->m_shift.m_preview == TT_MAPITEM_UNKNOWN) || (citem->m_shift.m_preview == TT_MAPITEM_NORMAL))
      ththrow(("unsupported preview type -- %s", pars[2]))
  }
}


void thmap::parse_preview(char ** pars)
{
  thdb2dmi * citem = this->db->db2d.insert_map_item();
  citem->source = this->db->csrc;
  citem->psurvey = this->db->get_current_survey();
  citem->type = thmatch_token(*pars,thtt_2dmi);
  switch (citem->type) {
    case TT_MAPITEM_ABOVE:
    case TT_MAPITEM_BELOW:
      break;
    default:
      ththrow(("unsupported preview type -- %s", *pars))
  }
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
          if (((thmap*)mi->object)->nz > 0) {
            this->z += ((thmap*)mi->object)->z;
            this->nz++;
          }
          break;
        case TT_SCRAP_CMD:
          if (!thisnan(((thscrap*)mi->object)->z)) {
            this->z += ((thscrap*)mi->object)->z;
            this->nz++;
          }
          break;
      }
    }
    mi = mi->next_item;
  }
  if (this->nz > 0) {
    this->z /= double(this->nz);
  } else {
    this->z = thnan;
  }
}


int thmap::get_context()
{
  return (THCTX_SURVEY);
}
