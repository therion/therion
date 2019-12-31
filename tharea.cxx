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
  this->place = TT_2DOBJ_PLACE_DEFAULT_BOTTOM;
  this->first_line = NULL;
  this->last_line = NULL;
  this->m_outline_line = NULL;
}


tharea::~tharea()
{
  delete this->m_outline_line;
}


void tharea::start_insert() {
  if (this->type == TT_AREA_TYPE_U) {
    if (this->m_subtype_str == NULL)
      ththrow(("missing subtype specification for area of user defined type"))
    this->db->db2d.register_u_symbol(this->get_class_id(), this->m_subtype_str);
  }
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


const char * tharea::get_cmd_end()
{
  return "endarea";
}


const char * tharea::get_cmd_name()
{
  return "area";
}


thcmd_option_desc tharea::get_cmd_option_desc(const char * opts)
{
//  int id = thmatch_token(opts, thtt_area_opt);
//  if (id == TT_AREA_UNKNOWN)
    return th2ddataobject::get_cmd_option_desc(opts);
//  else
//    return thcmd_option_desc(id);
}


void tharea::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  char * type, * subtype;
  if (cod.id == 1)
    cod.id = TT_AREA_TYPE;

  switch (cod.id) {

    case 0:
      thsplit_args(& this->db->db2d.mbf, *args);
      this->insert_border_line(this->db->db2d.mbf.get_size(),
        this->db->db2d.mbf.get_buffer());
      break;

    case TT_AREA_TYPE:
      th2dsplitTT(*args, &type, &subtype);
      this->parse_type(type);
      if (strlen(subtype) > 0)
        this->parse_subtype(subtype);
      break;

    default:
      th2ddataobject::set(cod, args, argenc, indataline);

  }
}


void tharea::parse_type(char * tstr)
{
  this->type = thmatch_token(tstr, thtt_area_types);
  if (this->type == TT_AREA_TYPE_UNKNOWN)
    ththrow(("unknown area type -- %s", tstr))
  if (this->type == TT_AREA_TYPE_DIMENSIONS)
    ththrow(("area dimensions is not supported as ordinary type"))
}



void tharea::parse_subtype(char * ststr)
{
  if (this->type == TT_AREA_TYPE_UNKNOWN)
    ththrow(("area type must be specified before subtype"))
  if (this->type == TT_AREA_TYPE_U) {
    this->parse_u_subtype(ststr);
    return;
  } else
    ththrow(("invalid type - subtype combination"))
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
  int macroid = SYMA_WATER, omacroid;
#define tharea_type_export_mp(type,mid) case type: \
  macroid = mid; \
  break;
  switch (this->type) {
    tharea_type_export_mp(TT_AREA_TYPE_SAND, SYMA_SAND)
    tharea_type_export_mp(TT_AREA_TYPE_DEBRIS, SYMA_DEBRIS)
    tharea_type_export_mp(TT_AREA_TYPE_SUMP, SYMA_SUMP)
    tharea_type_export_mp(TT_AREA_TYPE_WATER, SYMA_WATER)
    tharea_type_export_mp(TT_AREA_TYPE_BLOCKS, SYMA_BLOCKS)
    tharea_type_export_mp(TT_AREA_TYPE_SNOW, SYMA_SNOW)
    tharea_type_export_mp(TT_AREA_TYPE_ICE, SYMA_ICE)
    tharea_type_export_mp(TT_AREA_TYPE_CLAY, SYMA_CLAY)
    tharea_type_export_mp(TT_AREA_TYPE_PEBBLES, SYMA_PEBBLES)
    tharea_type_export_mp(TT_AREA_TYPE_BEDROCK, SYMA_BEDROCK)
    tharea_type_export_mp(TT_AREA_TYPE_FLOWSTONE, SYMA_FLOWSTONE)
    tharea_type_export_mp(TT_AREA_TYPE_MOONMILK, SYMA_MOONMILK)
    tharea_type_export_mp(TT_AREA_TYPE_U, SYMA_U)
  }
  omacroid = macroid;
  if (this->context >= 0)
    macroid = this->context;
  if (!out->symset->is_assigned(macroid))
    return(false);
  if (this->first_line == NULL)
    return(false);

  if (out->file == NULL)
    return(true);

  th2ddataobject::export_mp(out);
  thdb_revision_set_type::iterator ri =
      this->db->revision_set.find(threvision(this->id, 0));
  fprintf(out->file,"current_src := \"%s [%ld]\";\n", ri->srcf.name, ri->srcf.line);
  fprintf(out->file,"string area_border[];\n");
  thdb2dab * bl = this->first_line;
  int blnum = 1;
  while (bl != NULL) {
    fprintf(out->file,"area_border[%d] := \"%s\";\n", blnum, bl->line->name);
    bl = bl->next_line;
    blnum++;
  }

  if (this->type == TT_AREA_TYPE_U) {
    out->symset->export_mp_symbol_options(out->file, -1);
    fprintf(out->file,"a_u_%s(buildcycle(",this->m_subtype_str);
    this->db->db2d.use_u_symbol(this->get_class_id(), this->m_subtype_str);
  } else {
    out->symset->export_mp_symbol_options(out->file, omacroid);
    fprintf(out->file,"%s(buildcycle(",out->symset->get_mp_macro(omacroid));
  }

  this->first_line->line->export_path_mp(out);
  bl = this->first_line->next_line;
  while (bl != NULL) {
    fprintf(out->file,",\n");
    bl->line->export_path_mp(out);
    bl = bl->next_line;
  }

  fprintf(out->file,"));\n");

  th2ddataobject::export_mp_end(out);
  return(false);

}




