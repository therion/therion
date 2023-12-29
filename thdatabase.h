/**
 * @file thdatabase.h
 * Database module.
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
 
#ifndef thdatabase_h
#define thdatabase_h

#include <map>
#include <list>
#include <set>
#include <memory>

#include "thmbuffer.h"
#include "thbuffer.h"
#include "thdb1d.h"
#include "thdb2d.h"
#include "thobjectname.h"
#include "thobjectsrc.h"
#include "thattr.h"
#include <cstdio>

/**
 * Input contexts.
 */

enum {
  THCTX_NONE = 1,  ///< None
  THCTX_SURVEY = 2,  ///< Survey context
  THCTX_SCRAP = 4,  ///< Scrap context
};


/**
 * Option positions.
 */

enum {
  THOP_INLINE = 1,  ///< Whether option stands in separate line.
  THOP_CONFIGURE = 2,  ///< Whether option is set via configure command.
};



/**
 * Survey name class.
 */
 
class thsurveyname {  
  public:
  const char * n;  ///< Data.
  thsurveyname(): n("") {}  ///< Standard constructor.
  
  /**
   * Default constructor.
   *
   * @param n Survey name.
   */
   
  thsurveyname(const char * nn) : n(nn) {}
  

  /**
   * Comparison operator.
   */
   
  friend bool operator < (const class thsurveyname & n1,
    const class thsurveyname & n2);
    
};


/**
 * Revision class.
 */
 
class threvision {  
  
  public:
  
  unsigned long id,  ///< Object id.
    r;   ///< Revision.
  thobjectsrc srcf;  ///< File name.
  
  threvision(): id(0), r(0) {}  ///< Standard constructor.
  
  /**
   * Default constructor.
   *
   * @param rid Revision object id.
   * @param rr Revision number.
   */
   
  threvision(unsigned long rid, unsigned long rr)
      : id(rid), r(rr) {}

  /**
   * Default constructor.
   *
   * @param rid Revision object id.
   * @param rr Revision number.
   * @param rsrcf File name.
   */
   
  threvision(unsigned long rid, unsigned long rr, thobjectsrc rsrcf)
      : id(rid), r(rr), srcf(rsrcf) {}
  

  /**
   * Comparison operator.
   */
   
  friend bool operator < (const class threvision & r1,
    const class threvision & r2);
    
};


typedef std::set < thsurveyname > thdb_dictionary_type;  ///< Database dictionary.
typedef std::set < threvision > thdb_revision_set_type;  ///< Object revision set type.
typedef std::map < thobjectid, class thdataobject * > thdb_object_map_type;  ///< Object map
typedef std::map < thsurveyname, class thsurvey * > thdb_survey_map_type;   ///< Survey map  
typedef std::map < thsurveyname, class thdataobject * > thdb_grade_map_type;   ///< Grade map  
typedef thdb_grade_map_type thdb_layout_map_type;   ///< Layout map  
typedef std::list < std::unique_ptr<thdataobject> > thdb_object_list_type;   ///< Object list

  
/**
 * Main database class.
 *
 * Store all the data maintained by therion.
 */

class thdatabase {


  public:

  thmbuffer buff_strings;   ///< String storage buffer.
  
  
  int ccontext = {};  ///< Current context.
  class thsurvey * csurveyptr = {},  ///< Pointer to the current survey.
    * fsurveyptr = {};  ///< Pointer to the first survey.
  unsigned csurveylevel = {}; ///< Current survey level.
  class thscrap * cscrapptr = {}; ///< Current scrap.
  class th2ddataobject * lcscrapoptr = {}; ///< Last object in given current scrap.
  class thdataobject * lcsobjectptr = {};  ///< Last object in given current survey.

  thdb_dictionary_type dictionary;  ///< Database dictionary.
  
  unsigned long objid = {},  ///< Object identifier
    nscraps = {};  ///< Total number of scraps.
  
  void reset_context();  ///< Reset database context.

  public:
  
  thbuffer buff_enc;   ///< Encoding buffer.  
  thbuffer buff_tmp;  ///< Temporary buffer.
  thmbuffer mbuff_tmp, mbuff_tmp2;  ///< Temporary mbuffer.
  thmbuffer buff_stations;  ///< Stations storage buffer.
  thdb_revision_set_type revision_set;  ///< Object revisions.
  thdb1d db1d;  ///< Survey data part of database.
  thdb2d db2d;  ///< 2D data part of database.
  thattr attr;  ///< Object attributes.
  thobjectsrc csrc;  ///< Current source position.
  thdb_object_map_type object_map;  ///< Object search hash
  thdb_survey_map_type survey_map;   ///< Survey search hash
  thdb_grade_map_type grade_map;   ///< Grade search hash
  thdb_layout_map_type layout_map;   ///< Layout search hash
  thdb_layout_map_type lookup_map;   ///< Lookup search hash
  thdb_object_list_type object_list;   ///< Object list
  
  
  /**
   * Standard constructor.
   */
  
  thdatabase();
  
  
  /**
   * Clear the contents of the database.
   */
   
  void clear();
  
  
  /**
   * Create a data object linked to the database. Use this method when object
   * type is a runtime information, for example when parsing a file.
   * If the type is known at compile time, use create<T>(thobjectsrc).
   */
   
  std::unique_ptr<thdataobject> create(const char * oclass, thobjectsrc osrc);
  
  
  /**
   * Create a data object linked to the database.
   * @tparam type of the object
   * @param osrc source info
   * @return instance of T
   */
  template <typename T>
  std::unique_ptr<T> create(const thobjectsrc& osrc);
  
  
  /**
   * Revise object in database.
   */
   
  class thdataobject * revise(char * name, class thsurvey * fathersptr,
      thobjectsrc osrc);
  
  
  /**
   * Return pointer to current survey.
   */
   
  class thsurvey * get_current_survey();
  
  
  /**
   * Return pointer to current scrap.
   */
   
  class thscrap * get_current_scrap();
  
  
  /**
   * Insert data object into database.
   */
   
  void check_context(class thdataobject * optr);

  /**
   * Insert data object into database.
   */
   
  void insert(std::unique_ptr<thdataobject> unique_optr);
  
  
  /**
   * Insert survey grade object into database.
   */
   
  void insert_grade(std::unique_ptr<thdataobject> optr);


  /**
   * Insert map layout object into database.
   */
   
  void insert_layout(std::unique_ptr<thdataobject> optr);

  /**
   * Insert lookup object into database.
   */
   
  void insert_lookup(std::unique_ptr<thdataobject> optr);
  
  
  /**
   * Retrieve survey grade from database.
   */
   
  class thgrade * get_grade(char * gname);


  /**
   * Retrieve map layout from database.
   */
   
  class thlayout * get_layout(const char * gname);

  /**
   * Retrieve lookup from database.
   */
   
  class thlookup * get_lookup(const char * gname);
  
  
  /**
   * Store given string in the database.
   */
   
  const char * strstore(const char * src, bool use_dic = false);
  
  
  /**
   * End objects insertion.
   */
   
  void end_insert();
  
  
  /**
   * Print database contents into file.
   */
   
  void self_print(FILE * outf);
  

  /**
   * Insert a datastation into database.
   */
   
  bool insert_datastation(thobjectname on, thsurvey * ps);
  
  
  /**
   * Get object from database.
   */
   
  thdataobject * get_object(thobjectname on, thsurvey * ps);

  
  /**
   * Get survey from database.
   */
   
  thsurvey * get_survey(const char * sn, thsurvey * ps);
  thsurvey * get_survey_noexc(const char * sn, thsurvey * ps);
  thsurvey * get_exact_survey_noexc(const char * sn, thsurvey * ps);


  /**
   * Get survey id from database.
   */
   
  unsigned long get_survey_id(const char * sn, thsurvey * ps);
  

  /**
   * Print library from current database.
   */
 
  void self_print_library();
  
  /**
   * Process database references.
   */
   
  void preprocess();
  
  void insert_equate(int nargs, char ** args);

private:
  /**
   * @brief adds thdata if the object is thsurvey
   * @param obj new object
   * @param osrc source info
   */
  void add_data(thdataobject* obj, const thobjectsrc& osrc);
};

// Template definition must be available in a header file.
template <typename T>
std::unique_ptr<T> thdatabase::create(const thobjectsrc& osrc)
{
  static_assert(std::is_base_of<thdataobject, T>::value, "created object must be derived from thdataobject");

  auto ret = std::make_unique<T>();

  ret->assigndb(this);
  // set object id and mark revision
  ret->id = ++this->objid;
  this->attr.insert_object((long) ret->id);
  ret->source = osrc;
  this->revision_set.insert(threvision(ret->id, 0, osrc));

  // add thdata if the object is thsurvey
  add_data(ret.get(), osrc);

  return ret;
}

/**
 * Database module.
 */
 
extern thdatabase thdb;


#endif


