/**
 * @file thpoint.cxx
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

#include "thpoint.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdatabase.h"
#include "thinfnan.h"
#include "thexpmap.h"
#include "thtflength.h"
#include "thtexfonts.h"
#include "thdate.h"
#include "thscrap.h"
#include "thobjectname.h"
#include "therion.h"
#include <string>
#include <cstdio>


thpoint::thpoint()
{
  // replace this by setting real properties initialization
  this->type = TT_POINT_TYPE_UNKNOWN;
  this->subtype = TT_POINT_SUBTYPE_UNKNOWN;
  this->point = thdb.db2d.insert_point();
  this->cpoint = NULL;
  this->point->pscrap = thdb.get_current_scrap();

  this->station_name.clear();
  this->from_name.clear();

  this->orient = thnan;
  this->xsize = thnan;
  this->ysize = thnan;
  this->align = TT_POINT_ALIGN_C;

  this->text = NULL;
}


thpoint::~thpoint()
{
  if (this->type == TT_POINT_TYPE_DATE) {
    thdate * dp = (thdate *) this->text;
    delete dp;
    this->text = NULL;
  }
}


int thpoint::get_class_id()
{
  return TT_POINT_CMD;
}


bool thpoint::is(int class_id)
{
  if (class_id == TT_POINT_CMD)
    return true;
  else
    return th2ddataobject::is(class_id);
}

int thpoint::get_cmd_nargs()
{
  // replace by real number of arguments
  return 3;
}


const char * thpoint::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


const char * thpoint::get_cmd_name()
{
  // insert command name here
  return "point";
}


thcmd_option_desc thpoint::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_point_opt);
  if (id == TT_POINT_UNKNOWN)
    return th2ddataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thpoint::start_insert() {
  if (this->type == TT_POINT_TYPE_U) {
    if (this->m_subtype_str == NULL)
      ththrow("missing subtype specification for point of user defined type");
    this->db->db2d.register_u_symbol(this->get_class_id(), this->m_subtype_str);
  }
}


void thpoint::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  double dv;
  int sv;
  char * type, * subtype;

  if (cod.id == 3)
    cod.id = TT_POINT_TYPE;

  switch (cod.id) {

    case 1:
    case 2:
      thparse_double(sv,dv,*args);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid number -- {}",*args);
      if (cod.id == 1)
        this->point->x = dv;
      else
        this->point->y = dv;
      break;

    case TT_POINT_TYPE:
      th2dsplitTT(*args, &type, &subtype);
      this->parse_type(type);
      if (strlen(subtype) > 0)
        this->parse_subtype(subtype);
      break;

    case TT_POINT_DIST:
	  this->parse_value(*args, true);
	  break;

    case TT_POINT_VALUE:
      this->parse_value(*args);
      break;

    case TT_POINT_TEXT:
      thencode(&(this->db->buff_enc), *args, argenc);
      this->parse_text(this->db->buff_enc.get_buffer());
      break;

    case TT_POINT_EXPLORED:
      this->parse_explored(*args);
      break;

    case TT_POINT_ALIGN:
      this->parse_align(*args);
      break;

    case TT_POINT_SCRAP:
      if (this->type != TT_POINT_TYPE_SECTION)
        ththrow("point not section -- -scrap");
      thparse_objectname(this->station_name, & this->db->buff_stations, *args);
      break;

    case TT_POINT_XSIZE:
      ththrow("-x-size not valid with type {}", thmatch_string(this->type,thtt_point_types));
      thparse_double(sv,this->xsize,*args);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid number -- {}",*args);
      if (this->xsize <= 0.0)
        ththrow("size not positive -- {}",*args);
      break;

    case TT_POINT_SIZE:
      ththrow("-size not valid with type {}", thmatch_string(this->type,thtt_point_types));
      thparse_double(sv,this->xsize,*args);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid number -- {}",*args);
      if (this->xsize <= 0.0)
        ththrow("size not positive -- {}",*args);
      this->ysize = this->xsize;
      break;

    case TT_POINT_YSIZE:
      ththrow("-y-size not valid with type {}", thmatch_string(this->type,thtt_point_types));
      thparse_double(sv,this->ysize,*args);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid number -- {}",*args);
      if (this->ysize <= 0.0)
        ththrow("size not positive -- {}",*args);
      break;

    case TT_POINT_ORIENT:
      switch (this->type) {
        case TT_POINT_TYPE_STATION:
          ththrow("-orientation not valid with type {}", thmatch_string(this->type,thtt_point_types));
      }
      thparse_double(sv,this->orient,*args);
      if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
        ththrow("invalid number -- {}",*args);
      if ((this->orient < 0.0) || (this->orient >= 360.0))
        ththrow("orientation out of range -- {}",*args);
      break;

    case TT_POINT_SUBTYPE:
      this->parse_subtype(*args);
      break;

    case TT_POINT_STATION:
      switch (this->type) {
        case TT_POINT_TYPE_STATION:
        case TT_POINT_TYPE_CONTINUATION:
          thparse_objectname(this->station_name, & this->db->buff_stations, *args, thdb.cscrapptr);
          break;
        default:
          ththrow("station reference not allowed with this point type");
      }
      break;

    case TT_POINT_FROM:
      this->parse_from(*args);
      break;

    // if not found, try to set fathers properties
    default:
      if (cod.id == TT_2DOBJ_CLIP) {
        switch (this->type) {
          case TT_POINT_TYPE_STATION_NAME:
          case TT_POINT_TYPE_LABEL:
          case TT_POINT_TYPE_REMARK:
          case TT_POINT_TYPE_DATE:
          case TT_POINT_TYPE_ALTITUDE:
          case TT_POINT_TYPE_HEIGHT:
          case TT_POINT_TYPE_DIMENSIONS:
          case TT_POINT_TYPE_MAP_CONNECTION:
          case TT_POINT_TYPE_PASSAGE_HEIGHT:
            ththrow("-clip not valid with type {}", thmatch_string(this->type,thtt_point_types));
            break;
        }
      }
      th2ddataobject::set(cod, args, argenc, indataline);
  }
}


void thpoint::self_print_properties(FILE * outf)
{
  th2ddataobject::self_print_properties(outf);
  fprintf(outf,"thpoint:\n");
  fprintf(outf,"\ttype: %d\n",this->type);
  fprintf(outf,"\tsubtype: %d\n",this->subtype);
  fprintf(outf,"\talign: %d\n",this->align);
  fprintf(outf,"\tpoint: %f,%f (%f,%f,%f,%f)\n",this->point->x, this->point->y,
    this->point->xt,this->point->yt,this->point->zt,this->point->at);
  fprintf(outf,"\torientation: %f\n", this->orient);
  fprintf(outf,"\tx-size: %f\n", this->xsize);
  fprintf(outf,"\ty-size: %f\n", this->ysize);
  if (this->type == TT_POINT_TYPE_STATION) {
    fprintf(outf,"\tstation: ");
    fprintf(outf,this->station_name);
    fprintf(outf,"\n\tfrom station: ");
    fprintf(outf,this->from_name);
  }
  // insert intended print of object properties here
}


void thpoint::parse_type(char * tstr)
{
  this->type = thmatch_token(tstr, thtt_point_types);
  if (this->type == TT_POINT_TYPE_UNKNOWN)
    ththrow("unknown point type -- {}", tstr);
  this->xsize = thnan;
  this->ysize = thnan;
  switch (this->type) {
    case TT_POINT_TYPE_DATE:
      if (this->type == TT_POINT_TYPE_DATE)
        this->text = (char *) new thdate;
      break;
    case TT_POINT_TYPE_ALTITUDE:
      this->xsize = 0.0;
      this->ysize = 1.0;
      break;
  }
}


void thpoint::parse_subtype(char * ststr)
{
  if (this->type == TT_POINT_TYPE_UNKNOWN)
    ththrow("point type must be specified before subtype");
  if (this->type == TT_POINT_TYPE_U) {
    this->parse_u_subtype(ststr);
    return;
  }
  this->subtype = thmatch_token(ststr, thtt_point_subtypes);
  if (this->subtype == TT_POINT_SUBTYPE_UNKNOWN)
    ththrow("unknown point subtype -- {}", ststr);
  // let's check type - subtype
  bool combok = false;
  switch (this->type) {
    case TT_POINT_TYPE_STATION:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_TEMP:
        case TT_POINT_SUBTYPE_FIXED:
        case TT_POINT_SUBTYPE_PAINTED:
        case TT_POINT_SUBTYPE_NATURAL:
          combok = true;
          break;
      }
      break;
    case TT_POINT_TYPE_AIR_DRAUGHT:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_UNDEF:
        case TT_POINT_SUBTYPE_WINTER:
        case TT_POINT_SUBTYPE_SUMMER:
          combok = true;
          break;
      }
      break;
    case TT_POINT_TYPE_WATER_FLOW:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_PERMANENT:
        case TT_POINT_SUBTYPE_INTERMITTENT:
        case TT_POINT_SUBTYPE_PALEO:
          combok = true;
          break;
      }
      break;
  }
  if (!combok)
    ththrow("invalid point type - subtype combination");
}

void thpoint::parse_from(char * estr)
{
  thsplit_words(& this->db->db2d.mbf, estr);
  int npar = this->db->db2d.mbf.get_size();
  char ** pars = this->db->db2d.mbf.get_buffer();
  if (npar != 1)
    ththrow("invalid from station reference -- {}", estr);
  thparse_objectname(this->from_name,& this->db->buff_stations,pars[0],this->fscrapptr);
}


const char * thpoint_export_mp_align2mp(int a) {
  switch (a) {
    case TT_POINT_ALIGN_R:
      return ".rt";
    case TT_POINT_ALIGN_L:
      return ".lft";
    case TT_POINT_ALIGN_T:
      return ".top";
    case TT_POINT_ALIGN_B:
      return ".bot";
    case TT_POINT_ALIGN_BR:
      return ".lrt";
    case TT_POINT_ALIGN_BL:
      return ".llft";
    case TT_POINT_ALIGN_TR:
      return ".urt";
    case TT_POINT_ALIGN_TL:
      return ".ulft";
    default:
      return "";
  }
}


bool thpoint::export_mp(class thexpmapmpxs * out)
{
  th2ddataobject::export_mp(out);
  thattr * station_attr = NULL;
  long station_attr_id = 0;

  bool postprocess = true, expsym = false;
  std::string attr_text;
  int macroid = -1, omacroid = -1, cmark;
  const char * postprocess_label = NULL;
  this->db->buff_enc.guarantee(8128);
//  char * buff = this->db->buff_enc.get_buffer();
  double xrr = (thisnan(this->orient) ? out->rr : 0.0);

  if (this->scale_numeric < out->layout->min_symbol_scale) return(false);

  switch(this->type) {

    case TT_POINT_TYPE_DIMENSIONS:
    case TT_POINT_TYPE_EXTRA:
    case TT_POINT_TYPE_MAP_CONNECTION:
      postprocess = false;
      break;
    case TT_POINT_TYPE_LABEL:
    case TT_POINT_TYPE_REMARK:
    case TT_POINT_TYPE_STATION_NAME:
      if (this->text != NULL) {
        switch (this->type) {
          case TT_POINT_TYPE_LABEL:
            macroid = SYMP_LABEL;
            break;
          case TT_POINT_TYPE_REMARK:
            macroid = SYMP_REMARK;
            break;
          default:
            macroid = SYMP_STATIONNAME;
            break;
        }
        omacroid = macroid;
        if (this->context >= 0)
          macroid = this->context;
        if (!out->symset->is_assigned(macroid))
          return(false);
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(omacroid);
        out->symset->export_mp_symbol_options(out->file, omacroid);
        fprintf(out->file,"p_label%s(btex ",thpoint_export_mp_align2mp(thdb2d_rotate_align(this->align, xrr)));
        switch (this->type) {
          case TT_POINT_TYPE_STATION_NAME:
            fprintf(out->file,"\\thstationname ");
            break;
          case TT_POINT_TYPE_LABEL:
            fprintf(out->file,"\\thlabel");
            break;
          case TT_POINT_TYPE_REMARK:
            fprintf(out->file,"\\thremark ");
            break;
        }
        out->layout->export_mptex_font_size(out->file, this, true);
        fprintf(out->file,"%s etex,",
          ((this->type == TT_POINT_TYPE_STATION_NAME) && (!this->station_name.is_empty()))
          ? utf2tex(thobjectname_print_full_name(this->station_name.name, this->station_name.psurvey, out->layout->survey_level)).c_str()
          : ths2tex(this->text, out->layout->lang).c_str());
        if (this->type == TT_POINT_TYPE_STATION_NAME)
          postprocess_label = "p_label_mode_station";
        else
          postprocess_label = "p_label_mode_label";
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_STATION:
      switch (this->subtype) {
        case TT_POINT_SUBTYPE_FIXED:
          macroid = SYMP_STATION_FIXED;
          cmark = TT_DATAMARK_FIXED;
          break;
        case TT_POINT_SUBTYPE_NATURAL:
          macroid = SYMP_STATION_NATURAL;
          cmark = TT_DATAMARK_NATURAL;
          break;
        case TT_POINT_SUBTYPE_PAINTED:
          macroid = SYMP_STATION_PAINTED;
          cmark = TT_DATAMARK_PAINTED;
          break;
        default:
          macroid = SYMP_STATION_TEMPORARY;
          cmark = TT_DATAMARK_TEMP;
      }

      omacroid = macroid;
      if (this->context >= 0)
        macroid = this->context;
      expsym = out->symset->is_assigned(macroid);
      if (expsym) out->symset->get_mp_macro(omacroid);

      {
        thdb1ds * pst = NULL;
        std::string commentstr("0");
        if (this->station_name.id != 0) {
          pst = &(this->db->db1d.station_vec[this->station_name.id - 1]);
          station_attr_id = this->station_name.id;
          station_attr = &(thdb.db1d.m_station_attr);
          station_attr->export_mp_object_begin(out->file, station_attr_id);
        }
        if (pst != NULL) {
#define thexpmatselected_stationflag(flag,mid) if (((pst->flags & flag) == flag) && out->symset->is_assigned(mid)) expsym = true;
          thexpmatselected_stationflag(TT_STATIONFLAG_ENTRANCE, SYMP_FLAG_ENTRANCE)
          thexpmatselected_stationflag(TT_STATIONFLAG_SINK, SYMP_FLAG_SINK)
          thexpmatselected_stationflag(TT_STATIONFLAG_SPRING, SYMP_FLAG_SPRING)
          thexpmatselected_stationflag(TT_STATIONFLAG_DOLINE, SYMP_FLAG_DOLINE)
          thexpmatselected_stationflag(TT_STATIONFLAG_DIG, SYMP_FLAG_DIG)
          thexpmatselected_stationflag(TT_STATIONFLAG_ARCH, SYMP_FLAG_ARCH)
          thexpmatselected_stationflag(TT_STATIONFLAG_OVERHANG, SYMP_FLAG_OVERHANG)
          thexpmatselected_stationflag(TT_STATIONFLAG_CONT, SYMP_FLAG_CONTINUATION)
          thexpmatselected_stationflag(TT_STATIONFLAG_AIRDRAUGHT, SYMP_FLAG_AIRDRAUGHT)
          thexpmatselected_stationflag(TT_STATIONFLAG_AIRDRAUGHT_SUMMER, SYMP_FLAG_AIRDRAUGHT)
          thexpmatselected_stationflag(TT_STATIONFLAG_AIRDRAUGHT_WINTER, SYMP_FLAG_AIRDRAUGHT)
        }

        if (out->file == NULL) return(expsym);

        if ((pst != NULL) && (pst->comment != NULL) && (strlen(pst->comment) > 0)) {
          commentstr = "btex \\thcomment ";
          commentstr += ths2tex(pst->comment, out->layout->lang);
          commentstr += " etex";
        }
        if (out->symset->is_assigned(macroid))
          out->symset->export_mp_symbol_options(out->file, omacroid);
        else
          out->symset->export_mp_symbol_options(out->file, -1);
        fprintf(out->file,"p_station(");
        this->point->export_mp(out);
        fprintf(out->file,",%d,%s,\"\"",
          out->symset->is_assigned(macroid) ? cmark : 0,
          commentstr.c_str()
          );
#define thexpmat_stationflag(flag,mid,str) if (((pst->flags & flag) == flag) && out->symset->is_assigned(mid)) fprintf(out->file,",\"%s\"", str);
        if (pst != NULL) {
          thexpmat_stationflag(TT_STATIONFLAG_ENTRANCE, SYMP_FLAG_ENTRANCE, "entrance")
          thexpmat_stationflag(TT_STATIONFLAG_SINK, SYMP_FLAG_SINK, "sink")
          thexpmat_stationflag(TT_STATIONFLAG_SPRING, SYMP_FLAG_SPRING, "spring")
          thexpmat_stationflag(TT_STATIONFLAG_DOLINE, SYMP_FLAG_DOLINE, "doline")
          thexpmat_stationflag(TT_STATIONFLAG_DIG, SYMP_FLAG_DIG, "dig")
          thexpmat_stationflag(TT_STATIONFLAG_ARCH, SYMP_FLAG_ARCH, "arch")
          thexpmat_stationflag(TT_STATIONFLAG_OVERHANG, SYMP_FLAG_OVERHANG, "overhang")
          thexpmat_stationflag(TT_STATIONFLAG_CONT, SYMP_FLAG_CONTINUATION, "continuation")
          thexpmat_stationflag(TT_STATIONFLAG_AIRDRAUGHT, SYMP_FLAG_AIRDRAUGHT, "air-draught")
          thexpmat_stationflag(TT_STATIONFLAG_AIRDRAUGHT_SUMMER, SYMP_FLAG_AIRDRAUGHT, "air-draught:summer")
          thexpmat_stationflag(TT_STATIONFLAG_AIRDRAUGHT_WINTER, SYMP_FLAG_AIRDRAUGHT, "air-draught:winter")
        }
        fprintf(out->file,");\n");
      }

      if (out->symset->is_assigned(macroid) && out->layout->is_debug_stations()) {
        fprintf(out->file,"p_debug(0,1,");
        this->point->export_mp(out,0);
        fprintf(out->file,");\n");
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_ALTITUDE:
      macroid = SYMP_ALTITUDE;
      if (this->context >= 0)
        macroid = this->context;
      if ((!thisnan(this->xsize)) && (out->symset->is_assigned(macroid))) {
        //sprintf(buff,"%.0f",this->xsize - out->layout->goz);
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(SYMP_ALTITUDE);
        out->symset->export_mp_symbol_options(out->file, SYMP_ALTITUDE);
        fprintf(out->file,"p_label%s(btex \\thaltitude ",
					thpoint_export_mp_align2mp(thdb2d_rotate_align(this->align, xrr)));
        out->layout->export_mptex_font_size(out->file, this, false);
        fprintf(out->file,"%s etex,",
					utf2tex(out->layout->units.format_length(this->xsize - out->layout->goz)).c_str());
        postprocess_label = "p_label_mode_altitude";
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_HEIGHT:
      if ((this->tags & (TT_POINT_TAG_HEIGHT_P |
          TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) != 0) {

        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case TT_POINT_TAG_HEIGHT_P:
            macroid = SYMP_HEIGHT_POSITIVE;
            break;
          case TT_POINT_TAG_HEIGHT_N:
            macroid = SYMP_HEIGHT_NEGATIVE;
            break;
          case TT_POINT_TAG_HEIGHT_U:
            macroid = SYMP_HEIGHT_UNSIGNED;
            break;
          default:
            return(false);
        }

        omacroid = macroid;
        if (this->context >= 0)
          macroid = this->context;
        if (!out->symset->is_assigned(macroid))
          return(false);
        if (out->file == NULL)
          return(true);

        out->symset->get_mp_macro(omacroid);
        out->symset->export_mp_symbol_options(out->file, omacroid);
        fprintf(out->file,"p_label%s(btex \\thheight ",thpoint_export_mp_align2mp(thdb2d_rotate_align(this->align, xrr)));
        out->layout->export_mptex_font_size(out->file, this, false);


        if ((this->tags & TT_POINT_TAG_HEIGHT_P) != 0)
          fprintf(out->file,"\\thheightpos ");
        else if ((this->tags & TT_POINT_TAG_HEIGHT_N) != 0)
          fprintf(out->file,"\\thheightneg ");

        if (!thisnan(this->xsize)) {
          //if (double(int(this->xsize)) != this->xsize)
          //  sprintf(buff,"%.1f",this->xsize);
          //else
          //  sprintf(buff,"%.0f",this->xsize);
          fprintf(out->file,"%s",utf2tex(out->layout->units.format_human_length(this->xsize)).c_str());
        }
        this->db->buff_enc.strcpy((this->tags & (TT_POINT_TAG_HEIGHT_PQ |
            TT_POINT_TAG_HEIGHT_NQ | TT_POINT_TAG_HEIGHT_UQ)) != 0 ? "?" : "" );
        fprintf(out->file,"%s etex,",utf2tex(this->db->buff_enc.get_buffer()).c_str());
        postprocess_label = "p_label_mode_height";
      }
      postprocess = false;
      break;


    case TT_POINT_TYPE_DATE:
      macroid = SYMP_DATE;
      if (this->context >= 0)
        macroid = this->context;
      if  ((out->symset->is_assigned(macroid)) &&
          ((this->tags & TT_POINT_TAG_DATE) > 0)) {
//        ((thdate *)this->text)->print_export_str();
//        fprintf(out->file,"Datelabel%s(\"%s\",",
//            thpoint_export_mp_align2mp(this->align),
//            ((thdate *)this->text)->get_str());
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(SYMP_DATE);
        out->symset->export_mp_symbol_options(out->file, SYMP_DATE);
        fprintf(out->file,"p_label%s(btex \\thdate ",
            thpoint_export_mp_align2mp(thdb2d_rotate_align(this->align, xrr)));
        out->layout->export_mptex_font_size(out->file, this, false);

        fprintf(out->file,"%s etex,",
            utf2tex(((thdate *)this->text)->get_str(TT_DATE_FMT_LOCALE)).c_str());
        postprocess_label = "p_label_mode_date";
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_PASSAGE_HEIGHT:
      if ((this->tags & (TT_POINT_TAG_HEIGHT_P |
          TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) != 0) {

        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case (TT_POINT_TAG_HEIGHT_P | TT_POINT_TAG_HEIGHT_N):
            macroid = SYMP_PASSAGEHEIGHT_BOTH;
            break;
          case TT_POINT_TAG_HEIGHT_P:
            macroid = SYMP_PASSAGEHEIGHT_POSITIVE;
            break;
          case TT_POINT_TAG_HEIGHT_N:
            macroid = SYMP_PASSAGEHEIGHT_NEGATIVE;
            break;
          case TT_POINT_TAG_HEIGHT_U:
            macroid = SYMP_PASSAGEHEIGHT_UNSIGNED;
            break;
          default:
            return(false);
        }

        omacroid = macroid;
        if (this->context >= 0)
          macroid = this->context;
        if (!out->symset->is_assigned(macroid))
          return(false);
        if (out->file == NULL)
          return(true);
        out->symset->get_mp_macro(omacroid);
        out->symset->export_mp_symbol_options(out->file, omacroid);
        fprintf(out->file,"p_label%s(btex ",thpoint_export_mp_align2mp(thdb2d_rotate_align(this->align, xrr)));
        switch (this->tags & (TT_POINT_TAG_HEIGHT_P |
        TT_POINT_TAG_HEIGHT_N | TT_POINT_TAG_HEIGHT_U)) {
          case (TT_POINT_TAG_HEIGHT_P | TT_POINT_TAG_HEIGHT_N):
            fprintf(out->file,"\\thframed \\updown");
            postprocess_label = "p_label_mode_passageheightposneg";
            break;
          case TT_POINT_TAG_HEIGHT_P:
            fprintf(out->file,"\\thframed ");
            postprocess_label = "p_label_mode_passageheightpos";
            break;
          case TT_POINT_TAG_HEIGHT_N:
            fprintf(out->file,"\\thframed ");
            postprocess_label = "p_label_mode_passageheightneg";
            break;
          default:
            fprintf(out->file,"\\thframed ");
            postprocess_label = "p_label_mode_passageheight";
            break;
        }

        if (!thisnan(this->xsize)) {
          fprintf(out->file,"{");
          out->layout->export_mptex_font_size(out->file, this, false);

          fprintf(out->file,"%s}", utf2tex(out->layout->units.format_human_length(this->xsize)).c_str());
        }

        if (!thisnan(this->ysize)) {
          fprintf(out->file,"{");
          out->layout->export_mptex_font_size(out->file, this, false);

          fprintf(out->file,"%s}", utf2tex(out->layout->units.format_human_length(this->ysize)).c_str());
        }

        fprintf(out->file," etex,");
      }
      postprocess = false;
      break;

    case TT_POINT_TYPE_SECTION:
      postprocess = false;
      break;

#define thpoint_type_export_mp(type,mid) case type: \
  macroid = mid; \
  break;

// ostatne typy
    case TT_POINT_TYPE_WATER_FLOW:
      switch (this->subtype) {
        thpoint_type_export_mp(TT_POINT_SUBTYPE_PALEO,SYMP_WATERFLOW_PALEO);
        thpoint_type_export_mp(TT_POINT_SUBTYPE_INTERMITTENT,SYMP_WATERFLOW_INTERMITTENT);
        default:
          macroid = SYMP_WATERFLOW_PERMANENT;
      }
      break;

    case TT_POINT_TYPE_AIR_DRAUGHT:
      switch (this->subtype) {
        thpoint_type_export_mp(TT_POINT_SUBTYPE_WINTER,SYMP_AIRDRAUGHT_WINTER);
        thpoint_type_export_mp(TT_POINT_SUBTYPE_SUMMER,SYMP_AIRDRAUGHT_SUMMER);
        default:
          macroid = SYMP_AIRDRAUGHT;
      }
      break;

    case TT_POINT_TYPE_CONTINUATION:
      macroid = SYMP_CONTINUATION;
      {
        std::string tmp;
        if ((this->text != NULL) && (strlen(this->text) > 0)) {
          if (tmp.length() > 0)
            tmp += " -- ";
          tmp += this->text;
        }
        if (tmp.length() > 0) {
          attr_text = "btex \\thcomment ";
          attr_text += ths2tex(tmp, out->layout->lang);
          attr_text += "etex";
        }
      }
      break;

// specialne symboly
    thpoint_type_export_mp(TT_POINT_TYPE_SPRING,SYMP_SPRING)
    thpoint_type_export_mp(TT_POINT_TYPE_SINK,SYMP_SINK)
    thpoint_type_export_mp(TT_POINT_TYPE_ENTRANCE,SYMP_ENTRANCE)
    thpoint_type_export_mp(TT_POINT_TYPE_GRADIENT,SYMP_GRADIENT)

// vystroj
    thpoint_type_export_mp(TT_POINT_TYPE_ROPE,SYMP_ROPE)
    thpoint_type_export_mp(TT_POINT_TYPE_FIXED_LADDER,SYMP_FIXEDLADDER)
    thpoint_type_export_mp(TT_POINT_TYPE_ROPE_LADDER,SYMP_ROPELADDER)
    thpoint_type_export_mp(TT_POINT_TYPE_STEPS,SYMP_STEPS)
    thpoint_type_export_mp(TT_POINT_TYPE_BRIDGE,SYMP_BRIDGE)
    thpoint_type_export_mp(TT_POINT_TYPE_TRAVERSE,SYMP_TRAVERSE)
    thpoint_type_export_mp(TT_POINT_TYPE_NO_EQUIPMENT,SYMP_NOEQUIPMENT)
    thpoint_type_export_mp(TT_POINT_TYPE_ANCHOR,SYMP_ANCHOR)
    thpoint_type_export_mp(TT_POINT_TYPE_CAMP,SYMP_CAMP)
    thpoint_type_export_mp(TT_POINT_TYPE_DIG,SYMP_DIG)
    thpoint_type_export_mp(TT_POINT_TYPE_HANDRAIL,SYMP_HANDRAIL)
    thpoint_type_export_mp(TT_POINT_TYPE_VIA_FERRATA,SYMP_VIAFERRATA)
    thpoint_type_export_mp(TT_POINT_TYPE_ALTAR,SYMP_ALTAR)
    thpoint_type_export_mp(TT_POINT_TYPE_ARCHEOEXCAVATION,SYMP_ARCHEOEXCAVATION)
    thpoint_type_export_mp(TT_POINT_TYPE_AUDIO,SYMP_AUDIO)
    thpoint_type_export_mp(TT_POINT_TYPE_BAT,SYMP_BAT)
    thpoint_type_export_mp(TT_POINT_TYPE_BONES,SYMP_BONES)
    thpoint_type_export_mp(TT_POINT_TYPE_CURTAINS,SYMP_CURTAINS)
    thpoint_type_export_mp(TT_POINT_TYPE_DANGER,SYMP_DANGER)
    thpoint_type_export_mp(TT_POINT_TYPE_DISCPILLAR,SYMP_DISCPILLAR)
    thpoint_type_export_mp(TT_POINT_TYPE_DISCPILLARS,SYMP_DISCPILLARS)
    thpoint_type_export_mp(TT_POINT_TYPE_DISCSTALACTITES,SYMP_DISCSTALACTITES)
    thpoint_type_export_mp(TT_POINT_TYPE_DISCSTALAGMITES,SYMP_DISCSTALAGMITES)
    thpoint_type_export_mp(TT_POINT_TYPE_DISCSTALACTITE,SYMP_DISCSTALACTITE)
    thpoint_type_export_mp(TT_POINT_TYPE_DISCSTALAGMITE,SYMP_DISCSTALAGMITE)
    thpoint_type_export_mp(TT_POINT_TYPE_ELECTRICLIGHT,SYMP_ELECTRICLIGHT)
    thpoint_type_export_mp(TT_POINT_TYPE_EXVOTO,SYMP_EXVOTO)
    thpoint_type_export_mp(TT_POINT_TYPE_GATE,SYMP_GATE)
    thpoint_type_export_mp(TT_POINT_TYPE_HELICTITES,SYMP_HELICTITES)
    thpoint_type_export_mp(TT_POINT_TYPE_HUMANBONES,SYMP_HUMANBONES)
    thpoint_type_export_mp(TT_POINT_TYPE_MASONRY,SYMP_MASONRY)
    thpoint_type_export_mp(TT_POINT_TYPE_MINUS,SYMP_MINUS)
    thpoint_type_export_mp(TT_POINT_TYPE_NAMEPLATE,SYMP_NAMEPLATE)
    thpoint_type_export_mp(TT_POINT_TYPE_NO_WHEELCHAIR,SYMP_NOWHEELCHAIR)
    thpoint_type_export_mp(TT_POINT_TYPE_PENDANT,SYMP_PENDANT)
    thpoint_type_export_mp(TT_POINT_TYPE_PILLARWITHCURTAINS,SYMP_PILLARWITHCURTAINS)
    thpoint_type_export_mp(TT_POINT_TYPE_PILLARSWITHCURTAINS,SYMP_PILLARSWITHCURTAINS)
    thpoint_type_export_mp(TT_POINT_TYPE_PHOTO,SYMP_PHOTO)
    thpoint_type_export_mp(TT_POINT_TYPE_PLUS,SYMP_PLUS)
    thpoint_type_export_mp(TT_POINT_TYPE_PLUSMINUS,SYMP_PLUSMINUS)
    thpoint_type_export_mp(TT_POINT_TYPE_SEEDGERMINATION,SYMP_SEEDGERMINATION)
    thpoint_type_export_mp(TT_POINT_TYPE_STALACTITESTALAGMITE,SYMP_STALACTITESTALAGMITE)
    thpoint_type_export_mp(TT_POINT_TYPE_STALACTITESSTALAGMITES,SYMP_STALACTITESSTALAGMITES)
    thpoint_type_export_mp(TT_POINT_TYPE_TREETRUNK,SYMP_TREETRUNK)
    thpoint_type_export_mp(TT_POINT_TYPE_VOLCANO,SYMP_VOLCANO)
    thpoint_type_export_mp(TT_POINT_TYPE_WALKWAY,SYMP_WALKWAY)
    thpoint_type_export_mp(TT_POINT_TYPE_WATERDRIP,SYMP_WATERDRIP)
    thpoint_type_export_mp(TT_POINT_TYPE_WHEELCHAIR,SYMP_WHEELCHAIR)

// ukoncenia chodby
    thpoint_type_export_mp(TT_POINT_TYPE_NARROW_END,SYMP_NARROWEND)
    thpoint_type_export_mp(TT_POINT_TYPE_LOW_END,SYMP_LOWEND)
    thpoint_type_export_mp(TT_POINT_TYPE_FLOWSTONE_CHOKE,SYMP_FLOWSTONECHOKE)
    thpoint_type_export_mp(TT_POINT_TYPE_BREAKDOWN_CHOKE,SYMP_BREAKDOWNCHOKE)
    thpoint_type_export_mp(TT_POINT_TYPE_CLAY_CHOKE,SYMP_CLAYCHOKE)
    thpoint_type_export_mp(TT_POINT_TYPE_MUD,SYMP_MUD)

// vypln
    thpoint_type_export_mp(TT_POINT_TYPE_FLOWSTONE,SYMP_FLOWSTONE)
    thpoint_type_export_mp(TT_POINT_TYPE_MOONMILK,SYMP_MOONMILK)
    thpoint_type_export_mp(TT_POINT_TYPE_STALACTITE,SYMP_STALACTITE)
    thpoint_type_export_mp(TT_POINT_TYPE_STALAGMITE,SYMP_STALAGMITE)
    thpoint_type_export_mp(TT_POINT_TYPE_PILLAR,SYMP_PILLAR)
    thpoint_type_export_mp(TT_POINT_TYPE_STALACTITES,SYMP_STALACTITES)
    thpoint_type_export_mp(TT_POINT_TYPE_STALAGMITES,SYMP_STALAGMITES)
    thpoint_type_export_mp(TT_POINT_TYPE_PILLARS,SYMP_PILLARS)
    thpoint_type_export_mp(TT_POINT_TYPE_ICE_STALACTITE,SYMP_ICESTALACTITE)
    thpoint_type_export_mp(TT_POINT_TYPE_ICE_STALAGMITE,SYMP_ICESTALAGMITE)
    thpoint_type_export_mp(TT_POINT_TYPE_ICE_PILLAR,SYMP_ICEPILLAR)
    thpoint_type_export_mp(TT_POINT_TYPE_CURTAIN,SYMP_CURTAIN)
    thpoint_type_export_mp(TT_POINT_TYPE_HELICTITE,SYMP_HELICTITE)
    thpoint_type_export_mp(TT_POINT_TYPE_SODA_STRAW,SYMP_SODASTRAW)
    thpoint_type_export_mp(TT_POINT_TYPE_CRYSTAL,SYMP_CRYSTAL)
    thpoint_type_export_mp(TT_POINT_TYPE_WALL_CALCITE,SYMP_WALLCALCITE)
    thpoint_type_export_mp(TT_POINT_TYPE_POPCORN,SYMP_POPCORN)
    thpoint_type_export_mp(TT_POINT_TYPE_DISK,SYMP_DISK)
    thpoint_type_export_mp(TT_POINT_TYPE_GYPSUM,SYMP_GYPSUM)
    thpoint_type_export_mp(TT_POINT_TYPE_GYPSUM_FLOWER,SYMP_GYPSUMFLOWER)
    thpoint_type_export_mp(TT_POINT_TYPE_ARAGONITE,SYMP_ARAGONITE)
    thpoint_type_export_mp(TT_POINT_TYPE_CAVE_PEARL,SYMP_CAVEPEARL)
    thpoint_type_export_mp(TT_POINT_TYPE_RIMSTONE_POOL,SYMP_RIMSTONEPOOL)
    thpoint_type_export_mp(TT_POINT_TYPE_RIMSTONE_DAM,SYMP_RIMSTONEDAM)
    thpoint_type_export_mp(TT_POINT_TYPE_ANASTOMOSIS,SYMP_ANASTOMOSIS)
    thpoint_type_export_mp(TT_POINT_TYPE_KARREN,SYMP_KARREN)
    thpoint_type_export_mp(TT_POINT_TYPE_SCALLOP,SYMP_SCALLOP)
    thpoint_type_export_mp(TT_POINT_TYPE_FLUTE,SYMP_FLUTE)
    thpoint_type_export_mp(TT_POINT_TYPE_RAFT_CONE,SYMP_RAFTCONE)
    thpoint_type_export_mp(TT_POINT_TYPE_CLAY_TREE,SYMP_CLAYTREE)


// plosne vyplne
    thpoint_type_export_mp(TT_POINT_TYPE_BEDROCK,SYMP_BEDROCK)
    thpoint_type_export_mp(TT_POINT_TYPE_SAND,SYMP_SAND)
    thpoint_type_export_mp(TT_POINT_TYPE_RAFT,SYMP_RAFT)
    thpoint_type_export_mp(TT_POINT_TYPE_CLAY,SYMP_CLAY)
    thpoint_type_export_mp(TT_POINT_TYPE_PEBBLES,SYMP_PEBBLES)
    thpoint_type_export_mp(TT_POINT_TYPE_DEBRIS,SYMP_DEBRIS)
    thpoint_type_export_mp(TT_POINT_TYPE_BLOCKS,SYMP_BLOCKS)
    thpoint_type_export_mp(TT_POINT_TYPE_WATER,SYMP_WATER)
    thpoint_type_export_mp(TT_POINT_TYPE_ICE,SYMP_ICE)
    thpoint_type_export_mp(TT_POINT_TYPE_SNOW,SYMP_SNOW)
    thpoint_type_export_mp(TT_POINT_TYPE_GUANO,SYMP_GUANO)
    thpoint_type_export_mp(TT_POINT_TYPE_MUDCRACK,SYMP_MUDCRACK)

// ina vypln
    thpoint_type_export_mp(TT_POINT_TYPE_ARCHEO_MATERIAL,SYMP_ARCHEOMATERIAL)
    thpoint_type_export_mp(TT_POINT_TYPE_PALEO_MATERIAL,SYMP_PALEOMATERIAL)
    thpoint_type_export_mp(TT_POINT_TYPE_VEGETABLE_DEBRIS,SYMP_VEGETABLEDEBRIS)
    thpoint_type_export_mp(TT_POINT_TYPE_ROOT,SYMP_ROOT)

    case TT_POINT_TYPE_U:
    	macroid = this->db->db2d.register_u_symbol(TT_POINT_CMD, this->m_subtype_str);
    	break;

    default:
      macroid = SYMP_UNDEFINED;
      break;
  }

  if (postprocess_label != NULL) {
    this->point->export_mp(out);
    fprintf(out->file,",%.1f,%s);\n",(thisnan(this->orient) ? 0 : 360.0 - this->orient - out->rr), postprocess_label);
  }

  omacroid = macroid;
  if (this->context >= 0)
    macroid = this->context;

  if ((macroid > 0) && postprocess) {
    if (out->symset->is_assigned(macroid)) {
      if (out->file == NULL)
        return(true);
      if (attr_text.length() > 0) {
        fprintf(out->file, "picture ATTR__text;\nATTR__text := %s;\n", attr_text.c_str());
      }
      if (this->type == TT_POINT_TYPE_U) {
         out->symset->export_mp_symbol_options(out->file, omacroid);
         fprintf(out->file,"p_u_%s(",this->m_subtype_str);
         out->symset->usymbols[omacroid].m_used = true;
         this->db->db2d.use_u_symbol(this->get_class_id(), this->m_subtype_str);
      } else {
         out->symset->export_mp_symbol_options(out->file, omacroid);
         fprintf(out->file,"%s(",out->symset->get_mp_macro(omacroid));
      }
    }
    else
      postprocess = false;
  }

  if (postprocess) {
    this->point->export_mp(out);
    double scl = this->scale_numeric;
    const char * al = "(0,0)";
    switch (thdb2d_rotate_align(this->align, xrr)) {
      case TT_POINT_ALIGN_B:
        al = "(0,-1)";
        break;
      case TT_POINT_ALIGN_T:
        al = "(0,1)";
        break;
      case TT_POINT_ALIGN_L:
        al = "(-1,0)";
        break;
      case TT_POINT_ALIGN_R:
        al = "(1,0)";
        break;
      case TT_POINT_ALIGN_BL:
        al = "(-1,-1)";
        break;
      case TT_POINT_ALIGN_TL:
        al = "(-1,1)";
        break;
      case TT_POINT_ALIGN_BR:
        al = "(1,-1)";
        break;
      case TT_POINT_ALIGN_TR:
        al = "(1,1)";
        break;
    }
    fprintf(out->file,",%.1f,%.2f,%s",
        (thisnan(this->orient) ? 0 : 360 - this->orient - out->rr),scl,al);
    fprintf(out->file, ");\n");

    if (out->layout->is_debug_stations() || out->layout->is_debug_joins()) {
      fprintf(out->file,"p_debug(-1,0,");
      this->point->export_mp(out);
      fprintf(out->file,");\n");
    }
    if (out->layout->is_debug_joins()) {
      fprintf(out->file,"p_debug(1,0,");
      this->point->export_mp(out,1);
      fprintf(out->file,");\n");
    }
    if (out->layout->is_debug_stations()) {
      fprintf(out->file,"p_debug(0,0,");
      this->point->export_mp(out,0);
      fprintf(out->file,");\n");
    }
    if (attr_text.length() > 0) {
      fprintf(out->file, "save ATTR__text;\n");
    }
  }

  if (station_attr != NULL) {
    station_attr->export_mp_object_end(out->file, station_attr_id);
  }
  th2ddataobject::export_mp_end(out);
  return(false);
}

void thpoint::parse_align(char * tstr) {
  switch (this->type) {
    case TT_POINT_TYPE_STATION:
      ththrow("-align not valid with type {}", thmatch_string(this->type,thtt_point_types));
      break;
  }
  this->align = thmatch_token(tstr, thtt_point_aligns);
  if (this->align == TT_POINT_ALIGN_UNKNOWN)
    ththrow("unknown alignment -- {}", tstr);
}


void thpoint::parse_text(char * ss) {
  switch (this->type) {
    case TT_POINT_TYPE_LABEL:
    case TT_POINT_TYPE_REMARK:
    case TT_POINT_TYPE_STATION_NAME:
    case TT_POINT_TYPE_CONTINUATION:
      break;
    default:
      ththrow("-text not valid with type {}", thmatch_string(this->type,thtt_point_types));
      break;
  }
  if (strlen(ss) > 0)
    this->text = this->db->strstore(ss);
  else
    this->text = NULL;
}


void thpoint::parse_explored(char * ss) {
  switch (this->type) {
    case TT_POINT_TYPE_CONTINUATION:
      break;
    default:
      ththrow("-explored not valid with type {}", thmatch_string(this->type,thtt_point_types));
      break;
  }
  int sv;
  thparse_length(sv, this->xsize, ss);
  if (sv == TT_SV_UNKNOWN)
      ththrow("ivalid explored length -- {}", ss);
}


void thpoint_parse_value(int & sv, double & dv, bool & qw, int & sign, char * str) {

  // odstrani znamienka a nastavi sign
  sign = 0;
  qw = false;
  bool error = false;
  sv = TT_SV_NAN;
  dv = thnan;
  if (strlen(str) > 0) {
    switch (str[0]) {
      case '+':
        sign = 1;
        str++;
        break;
      case '-':
        sign = -1;
        str++;
        break;
    }
  } else
    error = true;

  // skusi odstranit otaznik
  size_t strl = strlen(str);
  if ((strl > 0) && (!error)) {
    if (str[strl-1] == '?') {
      str[strl-1] = 0;
      qw = true;
    }
  }

  // parsne cislo
  if (strlen(str)) {
    thparse_double(sv,dv,str);
    if ((sv != TT_SV_NAN) && (sv != TT_SV_NUMBER))
      error = true;
    else if ((sv != TT_SV_NAN) && (dv < 0.0))
      error = true;
  }

  if ((sv == TT_SV_NAN) && (sign == 0))
    error = true;

  if (error)
    ththrow("invalid value -- {}",str);

}


void thpoint::parse_value(char * ss, bool is_dist) {
  bool opt_ok = false;
  switch (this->type) {
    case TT_POINT_TYPE_EXTRA:
      opt_ok = is_dist;
      if (!opt_ok) {
          thwarning(("%s [%lu] -- using -value with point extra is deprecated, please use -dist instead", this->source.name, this->source.line));
    	  opt_ok = true;
      }
      break;
    case TT_POINT_TYPE_ALTITUDE:
    case TT_POINT_TYPE_HEIGHT:
    case TT_POINT_TYPE_PASSAGE_HEIGHT:
    case TT_POINT_TYPE_DIMENSIONS:
    case TT_POINT_TYPE_DATE:
      opt_ok = !is_dist;
	  break;
  }
  if (!opt_ok)
	  ththrow("{} not valid with type {}", (is_dist ? "-dist" : "-value"),  thmatch_string(this->type,thtt_point_types));

  thsplit_words(& this->db->db2d.mbf,ss);
  int npar = this->db->db2d.mbf.get_size();
  char ** pars = this->db->db2d.mbf.get_buffer();
  int sv, ux, vx, sv2;
  bool quest, quest2;
  double dv, dv2;
  int sign, sign2;
  thtflength lentf;
  thdate * dp;

  switch (this->type) {

    case TT_POINT_TYPE_EXTRA:
      ux = 0;
      switch (npar) {
        case 1:
          break;
        case 2:
          ux = 1;
          break;
        default:
          ththrow("invalid distance -- {}",ss);
      }
      this->xsize = thnan;
      thparse_double(sv,dv,pars[0]);
      if (sv != TT_SV_NUMBER)
        ththrow("not a number -- {}", pars[0]);
      if (ux > 0) {
        lentf.parse_units(pars[ux]);
        dv = lentf.transform(dv);
      }
      this->xsize = dv;
      break;

    // let's parse altitude
    case TT_POINT_TYPE_ALTITUDE:
      thparse_altitude(ss, this->xsize, this->ysize);
      break;

    case TT_POINT_TYPE_HEIGHT:
      ux = 0;
      switch (npar) {
        case 1:
          break;
        case 2:
          ux = 1;
          break;
        default:
          ththrow("invalid value -- {}",ss);
      }
      this->xsize = thnan;
      this->tags &= ~(TT_POINT_TAG_HEIGHT_ALL);
      thpoint_parse_value(sv,dv,quest,sign,pars[0]);
      if ((ux > 0) && (!thisnan(dv))) {
        lentf.parse_units(pars[ux]);
        dv = lentf.transform(dv);
      }
      if (sign < 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_N;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_NQ;
      }
      else if (sign > 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_P;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_PQ;
      }
      else {
        this->tags |= TT_POINT_TAG_HEIGHT_U;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_UQ;
      }
      this->xsize = dv;
      break;

    case TT_POINT_TYPE_DIMENSIONS:
      this->xsize = thnan;
      this->ysize = thnan;
      switch (npar) {
        case 3:
          // parse units
          lentf.parse_units(pars[2]);
      		[[fallthrough]];
        case 2:
          thparse_double(sv,dv,pars[0]);
          if (sv != TT_SV_NUMBER) {
            ththrow("invalid above dimension -- {}",pars[0]);
          }
          this->xsize = lentf.transform(dv);
          if (this->xsize < 0.0)
            ththrow("negative above dimension -- {}",pars[0]);

          thparse_double(sv,dv,pars[1]);
          if (sv != TT_SV_NUMBER) {
            ththrow("invalid below dimension -- {}",pars[1]);
          }
          this->ysize = lentf.transform(dv);
          if (this->ysize < 0.0)
            ththrow("negative below dimension -- {}",pars[1]);
          break;
        default:
          ththrow("invalid value -- {}",ss);
      }
      break;

    case TT_POINT_TYPE_PASSAGE_HEIGHT:
      ux = 0;
      vx = 0;
      switch (npar) {
        case 1:
          break;
        case 2:
          ux = 1;
          try {
            lentf.parse_units(pars[ux]);
          } catch (...) {
            ux = 0;
            vx = 1;
          }
          break;
        case 3:
          ux = 2;
          vx = 1;
          break;
        default:
          ththrow("invalid value -- {}",ss);
      }
      this->xsize = thnan;
      this->ysize = thnan;
      this->tags &= ~(TT_POINT_TAG_HEIGHT_ALL);
      thpoint_parse_value(sv,dv,quest,sign,pars[0]);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid value -- {}",pars[0]);
      if (vx > 0) {
        thpoint_parse_value(sv2,dv2,quest2,sign2,pars[vx]);
        if (sv2 != TT_SV_NUMBER)
          ththrow("invalid value -- {}",pars[vx]);
        if ((sign == 0) || (sign2 == 0) || (sign == sign2))
          ththrow("invalid combination of values -- {}",ss);
      }
      if ((ux > 0) && (!thisnan(dv))) {
        lentf.parse_units(pars[ux]);
        dv = lentf.transform(dv);
        if (vx > 0)
          dv2 = lentf.transform(dv2);
      }
      if (sign < 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_N;
        this->ysize = dv;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_NQ;
      }
      else if (sign > 0) {
        this->tags |= TT_POINT_TAG_HEIGHT_P;
        this->xsize = dv;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_PQ;
      }
      else {
        this->tags |= TT_POINT_TAG_HEIGHT_U;
        this->xsize = dv;
        if (quest)
          this->tags |= TT_POINT_TAG_HEIGHT_UQ;
      }
      if (vx > 0) {
        if (sign2 < 0) {
          this->tags |= TT_POINT_TAG_HEIGHT_N;
          this->ysize = dv2;
          if (quest)
            this->tags |= TT_POINT_TAG_HEIGHT_NQ;
        }
        else if (sign2 > 0) {
          this->tags |= TT_POINT_TAG_HEIGHT_P;
          this->xsize = dv2;
          if (quest)
            this->tags |= TT_POINT_TAG_HEIGHT_PQ;
        }
      }
      break;

    case TT_POINT_TYPE_DATE:
      if (npar != 1)
        ththrow("invalid date -- {}",ss);
      dp = (thdate *) this->text;
      dp->parse(pars[0]);
      this->tags |= TT_POINT_TAG_DATE;
      break;
  }
}


void thpoint::check_extra()
{
  thdb2dcp * cp;
  if (this->from_name.id == 0) {
    double mind = -1.0, cd;
    cp = this->fscrapptr->fcpp;
    while (cp != NULL) {
      if (cp->st != NULL) {
        cd = thvec2(this->point->x - cp->pt->x, this->point->y - cp->pt->y).length();
        if ((mind < 0.0) || (cd < mind)) {
          this->from_name.id = cp->st->uid;
          mind = cd;
        }
      }
      cp = cp->nextcp;
    }
  }
  if (thisnan(this->xsize) && (this->from_name.id > 0)) {
    size_t myuid;
    thdb2dcp * mycp;
    myuid = thdb.db1d.station_vec[this->from_name.id - 1].uid;
    cp = this->fscrapptr->fcpp;
    mycp = NULL;
    double cdist(0.0);
    size_t ccount(0);
    while (cp != NULL) {
      if ((cp->st != NULL) && (cp->st->uid == myuid)) {
        mycp = cp;
        break;
      }
      cp = cp->nextcp;
    }
    if (mycp == NULL)
      return;
    thdb1d_tree_node * nodes = thdb.db1d.get_tree_nodes();
    thdb1d_tree_arrow * carrow;
    cp = this->fscrapptr->fcpp;
    while (cp != NULL) {
      if ((cp->st != NULL) && (cp->st->uid != myuid)) {
        carrow = nodes[myuid - 1].first_arrow;
        while (carrow != NULL) {
          if (carrow->end_node->uid == cp->st->uid) {
            bool reverse = false;
            if (carrow->is_reversed) reverse = !reverse;
            if (carrow->leg->reverse) reverse = !reverse;
            thdataleg * cl = carrow->leg->leg;
            thline2 xl;
            if (reverse) {
              xl = thline2(
              thvec2(mycp->pt->x, mycp->pt->y),
              thvec2(cp->pt->x, cp->pt->y));
            } else {
              xl = thline2(
              thvec2(cp->pt->x, cp->pt->y),
              thvec2(mycp->pt->x, mycp->pt->y));
            }

            ccount++;
            switch (this->fscrapptr->proj->type) {
              case TT_2DPROJ_ELEV:
              case TT_2DPROJ_EXTEND:
                if (this->point->y > mycp->pt->y) {
                  if (!reverse)
                    cdist += cl->from_up;
                  else
                    cdist += cl->to_up;
                } else {
                  if (!reverse)
                    cdist += cl->from_down;
                  else
                    cdist += cl->to_down;
                }
                break;
              default:
                if (xl.eval(thvec2(this->point->x, this->point->y)) < 0.0) {
                  if (!reverse)
                    cdist += cl->from_left;
                  else
                    cdist += cl->to_left;
                } else {
                  if (!reverse)
                    cdist += cl->from_right;
                  else
                    cdist += cl->to_right;
                }
            }
          }
          carrow = carrow->next_arrow;
        }
      }
      cp = cp->nextcp;
    }
    if (ccount > 0)
      this->xsize = cdist / double(ccount);
  }
}

thdate * thpoint::get_date()
{
  if (this->type == TT_POINT_TYPE_DATE) {
    return (thdate *) this->text;
  }
  return nullptr;
}




