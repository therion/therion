/**
 * @file thdb1d.h
 * Survey data structure processing class.
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
 
#ifndef thdb1d_h
#define thdb1d_h

#include "thobjectid.h"
#include "thinfnan.h"
#include "thdataleg.h"
#include "thdb3d.h"
#include "thdataleg.h"
#include "thattr.h"
#include <map>
#include <vector>
#include <list>

enum {
  TT_TEMPSTATION_NONE,
  TT_TEMPSTATION_WALL,
  TT_TEMPSTATION_FEATURE,
};

struct thdb1d_loop_leg {
  thdb1d_loop_leg * next_leg, * prev_leg;
  thdataleg * leg;
  bool reverse;
};

struct thdb1d_loop {
  thdb1d_loop_leg * first_leg, * last_leg;
  class thdb1ds * from, * to;
  unsigned long nlegs;
  unsigned long id;
  bool open;
  double err_dx, err_dy, err_dz, err_length, src_length, err;
};

struct thdb1d_traverse {
  thdb1d_loop_leg * first_leg, * last_leg;
  class thdb1ds * from, * to;
  unsigned long nlegs;
  unsigned long id;
  double src_length, E, H, V;
};

class thdb1d_tree_node {

  public:


  bool is_attached, is_fixed;
  int xx_left;
  bool xx_touched, extendx_ok;

  thdb1d_tree_node * next_eq, * prev_eq;
  
  unsigned long id, uid, narrows;
  
  double xx, extendx, xx_ratio;
  
  class thdb1d_tree_arrow * first_arrow, * last_arrow, * back_arrow;
  std::list<class thdb1d_tree_arrow * > back_buffer;
  void push_back_arrow(class thdb1d_tree_arrow * arrow);
  class thdb1d_tree_arrow * pop_back_arrow();
  
  thdb1d_tree_node() : is_attached(false), is_fixed(false), xx_left(1), xx_touched(false), extendx_ok(false),
    next_eq(NULL), prev_eq(NULL),
    id(0), uid(0), narrows(0), xx(0.0), extendx(0.0), xx_ratio(1.0),
    first_arrow(NULL), last_arrow(NULL), back_arrow(NULL) {}
  
};



class thdb1d_tree_arrow {

  public:

  bool is_discovery, is_nosurvey, is_reversed;
  
  thdb1d_tree_node * start_node, * end_node;
  
  class thdb1dl * leg;
  
  unsigned extend;

  std::list<unsigned long> extend_ignore_before;
  
  thdb1d_tree_arrow * negative;
  
  thdb1d_tree_arrow * next_arrow;
  
  thdb1d_tree_arrow() : is_discovery(false), is_nosurvey(false), is_reversed(false),
    start_node(NULL), end_node(NULL), 
    leg(NULL), extend(TT_EXTENDFLAG_NORMAL), negative(NULL), next_arrow(NULL) {}
  
};



/**
 * Stations search map.
 */
 
typedef std::map <thobjectid, unsigned long> thdb1d_station_map_type;


/**
 * Station information class.
 */
 
class thdb1ds {

  public:
  
  unsigned long uid;  ///< Unique ID of a station.
  
  double x, y, z;   // coordinates
  double xx;        // coordinate for extended elevation
  double asl; // average cave shot length from this point
  
  const char * name,  ///< Station name.
    * comment;  ///< Station comment.
  
  class thsurvey * survey;  ///< Station survey.
  class thdataobject * fixcontext;  ///< Fix context.
  bool tmpselect;
  
  std::list<class thdata *> data;  ///< Station data.
  unsigned data_priority; ///< 0 - undefined, 1 - equate, 2 - leg, 3 - fix
  
  char temps;           ///< Station temporary status.
  unsigned long flags;  ///< Station flags.
  int mark,             ///< Mark type.
    extend;             ///< Extend flags: normal, reverse, left, right, break
  double extend_ratio;
  bool mark_station;
  
  bool adjusted, fixed;
  int placed;
  double sdx, sdy, sdz;
  
  double explored;
  
  bool d3_parsed; ///< Whether splay shots have been parsed.
  thdb3ddata d3_outline; ///< splay 3d outline.


  /**
   * Default constructor.
   */
   
  thdb1ds() : uid(0), x(0), y(0), z(0), xx(0), asl(0), name(NULL), comment(NULL), survey(NULL), fixcontext(NULL), tmpselect(false),
    data_priority(0), temps(TT_TEMPSTATION_NONE),
    flags(TT_STATIONFLAG_NONE), mark(TT_DATAMARK_TEMP), extend(TT_EXTENDFLAG_NORMAL), extend_ratio(thnan), mark_station(false),
    adjusted(false), fixed(false), placed(0), sdx(0.0), sdy(0.0), sdz(0.0),
    explored(thnan), d3_parsed(false) {}
  

  /**
   * Default constructor with 2 parameters.
   */
   
  thdb1ds(const char * n, class thsurvey * ps) : uid(0), x(0), y(0), z(0), xx(0), asl(0), name(n), 
    comment(NULL), survey(ps), fixcontext(NULL), tmpselect(false),
    data_priority(0), temps(TT_TEMPSTATION_NONE),
    flags(TT_STATIONFLAG_NONE),
    mark(TT_DATAMARK_TEMP), extend(TT_EXTENDFLAG_NORMAL), extend_ratio(thnan), mark_station(false),
    adjusted(false), fixed(false), placed(0), sdx(0.0), sdy(0.0), sdz(0.0),
    explored(thnan), d3_parsed(false) {}
    
  
  /**
   * Set parent data.
   */
  
  void set_parent_data(class thdata * pd, unsigned pd_priority);
  

  /**
   * Is station temporary.
   */
  bool is_temporary() {return (temps != TT_TEMPSTATION_NONE);}

  /**
   * Set temporary status.
   */
  void set_temporary(char ts) {temps = ts;}

  /**
   * Set temporary status.
   */
  void set_temporary(const char * name);

  /**
   * Export station attributes to metapost.
   */

  void export_mp_flags(FILE * out);
  

  /**
   * Return 3D splay outline. Process if necessary.
   */

  thdb3ddata * get_3d_outline();


};


/**
 * Stations vector.
 */
 
typedef std::vector < thdb1ds > thdb1d_station_vec_type;


/**
 * Leg information class.
 */
 
class thdb1dl {

  public:
  
  class thdataleg * leg;
  class thdata * data;
  class thsurvey * survey;
  
  bool reverse;
  
  unsigned long series_id, component_id;
  
  thdb1dl() : leg(NULL), data(NULL), survey(NULL), reverse(false),
    series_id(0), component_id(0) {}
  
  thdb1dl(class thdataleg * l, class thdata * d, class thsurvey * s) : 
    leg(l), data(d), survey(s), reverse(false),
    series_id(0), component_id(0)  {}
  
};


/**
 * Legs vector.
 */
 
typedef std::vector < thdb1dl > thdb1d_leg_vec_type;
typedef std::list < thdb1d_loop_leg > thdb1d_loop_leg_list_type;
typedef std::list < thdb1d_loop > thdb1d_loop_list_type;
typedef std::list < thdb1d_traverse > thdb1d_traverse_list_type;



/**
 * Survey data structure processing class.
 */
 
class thdb1d {

  public:

  class thdatabase * db;  ///< Our database.
  
  thdb1dl ** tree_legs; ///< Tree structure.
  
  thdb1d_tree_arrow * tree_arrows;
  
  thdb1d_tree_node  * tree_nodes;
  
  thdb3ddata d3_data, d3_surface, d3_walls, d3_splay;

  thattr m_station_attr;

  double min_year, max_year;
  
  bool d3_data_parsed;
  
  unsigned long num_tree_legs; ///< Number of legs in the tree.
  
  unsigned long lsid;  ///< Last station id.
  
  double nlength;
  
  /**
   * Scan survey stations.
   */
   
  void scan_data();
  
  /**
   * Process survey data tree.
   */
   
  void process_tree();

  /**
   * Insert station into mapping.
   */
   
  unsigned long insert_station(class thobjectname on, class thsurvey * ps, class thdata * pd, unsigned priority);
  
  /**
   * Process survey statistics.
   */
   
  void process_survey_stat();
  
  void postprocess_objects();

  public:
  
  thdb1d_station_vec_type station_vec;  ///< Stations data.

  thdb1d_station_map_type station_map;  ///< Stations map.
  
  thdb1d_leg_vec_type leg_vec;  ///< Survey shots.
  
  thdb1d_loop_leg_list_type loop_leg_list;

  thdb1d_loop_list_type loop_list;

  thdb1d_traverse_list_type traverse_list;

  /**
   * Standard constructor.
   */
  
  thdb1d();
  
  
  /**
   * Destructor.
   */
  
  ~thdb1d();
  
  
  /**
   * Assign database pointer.
   */
   
  void assigndb(thdatabase * dbptr);
  

  /**
   * Make a loop closure and calculate stations coordinates.
   */
  
  void process_data();


  /**
   * Return station id if exist, 0 otherwise.
   */
   
  unsigned long get_station_id(class thobjectname on, class thsurvey * ps);
  
  
  /**
   * Print self.
   */
   
  void self_print(FILE * outf);
  
  
  /**
   * Return number of legs in the tree.
   */
   
  unsigned long get_tree_size();
   
    
  /**
   * Return tree legs.
   */
   
  thdb1dl ** get_tree_legs();
   
  thdb1d_tree_arrow * get_tree_arrows() {return this->tree_arrows;}
  
  thdb1d_tree_node * get_tree_nodes() {return this->tree_nodes;}
  
  /**
   * Find loops.
   */

  void find_loops();

  /**
   * Make loop closure and calculate station coordinates.
   */
  
  void close_loops();
  
  void print_loops();
  
  void process_xelev();
  
  thdb3ddata * get_3d();
  thdb3ddata * get_3d_splay();
  thdb3ddata * get_3d_surface();
  thdb3ddata * get_3d_walls();
    
};


#endif


