/**
 * @file thlayout.h
 * Map layout module.
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
 
#ifndef thlayout_h
#define thlayout_h


#include "thdataobject.h"
#include "thlayoutln.h"
#include <list>

/**
 * layout command options tokens.
 */
 
enum {
  TT_LAYOUT_UNKNOWN = 2000,
  TT_LAYOUT_SCALE = 2001,
  TT_LAYOUT_ORIGIN = 2002,
  TT_LAYOUT_SIZE = 2003,
  TT_LAYOUT_GRID_SIZE = 2004,
  TT_LAYOUT_GRID_ORIGIN = 2005,
  TT_LAYOUT_ORIGIN_LABEL = 2006,
  TT_LAYOUT_NAV_SIZE = 2007,
  TT_LAYOUT_NAV_FACTOR = 2008,
  TT_LAYOUT_OWN_PAGES = 2009,
  TT_LAYOUT_TITLE_PAGES = 2010,
  TT_LAYOUT_OPACITY = 2011,
  TT_LAYOUT_OVERLAP = 2012,
  TT_LAYOUT_PAGE_NUMBERS = 2013,
  TT_LAYOUT_PAGE_SETUP = 2014,
  TT_LAYOUT_COPY = 2015,
  TT_LAYOUT_DOC_TITLE = 2016,
  TT_LAYOUT_DOC_AUTHOR = 2017,
  TT_LAYOUT_DOC_SUBJECT = 2018,
  TT_LAYOUT_DOC_KEYWORDS = 2019,
  TT_LAYOUT_EXCLUDE_PAGES = 2020,
  TT_LAYOUT_GRID = 2021,
  TT_LAYOUT_PAGE_GRID = 2022,
  TT_LAYOUT_TRANSPARENCY = 2023,
};


/**
 * Layout grid tokens.
 */

enum {
  TT_LAYOUT_GRID_UNKNOWN = 0,
  TT_LAYOUT_GRID_NONE,
  TT_LAYOUT_GRID_CROSS,
  TT_LAYOUT_GRID_LINE,
};


/**
 * Layout grid token table.
 */
 
static const thstok thtt_layout_grid[] = {
  {"cross", TT_LAYOUT_GRID_CROSS},
  {"line", TT_LAYOUT_GRID_LINE},
  {"none", TT_LAYOUT_GRID_NONE},
  {"off", TT_LAYOUT_GRID_NONE},
  {"on", TT_LAYOUT_GRID_LINE},
  {NULL, TT_LAYOUT_GRID_UNKNOWN}
};


class thlayout_copy_src {

  public:
  
  char * srcn;
  thlayout * srcptr;
  class thlayout_copy_src * next_src;
  thlayout_copy_src () : srcn(NULL), srcptr(NULL), next_src(NULL) {};
  
};


/**
 * layout command options parsing table.
 */
 
static const thstok thtt_layout_opt[] = {
  {"copy",TT_LAYOUT_COPY},
  {"doc-author",TT_LAYOUT_DOC_AUTHOR},
  {"doc-keywords",TT_LAYOUT_DOC_KEYWORDS},
  {"doc-subject",TT_LAYOUT_DOC_SUBJECT},
  {"doc-title",TT_LAYOUT_DOC_TITLE},
  {"exclude-pages",TT_LAYOUT_EXCLUDE_PAGES},
  {"grid",TT_LAYOUT_GRID},
  {"grid-origin",TT_LAYOUT_GRID_ORIGIN},
  {"grid-size",TT_LAYOUT_GRID_SIZE},
  {"nav-factor",TT_LAYOUT_NAV_FACTOR},
  {"nav-size",TT_LAYOUT_NAV_SIZE},
  {"opacity",TT_LAYOUT_OPACITY},
  {"origin",TT_LAYOUT_ORIGIN},
  {"origin-label",TT_LAYOUT_ORIGIN_LABEL},
  {"overlap",TT_LAYOUT_OVERLAP},
  {"own-pages",TT_LAYOUT_OWN_PAGES},
  {"page-grid",TT_LAYOUT_PAGE_GRID},
  {"page-numbers",TT_LAYOUT_PAGE_NUMBERS},
  {"page-setup",TT_LAYOUT_PAGE_SETUP},
  {"scale", TT_LAYOUT_SCALE},
  {"size", TT_LAYOUT_SIZE},
  {"title-pages",TT_LAYOUT_TITLE_PAGES},
  {"transparency",TT_LAYOUT_TRANSPARENCY},
  {NULL, TT_LAYOUT_UNKNOWN},
};


/**
 * layout class.
 */

class thlayout : public thdataobject {

  public:
    
  double scale, ox, oy, oz, hsize, vsize, paphs, papvs, paghs, pagvs, marls, marts, gxs, gys, gox, goy, goz, navf, overlap, opacity;
  
  char * olx, * oly, 
    * doc_title, * doc_author, * doc_subject, * doc_keywords, * excl_list;
  
  unsigned navsx, navsy, ownp;
  
  char grid;
  
  thlayoutln * first_line, * last_line;
  
  bool titlep, transparency, pgsnum, lock, excl_pages, page_grid;
  
  bool def_grid_size, def_grid_origin, def_nav_factor, def_nav_size, 
    def_opacity, def_transparency, def_origin, def_origin_label, def_overlap, def_own_pages,
    def_page_numbers, def_page_setup, def_scale, def_size, def_title_pages,
    def_tex_lines, def_doc_title, def_doc_author, def_doc_subject,
    def_doc_keywords, def_excl_pages, def_grid, def_page_grid;
  
  thlayout_copy_src * first_copy_src, * last_copy_src;
  
  /**
   * Standard constructor.
   */
  
  thlayout();
  
  
  /**
   * Standard destructor.
   */
   
  ~thlayout();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual char * get_class_name() {return "thlayout";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  virtual bool is(int class_id);
  
  
  /**
   * Return number of command arguments.
   */
   
  virtual int get_cmd_nargs();
  
  
  /**
   * Return command name.
   */
   
  virtual char * get_cmd_name();
  
  
  /**
   * Return command end option.
   */
   
  virtual char * get_cmd_end();
  
  
  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(char * opts);
  
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments arry.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Delete this object.
   *
   * @warn Always use this methos instead of delete function.
   */
   
  virtual void self_delete();


  /**
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  

  /**
   * Print object in C++ format.
   */
   
  void self_print_library(); 
  

  /**
   * Get context for object.
   */
   
  virtual int get_context();
  
  /**
   * Parse option with 3 or 2 or 1 numbers and units.
   */
   
  void parse_len(double & d1, double & d2, double & d3, int nargs, char ** args, bool nonneg = false);
  void parse_len6(double & d1, double & d2, double & d3, double & d4, double & d5, double & d6, int nargs, char ** args, bool nonneg = false);


  /**
   * Export configuration file.
   */
   
  void export_config(FILE * o, thdb2dprj * prj, double x_scale, double x_origin_shx, double x_origin_shy);
  
  /**
   * Export main tex file.
   */
   
  void export_pdftex(FILE * o, thdb2dprj * prj);
  
  /**
   * Process copy.
   */
   
  void process_copy();

};

/**
 * Copy src list.
 */
 
extern std::list <thlayout_copy_src> thlayout_copy_src_list;

#endif


