/**
 * @file thsurvey.h
 * Survey module.
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
 
#ifndef thsurvey_h
#define thsurvey_h

#include "thdataobject.h"
#include "thtfpwf.h"
#include "thperson.h"
#include <map>

/**
 * survey command options tokens.
 */
 
enum {
  TT_SURVEY_UNKNOWN = 2000,
  TT_SURVEY_DECLINATION = 2001,
  TT_SURVEY_PERSON_RENAME = 2002,
};


/**
 * survey command options parsing table.
 */
 
static const thstok thtt_survey_opt[] = {
  {"declination", TT_SURVEY_DECLINATION},
  {"person-rename", TT_SURVEY_PERSON_RENAME},
  {NULL, TT_SURVEY_UNKNOWN},
};


typedef std::map<thperson,thperson> thsurveyp2pmap;


/**
 * Survey statistics class.
 */
 
class thsurveystat {
  
  public:
  
  double length, length_duplicate, length_surface;
  
  class thdb1ds * station_top, * station_bottom,
    * station_east, * station_west, * station_north, * station_south;
  
  int station_state; ///< 0 - undefined, 1 - surface, 2 - underground
    
  unsigned long num_stations, num_shots;
  
  thsurveystat() : length(0.0), length_duplicate(0.0), length_surface(0.0),
    station_top(NULL), station_bottom(NULL), station_east(NULL), 
    station_west(NULL), station_north(NULL), station_south(NULL),
    station_state(0), num_stations(0), num_shots(0) {}
  
};



/**
 * Survey class.
 *
 * Main data structure object.
 */

class thsurvey : public thdataobject {

  public:
  
  friend class thdatabase;
  friend class thdataobject;
  friend class thdb1d;
  
  char * full_name,  ///< Full survey name.
       * reverse_full_name;  ///< Reverse full name - for sorting.
  
  thdataobject * foptr,  ///< First object in survey.
    * loptr;  ///< Last object in survey.
  
  void parse_declination(char * str);
  
  void full_name_reverse();

  bool decdef;  ///< Check whether declination is defined.
  thtfpwf declin;  ///< Return the declination pwf function.
  double decuds;  ///< Declination for undated surveys.
  
  long num1;  ///< Number of something.
  
  thsurveystat stat;  ///< Survey statistics.
  
  thsurveyp2pmap person_renames;
    
  public:

  /**
   * Standard constructor.
   */
  
  thsurvey();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~thsurvey();
  
  
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
   * Get context for object.
   */
   
  virtual int get_context();

  
  /**
   * Return class name.
   */
   
  virtual char * get_class_name() {return "thsurvey";};


  /**
   * Return full survey name.
   */
   
   virtual char * get_full_name();
  

  /**
   * Return reverse full survey name.
   */
   
   virtual char * get_reverse_full_name() {return this->reverse_full_name;}
  

  /**
   * Print object contents into file.
   */
   
  virtual void self_print_properties(FILE * outf);
  
  
  /**
   * Return true, if declination was specified.
   */
   
  bool get_decdef() { return this->decdef; }
  

  /**
   * Return the declination pwf function.
   */
   
  thtfpwf * get_declin() { return &(this->declin); }
  

  /**
   * Return true, if declination was specified.
   */
   
  double get_decuds() { return this->decuds; }
  
  
  /**
   * Get first object in survey.
   */
   
  thdataobject * get_first_survey_object() {return this->foptr;}
  
  
  /**
   * Get last object in survey.
   */
   
  thdataobject * get_last_survey_object() {return this->loptr;}
  
};


#endif


