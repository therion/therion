/**
 * @file thexpmap.h
 * Export class.
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
 
#ifndef thexpmap_h
#define thexpmap_h


#include "thexport.h"


/**
 * map export options.
 */

enum {
  TT_EXPMAP_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPMAP_OPT_FORMAT,  ///< Output option.
  TT_EXPMAP_OPT_PROJECTION,  ///< Output projection.
  TT_EXPMAP_OPT_LAYOUT,  ///< Layout.
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_expmap_opt[] = {
  {"-fmt", TT_EXPMAP_OPT_FORMAT},
  {"-format", TT_EXPMAP_OPT_FORMAT},
  {"-layout", TT_EXPMAP_OPT_LAYOUT},
  {"-proj", TT_EXPMAP_OPT_PROJECTION},
  {"-projection", TT_EXPMAP_OPT_PROJECTION},
  {NULL, TT_EXPMAP_OPT_UNKNOWN}
};



/**
 * map export formats.
 */

enum {
  TT_EXPMAP_FMT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPMAP_FMT_SVG,  ///< SVG
  TT_EXPMAP_FMT_PDF,  ///< PDF
};


/**
 * Format parsing table.
 */
 
static const thstok thtt_expmap_fmt[] = {
  {"pdf", TT_EXPMAP_FMT_PDF},
  //{"svg", TT_EXPMAP_FMT_SVG},
  {NULL, TT_EXPMAP_FMT_UNKNOWN}
};



/**
 * Scrap mp export struct.
 */

class thexpmapmpxs {

  public:
  
  FILE * file; ///< output file
  double ms, mx, my; ///< Scale and centering.
  
  thexpmapmpxs() : file(NULL), ms(1.0), mx(0.0), my(0.0) {}
  
};

/**
 * Main export class.
 */
 
class thexpmap : public thexport {


  public:
  
  friend class thexporter;
  
  int format;  ///< Output format.
  char * projstr,  ///< Projection string.
    * layoutstr;  ///< Layout string.
  class thdb2dprj * projptr;  ///< Projection pointer.
  class thlayout * layout;  ///< Layout pointer.
  thbuffer layoutopts;  ///< Layout options buffer.
  
  void export_pdf(class thdb2dxm * maps, class thdb2dprj * prj);
  unsigned export_mp(thexpmapmpxs * out, class thscrap * scrap, unsigned startnum, bool outline_mode);
  

  public:
  
  thexpmap(); ///< Default constructor.

  virtual ~thexpmap(); ///< Default destructor.

  /**
   * Parse map export options.
   */
   
  virtual void parse_options(int & argx, int nargs, char ** args);

  /**
   * Parse layout options.
   */
   
  void parse_layout_option(int & argx, int nargs, char ** args);

  
  /**
   * Dump object into file.
   */
   
  virtual void dump_body(FILE * xf);


  /**
   * Dump object into file.
   */
   
  virtual void dump_header(FILE * xf);
  

  /**
   * Make export.
   */
   
  virtual void process_db(class thdatabase * dbp);


  /**
   * Return default format.
   */
   
  int get_default_format();


  /**
   * Parse export projection.
   */
   
  void parse_projection(class thdatabase * dbp);  
  
};


/**
 * Quick map export scale.
 */
 
extern double thexpmap_quick_map_export_scale;


#endif


