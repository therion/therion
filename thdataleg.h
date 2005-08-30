/**
 * @file thdataleg.h
 * Survey leg class.
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
 
#ifndef thdataleg_h
#define thdataleg_h

#include "thparse.h"
#include "thobjectname.h"
#include "thobjectsrc.h"
#include "thinfnan.h"
#include <list>

/**
 * survey leg components.
 */
 
enum {
  TT_DATALEG_UNKNOWN,
  TT_DATALEG_STATION,
  TT_DATALEG_FROM,
  TT_DATALEG_TO,
  TT_DATALEG_DIRECTION,
  TT_DATALEG_LENGTH,
  TT_DATALEG_BEARING,
  TT_DATALEG_BACKBEARING,
  TT_DATALEG_GRADIENT,
  TT_DATALEG_BACKGRADIENT,
  TT_DATALEG_COUNT,
  TT_DATALEG_FROMCOUNT,
  TT_DATALEG_TOCOUNT,
  TT_DATALEG_DEPTH,
  TT_DATALEG_FROMDEPTH,
  TT_DATALEG_TODEPTH,
  TT_DATALEG_DEPTHCHANGE,
  TT_DATALEG_NORTHING,
  TT_DATALEG_EASTING,
  TT_DATALEG_ALTITUDE,
  TT_DATALEG_NOTES,
  TT_DATALEG_PICTURES,
  TT_DATALEG_POSITION,
  TT_DATALEG_X,
  TT_DATALEG_Y,
  TT_DATALEG_Z,
  TT_DATALEG_NEWLINE,
  TT_DATALEG_INSTRUMENTS,
  TT_DATALEG_ASSISTANT,
  TT_DATALEG_UP,
  TT_DATALEG_DOWN,
  TT_DATALEG_LEFT,
  TT_DATALEG_RIGHT,
  TT_DATALEG_DIMS,
  TT_DATALEG_IGNORE,
  TT_DATALEG_IGNOREALL,
};



/**
 * survey leg parsing table.
 */
 
static const thstok thtt_dataleg_comp[] = {
  {"altitude", TT_DATALEG_ALTITUDE},
  {"assistent", TT_DATALEG_ASSISTANT},
  {"backbearing", TT_DATALEG_BACKBEARING},
  {"backclino", TT_DATALEG_BACKGRADIENT},
  {"backcompass", TT_DATALEG_BACKBEARING},
  {"backgradient", TT_DATALEG_BACKGRADIENT},
  {"bearing", TT_DATALEG_BEARING},
  {"ceiling", TT_DATALEG_UP},
  {"clino", TT_DATALEG_GRADIENT},
  {"compass", TT_DATALEG_BEARING},
  {"count", TT_DATALEG_COUNT},
  {"counter", TT_DATALEG_COUNT},
  {"depth", TT_DATALEG_DEPTH},
  {"depthchange", TT_DATALEG_DEPTHCHANGE},
  {"dimensions", TT_DATALEG_DIMS},
  {"direction", TT_DATALEG_DIRECTION},
  {"dog", TT_DATALEG_ASSISTANT},
  {"down", TT_DATALEG_DOWN},
  {"dx", TT_DATALEG_EASTING},
  {"dy", TT_DATALEG_NORTHING},
  {"dz", TT_DATALEG_ALTITUDE},
  {"easting", TT_DATALEG_EASTING},
  {"floor", TT_DATALEG_DOWN},
  {"from", TT_DATALEG_FROM},
  {"fromcount", TT_DATALEG_FROMCOUNT},
  {"fromcounter", TT_DATALEG_FROMCOUNT},
  {"fromdepth", TT_DATALEG_FROMDEPTH},
  {"gradient", TT_DATALEG_GRADIENT},
  {"ignore", TT_DATALEG_IGNORE},
  {"ignoreall", TT_DATALEG_IGNOREALL},
  {"instruments", TT_DATALEG_INSTRUMENTS},
  {"insts", TT_DATALEG_INSTRUMENTS},
  {"left", TT_DATALEG_LEFT},
  {"length", TT_DATALEG_LENGTH},
  {"newline", TT_DATALEG_NEWLINE},
  {"northing", TT_DATALEG_NORTHING},
  {"notebook", TT_DATALEG_NOTES},
  {"notes", TT_DATALEG_NOTES},
  {"pics", TT_DATALEG_PICTURES},
  {"pictures", TT_DATALEG_PICTURES},
  {"position", TT_DATALEG_POSITION},
  {"right", TT_DATALEG_RIGHT},
  {"station", TT_DATALEG_STATION},
  {"tape", TT_DATALEG_LENGTH},
  {"to", TT_DATALEG_TO},
  {"tocount", TT_DATALEG_TOCOUNT},
  {"tocounter", TT_DATALEG_TOCOUNT},
  {"todepth", TT_DATALEG_TODEPTH},
  {"up", TT_DATALEG_UP},
  {"x", TT_DATALEG_X},
  {"y", TT_DATALEG_Y},
  {"z", TT_DATALEG_Z},
  {NULL, TT_DATALEG_UNKNOWN},
};


/**
 * Infer components.
 */
 
enum {
  TT_DATALEG_INFER_PLUMBS,
  TT_DATALEG_INFER_EQUATES,
  TT_DATALEG_INFER_UNKNOWN,
};


/**
 * Dataleg infer componets.
 */
 
static const thstok thtt_dataleg_infer[] = {
  {"equates", TT_DATALEG_INFER_EQUATES},
  {"plumbs", TT_DATALEG_INFER_PLUMBS},
  {NULL, TT_DATALEG_INFER_UNKNOWN},
};


/**
 * Shape components.
 */
 
enum {
  TT_DATALEG_SHAPE_DIAMOND,
  TT_DATALEG_SHAPE_OCTAGON,
  TT_DATALEG_SHAPE_RECTANGLE,
  TT_DATALEG_SHAPE_TRIANGLE,
  TT_DATALEG_SHAPE_TUNNEL,
  TT_DATALEG_SHAPE_UNKNOWN,
};

static const thstok thtt_dataleg_shape[] = {
  {"diamond", TT_DATALEG_SHAPE_DIAMOND},
  {"octagon", TT_DATALEG_SHAPE_OCTAGON},
  {"rect", TT_DATALEG_SHAPE_RECTANGLE},
  {"rectangle", TT_DATALEG_SHAPE_RECTANGLE},
  {"triangle", TT_DATALEG_SHAPE_TRIANGLE},
  {"tunnel", TT_DATALEG_SHAPE_TUNNEL},
  {NULL, TT_DATALEG_SHAPE_UNKNOWN},
};


/**
 * Data types.
 */
 
enum {
  TT_DATATYPE_NORMAL,
  TT_DATATYPE_DIVING,
  TT_DATATYPE_CARTESIAN,
  TT_DATATYPE_CYLPOLAR,
  TT_DATATYPE_NOSURVEY,
  TT_DATATYPE_DIMS,
  TT_DATATYPE_UNKNOWN,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_datatype[] = {
  {"cartesian", TT_DATATYPE_CARTESIAN},
  {"cylpolar", TT_DATATYPE_CYLPOLAR},
  {"dimensions", TT_DATATYPE_DIMS},
  {"diving", TT_DATATYPE_DIVING},
  {"normal", TT_DATATYPE_NORMAL},
  {"nosurvey", TT_DATATYPE_NOSURVEY},
  {"topofil", TT_DATATYPE_NORMAL},
  {NULL, TT_DATATYPE_UNKNOWN},
};


/**
 * Mark types.
 */
 
enum {
  TT_DATAMARK_UNKNOWN = 0U,
  TT_DATAMARK_TEMP = 1U,
  TT_DATAMARK_PAINTED = 2U,
  TT_DATAMARK_NATURAL = 3U,
  TT_DATAMARK_FIXED = 4U,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_datamark[] = {
  {"fixed", TT_DATAMARK_FIXED},
  {"natural", TT_DATAMARK_NATURAL},
  {"painted", TT_DATAMARK_PAINTED},
  {"temp", TT_DATAMARK_TEMP},
  {"temporary", TT_DATAMARK_TEMP},
  {NULL, TT_DATAMARK_UNKNOWN},
};


/**
 * Data leg flags.
 */
 
enum {
  TT_DATALFLAG_UNKNOWN,
  TT_DATALFLAG_SURFACE,
  TT_DATALFLAG_DUPLICATE,
  TT_DATALFLAG_NOT,  
};


enum {
  TT_EXTENDFLAG_UNKNOWN = 0,
  TT_EXTENDFLAG_NORMAL = 1,
  TT_EXTENDFLAG_REVERSE = 2,
  TT_EXTENDFLAG_LEFT = 4,
  TT_EXTENDFLAG_RIGHT = 8,
  TT_EXTENDFLAG_DIRECTION = 15,
  TT_EXTENDFLAG_BREAK = 16,
  TT_EXTENDFLAG_START = 32,
  TT_EXTENDFLAG_IGNORE = 64,
};


static const thstok thtt_extendflag[] = {
  {"break", TT_EXTENDFLAG_BREAK},
  {"ignore", TT_EXTENDFLAG_IGNORE},
  {"left", TT_EXTENDFLAG_LEFT},
  {"normal", TT_EXTENDFLAG_NORMAL},
  {"reverse", TT_EXTENDFLAG_REVERSE},
  {"right", TT_EXTENDFLAG_RIGHT},
  {"start", TT_EXTENDFLAG_START},
  {NULL, TT_EXTENDFLAG_UNKNOWN},
};


/**
 * Data leg flags.
 */

enum {
  TT_LEGFLAG_NONE = 0,
  TT_LEGFLAG_SURFACE = 1,
  TT_LEGFLAG_DUPLICATE = 2,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_datalflag[] = {
  {"duplicate", TT_DATALFLAG_DUPLICATE},
  {"not", TT_DATALFLAG_NOT},
  {"surface", TT_DATALFLAG_SURFACE},
  {NULL, TT_DATALFLAG_UNKNOWN},
};


/**
 * Data station flags.
 */
 
enum {
  TT_DATASFLAG_UNKNOWN,
  TT_DATASFLAG_ENTRANCE,
  TT_DATASFLAG_CONT,
  TT_DATASFLAG_FIXED,
};

/**
 * Data leg flags.
 */

enum {
  TT_STATIONFLAG_NONE = 0,
  TT_STATIONFLAG_ENTRANCE = 1,
  TT_STATIONFLAG_CONT = 2,
  TT_STATIONFLAG_FIXED = 4,
  TT_STATIONFLAG_UNDERGROUND = 8,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_datasflag[] = {
  {"continuation", TT_DATASFLAG_CONT},
  {"entrance", TT_DATASFLAG_ENTRANCE},
  {NULL, TT_DATASFLAG_UNKNOWN},
};


/**
 * Main survey leg class.
 */

class thdataleg {

  public:
  
  bool is_valid;  ///< whether leg is valid.
  
  thobjectsrc srcf;  ///< Source file.
  
  int data_type,  ///< leg data type
      flags;  ///< Leg flags.
      
  unsigned char s_mark,  ///< Type of the station mark
    extend;  ///< Extend flags: normal, reverse, left, right, break

  int walls, shape;
  
  thobjectname station, from, to;
  class thsurvey * psurvey;  ///< parent survey
  
  double length, counter, fromcounter, tocounter, depth, fromdepth,
    todepth, depthchange, bearing, gradient, dx, dy, dz,
    backbearing, backgradient, total_length, total_bearing, total_gradient,
    total_dx, total_dy, total_dz, adj_dx, adj_dy, adj_dz,
    from_up, from_down, from_left, from_right,
    to_up, to_down, to_left, to_right, vtresh;
    
  double length_sd, counter_sd, depth_sd, bearing_sd, gradient_sd,
    dx_sd, dy_sd, dz_sd, x_sd, y_sd, z_sd, declination, 
    total_sdx, total_sdy, total_sdz, fxx, txx;
    
  bool infer_plumbs, infer_equates, direction, adjusted, to_be_adjusted, topofil, plumbed;
  
 
  /**
   * Standard constructor.
   */
  
  thdataleg();
  
  
  /**
   * Standard destructor.
   */
   
  ~thdataleg();
  
  
  /**
   * Clear survey leg data.
   */
  
  void clear();
  
  void calc_total_stds();
  
};


/**
 * Fix station class.
 */
 
class thdatafix {
  
  public:
  
  thobjectname station;  ///< Station name.

  thobjectsrc srcf;  ///< Source file.

  class thsurvey * psurvey;  ///< parent survey

  double x, y, z, sdx, sdy, sdz, cxy, cyz, cxz;  // data
  
  thdatafix();  ///< Standard constructor.
  
};


/**
 * Station class.
 */
 
class thdatass {
  
  public:
  
  thobjectname station;  ///< Station name.
  
  thobjectsrc srcf;  ///< Source file.

  class thsurvey * psurvey;  ///< parent survey

  char * comment;  ///< Station comment.  
  
  int flags;  ///< Station flags.
  
  thdatass();  ///< Standard constructor.
  
};


/**
 * Equate class.
 */
 
class thdataequate {
  
  public:
  
  thobjectname station;  ///< Station name.

  thobjectsrc srcf;  ///< Source file.

  class thsurvey * psurvey;  ///< parent survey

  int eqid;  ///< Station equivalency id.  

  thdataequate();  ///< Standard constructor.
  
};


class thdatamark {
	
	public:
	
	thobjectname station;

  class thsurvey * psurvey;  ///< parent survey

  thobjectsrc srcf;  ///< Source file.
	
	unsigned char mark;
	
	thdatamark() : mark(TT_DATAMARK_TEMP) {}
	
};


class thdataextend {
	
	public:
	
	thobjectname to, from;

  class thsurvey * psurvey;  ///< parent survey

  thobjectsrc srcf;  ///< Source file.
	
	int extend;
	
	thdataextend() {}
	
};



class thstdims {
  
  public:
  
	thobjectname station;

  class thsurvey * psurvey;  ///< parent survey

  thobjectsrc srcf;  ///< Source file.
  
  double up, down, left, right;
  
  thstdims() : up(thnan), down(thnan), left(thnan), right(thnan) {}
  
};


typedef std::list < thdataleg > thdataleg_list;  ///< Data leg list.

typedef std::list < thdatafix > thdatafix_list;  ///< Data fix list.

typedef std::list < thdatass > thdatass_list;  ///< Data stations list.

typedef std::list < thdataequate > thdataequate_list;  ///< Data equates list.

typedef std::list < thdataextend > thdataextend_list;  ///< Data extend list.

typedef std::list < thdatamark > thdatamark_list;  ///< Universal object name list.

typedef std::list < thstdims > thstdims_list;  ///< Station dimensions.

#endif


