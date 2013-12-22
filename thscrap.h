/**
 * @file thscrap.h
 * Basic unit of 2D map.
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
 
#ifndef thscrap_h
#define thscrap_h


#include "thdataobject.h"
#include "thdb2dpt.h"
#include "thdb2dcp.h"
#include "thscraplo.h"
#include "thscraplp.h"
#include "thscrapen.h"
#include "thmapstat.h"
#include "thdb3d.h"
#include "thsketch.h"
#include "thtrans.h"

/**
 * scrap command options tokens.
 */
 
enum {
  TT_SCRAP_UNKNOWN = 2000,
  TT_SCRAP_PROJECTION = 2001,
  TT_SCRAP_SCALE = 2002,
  TT_SCRAP_STATIONS = 2003,
  TT_SCRAP_3D = 2004,
  TT_SCRAP_FLIP = 2005,
  TT_SCRAP_SKETCH = 2006,
};

enum {
  TT_SCRAP_FLIP_UNKNOWN,
  TT_SCRAP_FLIP_NONE,
  TT_SCRAP_FLIP_HORIZ,
  TT_SCRAP_FLIP_VERT,
};


/**
 * scrap command options parsing table.
 */
 
static const thstok thtt_scrap_opt[] = {
  {"flip", TT_SCRAP_FLIP},
  {"proj", TT_SCRAP_PROJECTION},
  {"projection", TT_SCRAP_PROJECTION},
  {"scale", TT_SCRAP_SCALE},
  {"sketch", TT_SCRAP_SKETCH},
  {"stations", TT_SCRAP_STATIONS},
  {"walls", TT_SCRAP_3D},
  {NULL, TT_SCRAP_UNKNOWN},
};


/**
 * scrap class.
 */

class thscrap : public thdataobject {

  public:
  
  friend class thdatabase;
  friend class thdb2d;

  class th2ddataobject * fs2doptr,  ///< First scrap 2D object.
    * ls2doptr;  ///< Last scrap 2D object.
    
  double lxmin, lxmax, lymin, lymax, ///< Coordinate limits.
    R, G, B; ///< Scrap color.
    
  thdb2dcp * fcpp, * lcpp; ///< Scrap control points.
  unsigned int ncp; ///< Number of control points.

  class thsurvey * centerline_survey;
  bool centerline_io;
      
  thdb2dprj * proj; ///< Scrap projection.
  class thprjx_scrap * xscrap;  ///< Extended scrap link.
  thscrap * proj_next_scrap, ///< Next scrap in projection.
    * proj_prev_scrap; ///< Prev next projection.
  
  double scale, scale_r1x, scale_r1y, scale_p1x, scale_p1y,
    scale_r2x, scale_r2y, scale_p2x, scale_p2y;  ///< Scrap scale.
  bool scale_p9; ///< 9 parameters scaling
  
  double mx, my, mxx, mxy, myx, myy, mr, ms;  ///< Calibration coeficients.
  
  double maxdist, avdist;
  thdb2dpt * maxdistp1, * maxdistp2;
  
  thscraplo * outline_first;
  bool outline_parsed, polygon_parsed, ends_parsed, d3_parsed, exported;
  thscraplp * polygon_first, * polygon_last;
  thscrapen * ends_first;
  
  thmapstat_datamap adata;
  thmorph2trans morph;
  
  int d3, flip;
  thdb3ddata d3_outline;
  
  void parse_scale(char * ss);
  void parse_stations(char * ss);
  void process_3d();
  thscraplp * polygon_insert(thscraplp * before = NULL);
  
  thdb1ds * get_nearest_station(thdb2dpt * pt);

  thsketch_list sketch_list;
  
  double z, a;
    
  public:

  /**
   * Standard constructor.
   */
  
  thscrap();
  
  
  /**
   * Standard destructor.
   */
   
  ~thscrap();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thscrap";};
  
  
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
   
  virtual const char * get_cmd_name();
  
  
  /**
   * Return command end option.
   */
   
  virtual const char * get_cmd_end();
  
  
  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(const char * opts);
  
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments arry.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Parse sketch option.
   */

  void parse_sketch(char ** args, int argenc);


  /**
   * Delete this object.
   *
   * @warn Always use this methos instead of delete function.
   */
   
  virtual void self_delete();


  /**
   * Create a sketch linked to current scrap.
   */

  void insert_sketch(char ** args);


  /**
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  
  
  /**
   * Insert control point to scrap.
   */
  
  thdb2dcp * insert_control_point();
  
  
  /**
   * Reset coordinates limits.
   */
   
  void reset_limits();
  
  /**
   * Reset transformation.
   */
   
  void reset_transformation();
  
  
  /**
   * Return outline. Process if necessary.
   */
   
  thscraplo * get_outline();
  
  
  /**
   * Return polygon. Process if necessary.
   */
 
  thscraplp * get_polygon();


  /**
   * Return polygon. Process if necessary.
   */
 
  thscrapen * get_ends();

  /**
   * Calculate scrap z.
   */
     
  void calc_z();
  
  
  /**
   * Insert associated data from given station.
   */
   
 void insert_adata(class thdb1ds * as);
 
 
 /**
  * Return 3D data. Process if necessary.
  */
  
 thdb3ddata * get_3d_outline();
 
 void update_limits(double x, double y);

 virtual void start_insert();


};


#endif


