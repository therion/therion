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
#include "thexception.h"
#include "thchenc.h"
#include "thdata.h"
#include "thparse.h"
#include "thinfnan.h"
#include "thpdfdata.h"

thlayout::thlayout()
{

  this->ccode = TT_LAYOUT_CODE_TEX_ATLAS;

  this->def_scale = false;
  this->scale = 0.005;
  
  this->def_origin = false;
  this->ox = thnan;
  this->oy = thnan;
  this->oz = thnan;

  this->def_size = false;
  this->hsize = 0.18;
  this->vsize = 0.222;
  
  this->def_page_setup = false;
  this->paphs = 0.21;
  this->papvs = 0.297;
  this->paghs = 0.20;
  this->pagvs = 0.287;
  this->marls = 0.005;
  this->marts = 0.005;
  

  this->def_overlap = false;
  this->overlap = 0.01;
  
  this->def_grid_origin = false;
  this->gox = thnan;
  this->goy = thnan;
  this->goz = thnan;
  
  this->def_grid_size = false;
  this->gxs = 10.0;
  this->gys = 10.0;
  
  this->def_origin_label = false;
  this->olx = "0";
  this->oly = "0";
  
  this->def_nav_factor = false;
  this->navf = 30.0;
  
  this->def_nav_size = false;
  this->navsx = 2;
  this->navsy = 2;
  
  this->def_own_pages = false;
  this->ownp = 0;
  
  this->def_title_pages = false;
  this->titlep = false;
  
  this->def_doc_title = false;
  this->doc_title = NULL;
  
  this->def_doc_author = false;
  this->doc_author = NULL;
  
  this->def_doc_keywords = false;
  this->doc_keywords = NULL;
  
  this->def_doc_subject = false;
  this->doc_subject = NULL;
  
  this->def_excl_pages = false;
  this->excl_pages = false;
  this->excl_list = NULL;
  
  this->def_opacity = false;
  this->opacity = 0.7;

  this->def_transparency = false;
  this->transparency = true;

  this->def_layers = false;
  this->layers = true;

  this->def_grid = false;
  this->grid = TT_LAYOUT_GRID_LINE;
  
  this->def_page_grid = false;
  this->page_grid = false;
  
  this->def_page_numbers = false;
  this->pgsnum = true;
  
  this->def_tex_lines = false;
  this->first_line = NULL;
  this->last_line = NULL;
  
  this->lock = false;
  this->first_copy_src = NULL;
  this->last_copy_src = NULL;
  
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


char * thlayout::get_cmd_end()
{
  return "endlayout";
}


char * thlayout::get_cmd_name()
{
  // insert command name here
  return "layout";
}


thcmd_option_desc thlayout::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_layout_opt);
  switch (id) {
    case TT_LAYOUT_NAV_SIZE:
    case TT_LAYOUT_OVERLAP:
    case TT_LAYOUT_SCALE:
    case TT_LAYOUT_EXCLUDE_PAGES:
    case TT_LAYOUT_ORIGIN_LABEL:
      return thcmd_option_desc(id,2);
    case TT_LAYOUT_GRID_SIZE:
    case TT_LAYOUT_SIZE:
      return thcmd_option_desc(id,3);
    case TT_LAYOUT_ORIGIN:
    case TT_LAYOUT_GRID_ORIGIN:
      return thcmd_option_desc(id,4);
    case TT_LAYOUT_PAGE_SETUP:
      return thcmd_option_desc(id,7);
    case TT_LAYOUT_UNKNOWN:
      return thdataobject::get_cmd_option_desc(opts);
    default:
      return thcmd_option_desc(id);
  }
}

void thlayout_parse_scale(thlayout * pl,char ** args) {
  double dv;
  int sv;
  thparse_double(sv,dv,args[0]);
  if (sv != TT_SV_NUMBER)
    ththrow(("invalid number -- %s",args[0]))
  if (dv <= 0.0)
    ththrow(("positive number expected -- %s",args[0]))
  pl->scale = dv;
  thparse_double(sv,dv,args[1]);
  if (sv != TT_SV_NUMBER)
    ththrow(("invalid number -- %s",args[1]))
  if (dv <= 0.0)
    ththrow(("positive number expected -- %s",args[0]))
  pl->scale /= dv;  
}



void thlayout::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  double dum;
  thlayout_copy_src dumm;
  int sv;
  thlayout_copy_src * lcp;
  switch (cod.id) {

    case 0:
      if (!this->def_tex_lines) {
        this->first_line = this->db->db2d.insert_layoutln();
        this->last_line = this->first_line;
        this->def_tex_lines = true;
      } else {
        this->last_line->next_line = this->db->db2d.insert_layoutln();
        this->last_line = this->last_line->next_line;
      }
      thencode(&(this->db->buff_enc), *args, argenc);
      this->last_line->line = this->db->strstore(this->db->buff_enc.get_buffer());
      this->last_line->code = this->ccode;
      break;
      
    case TT_LAYOUT_SCALE:
      thlayout_parse_scale(this,args);
      this->def_scale = true;
      break;    

    case TT_LAYOUT_OVERLAP:
      this->parse_len(this->overlap, dum, dum, 1, args, true);
      this->def_overlap = true;
      break;

    case TT_LAYOUT_SIZE:
      this->parse_len(this->hsize, this->vsize, dum, 2, args, true);
      this->def_size = true;
      break;

    case TT_LAYOUT_PAGE_SETUP:
      this->parse_len6(this->paphs, this->papvs, this->paghs, this->pagvs, this->marls, this->marts, 6, args, true);
      this->def_page_setup = true;
      break;

    case TT_LAYOUT_ORIGIN:
      this->parse_len(this->ox, this->oy, this->oz, 3, args, false);
      this->def_origin = true;
      break;

    case TT_LAYOUT_GRID_SIZE:
      this->parse_len(this->gxs, this->gys, dum, 2, args, true);
      this->def_grid_size = true;
      break;

    case TT_LAYOUT_GRID_ORIGIN:
      this->parse_len(this->gox, this->goy, this->goz, 3, args, false);
      this->def_grid_origin = true;
      break;

    case TT_LAYOUT_TRANSPARENCY:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid transparency switch -- %s",args[0]))
      this->transparency = (sv == TT_TRUE);
      this->def_transparency = true;
      break;
    
    case TT_LAYOUT_LAYERS:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid layers switch -- %s",args[0]))
      this->layers = (sv == TT_TRUE);
      this->def_layers = true;
      break;
    
    case TT_LAYOUT_OPACITY:
      thparse_double(sv,dum,args[0]);        
      if ((sv != TT_SV_NUMBER) || (dum < 0.0) || (dum > 100.0))
        ththrow(("invalid opacity value -- %s", args[0]))
      this->opacity = dum / 100.0;
      this->def_opacity = true;
      break;
    
    case TT_LAYOUT_GRID:
      sv = thmatch_token(args[0],thtt_layout_grid);
      if (sv == TT_LAYOUT_GRID_UNKNOWN)
        ththrow(("invalid grid switch -- %s",args[0]))
      this->grid = (char) sv;
      this->def_grid = true;
      break;
      
    case TT_LAYOUT_CODE:
      sv = thmatch_token(args[0],thtt_layout_code);
      if (sv == TT_LAYOUT_CODE_UNKNOWN)
        ththrow(("invalid code switch -- %s",args[0]))
      this->ccode = (char) sv;
      break;
      
    case TT_LAYOUT_PAGE_GRID:
      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid page-grid switch -- %s",args[0]))
      this->page_grid = (sv == TT_TRUE);
      this->def_page_grid = true;
      break;
    
    case TT_LAYOUT_EXCLUDE_PAGES:

      sv = thmatch_token(args[0],thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid pages exclusion switch -- %s",args[0]))
      this->excl_pages = (sv == TT_TRUE);

      if (strlen(args[1]) > 0)
        this->excl_list = this->db->strstore(args[1]);
      else if (this->excl_pages)
        ththrow(("invalid pages exclusion list -- %s",args[1]));

      this->def_excl_pages = true;
      break;
    
    case TT_LAYOUT_PAGE_NUMBERS:
      sv = thmatch_token(*args,thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid page numbers switch -- %s",*args))
      this->pgsnum = (sv == TT_TRUE);
      this->def_page_numbers = true;
      break;
    
    case TT_LAYOUT_TITLE_PAGES:
      sv = thmatch_token(*args,thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid title-pages switch -- %s",*args))
      this->titlep = (sv == TT_TRUE);
      this->def_title_pages = true;
      break;
    
    case TT_LAYOUT_DOC_TITLE:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_title = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_title = "";
      this->def_doc_title = true;  
      break;
    
    case TT_LAYOUT_DOC_AUTHOR:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_author = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_author = "";
      this->def_doc_author = true;  
      break;
    
    case TT_LAYOUT_DOC_SUBJECT:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_subject = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_subject = "";
      this->def_doc_subject = true;  
      break;
    
    case TT_LAYOUT_DOC_KEYWORDS:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->doc_keywords = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->doc_keywords = "";
      this->def_doc_keywords = true;  
      break;
    
    case TT_LAYOUT_ORIGIN_LABEL:

      if (strlen(args[0]) > 0) {
        //thencode(&(this->db->buff_enc), args[0], argenc);
        this->olx = this->db->strstore(args[0]);
      } else
        ththrow(("invalid label -- %s",args[0]));

      if (strlen(args[1]) > 0) {
        //thencode(&(this->db->buff_enc), args[1], argenc);
        this->oly = this->db->strstore(args[1]);
      } else
        ththrow(("invalid label -- %s",args[1]));

      this->def_origin_label = true;
      break;
    
    case TT_LAYOUT_NAV_SIZE:
    
      thparse_double(sv,dum,args[0]);        
      if ((sv != TT_SV_NUMBER) || (dum <= 0))
        ththrow(("invalid navigator size -- %s", *args))
      if (double(int(dum)) != dum)
        ththrow(("invalid navigator size -- %s", *args))
      this->navsx = unsigned(dum);

      thparse_double(sv,dum,args[1]);        
      if ((sv != TT_SV_NUMBER) || (dum <= 0))
        ththrow(("invalid navigator size -- %s", *args))
      if (double(int(dum)) != dum)
        ththrow(("invalid navigator size -- %s", *args))
      this->navsy = unsigned(dum);
      
      this->def_nav_size = true;
      break;
      
    case TT_LAYOUT_COPY:
      if (th_is_extkeyword(*args)) {
        lcp = & ( * thlayout_copy_src_list.insert(thlayout_copy_src_list.end(),dumm));
        if (this->first_copy_src == NULL) {
          this->first_copy_src = lcp;
          this->last_copy_src = lcp;
        } else {
          this->last_copy_src->next_src = lcp;
          this->last_copy_src = lcp;
        }
        lcp->srcn = this->db->strstore(*args);
      } else 
        ththrow(("invalid keyword -- %s", *args));
      break;
    
    case TT_LAYOUT_NAV_FACTOR:
      thparse_double(sv,this->navf,*args);
      if (sv != TT_SV_NUMBER)
        ththrow(("invalid factor -- %s", *args))
      if (this->navf <= 0.0)
        ththrow(("negative factor not allowed -- %s", *args))
      this->def_nav_factor = true;
      break;
    
    case TT_LAYOUT_OWN_PAGES:
      thparse_double(sv,dum,*args);        
      if ((sv != TT_SV_NUMBER) || (dum <= 0))
        ththrow(("invalid number of own pages -- %s", *args))
      if (double(int(dum)) != dum)
        ththrow(("invalid number of own pages -- %s", *args))
      this->ownp = unsigned(dum);
      this->def_own_pages = true;
      break;
    
    case 1:
      cod.id = TT_DATAOBJECT_NAME;
    default:
      thdataobject::set(cod, args, argenc, indataline);
      break;
  }
}


void thlayout::self_delete()
{
  delete this;
}

void thlayout::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thlayout:\n");
  fprintf(outf,"\tscale: %f\n",this->scale);
}


int thlayout::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}


void thlayout::self_print_library() {

  thprintf("\toname = \"%s\";\n", this->get_name());
  thprintf("\tplayout->set(thcmd_option_desc(TT_DATAOBJECT_NAME,1),&oname,TT_UTF_8,0);\n");
  // decode title
  thdecode_c(&(this->db->buff_enc), this->get_title());
  thprintf("\toname = \"%s\";\n", this->db->buff_enc.get_buffer());
  thprintf("\tplayout->set(thcmd_option_desc(TT_DATAOBJECT_TITLE,1),&oname,TT_UTF_8,0);\n");


  thprintf("\tplayout->def_scale = %s;\n",(this->def_scale ? "true" : "false"));
  thprintf("\tplayout->scale = %lg;\n",this->scale);

  thprintf("\tplayout->def_page_setup = %s;\n",(this->def_page_setup ? "true" : "false"));
  thprintf("\tplayout->hsize = %lg;\n",this->hsize);
  thprintf("\tplayout->vsize = %lg;\n",this->vsize);
  thprintf("\tplayout->paphs = %lg;\n",this->paphs);
  thprintf("\tplayout->papvs = %lg;\n",this->papvs);
  thprintf("\tplayout->paghs = %lg;\n",this->paghs);
  thprintf("\tplayout->pagvs = %lg;\n",this->pagvs);
  thprintf("\tplayout->marts = %lg;\n",this->marts);
  thprintf("\tplayout->marls = %lg;\n",this->marls);

  thprintf("\tplayout->def_overlap = %s;\n",(this->def_overlap ? "true" : "false"));
  thprintf("\tplayout->overlap = %lg;\n",this->overlap);

  thprintf("\tplayout->def_transparency = %s;\n",(this->def_transparency ? "true" : "false"));
  thprintf("\tplayout->transparency = %s;\n",(this->transparency ? "true" : "false"));

  thprintf("\tplayout->def_layers = %s;\n",(this->def_layers ? "true" : "false"));
  thprintf("\tplayout->layers = %s;\n",(this->layers ? "true" : "false"));

  thprintf("\tplayout->def_opacity = %s;\n",(this->def_opacity ? "true" : "false"));
  thprintf("\tplayout->opacity = %lg;\n",this->opacity);

  thprintf("\tplayout->def_grid = %s;\n",(this->def_grid ? "true" : "false"));
  thprintf("\tplayout->grid = %d;\n",this->grid);

  thprintf("\tplayout->def_page_grid = %s;\n",(this->def_page_grid ? "true" : "false"));
  thprintf("\tplayout->page_grid = %s;\n",(this->page_grid ? "true" : "false"));


  thprintf("\tplayout->def_origin = %s;\n",(this->def_origin ? "true" : "false"));
  if (!thisnan(this->ox))
    thprintf("\tplayout->ox = %lg;\n",this->ox);
  if (!thisnan(this->oy))
    thprintf("\tplayout->oy = %lg;\n",this->oy);
  if (!thisnan(this->oz))
    thprintf("\tplayout->oz = %lg;\n",this->oz);

  thprintf("\tplayout->def_origin_label = %s;\n",(this->def_origin_label ? "true" : "false"));
  thdecode_c(&(this->db->buff_enc), this->olx);
  thprintf("\tplayout->olx = \"%s\";\n", this->db->buff_enc.get_buffer());
  thdecode_c(&(this->db->buff_enc), this->oly);
  thprintf("\tplayout->oly = \"%s\";\n", this->db->buff_enc.get_buffer());

  thprintf("\tplayout->def_doc_title = %s;\n",(this->def_doc_title ? "true" : "false"));
  if (this->doc_title == NULL) {
    thprintf("\tplayout->doc_title = NULL;\n");
  } else {
    thdecode_c(&(this->db->buff_enc), this->doc_title);
    thprintf("\tplayout->doc_title = \"%s\";\n", this->db->buff_enc.get_buffer());
  }
  
  thprintf("\tplayout->def_doc_author = %s;\n",(this->def_doc_author ? "true" : "false"));
  if (this->doc_author == NULL) {
    thprintf("\tplayout->doc_author = NULL;\n");
  } else {
    thdecode_c(&(this->db->buff_enc), this->doc_author);
    thprintf("\tplayout->doc_author = \"%s\";\n", this->db->buff_enc.get_buffer());
  }

  thprintf("\tplayout->def_doc_subject = %s;\n",(this->def_doc_author ? "true" : "false"));
  if (this->doc_subject == NULL) {
    thprintf("\tplayout->doc_subject = NULL;\n");
  } else {
    thdecode_c(&(this->db->buff_enc), this->doc_subject);
    thprintf("\tplayout->doc_subject = \"%s\";\n", this->db->buff_enc.get_buffer());
  }
  
  thprintf("\tplayout->def_doc_keywords = %s;\n",(this->def_doc_keywords ? "true" : "false"));
  if (this->doc_keywords == NULL) {
    thprintf("\tplayout->doc_keywords = NULL;\n");
  } else {
    thdecode_c(&(this->db->buff_enc), this->doc_keywords);
    thprintf("\tplayout->doc_keywords = \"%s\";\n", this->db->buff_enc.get_buffer());
  }
  
  thprintf("\tplayout->def_excl_pages = %s;\n",(this->def_excl_pages ? "true" : "false"));
  thprintf("\tplayout->excl_pages = %s;\n",(this->excl_pages ? "true" : "false"));
  if (this->excl_list == NULL) {
    thprintf("\tplayout->excl_list = NULL;\n");
  } else {
    thdecode_c(&(this->db->buff_enc), this->excl_list);
    thprintf("\tplayout->excl_list = \"%s\";\n", this->db->buff_enc.get_buffer());
  }
  
  thprintf("\tplayout->def_grid_size = %s;\n",(this->def_grid_size ? "true" : "false"));
  thprintf("\tplayout->gxs = %lg;\n",this->gxs);
  thprintf("\tplayout->gys = %lg;\n",this->gys);

  thprintf("\tplayout->def_grid_origin = %s;\n",(this->def_grid_origin ? "true" : "false"));
  if (!thisnan(this->gox))
    thprintf("\tplayout->gox = %lg;\n",this->gox);
  if (!thisnan(this->goy))
    thprintf("\tplayout->goy = %lg;\n",this->goy);
  if (!thisnan(this->goz))
    thprintf("\tplayout->goz = %lg;\n",this->goz);

  thprintf("\tplayout->def_nav_factor = %s;\n",(this->def_nav_factor ? "true" : "false"));
  thprintf("\tplayout->navf = %lg;\n",this->navf);

  thprintf("\tplayout->def_nav_size = %s;\n",(this->def_nav_size ? "true" : "false"));
  thprintf("\tplayout->navsx = %d;\n",this->navsx);
  thprintf("\tplayout->navsy = %d;\n",this->navsy);
  
  thprintf("\tplayout->def_own_pages = %s;\n",(this->def_own_pages ? "true" : "false"));
  thprintf("\tplayout->ownp = %d;\n",this->ownp);

  thprintf("\tplayout->def_title_pages = %s;\n",(this->def_title_pages ? "true" : "false"));
  thprintf("\tplayout->titlep = %s;\n",(this->titlep ? "true" : "false"));

  thprintf("\tplayout->def_page_numbers = %s;\n",(this->def_page_numbers ? "true" : "false"));
  thprintf("\tplayout->pgsnum = %s;\n",(this->pgsnum ? "true" : "false"));

  
  thlayoutln * ln = this->first_line;
  char last_code = TT_LAYOUT_CODE_UNKNOWN;
  while(ln != NULL) {
    thdecode_c(&(this->db->buff_enc), ln->line);
    if (ln->code != last_code) {
      thprintf("\tplayout->ccode = \"");
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
      thprintf("\";\n");
    }
    thprintf("\toname = \"%s\";\n", this->db->buff_enc.get_buffer());
    thprintf("\tplayout->set(thcmd_option_desc(0,1),&oname,TT_UTF_8,0);\n");
    ln = ln->next_line;
  }
  thprintf("\tplayout->def_tex_lines = %s;\n",(this->def_tex_lines ? "true" : "false"));
}


void thlayout::parse_len(double & d1, double & d2, double & d3, int nargs, char ** args, bool nonneg) {
  int sv;
  thtflength lentf;
  lentf.parse_units(args[nargs]);
  switch (nargs) {
    case 3:
      thparse_double(sv,d3,args[2]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[2]));
      d3 = lentf.transform(d3);
      if (nonneg && (d3 <= 0.0))
        ththrow(("not a positive number -- %s", args[2]));
    case 2:
      thparse_double(sv,d2,args[1]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[1]));
      d2 = lentf.transform(d2);
      if (nonneg && (d2 <= 0.0))
        ththrow(("not a positive number -- %s", args[1]));
    case 1:
      thparse_double(sv,d1,args[0]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[0]));
      d1 = lentf.transform(d1);
      if (nonneg && (d1 <= 0.0))
        ththrow(("not a positive number -- %s", args[0]));
  }
}


void thlayout::parse_len6(double & d1, double & d2, double & d3, double & d4, double & d5, double & d6, int nargs, char ** args, bool nonneg) {
  int sv;
  thtflength lentf;
  lentf.parse_units(args[nargs]);
  switch (nargs) {
    case 6:
      thparse_double(sv,d6,args[5]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[5]));
      d6 = lentf.transform(d6);
      if (nonneg && (d6 <= 0.0))
        ththrow(("not a positive number -- %s", args[5]));
    case 5:
      thparse_double(sv,d5,args[4]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[4]));
      d5 = lentf.transform(d5);
      if (nonneg && (d5 <= 0.0))
        ththrow(("not a positive number -- %s", args[4]));
    case 4:
      thparse_double(sv,d4,args[3]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[3]));
      d4 = lentf.transform(d4);
      if (nonneg && (d4 <= 0.0))
        ththrow(("not a positive number -- %s", args[3]));
    case 3:
      thparse_double(sv,d3,args[2]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[2]));
      d3 = lentf.transform(d3);
      if (nonneg && (d3 <= 0.0))
        ththrow(("not a positive number -- %s", args[2]));
    case 2:
      thparse_double(sv,d2,args[1]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[1]));
      d2 = lentf.transform(d2);
      if (nonneg && (d2 <= 0.0))
        ththrow(("not a positive number -- %s", args[1]));
    case 1:
      thparse_double(sv,d1,args[0]);
      if ((sv != TT_SV_NUMBER))
        ththrow(("invalid number -- %s", args[0]));
      d1 = lentf.transform(d1);
      if (nonneg && (d1 <= 0.0))
        ththrow(("not a positive number -- %s", args[0]));
  }
}

#define THM2PT 2834.64566929

void thlayout::export_config(FILE * o, thdb2dprj * prj, double x_scale, double x_origin_shx, double x_origin_shy) {
  double pgox, pgoy;
  fprintf(o,"Configuration file\n");
  fprintf(o,"hsize: %.2fmm\nvsize: %.2fmm\n",this->hsize * 1000.0,this->vsize * 1000.0);
  fprintf(o,"Overlap: %.2fmm\n",this->overlap * 1000.0);
  //fprintf(o,"Hoffset: %.2fmm\nVoffset: %.2fmm\n",this->hsize * 500.0,this->vsize * 500.0);

  fprintf(o,"HGrid: %.2fmm\nVGrid: %.2fmm\n",this->gxs * this->scale * 1000.0, this->gys * this->scale * 1000.0);
  switch (prj->type) {
    case TT_2DPROJ_EXTEND:
    case TT_2DPROJ_ELEV:
      pgox = ((thisnan(this->gox) ? 0.0 : this->gox) - prj->shift_x) * x_scale + x_origin_shx;
      pgoy = ((thisnan(this->goz) ? 0.0 : this->goz) - prj->shift_z) * x_scale + x_origin_shy;
      break;
    default:
      pgox = ((thisnan(this->gox) ? 0.0 : this->gox) - prj->shift_x) * x_scale + x_origin_shx;
      pgoy = ((thisnan(this->goy) ? 0.0 : this->goy) - prj->shift_y) * x_scale + x_origin_shy;
      break;
  }
  pgox = pgox - (THM2PT * this->gxs * (double)(long)(pgox / this->gxs / THM2PT));
  pgoy = pgoy - (THM2PT * this->gys * (double)(long)(pgoy / this->gys / THM2PT));
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

  
void thlayout::export_pdftex(FILE * o, thdb2dprj * prj, char mode) {

  fprintf(o,"\\opacity{%.2f}\n",this->opacity);
  fprintf(o,"\\def\\scale{%lu}\n",(unsigned long)(1.0 / this->scale));
  fprintf(o,"\\pagesetup{%.4fcm}{%.4fcm}{%.4fcm}{%.4fcm}{%.4fcm}{%.4fcm}\n",
    this->paphs*100.0, this->papvs*100.0, 
    this->paghs*100.0, this->pagvs*100.0, 
    this->marls*100.0, this->marts*100.0);

  bool anyline = false;
  if (this->first_line != NULL) {
    thlayoutln * ln = this->first_line;
    while(ln != NULL) {
      if (ln->code == mode) {
        anyline = true;
        thdecode(&(this->db->buff_enc), TT_ISO8859_2, ln->line);
        fprintf(o, "%s\n", this->db->buff_enc.get_buffer());
      }
      ln = ln->next_line;
    }
  }
  
  if (!anyline) {
    fprintf(o,"\\insertmaps\n");
  }

}


void thlayout::export_mpost(FILE * o) {

  bool anyline = false;
  if (this->first_line != NULL) {
    thlayoutln * ln = this->first_line;
    while(ln != NULL) {
      if (ln->code == TT_LAYOUT_CODE_METAPOST) {
        anyline = true;
        thdecode(&(this->db->buff_enc), TT_ISO8859_2, ln->line);
        fprintf(o, "%s\n", this->db->buff_enc.get_buffer());
      }
      ln = ln->next_line;
    }
  }
  
  if (!anyline) {
  }

}




void thlayout::process_copy() {
  thlayout_copy_src * csp;
  thlayout * srcl;
  // ak je locknuty -> tak warning a koniec
  if (this->lock) {
    this->throw_source();
    threwarning2(("recursive layout copying"))
    return;
  }
  this->lock = true;
  this->last_copy_src = this->first_copy_src;
  while (this->first_copy_src != NULL) {
    csp = this->first_copy_src;
    // najdeme si layout podla mena
    csp->srcptr = this->db->get_layout(csp->srcn);
    if (csp->srcptr == NULL) {
      this->throw_source();
      threwarning2(("source layout not found -- %s", csp->srcn))
    } else {
      srcl = csp->srcptr;
      // ak ma este nevyriesene zavislosti
      if (srcl->first_copy_src != NULL)
        srcl->process_copy();
      
      // teraz skopirujme co nemame a on ma
      
      if (!this->def_scale)
        this->scale = srcl->scale;

      if (!this->def_origin) {
        this->ox = srcl->ox;
        this->oy = srcl->oy;
        this->oz = srcl->oz;
      }

      if (!this->def_size) {
        this->hsize = srcl->hsize;
        this->vsize = srcl->vsize;
      }
  
      if (!this->def_page_setup) {
        this->paphs = srcl->paphs;
        this->papvs = srcl->papvs;
        this->paghs = srcl->paghs;
        this->pagvs = srcl->pagvs;
        this->marls = srcl->marls;
        this->marts = srcl->marts;
      }  

      if (!this->def_overlap)
        this->overlap = srcl->overlap;
  
      if (!this->def_grid_origin) {
        this->gox = srcl->gox;
        this->goy = srcl->goy;
        this->goz = srcl->goz;
      }  

      if (!this->def_excl_pages) {
        this->excl_pages = srcl->excl_pages;
        this->excl_list = srcl->excl_list;
      }

      if (!this->def_doc_title)
        this->doc_title = srcl->doc_title;

      if (!this->def_doc_author)
        this->doc_author = srcl->doc_author;

      if (!this->def_doc_subject)
        this->doc_subject = srcl->doc_subject;

      if (!this->def_doc_keywords)
        this->doc_keywords = srcl->doc_keywords;

      if (!this->def_grid_size) {
        this->gxs = srcl->gxs;
        this->gys = srcl->gys;
      }
  
      if (!this->def_origin_label) {
        this->olx = srcl->olx;
        this->oly = srcl->oly;
      }
  
      if (!this->def_nav_factor)
        this->navf = srcl->navf;
  
      if (!this->def_nav_size) {
        this->navsx = srcl->navsx;
        this->navsy = srcl->navsy;
      }

      if (!this->def_own_pages)
        this->ownp = srcl->ownp;
      
      if (!this->def_title_pages)
        this->titlep = srcl->titlep;
        
      if (!this->def_opacity)
        this->opacity = srcl->opacity;
  
      if (!this->def_transparency)
        this->transparency = srcl->transparency;
  
      if (!this->def_layers)
        this->layers = srcl->layers;
  
      if (!this->def_grid)
        this->grid = srcl->grid;

      if (!this->def_page_grid)
        this->page_grid = srcl->page_grid;
  
      if (!this->def_page_numbers)
        this->pgsnum = srcl->pgsnum;

      if (!this->def_tex_lines) {
        this->first_line = srcl->first_line;
        this->last_line = srcl->last_line;
      }
      
    } 
    // pokracujeme v cykle
    this->first_copy_src = this->first_copy_src->next_src;
  }
  this->lock = false;
}

void thlayout::set_thpdf_layout(thdb2dprj * prj, double x_scale, double x_origin_shx, double x_origin_shy) {
  //string excl_list,labelx,labely;
  //bool  excl_pages,background,title_pages,page_numbering,
  //      transparency,map_grid; 
  //float hsize,vsize,overlap,
  //     hgrid,vgrid,hgridoffset,vgridoffset,
	//nav_factor;
  //int nav_right,nav_up,own_pages;

  double pgox, pgoy;

  LAYOUT.excl_list = (this->excl_list != NULL ? this->excl_list : "");
  LAYOUT.labelx = this->olx;
  LAYOUT.labely = this->oly;
  LAYOUT.excl_pages = this->excl_pages;
  //TODO
  LAYOUT.background = false;
  LAYOUT.title_pages = this->titlep;
  LAYOUT.page_numbering = this->pgsnum;
  LAYOUT.transparency = this->transparency;
  LAYOUT.OCG = this->layers;
  //TODO
  LAYOUT.map_grid = this->page_grid;
  LAYOUT.hsize = this->hsize * THM2PT;
  LAYOUT.vsize = this->vsize * THM2PT;
  LAYOUT.overlap = this->overlap * THM2PT;
  LAYOUT.hgrid = this->gxs * THM2PT;
  LAYOUT.vgrid = this->gys * THM2PT;

  switch (prj->type) {
    case TT_2DPROJ_EXTEND:
    case TT_2DPROJ_ELEV:
      pgox = ((thisnan(this->gox) ? 0.0 : this->gox) - prj->shift_x) * x_scale + x_origin_shx;
      pgoy = ((thisnan(this->goz) ? 0.0 : this->goz) - prj->shift_z) * x_scale + x_origin_shy;
      break;
    default:
      pgox = ((thisnan(this->gox) ? 0.0 : this->gox) - prj->shift_x) * x_scale + x_origin_shx;
      pgoy = ((thisnan(this->goy) ? 0.0 : this->goy) - prj->shift_y) * x_scale + x_origin_shy;
      break;
  }
  pgox = pgox - (THM2PT * this->gxs * (double)(long)(pgox / this->gxs / THM2PT));
  pgoy = pgoy - (THM2PT * this->gys * (double)(long)(pgoy / this->gys / THM2PT));  

  LAYOUT.hgridoffset = pgox;
  LAYOUT.vgridoffset = pgoy;
  LAYOUT.nav_factor = this->navf;
  LAYOUT.nav_right = this->navsx;
  LAYOUT.nav_up = this->navsy;
  LAYOUT.own_pages = this->ownp;
  
  if (this->doc_title != NULL)
    LAYOUT.doc_title = this->doc_title;
  if (this->doc_author != NULL)
    LAYOUT.doc_author = this->doc_author;
  if (this->doc_subject != NULL)
    LAYOUT.doc_subject = this->doc_subject;
  if (this->doc_keywords != NULL)
    LAYOUT.doc_keywords = this->doc_keywords;
  LAYOUT.opacity = this->opacity;
  
}


std::list <thlayout_copy_src> thlayout_copy_src_list;
