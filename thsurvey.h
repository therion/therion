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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thsurvey_h
#define thsurvey_h

#include "thdataobject.h"
#include "thtfpwf.h"
#include "thperson.h"
#include "thdata.h"
#include "thbuffer.h"
#include <map>
#include <memory>

/**
 * survey command options tokens.
 */
 
enum {
  TT_SURVEY_UNKNOWN = 2000,
  TT_SURVEY_DECLINATION = 2001,
  TT_SURVEY_PERSON_RENAME = 2002,
  TT_SURVEY_NAMESPACE = 2003,
  TT_SURVEY_ENTRANCE = 2004,
};


/**
 * survey command options parsing table.
 */
 
static const thstok thtt_survey_opt[] = {
  {"declination", TT_SURVEY_DECLINATION},
  {"entrance", TT_SURVEY_ENTRANCE},
  {"namespace", TT_SURVEY_NAMESPACE},
  {"person-rename", TT_SURVEY_PERSON_RENAME},
  {NULL, TT_SURVEY_UNKNOWN},
};


typedef std::map<thperson,thperson> thsurveyp2pmap;


/**
 * Survey statistics class.
 */
 
class thsurveystat {
  
  public:
  
  double length, length_duplicate, length_splay, length_surface, 
    length_approx, length_explored;
  
  class thdb1ds * station_top, * station_bottom,
    * station_east, * station_west, * station_north, * station_south;
  
  int station_state; ///< 0 - undefined, 1 - surface, 2 - underground
    
  unsigned long num_stations, num_shots, num_entrances;
  
  thsurveystat() : length(0.0), length_duplicate(0.0), length_splay(0.0),
    length_surface(0.0), length_approx(0.0), length_explored(0.0),
    station_top(NULL), station_bottom(NULL), station_east(NULL), 
    station_west(NULL), station_north(NULL), station_south(NULL),
    station_state(0), num_stations(0), num_shots(0), num_entrances(0) {}
  
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
  
  const char * full_name;  ///< Full survey name.
  thbuffer reverse_full_name;  ///< Reverse full name - for sorting.
  
  thdataobject * foptr,  ///< First object in survey.
    * loptr;  ///< Last object in survey.

  unsigned level;
    
  std::unique_ptr<class thdata> tmp_data_holder;
  thdata * data;
  
  void parse_declination(char * str);
  
  void full_name_reverse();

  bool decdef;  ///< Check whether declination is defined.
  thtfpwf declin;  ///< Return the declination pwf function.
  double decuds;  ///< Declination for undated surveys.
  
  long num1;  ///< Number of something.
  
  thsurveystat stat;  ///< Survey statistics.
  
  thsurveyp2pmap person_renames;

  thobjectname entrance;  ///< Entrance station name.

  bool privatens;
  thsurvey * surveyns;

    
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
  
  int get_class_id() override;
  
  
  /**
   * Return true, if son of given class.
   */
  
  bool is(int class_id) override;
  
  
  /**
   * Return number of command arguments.
   */
   
  int get_cmd_nargs() override;
  
  
  /**
   * Return command end option.
   */
   
  const char * get_cmd_end() override;
  
  
  /**
   * Return command name.
   */
   
  const char * get_cmd_name() override;
  
  
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
   * Get context for object.
   */
   
  int get_context() override;

  
  /**
   * Return class name.
   */
   
  const char * get_class_name() override {return "thsurvey";};


  /**
   * Return full survey name.
   */
   
   virtual const char * get_full_name();
  

  /**
   * Return reverse full survey name.
   */
   
   virtual const char * get_reverse_full_name() {return this->reverse_full_name;}
  

  /**
   * Print object contents into file.
   */
   
  void self_print_properties(FILE * outf) override;
  
  
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


  /**
   * Return namespace survey.
   */

  thsurvey * get_nss();

  
  /**
   * Get entrance station.
   */

  thobjectname get_entrance();


};


#endif


