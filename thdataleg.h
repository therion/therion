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
};


/**
 * survey leg parsing table.
 */
 
static const thstok thtt_dataleg_comp[] = {
  {"altitude", TT_DATALEG_ALTITUDE},
  {"backbearing", TT_DATALEG_BACKBEARING},
  {"backclino", TT_DATALEG_BACKGRADIENT},
  {"backcompass", TT_DATALEG_BACKBEARING},
  {"backgradient", TT_DATALEG_BACKGRADIENT},
  {"bearing", TT_DATALEG_BEARING},
  {"clino", TT_DATALEG_GRADIENT},
  {"compass", TT_DATALEG_BEARING},
  {"count", TT_DATALEG_COUNT},
  {"counter", TT_DATALEG_COUNT},
  {"depth", TT_DATALEG_DEPTH},
  {"depthchange", TT_DATALEG_DEPTHCHANGE},
  {"direction", TT_DATALEG_DIRECTION},
  {"dx", TT_DATALEG_EASTING},
  {"dy", TT_DATALEG_NORTHING},
  {"dz", TT_DATALEG_ALTITUDE},
  {"easting", TT_DATALEG_EASTING},
  {"from", TT_DATALEG_FROM},
  {"fromcount", TT_DATALEG_FROMCOUNT},
  {"fromcounter", TT_DATALEG_FROMCOUNT},
  {"fromdepth", TT_DATALEG_FROMDEPTH},
  {"gradient", TT_DATALEG_GRADIENT},
  {"length", TT_DATALEG_LENGTH},
  {"newline", TT_DATALEG_NEWLINE},
  {"northing", TT_DATALEG_NORTHING},
  {"notebook", TT_DATALEG_NOTES},
  {"notes", TT_DATALEG_NOTES},
  {"pictures", TT_DATALEG_PICTURES},
  {"position", TT_DATALEG_POSITION},
  {"station", TT_DATALEG_STATION},
  {"tape", TT_DATALEG_LENGTH},
  {"to", TT_DATALEG_TO},
  {"tocount", TT_DATALEG_TOCOUNT},
  {"tocounter", TT_DATALEG_TOCOUNT},
  {"todepth", TT_DATALEG_TODEPTH},
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
 * Data types.
 */
 
enum {
  TT_DATATYPE_NORMAL,
  TT_DATATYPE_DIVING,
  TT_DATATYPE_CARTESIAN,
  TT_DATATYPE_CYLPOLAR,
  TT_DATATYPE_NOSURVEY,
  TT_DATATYPE_UNKNOWN,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_datatype[] = {
  {"cartesian", TT_DATATYPE_CARTESIAN},
  {"cylpolar", TT_DATATYPE_CYLPOLAR},
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
      
  unsigned char s_mark;  ///< Type of the station mark
  
  thobjectname station, from, to;
  class thsurvey * psurvey;  ///< parent survey
  
  double length, counter, fromcounter, tocounter, depth, fromdepth,
    todepth, depthchange, bearing, gradient, dx, dy, dz,
    backbearing, backgradient, total_length;
    
  double length_sd, counter_sd, depth_sd, bearing_sd, gradient_sd,
    dx_sd, dy_sd, dz_sd, declination;
    
  bool infer_plumbs, infer_equates, direction;
  
 
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


typedef std::list < thdataleg > thdataleg_list;  ///< Data leg list.

typedef std::list < thdatafix > thdatafix_list;  ///< Data fix list.

typedef std::list < thdatass > thdatass_list;  ///< Data stations list.

typedef std::list < thdataequate > thdataequate_list;  ///< Data equates list.

#endif


