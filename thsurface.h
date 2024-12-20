/**
 * @file thsurface.h
 * surface module.
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
 
#ifndef thsurface_h
#define thsurface_h

#include "thdb3d.h"
#include "thdataobject.h"
#include "thobjectname.h"
#include "thtflength.h"

#include <memory>

/**
 * surface command options tokens.
 */
 
enum {
  TT_SURFACE_UNKNOWN = 2000,
  TT_SURFACE_PICTURE = 2001,
  TT_SURFACE_GRID = 2002,
  TT_SURFACE_GRID_UNITS = 2003,
  TT_SURFACE_GRID_FLIP = 2004,
};


/**
 * surface command options parsing table.
 */
 
static const thstok thtt_surface_opt[] = {
  {"bitmap", TT_SURFACE_PICTURE},
  {"grid", TT_SURFACE_GRID},
  {"grid-flip", TT_SURFACE_GRID_FLIP},
  {"grid-units", TT_SURFACE_GRID_UNITS},
  {NULL, TT_SURFACE_UNKNOWN},
};


/**
 * surface class.
 */

class thsurface : public thdataobject {

  public:

  // insert here real properties
  const char * pict_name;
  bool pict_stations;
  thobjectname s1, s2;
  class thsurvey * ssurvey = nullptr;
  double pict_X1, pict_Y1, pict_X2, pict_Y2,
    pict_x1, pict_y1, pict_x2, pict_y2,
    grid_ox, grid_oy, grid_dx, grid_dy, grid_dxx, grid_dxy, grid_dyx, grid_dyy,
    calib_x, calib_y, calib_s, calib_r, calib_xx, calib_yy, calib_xy, calib_yx,
    pict_dpi = 0.0, pict_height = 0.0, pict_width = 0.0;
  long grid_nx, grid_ny, grid_counter, grid_size;
  int grid_flip, pict_type;
  thtflength grid_units;
  std::unique_ptr<double[]> grid;
  thdb3ddata d3d;
  bool d3dok;
  int pict_cs, grid_cs;

  void parse_grid(char * spec);
  void parse_grid_setup(char ** args);
  void parse_picture(char ** args);
  void calibrate();  

  /**
   * Standard constructor.
   */
  
  thsurface();
  
  
  /**
   * Return class identifier.
   */
  
  int get_class_id() override;
  
  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thsurface";};
  
  
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
   * check stations.
   */
 
  void check_stations();

  void start_insert() override;
  
  thdb3ddata * get_3d();

  /**
   * Convert all points in object.
   */

  void convert_all_cs() override;


};


#endif


