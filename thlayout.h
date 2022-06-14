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
#include "thsymbolset.h"
#include "thlocale.h"
#include "thlayoutclr.h"
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
  TT_LAYOUT_LAYERS = 2024,
  TT_LAYOUT_CODE = 2025,
  TT_LAYOUT_BASE_SCALE = 2026,
  TT_LAYOUT_SYMBOL_DEFAULTS = 2027,
  TT_LAYOUT_SYMBOL_ASSIGN = 2028,
  TT_LAYOUT_SYMBOL_HIDE = 2029,
  TT_LAYOUT_COLOR = 2030,
  TT_LAYOUT_SYMBOL_SHOW = 2031,
  TT_LAYOUT_LEGEND = 2032,
  TT_LAYOUT_DOC_COMMENT = 2033,
  TT_LAYOUT_MAP_HEADER = 2034,
  TT_LAYOUT_LANG = 2035,
  TT_LAYOUT_MAP_ITEM = 2036,
  TT_LAYOUT_SCALE_BAR = 2037,
  TT_LAYOUT_DEBUG = 2038,
  TT_LAYOUT_LEGEND_WIDTH = 2039,
  TT_LAYOUT_LEGEND_COLUMNS = 2040,
  TT_LAYOUT_SURFACE = 2041,
  TT_LAYOUT_SURFACE_OPACITY = 2042,
  TT_LAYOUT_ROTATE = 2043,
  TT_LAYOUT_ENDCODE = 2044,
  TT_LAYOUT_COLOR_LEGEND = 2045,
  TT_LAYOUT_MAP_HEADER_BG = 2046,
	TT_LAYOUT_UNITS = 2047,
	TT_LAYOUT_SURVEY_LEVEL = 2048,
	TT_LAYOUT_SKETCHES = 2049,
	TT_LAYOUT_NORTH = 2050,
  TT_LAYOUT_MAP_IMAGE = 2051,
  TT_LAYOUT_GRID_COORDS = 2052,
  TT_LAYOUT_SYMBOL_COLOR = 2053,
  TT_LAYOUT_FONT_SETUP = 2054,
  TT_LAYOUT_MIN_SYMBOL_SCALE = 2055,
  TT_LAYOUT_COLOR_MODEL = 2056,
  TT_LAYOUT_COLOR_PROFILE = 2057,
  TT_LAYOUT_SMOOTH_SHADING = 2058,
};


/**
 * Layout grid tokens.
 */

enum {
  TT_LAYOUT_GRID_UNKNOWN = 0,
  TT_LAYOUT_GRID_OFF,
  TT_LAYOUT_GRID_TOP,
  TT_LAYOUT_GRID_BOTTOM,
};


/**
 * Layout grid token table.
 */
 
static const thstok thtt_layout_grid[] = {
  {"bottom", TT_LAYOUT_GRID_BOTTOM},
  {"off", TT_LAYOUT_GRID_OFF},
  {"top", TT_LAYOUT_GRID_TOP},
  {NULL, TT_LAYOUT_GRID_UNKNOWN}
};


/**
 * Layout grid tokens.
 */

enum {
  TT_LAYOUT_GRIDCOORDS_UNKNOWN = 0,
  TT_LAYOUT_GRIDCOORDS_OFF,
  TT_LAYOUT_GRIDCOORDS_BORDER,
  TT_LAYOUT_GRIDCOORDS_ALL,
};


/**
 * Layout grid token table.
 */
 
static const thstok thtt_layout_gridcoords[] = {
  {"all", TT_LAYOUT_GRIDCOORDS_ALL},
  {"border", TT_LAYOUT_GRIDCOORDS_BORDER},
  {"off", TT_LAYOUT_GRIDCOORDS_OFF},
  {NULL, TT_LAYOUT_GRIDCOORDS_UNKNOWN}
};


/**
 * Layout map-item token table.
 */
 
//static const thstok thtt_layout_mapitem[] = {
//  {"north-arrow", SYMS_NORTHARROW},
//  {"scale-bar", SYMS_SCALEBAR},
//  {NULL, SYMS_ZZZ}
//};

/**
 * Layout legend tokens.
 */

enum {
  TT_LAYOUT_LEGEND_UNKNOWN = 0,
  TT_LAYOUT_LEGEND_OFF,
  TT_LAYOUT_LEGEND_ON,
  TT_LAYOUT_LEGEND_ALL,
};


/**
 * Layout legend token table.
 */
 
static const thstok thtt_layout_legend[] = {
  {"all", TT_LAYOUT_LEGEND_ALL},
  {"off", TT_LAYOUT_LEGEND_OFF},
  {"on", TT_LAYOUT_LEGEND_ON},
  {NULL, TT_LAYOUT_LEGEND_UNKNOWN},
};


/**
 * Layout color legend tokens.
 */

enum {
  TT_LAYOUT_COLORLEGEND_UNKNOWN = 0,
  TT_LAYOUT_COLORLEGEND_OFF,
  TT_LAYOUT_COLORLEGEND_DISCRETE,
  TT_LAYOUT_COLORLEGEND_SMOOTH,
};


/**
 * Layout color legend token table.
 */

static const thstok thtt_layout_colorlegend[] = {
  {"discrete", TT_LAYOUT_COLORLEGEND_DISCRETE},
  {"off", TT_LAYOUT_COLORLEGEND_OFF},
  {"on", TT_LAYOUT_COLORLEGEND_SMOOTH}, // legacy option
  {"smooth", TT_LAYOUT_COLORLEGEND_SMOOTH},
  {NULL, TT_LAYOUT_LEGEND_UNKNOWN},
};



/**
 * Layout debug tokens.
 */

enum {
  TT_LAYOUT_DEBUG_OFF,
  TT_LAYOUT_DEBUG_STATIONS,
  TT_LAYOUT_DEBUG_JOINS,
  TT_LAYOUT_DEBUG_SCRAPNAMES,
  TT_LAYOUT_DEBUG_STATIONNAMES,
  TT_LAYOUT_DEBUG_ALL,
  TT_LAYOUT_DEBUG_UNKNOWN,
};


/**
 * Layout legend token table.
 */
 
static const thstok thtt_layout_debug[] = {
  {"all", TT_LAYOUT_DEBUG_ALL},
  {"first", TT_LAYOUT_DEBUG_STATIONS},
  {"off", TT_LAYOUT_DEBUG_OFF},
  {"on", TT_LAYOUT_DEBUG_ALL},
  {"scrap-names", TT_LAYOUT_DEBUG_SCRAPNAMES},
  {"second", TT_LAYOUT_DEBUG_JOINS},
  {"station-names", TT_LAYOUT_DEBUG_STATIONNAMES},
  {NULL, TT_LAYOUT_DEBUG_UNKNOWN},
};


/**
 * Layout surface tokens.
 */

enum {
  TT_LAYOUT_SURFACE_OFF,
  TT_LAYOUT_SURFACE_TOP,
  TT_LAYOUT_SURFACE_BOTTOM,
  TT_LAYOUT_SURFACE_UNKNOWN,
};


/**
 * Layout legend token table.
 */
 
static const thstok thtt_layout_surface[] = {
  {"bottom", TT_LAYOUT_SURFACE_BOTTOM},
  {"off", TT_LAYOUT_SURFACE_OFF},
  {"top", TT_LAYOUT_SURFACE_TOP},
  {NULL, TT_LAYOUT_SURFACE_UNKNOWN},
};


/**
 * Layout smooth shading tokens.
 */

enum {
  TT_LAYOUT_SMOOTHSHADING_OFF,
  TT_LAYOUT_SMOOTHSHADING_QUICK,
  TT_LAYOUT_SMOOTHSHADING_UNKNOWN,
};


/**
 * Layout smooth shading token table.
 */
 
static const thstok thtt_layout_smoothshading[] = {
  {"off", TT_LAYOUT_SMOOTHSHADING_OFF},
  {"quick", TT_LAYOUT_SMOOTHSHADING_QUICK},
  {NULL, TT_LAYOUT_SMOOTHSHADING_UNKNOWN},
};







/**
 * Layout legend tokens.
 */

enum {
  TT_LAYOUT_MAP_HEADER_UNKNOWN = 0,
  TT_LAYOUT_MAP_HEADER_N,
  TT_LAYOUT_MAP_HEADER_NW,
  TT_LAYOUT_MAP_HEADER_NE,
  TT_LAYOUT_MAP_HEADER_E,
  TT_LAYOUT_MAP_HEADER_W,
  TT_LAYOUT_MAP_HEADER_S,
  TT_LAYOUT_MAP_HEADER_SW,
  TT_LAYOUT_MAP_HEADER_SE,
  TT_LAYOUT_MAP_HEADER_OFF,
  TT_LAYOUT_MAP_HEADER_CENTER,
};


/**
 * Layout legend token table.
 */
 
static const thstok thtt_layout_map_header[] = {
  {"c", TT_LAYOUT_MAP_HEADER_CENTER},
  {"center", TT_LAYOUT_MAP_HEADER_CENTER},
  {"e", TT_LAYOUT_MAP_HEADER_E},
  {"n", TT_LAYOUT_MAP_HEADER_N},
  {"ne", TT_LAYOUT_MAP_HEADER_NE},
  {"nw", TT_LAYOUT_MAP_HEADER_NW},
  {"off", TT_LAYOUT_MAP_HEADER_OFF},
  {"s", TT_LAYOUT_MAP_HEADER_S},
  {"se", TT_LAYOUT_MAP_HEADER_SE},
  {"sw", TT_LAYOUT_MAP_HEADER_SW},
  {"w", TT_LAYOUT_MAP_HEADER_W},
  {NULL, TT_LAYOUT_MAP_HEADER_UNKNOWN},
};



/**
 * Layout code tokens.
 */

enum {
  TT_LAYOUT_CODE_UNKNOWN = 0,
  TT_LAYOUT_CODE_TEX_ATLAS,
  TT_LAYOUT_CODE_TEX_MAP,
  TT_LAYOUT_CODE_METAPOST,
  TT_LAYOUT_CODE_SYMBOL_DEFAULTS,
  TT_LAYOUT_CODE_SYMBOL_ASSIGN,
  TT_LAYOUT_CODE_SYMBOL_HIDE,
  TT_LAYOUT_CODE_SYMBOL_SHOW,
  TT_LAYOUT_CODE_SYMBOL_COLOR,
  TT_LAYOUT_CODE_MAP_ITEM,
};


/**
 * Layout grid token table.
 */
 
static const thstok thtt_layout_code[] = {
  {"metapost", TT_LAYOUT_CODE_METAPOST},
  {"mpost", TT_LAYOUT_CODE_METAPOST},
  {"tex-atlas", TT_LAYOUT_CODE_TEX_ATLAS},
  {"tex-map", TT_LAYOUT_CODE_TEX_MAP},
  {NULL, TT_LAYOUT_CODE_UNKNOWN}
};


/**
 * Layout north tokens.
 */

enum {
  TT_LAYOUT_NORTH_UNKNOWN = 0,
  TT_LAYOUT_NORTH_TRUE,
  TT_LAYOUT_NORTH_GRID,
};


/**
 * Layout color token table.
 */
 
static const thstok thtt_layout_north[] = {
  {"grid", TT_LAYOUT_NORTH_GRID},
  {"true", TT_LAYOUT_NORTH_TRUE},
  {NULL, TT_LAYOUT_NORTH_UNKNOWN}
};

/**
 * Layout length statistics tokens.
 */

enum {
  TT_LAYOUT_LENSTAT_UNKNOWN = 0,
  TT_LAYOUT_LENSTAT_OFF,
  TT_LAYOUT_LENSTAT_HIDE,
  TT_LAYOUT_LENSTAT_ON,
};


/**
 * Layout color token table.
 */
 
static const thstok thtt_layout_lenstat[] = {
  {"hide", TT_LAYOUT_LENSTAT_HIDE},
  {"off", TT_LAYOUT_LENSTAT_OFF},
  {"on", TT_LAYOUT_LENSTAT_ON},
  {NULL, TT_LAYOUT_LENSTAT_UNKNOWN}
};


/**
 * Layout color tokens.
 */

enum {
  TT_LAYOUT_COLOR_UNKNOWN = 0,
  TT_LAYOUT_COLOR_MAP_BG,
  TT_LAYOUT_COLOR_MAP_FG,
  TT_LAYOUT_COLOR_PREVIEW_BELOW,
  TT_LAYOUT_COLOR_PREVIEW_ABOVE,
  TT_LAYOUT_COLOR_LABELS,
};


/**
 * Layout color token table.
 */
 
static const thstok thtt_layout_color[] = {
  {"labels", TT_LAYOUT_COLOR_LABELS},
  {"map-bg", TT_LAYOUT_COLOR_MAP_BG},
  {"map-fg", TT_LAYOUT_COLOR_MAP_FG},
  {"preview-above", TT_LAYOUT_COLOR_PREVIEW_ABOVE},
  {"preview-below", TT_LAYOUT_COLOR_PREVIEW_BELOW},
  {NULL, TT_LAYOUT_COLOR_UNKNOWN}
};


/**
 * Layout color criterion tokens.
 */

enum {
  TT_LAYOUT_CCRIT_UNKNOWN = 0,
  TT_LAYOUT_CCRIT_ALTITUDE,
  TT_LAYOUT_CCRIT_DEPTH,
  TT_LAYOUT_CCRIT_TOPODATE,
  TT_LAYOUT_CCRIT_EXPLODATE,
  TT_LAYOUT_CCRIT_MAP,
  TT_LAYOUT_CCRIT_SCRAP,
  TT_LAYOUT_CCRIT_SURVEY,
};

static const thstok thtt_layout_ccrit[] = {
  {"altitude", TT_LAYOUT_CCRIT_ALTITUDE},
  {"altitudes", TT_LAYOUT_CCRIT_ALTITUDE},
  {"depth", TT_LAYOUT_CCRIT_DEPTH},
  {"explo-date", TT_LAYOUT_CCRIT_EXPLODATE},
  {"map", TT_LAYOUT_CCRIT_MAP},
  {"maps", TT_LAYOUT_CCRIT_MAP},
  {"scrap", TT_LAYOUT_CCRIT_SCRAP},
  {"scraps", TT_LAYOUT_CCRIT_SCRAP},
  {"topo-date", TT_LAYOUT_CCRIT_TOPODATE},
  {NULL, TT_LAYOUT_CCRIT_UNKNOWN}
};



class thlayout_copy_src {

  public:
  
  const char * srcn;
  thlayout * srcptr;
  class thlayout_copy_src * next_src;
  thlayout_copy_src () : srcn(NULL), srcptr(NULL), next_src(NULL) {};
  
};


/**
 * layout command options parsing table.
 */
 
static const thstok thtt_layout_opt[] = {
  {"base-scale",TT_LAYOUT_BASE_SCALE},
  {"code",TT_LAYOUT_CODE},
  {"color",TT_LAYOUT_COLOR},
  {"color-legend",TT_LAYOUT_COLOR_LEGEND},
  {"color-model", TT_LAYOUT_COLOR_MODEL},
  {"color-profile", TT_LAYOUT_COLOR_PROFILE},
  {"colour",TT_LAYOUT_COLOR},
  {"colour-legend",TT_LAYOUT_COLOR_LEGEND},
  {"colour-model", TT_LAYOUT_COLOR_MODEL},
  {"colour-profile", TT_LAYOUT_COLOR_PROFILE},
  {"copy",TT_LAYOUT_COPY},
  {"debug",TT_LAYOUT_DEBUG},
  {"doc-author",TT_LAYOUT_DOC_AUTHOR},
  {"doc-keywords",TT_LAYOUT_DOC_KEYWORDS},
  {"doc-subject",TT_LAYOUT_DOC_SUBJECT},
  {"doc-title",TT_LAYOUT_DOC_TITLE},
  {"endcode",TT_LAYOUT_ENDCODE},
  {"exclude-pages",TT_LAYOUT_EXCLUDE_PAGES},
  {"fonts-setup", TT_LAYOUT_FONT_SETUP},
  {"grid",TT_LAYOUT_GRID},
  {"grid-coords",TT_LAYOUT_GRID_COORDS},
  {"grid-origin",TT_LAYOUT_GRID_ORIGIN},
  {"grid-size",TT_LAYOUT_GRID_SIZE},
  {"language",TT_LAYOUT_LANG},
  {"layers",TT_LAYOUT_LAYERS},
  {"legend",TT_LAYOUT_LEGEND},
  {"legend-columns",TT_LAYOUT_LEGEND_COLUMNS},
  {"legend-width",TT_LAYOUT_LEGEND_WIDTH},
  {"map-comment",TT_LAYOUT_DOC_COMMENT},
  {"map-header",TT_LAYOUT_MAP_HEADER},
  {"map-header-bg",TT_LAYOUT_MAP_HEADER_BG},
  {"map-image",TT_LAYOUT_MAP_IMAGE},
  {"min-symbol-scale", TT_LAYOUT_MIN_SYMBOL_SCALE},
  {"nav-factor",TT_LAYOUT_NAV_FACTOR},
  {"nav-size",TT_LAYOUT_NAV_SIZE},
  {"north",TT_LAYOUT_NORTH},
  {"opacity",TT_LAYOUT_OPACITY},
  {"origin",TT_LAYOUT_ORIGIN},
  {"origin-label",TT_LAYOUT_ORIGIN_LABEL},
  {"overlap",TT_LAYOUT_OVERLAP},
  {"own-pages",TT_LAYOUT_OWN_PAGES},
  {"page-grid",TT_LAYOUT_PAGE_GRID},
  {"page-numbers",TT_LAYOUT_PAGE_NUMBERS},
  {"page-setup",TT_LAYOUT_PAGE_SETUP},
  {"rotate",TT_LAYOUT_ROTATE},
  {"scale", TT_LAYOUT_SCALE},
  {"scale-bar", TT_LAYOUT_SCALE_BAR},
  {"size", TT_LAYOUT_SIZE},
  {"sketches", TT_LAYOUT_SKETCHES},
  {"smooth-shading",TT_LAYOUT_SMOOTH_SHADING},
  {"statistics",TT_LAYOUT_MAP_ITEM},
  {"surface",TT_LAYOUT_SURFACE},
  {"surface-opacity",TT_LAYOUT_SURFACE_OPACITY},
  {"survey-level",TT_LAYOUT_SURVEY_LEVEL},
  {"symbol-assign", TT_LAYOUT_SYMBOL_ASSIGN},
  {"symbol-color", TT_LAYOUT_SYMBOL_COLOR},
  {"symbol-colour", TT_LAYOUT_SYMBOL_COLOR},
  {"symbol-hide", TT_LAYOUT_SYMBOL_HIDE},
  {"symbol-set", TT_LAYOUT_SYMBOL_DEFAULTS},
  {"symbol-show", TT_LAYOUT_SYMBOL_SHOW},
  {"title-pages",TT_LAYOUT_TITLE_PAGES},
  {"transparency",TT_LAYOUT_TRANSPARENCY},
  {"units",TT_LAYOUT_UNITS},
  {NULL, TT_LAYOUT_UNKNOWN},
};



struct thlayout_map_image {
  double m_x, m_y;
  const char * m_fn;
  int m_align;
  thlayout_map_image() : m_x(0.0), m_y(0.0), m_fn(""), m_align(TT_LAYOUT_MAP_HEADER_OFF) {}
  bool defined();
  void parse(char ** args, const char * cpath);
};

typedef std::list<thlayout_map_image> thlayout_map_image_list;


/**
 * layout class.
 */

class thlayout : public thdataobject {

  public:

  class thconfig * m_pconfig;

  class thlookup * m_lookup;
    
  double scale, scale_bar, base_scale, ox, oy, oz, hsize, vsize, paphs, papvs, paghs, pagvs, marls, marts, gxs, gys, gzs, gox, goy, goz, navf, overlap, opacity,
    map_header_x, map_header_y, legend_width, surface_opacity, rotate;
  
  int o_cs, go_cs;

  const char * olx, * oly, 
    * doc_title, * doc_comment, * doc_author, * doc_subject, * doc_keywords, * excl_list;
  
  unsigned navsx, navsy, ownp, legend_columns;
  
  char grid, ccode;
  
  int legend, color_legend, map_header, lang, north, max_explos, max_topos, max_cartos, max_copys,
    debug, survey_level, surface, grid_coords, color_model, smooth_shading;

  const char * color_profile_rgb, * color_profile_cmyk, * color_profile_gray;
  
  thlayout_color color_map_bg, color_map_fg, color_preview_below, color_preview_above;
  int color_crit; // none, altitude, ...
  const char * color_crit_fname;

  double min_symbol_scale, font_setup[5];

  
  thlayoutln * first_line, * last_line;
  
  bool titlep, transparency, layers, pgsnum, lock, excl_pages, page_grid, 
    map_header_bg, sketches, color_labels;

  int explo_lens, topo_lens, carto_lens, copy_lens;
		
	thlocale units;

  thlayout_map_image_list map_image_list;
  
  int def_grid_size, def_grid_origin, def_nav_factor, def_nav_size, 
    def_opacity, def_transparency, def_layers, def_base_scale,
    def_rotate, def_sketches, def_north,
    def_origin, def_origin_label, def_overlap, def_own_pages,
    def_page_numbers, def_page_setup, def_scale, def_size, def_title_pages,
    def_tex_lines, def_doc_title, def_doc_comment, def_doc_author, def_doc_subject,
    def_doc_keywords, def_excl_pages, def_grid, def_page_grid,
    def_legend, def_color_legend, def_legend_width, def_legend_columns, 
    def_map_header, def_lang, def_scale_bar, def_map_header_bg,
    def_max_explos, def_max_topos, def_max_cartos,
    def_max_copys, def_explo_lens, def_topo_lens, def_debug, def_survey_level, def_surface,
    def_surface_opacity, def_units, def_grid_coords, def_color_labels,
    def_font_setup, def_min_symbol_scale, def_color_model, def_carto_lens, def_copy_lens,
	def_color_profile_rgb, def_color_profile_cmyk, def_color_profile_gray, def_smooth_shading;
    
  
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
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thlayout";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  bool is(int class_id) override;
  
  
  /**
   * Return number of command arguments.
   */
   
  int get_cmd_nargs() override;
  
  
  /**
   * Return command name.
   */
   
  const char * get_cmd_name() override;
  
  
  /**
   * Return command end option.
   */
   
  const char * get_cmd_end() override;
  
  
  /**
   * Return option description.
   */
   
  thcmd_option_desc get_cmd_option_desc(const char * opts) override;
  thcmd_option_desc get_default_cod(int id);
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments array.
   * @param argenc Arguments encoding.
   */
   
  void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline) override;


  /**
   * Print object properties.
   */
   
  void self_print_properties(FILE * outf) override; 
  

  /**
   * Print object in C++ format.
   */
   
  void self_print_library(); 
  

  /**
   * Get context for object.
   */
   
  int get_context() override;
  
  /**
   * Parse option with 3 or 2 or 1 numbers and units.
   */
   
  void parse_len(double & d1, double & d2, double & d3, int nargs, char ** args, int nonneg = -1);
  void parse_len6(double & d1, double & d2, double & d3, double & d4, double & d5, double & d6, int nargs, char ** args, int nonneg = -1);


  /**
   * Export configuration file.
   */
   
  void export_config(FILE * o, thdb2dprj * prj, double x_scale, double x_origin_shx, double x_origin_shy);
  

  /**
   * Export font size.
   */
  void export_mptex_font_size(FILE * o, class th2ddataobject * obj, bool print_default_scale);


  /**
   * Export main tex file.
   */
   
  void export_pdftex(FILE * o, thdb2dprj * prj, char mode);
  void export_mpost(FILE * o);
  void export_mpost_symbols(FILE * o, struct thsymbolset * symset);
  
  /**
   * Process copy.
   */
   
  void process_copy();
  
  bool is_debug_stations();
  bool is_debug_joins();
  bool is_debug_scrapnames();
  bool is_debug_stationnames();

  
  /**
   * set LAYOUT variable.
   */
   
  void set_thpdf_layout(thdb2dprj * prj, double x_scale, double x_origin_shx, double x_origin_shy);

  /**
   * Convert all points in object.
   */

  void convert_all_cs() override;
   

};

/**
 * Copy src list.
 */
 
extern std::list <thlayout_copy_src> thlayout_copy_src_list;

#endif


