/**
 * @file thexpmodel.h
 * Export class.
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
 
#ifndef thexpmodel_h
#define thexpmodel_h


#include "thexport.h"
#include "thsurvey.h"


/**
 * Model export options.
 */

enum {
  TT_EXPMODEL_OPT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPMODEL_OPT_FORMAT,  ///< Output option.
  TT_EXPMODEL_OPT_ENABLE,  ///< Output option.
  TT_EXPMODEL_OPT_DISABLE,  ///< Output option.
  TT_EXPMODEL_OPT_WALLSRC,  ///< Output option.
  TT_EXPMODEL_OPT_ENCODING,  ///< Output encoding.
};


/**
 * Options parsing table.
 */
 
static const thstok thtt_expmodel_opt[] = {
  {"-disable", TT_EXPMODEL_OPT_DISABLE},
  {"-enable", TT_EXPMODEL_OPT_ENABLE},
  {"-enc", TT_EXPMODEL_OPT_ENCODING},
  {"-encoding", TT_EXPMODEL_OPT_ENCODING},
  {"-fmt", TT_EXPMODEL_OPT_FORMAT},
  {"-format", TT_EXPMODEL_OPT_FORMAT},
  {"-wall-source", TT_EXPMODEL_OPT_WALLSRC},
  {NULL, TT_EXPMODEL_OPT_UNKNOWN}
};

/**
 * Model export item options.
 */

enum {
  TT_EXPMODEL_ITEM_UNKNOWN = 0,
  TT_EXPMODEL_ITEM_WALLS = 1,
  TT_EXPMODEL_ITEM_CAVECENTERLINE = 2,
  TT_EXPMODEL_ITEM_SURFACECENTERLINE = 4,
  TT_EXPMODEL_ITEM_CENTERLINE = 6,
  TT_EXPMODEL_ITEM_SURFACE = 8,
  TT_EXPMODEL_ITEM_SPLAYSHOTS = 16,
  TT_EXPMODEL_ITEM_ENTRANCES = 32,
  TT_EXPMODEL_ITEM_ALL = 255,
};

/**
 * Options parsing table.
 */
 
static const thstok thtt_expmodel_items[] = {
  {"all", TT_EXPMODEL_ITEM_ALL},
  {"cave-centerline", TT_EXPMODEL_ITEM_CAVECENTERLINE},
  {"cave-centreline", TT_EXPMODEL_ITEM_CAVECENTERLINE},
  {"centerline", TT_EXPMODEL_ITEM_CENTERLINE},
  {"centreline", TT_EXPMODEL_ITEM_CENTERLINE},
  {"entrances", TT_EXPMODEL_ITEM_ENTRANCES},
  {"splay-shots", TT_EXPMODEL_ITEM_SPLAYSHOTS},
  {"surface", TT_EXPMODEL_ITEM_SURFACE},
  {"surface-centerline", TT_EXPMODEL_ITEM_SURFACECENTERLINE},
  {"surface-centreline", TT_EXPMODEL_ITEM_SURFACECENTERLINE},
  {"walls", TT_EXPMODEL_ITEM_WALLS},
  {NULL, TT_EXPMODEL_ITEM_UNKNOWN}
};





/**
 * Model export formats.
 */

enum {
  TT_EXPMODEL_FMT_UNKNOWN = 0,  ///< Unknown option
  TT_EXPMODEL_FMT_SURVEX,  ///< survex
  TT_EXPMODEL_FMT_COMPASS,  ///< compass
  TT_EXPMODEL_FMT_THERION,  ///< therion
  TT_EXPMODEL_FMT_LOCH,  ///< loch
  TT_EXPMODEL_FMT_3DMF,  ///< 3dmf
  TT_EXPMODEL_FMT_VRML,  ///< vrml
  TT_EXPMODEL_FMT_DXF,  ///< dxf
  TT_EXPMODEL_FMT_SHP,  ///< esri shapefile
  TT_EXPMODEL_FMT_KML,  ///< google earth format
};


/**
 * Format parsing table.
 */
 
static const thstok thtt_expmodel_fmt[] = {
  {"3d", TT_EXPMODEL_FMT_SURVEX},
  {"3dmf", TT_EXPMODEL_FMT_3DMF},
  {"compass", TT_EXPMODEL_FMT_COMPASS},
  {"dxf", TT_EXPMODEL_FMT_DXF},
  {"esri", TT_EXPMODEL_FMT_SHP},
  {"kml", TT_EXPMODEL_FMT_KML},
  {"loch", TT_EXPMODEL_FMT_LOCH},
  {"shapefile", TT_EXPMODEL_FMT_SHP},
  {"shapefiles", TT_EXPMODEL_FMT_SHP},
  {"shp", TT_EXPMODEL_FMT_SHP},
  {"survex", TT_EXPMODEL_FMT_SURVEX},
  {"vrml", TT_EXPMODEL_FMT_VRML},
//  {"xtherion", TT_EXPMODEL_FMT_THERION},
  {NULL, TT_EXPMODEL_FMT_UNKNOWN}
};



/**
 * Model wall sources.
 */

enum {
  TT_WSRC_UNKNOWN = 0, 
  TT_WSRC_CENTERLINE = 1,  
  TT_WSRC_MAPS = 2,
  TT_WSRC_SPLAYS = 4,
  TT_WSRC_SCANS = 8,
  TT_WSRC_ALL = 15,
};

/**
 * Options parsing table.
 */
 
static const thstok thtt_expmodel_wallsrc[] = {
  {"all", TT_WSRC_ALL},
  {"centerline", TT_WSRC_CENTERLINE},
  {"centreline", TT_WSRC_CENTERLINE},
  {"maps", TT_WSRC_MAPS},
  {"scans", TT_WSRC_SCANS},
  {"splays", TT_WSRC_SPLAYS},
  {NULL, TT_WSRC_UNKNOWN}
};


/**
 * Main export class.
 */
 
class thexpmodel : public thexport {

  public:

  unsigned items,
    wallsrc;

  bool is_leg_exported(class thdb1dl * l); ///< Whether to export leg.

  void export_3d_file(class thdatabase * dbp);  ///< Export survex 3d file.
  
  void export_plt_file(class thdatabase * dbp);  ///< Export compass plt file.
  
  //void export_thm_file(class thdatabase * dbp);  ///< Export therion model file.

  void export_vrml_file(class thdatabase * dbp);  ///< Export vrml file.

  void export_3dmf_file(class thdatabase * dbp);  ///< Export 3dmf file.

  void export_dxf_file(class thdatabase * dbp);  ///< Export compass plt file.

  void export_lox_file(class thdatabase * dbp);  ///< Export lox file.

  void export_shp_file(class thdatabase * dbp);  ///< Export shp file.

  void export_kml_file(class thdatabase * dbp);  ///< Export kml file.

  void export_kml_survey_file(FILE * out, thsurvey * surv);  ///< Helper function to export part of the survey tree

  public:
  
  thexpmodel(); ///< Default constructor.

  virtual ~thexpmodel(); ///< Default destructor.

  /**
   * Parse model export options.
   */
   
  void parse_options(int & argx, int nargs, char ** args) override;

  
  /**
   * Dump object into file.
   */
   
  void dump_body(FILE * xf) override;


  /**
   * Dump object into file.
   */
   
  void dump_header(FILE * xf) override;
  

  /**
   * Make export.
   */
   
  void process_db(class thdatabase * dbp) override;
  
};


#endif


