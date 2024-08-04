/**
 * @file thlayout.cxx
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
 
#include "thlayout.h"
#include "thlookup.h"
#include "thexception.h"
#include "thchenc.h"
#include "thtfangle.h"
#include "thparse.h"
#include "thinfnan.h"
#include "thpdfdata.h"
#include "thsymbolset.h"
#include "thtflength.h"
#include "thlang.h"
#include "thcsdata.h"
#include "thconfig.h"
#include "th2ddataobject.h"
#include "thdatabase.h"
#include "therion.h"
#include <string.h>
#include <filesystem>
#include <algorithm>
#include <cassert>

namespace fs = std::filesystem;

enum {
  TTLDBG_JOINS = 1,
  TTLDBG_STATIONS = 2,
  TTLDBG_SCRAPNAMES = 4,
  TTLDBG_STATIONNAMES = 8,
};



thlayout::thlayout()
{

  this->ccode = TT_LAYOUT_CODE_UNKNOWN;
  this->m_pconfig = NULL;
  this->m_lookup = NULL;

  this->def_scale = 0;
  this->scale = 0.005;
  this->def_base_scale = 0;
  this->base_scale = 0.005;
  
  this->def_rotate = 0;
  this->rotate = 0.0;
  
  this->def_origin = 0;
  this->ox = thnan;
  this->oy = thnan;
  this->oz = thnan;
  this->o_cs = TTCS_LOCAL;

  this->def_size = 0;
  this->hsize = 0.18;
  this->vsize = 0.222;
  
  this->def_page_setup = 0;
  this->paphs = 0.21;
  this->papvs = 0.297;
  this->paghs = 0.20;
  this->pagvs = 0.287;
  this->marls = 0.005;
  this->marts = 0.005;
  

  this->def_overlap = 0;
  this->overlap = 0.01;
  
  this->def_grid_origin = 0;
  this->gox = thnan;
  this->goy = thnan;
  this->goz = thnan;
  this->go_cs = TTCS_LOCAL;

  
  this->def_grid_size = 0;
  this->gxs = thnan;
  this->gys = thnan;
  this->gzs = thnan;
  
  this->def_min_symbol_scale = 0;
  this->min_symbol_scale = 0.0;

  this->def_font_setup = 0;
  this->font_setup[0] = thnan;

  this->def_origin_label = 0;
  this->olx = "0";
  this->oly = "0";
  
  this->def_nav_factor = 0;
  this->navf = 30.0;
  
  this->def_nav_size = 0;
  this->navsx = 2;
  this->navsy = 2;
  
  this->def_own_pages = 0;
  this->ownp = 0;
  
  this->def_title_pages = 0;
  this->titlep = false;
  
  this->def_doc_title = 0;
  this->doc_title = "";
  
  this->def_doc_comment = 0;
  this->doc_comment = "";
  
  this->def_doc_author = 0;
  this->doc_author = "";
  
  this->def_doc_keywords = 0;
  this->doc_keywords = "";
  
  this->def_doc_subject = 0;
  this->doc_subject = "";
  
  this->def_excl_pages = 0;
  this->excl_pages = false;
  this->excl_list = NULL;
  
  this->def_opacity = 0;
  this->opacity = 0.75;

  this->def_map_header_bg = 0;
  this->map_header_bg = false;

  this->def_grid_coords = 0;
  this->grid_coords = TT_LAYOUT_GRIDCOORDS_OFF;

  this->def_surface = 0;
  this->surface = TT_LAYOUT_SURFACE_OFF;
  this->def_surface_opacity = 0;
  this->surface_opacity = 0.75;

  this->def_north = 0;
  this->north = TT_LAYOUT_NORTH_TRUE;
  
  this->def_smooth_shading = 0;
  this->smooth_shading = TT_LAYOUT_SMOOTHSHADING_QUICK;

  this->def_transparency = 0;
  this->transparency = true;

  this->def_sketches = 0;
  this->sketches = false;

  this->def_legend = 0;
  this->legend = TT_LAYOUT_LEGEND_OFF;
  this->def_legend_width = 0;
  this->legend_width = 0.14;
  this->def_legend_columns = 0;
  this->legend_columns = 2;

  this->def_color_legend = 0;
  this->color_legend = TT_LAYOUT_COLORLEGEND_SMOOTH;

  this->def_color_model = 0;
  this->color_model = TT_LAYOUTCLRMODEL_CMYK;

  this->def_color_profile_rgb = 0;
  this->color_profile_rgb = "";
  this->def_color_profile_cmyk = 0;
  this->color_profile_cmyk = "";
  this->def_color_profile_gray = 0;
  this->color_profile_gray = "";
  
  this->def_scale_bar = 0;
  this->scale_bar = -1.0;

  this->def_map_header = 0;
  this->map_header = TT_LAYOUT_MAP_HEADER_NE;
  this->map_header_x = 100.0;
  this->map_header_y = 0.0;

  this->def_debug = 0;
  this->debug = 0;

  this->def_survey_level = 0;
  this->survey_level = 0;

  
  this->def_max_explos = 0;
  this->max_explos = -1;
  this->def_max_topos = 0;
  this->max_topos = -1;
  this->def_max_cartos = 0;
  this->max_cartos = -1;
  this->def_max_copys = 0;
  this->max_copys = -1;

  this->def_explo_lens = 0;
  this->explo_lens = TT_LAYOUT_LENSTAT_HIDE;
  this->def_topo_lens = 0;
  this->topo_lens = TT_LAYOUT_LENSTAT_HIDE;
  this->def_carto_lens = 0;
  this->carto_lens = TT_LAYOUT_LENSTAT_HIDE;
  this->def_copy_lens = 0;
  this->copy_lens = TT_LAYOUT_LENSTAT_HIDE;

  this->def_lang = 0;
  this->lang = thcfg.get_lang();
  
  this->def_units = 0;
  this->units = thdeflocale;
  
  this->def_layers = 0;
  this->layers = true;

  this->def_grid = 0;
  this->grid = TT_LAYOUT_GRID_OFF;
  
  this->def_page_grid = 0;
  this->page_grid = false;
  
  this->def_page_numbers = 0;
  this->pgsnum = true;
  
  this->def_color_labels = 0;
  this->color_labels = false;
  
  this->def_tex_lines = 0;
  
  this->lock = false;
  
  this->color_preview_below.R = 0.5;
  this->color_preview_below.G = 0.5;
  this->color_preview_below.B = 0.5;
  
  this->color_preview_above.R = 0;
  this->color_preview_above.G = 0;
  this->color_preview_above.B = 0;
  
  this->color_crit = TT_LAYOUT_CCRIT_UNKNOWN;
  this->color_crit_fname = NULL;
}


thlayout::~thlayout()
{
}


int thlayout::get_class_id() 
{
  return TT_LAYOUT_CMD;
}


bool thlayout::is(int class_id)
{
  if (class_id == TT_LAYOUT_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thlayout::get_cmd_nargs() 
{
  return 1;
}


const char * thlayout::get_cmd_end()
{
  return "endlayout";
}


const char * thlayout::get_cmd_name()
{
  // insert command name here
  return "layout";
}


thcmd_option_desc thlayout::get_default_cod(int id) {
  switch (id) {
    case TT_LAYOUT_ENDCODE:
      return thcmd_option_desc(id,0);
    case TT_LAYOUT_NAV_SIZE:
    case TT_LAYOUT_OVERLAP:
    case TT_LAYOUT_SCALE:
    case TT_LAYOUT_SCALE_BAR:
    case TT_LAYOUT_BASE_SCALE:
    case TT_LAYOUT_EXCLUDE_PAGES:
    case TT_LAYOUT_ORIGIN_LABEL:
    case TT_LAYOUT_SYMBOL_HIDE:
    case TT_LAYOUT_SYMBOL_SHOW:
    case TT_LAYOUT_MAP_ITEM:
    case TT_LAYOUT_COLOR:
    case TT_LAYOUT_LEGEND_WIDTH:
    case TT_LAYOUT_COLOR_PROFILE:
      return thcmd_option_desc(id,2);
    case TT_LAYOUT_SYMBOL_ASSIGN:
    case TT_LAYOUT_SIZE:
    case TT_LAYOUT_MAP_HEADER:
    case TT_LAYOUT_SYMBOL_COLOR:
      return thcmd_option_desc(id,3);
    case TT_LAYOUT_ORIGIN:
    case TT_LAYOUT_MAP_IMAGE:
    case TT_LAYOUT_GRID_SIZE:
    case TT_LAYOUT_GRID_ORIGIN:
      return thcmd_option_desc(id,4);
    case TT_LAYOUT_FONT_SETUP:
      return thcmd_option_desc(id,5);
    case TT_LAYOUT_PAGE_SETUP:
      return thcmd_option_desc(id,7);
    default:
      return thcmd_option_desc(id);
  }
}


thcmd_option_desc thlayout::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_layout_opt);
  if (id == TT_LAYOUT_UNKNOWN) 
    return thdataobject::get_cmd_option_desc(opts);
  else 
    return this->get_default_cod(id);
}

void thlayout_parse_scale(double * scale,char ** args) {
  double dv;
  int sv;
  thparse_double(sv,dv,args[0]);
  if (sv != TT_SV_NUMBER)
    ththrow("invalid number -- {}",args[0]);
  if (dv <= 0.0)
    ththrow("positive number expected -- {}",args[0]);
  *scale = dv;
  thparse_double(sv,dv,args[1]);
  if (sv != TT_SV_NUMBER)
    ththrow("invalid number -- {}",args[1]);
  if (dv <= 0.0)
    ththrow("positive number expected -- {}",args[1]);
  *scale /= dv;  
  if ((*scale > 1.000001e-1) || (*scale < 9.99999e-6))
    ththrow("scale out of range -- {} {}",args[0], args[1]);
}

void thlayout_parse_rotate(double & rotate, char * rotstr) {
  thsplit_words(&(thdb.mbuff_tmp), rotstr);
  int nargs = thdb.mbuff_tmp.get_size(), sv;
  char ** args = thdb.mbuff_tmp.get_buffer();
  thtfangle atf;
  switch (nargs) {
    case 2:
      atf.parse_units(args[1]);
      [[fallthrough]];
    case 1:
      thparse_double(sv, rotate, args[0]);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid rotate specification -- {}", rotstr);
      rotate = atf.transform(rotate);
      break;
    default:
      ththrow("invalid rotate specification -- {}", rotstr);
  }
}

enum {
  TTL_MAPITEM_CARTO,
  TTL_MAPITEM_COPYRIGHT,
  TTL_MAPITEM_EXPLO,
  TTL_MAPITEM_TOPO,
  TTL_MAPITEM_EXPLO_LENS,
  TTL_MAPITEM_TOPO_LENS,
  TTL_MAPITEM_CARTO_LENS,
  TTL_MAPITEM_COPY_LENS,
  TTL_MAPITEM_SORT,
  TTL_MAPITEM_UNKNOWN,
};

static const thstok thlayout_mapitems[] = {
  {"carto", TTL_MAPITEM_CARTO},
  {"carto-count", TTL_MAPITEM_CARTO_LENS},
  {"copyright", TTL_MAPITEM_COPYRIGHT},
  {"copyright-count", TTL_MAPITEM_COPY_LENS},
  {"explo", TTL_MAPITEM_EXPLO},
  {"explo-length", TTL_MAPITEM_EXPLO_LENS},
  {"topo", TTL_MAPITEM_TOPO},
  {"topo-length", TTL_MAPITEM_TOPO_LENS},
  {NULL, TTL_MAPITEM_UNKNOWN},
};



void thlayout::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  double dum;
  int sv, sv2, dum_int;
  const char * tmp1;
  const char * tmp2;
  //bool parsed;
  thlayoutln * last_line = nullptr;
  thcmd_option_desc defcod = this->get_default_cod(cod.id);
  switch (cod.id) {
    case TT_DATAOBJECT_AUTHOR:
    case TT_DATAOBJECT_COPYRIGHT:
      defcod.nargs = 2;
      [[fallthrough]];
    default:
      if (cod.nargs > defcod.nargs)
        ththrow("too many arguments -- {}", args[defcod.nargs]);
  }
  
  switch (cod.id) {

    case 0:
    case TT_LAYOUT_SYMBOL_DEFAULTS:
    case TT_LAYOUT_SYMBOL_ASSIGN:
    case TT_LAYOUT_SYMBOL_HIDE:
    case TT_LAYOUT_SYMBOL_SHOW:
    case TT_LAYOUT_SYMBOL_COLOR:
      last_line = &lines.emplace_back();
      if (this->def_tex_lines < 2) {
        this->def_tex_lines = 2;
      }
      switch(cod.id) {
        case 0:
          if (this->ccode == TT_LAYOUT_CODE_UNKNOWN) {
            ththrow("unknown option -- {}", *args);
          }
          thencode(&(this->db->buff_enc), *args, argenc);
          last_line->line = this->db->strstore(this->db->buff_enc.get_buffer());
          last_line->code = this->ccode;
          last_line->path = this->db->strstore((this->m_pconfig == NULL) ? "" : this->m_pconfig->cfg_file.get_cif_abspath().c_str(), true);
          break;
        case TT_LAYOUT_SYMBOL_DEFAULTS:
          if (args != NULL) {
            if (!th_is_keyword(*args))
              ththrow("invalid keyword -- {}", args[0]);
            thencode(&(this->db->buff_enc), *args, argenc);
            last_line->line = this->db->strstore(this->db->buff_enc.get_buffer());
          }
          last_line->code = TT_LAYOUT_CODE_SYMBOL_DEFAULTS;
          break;
        case TT_LAYOUT_SYMBOL_ASSIGN:
          last_line->smid = thsymbolset_get_id(args[0],args[1]);
          if (last_line->smid == -1)
            ththrow("unknown symbol specification -- {} {}", args[0], args[1]);
          if (!thsymbolset_assign[last_line->smid])
            ththrow("symbol can not be assigned -- {} {}", args[0], args[1]);
          if (!th_is_keyword(args[2]))
            ththrow("invalid keyword -- {}", args[2]);
          thencode(&(this->db->buff_enc), args[2], argenc);
          last_line->line = this->db->strstore(this->db->buff_enc.get_buffer());
          last_line->code = TT_LAYOUT_CODE_SYMBOL_ASSIGN;
          break;
//        case TT_LAYOUT_MAP_ITEM:
//          this->last_line->smid = thmatch_token(args[0],thtt_layout_mapitem);
//          if (this->last_line->smid == SYMS_ZZZ)
//            ththrow("unknown map-item specification -- %s", args[0]);
//          if (!th_is_keyword(args[1]))
//            ththrow("invalid keyword -- %s", args[1]);
//          thencode(&(this->db->buff_enc), args[1], argenc);
//          this->last_line->line = this->db->strstore(this->db->buff_enc.get_buffer());
//          this->last_line->code = TT_LAYOUT_CODE_MAP_ITEM;
//          break;
        case TT_LAYOUT_SYMBOL_HIDE:
          last_line->smid = thsymbolset_get_id(args[0],args[1]);
          if (last_line->smid == -1)
            ththrow("unknown symbol specification -- {} {}", args[0], args[1]);
          last_line->code = TT_LAYOUT_CODE_SYMBOL_HIDE;
          break;
        case TT_LAYOUT_SYMBOL_SHOW:
          last_line->smid = thsymbolset_get_id(args[0],args[1]);
          if (last_line->smid == -1)
            ththrow("unknown symbol specification -- {} {}", args[0], args[1]);
          last_line->code = TT_LAYOUT_CODE_SYMBOL_SHOW;
          break;
        case TT_LAYOUT_SYMBOL_COLOR:
          last_line->smid = thsymbolset_get_id(args[0],args[1]);
          if (last_line->smid == -1)
            ththrow("unknown symbol specification -- {} {}", args[0], args[1]);
          last_line->sclr.parse(args[2]);
          last_line->code = TT_LAYOUT_CODE_SYMBOL_COLOR;
          break;
      }
      break;

    case TT_LAYOUT_SURVEY_LEVEL:
      thparse_double(sv,dum,args[0]);
      switch (sv) {
        case TT_SV_NUMBER:
          dum_int = int(dum);
          if ((double(dum_int) != dum) || (dum_int < 0))
            ththrow("not an integer -- {}", args[1]);
          break;
        case TT_SV_ALL:
          dum_int = -1;
          break;
        case TT_SV_OFF:
          dum_int = 0;
          break;   
        default:
          ththrow("invalid number or switch -- {}", args[1]);
      }
      this->survey_level = dum_int;
      this->def_survey_level = 2;
      break;

    case TT_LAYOUT_MAP_ITEM:
      sv2 = thmatch_token(args[0],thlayout_mapitems);
      switch (sv2) {
        case TTL_MAPITEM_EXPLO_LENS:
          sv = thmatch_token(args[1],thtt_layout_lenstat);
          if (sv == TT_LAYOUT_LENSTAT_UNKNOWN)
            ththrow("invalid map-item explo-length switch -- {}",args[1]);
          this->explo_lens = sv;
          this->def_explo_lens = 2;
          break;
        case TTL_MAPITEM_TOPO_LENS:
          sv = thmatch_token(args[1],thtt_layout_lenstat);
          if (sv == TT_LAYOUT_LENSTAT_UNKNOWN)
            ththrow("invalid map-item topo-length switch -- {}",args[1]);
          this->topo_lens = sv;
          this->def_topo_lens = 2;
          break;
        case TTL_MAPITEM_CARTO_LENS:
          sv = thmatch_token(args[1],thtt_layout_lenstat);
          if (sv == TT_LAYOUT_LENSTAT_UNKNOWN)
            ththrow("invalid map-item carto-length switch -- {}",args[1]);
          this->carto_lens = sv;
          this->def_carto_lens = 2;
          break;
        case TTL_MAPITEM_COPY_LENS:
          sv = thmatch_token(args[1],thtt_layout_lenstat);
          if (sv == TT_LAYOUT_LENSTAT_UNKNOWN)
            ththrow("invalid map-item copyright-length switch -- {}",args[1]);
          this->copy_lens = sv;
          this->def_copy_lens = 2;
          break;
        case TTL_MAPITEM_EXPLO:
        case TTL_MAPITEM_TOPO:
        case TTL_MAPITEM_CARTO:
        case TTL_MAPITEM_COPYRIGHT:
          thparse_double(sv,dum,args[1]);
          switch (sv) {
            case TT_SV_NUMBER:
              dum_int = int(dum);
              if ((double(dum_int) != dum) || (dum_int < 0))
                ththrow("not a non-negative integer -- {}", args[1]);
              break;
            case TT_SV_ALL:
              dum_int = -1;
              break;
            case TT_SV_OFF:
              dum_int = 0;
              break;   
            default:
              ththrow("invalid number or switch -- {}", args[1]);
          }
          switch (sv2) {
            case TTL_MAPITEM_EXPLO:
              this->max_explos = dum_int;
              this->def_max_explos = 2;
              break;
            case TTL_MAPITEM_TOPO:
              this->max_topos = dum_int;
              this->def_max_topos = 2;
              break;
            case TTL_MAPITEM_CARTO:
              this->max_cartos = dum_int;
              this->def_max_cartos = 2;
              break;
            case TTL_MAPITEM_COPYRIGHT:
              this->max_copys = dum_int;
              this->def_max_copys = 2;
              break;
          }
          break;
        default:
          ththrow("unknown statistics -- {}", args[0]);
      }
      break;
      
    case TT_LAYOUT_SCALE:
      thlayout_parse_scale(&(this->scale),args);
      this->def_scale = 2;
      break;    
  
    case TT_LAYOUT_ROTATE:
      thlayout_parse_rotate(this->rotate,args[0]);
      this->def_rotate = 2;
      break;

    case TT_LAYOUT_BASE_SCALE:
      thlayout_parse_scale(&(this->base_scale),args);
      this->def_base_scale = 2;
      break;    

    case TT_LAYOUT_OVERLAP:
      this->parse_len(this->overlap, dum, dum, 1, args, 0);
      this->def_overlap = 2;
      break;

    case TT_LAYOUT_LEGEND_WIDTH:
      this->parse_len(this->legend_width, dum, dum, 1, args, 0);
      this->def_legend_width = 2;
      break;

    case TT_LAYOUT_LEGEND_COLUMNS:
      thparse_double(sv,dum,args[0]);
      switch (sv) {
        case TT_SV_NUMBER:
          dum_int = int(dum);
          if ((double(dum_int) != dum) || (dum_int < 0))
            ththrow("not a non-negative integer -- {}", args[0]);
          break;
        default:
          ththrow("invalid number -- {}", args[0]);
      }
      this->legend_columns = (unsigned) dum_int;
      this->def_legend_columns = 2;
      break;

    case TT_LAYOUT_SIZE:
      this->parse_len(this->hsize, this->vsize, dum, 2, args, 1);
      this->def_size = 2;
      break;

    case TT_LAYOUT_PAGE_SETUP:
      this->parse_len6(this->paphs, this->papvs, this->paghs, this->pagvs, this->marls, this->marts, 6, args, 1);
      this->def_page_setup = 2;
      break;

    case TT_LAYOUT_ORIGIN:
      //this->parse_len(this->ox, this->oy, this->oz, 3, args, -1);
      this->o_cs = this->cs;
      this->read_cs(args[0], args[1], this->ox, this->oy, false);
      if (this->cs == TTCS_LOCAL) {
        this->parse_len(this->ox, this->oy, this->oz, 3, args, -1);
      } else {
        this->parse_len(this->oz, dum, dum, 1, &(args[2]), -1);
      }
      this->def_origin = 2;
      break;

    case TT_LAYOUT_GRID_SIZE:
      this->parse_len(this->gxs, this->gys, this->gzs, 3, args, 1);
      this->def_grid_size = 2;
      break;

    case TT_LAYOUT_FONT_SETUP:
      for(sv2 = 0; sv2 < 5; sv2++) {
        thparse_double(sv,dum,args[sv2]);
        if ((sv != TT_SV_NUMBER) || (dum <= 0.0)) {
          ththrow("invalid font size -- {}", args[sv2]);
        } else {
          if ((sv2 > 0) && (dum < this->font_setup[sv2-1])) {
            ththrow("font size should be increasing by scale -- {}", args[sv2]);
          }
          this->font_setup[sv2] = dum;
        }
      }
      this->def_font_setup = 2;
      break;

    case TT_LAYOUT_MIN_SYMBOL_SCALE:
      th2dparse_scale(args[0], sv, this->min_symbol_scale);
      this->def_min_symbol_scale = 2;
      break;

    case TT_LAYOUT_COLOR:
      sv = thmatch_token(args[0],thtt_layout_color);
      switch (sv) {
        case TT_LAYOUT_COLOR_LABELS:
          sv2 = thmatch_token(args[1], thtt_bool);
          if (sv2 == TT_UNKNOWN_BOOL)
            ththrow("invalid color labels switch (on|off expected) -- {}",args[1]);
          else {
            this->color_labels = (sv2 == TT_TRUE);
	    this->def_color_labels = 2;
          }
	  break;
        case TT_LAYOUT_COLOR_MAP_FG:
          thlookup_parse_reference(args[1], &(this->color_crit), &tmp1, &tmp2);
          //this->color_crit = thmatch_token(args[1], thtt_layout_ccrit);
          if (this->color_crit == TT_LAYOUT_CCRIT_UNKNOWN)
            this->color_map_fg.parse(args[1]);
          else {
            this->color_crit_fname = thdb.strstore(args[1]);
            this->color_map_fg.defined = 2;
          }
          break;
        case TT_LAYOUT_COLOR_MAP_BG:
          this->color_map_bg.parse(args[1], true);
          break;
        case TT_LAYOUT_COLOR_PREVIEW_BELOW:
          this->color_preview_below.parse(args[1]);
          break;
        case TT_LAYOUT_COLOR_PREVIEW_ABOVE:
          this->color_preview_above.parse(args[1]);
          break;
        default:
        ththrow("unknown color -- {}",args[0]);
      }
      break;
    

    case TT_LAYOUT_GRID_ORIGIN:
      this->go_cs = this->cs;
      this->read_cs(args[0], args[1], this->gox, this->goy, false);
      if (this->cs == TTCS_LOCAL) {
        this->parse_len(this->gox, this->goy, this->goz, 3, args, -1);
      } else {
        this->parse_len(this->goz, dum, dum, 1, &(args[2]), -1);
      }
      this->def_grid_origin = 2;
      break;

    case TT_LAYOUT_TRANSPARENCY:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid transparency switch -- {}",args[0]);
      this->transparency = (sv == TT_TRUE);
      this->def_transparency = 2;
      break;

    case TT_LAYOUT_SKETCHES:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid sketches switch -- {}",args[0]);
      this->sketches = (sv == TT_TRUE);
      this->def_sketches = 2;
      break;
    
    case TT_LAYOUT_LEGEND:
      sv = thmatch_token(args[0],thtt_layout_legend);
      if (sv == TT_LAYOUT_LEGEND_UNKNOWN)
        ththrow("invalid legend switch -- {}",args[0]);
      this->legend = sv;
      this->def_legend = 2;
      break;
    
    case TT_LAYOUT_COLOR_LEGEND:
      sv = thmatch_token(args[0],thtt_layout_colorlegend);
      if (sv == TT_LAYOUT_COLORLEGEND_UNKNOWN)
        ththrow("invalid color-legend switch -- {}",args[0]);
      this->color_legend = sv;
      this->def_color_legend = 2;
      break;

    case TT_LAYOUT_COLOR_MODEL:
      sv = thmatch_token(args[0],thtt_layoutclr_model);
      if (sv == TT_LAYOUTCLRMODEL_UNKNOWN)
        ththrow("invalid color-model switch -- {}",args[0]);
      this->color_model = sv;
      this->def_color_model = 2;
      break;


    case TT_LAYOUT_COLOR_PROFILE:
      sv = thmatch_token(args[0],thtt_layoutclr_model);
      tmp1 = this->db->strstore((this->m_pconfig == NULL) ? "" : this->m_pconfig->cfg_file.get_cif_abspath(args[1]).c_str(), true);
      switch (sv) {
      case TT_LAYOUTCLRMODEL_CMYK:
    	  this->color_profile_cmyk = tmp1;
          this->def_color_profile_cmyk = 2;
    	  break;
      case TT_LAYOUTCLRMODEL_RGB:
    	  this->color_profile_rgb = tmp1;
          this->def_color_profile_rgb = 2;
    	  break;
      case TT_LAYOUTCLRMODEL_GRAY:
    	  this->color_profile_gray = tmp1;
          this->def_color_profile_gray = 2;
    	  break;
      default:
          ththrow("invalid color-profile model -- {}",args[0]);
      }
      break;
      
    
    case TT_LAYOUT_SCALE_BAR:
      this->parse_len(this->scale_bar, dum, dum, 1, args, 1);
      this->def_scale_bar = 2;
      break;
    
    case TT_LAYOUT_MAP_HEADER:

      thparse_double(sv,this->map_header_x,args[0]);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid number -- {}",args[0]);
      if ((this->map_header_x < -100.0) || (this->map_header_x > 200.0))
        ththrow("number between 0.0 - 100.0 expected -- {}",args[0]);

      thparse_double(sv,this->map_header_y,args[1]);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid number -- {}",args[1]);
      if ((this->map_header_y < -100.0) || (this->map_header_y > 200.0))
        ththrow("number between 0.0 - 100.0 expected -- {}",args[1]);

      sv = thmatch_token(args[2],thtt_layout_map_header);
      if (sv == TT_LAYOUT_MAP_HEADER_UNKNOWN)
        ththrow("invalid map-header switch -- {}",args[2]);
      this->map_header = sv;
      this->def_map_header = 2;
      break;
    
    case TT_LAYOUT_MAP_IMAGE:
      {
        thlayout_map_image tmpi;
        tmpi.parse(args, (this->m_pconfig == NULL) ? "" : this->m_pconfig->cfg_file.get_cif_path());
        this->map_image_list.push_back(tmpi);
      }
      break;
    
    case TT_LAYOUT_DEBUG:
      sv = thmatch_token(args[0],thtt_layout_debug);
      if (sv == TT_LAYOUT_DEBUG_UNKNOWN)
        ththrow("invalid debug switch -- {}",args[0]);
      switch (sv) {
        case TT_LAYOUT_DEBUG_ALL:
          this->debug = TTLDBG_JOINS | TTLDBG_STATIONS | TTLDBG_SCRAPNAMES | TTLDBG_STATIONNAMES;
          break;
        case TT_LAYOUT_DEBUG_OFF:
          this->debug = 0;
          break;
        case TT_LAYOUT_DEBUG_STATIONS:
          this->debug |= TTLDBG_STATIONS;
          break;
        case TT_LAYOUT_DEBUG_JOINS:
          this->debug |= TTLDBG_JOINS;
          break;
        case TT_LAYOUT_DEBUG_SCRAPNAMES:
          this->debug |= TTLDBG_SCRAPNAMES;
          break;
        case TT_LAYOUT_DEBUG_STATIONNAMES:
          this->debug |= TTLDBG_STATIONNAMES;
          break;
      }
      this->def_debug = 2;
      break;
    
    case TT_LAYOUT_LANG:
      sv = thlang_parse(args[0]);
      if (sv == THLANG_UNKNOWN)
        ththrow("language not supported -- {}",args[0]);
      this->lang = sv;
      this->def_lang = 2;
      break;
    
    case TT_LAYOUT_UNITS:
      this->units.parse_units(args[0]);
      this->def_units = 2;
      break;
    
    case TT_LAYOUT_LAYERS:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid layers switch -- {}",args[0]);
      this->layers = (sv == TT_TRUE);
      this->def_layers = 2;
      break;

    case TT_LAYOUT_MAP_HEADER_BG:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid map-header-bg switch -- {}",args[0]);
      this->map_header_bg = (sv == TT_TRUE);
      this->def_map_header_bg = 2;
      break;
    
    case TT_LAYOUT_OPACITY:
      thparse_double(sv,dum,args[0]);        
      if ((sv != TT_SV_NUMBER) || (dum < 0.0) || (dum > 100.0))
        ththrow("invalid opacity value -- {}", args[0]);
      this->opacity = dum / 100.0;
      this->def_opacity = 2;
      break;
    
    case TT_LAYOUT_SURFACE_OPACITY:
      thparse_double(sv,dum,args[0]);        
      if ((sv != TT_SV_NUMBER) || (dum < 0.0) || (dum > 100.0))
        ththrow("invalid opacity value -- {}", args[0]);
      this->surface_opacity = dum / 100.0;
      this->def_surface_opacity = 2;
      break;
    
    case TT_LAYOUT_SURFACE:
      sv = thmatch_token(args[0],thtt_layout_surface);
      if (sv == TT_LAYOUT_SURFACE_UNKNOWN)
        ththrow("invalid surface switch -- {}",args[0]);
      this->surface = sv;
      this->def_surface = 2;
      break;
    
    case TT_LAYOUT_GRID_COORDS:
      sv = thmatch_token(args[0],thtt_layout_gridcoords);
      if (sv == TT_LAYOUT_GRIDCOORDS_UNKNOWN)
        ththrow("invalid grid-coords switch -- {}",args[0]);
      this->grid_coords = sv;
      this->def_grid_coords = 2;
      break;

    case TT_LAYOUT_NORTH:
      sv = thmatch_token(args[0],thtt_layout_north);
      if (sv == TT_LAYOUT_NORTH_UNKNOWN)
        ththrow("invalid north switch -- {}",args[0]);
      this->north = sv;
      this->def_north = 2;
      break;

    case TT_LAYOUT_SMOOTH_SHADING:
      sv = thmatch_token(args[0],thtt_layout_smoothshading);
      if (sv == TT_LAYOUT_SMOOTHSHADING_UNKNOWN)
        ththrow("invalid smooth-shading switch -- {}",args[0]);
      this->smooth_shading = sv;
      this->def_smooth_shading = 2;
      break;
      
    case TT_LAYOUT_GRID:
      sv = thmatch_token(args[0],thtt_layout_grid);
      if (sv == TT_LAYOUT_GRID_UNKNOWN)
        ththrow("invalid grid switch -- {}",args[0]);
      this->grid = (char) sv;
      this->def_grid = 2;
      break;
      
    case TT_LAYOUT_ENDCODE:
      this->ccode = TT_LAYOUT_CODE_UNKNOWN;
      break;
      
    case TT_LAYOUT_CODE:
      sv = thmatch_token(args[0],thtt_layout_code);
      if (sv == TT_LAYOUT_CODE_UNKNOWN)
        ththrow("invalid code switch -- {}",args[0]);
      this->ccode = (char) sv;
      break;
      
    case TT_LAYOUT_PAGE_GRID:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid page-grid switch -- {}",args[0]);
      this->page_grid = (sv == TT_TRUE);
      this->def_page_grid = 2;
      break;
    
    case TT_LAYOUT_EXCLUDE_PAGES:

      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid pages exclusion switch -- {}",args[0]);
      this->excl_pages = (sv == TT_TRUE);

      if (strlen(args[1]) > 0)
        this->excl_list = this->db->strstore(args[1]);
      else if (this->excl_pages)
        ththrow("invalid pages exclusion list -- {}",args[1]);

      this->def_excl_pages = 2;
      break;
    
    case TT_LAYOUT_PAGE_NUMBERS:
      sv = thmatch_token(*args,thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid page numbers switch -- {}",*args);
      this->pgsnum = (sv == TT_TRUE);
      this->def_page_numbers = 2;
      break;
    
    case TT_LAYOUT_TITLE_PAGES:
      sv = thmatch_token(*args,thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow("invalid title-pages switch -- {}",*args);
      this->titlep = (sv == TT_TRUE);
      this->def_title_pages = 2;
      break;
    
    case TT_LAYOUT_DOC_TITLE:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_title = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_title = "";
      this->def_doc_title = 2;  
      break;
    
    case TT_LAYOUT_DOC_COMMENT:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_comment = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_comment = "";
      this->def_doc_comment = 2;  
      break;
    
    case TT_LAYOUT_DOC_AUTHOR:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_author = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_author = "";
      this->def_doc_author = 2;  
      break;
    
    case TT_LAYOUT_DOC_SUBJECT:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_subject = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_subject = "";
      this->def_doc_subject = 2;  
      break;
    
    case TT_LAYOUT_DOC_KEYWORDS:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_keywords = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_keywords = "";
      this->def_doc_keywords = 2;  
      break;
    
    case TT_LAYOUT_ORIGIN_LABEL:

      if (strlen(args[0]) > 0) {
        //thencode(&(this->db->buff_enc), args[0], argenc);
        this->olx = this->db->strstore(args[0]);
      } else
        ththrow("invalid label -- {}",args[0]);

      if (strlen(args[1]) > 0) {
        //thencode(&(this->db->buff_enc), args[1], argenc);
        this->oly = this->db->strstore(args[1]);
      } else
        ththrow("invalid label -- {}",args[1]);

      this->def_origin_label = 2;
      break;
    
    case TT_LAYOUT_NAV_SIZE:
    
      thparse_double(sv,dum,args[0]);        
      if ((sv != TT_SV_NUMBER) || (dum <= 0))
        ththrow("invalid navigator size -- {}", *args);
      if (double(int(dum)) != dum)
        ththrow("invalid navigator size -- {}", *args);
      this->navsx = unsigned(dum);

      thparse_double(sv,dum,args[1]);        
      if ((sv != TT_SV_NUMBER) || (dum <= 0))
        ththrow("invalid navigator size -- {}", *args);
      if (double(int(dum)) != dum)
        ththrow("invalid navigator size -- {}", *args);
      this->navsy = unsigned(dum);
      
      this->def_nav_size = 2;
      break;
      
    case TT_LAYOUT_COPY:
      if (th_is_extkeyword(*args)) {
        auto lcp = &lines.emplace_back();
        lcp->code = TT_LAYOUT_CODE_COPY;
        lcp->line = this->db->strstore(*args);
      } else 
        ththrow("invalid keyword -- {}", *args);
      break;
    
    case TT_LAYOUT_NAV_FACTOR:
      thparse_double(sv,this->navf,*args);
      if (sv != TT_SV_NUMBER)
        ththrow("invalid factor -- {}", *args);
      if (this->navf <= 0.0)
        ththrow("negative factor not allowed -- {}", *args);
      this->def_nav_factor = 2;
      break;
    
    case TT_LAYOUT_OWN_PAGES:
      thparse_double(sv,dum,*args);        
      if ((sv != TT_SV_NUMBER) || (dum <= 0))
        ththrow("invalid number of own pages -- {}", *args);
      if (double(int(dum)) != dum)
        ththrow("invalid number of own pages -- {}", *args);
      this->ownp = unsigned(dum);
      this->def_own_pages = 2;
      break;
    
    case 1:
      cod.id = TT_DATAOBJECT_NAME;
      [[fallthrough]];
    default:
      thdataobject::set(cod, args, argenc, indataline);
      break;
  }
}


void thlayout::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thlayout:\n");
  fprintf(outf,"\tscale: %f\n",this->scale);
  fprintf(outf,"\trotate: %f\n",this->rotate);
}


int thlayout::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}


void thlayout::self_print_library() {

  thprintf("\toname = \"%s\";\n", this->get_name());
  thprintf("\tplayout->set(thcmd_option_desc(TT_DATAOBJECT_NAME,1),oname,TT_UTF_8,0);\n");
  // decode title
  thdecode_c(&(this->db->buff_enc), this->get_title());
  thprintf("\toname = \"%s\";\n", this->db->buff_enc.get_buffer());
  thprintf("\tplayout->set(thcmd_option_desc(TT_DATAOBJECT_TITLE,1),oname,TT_UTF_8,0);\n");


  thprintf("\tplayout->def_scale = %d;\n", this->def_scale);
  thprintf("\tplayout->scale = %lg;\n",this->scale);
  thprintf("\tplayout->def_base_scale = %d;\n", this->def_base_scale);
  thprintf("\tplayout->base_scale = %lg;\n",this->base_scale);

  thprintf("\tplayout->def_rotate = %d;\n", this->def_rotate);
  thprintf("\tplayout->rotate = %lg;\n",this->rotate);

  thprintf("\tplayout->def_page_setup = %d;\n", this->def_page_setup);
  thprintf("\tplayout->hsize = %lg;\n",this->hsize);
  thprintf("\tplayout->vsize = %lg;\n",this->vsize);
  thprintf("\tplayout->paphs = %lg;\n",this->paphs);
  thprintf("\tplayout->papvs = %lg;\n",this->papvs);
  thprintf("\tplayout->paghs = %lg;\n",this->paghs);
  thprintf("\tplayout->pagvs = %lg;\n",this->pagvs);
  thprintf("\tplayout->marts = %lg;\n",this->marts);
  thprintf("\tplayout->marls = %lg;\n",this->marls);

  thprintf("\tplayout->color_map_bg.defined = %d;\n", this->color_map_bg.defined);
  thprintf("\tplayout->color_map_bg.R = %lg;\n",this->color_map_bg.R);
  thprintf("\tplayout->color_map_bg.G = %lg;\n",this->color_map_bg.G);
  thprintf("\tplayout->color_map_bg.B = %lg;\n",this->color_map_bg.B);
  thprintf("\tplayout->color_map_bg.A = %lg;\n",this->color_map_bg.A);

  thprintf("\tplayout->color_map_fg.defined = %d;\n", this->color_map_fg.defined);
  thprintf("\tplayout->color_crit = %d;\n", this->color_crit);
  if (this->color_crit_fname != NULL)
    thprintf("\tplayout->color_crit_fname = \"%s\";\n", this->color_crit_fname);
  thprintf("\tplayout->color_map_fg.R = %lg;\n",this->color_map_fg.R);
  thprintf("\tplayout->color_map_fg.G = %lg;\n",this->color_map_fg.G);
  thprintf("\tplayout->color_map_fg.B = %lg;\n",this->color_map_fg.B);
  thprintf("\tplayout->color_map_fg.A = %lg;\n",this->color_map_fg.A);

  thprintf("\tplayout->color_preview_below.defined = %d;\n", this->color_preview_below.defined);
  thprintf("\tplayout->color_preview_below.R = %lg;\n",this->color_preview_below.R);
  thprintf("\tplayout->color_preview_below.G = %lg;\n",this->color_preview_below.G);
  thprintf("\tplayout->color_preview_below.B = %lg;\n",this->color_preview_below.B);
  thprintf("\tplayout->color_preview_below.A = %lg;\n",this->color_preview_below.A);

  thprintf("\tplayout->color_preview_above.defined = %d;\n", this->color_preview_above.defined);
  thprintf("\tplayout->color_preview_above.R = %lg;\n",this->color_preview_above.R);
  thprintf("\tplayout->color_preview_above.G = %lg;\n",this->color_preview_above.G);
  thprintf("\tplayout->color_preview_above.B = %lg;\n",this->color_preview_above.B);
  thprintf("\tplayout->color_preview_above.A = %lg;\n",this->color_preview_above.A);

  thprintf("\tplayout->def_overlap = %d;\n", this->def_overlap);
  thprintf("\tplayout->overlap = %lg;\n",this->overlap);

  thprintf("\tplayout->def_scale_bar = %d;\n", this->def_scale_bar);
  thprintf("\tplayout->scale_bar = %lg;\n",this->scale_bar);

  thprintf("\tplayout->def_transparency = %d;\n", this->def_transparency);
  thprintf("\tplayout->transparency = %s;\n",(this->transparency ? "true" : "false"));

  thprintf("\tplayout->def_sketches = %d;\n", this->def_sketches);
  thprintf("\tplayout->sketches = %s;\n",(this->sketches ? "true" : "false"));

  thprintf("\tplayout->def_legend = %d;\n", this->def_legend);
  thprintf("\tplayout->legend = %s;\n",(
    this->legend == TT_LAYOUT_LEGEND_OFF ? "TT_LAYOUT_LEGEND_OFF" : (
    this->legend == TT_LAYOUT_LEGEND_ON ? "TT_LAYOUT_LEGEND_ON" : "TT_LAYOUT_LEGEND_ALL"
    )));

  thprintf("\tplayout->def_survey_level = %d;\n", this->def_survey_level);
  thprintf("\tplayout->survey_level = %d;\n", this->survey_level);

  thprintf("\tplayout->def_color_legend = %d;\n", this->def_color_legend);
  thprintf("\tplayout->color_legend = %d;\n", this->color_legend);

  thprintf("\tplayout->def_color_model = %d;\n", this->def_color_model);
  thprintf("\tplayout->color_model = %d;\n", this->color_model);


  thprintf("\tplayout->def_legend_width = %d;\n", this->def_legend_width);
  thprintf("\tplayout->legend_width = %lg;\n",this->legend_width);

  thprintf("\tplayout->def_legend_columns = %d;\n", this->def_legend_columns);
  thprintf("\tplayout->legend_columns = %d;\n",this->legend_columns);

  thprintf("\tplayout->def_map_header = %d;\n", this->def_map_header);
  thprintf("\tplayout->map_header = %d;\n",this->map_header);
  thprintf("\tplayout->map_header_x = %lg;\n",this->map_header_x);
  thprintf("\tplayout->map_header_y = %lg;\n",this->map_header_y);

  thprintf("\tplayout->def_debug = %d;\n", this->def_debug);
  thprintf("\tplayout->debug = %d;\n",this->debug);

  thprintf("\tplayout->def_max_explos = %d;\n", this->def_max_explos);
  thprintf("\tplayout->max_explos = %d;\n",this->max_explos);
  thprintf("\tplayout->def_max_topos = %d;\n", this->def_max_topos);
  thprintf("\tplayout->max_topos = %d;\n",this->max_topos);
  thprintf("\tplayout->def_max_cartos = %d;\n", this->def_max_cartos);
  thprintf("\tplayout->max_cartos = %d;\n",this->max_cartos);
  thprintf("\tplayout->def_max_copys = %d;\n", this->def_max_copys);
  thprintf("\tplayout->max_copys = %d;\n",this->max_copys);

  thprintf("\tplayout->def_explo_lens = %d;\n",this->def_explo_lens);
  thprintf("\tplayout->explo_lens = %d;\n",this->explo_lens);
  thprintf("\tplayout->def_topo_lens = %d;\n",this->def_topo_lens);
  thprintf("\tplayout->topo_lens = %d;\n",this->topo_lens);
  thprintf("\tplayout->def_carto_lens = %d;\n",this->def_carto_lens);
  thprintf("\tplayout->carto_lens = %d;\n",this->carto_lens);
  thprintf("\tplayout->def_copy_lens = %d;\n",this->def_copy_lens);
  thprintf("\tplayout->copy_lens = %d;\n",this->copy_lens);

  thprintf("\tplayout->def_lang = %d;\n", this->def_lang);
  thprintf("\tplayout->lang = %s;\n",thlang_getcxxid(this->lang));

  thprintf("\tplayout->def_units = %d;\n", this->def_units);
  thprintf("\tplayout->units.units = %d;\n",this->units.units);

  thprintf("\tplayout->def_layers = %d;\n", this->def_layers);
  thprintf("\tplayout->layers = %s;\n",(this->layers ? "true" : "false"));

  thprintf("\tplayout->def_map_header_bg = %d;\n", this->def_map_header_bg);
  thprintf("\tplayout->map_header_bg = %s;\n",(this->map_header_bg ? "true" : "false"));

  thprintf("\tplayout->def_opacity = %d;\n", this->def_opacity);
  thprintf("\tplayout->opacity = %lg;\n",this->opacity);

  thprintf("\tplayout->def_surface_opacity = %d;\n", this->def_surface_opacity);
  thprintf("\tplayout->surface_opacity = %lg;\n", this->surface_opacity);

  thprintf("\tplayout->def_surface= %d;\n", this->def_surface);
  thprintf("\tplayout->surface = %d;\n", this->surface);

  thprintf("\tplayout->def_color_labels= %d;\n", this->def_color_labels);
  thprintf("\tplayout->color_labels = %s;\n", (this->color_labels ? "true" : "false"));

  thprintf("\tplayout->def_grid_coords = %d;\n", this->def_grid_coords);
  thprintf("\tplayout->grid_coords = %d;\n", this->grid_coords);

  thprintf("\tplayout->def_north= %d;\n", this->def_north);
  thprintf("\tplayout->north = %d;\n", this->north);

  thprintf("\tplayout->def_smooth_shading= %d;\n", this->def_smooth_shading);
  thprintf("\tplayout->smooth_shading = %d;\n", this->smooth_shading);

  thprintf("\tplayout->def_grid = %d;\n", this->def_grid);
  thprintf("\tplayout->grid = %d;\n",this->grid);

  thprintf("\tplayout->def_page_grid = %d;\n", this->def_page_grid);
  thprintf("\tplayout->page_grid = %s;\n",(this->page_grid ? "true" : "false"));


  thprintf("\tplayout->def_origin = %d;\n", this->def_origin);
  if (!thisnan(this->ox))
    thprintf("\tplayout->ox = %lg;\n",this->ox);
  if (!thisnan(this->oy))
    thprintf("\tplayout->oy = %lg;\n",this->oy);
  if (!thisnan(this->oz))
    thprintf("\tplayout->oz = %lg;\n",this->oz);

  thprintf("\tplayout->def_origin_label = %d;\n", this->def_origin_label);
  thdecode_c(&(this->db->buff_enc), this->olx);
  thprintf("\tplayout->olx = \"%s\";\n", this->db->buff_enc.get_buffer());
  thdecode_c(&(this->db->buff_enc), this->oly);
  thprintf("\tplayout->oly = \"%s\";\n", this->db->buff_enc.get_buffer());

  thprintf("\tplayout->def_doc_title = %d;\n", this->def_doc_title);
  thdecode_c(&(this->db->buff_enc), this->doc_title);
  thprintf("\tplayout->doc_title = \"%s\";\n", this->db->buff_enc.get_buffer());
  
  thprintf("\tplayout->def_doc_comment = %d;\n", this->def_doc_comment);
  thdecode_c(&(this->db->buff_enc), this->doc_comment);
  thprintf("\tplayout->doc_comment = \"%s\";\n", this->db->buff_enc.get_buffer());
  
  thprintf("\tplayout->def_doc_author = %d;\n", this->def_doc_author);
  thdecode_c(&(this->db->buff_enc), this->doc_author);
  thprintf("\tplayout->doc_author = \"%s\";\n", this->db->buff_enc.get_buffer());

  thprintf("\tplayout->def_doc_subject = %d;\n", this->def_doc_author);
  thdecode_c(&(this->db->buff_enc), this->doc_subject);
  thprintf("\tplayout->doc_subject = \"%s\";\n", this->db->buff_enc.get_buffer());
  
  thprintf("\tplayout->def_doc_keywords = %d;\n", this->def_doc_keywords);
  thdecode_c(&(this->db->buff_enc), this->doc_keywords);
  thprintf("\tplayout->doc_keywords = \"%s\";\n", this->db->buff_enc.get_buffer());
  
  thprintf("\tplayout->def_excl_pages = %d;\n", this->def_excl_pages);
  thprintf("\tplayout->excl_pages = %s;\n",(this->excl_pages ? "true" : "false"));
  if (this->excl_list == NULL) {
    thprintf("\tplayout->excl_list = NULL;\n");
  } else {
    thdecode_c(&(this->db->buff_enc), this->excl_list);
    thprintf("\tplayout->excl_list = \"%s\";\n", this->db->buff_enc.get_buffer());
  }

  thprintf("\tplayout->def_font_setup = %d;\n", this->def_font_setup);
  if (!thisnan(this->font_setup[0])) {
    thprintf("\tplayout->font_setup[0] = %lg;\n",this->font_setup[0]);
    thprintf("\tplayout->font_setup[1] = %lg;\n",this->font_setup[1]);
    thprintf("\tplayout->font_setup[2] = %lg;\n",this->font_setup[2]);
    thprintf("\tplayout->font_setup[3] = %lg;\n",this->font_setup[3]);
    thprintf("\tplayout->font_setup[4] = %lg;\n",this->font_setup[4]);
  }

  thprintf("\tplayout->def_min_symbol_scale = %d;\n", this->def_min_symbol_scale);
  if (this->min_symbol_scale > 0.0) {
    thprintf("\tplayout->min_symbol_scale = %lg;\n",this->min_symbol_scale);
  }

  
  thprintf("\tplayout->def_grid_size = %d;\n", this->def_grid_size);
  if (!thisnan(this->gxs)) {
    thprintf("\tplayout->gxs = %lg;\n",this->gxs);
    thprintf("\tplayout->gys = %lg;\n",this->gys);
    thprintf("\tplayout->gzs = %lg;\n",this->gzs);
  }

  thprintf("\tplayout->def_grid_origin = %d;\n", this->def_grid_origin);
  if (!thisnan(this->gox))
    thprintf("\tplayout->gox = %lg;\n",this->gox);
  if (!thisnan(this->goy))
    thprintf("\tplayout->goy = %lg;\n",this->goy);
  if (!thisnan(this->goz))
    thprintf("\tplayout->goz = %lg;\n",this->goz);

  thprintf("\tplayout->def_nav_factor = %d;\n", this->def_nav_factor);
  thprintf("\tplayout->navf = %lg;\n",this->navf);

  thprintf("\tplayout->def_nav_size = %d;\n", this->def_nav_size);
  thprintf("\tplayout->navsx = %d;\n",this->navsx);
  thprintf("\tplayout->navsy = %d;\n",this->navsy);
  
  thprintf("\tplayout->def_own_pages = %d;\n", this->def_own_pages);
  thprintf("\tplayout->ownp = %d;\n",this->ownp);

  thprintf("\tplayout->def_title_pages = %d;\n", this->def_title_pages);
  thprintf("\tplayout->titlep = %s;\n",(this->titlep ? "true" : "false"));

  thprintf("\tplayout->def_page_numbers = %d;\n", this->def_page_numbers);
  thprintf("\tplayout->pgsnum = %s;\n",(this->pgsnum ? "true" : "false"));

  
  char last_code = TT_LAYOUT_CODE_UNKNOWN;
  for (auto ln = lines.begin(); ln != lines.end(); ++ln) {
    switch (ln->code) {
      case TT_LAYOUT_CODE_METAPOST:
      case TT_LAYOUT_CODE_TEX_MAP:
      case TT_LAYOUT_CODE_TEX_ATLAS:
        thdecode_c(&(this->db->buff_enc), ln->line);
        if (ln->code != last_code) {
          thprintf("\tplayout->ccode = ");
          switch (ln->code) {
            case TT_LAYOUT_CODE_METAPOST:
              thprintf("TT_LAYOUT_CODE_METAPOST");
              break;
            case TT_LAYOUT_CODE_TEX_MAP:
              thprintf("TT_LAYOUT_CODE_TEX_MAP");
              break;
            default:
              thprintf("TT_LAYOUT_CODE_TEX_ATLAS");
              break;
          }
          thprintf(";\n");
        }
        thprintf("\toname = \"%s\";\n", this->db->buff_enc.get_buffer());
        thprintf("\tplayout->set(thcmd_option_desc(0,1),oname,TT_UTF_8,0);\n");
        break;
      case TT_LAYOUT_CODE_SYMBOL_ASSIGN:
      case TT_LAYOUT_CODE_MAP_ITEM:
      case TT_LAYOUT_CODE_SYMBOL_DEFAULTS:
      case TT_LAYOUT_CODE_SYMBOL_HIDE:
      case TT_LAYOUT_CODE_SYMBOL_SHOW:
      case TT_LAYOUT_CODE_SYMBOL_COLOR:
        if (ln->line != NULL) {
          thdecode_c(&(this->db->buff_enc), ln->line);
          thprintf("\toname = \"%s\";\n", this->db->buff_enc.get_buffer());
        }
        if (ln->code != TT_LAYOUT_CODE_SYMBOL_DEFAULTS)
          thprintf("\tplayout->set(thcmd_option_desc(TT_LAYOUT_SYMBOL_DEFAULTS,0),NULL,TT_UTF_8,0);\n");
        else
          thprintf("\tplayout->set(thcmd_option_desc(TT_LAYOUT_SYMBOL_DEFAULTS,1),oname,TT_UTF_8,0);\n");
        if (ln->code != TT_LAYOUT_CODE_SYMBOL_DEFAULTS) {
          switch (ln->code) {
            case TT_LAYOUT_CODE_SYMBOL_HIDE:
              thprintf("\tplayout->get_last_line()->code = TT_LAYOUT_CODE_SYMBOL_HIDE;\n");
              thprintf("\tplayout->get_last_line()->smid = %s;\n", thsymbolset_src[ln->smid]);
              break;
            case TT_LAYOUT_CODE_SYMBOL_SHOW:
              thprintf("\tplayout->get_last_line()->code = TT_LAYOUT_CODE_SYMBOL_SHOW;\n");
              thprintf("\tplayout->get_last_line()->smid = %s;\n", thsymbolset_src[ln->smid]);
              break;
            case TT_LAYOUT_CODE_MAP_ITEM:
              thprintf("\tplayout->get_last_line()->code = TT_LAYOUT_CODE_MAP_ITEM;\n");
              thprintf("\tplayout->get_last_line()->smid = %s;\n", thsymbolset_src[ln->smid]);
              break;
            case TT_LAYOUT_CODE_SYMBOL_ASSIGN:
              thprintf("\tplayout->get_last_line()->code = TT_LAYOUT_CODE_SYMBOL_ASSIGN;\n");
              thprintf("\tplayout->get_last_line()->smid = %s;\n", thsymbolset_src[ln->smid]);
              break;
            case TT_LAYOUT_CODE_SYMBOL_COLOR:
              thprintf("\tplayout->get_last_line()->code = TT_LAYOUT_CODE_SYMBOL_COLOR;\n");
              thprintf("\tplayout->get_last_line()->smid = %s;\n", thsymbolset_src[ln->smid]);
              thprintf("\tplayout->get_last_line()->sclr = thlayout_color(%.6f,%.6f,%.6f);\n", ln->sclr.R, ln->sclr.G, ln->sclr.B);
              break;
          }
        }
        break;
    }
  }
  thprintf("\tplayout->def_tex_lines = %d;\n", this->def_tex_lines);
  
}


void check_num(double num, int nonneg) {
  if (nonneg == 0) {
    if (num < 0) {
      ththrow("not a non-negative number -- {}", num);
    }
  }
  else if (nonneg > 0) {
    if (num <= 0) {
      ththrow("not a positive number -- {}", num);
    }
  }
}


void thlayout::parse_len(double & d1, double & d2, double & d3, int nargs, char ** args, int nonneg) {
  int sv;
  thtflength lentf;
  lentf.parse_units(args[nargs]);
  switch (nargs) {
    case 3:
      thparse_double(sv,d3,args[2]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[2]);
      d3 = lentf.transform(d3);
      check_num(d3,nonneg);
      [[fallthrough]];
    case 2:
      thparse_double(sv,d2,args[1]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[1]);
      d2 = lentf.transform(d2);
      check_num(d2,nonneg);
      [[fallthrough]];
    case 1:
      thparse_double(sv,d1,args[0]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[0]);
      d1 = lentf.transform(d1);
      check_num(d1,nonneg);
  }
}


void thlayout::parse_len6(double & d1, double & d2, double & d3, double & d4, double & d5, double & d6, int nargs, char ** args, int nonneg) {
  int sv;
  thtflength lentf;
  lentf.parse_units(args[nargs]);
  switch (nargs) {
    case 6:
      thparse_double(sv,d6,args[5]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[5]);
      d6 = lentf.transform(d6);
      check_num(d6,nonneg);
      [[fallthrough]];
    case 5:
      thparse_double(sv,d5,args[4]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[4]);
      d5 = lentf.transform(d5);
      check_num(d5,nonneg);
      [[fallthrough]];
    case 4:
      thparse_double(sv,d4,args[3]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[3]);
      d4 = lentf.transform(d4);
      check_num(d4,nonneg);
      [[fallthrough]];
    case 3:
      thparse_double(sv,d3,args[2]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[2]);
      d3 = lentf.transform(d3);
      check_num(d3,nonneg);
      [[fallthrough]];
    case 2:
      thparse_double(sv,d2,args[1]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[1]);
      d2 = lentf.transform(d2);
      check_num(d2,nonneg);
      [[fallthrough]];
    case 1:
      thparse_double(sv,d1,args[0]);
      if ((sv != TT_SV_NUMBER))
        ththrow("invalid number -- {}", args[0]);
      d1 = lentf.transform(d1);
      check_num(d1,nonneg);
  }
}

#define THM2PT 2834.64566929

void thlayout::export_config(FILE * o, thdb2dprj * prj, double x_scale, double x_origin_shx, double x_origin_shy) {
  double pgox, pgoy;
  double ghs, gvs;

  fprintf(o,"Configuration file\n");
  fprintf(o,"hsize: %.2fmm\nvsize: %.2fmm\n",this->hsize * 1000.0,this->vsize * 1000.0);
  fprintf(o,"Overlap: %.2fmm\n",this->overlap * 1000.0);
  //fprintf(o,"Hoffset: %.2fmm\nVoffset: %.2fmm\n",this->hsize * 500.0,this->vsize * 500.0);

  switch (prj->type) {
    case TT_2DPROJ_EXTEND:
    case TT_2DPROJ_ELEV:
      pgox = (this->gox - prj->shift_x) * x_scale + x_origin_shx;
      pgoy = (this->goz - prj->shift_z) * x_scale + x_origin_shy;
      ghs = this->gxs;
      gvs = this->gzs;
      break;
    default:
      pgox = (this->gox - prj->shift_x) * x_scale + x_origin_shx;
      pgoy = (this->goy - prj->shift_y) * x_scale + x_origin_shy;
      ghs = this->gxs;
      gvs = this->gys;
      break;
  }
  fprintf(o,"HGrid: %.2fmm\nVGrid: %.2fmm\n",ghs * this->scale * 1000.0, gvs * this->scale * 1000.0);
  pgox = pgox - (THM2PT * ghs * (double)(long)(pgox / ghs / THM2PT));
  pgoy = pgoy - (THM2PT * gvs * (double)(long)(pgoy / gvs / THM2PT));
  fprintf(o,"HGridOffset: %.2fmm\nVGridOffset: %.2fmm\n",pgox / THM2PT * 1000.0,pgoy / THM2PT * 1000.0);
  
  fprintf(o,"Background: 0\n");
  fprintf(o,"StartX: %s\nStartY: %s\nStartZ: 0\n", this->olx, this->oly);
  fprintf(o,"NavRight: %d\nNavUp: %d\n", this->navsx, this->navsy);
  fprintf(o,"NavFactor: %.2f\n", this->navf);
  fprintf(o,"OwnPages: %d\n", this->ownp);
  fprintf(o,"TitlePages: %d\n", (this->titlep ? 1 : 0));
  fprintf(o,"PageNumbering: %d\n", (this->pgsnum ? 1 : 0));
  fprintf(o,"Transparency: %d\n", (this->transparency ? 1 : 0));
  fprintf(o,"ExclPages: %d\n", (this->excl_pages ? 1 : 0));
  if (this->excl_pages) {
    fprintf(o,"ExclList: %s\n", this->excl_list);
  }  
  fprintf(o,"MapGrid: %d\n", (this->page_grid ? 1 : 0));
}



void thlayout_print_header_align(FILE * o, int a) {
    switch (a) {
      case TT_LAYOUT_MAP_HEADER_CENTER:
        fprintf(o,"C");
        break;
      case TT_LAYOUT_MAP_HEADER_N:
        fprintf(o,"N");
        break;
      case TT_LAYOUT_MAP_HEADER_NE:
        fprintf(o,"NE");
        break;
      case TT_LAYOUT_MAP_HEADER_E:
        fprintf(o,"E");
        break;
      case TT_LAYOUT_MAP_HEADER_W:
        fprintf(o,"W");
        break;
      case TT_LAYOUT_MAP_HEADER_S:
        fprintf(o,"S");
        break;
      case TT_LAYOUT_MAP_HEADER_SW:
        fprintf(o,"SW");
        break;
      case TT_LAYOUT_MAP_HEADER_SE:
        fprintf(o,"SE");
        break;
      default:
        fprintf(o,"NW");
    }
}

std::string fix_path_slashes(std::string s) {
	std::replace( s.begin(), s.end(), '\\', '/');
	return s;
}

  
void thlayout::export_pdftex(FILE * o, thdb2dprj * /*prj*/, char mode) { // TODO unused parameter prj

  fprintf(o,"\\opacity{%.2f}\n",this->opacity);
  fprintf(o,"\\def\\scale{%lu}\n",(unsigned long)(1.0 / this->scale + 0.5));
  fprintf(o,"\\pagesetup{%.4fcm}{%.4fcm}{%.4fcm}{%.4fcm}{%.4fcm}{%.4fcm}\n",
    this->paphs*100.0, this->papvs*100.0, 
    this->paghs*100.0, this->pagvs*100.0, 
    this->marls*100.0, this->marts*100.0);
  fprintf(o,"\\def\\maplayout{");
  thlayout_map_image_list::iterator mit;
  size_t nami = 0;
  for(mit = this->map_image_list.begin(); mit != this->map_image_list.end(); mit++) {
    if (mit->defined() && (mit->m_align != TT_LAYOUT_MAP_HEADER_OFF))
      nami++;
  }

  std::error_code ec;
  const auto cwd = fs::current_path(ec);
  thassert(!ec);

  if (this->map_header != TT_LAYOUT_MAP_HEADER_OFF) {
    fprintf(o,"\\legendbox{%.0f}{%.0f}{", this->map_header_x, this->map_header_y);
    thlayout_print_header_align(o, this->map_header);
    fprintf(o,"}");
    fprintf(o,"{\\the\\legendcontent}");
  } 
  if (nami > 0) {
    for(mit = this->map_image_list.begin(); mit != this->map_image_list.end(); mit++) {
      if (mit->defined() && (mit->m_align != TT_LAYOUT_MAP_HEADER_OFF)) {
        auto pict_path = (cwd / mit->m_fn).string();
        std::replace(pict_path.begin(), pict_path.end(), '\\', '/');
        fprintf(o,"\\legendbox{%.0f}{%.0f}{", mit->m_x, mit->m_y);
        thlayout_print_header_align(o, mit->m_align);
        fprintf(o,"}{\\loadpicture{%s}}", pict_path.c_str());
      }
    }
  }
  fprintf(o,"}\n");

  bool anyline = false;
  bool anylegend = false;
  const char * last_path = "";
  for (auto ln = lines.begin(); ln != lines.end(); ++ln) {
      if (ln->code == mode) {
        // ak najde \\formattedlegend v \\insertmaps tak anyline bude
        // true
        if ((!anyline) && (strstr(ln->line, "\\insertmaps") != NULL))
            anyline = true;
        if ((!anylegend) && (strstr(ln->line, "\\formattedlegend") != NULL))
            anylegend = true;
        if (strcmp(ln->path, last_path) != 0) {
        	last_path = ln->path;
        	fprintf(o, "\\includeprefix={%s}\n", fix_path_slashes(last_path).c_str());
        }
        thdecode_utf2tex(&(this->db->buff_enc), ln->line);
        fprintf(o, "%s\n", this->db->buff_enc.get_buffer());
      }
  }
  
  if (!anyline) {
    fprintf(o,"\\insertmaps\n");
    if (mode == TT_LAYOUT_CODE_TEX_ATLAS) {
      if ((!anylegend) && (this->legend != TT_LAYOUT_LEGEND_OFF)) {
        fprintf(o,"\\formattedlegend\n");
      }
    }
  }
}


void thlayout::export_mpost(FILE * o) {

  bool anyline = false;
  const char * last_path = "";
  for (auto ln = lines.begin(); ln != lines.end(); ++ln) {
      if (ln->code == TT_LAYOUT_CODE_METAPOST) {
        if (strcmp(ln->path, last_path) != 0) {
          last_path = ln->path;
          fprintf(o, "includeprefix := \"%s\";\n", fix_path_slashes(last_path).c_str());
        }
        anyline = true;
        thdecode(&(this->db->buff_enc), TT_ISO8859_2, ln->line);
        fprintf(o, "%s\n", this->db->buff_enc.get_buffer());
      }
  }
  
  if (!anyline) {
  }

}



void thlayout::export_mpost_symbols(FILE * o, thsymbolset * symset) {
  for (auto ln = lines.begin(); ln != lines.end(); ++ln) {
    switch (ln->code) {
      case TT_LAYOUT_CODE_SYMBOL_DEFAULTS:
        symset->export_symbol_defaults(o,ln->line);
        break;
      case TT_LAYOUT_CODE_SYMBOL_ASSIGN:
        symset->export_symbol_assign(o,ln->smid,ln->line);
        break;
      case TT_LAYOUT_CODE_SYMBOL_HIDE:
        symset->export_symbol_hide(o,ln->smid);
        break;
      case TT_LAYOUT_CODE_SYMBOL_SHOW:
        symset->export_symbol_show(o,ln->smid);
        break;
      case TT_LAYOUT_CODE_SYMBOL_COLOR:
        symset->export_symbol_color(o,ln->smid,&(ln->sclr));
        break;
      case TT_LAYOUT_CODE_MAP_ITEM:
        symset->export_symbol_assign(o,ln->smid,ln->line);
        break;
    }
  }
}


static bool is_copy_src(thlayoutln const &ll) {
  return ll.code == TT_LAYOUT_CODE_COPY;
}


void thlayout::process_copy() {
  // ak je locknuty -> tak warning a koniec
  if (this->lock) {
    thwarning(("%s -- recursive layout copying", this->throw_source().c_str()))
    return;
  }
  this->lock = true;

  for (auto csp = lines.begin(); csp != lines.end(); ++csp) {
    if (!is_copy_src(*csp)) {
      continue;
    }

    // najdeme si layout podla mena
    thlayout * srcl = this->db->get_layout(csp->line);
    if (srcl == NULL) {
      thwarning(("%s -- source layout not found -- %s", this->throw_source().c_str(), csp->line))
    } else {
      // ak ma este nevyriesene zavislosti
      srcl->process_copy();
      
      // teraz skopirujme co nemame a on ma
#define begcopy(whatx) if ((this->whatx < 2) && (srcl->whatx > 0)) { \
        this->whatx = 1;
#define endcopy }
      
      begcopy(def_scale)
        this->scale = srcl->scale;
      endcopy
        
      begcopy(def_rotate)
        this->rotate = srcl->rotate;
      endcopy
        
      begcopy(def_base_scale)
        this->base_scale = srcl->base_scale;
      endcopy

      begcopy(def_origin)
        this->o_cs = srcl->o_cs;
        this->ox = srcl->ox;
        this->oy = srcl->oy;
        this->oz = srcl->oz;
      endcopy

      begcopy(def_size)
        this->hsize = srcl->hsize;
        this->vsize = srcl->vsize;
      endcopy
  
      begcopy(def_page_setup)
        this->paphs = srcl->paphs;
        this->papvs = srcl->papvs;
        this->paghs = srcl->paghs;
        this->pagvs = srcl->pagvs;
        this->marls = srcl->marls;
        this->marts = srcl->marts;
      endcopy

      begcopy(def_overlap)
        this->overlap = srcl->overlap;
      endcopy

      begcopy(def_scale_bar)
        this->scale_bar = srcl->scale_bar;
      endcopy
  
      begcopy(def_grid_origin)
        this->go_cs = srcl->go_cs;
        this->gox = srcl->gox;
        this->goy = srcl->goy;
        this->goz = srcl->goz;
      endcopy

      begcopy(def_excl_pages)
        this->excl_pages = srcl->excl_pages;
        this->excl_list = srcl->excl_list;
      endcopy
      
      begcopy(color_map_fg.defined)
        this->color_crit = srcl->color_crit;
        this->color_crit_fname = srcl->color_crit_fname;
        this->color_map_fg.copy_color(srcl->color_map_fg);
      endcopy

      begcopy(color_preview_below.defined)
        this->color_preview_below.copy_color(srcl->color_preview_below);
      endcopy

      begcopy(color_preview_above.defined)
        this->color_preview_above.copy_color(srcl->color_preview_above);
      endcopy

      begcopy(color_map_bg.defined)
        this->color_map_bg.copy_color(srcl->color_map_bg);
      endcopy

      begcopy(def_doc_title)
        this->doc_title = srcl->doc_title;
      endcopy

	    if ((this->def_doc_comment > 0) && (strlen(this->doc_comment) > 0) && (srcl->def_doc_comment > 0)) {
		    std::string tmpstr(this->doc_comment);
		    tmpstr += "<br>";
		    tmpstr += srcl->doc_comment;
		    this->doc_comment = this->db->strstore(tmpstr.c_str());
	    } else {
		    begcopy(def_doc_comment)
		    this->doc_comment = srcl->doc_comment;
		    endcopy
	    }

      begcopy(def_doc_author)
        this->doc_author = srcl->doc_author;
      endcopy

      begcopy(def_doc_subject)
        this->doc_subject = srcl->doc_subject;
      endcopy

      begcopy(def_doc_keywords)
        this->doc_keywords = srcl->doc_keywords;
      endcopy

      begcopy(def_grid_size)
        this->gxs = srcl->gxs;
        this->gys = srcl->gys;
        this->gzs = srcl->gzs;
      endcopy

      begcopy(def_font_setup)
        this->font_setup[0] = srcl->font_setup[0];
        this->font_setup[1] = srcl->font_setup[1];
        this->font_setup[2] = srcl->font_setup[2];
        this->font_setup[3] = srcl->font_setup[3];
        this->font_setup[4] = srcl->font_setup[4];
      endcopy

      begcopy(def_min_symbol_scale)
        this->min_symbol_scale = srcl->min_symbol_scale;
      endcopy
  
      begcopy(def_origin_label)
        this->olx = srcl->olx;
        this->oly = srcl->oly;
      endcopy
  
      begcopy(def_nav_factor)
        this->navf = srcl->navf;
      endcopy
  
      begcopy(def_nav_size)
        this->navsx = srcl->navsx;
        this->navsy = srcl->navsy;
      endcopy

      begcopy(def_own_pages)
        this->ownp = srcl->ownp;
      endcopy
      
      begcopy(def_title_pages)
        this->titlep = srcl->titlep;
      endcopy
        
      begcopy(def_opacity)
        this->opacity = srcl->opacity;
      endcopy

      begcopy(def_surface)
        this->surface = srcl->surface;
      endcopy

      begcopy(def_color_labels)
        this->color_labels = srcl->color_labels;
      endcopy

      begcopy(def_grid_coords)
        this->grid_coords = srcl->grid_coords;
      endcopy

      begcopy(def_north)
        this->north = srcl->north;
      endcopy
	  
      begcopy(def_smooth_shading)
        this->smooth_shading = srcl->smooth_shading;
      endcopy

      begcopy(def_surface_opacity)
        this->surface_opacity = srcl->surface_opacity;
      endcopy
  
      begcopy(def_transparency)
        this->transparency = srcl->transparency;
      endcopy

      begcopy(def_sketches)
        this->sketches = srcl->sketches;
      endcopy

      begcopy(def_legend)
        this->legend = srcl->legend;
      endcopy

      begcopy(def_survey_level)
        this->survey_level = srcl->survey_level;
      endcopy

      begcopy(def_color_legend)
        this->color_legend = srcl->color_legend;
      endcopy

      begcopy(def_color_model)
        this->color_model = srcl->color_model;
      endcopy
	  
      begcopy(def_color_profile_rgb)
        this->color_profile_rgb = srcl->color_profile_rgb;
      endcopy
	  
      begcopy(def_color_profile_cmyk)
        this->color_profile_cmyk = srcl->color_profile_cmyk;
      endcopy

	  begcopy(def_color_profile_gray)
        this->color_profile_gray = srcl->color_profile_gray;
      endcopy

      begcopy(def_legend_width)
        this->legend_width = srcl->legend_width;
      endcopy

      begcopy(def_legend_columns)
        this->legend_columns = srcl->legend_columns;
      endcopy

      begcopy(def_map_header)
        this->map_header = srcl->map_header;
        this->map_header_x = srcl->map_header_x;
        this->map_header_y = srcl->map_header_y;
      endcopy

      begcopy(def_debug)
        this->debug = srcl->debug;
      endcopy

      begcopy(def_max_explos)
        this->max_explos = srcl->max_explos;
      endcopy

      begcopy(def_max_topos)
        this->max_topos = srcl->max_topos;
      endcopy

      begcopy(def_max_cartos)
        this->max_cartos = srcl->max_cartos;
      endcopy

      begcopy(def_max_copys)
        this->max_copys = srcl->max_copys;
      endcopy

      begcopy(def_explo_lens)
        this->explo_lens = srcl->explo_lens;
      endcopy

      begcopy(def_topo_lens)
        this->topo_lens = srcl->topo_lens;
      endcopy

	  begcopy(def_carto_lens)
        this->carto_lens = srcl->carto_lens;
      endcopy

      begcopy(def_copy_lens)
        this->copy_lens = srcl->copy_lens;
      endcopy

      begcopy(def_lang)
        this->lang = srcl->lang;
      endcopy

      begcopy(def_units)
        this->units = srcl->units;
      endcopy
  
      begcopy(def_layers)
        this->layers = srcl->layers;
      endcopy

      begcopy(def_map_header_bg)
        this->map_header_bg = srcl->map_header_bg;
      endcopy
  
      begcopy(def_grid)
        this->grid = srcl->grid;
      endcopy

      begcopy(def_page_grid)
        this->page_grid = srcl->page_grid;
      endcopy
  
      begcopy(def_page_numbers)
        this->pgsnum = srcl->pgsnum;
      endcopy

      assert(std::none_of(srcl->lines.begin(), srcl->lines.end(), is_copy_src));
      this->lines.insert(csp, srcl->lines.begin(), srcl->lines.end());

      // copy map images
      thlayout_map_image_list::iterator mit, mits;
      mits = this->map_image_list.begin();
      for (mit = srcl->map_image_list.begin(); mit != srcl->map_image_list.end(); mit++) {
        this->map_image_list.insert(mits, *mit);
      }
      
    } 
  }

  // now remove the processed copy lines
  lines.remove_if(is_copy_src);

  this->lock = false;
}


void thlayout::set_thpdf_layout(thdb2dprj * /*prj*/, double /*x_scale*/, double /*x_origin_shx*/, double /*x_origin_shy*/) { // TODO unused parameters

  //string excl_list,labelx,labely;
  //bool  excl_pages,background,title_pages,page_numbering,
  //      transparency,map_grid; 
  //float hsize,vsize,overlap,
  //     hgrid,vgrid,hgridoffset,vgridoffset,
  //nav_factor;
  //int nav_right,nav_up,own_pages;

  LAYOUT.excl_list = (this->excl_list != NULL ? this->excl_list : "");
  LAYOUT.labelx = this->olx;
  LAYOUT.labely = this->oly;
  LAYOUT.excl_pages = this->excl_pages;
  LAYOUT.title_pages = this->titlep;
  LAYOUT.page_numbering = this->pgsnum;
  LAYOUT.transparency = this->transparency;
  LAYOUT.OCG = this->layers;
  LAYOUT.map_header_bg = this->map_header_bg;
  LAYOUT.transparent_map_bg = (this->color_map_bg.A < 1.0);
  //TODO
  LAYOUT.map_grid = this->page_grid;
  LAYOUT.hsize = this->hsize * THM2PT;
  LAYOUT.vsize = this->vsize * THM2PT;
  LAYOUT.legend_width = this->legend_width * THM2PT;
  LAYOUT.legend_columns = (int) this->legend_columns;
  LAYOUT.overlap = this->overlap * THM2PT;
  
  switch (this->grid_coords) {
    case TT_LAYOUT_GRIDCOORDS_OFF:
      LAYOUT.grid_coord_freq = 0;
      break;
    case TT_LAYOUT_GRIDCOORDS_ALL:
      LAYOUT.grid_coord_freq = 2;
      break;
    default:
      LAYOUT.grid_coord_freq = 1;
      break;
  }

  switch (this->surface) {
    case TT_LAYOUT_SURFACE_OFF:
      LAYOUT.surface = 0;
      break;
    case TT_LAYOUT_SURFACE_TOP:
      LAYOUT.surface = 2;
      break;
    default:
      LAYOUT.surface = 1;
      break;
  }

  // TODO: sketches into layer
  if (this->sketches) {
    LAYOUT.surface = 1;
  }

  LAYOUT.surface_opacity = this->surface_opacity;
  LAYOUT.nav_factor = this->navf;
  LAYOUT.nav_right = this->navsx;
  LAYOUT.nav_up = this->navsy;
  LAYOUT.own_pages = this->ownp;
  
  if (strlen(this->doc_title) > 0)
    LAYOUT.doc_title = this->doc_title;  
  if (strlen(this->doc_comment) > 0)
    LAYOUT.doc_comment = this->doc_comment;
  if (strlen(this->doc_author) > 0)
    LAYOUT.doc_author = this->doc_author;
  if (strlen(this->doc_subject) > 0)
    LAYOUT.doc_subject = this->doc_subject;
  if (strlen(this->doc_keywords) > 0)
    LAYOUT.doc_keywords = this->doc_keywords;
  LAYOUT.opacity = this->opacity;
  
  LAYOUT.colored_text = this->color_labels;
  switch (this->color_model) {
  case TT_LAYOUTCLRMODEL_GRAY:
	  LAYOUT.output_colormodel = colormodel::grey;
	  break;
  case TT_LAYOUTCLRMODEL_RGB:
	  LAYOUT.output_colormodel = colormodel::rgb;
	  break;
  default:
	  LAYOUT.output_colormodel = colormodel::cmyk;
	  break;
  }
  
  // color profiles abs paths
  LAYOUT.icc_profile_rgb = this->color_profile_rgb;
  LAYOUT.icc_profile_cmyk = this->color_profile_cmyk;
  LAYOUT.icc_profile_gray = this->color_profile_gray;
  
  this->color_map_bg.set_color(this->color_model, LAYOUT.col_background);
  this->color_map_fg.set_color(this->color_model, LAYOUT.col_foreground);
  this->color_preview_above.set_color(this->color_model, LAYOUT.col_preview_above);
  this->color_preview_below.set_color(this->color_model, LAYOUT.col_preview_below);

  //LAYOUT.col_background.set(this->color_map_bg.R, this->color_map_bg.G, this->color_map_bg.B);

  //LAYOUT.col_foreground.set(this->color_map_fg.R, this->color_map_fg.G, this->color_map_fg.B);

  //LAYOUT.col_preview_above.set(this->color_preview_above.R, this->color_preview_above.G, this->color_preview_above.B);

  //LAYOUT.col_preview_below.set(this->color_preview_below.R, this->color_preview_below.G, this->color_preview_below.B);
  
  LAYOUT.lang = this->lang;
  LAYOUT.langstr = thlang_getid(this->lang);
}


bool thlayout::is_debug_stations() {
  return ((this->debug & TTLDBG_STATIONS) != 0);
}

bool thlayout::is_debug_joins() {
  return ((this->debug & TTLDBG_JOINS) != 0);
}

bool thlayout::is_debug_scrapnames() {
  return ((this->debug & TTLDBG_SCRAPNAMES) != 0);
}

bool thlayout::is_debug_stationnames() {
  return ((this->debug & TTLDBG_STATIONNAMES) != 0);
}

bool thlayout_map_image::defined() {
  return (strlen(this->m_fn) > 0);
}


void thlayout_map_image::parse(char ** args, const char * cpath) {

  int sv;

  thparse_double(sv,this->m_x,args[0]);
  if (sv != TT_SV_NUMBER)
    ththrow("invalid number -- {}",args[0]);
  if ((this->m_x < -100.0) || (this->m_x > 200.0))
    ththrow("number between -100.0 - 200.0 expected -- {}",args[0]);

  thparse_double(sv,this->m_y,args[1]);
  if (sv != TT_SV_NUMBER)
    ththrow("invalid number -- {}",args[1]);
  if ((this->m_y < -100.0) || (this->m_y > 200.0))
    ththrow("number between -100.0 - 200.0 expected -- {}",args[1]);

  sv = thmatch_token(args[2],thtt_layout_map_header);
  if (sv == TT_LAYOUT_MAP_HEADER_UNKNOWN)
    ththrow("invalid map-image align switch -- {}",args[2]);
  this->m_align = sv;

  if (strlen(args[3]) == 0)
    ththrow("empty image file name not allowed");

  std::string fpath;
  fpath = cpath;
  if (thpath_is_absolute(args[3]))
    fpath = args[3];
  else
    fpath += args[3];
  this->m_fn = thdb.strstore(fpath.c_str());

}


void thlayout::export_mptex_font_size(FILE * o, th2ddataobject * obj, bool print_default_scale) {
  switch (obj->scale) {
    case TT_2DOBJ_SCALE_XL:
      fprintf(o,"\\thhugesize ");
      break;
    case TT_2DOBJ_SCALE_L:
      fprintf(o,"\\thlargesize ");
      break;
    case TT_2DOBJ_SCALE_S:
      fprintf(o,"\\thsmallsize ");
      break;
    case TT_2DOBJ_SCALE_XS:
      fprintf(o,"\\thtinysize ");
      break;
    case TT_2DOBJ_SCALE_NUMERIC:
    	{
    		double optical_zoom = this->scale / this->base_scale;
    		double font_size = 0;
    		if (obj->scale_numeric <= 0.5)
    			font_size = optical_zoom * obj->scale_numeric / 0.5 * this->font_setup[0];
                else if (obj->scale_numeric <= 0.707)
    			font_size = optical_zoom * obj->scale_numeric / 0.707 * this->font_setup[1];
                else if (obj->scale_numeric >= 2.0)
    			font_size = optical_zoom * obj->scale_numeric / 2.0 * this->font_setup[4];
                else if (obj->scale_numeric >= 1.414)
    			font_size = optical_zoom * obj->scale_numeric / 1.414 * this->font_setup[3];
                else
    			font_size = optical_zoom * obj->scale_numeric * this->font_setup[2];
    		fprintf(o,"\\size[%.1f]\\basefontsize=%.0f\\relax ", font_size, font_size);
    	}
    	break;
    default:
    	if (print_default_scale) fprintf(o,"\\thnormalsize ");
    	break;
  }

}

void thlayout::convert_all_cs() {
	if (!thisnan(this->ox))
		this->convert_cs(this->o_cs, this->ox, this->oy, this->ox, this->oy);
	if (!thisnan(this->gox))
		this->convert_cs(this->go_cs, this->gox, this->goy, this->gox, this->goy);
}


