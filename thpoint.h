/**
 * @file thpoint.h
 * point module.
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
 
#ifndef thpoint_h
#define thpoint_h


#include "th2ddataobject.h"
#include "thparse.h"
#include "thdb2dpt.h"

/**
 * point command options tokens.
 */
 
enum {
  TT_POINT_UNKNOWN = 3000,
  TT_POINT_TYPE = 3001,
  TT_POINT_SUBTYPE = 3002,
  TT_POINT_STATION = 3003,
  TT_POINT_FROM = 3004,
  TT_POINT_ORIENT = 3005,
  TT_POINT_XSIZE = 3006,
  TT_POINT_YSIZE = 3007,
  TT_POINT_SIZE = 3008,
  TT_POINT_ALIGN = 3009,
  TT_POINT_VALUE = 3010,
  TT_POINT_TEXT = 3011,
  TT_POINT_SCRAP = 3012,
  TT_POINT_DIST = 3013,
  TT_POINT_EXPLORED = 3014,
};


/**
 * point command options parsing table.
 */
 
static const thstok thtt_point_opt[] = {
  {"align", TT_POINT_ALIGN},
  {"dist", TT_POINT_DIST},
  {"explored", TT_POINT_EXPLORED},
  {"from", TT_POINT_FROM},
  {"name", TT_POINT_STATION},
  {"orient", TT_POINT_ORIENT},
  {"orientation", TT_POINT_ORIENT},
  {"scrap", TT_POINT_SCRAP},
  {"size", TT_POINT_SIZE},
  {"station", TT_POINT_STATION},
  {"subtype", TT_POINT_SUBTYPE},
  {"text", TT_POINT_TEXT},
  {"value", TT_POINT_VALUE},
  {"x-size", TT_POINT_XSIZE},
  {"y-size", TT_POINT_YSIZE},
  {NULL, TT_POINT_UNKNOWN},
};

/**
 * Special point tags.
 */

enum {
// height (passage) tags
  TT_POINT_TAG_HEIGHT_U = 1,
  TT_POINT_TAG_HEIGHT_P = 2,
  TT_POINT_TAG_HEIGHT_N = 4,
  TT_POINT_TAG_HEIGHT_UQ = 8,
  TT_POINT_TAG_HEIGHT_PQ = 16,
  TT_POINT_TAG_HEIGHT_NQ = 32,
  TT_POINT_TAG_HEIGHT_ALL = TT_POINT_TAG_HEIGHT_U |
    TT_POINT_TAG_HEIGHT_P | TT_POINT_TAG_HEIGHT_N |
    TT_POINT_TAG_HEIGHT_UQ |
    TT_POINT_TAG_HEIGHT_PQ | TT_POINT_TAG_HEIGHT_NQ,

// date
  TT_POINT_TAG_DATE = 1,
  
};


/**
 * Point types tokens.
 */

enum {
  TT_POINT_TYPE_UNKNOWN,

// specialne typy
  TT_POINT_TYPE_U,
  TT_POINT_TYPE_STATION,
  TT_POINT_TYPE_SECTION,
  TT_POINT_TYPE_WATER_FLOW,
  TT_POINT_TYPE_SPRING,
  TT_POINT_TYPE_SINK,
  TT_POINT_TYPE_LABEL,
  TT_POINT_TYPE_REMARK,
  TT_POINT_TYPE_ALTITUDE,
  TT_POINT_TYPE_HEIGHT,
  TT_POINT_TYPE_PASSAGE_HEIGHT,
  TT_POINT_TYPE_STATION_NAME,
  TT_POINT_TYPE_DATE,
  TT_POINT_TYPE_AIR_DRAUGHT,
  TT_POINT_TYPE_ENTRANCE,
  TT_POINT_TYPE_GRADIENT,

  TT_POINT_TYPE_DIMENSIONS,
  TT_POINT_TYPE_MAP_CONNECTION,
  TT_POINT_TYPE_EXTRA,
  
// vystroj
  TT_POINT_TYPE_NO_EQUIPMENT,
  TT_POINT_TYPE_ANCHOR,
  TT_POINT_TYPE_ROPE,
  TT_POINT_TYPE_FIXED_LADDER,
  TT_POINT_TYPE_ROPE_LADDER,
  TT_POINT_TYPE_STEPS,
  TT_POINT_TYPE_BRIDGE,
  TT_POINT_TYPE_TRAVERSE,
	TT_POINT_TYPE_VIA_FERRATA,
  TT_POINT_TYPE_CAMP,
  TT_POINT_TYPE_DIG,
	TT_POINT_TYPE_HANDRAIL,
  TT_POINT_TYPE_ALTAR,
  TT_POINT_TYPE_DANGER,
  TT_POINT_TYPE_ELECTRICLIGHT,
  TT_POINT_TYPE_EXVOTO,
  TT_POINT_TYPE_GATE,
  TT_POINT_TYPE_HUMANBONE,
  TT_POINT_TYPE_MASONRY,
  TT_POINT_TYPE_MINUS,
  TT_POINT_TYPE_NAMEPLATE,
  TT_POINT_TYPE_NOTWHEELCHAIR,
  TT_POINT_TYPE_PLUS,
  TT_POINT_TYPE_PLUSMINUS,
  TT_POINT_TYPE_WALKWAY,
  TT_POINT_TYPE_WHEELCHAIR,

// ukoncenia chodby
  TT_POINT_TYPE_CONTINUATION,
  TT_POINT_TYPE_NARROW_END,
  TT_POINT_TYPE_LOW_END,
  TT_POINT_TYPE_FLOWSTONE_CHOKE,
  TT_POINT_TYPE_BREAKDOWN_CHOKE,
  TT_POINT_TYPE_CLAY_CHOKE,

// vypln
  TT_POINT_TYPE_FLOWSTONE,
  TT_POINT_TYPE_MOONMILK,
  TT_POINT_TYPE_STALACTITE,
  TT_POINT_TYPE_STALAGMITE,
  TT_POINT_TYPE_PILLAR,
  TT_POINT_TYPE_CURTAIN,
  TT_POINT_TYPE_HELICTITE,
  TT_POINT_TYPE_SODA_STRAW,
  TT_POINT_TYPE_CRYSTAL,
  TT_POINT_TYPE_WALL_CALCITE,
  TT_POINT_TYPE_POPCORN,
  TT_POINT_TYPE_DISK,
  TT_POINT_TYPE_GYPSUM,
  TT_POINT_TYPE_GYPSUM_FLOWER,
  TT_POINT_TYPE_ARAGONITE,
  TT_POINT_TYPE_CAVE_PEARL,
  TT_POINT_TYPE_RIMSTONE_POOL,
  TT_POINT_TYPE_RIMSTONE_DAM,
  TT_POINT_TYPE_ANASTOMOSIS,
  TT_POINT_TYPE_KARREN,
  TT_POINT_TYPE_SCALLOP,
  TT_POINT_TYPE_FLUTE,
  TT_POINT_TYPE_RAFT_CONE,
  TT_POINT_TYPE_ICE_STALACTITE,
  TT_POINT_TYPE_ICE_STALAGMITE,
  TT_POINT_TYPE_ICE_PILLAR,
  TT_POINT_TYPE_STALACTITES,
  TT_POINT_TYPE_STALAGMITES,
  TT_POINT_TYPE_PILLARS,

  
// plosne vyplne
  TT_POINT_TYPE_BEDROCK,
  TT_POINT_TYPE_SAND,
  TT_POINT_TYPE_RAFT,
  TT_POINT_TYPE_CLAY,
  TT_POINT_TYPE_PEBBLES,
  TT_POINT_TYPE_DEBRIS,
  TT_POINT_TYPE_BLOCKS,
  TT_POINT_TYPE_WATER,
  TT_POINT_TYPE_ICE,
  TT_POINT_TYPE_SNOW,
  TT_POINT_TYPE_GUANO,
  TT_POINT_TYPE_CLAY_TREE,
  TT_POINT_TYPE_MUD,
  TT_POINT_TYPE_MUDCRACK,


// ina vypln
  TT_POINT_TYPE_ARCHEO_MATERIAL,
  TT_POINT_TYPE_PALEO_MATERIAL,
  TT_POINT_TYPE_VEGETABLE_DEBRIS,
  TT_POINT_TYPE_ROOT,

};


/**
 * Point types parsing table.
 */
 
static const thstok thtt_point_types[] = {
  {"air-draught",TT_POINT_TYPE_AIR_DRAUGHT},
  {"altar",TT_POINT_TYPE_ALTAR},
  {"altitude",TT_POINT_TYPE_ALTITUDE},
  {"anastomosis",TT_POINT_TYPE_ANASTOMOSIS},
  {"anchor",TT_POINT_TYPE_ANCHOR},
  {"aragonite",TT_POINT_TYPE_ARAGONITE},
  {"archeo-material",TT_POINT_TYPE_ARCHEO_MATERIAL},
  {"bedrock",TT_POINT_TYPE_BEDROCK},
  {"blocks",TT_POINT_TYPE_BLOCKS},
  {"breakdown-choke",TT_POINT_TYPE_BREAKDOWN_CHOKE},
  {"bridge",TT_POINT_TYPE_BRIDGE},
  {"camp",TT_POINT_TYPE_CAMP},
  {"cave-pearl",TT_POINT_TYPE_CAVE_PEARL},
  {"clay",TT_POINT_TYPE_CLAY},
  {"clay-choke",TT_POINT_TYPE_CLAY_CHOKE},
  {"clay-tree",TT_POINT_TYPE_CLAY_TREE},
  {"continuation",TT_POINT_TYPE_CONTINUATION},
  {"crystal",TT_POINT_TYPE_CRYSTAL},
  {"curtain",TT_POINT_TYPE_CURTAIN},
  {"danger",TT_POINT_TYPE_DANGER},
  {"date",TT_POINT_TYPE_DATE},
  {"debris",TT_POINT_TYPE_DEBRIS},
  {"dig",TT_POINT_TYPE_DIG},
  {"dimensions",TT_POINT_TYPE_DIMENSIONS},
  {"disk",TT_POINT_TYPE_DISK},
  {"electric-light",TT_POINT_TYPE_ELECTRICLIGHT},
  {"entrance",TT_POINT_TYPE_ENTRANCE},
  {"ex-voto",TT_POINT_TYPE_EXVOTO},
  {"extra",TT_POINT_TYPE_EXTRA},
  {"fixed-ladder",TT_POINT_TYPE_FIXED_LADDER},
  {"flowstone",TT_POINT_TYPE_FLOWSTONE},
  {"flowstone-choke",TT_POINT_TYPE_FLOWSTONE_CHOKE},
  {"flute",TT_POINT_TYPE_FLUTE},
  {"gate",TT_POINT_TYPE_GATE},
  {"gradient",TT_POINT_TYPE_GRADIENT},
  {"guano",TT_POINT_TYPE_GUANO},
  {"gypsum",TT_POINT_TYPE_GYPSUM},
  {"gypsum-flower",TT_POINT_TYPE_GYPSUM_FLOWER},
  {"handrail",TT_POINT_TYPE_HANDRAIL},
  {"height",TT_POINT_TYPE_HEIGHT},
  {"helictite",TT_POINT_TYPE_HELICTITE},
  {"human-bone",TT_POINT_TYPE_HUMANBONE},
  {"ice",TT_POINT_TYPE_ICE},
  {"ice-pillar",TT_POINT_TYPE_ICE_PILLAR},
  {"ice-stalactite",TT_POINT_TYPE_ICE_STALACTITE},
  {"ice-stalagmite",TT_POINT_TYPE_ICE_STALAGMITE},
  {"karren",TT_POINT_TYPE_KARREN},
  {"label",TT_POINT_TYPE_LABEL},
  {"low-end",TT_POINT_TYPE_LOW_END},
  {"map-connection",TT_POINT_TYPE_MAP_CONNECTION},
  {"masonry",TT_POINT_TYPE_MASONRY},
  {"minus",TT_POINT_TYPE_MINUS},
  {"moonmilk",TT_POINT_TYPE_MOONMILK},
  {"mud",TT_POINT_TYPE_MUD},
  {"mudcrack",TT_POINT_TYPE_MUDCRACK},
  {"nameplate",TT_POINT_TYPE_NAMEPLATE},
  {"narrow-end",TT_POINT_TYPE_NARROW_END},
  {"no-equipment",TT_POINT_TYPE_NO_EQUIPMENT},
  {"notwheelchair",TT_POINT_TYPE_NOTWHEELCHAIR},
  {"paleo-material",TT_POINT_TYPE_PALEO_MATERIAL},
  {"passage-height",TT_POINT_TYPE_PASSAGE_HEIGHT},
  {"pebbles",TT_POINT_TYPE_PEBBLES},
  {"pillar",TT_POINT_TYPE_PILLAR},
  {"pillars",TT_POINT_TYPE_PILLARS},
  {"plus",TT_POINT_TYPE_PLUS},
  {"plus-minus",TT_POINT_TYPE_PLUSMINUS},
  {"popcorn",TT_POINT_TYPE_POPCORN},
  {"raft",TT_POINT_TYPE_RAFT},
  {"raft-cone",TT_POINT_TYPE_RAFT_CONE},
  {"remark",TT_POINT_TYPE_REMARK},
  {"rimstone-dam",TT_POINT_TYPE_RIMSTONE_DAM},
  {"rimstone-pool",TT_POINT_TYPE_RIMSTONE_POOL},
  {"root",TT_POINT_TYPE_ROOT},
  {"rope",TT_POINT_TYPE_ROPE},
  {"rope-ladder",TT_POINT_TYPE_ROPE_LADDER},
  {"sand",TT_POINT_TYPE_SAND},
  {"scallop",TT_POINT_TYPE_SCALLOP},
  {"section",TT_POINT_TYPE_SECTION},
  {"sink",TT_POINT_TYPE_SINK},
  {"snow",TT_POINT_TYPE_SNOW},
  {"soda-straw",TT_POINT_TYPE_SODA_STRAW},
  {"spring",TT_POINT_TYPE_SPRING},
  {"stalactite",TT_POINT_TYPE_STALACTITE},
  {"stalactites",TT_POINT_TYPE_STALACTITES},
  {"stalagmite",TT_POINT_TYPE_STALAGMITE},
  {"stalagmites",TT_POINT_TYPE_STALAGMITES},
  {"station", TT_POINT_TYPE_STATION},
  {"station-name", TT_POINT_TYPE_STATION_NAME},
  {"steps",TT_POINT_TYPE_STEPS},
  {"traverse",TT_POINT_TYPE_TRAVERSE},
  {"u",TT_POINT_TYPE_U},
  {"vegetable-debris",TT_POINT_TYPE_VEGETABLE_DEBRIS},
  {"via-ferrata",TT_POINT_TYPE_VIA_FERRATA},
  {"walkway",TT_POINT_TYPE_WALKWAY},
  {"wall-calcite",TT_POINT_TYPE_WALL_CALCITE},
  {"water",TT_POINT_TYPE_WATER},
  {"water-flow",TT_POINT_TYPE_WATER_FLOW},
  {"wheelchair",TT_POINT_TYPE_WHEELCHAIR},
	{NULL, TT_POINT_TYPE_UNKNOWN},
};

/**
 * Point align tokens.
 */

enum {
  TT_POINT_ALIGN_UNKNOWN,
  TT_POINT_ALIGN_T,
  TT_POINT_ALIGN_B,
  TT_POINT_ALIGN_L,
  TT_POINT_ALIGN_R,
  TT_POINT_ALIGN_TL,
  TT_POINT_ALIGN_BL,
  TT_POINT_ALIGN_TR,
  TT_POINT_ALIGN_BR,
  TT_POINT_ALIGN_C,
};


/**
 * Point align parsing table.
 */
 
static const thstok thtt_point_aligns[] = {
  {"b", TT_POINT_ALIGN_B},
  {"bl", TT_POINT_ALIGN_BL},
  {"bottom", TT_POINT_ALIGN_B},
  {"bottom-left", TT_POINT_ALIGN_BL},
  {"bottom-right", TT_POINT_ALIGN_BR},
  {"br", TT_POINT_ALIGN_BR},
  {"c", TT_POINT_ALIGN_C},
  {"center", TT_POINT_ALIGN_C},
  {"l", TT_POINT_ALIGN_L},
  {"left", TT_POINT_ALIGN_L},
  {"r", TT_POINT_ALIGN_R},
  {"right", TT_POINT_ALIGN_R},
  {"t", TT_POINT_ALIGN_T},
  {"tl", TT_POINT_ALIGN_TL},
  {"top", TT_POINT_ALIGN_T},
  {"top-left", TT_POINT_ALIGN_TL},
  {"top-right", TT_POINT_ALIGN_TR},
  {"tr", TT_POINT_ALIGN_TR},
	{NULL, TT_POINT_ALIGN_UNKNOWN},
};




/**
 * Point subtypes tokens.
 */

enum {
  TT_POINT_SUBTYPE_UNKNOWN,
  TT_POINT_SUBTYPE_TEMP,
  TT_POINT_SUBTYPE_PAINTED,
  TT_POINT_SUBTYPE_FIXED,
  TT_POINT_SUBTYPE_NATURAL,
  TT_POINT_SUBTYPE_PALEO,
  TT_POINT_SUBTYPE_PERMANENT,
  TT_POINT_SUBTYPE_INTERMITTENT,
  TT_POINT_SUBTYPE_POSITIVE,
  TT_POINT_SUBTYPE_NEGATIVE,
  TT_POINT_SUBTYPE_BOTH,
  TT_POINT_SUBTYPE_UNSIGNED,
  TT_POINT_SUBTYPE_WINTER,
  TT_POINT_SUBTYPE_SUMMER,
  TT_POINT_SUBTYPE_UNDEF,
};


/**
 * Point types parsing table.
 */
 
static const thstok thtt_point_subtypes[] = {
  {"both", TT_POINT_SUBTYPE_BOTH},
  {"fixed", TT_POINT_SUBTYPE_FIXED},
  {"intermittent", TT_POINT_SUBTYPE_INTERMITTENT},
  {"natural", TT_POINT_SUBTYPE_NATURAL},
  {"negative", TT_POINT_SUBTYPE_NEGATIVE},
  {"painted", TT_POINT_SUBTYPE_PAINTED},
  {"paleo", TT_POINT_SUBTYPE_PALEO},
  {"permanent", TT_POINT_SUBTYPE_PERMANENT},
  {"positive", TT_POINT_SUBTYPE_POSITIVE},
  {"summer", TT_POINT_SUBTYPE_SUMMER},
  {"temporary", TT_POINT_SUBTYPE_TEMP},
  {"undefined", TT_POINT_SUBTYPE_UNDEF},
  {"unsigned", TT_POINT_SUBTYPE_UNSIGNED},
  {"winter", TT_POINT_SUBTYPE_WINTER},
	{NULL, TT_POINT_SUBTYPE_UNKNOWN},
};


/**
 * point class.
 */

class thpoint : public th2ddataobject {

  public:
  
  friend class thdb2d;

  int type,  ///< Point type.
    subtype,  ///< Point subtype.
    align;  ///< Point align.
  double orient, xsize, ysize;  ///<...
  
  const char * text;  ///< Point text.
    
  thdb2dpt * point;  ///< Point coordinates.
  
  class thdb2dcp * cpoint; ///< Control point.
  
  thobjectname station_name,  ///< Station name.
    from_name;  ///< Extend name.

  char extend_opts;  ///< Extend options.

  virtual void start_insert();
    
  void parse_type(char * tstr);  ///< Parse point type.

  void parse_align(char * tstr);  ///< Parse point align.
  
  void parse_subtype(char * ststr);  ///< Parse point subtype.
  
  void parse_from(char * estr);  ///< Parse station extend.

  void parse_text(char * ss);  ///< Parse point text.

  void parse_explored(char * ss);  ///< Parse explored length.

  void parse_value(char * ss);  ///< Parse point value.

  void check_extra();

  /**
   * Standard constructor.
   */
  
  thpoint();
  
  
  /**
   * Standard destructor.
   */
   
  ~thpoint();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual const char * get_class_name() {return "thpoint";};
  
  
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
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 

  /**
   * Export to metapost file.
   */
   
  virtual bool export_mp(class thexpmapmpxs * out);
  
};

#endif


