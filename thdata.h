/**
 * @file thdata.h
 * Main survey data module.
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
 
#ifndef thdata_h
#define thdata_h

#include <set>

#include "thdataleg.h"
#include "thdataobject.h"
#include "thdate.h"
#include "thperson.h"
#include "thtfangle.h"
#include "thtflength.h"
#include "thtfpwf.h"

/**
 * data command options tokens.
 */
 
enum {
  TT_DATA_UNKNOWN = 2000,
  TT_DATA_DATE = 2001,
  TT_DATA_TEAM = 2002,
  TT_DATA_UNITS = 2003,
  TT_DATA_CALIBRATE = 2004,
  TT_DATA_DECLINATION = 2005,
  TT_DATA_INFER = 2006,
  TT_DATA_SD = 2007,
  TT_DATA_INSTRUMENT = 2008,
  TT_DATA_DATA = 2009,
  TT_DATA_BREAK = 2010,
  TT_DATA_MARK = 2011,
  TT_DATA_STATION = 2012,
  TT_DATA_FLAGS = 2013,
  TT_DATA_EQUATE = 2014,
  TT_DATA_FIX = 2015,
  TT_DATA_GRADE = 2016,
  TT_DATA_DISCOVERY_DATE = 2017,
  TT_DATA_DISCOVERY_TEAM = 2018,
  TT_DATA_GROUP = 2019,
  TT_DATA_ENDGROUP = 2020,
  TT_DATA_SHAPE = 2021,
  TT_DATA_VTRESH = 2022,
  TT_DATA_WALLS = 2023,
};


/**
 * data command options parsing table.
 */
 
static const thstok thtt_data_opt[] = {
  {"break", TT_DATA_BREAK},
  {"calibrate", TT_DATA_CALIBRATE},
  {"data", TT_DATA_DATA},
  {"date", TT_DATA_DATE},
  {"declination", TT_DATA_DECLINATION},
  {"endgroup", TT_DATA_ENDGROUP},
  {"equate", TT_DATA_EQUATE},
  {"explo-date", TT_DATA_DISCOVERY_DATE},
  {"explo-team", TT_DATA_DISCOVERY_TEAM},
  {"fix", TT_DATA_FIX},
  {"flags", TT_DATA_FLAGS},
  {"grade", TT_DATA_GRADE},
  {"group", TT_DATA_GROUP},
  {"infer", TT_DATA_INFER},
  {"instrument", TT_DATA_INSTRUMENT},
  {"mark", TT_DATA_MARK},
  {"sd", TT_DATA_SD},
//  {"shape", TT_DATA_SHAPE},
  {"station", TT_DATA_STATION},
  {"team", TT_DATA_TEAM},
  {"units", TT_DATA_UNITS},
  {"vtreshold", TT_DATA_VTRESH},
  {"walls", TT_DATA_WALLS},
  {NULL, TT_DATA_UNKNOWN},
};


/**
 * Team person's set type.
 */

typedef std::set <thperson> thdata_team_set_type;


/**
 * Maximal number of various data items.
 */
 
#define THDATA_MAX_ITEMS 22


/**
 * Main survey data class.
 */

class thdata : public thdataobject {

  public:
  
  thdata * cgroup, * ugroup;
  
  // dlu - data leg units
  thtflength dlu_length, dlu_counter, dlu_depth, dlu_dx, dlu_dy, dlu_dz,
    dlu_x, dlu_y, dlu_z, dlu_sdlength, dlu_up, dlu_down, dlu_left, dlu_right;
  thtfangle dlu_bearing, dlu_gradient, dlu_declination, dlu_sdangle;
  
  // dlc - data calibration
  thtfpwf dlc_length, dlc_gradient, dlc_bearing, dlc_counter, dlc_depth,
    dlc_dx, dlc_dy, dlc_dz, dlc_x, dlc_y, dlc_z, dlc_default,
    dlc_up, dlc_down, dlc_left, dlc_right;
    
  // dls - data standard deviation and declination
  double dls_length, dls_gradient, dls_bearing, dls_counter, dls_depth,
    dls_dx, dls_dy, dls_dz, dls_x, dls_y, dls_z, dl_declination,
    dl_survey_declination;
    
  // statistics  
  double stat_length, stat_dlength, stat_slength;
  class thdb1ds * stat_st_top, * stat_st_bottom;
  int stat_st_state;
    
  // dli - data leg infers
  bool dli_plumbs, dli_equates, dl_direction;
  
  // what is inserted
  bool di_station, di_from, di_to, di_length, di_bearing, di_gradient,
    di_backbearing, di_backgradient,
    di_depth, di_fromdepth, di_todepth, di_depthchange, di_count, di_fromcount,
    di_tocount, di_dx, di_dy, di_dz, di_direction, di_newline, di_interleaved,
    di_up, di_down, di_left, di_right;
  
  bool dl_survey_declination_on;
  
  int d_type, ///< Type of data.
    d_order[THDATA_MAX_ITEMS],  ///< Data order.
    d_nitems,  ///< Number of items.
    d_current,  ///< Currently inserted item.
    d_mark,  ///< Station mark type.
    d_shape, ///< Walls shape.
    d_walls, ///< Walls switch.
    d_flags,  ///< Leg flags.
    d_last_equate;  ///< Last data equate.
    
  double d_vtresh; ///< Vertical treshold.
    
  void reset_data_sd();  ///< Reset data sd
  
  void reset_data();  ///< Reset data type and order.
  
  thdataleg_list::iterator cd_leg, pd_leg;  // Current data leg.
  bool cd_leg_def, pd_leg_def;  // Whether these legs are defined.
  
  void set_data_calibration(int nargs, char ** args);  ///< Data calibration.
  
  void set_data_units(int nargs, char ** args);  ///< Set data units.
  
  void set_data_declination(int nargs, char ** args);  ///< Set data declination.
  
  void set_data_infer(int nargs, char ** args);  ///< Set infers.
  
  void set_data_instrument(int nargs, char ** args);  ///< Set instrument.
  
  void set_data_sd(int nargs, char ** args);  ///< Set data SD.

  void set_data_grade(int nargs, char ** args);  ///< Set data grade.

  void set_data_data(int nargs, char ** args);  ///< Set data type and order.

  void insert_data_leg(int nargs, char ** args);  ///< Insert data leg.
  
  void complete_interleaved_data();  ///< Complete data (if interleaved).

  void set_data_fix(int nargs, char ** args);  ///< Add fixed station to data.
  
  void set_data_equate(int nargs, char ** args);  ///< Add equivalency.
  
  void set_data_flags(int nargs, char ** args);  ///< Set data leg flags.
  
  void set_data_station(int nargs, char ** args, int argenc);  ///< Set station comment and flags.
	  
  void set_data_mark(int nargs, char ** args);  ///< Set type of station mark.

  void set_data_vtresh(int nargs, char ** args);  ///< Set vertical treshold

  void set_survey_declination();  ///< Set survey declination.
  
  void start_group();
  
  void end_group();

  public:

  thdate date,  ///< Survey date.
    discovery_date;  ///< Discovery date.
  
  thdata_team_set_type team_set,  ///< Team person set.
    discovery_team_set;  ///< Discovery team person set.
  
  thdataleg_list leg_list;  ///< Main data structure.

  thdatafix_list fix_list;  ///< Fix list.
  
	thdatamark_list mark_list; ///< List of station marks.
  
  thdataequate_list equate_list;  ///< Equate list.
  
  thdatass_list ss_list;  ///< Stations list.
  
  thstdims_list dims_list; ///< Dimensions list.
  
  /**
   * Standard constructor.
   */
  
  thdata();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thdata();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return true, if son of given class.
   */
  
  virtual bool is(int class_id);
  
  
  /**
   * Return number of command arguments.
   */
   
  virtual int get_cmd_nargs();
  
  
  /**
   * Return command end option.
   */
   
  virtual char * get_cmd_end();


  /**
   * Whether multiple ends.
   */
   
  virtual bool get_cmd_ends_state();


  /**
   * Whether cmd is end.
   */
   
  virtual bool get_cmd_ends_match(char * cmd);
  
  
  /**
   * Return command name.
   */
   
  virtual char * get_cmd_name();
  
  
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
   * Return class name.
   */
   
  virtual char * get_class_name() {return "thdata";};
  

  /**
   * Print object contents into file.
   */
   
  virtual void self_print_properties(FILE * outf);  


  /**
   * Called before insertion into database.
   */
   
  virtual void start_insert();
    
  
  /**
   * Complete dimensions.
   */
   
  void complete_dimensions();
  
};


#endif


