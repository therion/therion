/**
 * @file thattr.h
 * Generic object attribute structure.
 */
  
/* Copyright (C) 2007 Stacho Mudrak
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
 
#ifndef thattr_h
#define thattr_h

#include <list>
#include <map>
#include <string>
#include <cstdio>
#include "thmbuffer.h"
#include "thchenc.h"

enum {
  THATTR_NA,
  THATTR_STRING,
  THATTR_DOUBLE,
  THATTR_INTEGER,
};


/**
 * Single attribute structure.
 */
 
struct thattr_attr {
  std::string m_val_string;  ///< String value.
  double m_val_double;  ///< Double value.
  long m_val_long;  ///< Integer value.
  int m_type;  ///< Attribute type.
  
  struct thattr_field * m_field;  ///< Attribute's field.
  struct thattr_obj * m_obj;  ///< Attribute's object.
};

typedef std::map<size_t, thattr_attr> thattr_id2attr_map;


/**
 * Field structure.
 */

struct thattr_field {
  std::string m_name;  ///< Field name.
  size_t m_id;  ///< Internal field identifier.

  struct thattr * m_parent;  ///< Parent attribute class.

  int m_type;  ///< Generic export type.

  bool m_present;  ///< Whether there is any value present.

  long m_mini,  ///< Minimum integer value.
    m_maxi; ///< Maximum integer value.

  double m_mind,  ///< Minimum double value.
    m_maxd;  ///< Maximum double value.

  size_t m_maxs;  ///< Maximum string length.

  int m_xdbf_width,  ///< DBF field width.
    m_xdbf_decimals,  ///< DBF field decimals.
    m_xdbf_field;  ///< DBF field identifier.
  std::string m_xdbf_name;  ///< DBF field name.

  std::string m_xmp_name,  ///< Metapost field name.
    m_xmp_last_string;  ///< Last string value exported for given field.
  bool m_xmp_numeric,  ///< Whether MP field is numeric.
    m_xmp_transform;  ///< Whether MP field is transformed.
  double m_xmp_last_numeric;  ///< Last numeric value exported for given field.

};

typedef std::map<std::string, thattr_field*> thattr_name2field_map;
typedef std::list<thattr_field> thattr_field_list;


/**
 * Object structure.
 */
 
struct thattr_obj {
  long m_user_id;  ///< User defined ID.
  size_t m_id;  ///< Internal ID.
  void * m_data;  ///< User defined data.
  thattr_id2attr_map m_attributes;  ///< Map of attributes.

  struct thattr * m_parent;  ///< Parent attribute class.

  thattr_obj() : m_user_id(-1), m_data(NULL) {}
};

typedef std::map<size_t, thattr_obj*> thattr_id2obj_map;
typedef std::map<long, thattr_obj*> thattr_usrid2obj_map;
typedef std::list<thattr_obj> thattr_obj_list;


/**
 * Generic object attribute structure.
 */
 
struct thattr {

  size_t m_num_fields,  ///< Number of fields.
    m_num_objects;  ///< Number of objects.

  thattr_obj_list m_obj_list;  ///< List of objects.
  thattr_id2obj_map m_obj_map;  ///< ID -> Object map.
  thattr_usrid2obj_map m_obj_usrmap;  ///< User ID -> Object map.

  thattr_field_list m_field_list;  ///< List of all fields.
  thattr_name2field_map m_field_map;  ///< Name -> Field map.

  thattr();


  thattr_field * insert_field(const char * name);
  thattr_field * get_field(const char * name, bool ins = false);

  thattr_obj * m_obj_last;  ///< Last inserted object.
  thattr_obj * insert_object(void * data, long user_id = -1);
  thattr_obj * get_object(long user_id);
  
  void insert_attribute(const char * name, thattr_attr & attr, long user_id = -1);
  void insert_attribute(const char * name, const char * value, long user_id = -1);
  void insert_attribute(const char * name, double value, long user_id = -1);
  void insert_attribute(const char * name, long value, long user_id = -1);
  void copy_attributes(thattr_obj * object, long user_id = -1);

  void analyze_fields();
  void export_dbf(const char * fname, int encoding = TT_UTF_8);
  void export_mp_header(FILE * f);
  void export_mp_object(FILE * f, long user_id);

};


#endif




