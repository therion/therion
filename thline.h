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
  TT_LINE_HEIGHT = 3019,
  TT_LINE_ANCHORS = 3020,
  TT_LINE_REBELAYS = 3021,
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
// tags for type rope, indicating to show rebelays and anchors
  TT_LINE_TAG_ROPE_ANCHORS = 1,
  TT_LINE_TAG_ROPE_REBELAYS = 2,
};



/**
 * line command options parsing table.
 */

static const thstok thtt_line_opt[] = {
  {"adjust", TT_LINE_ADJUST},
  {"altitude", TT_LINE_ALTITUDE},
  {"anchors", TT_LINE_ANCHORS},
  {"border", TT_LINE_BORDER},
  {"close", TT_LINE_CLOSED},
  {"direction", TT_LINE_DIRECTION},
  {"gradient", TT_LINE_GRADIENT},
  {"head", TT_LINE_HEAD},
  {"height", TT_LINE_HEIGHT},
  {"l-size",TT_LINE_LSIZE},
  {"mark", TT_LINE_MARK},
  {"orient", TT_LINE_ORIENT},
  {"orientation", TT_LINE_ORIENT},
  {"outline", TT_LINE_OUTLINE},
  {"r-size",TT_LINE_RSIZE},
  {"rebelays", TT_LINE_REBELAYS},
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
	TT_LINE_TYPE_HANDRAIL,
	TT_LINE_TYPE_FIXED_LADDER,
	TT_LINE_TYPE_ROPE_LADDER,
	TT_LINE_TYPE_VIA_FERRATA,
	TT_LINE_TYPE_ROPE,
	TT_LINE_TYPE_STEPS,
  TT_LINE_TYPE_ABYSSENTRANCE,
  TT_LINE_TYPE_DRIPLINE,
  TT_LINE_TYPE_FAULT,
  TT_LINE_TYPE_JOINT,
  TT_LINE_TYPE_LOWCEILING,
  TT_LINE_TYPE_PITCHIMNEY,
  TT_LINE_TYPE_RIMSTONEDAM,
  TT_LINE_TYPE_RIMSTONEPOOL,
  TT_LINE_TYPE_WALKWAY,
};

/**
 * Line types parsing table.
 */

static const thstok thtt_line_types[] = {
  {"abyss-entrance", TT_LINE_TYPE_ABYSSENTRANCE},
  {"arrow", TT_LINE_TYPE_ARROW},
  {"border", TT_LINE_TYPE_BORDER},
  {"ceiling-meander", TT_LINE_TYPE_CEILING_MEANDER},
  {"ceiling-step", TT_LINE_TYPE_CEILING_STEP},
  {"chimney", TT_LINE_TYPE_CHIMNEY},
  {"contour", TT_LINE_TYPE_CONTOUR},
  {"dripline", TT_LINE_TYPE_DRIPLINE},
  {"fault", TT_LINE_TYPE_FAULT},
  {"fixed-ladder", TT_LINE_TYPE_FIXED_LADDER},
  {"floor-meander", TT_LINE_TYPE_FLOOR_MEANDER},
  {"floor-step", TT_LINE_TYPE_FLOOR_STEP},
  {"flowstone", TT_LINE_TYPE_FLOWSTONE},
  {"gradient", TT_LINE_TYPE_GRADIENT},
  {"handrail", TT_LINE_TYPE_HANDRAIL},
  {"joint", TT_LINE_TYPE_JOINT},
  {"label", TT_LINE_TYPE_LABEL},
  {"low-ceiling", TT_LINE_TYPE_LOWCEILING},
  {"map-connection", TT_LINE_TYPE_MAP_CONNECTION},
  {"moonmilk", TT_LINE_TYPE_MOONMILK},
  {"overhang", TT_LINE_TYPE_OVERHANG},
  {"pit", TT_LINE_TYPE_PIT},
  {"pit-chimney", TT_LINE_TYPE_PITCHIMNEY},
  {"pitch", TT_LINE_TYPE_PIT},
  {"rimstone-dam", TT_LINE_TYPE_RIMSTONEDAM},
  {"rimstone-pool", TT_LINE_TYPE_RIMSTONEPOOL},
  {"rock-border", TT_LINE_TYPE_ROCK_BORDER},
  {"rock-edge", TT_LINE_TYPE_ROCK_EDGE},
  {"rope", TT_LINE_TYPE_ROPE},
  {"rope-ladder", TT_LINE_TYPE_ROPE_LADDER},
  {"section", TT_LINE_TYPE_SECTION},
  {"slope", TT_LINE_TYPE_SLOPE},
  {"steps", TT_LINE_TYPE_STEPS},
  {"survey", TT_LINE_TYPE_SURVEY},
  {"u",TT_LINE_TYPE_U},
  {"via-ferrata",TT_LINE_TYPE_VIA_FERRATA},
  {"walkway", TT_LINE_TYPE_WALKWAY},
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
  TT_LINE_SUBTYPE_OVERLYING,
  TT_LINE_SUBTYPE_PIT,
  TT_LINE_SUBTYPE_MOONMILK,
  TT_LINE_SUBTYPE_FLOWSTONE,
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
  {"flowstone",TT_LINE_SUBTYPE_FLOWSTONE},
  {"ice",TT_LINE_SUBTYPE_ICE},
  {"intermittent",TT_LINE_SUBTYPE_INTERMITTENT},
  {"invisible", TT_LINE_SUBTYPE_INVISIBLE},
  {"moonmilk",TT_LINE_SUBTYPE_MOONMILK},
  {"overlying",TT_LINE_SUBTYPE_OVERLYING},
  {"pebbles",TT_LINE_SUBTYPE_PEBBLES},
  {"permanent",TT_LINE_SUBTYPE_PERMANENT},
  {"pit",TT_LINE_SUBTYPE_PIT},
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

  const char * text;  ///< Label text.

  bool reverse;  ///< Whether line should be reversed.
  bool is_closed;  ///< Whether line is closed.

  double m_height; ///< Height of a pit.

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
  void parse_height(char * ss);  ///< Parse pit/chimney height.
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

  int get_class_id() override;


  /**
   * Return class name.
   */

  const char * get_class_name() override {return "thline";};


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
   * Return marked line station.
   */

  class thdb2dlp * get_marked_station(const char * mark);


  /**
   * Export to metapost file.
   */

  bool export_mp(class thexpmapmpxs * out) override;


  /**
   * Export only path to metapost file.
   */

  virtual unsigned export_path_mp(class thexpmapmpxs * out,
      int from = 0, int to = -1, int dbglevel = -1);

  void start_insert() override;

};


#endif


