/**
 * @file thscan.h
 * 3D scan module.
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
 
#pragma once

#include "thdataleg.h"
#include "thdataobject.h"
#include "thdb3d.h"
#include "thobjectname.h"
#include "thtflength.h"

#include <memory>
#include <string>

/**
 * scan command options tokens.
 */
enum {
  TT_SCAN_UNKNOWN = 2000,
  TT_SCAN_FILE = 2001,
  TT_SCAN_AXES = 2002,
  TT_SCAN_UNITS = 2003,
};

/**
 * scan command options parsing table.
 */
static const thstok thtt_scan_opt[] = {
  {"axes", TT_SCAN_AXES},
  {"file", TT_SCAN_FILE},
  {"units", TT_SCAN_UNITS},
  {NULL, TT_SCAN_UNKNOWN},
};

/**
 * scan class.
 */
class thscan : public thdataobject {

public:
  // insert here real properties
  std::string datasrc;  //< Geometry source.
  thdatafix_list calib; //< Stations calibration.
  thtflength units; //< Source units.

  int datasrc_cs; //< Source CS
  int datasrc_coords[3] = {}; //< Source coordinates order.

  thdb3ddata d3d; //< Output model.
  bool d3dok; //< Output data.

  void transform_coords(double & x, double & y, double & z); ///< Transform coordinate order.
  void parse_data_source(char ** args); ///< Parse source.
  void parse_data_source_coords(char ** args); ///< Parse source coordinates order.
  void parse_calib(int nargs, char ** args);  ///< Add fixed station to data.

  /**
   * Constructor.
   */
  thscan();
  
  /**
   * Return class identifier.
   */
  int get_class_id() override;
  
  /**
   * Return class name.
   */
  const char * get_class_name() override {return "thscan";};
  
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

  //void start_insert() override;
  
  thdb3ddata * get_3d();

  /**
   * Convert all points in object.
   */
  void convert_all_cs() override;
};
