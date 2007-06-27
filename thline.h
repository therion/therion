/**
 * @file thline.h
 * line module.
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
 
#ifndef thline_h
#define thline_h


#include "th2ddataobject.h"

/**
 * line command options tokens.
 */
 
enum {
  TT_LINE_UNKNOWN = 3000,
  TT_LINE_TYPE = 3001,
  TT_LINE_SUBTYPE = 3002,
  TT_LINE_REVERSE = 3003,
  TT_LINE_CLOSED = 3004,
  TT_LINE_SMOOTH = 3005,
  TT_LINE_OUTLINE = 3006,
  TT_LINE_MARK = 3007,
  TT_LINE_RSIZE = 3008,
  TT_LINE_LSIZE = 3009,
  TT_LINE_ORIENT = 3010,
  TT_LINE_SIZE = 3011,
  TT_LINE_GRADIENT = 3012,
  TT_LINE_BORDER = 3013,
  TT_LINE_ALTITUDE = 3014,
  TT_LINE_DIRECTION = 3015,
  TT_LINE_HEAD = 3016,
  TT_LINE_TEXT = 3017,
  TT_LINE_ADJUST = 3018,
};


enum {
// tags pre slope
  TT_LINE_TAG_BORDER = 1,
// tags pre contour
  TT_LINE_TAG_GRADIENT_CENTER = 1,
  TT_LINE_TAG_GRADIENT_POINT = 2,
  TT_LINE_TAG_GRADIENT_NONE = 4,
// tags pre section
  TT_LINE_TAG_DIRECTION_BEGIN = 1,
  TT_LINE_TAG_DIRECTION_END = 2,
  TT_LINE_TAG_DIRECTION_POINT = 4,
// tags pre arrow
  TT_LINE_TAG_HEAD_BEGIN = 1,
  TT_LINE_TAG_HEAD_END = 2,
};



/**
 * line command options parsing table.
 */

static const thstok thtt_line_opt[] = {
  {"adjust", TT_LINE_ADJUST},
  {"altitude", TT_LINE_ALTITUDE},
  {"border", TT_LINE_BORDER},
  {"close", TT_LINE_CLOSED},
  {"direction", TT_LINE_DIRECTION},
  {"gradient", TT_LINE_GRADIENT},
  {"head", TT_LINE_HEAD},
  {"l-size",TT_LINE_LSIZE},
  {"mark", TT_LINE_MARK},
  {"orient", TT_LINE_ORIENT},
  {"orientation", TT_LINE_ORIENT},
  {"outline", TT_LINE_OUTLINE},
  {"r-size",TT_LINE_RSIZE},
  {"reverse", TT_LINE_REVERSE},
  {"size",TT_LINE_SIZE},
  {"smooth",TT_LINE_SMOOTH},
  {"subtype", TT_LINE_SUBTYPE},
  {"text",TT_LINE_TEXT},
  {NULL, TT_LINE_UNKNOWN},
};


/**
 * Line outlines.
 */
 
enum {
  TT_LINE_OUTLINE_UNKNOWN,
  TT_LINE_OUTLINE_IN,
  TT_LINE_OUTLINE_OUT,
  TT_LINE_OUTLINE_NONE,
};

/**
 * line outlines parsing table.
 */
 
static const thstok thtt_line_outlines[] = {
  {"in", TT_LINE_OUTLINE_IN},
  {"none", TT_LINE_OUTLINE_NONE},
  {"out", TT_LINE_OUTLINE_OUT},
  {NULL, TT_LINE_OUTLINE_UNKNOWN},
};


/**
 * Line point adjustments.
 */
 
enum {
  TT_LINE_ADJUST_UNKNOWN,
  TT_LINE_ADJUST_HORIZONTAL,
  TT_LINE_ADJUST_VERTICAL,
  TT_LINE_ADJUST_NONE,
};

/**
 * line adjusts parsing table.
 */
 
static const thstok thtt_line_adjusts[] = {
  {"horizontal", TT_LINE_ADJUST_HORIZONTAL},
  {"none", TT_LINE_ADJUST_NONE},
  {"vertical", TT_LINE_ADJUST_VERTICAL},
  {NULL, TT_LINE_ADJUST_UNKNOWN},
};


/**
 * Line types.
 */
 
enum {
  TT_LINE_TYPE_UNKNOWN,
  TT_LINE_TYPE_U,
  TT_LINE_TYPE_WALL,
  TT_LINE_TYPE_PIT,
  TT_LINE_TYPE_CHIMNEY,
  TT_LINE_TYPE_ROCK_BORDER,
  TT_LINE_TYPE_SURVEY,
  TT_LINE_TYPE_ROCK_EDGE,
  TT_LINE_TYPE_FLOWSTONE,
  TT_LINE_TYPE_MOONMILK,
  TT_LINE_TYPE_BORDER,
  TT_LINE_TYPE_SLOPE,
  TT_LINE_TYPE_CONTOUR,
  TT_LINE_TYPE_OVERHANG,
  TT_LINE_TYPE_SECTION,
  TT_LINE_TYPE_ARROW,
  TT_LINE_TYPE_LABEL,
  TT_LINE_TYPE_CEILING_STEP,
  TT_LINE_TYPE_FLOOR_STEP,
  TT_LINE_TYPE_WATER_FLOW,
  TT_LINE_TYPE_GRADIENT,
  TT_LINE_TYPE_CEILING_MEANDER,
  TT_LINE_TYPE_FLOOR_MEANDER,
  TT_LINE_TYPE_MAP_CONNECTION,
};

/**
 * Line types parsing table.
 */
 
static const thstok thtt_line_types[] = {
  {"arrow", TT_LINE_TYPE_ARROW},
  {"border", TT_LINE_TYPE_BORDER},
  {"ceiling-meander", TT_LINE_TYPE_CEILING_MEANDER},
  {"ceiling-step", TT_LINE_TYPE_CEILING_STEP},
  {"chimney", TT_LINE_TYPE_CHIMNEY},
  {"contour", TT_LINE_TYPE_CONTOUR},
  {"floor-meander", TT_LINE_TYPE_FLOOR_MEANDER},
  {"floor-step", TT_LINE_TYPE_FLOOR_STEP},
  {"flowstone", TT_LINE_TYPE_FLOWSTONE},
  {"gradient", TT_LINE_TYPE_GRADIENT},
  {"label", TT_LINE_TYPE_LABEL},
  {"map-connection", TT_LINE_TYPE_MAP_CONNECTION},
  {"moonmilk", TT_LINE_TYPE_MOONMILK},
  {"overhang", TT_LINE_TYPE_OVERHANG},
  {"pit", TT_LINE_TYPE_PIT},
  {"pitch", TT_LINE_TYPE_PIT},
  {"rock-border", TT_LINE_TYPE_ROCK_BORDER},
  {"rock-edge", TT_LINE_TYPE_ROCK_EDGE},
  {"section", TT_LINE_TYPE_SECTION},
  {"slope", TT_LINE_TYPE_SLOPE},
  {"survey", TT_LINE_TYPE_SURVEY},
  {"u",TT_LINE_TYPE_U},
  {"wall", TT_LINE_TYPE_WALL},
  {"water-flow", TT_LINE_TYPE_WATER_FLOW},
  {NULL, TT_LINE_TYPE_UNKNOWN},
};


/**
 * Line subtypes.
 */
 
enum {
  TT_LINE_SUBTYPE_UNKNOWN,
  TT_LINE_SUBTYPE_TEMPORARY,
  TT_LINE_SUBTYPE_INVISIBLE,
  TT_LINE_SUBTYPE_VISIBLE,
  TT_LINE_SUBTYPE_BEDROCK,
  TT_LINE_SUBTYPE_SAND,
  TT_LINE_SUBTYPE_CLAY,
  TT_LINE_SUBTYPE_PEBBLES,
  TT_LINE_SUBTYPE_DEBRIS,
  TT_LINE_SUBTYPE_BLOCKS,
  TT_LINE_SUBTYPE_ICE,
  TT_LINE_SUBTYPE_UNDERLYING,
  TT_LINE_SUBTYPE_UNSURVEYED,
  TT_LINE_SUBTYPE_PRESUMED,
  TT_LINE_SUBTYPE_PERMANENT,
  TT_LINE_SUBTYPE_INTERMITTENT,
  TT_LINE_SUBTYPE_CONJECTURAL,
  TT_LINE_SUBTYPE_CAVE,
  TT_LINE_SUBTYPE_SURFACE,
};

/**
 * Line subtypes parsing table.
 */
 
static const thstok thtt_line_subtypes[] = {
  {"bedrock", TT_LINE_SUBTYPE_BEDROCK},
  {"blocks",TT_LINE_SUBTYPE_BLOCKS},
  {"cave",TT_LINE_SUBTYPE_CAVE},
  {"clay",TT_LINE_SUBTYPE_CLAY},
  {"conjectural",TT_LINE_SUBTYPE_CONJECTURAL},
  {"debris",TT_LINE_SUBTYPE_DEBRIS},
  {"ice",TT_LINE_SUBTYPE_ICE},
  {"intermittent",TT_LINE_SUBTYPE_INTERMITTENT},
  {"invisible", TT_LINE_SUBTYPE_INVISIBLE},
  {"pebbles",TT_LINE_SUBTYPE_PEBBLES},
  {"permanent",TT_LINE_SUBTYPE_PERMANENT},
  {"presumed",TT_LINE_SUBTYPE_PRESUMED},
  {"sand",TT_LINE_SUBTYPE_SAND},
  {"surface",TT_LINE_SUBTYPE_SURFACE},
  {"temporary",TT_LINE_SUBTYPE_TEMPORARY},
  {"underlying",TT_LINE_SUBTYPE_UNDERLYING},
  {"unsurveyed",TT_LINE_SUBTYPE_UNSURVEYED},
  {"visible", TT_LINE_SUBTYPE_VISIBLE},
  {NULL, TT_LINE_SUBTYPE_UNKNOWN},
};



/**
 * line class.
 */

class thline : public th2ddataobject {

  public:
  
  friend class thdb2d;

  int type,  ///< Line type.
    outline,  ///< Line outline.
    closed,  ///< How line is closed.
    csubtype;  ///< Current subtype.
    
  char * text;  ///< Label text.
  
  bool reverse;  ///< Whether line should be reversed.
  bool is_closed;  ///< Whether line is closed.
  
  class thdb2dlp * first_point,  ///< First line point.
    * last_point;  ///< Last line point.
    
  void parse_type(char * ss);  ///< Parse line type.
  void parse_subtype(char * ss);  ///< Parse line subtype.
  void parse_border(char * ss);  ///< Parse line type.
  void parse_size(int w, char * ss);  ///< Parse line size.
  void parse_gradient(char * ss);  ///< Parse line subtype.
  void parse_direction(char * ss);  ///< Parse line direction.
  void parse_altitude(char * ss);  ///< Parse wall altitude
  void parse_head(char * ss);  ///< Parse line head.
  void parse_adjust(char * ss);  ///< Parse line head.
  void parse_text(char * ss);  ///< Parse line text.
  void insert_line_point(int nargs, char ** args, double * nums = NULL);  ///< Insert line point.
  void insert_point_mark(char * ss);  ///< Insert line point mark.
  
  void preprocess();  ///< Reverse if necessary and close.

  /**
   * Standard constructor.
   */
  
  thline();
  
  
  /**
   * Standard destructor.
   */
   
  ~thline();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual char * get_class_name() {return "thline";};
  
  
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
   
  virtual char * get_cmd_name();
  
  
  /**
   * Return command end option.
   */
   
  virtual char * get_cmd_end();
  
  
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
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  
  
  /**
   * Return marked line station.
   */
   
  class thdb2dlp * get_marked_station(char * mark);


  /**
   * Export to metapost file.
   */
   
  virtual bool export_mp(class thexpmapmpxs * out);


  /**
   * Export only path to metapost file.
   */
   
  virtual unsigned export_path_mp(class thexpmapmpxs * out,
      int from = 0, int to = -1, int dbglevel = -1);

  virtual void start_insert();
  
};


#endif


