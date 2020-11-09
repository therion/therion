/**
 * @file thlookup.h
 * Map / model color lookup module.
 */
  
/* Copyright (C) 2012 Stacho Mudrak
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
 
#ifndef thlookup_h
#define thlookup_h


#include "thdataobject.h"
#include "thlocale.h"
#include "thmapstat.h"
#include <list>

/**
 * lookup command options tokens.
 */
 
enum {
  TT_LOOKUP_UNKNOWN = 2000,
  TT_LOOKUP_TITLE = 2001,
};


/**
 * Lookup command options parsing table.
 */
 
static const thstok thtt_lookup_opt[] = {
  {"title", TT_LOOKUP_TITLE},
  {NULL, TT_LOOKUP_UNKNOWN},
};

/**
 * Lookup table row.
 */

struct thlookup_table_row {
  double m_valueDbl, m_valueDblFrom;
  thdate m_valueDate, m_valueDateFrom;
  const char * m_valueString;
  class thdataobject * m_ref;
  const char * m_label;
  thlayout_color m_color;
  thlookup_table_row() : m_valueDbl(thnan), m_valueDblFrom(thnan), m_valueString(""), m_ref(NULL), m_label("") {}
  void parse(class thlookup * lkp, char * args);
};

typedef std::list<thlookup_table_row> thlookup_table_list;


/**
 * Parse lookup name,.
 */

void thlookup_parse_reference(const char * arg, int * type, const char ** index, const char ** nname);

/**
 * lookup class.
 */

class thlookup : public thdataobject {

  public:

  int m_type;
  bool m_intervals, m_ascending;
  thlookup_table_list m_table;
  const char * m_title;
  thmapstat m_autoStat;

  /**
   * Standard constructor.
   */
  
  thlookup();
  
  
  /**
   * Standard destructor.
   */
   
  ~thlookup();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thlookup";};
  
  
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
  thcmd_option_desc get_default_cod(int id);
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments array.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Delete this object.
   *
   * @warn Always use this method instead of delete function.
   */
   
  virtual void self_delete();


  /**
   * Get context for object.
   */
   
  virtual int get_context();

  /**
   * Set scrap color.
   */

  virtual void color_scrap(thscrap * s);
  
  /**
   * Postprocess lookup.
   */

  virtual void postprocess_object_references();

  /**
   * Process scrap statistics.
   */

  virtual void add_auto_item(class thdataobject * o, thlayout_color c);

  /**
   * Process scrap statistics.
   */

  virtual void scan_map(class thmap * m);

  /**
   * Generate automatic legend.
   */

  virtual void auto_generate_items();

  /**
   * Generate automatic legend.
   */

  virtual void postprocess();

  /**
   * Export color legend, if applicable.
   */

  virtual void export_color_legend(thlayout * layout, std::unique_ptr<thlookup> lookup_holder);

};

#endif


