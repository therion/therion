/**
 * @file th2ddataobject.h
 * 2ddataobject module.
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
 
#ifndef th2ddataobject_h
#define th2ddataobject_h


#include "thdataobject.h"

/**
 * Point scale tokens.
 */

enum {
  TT_2DOBJ_SCALE_UNKNOWN,
  TT_2DOBJ_SCALE_NUMERIC,
  TT_2DOBJ_SCALE_XS,
  TT_2DOBJ_SCALE_S,
  TT_2DOBJ_SCALE_M,
  TT_2DOBJ_SCALE_L,
  TT_2DOBJ_SCALE_XL,
};


/**
 * Point scale parsing table.
 */
 
static const thstok thtt_2dobj_scales[] = {
  {"L", TT_2DOBJ_SCALE_L},
  {"M", TT_2DOBJ_SCALE_M},
  {"S", TT_2DOBJ_SCALE_S},
  {"XL", TT_2DOBJ_SCALE_XL},
  {"XS", TT_2DOBJ_SCALE_XS},
  {"huge", TT_2DOBJ_SCALE_XL},
  {"l", TT_2DOBJ_SCALE_L},
  {"large", TT_2DOBJ_SCALE_L},
  {"m", TT_2DOBJ_SCALE_M},
  {"normal", TT_2DOBJ_SCALE_M},
  {"s", TT_2DOBJ_SCALE_S},
  {"small", TT_2DOBJ_SCALE_S},
  {"tiny", TT_2DOBJ_SCALE_XS},
  {"xl", TT_2DOBJ_SCALE_XL},
  {"xs", TT_2DOBJ_SCALE_XS},
	{NULL, TT_2DOBJ_SCALE_UNKNOWN},
};

 
/**
 * 2ddataobject command options tokens.
 */

enum {
  TT_2DOBJ_UNKNOWN = 2000,
  TT_2DOBJ_SCALE = 2001,
  TT_2DOBJ_CLIP = 2002,
  TT_2DOBJ_PLACE = 2003,
  TT_2DOBJ_VISIBILITY = 2004,
  TT_2DOBJ_CONTEXT = 2005,
};


/**
 * Default 2D object tags.
 */

enum {
  TT_2DOBJ_TAG_NONE = 0,
  TT_2DOBJ_TAG_CLIP_AUTO = 1024,
  TT_2DOBJ_TAG_CLIP_ON = 2048,
  TT_2DOBJ_TAG_VISIBILITY_ON = 4096,
};

enum {
  TT_2DOBJ_PLACE_BOTTOM = 0,
  TT_2DOBJ_PLACE_DEFAULT_BOTTOM = 1,
  TT_2DOBJ_PLACE_DEFAULT = 3,
  TT_2DOBJ_PLACE_DEFAULT_TOP = 4,
  TT_2DOBJ_PLACE_TOP = 5,
  TT_2DOBJ_PLACE_UNKNOWN = 6,
};


/**
 * 2ddataobject command options parsing table.
 */
 
static const thstok thtt_2ddataobject_opt[] = {
  {"clip", TT_2DOBJ_CLIP},
  {"context", TT_2DOBJ_CONTEXT},
  {"place", TT_2DOBJ_PLACE},
  {"scale", TT_2DOBJ_SCALE},
  {"visibility", TT_2DOBJ_VISIBILITY},
  {"visible", TT_2DOBJ_VISIBILITY},
  {NULL, TT_2DOBJ_UNKNOWN},
};


/**
 * 2ddataobject place options.
 */
 
static const thstok thtt_2ddataobject_place[] = {
  {"bottom", TT_2DOBJ_PLACE_BOTTOM},
  {"default", TT_2DOBJ_PLACE_DEFAULT},
  {"top", TT_2DOBJ_PLACE_TOP},
  {NULL, TT_2DOBJ_PLACE_UNKNOWN},
};


/**
 * 2ddataobject class.
 */

class th2ddataobject : public thdataobject {

  public:
  
  const char * m_subtype_str;

  th2ddataobject * pscrapoptr, ///< Previous object in the scrap.
      * nscrapoptr;  ///< Next object in the scrap.
      
  class thscrap * fscrapptr;  ///< Father scrap pointer.
  
  int scale;  ///< 2D map object scaling.
  double scale_numeric;  ///< 2D map object numeric exact scaling.
  unsigned tags;  ///< 2D map object tags.
  int place, context;

  /**
   * Standard constructor.
   */
  
  th2ddataobject();
  
  
  /**
   * Standard destructor.
   */
   
  ~th2ddataobject();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "th2ddataobject";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  virtual bool is(int class_id);
  
  

  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(const char * opts);
  
  
  /**
   * Set command option.
   *
   * @param cod Command option description.
   * @param args Option arguments array.
   * @param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  

  /**
   * Get context for object.
   */
   
  virtual int get_context();
  
  
  /**
   * Export object to metapost file.
   */
  
  virtual bool export_mp(class thexpmapmpxs * out);
  
  /**
   * Export object to metapost file.
   */
  
  virtual bool export_mp_end(class thexpmapmpxs * out);

  /**
   * Parse user defined subtype.
   */
  void parse_u_subtype(const char * subtype);
  
};


void th2dsplitTT(char * src, char ** type, char ** subtype);

void th2dparse_scale(const char * spec, int & type, double & value);

#endif


