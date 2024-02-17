/**
 * @file thdb1d.cxx
 */
  
/* Copyright (C) 2004 Stacho Mudrak
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
 
#include "thdb1d.h"
#include "thsurvey.h"
#include "thobjectname.h"
#include <stdlib.h>
#include "thsvxctrl.h"
#include "thdataobject.h"
#include "thdatabase.h"
#include "thdataleg.h"
#include "thexception.h"
#include "thdata.h"
#include "thinfnan.h"
#include <math.h>
#include <set>
#include <algorithm>
#include "thpoint.h"
#include "thlogfile.h"
#include "thsurface.h"
#include "thlocale.h"
#include "thinit.h"
#include "thconfig.h"
#include "thtrans.h"
#include "loch/lxMath.h"
#include "thcsdata.h"
#include "thgeomagdata.h"
#include "QuickHull.hpp"

//#define THUSESVX
//#define THDEBUG

thdb1d::thdb1d()
{
  this->db = NULL;
  this->tree_legs = NULL;
  this->num_tree_legs = 0;
  this->lsid = 0;
  this->nlength = 0.0;
  
  this->tree_arrows = NULL;
  this->tree_nodes = NULL;

  this->min_year = thnan;
  this->max_year = thnan;
  
  this->d3_data_parsed = false;
}

void thdb1ds::export_mp_flags(FILE * out)
{
  fprintf(out, "ATTR__stationflag_splay := %s;\n", (this->is_temporary() ? "true" : "false"));
}

void thdb1ds::set_temporary(const char * name)
{
  temps = TT_TEMPSTATION_NONE;
  if (name != NULL) {
    switch (name[0]) {
      case '.':
        temps = TT_TEMPSTATION_FEATURE;
        break;
      case '-':
        temps = TT_TEMPSTATION_WALL;
        break;
    }
  }
}


thdb1d::~thdb1d()
{
  delete [] this->tree_legs;
  delete [] this->tree_arrows;
  delete [] this->tree_nodes;
}


void thdb1d::assigndb(thdatabase * dbptr)
{
  this->db = dbptr;
}


void thdb1d::scan_data()
{
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  thdataleg_list::iterator lei;
  thdatafix_list::iterator fii;
  thdatamark_list::iterator mii;
  thdatass_list::iterator ssi;
  thdataextend_list::iterator xi;
  thdataequate_list::iterator eqi;
  thstdims_list::iterator di;
  double meridian_convergence = thcfg.get_outcs_convergence(), declin, leggridmc, lastleggridmc(0.0);
  int lastleggridmccs = TTCS_LOCAL;
  thdata * dp;
  unsigned used_declination = 0;
  unsigned long prevlsid;
  double dcc, sindecl, cosdecl, tmpx, tmpy;
  thdb1ds * tsp1, * tsp2;  // Temporary stations.
  this->min_year = thnan;
  this->max_year = thnan;

  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = dynamic_cast<thdata*>(obi->get());
      if (dp->date.is_defined()) {
        double syear, eyear;
        syear = dp->date.get_start_year();
        eyear = syear;
        if (dp->date.is_interval())
          eyear = dp->date.get_end_year();
        if (thisnan(this->min_year)) {
          this->min_year = syear;
          this->max_year = eyear;
        } else {
          if (this->min_year > syear) this->min_year = syear;
          if (this->max_year < eyear) this->max_year = eyear;
        }
      }
    }
    obi++;
  }

  double default_dpdeclin;
  bool default_dpdeclinused;
  default_dpdeclin = 0.0;
  default_dpdeclinused = false;
  if (!thisnan(this->min_year)) {
    thcfg.get_outcs_mag_decl(this->min_year, default_dpdeclin);
    default_dpdeclinused = true;
  }

  std::set<std::string> undated_surveys_set;

  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = dynamic_cast<thdata*>(obi->get());
      bool dpdeclindef;
      double dpdeclin;

      dpdeclin = 0.0;
      dpdeclindef = false;
      if (dp->date.is_defined() && (thcfg.get_outcs_mag_decl(dp->date.get_average_year(), dpdeclin)))
        dpdeclindef = true;      
        
      // scan data shots
      lei = dp->leg_list.begin();
      try {
        while(lei != dp->leg_list.end()) {
          if (lei->is_valid) {
            // thdataleg * lf = &(*lei);
            lei->from.id = this->insert_station(lei->from, lei->psurvey, dp, 3);
            lei->to.id = this->insert_station(lei->to, lei->psurvey, dp, 3);
            if (((strcmp(lei->from.name,".") == 0) || (strcmp(lei->from.name,"-") == 0)) && ((strcmp(lei->to.name,".") == 0) || (strcmp(lei->to.name,"-") == 0)))
              ththrow("shot between stations without names not allowed");
            if ((strcmp(lei->from.name,"-") == 0) || (strcmp(lei->to.name,"-") == 0) || (strcmp(lei->from.name,".") == 0) || (strcmp(lei->to.name,".") == 0)) {
              lei->flags |= TT_LEGFLAG_SPLAY;
              lei->walls = TT_FALSE;
            }
            this->leg_vec.push_back(thdb1dl(&(*lei),dp,lei->psurvey));
            
            // check station marks
            tsp1 = &(this->station_vec[lei->from.id - 1]);
            tsp2 = &(this->station_vec[lei->to.id - 1]);
            if (lei->s_mark > (unsigned int)tsp1->mark)
              tsp1->mark = lei->s_mark;
            if (lei->s_mark > (unsigned int)tsp2->mark)
              tsp2->mark = lei->s_mark;
            
            // set underground station flag
            if ((lei->flags & TT_LEGFLAG_SURFACE) == 0) {
              tsp1->flags |= TT_STATIONFLAG_UNDERGROUND;
              tsp2->flags |= TT_STATIONFLAG_UNDERGROUND;
            }
            

            // check the length            
            if ((lei->data_type == TT_DATATYPE_NORMAL) ||
                (lei->data_type == TT_DATATYPE_DIVING) ||
                (lei->data_type == TT_DATATYPE_CYLPOLAR)) {
              if (thisnan(lei->length) && (!thisnan(lei->tocounter)) &&
                  (!thisnan(lei->fromcounter))) {
                lei->length = lei->tocounter - lei->fromcounter;
                lei->length_sd = lei->counter_sd;
              }
            }
            // check depth change
            if (lei->data_type == TT_DATATYPE_DIVING) {
              if (!thisnan(lei->depthchange))
                dcc = lei->depthchange;
              else
                dcc = lei->todepth - lei->fromdepth;
              lei->depthchange = dcc;  
              if (thdefinitely_greater_than(fabs(dcc),lei->length,1e-6))
                ththrow("length reading is less than change in depth -- {}", fabs(dcc)-lei->length);
            }
            
            // check backwards compass reading
            if ((lei->data_type == TT_DATATYPE_NORMAL) ||
                (lei->data_type == TT_DATATYPE_DIVING) ||
                (lei->data_type == TT_DATATYPE_CYLPOLAR)) {
                if (!thisnan(lei->backbearing)) {
                if (thisnan(lei->bearing)) {
                  lei->backbearing -= 180.0;
                  if (lei->backbearing < 0)
                    lei->backbearing += 360.0;
                  lei->bearing = lei->backbearing;
                } 
                else {
                  lei->backbearing -= 180.0;
                  if (lei->backbearing < 0)
                    lei->backbearing += 360.0;
                  double bearing_diff;
                  if (lei->bearing > lei->backbearing)
                	  bearing_diff = lei->bearing - lei->backbearing;
                  else
                	  bearing_diff = lei->backbearing - lei->bearing;
                  if (bearing_diff > 180.0)
                	  bearing_diff -= 360.0;
                  double bearing_sd;
                  if (thisnan(lei->bearing_sd))
                	  bearing_sd = 1.25;
                  else
                	  bearing_sd = lei->bearing_sd;
                  if (abs(bearing_diff) > 3.0 * bearing_sd)
                  	thwarning(("%s [%lu] -- forwards and backwards bearing readings do not match -- %.2f > %.2f", lei->srcf.name, lei->srcf.line, bearing_diff, 2.0 * bearing_sd));
									// calculate average of two angles
                  //lei->bearing += lei->backbearing;
                  //lei->bearing = lei->bearing / 2.0;
									double sumx, sumy;
									sumx = cos((90.0 - lei->bearing)/180.0*THPI) + cos((90.0 - lei->backbearing)/180.0*THPI);
									sumy = sin((90.0 - lei->bearing)/180.0*THPI) + sin((90.0 - lei->backbearing)/180.0*THPI);
									lei->bearing = 90.0 - (atan2(sumy, sumx) / THPI * 180.0);
                  if (lei->bearing < 0.0)
                    lei->bearing += 360.0;
                }
              }
            }

            // check backwards gradient reading
            if (lei->data_type == TT_DATATYPE_NORMAL) {
              if (!thisnan(lei->backgradient)) {
                if (thisnan(lei->gradient)) {
                  lei->backgradient = - lei->backgradient;
                  lei->gradient = lei->backgradient;
                } 
                else {
                  if ((thisinf(lei->gradient) == 0) && 
                      (thisinf(lei->backgradient) == 0)) {
                    lei->backgradient = - lei->backgradient;
                    double gradient_sd;
                    if (thisnan(lei->gradient_sd))
                  	  gradient_sd = 1.25;
                    else
                  	  gradient_sd = lei->gradient_sd;
                    if (abs(lei->backgradient - lei->gradient) > (3.0 * gradient_sd))
                    	thwarning(("%s [%lu] -- forwards and backwards gradient readings do not match", lei->srcf.name, lei->srcf.line));
                    lei->gradient += lei->backgradient;
                    lei->gradient = lei->gradient / 2.0;
                  }
                  else {
                    if (thisinf(lei->gradient) != -thisinf(lei->backgradient))
                      ththrow("invalid plumbed shot");
                  }
                }
              }
            }

            // check backwards length reading
            if ((lei->data_type == TT_DATATYPE_NORMAL) ||
                (lei->data_type == TT_DATATYPE_DIVING) ||
                (lei->data_type == TT_DATATYPE_CYLPOLAR)) {
              if (!thisnan(lei->backlength)) {
                if (thisnan(lei->length)) {
                  lei->length = lei->backlength;
                } else {
                  double length_sd;
                  if (thisnan(lei->length_sd))
                	  length_sd = 0.25;
                  else
                	  length_sd = lei->length_sd;
				  if (abs(lei->backlength - lei->length) > (3.0 * length_sd))
					thwarning(("%s [%lu] -- forwards and backwards length readings do not match", lei->srcf.name, lei->srcf.line));
                  lei->length += lei->backlength;
                  lei->length /= 2.0;
                }
              }
            }

            
            // calculate leg total length and std
            switch (lei->data_type) {
              case TT_DATATYPE_NORMAL:
                lei->total_length = lei->length;
                lei->total_bearing = (thisnan(lei->bearing) ? 0.0 : lei->bearing);
                if (!lei->direction) {
                  lei->total_bearing += 180.0;
                  if (lei->total_bearing >= 360.0)
                    lei->total_bearing -= 360.0;
                }               
                lei->total_gradient = (thisinf(lei->gradient) == 1 ? 90.0 
                  : (thisinf(lei->gradient) == -1 ? -90.0 
                  : lei->gradient));
                                
                if (!lei->direction)
                  lei->total_gradient *= -1.0;
                lei->total_dz = lei->total_length * cos(lei->total_gradient/180*THPI);
                lei->total_dx = lei->total_dz * sin(lei->total_bearing/180*THPI);
                lei->total_dy = lei->total_dz * cos(lei->total_bearing/180*THPI);
                lei->total_dz = lei->total_length * sin(lei->total_gradient/180*THPI);
                lei->plumbed = (thisinf(lei->gradient) != 0);
                if (lei->infer_plumbs && (!lei->plumbed)) {
                  lei->plumbed = (lei->gradient == -90.0) || (lei->gradient == 90.0);
                }
                break;
                
              case TT_DATATYPE_DIVING:
                lei->total_length = lei->length;
                lei->total_bearing = (thisnan(lei->bearing) ? 0.0 : lei->bearing);
                if (!lei->direction) {
                  lei->total_bearing += 180.0;
                  if (lei->total_bearing >= 360.0)
                    lei->total_bearing -= 360.0;
                }               
                lei->total_gradient = asin(lei->depthchange / lei->length) / THPI * 180.0;
                if (!lei->direction)
                  lei->total_gradient *= -1.0;
                lei->total_dz = lei->total_length * cos(lei->total_gradient/180*THPI);
                lei->total_dx = lei->total_dz * sin(lei->total_bearing/180*THPI);
                lei->total_dy = lei->total_dz * cos(lei->total_bearing/180*THPI);
                lei->total_dz = lei->total_length * sin(lei->total_gradient/180*THPI);
                if (lei->infer_plumbs && (!lei->plumbed)) {
                  lei->plumbed = (lei->depthchange == lei->length) && (lei->depthchange != 0.0);
                }
                break;
                
              case TT_DATATYPE_CYLPOLAR:
                lei->total_length = sqrt(thnanpow2(lei->length) + thnanpow2(lei->depthchange));
                lei->total_bearing = (thisnan(lei->bearing) ? 0.0 : lei->bearing);
                if (!lei->direction) {
                  lei->total_bearing += 180.0;
                  if (lei->total_bearing >= 360.0)
                    lei->total_bearing -= 360.0;
                }               
                lei->total_gradient = atan2(lei->depthchange, lei->length) / THPI * 180.0;
                if (!lei->direction)
                  lei->total_gradient *= -1.0;
                lei->total_dz = lei->total_length * cos(lei->total_gradient/180*THPI);
                lei->total_dx = lei->total_dz * sin(lei->total_bearing/180*THPI);
                lei->total_dy = lei->total_dz * cos(lei->total_bearing/180*THPI);
                lei->total_dz = lei->total_length * sin(lei->total_gradient/180*THPI);
                if (lei->infer_plumbs && (!lei->plumbed)) {
                  lei->plumbed = (lei->length == 0.0) && (lei->depthchange != 0.0);
                }
                break;
                
              case TT_DATATYPE_CARTESIAN:
                lei->total_dx = (lei->direction ? 1.0 : -1.0) * lei->dx;
                lei->total_dy = (lei->direction ? 1.0 : -1.0) * lei->dy;
                lei->total_dz = (lei->direction ? 1.0 : -1.0) * lei->dz;
                lei->total_length = thdxyz2length(lei->total_dx,lei->total_dy,lei->total_dz);
                lei->total_bearing = thdxyz2bearing(lei->total_dx,lei->total_dy,lei->total_dz);
                lei->total_gradient = thdxyz2clino(lei->total_dx,lei->total_dy,lei->total_dz);
                if (lei->infer_plumbs && (!lei->plumbed)) {
                  lei->plumbed = (lei->dx == 0.0) && (lei->dy == 0.0) && (lei->dz != 0.0);
                }
                break;
            }

            leggridmc = 0.0;
            if (lei->gridcs != TTCS_LOCAL) {
              if (lei->gridcs != lastleggridmccs) {
                if (lei->gridcs == thcfg.outcs) {
                  lastleggridmc = meridian_convergence;
                } else {
                  lastleggridmc = thcfg.get_cs_convergence(lei->gridcs);
                }
                lastleggridmccs = lei->gridcs;
              }
              leggridmc = lastleggridmc;
            }

            if ((leggridmc != 0.0) || (meridian_convergence != 0.0) || (!thisnan(lei->declination)) || dpdeclindef) {
              declin = meridian_convergence - leggridmc;
              if (!thisnan(lei->declination)) {
                declin += lei->declination;
                used_declination |= 2;
              } else if (dpdeclindef) {
                lei->implicit_declination = dpdeclin;
                declin += dpdeclin;
                used_declination |= 4;
              } else {
                undated_surveys_set.insert(dp->fsptr->full_name);
                lei->implicit_declination = default_dpdeclin;
                declin += default_dpdeclin;
                used_declination |= 1;
              }
              lei->total_bearing += declin;
              if (lei->total_bearing >= 360.0)
                lei->total_bearing -= 360.0;
              if (lei->total_bearing < 0.0)
                lei->total_bearing += 360.0;
              cosdecl = cos(declin/180*THPI);
              sindecl = sin(declin/180*THPI);
              tmpx = lei->total_dx;
              tmpy = lei->total_dy;
              lei->total_dx = (cosdecl * tmpx) + (sindecl * tmpy);
              lei->total_dy = (cosdecl * tmpy) - (sindecl * tmpx);
            }
            
            lei->adj_dx = lei->total_dx;
            lei->adj_dy = lei->total_dy;
            lei->adj_dz = lei->total_dz;
            
            lei->calc_total_stds();
            
          }
          
          lei++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", lei->srcf.name, lei->srcf.line);
      }
          
      // scan data fixes
      fii = dp->fix_list.begin();
      thdb1ds * tmps;
      unsigned olevel, nlevel;
      try {
        while(fii != dp->fix_list.end()) {
          fii->station.id = this->insert_station(fii->station, fii->psurvey, dp, 2);
          tmps = &(this->station_vec[fii->station.id - 1]);
          tmps->flags |= TT_STATIONFLAG_FIXED;
          if (fii->srcf.context != NULL) {
            if (tmps->fixcontext != NULL) {
              if (tmps->fixcontext->fsptr == NULL)
                olevel = 0;
              else
                olevel = tmps->fixcontext->fsptr->level;
              if (fii->srcf.context->fsptr == NULL)
                nlevel = 0;
              else
                nlevel = fii->srcf.context->fsptr->level;
              if (nlevel <= olevel) {
                tmps->fixcontext = fii->srcf.context;
              }
            } else {
              tmps->fixcontext = fii->srcf.context;
            }
          }
          fii++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", fii->srcf.name, fii->srcf.line);
      }
    }
  
    obi++;
  }

  // process equates separately
  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = dynamic_cast<thdata*>(obi->get());
      // scan data equates
      eqi = dp->equate_list.begin();
      try {
        while(eqi != dp->equate_list.end()) {
          prevlsid = this->lsid;
          eqi->station.id = this->insert_station(eqi->station, eqi->psurvey, dp, 1);
          if ((prevlsid < eqi->station.id) && (eqi->station.survey != NULL))
            thwarning(("%s [%d] -- equate used to define new station (%s@%s)", eqi->srcf.name, eqi->srcf.line, eqi->station.name, eqi->station.survey));
          eqi++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", eqi->srcf.name, eqi->srcf.line);
      }
		}
    obi++;
	}


  // after scanning
  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {

    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      thsurvey * srv = dynamic_cast<thsurvey*>(obi->get());
      if (!srv->entrance.is_empty()) {
        srv->entrance.id = this->get_station_id(srv->entrance, srv);
        if (srv->entrance.id == 0) {
          if (srv->entrance.survey == NULL)
            ththrow("station doesn't exist -- {}", srv->entrance.name);
          else
            ththrow("station doesn't exist -- {}@{}", srv->entrance.name, srv->entrance.survey);
        }
        // set entrance flag & comment to given station
        this->station_vec[srv->entrance.id-1].flags |= TT_STATIONFLAG_ENTRANCE;            
        if (strlen(srv->title) > 0)
          this->station_vec[srv->entrance.id-1].comment = srv->title;
        else
          this->station_vec[srv->entrance.id-1].comment = srv->name;
      }
    }


    if ((*obi)->get_class_id() == TT_DATA_CMD) {

      dp = dynamic_cast<thdata*>(obi->get());

      // scan data stations
      ssi = dp->ss_list.begin();
      try {
        while(ssi != dp->ss_list.end()) {
          ssi->station.id = this->get_station_id(ssi->station, ssi->psurvey);
          if (ssi->station.id == 0) {
            if (ssi->station.survey == NULL)
              ththrow("station doesn't exist -- {}", ssi->station.name);
            else
              ththrow("station doesn't exist -- {}@{}", ssi->station.name, ssi->station.survey);
          }
          // set station flags and comment
          else {
            if (ssi->comment != NULL)
              this->station_vec[ssi->station.id-1].comment = ssi->comment;
            this->station_vec[ssi->station.id-1].explored = ssi->explored;
            this->station_vec[ssi->station.id-1].flags |= ssi->flags;            
            // set station attributes
            this->m_station_attr.insert_object(nullptr, (long) ssi->station.id);
            thdatass_attr_map::iterator ami;
            for(ami = ssi->attr.begin(); ami != ssi->attr.end(); ami++) {
              this->m_station_attr.insert_attribute(ami->first.c_str(), ami->second);
            }
          }
          ssi++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", ssi->srcf.name, ssi->srcf.line);
      }

      // scan data marks
      mii = dp->mark_list.begin();
      try {
        while(mii != dp->mark_list.end()) {
          mii->station.id = this->get_station_id(mii->station, mii->psurvey);
          if (mii->station.id == 0) {
            if (mii->station.survey == NULL)
              ththrow("station doesn't exist -- {}", mii->station.name);
            else
              ththrow("station doesn't exist -- {}@{}", mii->station.name, mii->station.survey);
          }
          // set station flags and comment
          else {
            thdb1ds * markst = &(this->station_vec[mii->station.id - 1]);
            markst->mark_station = true;
            if (markst->mark < mii->mark)
              markst->mark = mii->mark;
          }
          mii++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", mii->srcf.name, mii->srcf.line);
      }
        
      // scan extends
      xi = dp->extend_list.begin();
      try {
        while(xi != dp->extend_list.end()) {
          if (!xi->before.is_empty()) {
            xi->before.id = this->get_station_id(xi->before, xi->psurvey);
            if (xi->before.id == 0) {
              if (xi->before.survey == NULL)
                ththrow("station doesn't exist -- {}", xi->before.name);
              else
                ththrow("station doesn't exist -- {}@{}", xi->before.name, xi->before.survey);
            }
          }
          if (!xi->to.is_empty()) {
            xi->to.id = this->get_station_id(xi->to, xi->psurvey);
            if (xi->to.id == 0) {
              if (xi->to.survey == NULL)
                ththrow("station doesn't exist -- {}", xi->to.name);
              else
                ththrow("station doesn't exist -- {}@{}", xi->to.name, xi->to.survey);
            }
          }
          xi->from.id = this->get_station_id(xi->from, xi->psurvey);
          if (xi->from.id == 0) {
            if (xi->from.survey == NULL)
              ththrow("station doesn't exist -- {}", xi->from.name);
            else
              ththrow("station doesn't exist -- {}@{}", xi->from.name, xi->from.survey);
          }
          xi++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", xi->srcf.name, xi->srcf.line);
      }

      // scan dimensions
      di = dp->dims_list.begin();
      try {
        while(di != dp->dims_list.end()) {
          di->station.id = this->get_station_id(di->station, di->psurvey);
          if (di->station.id == 0) {
            if (di->station.survey == NULL)
              ththrow("station doesn't exist -- {}", di->station.name);
            else
              ththrow("station doesn't exist -- {}@{}", di->station.name, di->station.survey);
          }
          di++;
        }
      }
      catch (...) {
        threthrow("{} [{}]", di->srcf.name, di->srcf.line);
      }
        
      dp->complete_dimensions();

    }  
    obi++;
  }

  if (((used_declination & 1) != 0) && ((used_declination & 4) != 0)) {
    if (default_dpdeclinused)
      thwarning(("year %.0f magnetic declination used for undated surveys", this->min_year))
    else
      thwarning(("unable to determine magnetic declination used for undated surveys"))
    thprintf("undated surveys:\n");
    for(auto usi = undated_surveys_set.begin(); usi != undated_surveys_set.end(); usi++) {
      thprintf(usi->c_str());
      thprintf("\n");
    }
  }

  if (thcfg.m_decl_out_of_geomag_range)
    thwarning(("magnetic declination calculated for dates outside of optimal model range (%d - %d)", thgeomag_minyear, thgeomag_minyear + thgeomag_step * (thgeomag_maxmindex + 1) - 1))

  thcfg.log_outcs(this->min_year, this->max_year);

}


void thdb1d::process_data()
{
  this->scan_data();
  this->find_loops();
  this->close_loops();
	if (thini.loopc == THINIT_LOOPC_SURVEX) {
	  thsvxctrl survex;
		try {
		  survex.process_survey_data(this->db);
	  } catch (const std::exception& e) {
			thwarning((e.what()))
		}
	}
  this->process_survey_stat();
  this->postprocess_objects();
  this->process_xelev();
}


unsigned long thdb1d::get_station_id(thobjectname on, thsurvey * ps)
{
  unsigned long csurvey_id = this->db->get_survey_id(on.survey, ps);

  thdb1d_station_map_type::iterator sti;
  sti = this->station_map.find(thobjectid(on.name, csurvey_id)); 
  if (sti == this->station_map.end())
    return 0;
  else
    return sti->second;
        
}

void thdb1ds::set_parent_data(class thdata * pd, unsigned pd_priority) {
	if (pd_priority > this->data_priority) {
		this->data.clear();
		this->data_priority = pd_priority;
	}
	if (pd_priority == this->data_priority)
		this->data.push_back(pd);
}


unsigned long thdb1d::insert_station(class thobjectname on, class thsurvey * ps, class thdata * pd, unsigned pd_priority)
{
  bool is_temp = false;
  if ((strcmp(on.name,"-") == 0) || (strcmp(on.name,".") == 0)) {
    is_temp = true;
  }

  // first insert object into database
  ps = this->db->get_survey(on.survey, ps);
  unsigned long csurvey_id = (ps == NULL) ? 0 : ps->id;
  on.survey = NULL;

  if (!is_temp) {
    thdb1d_station_map_type::iterator sti;
    sti = this->station_map.find(thobjectid(on.name, csurvey_id)); 
    if (sti != this->station_map.end()) {
      this->station_vec[sti->second - 1].set_parent_data(pd,pd_priority);
      return sti->second;
    }
    if (!(this->db->insert_datastation(on, ps))) {
      if (on.survey != NULL)
        ththrow("object already exist -- {}@{}", on.name, on.survey);
      else
        ththrow("object already exist -- {}", on.name);
    }
  }  
  this->lsid++;
  this->station_vec.push_back(thdb1ds(on.name, ps));
  thdb1ds * cstation = &(this->station_vec[this->lsid - 1]);
  if (!is_temp) {
    this->station_map[thobjectid(on.name, csurvey_id)] = this->lsid;
  } else {
    cstation->set_temporary(on.name);
  }
  cstation->set_parent_data(pd,pd_priority);
  return this->lsid;  
}


void thdb1d::self_print(FILE * outf)
{
  unsigned int sid;
  fprintf(outf,"survey stations\n");
  thdb1ds * sp;
  for (sid = 0; sid < this->lsid; sid++) {
    sp = & (this->station_vec[sid]);
    fprintf(outf,"\t%d:%ld\t%s@%s\t%.2f\t%.2f\t%.2f", sid + 1, sp->uid, sp->name, 
        sp->survey->full_name, sp->x, sp->y, sp->z);
    fprintf(outf,"\tflags:");
    if (sp->flags & TT_STATIONFLAG_ENTRANCE)
      fprintf(outf,"E");
    if (sp->flags & TT_STATIONFLAG_CONT)
      fprintf(outf,"C");
    if (sp->flags & TT_STATIONFLAG_FIXED)
      fprintf(outf,"F");
    if (sp->flags & TT_STATIONFLAG_UNDERGROUND)
      fprintf(outf,"U");
    fprintf(outf,"\tmark:");
    switch (sp->mark) {
      case TT_DATAMARK_FIXED:
        fprintf(outf,"fixed");
        break;
      case TT_DATAMARK_PAINTED:
        fprintf(outf,"painted");
        break;
      case TT_DATAMARK_TEMP:
        fprintf(outf,"temporary");
        break;
      case TT_DATAMARK_NATURAL:
        fprintf(outf,"natural");
        break;
    }
    if (sp->comment != NULL)
      fprintf(outf,"\t\"%s\"", sp->comment);
    fprintf(outf,"\n");
  }
  fprintf(outf,"end -- survey stations\n");
}


unsigned long thdb1d::get_tree_size()
{
  if (this->tree_legs == NULL)
    this->process_tree();
  return this->num_tree_legs;
}


thdb1dl ** thdb1d::get_tree_legs()
{
  if (this->tree_legs == NULL)
    this->process_tree();
  return this->tree_legs;
}


void thdb1d_equate_nodes(thdb1d * thisdb, thdb1d_tree_node * n1, thdb1d_tree_node * n2)
{

  if (n1->uid == n2->uid)
    return;

  thdb1d_tree_node * n3;
  
  if (n1->is_fixed && n2->is_fixed) {
    ththrow("equate of two fixed stations -- {}@{} and {}@{}",
          thisdb->station_vec[n1->id - 1].name,
          thisdb->station_vec[n1->id - 1].survey->get_full_name(),
          thisdb->station_vec[n2->id - 1].name,
          thisdb->station_vec[n2->id - 1].survey->get_full_name());
  }
    
  if (n1->id == n2->id) {
    ththrow("equate of two identical stations -- {}@{} and {}@{}",
          thisdb->station_vec[n1->id - 1].name,
          thisdb->station_vec[n1->id - 1].survey->get_full_name(),
          thisdb->station_vec[n2->id - 1].name,
          thisdb->station_vec[n2->id - 1].survey->get_full_name());
  }
    
  // vymeni ich ak uid1 nie je fixed
  if ((n2->is_fixed) && (!n1->is_fixed)) {
    n3 = n1;
    n1 = n2;
    n2 = n3;
  }
  
  // priradi uid1 do uid2
  n2->uid = n1->uid;

  n3 = n2->prev_eq;
  while (n3 != NULL) {
    n3->uid = n1->uid;
    n3 = n3->prev_eq;
  }

  n3 = n2->next_eq;
  while (n3 != NULL) {
    n3->uid = n1->uid;
    n3 = n3->next_eq;
  }
  
  // teraz spojme n1->prev s n2->next
  while (n1->prev_eq != NULL) {
    n1 = n1->prev_eq;
  }
  while (n2->next_eq != NULL) {
    n2 = n2->next_eq;
  }
  n1->prev_eq = n2;
  n2->next_eq = n1;
  
}


void thdb1d::process_tree()
{

  size_t tn_legs = this->leg_vec.size();
  size_t tn_stations = this->station_vec.size();
  
#ifdef THDEBUG
    thprintf("\n\nscanning centreline tree\n");
#else
    thprintf("scanning centreline tree ... ");
    thtext_inline = true;
#endif 

  thdb1d_tree_node * nodes = new thdb1d_tree_node [tn_stations];
  this->tree_nodes = nodes;
  thdb1d_tree_arrow * arrows = new thdb1d_tree_arrow [2 * tn_legs];
  this->tree_arrows = arrows;
  thdb1d_tree_node * n1, * n2, * n3, * current_node = NULL;
  thdb1d_tree_arrow * a1, * a2;
  unsigned long i, ii;
  bool any_fixed = false;
  
  // let's parse all nodes
  for(i = 0, ii = 1, n1 = nodes; i < tn_stations; i++, n1++, ii++) {
    n1->id = ii;
    n1->uid = ii;
    n1->is_fixed = ((this->station_vec[i].flags & TT_STATIONFLAG_FIXED) != 0);
//    if (n1->is_fixed)
//      thprintf("FX: %s@%s\n", this->station_vec[i].name, this->station_vec[i].survey->full_name);
    if (n1->is_fixed)
      any_fixed = true;
  }
  
  // let's parse all equates
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  thdataequate_list::iterator eqi;
  thdata * dp;
  int last_eq = -1;
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = dynamic_cast<thdata*>(obi->get());
      eqi = dp->equate_list.begin();
      last_eq = -1;
      while(eqi != dp->equate_list.end()) {
        if (eqi->eqid != last_eq) {
          n1 = nodes + (eqi->station.id - 1);
          last_eq = eqi->eqid;
        }
        else {
          n2 = nodes + (eqi->station.id - 1);
          thdb1d_equate_nodes(this,n1,n2);
        }
        eqi++;
      }
    }
    obi++;
  }
  
  // now let's equate infer legs, zero lengthed
  thdb1d_leg_vec_type::iterator iil;
  for(iil = this->leg_vec.begin(); iil != this->leg_vec.end(); iil++) {
    if (iil->leg->infer_equates) {
      if (iil->leg->total_length == 0.0) {
        thdb1d_equate_nodes(this, nodes + (iil->leg->from.id - 1),
          nodes + (iil->leg->to.id - 1));
      }
    }
  }
  
  // write uid into original database
  for(i = 0, n1 = nodes; i < tn_stations; i++, n1++) {
    this->station_vec[i].uid = n1->uid;
  }
  
  // go leg by leg and fill arrows  
  for(iil = this->leg_vec.begin(), a1 = arrows; iil != this->leg_vec.end(); iil++) {
    
    if (iil->leg->infer_equates)
      if (iil->leg->total_length == 0.0)
        continue;
        
    a2 = a1 + 1;
    a1->negative = a2;
    a1->leg = &(*iil);
    a1->start_node = nodes + (nodes[iil->leg->from.id - 1].uid - 1);
    a1->end_node = nodes + (nodes[iil->leg->to.id - 1].uid - 1);
    
//    if ((iil->leg->data_type == TT_DATATYPE_NOSURVEY) &&
//      ((!a1->start_node->is_fixed) || (!a1->end_node->is_fixed))) {
////        thprintf("%s@%s - %s@%s\n", iil->leg->from.name, iil->leg->from.survey, iil->leg->to.name, iil->leg->to.survey);
//        ththrow("unsurveyed shot between unfixed stations -- %s [%d]",
//          iil->leg->srcf.name, iil->leg->srcf.line
//        ));
//    }
    if (iil->leg->data_type == TT_DATATYPE_NOSURVEY) {
        a1->is_nosurvey = true;
        a2->is_nosurvey = true;
    }
    
    // hide splay-ed flags from extended elevation2
    //if ((iil->leg->flags && TT_LEGFLAG_SPLAY) != 0) {
    //  iil->leg->extend |= TT_EXTENDFLAG_HIDE;
    //  a2->extend |= TT_EXTENDFLAG_HIDE;
    //  a1->extend |= TT_EXTENDFLAG_HIDE;
    //}
    
    a2->negative = a1;
    a2->leg = a1->leg;
    a2->start_node = a1->end_node;
    a2->end_node = a1->start_node;
    a2->is_reversed = true;
    
    // assign nodes
    a1->next_arrow = a1->start_node->first_arrow;
    a1->start_node->first_arrow = a1;
    a2->next_arrow = a2->start_node->first_arrow;
    a2->start_node->first_arrow = a2;
    a1->start_node->narrows++;
    a2->start_node->narrows++;

    a1 += 2;
  }
  
  // process the tree
  // 1. set all nodes without legs as attached
  for(i = 0, n1 = nodes; i < tn_stations; i++, n1++) {
    if (n1->id != n1->uid)
      n1->is_attached = true;
    else if ((n1->first_arrow == NULL) && (n1->is_fixed))
      n1->is_attached = true;
  }
  
  unsigned long series = 0, component = 0, tarrows = 0, last_series = 0;
  bool component_break = true;
  this->tree_legs = new thdb1dl* [tn_legs];
  thdb1dl ** current_leg = this->tree_legs;

  while (tarrows < tn_legs) {
  
    if (component_break) {
      
      // let's find starting node
      n2 = NULL;
      n3 = NULL;
      bool n2null = true;
      for(i = 0, n1 = nodes; i < tn_stations; i++, n1++) {
        if (!n1->is_attached) {
          if (n2null) {
            n2 = n1;
            n3 = n1;
            n2null = false;
          }
          if (n1->is_fixed) {
            n2 = n1;
            break;
          }
        }
      }
      
      
      // something is wrong
      if (n2 == NULL) {
        ththrow("a software BUG is present (" __FILE__ ":{})", __LINE__);
//#ifdef THDEBUG
//        thprintf("warning -- not all stations connected to the network\n");
//#endif
        break;
      }

      if ((!n2->is_fixed) && (any_fixed || (component > 0))) {
        ththrow("can not connect {}@{} to centerline network",
          this->station_vec[n3->id - 1].name,
          this->station_vec[n3->id - 1].survey->get_full_name());
      }
      
    
      current_node = n2;
      current_node->is_attached = true;  
      component++;
      if (series == last_series)
        series++;
      component_break = false;
#ifdef THDEBUG
      thprintf("component %d -- %d (%s@%s)\n", component, current_node->id,
        this->station_vec[current_node->id - 1].name,
        this->station_vec[current_node->id - 1].survey->get_full_name());
#endif
      
    } // end of tremaux
        
    // let's make move
    if (current_node->last_arrow == NULL)
      current_node->last_arrow = current_node->first_arrow;
    else
      current_node->last_arrow = current_node->last_arrow->next_arrow;
    
    // skip discovery arrows or nosurvey arrows, if station on the other site is not connected
    while ((current_node->last_arrow != NULL) && ( 
        (current_node->last_arrow->is_discovery) || 
        ((current_node->last_arrow->is_nosurvey) && 
        (!current_node->last_arrow->end_node->is_attached))
        ))
        current_node->last_arrow = current_node->last_arrow->next_arrow;
    
    if (current_node->last_arrow == NULL) {

      // go back
      if (current_node->back_arrow == NULL)
        component_break = true;
      else {
        current_node = current_node->back_arrow->end_node;
#ifdef THDEBUG
        thprintf("%d (%s@%s) <-\n", current_node->id,
          this->station_vec[current_node->id - 1].name,
          this->station_vec[current_node->id - 1].survey->get_full_name());
#endif
      }

    }
    else {
    
      // go forward
      // check if not already discovered
      current_node->last_arrow->negative->is_discovery = true;
          
      tarrows++;
      *current_leg = current_node->last_arrow->leg;
#ifdef THDEBUG
      thdb1dl * prev_leg = *current_leg;
#endif
      (*current_leg)->reverse = current_node->last_arrow->is_reversed;
      (*current_leg)->series_id = series;
      last_series = series;
      (*current_leg)->component_id = component;
      current_leg++;
      
#ifdef THDEBUG
      thprintf("-> %d (%s@%s) [%d %s %d, series %d, arrow %d]\n", current_node->last_arrow->end_node->id,
        this->station_vec[current_node->last_arrow->end_node->id - 1].name,
        this->station_vec[current_node->last_arrow->end_node->id - 1].survey->get_full_name(),
        prev_leg->leg->from.id,
        (prev_leg->reverse ? "<=" : "=>"),
        prev_leg->leg->to.id,
        series, tarrows);
#endif

      if (!current_node->last_arrow->end_node->is_attached) {
        current_node->last_arrow->end_node->back_arrow = 
          current_node->last_arrow->negative;
        current_node = current_node->last_arrow->end_node;
        current_node->is_attached = true;
        if (current_node->narrows != 2)
          series++;      
      }
      else
        series++;
    }
  }

  this->num_tree_legs = tarrows;
  
#ifdef THDEBUG
    thprintf("\nend of scanning data tree\n\n");
#else
    thprintf("done\n");
    thtext_inline = false;
#endif
}



void thdb1d_scan_survey_station_limits(thsurvey * ss, thdb1ds * st, bool is_under) {
  if (st->is_temporary()) return;
  if (ss->stat.station_state == 0) {
    if (is_under)
      ss->stat.station_state = 2;
    else
      ss->stat.station_state = 1;
    ss->stat.station_top = st;
    ss->stat.station_bottom = st;
    ss->stat.station_south = st;
    ss->stat.station_north = st;
    ss->stat.station_east = st;
    ss->stat.station_west = st;
  } else if (is_under && (ss->stat.station_state == 1)) {
    ss->stat.station_state = 2;
    ss->stat.station_top = st;
    ss->stat.station_bottom = st;
    ss->stat.station_south = st;
    ss->stat.station_north = st;
    ss->stat.station_east = st;
    ss->stat.station_west = st;
  } else if (is_under || (ss->stat.station_state == 1)) {
    //ss->stat.station_state = 2;
    if (ss->stat.station_top->z < st->z)
      ss->stat.station_top = st;
    if (ss->stat.station_bottom->z > st->z)
      ss->stat.station_bottom = st;
    if (ss->stat.station_east->x < st->x)
      ss->stat.station_east = st;
    if (ss->stat.station_west->x > st->x)
      ss->stat.station_west = st;
    if (ss->stat.station_north->y < st->y)
      ss->stat.station_north = st;
    if (ss->stat.station_south->y > st->y)
      ss->stat.station_south = st;
  }
}


void thdb1d_scan_data_station_limits(thdata * ss, thdb1ds * st, bool is_under) {
  if (ss->stat_st_state == 0) {
    if (is_under)
      ss->stat_st_state = 2;
    else
      ss->stat_st_state = 1;
    ss->stat_st_top = st;
    ss->stat_st_bottom = st;
//    ss->stat_st_south = st;
//    ss->stat_st_north = st;
//    ss->stat_st_east = st;
//    ss->stat_st_west = st;
  } else if (is_under && (ss->stat_st_state == 1)) {
    ss->stat_st_state = 2;
    ss->stat_st_top = st;
    ss->stat_st_bottom = st;
//    ss->stat_st_south = st;
//    ss->stat_st_north = st;
//    ss->stat_st_east = st;
//    ss->stat_st_west = st;
  } else if (is_under || (ss->stat_st_state == 1)) {
    //ss->stat_st_state = 2;
    if (ss->stat_st_top->z < st->z)
      ss->stat_st_top = st;
    if (ss->stat_st_bottom->z > st->z)
      ss->stat_st_bottom = st;
//    if (ss->stat_st_east->x < st->x)
//      ss->stat_st_east = st;
//    if (ss->stat_st_west->x > st->x)
//      ss->stat_st_west = st;
//    if (ss->stat_st_north->y < st->y)
//      ss->stat_st_north = st;
//    if (ss->stat_st_south->y > st->y)
//      ss->stat_st_south = st;
  }
}




void thdb1d::process_survey_stat() {

#ifdef THDEBUG
    thprintf("\n\ncalculating basic statistics\n");
#else
    thprintf("calculating basic statistics ... ");
    thtext_inline = true;
#endif 

  thsurvey * ss;

  // prejde vsetky legy a spocita ich a dlzky pre kazde survey
  // do ktoreho patria
  thdb1d_leg_vec_type::iterator lit = this->leg_vec.begin();
  while (lit != this->leg_vec.end()) {    

    // skusi ci je duplikovane
    if ((lit->leg->flags & TT_LEGFLAG_SPLAY) != 0)
      lit->data->stat_splaylength += lit->leg->total_length;
    else if ((lit->leg->flags & TT_LEGFLAG_DUPLICATE) != 0)
      lit->data->stat_dlength += lit->leg->total_length;
    // ak nie skusi ci je surface
    else if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0)
      lit->data->stat_slength += lit->leg->total_length;
    // inak prida do length
    else {
      lit->data->stat_length += lit->leg->total_length;
      this->nlength += lit->leg->total_length;
      // check if approximate length
      if ((lit->leg->flags & TT_LEGFLAG_APPROXIMATE) != 0)
        lit->data->stat_alength += lit->leg->total_length;
    }

    // stations
    if ((lit->leg->flags & TT_LEGFLAG_SPLAY) == 0) {
      if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0) {
        thdb1d_scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->from.id - 1]), false);
        thdb1d_scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->to.id - 1]), false);
      } else {
        thdb1d_scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->from.id - 1]), true);
        thdb1d_scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->to.id - 1]), true);
      }
    }

    ss = lit->survey;
    while (ss != NULL) {
      // skusi ci nie je splay
      if ((lit->leg->flags & TT_LEGFLAG_SPLAY) != 0)
        ss->stat.length_splay += lit->leg->total_length;
      // skusi ci je duplikovane
      else if ((lit->leg->flags & TT_LEGFLAG_DUPLICATE) != 0)
        ss->stat.length_duplicate += lit->leg->total_length;
      // ak nie skusi ci je surface
      else if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0)
        ss->stat.length_surface += lit->leg->total_length;
      // inak prida do length
      else {
        if ((lit->leg->flags & TT_LEGFLAG_APPROXIMATE) != 0)
          ss->stat.length_approx += lit->leg->total_length;
        ss->stat.length += lit->leg->total_length;
      }
      if ((lit->leg->flags & TT_LEGFLAG_SPLAY) == 0) {
        if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0) {
          thdb1d_scan_survey_station_limits(ss, &(this->station_vec[lit->leg->from.id - 1]), false);
          thdb1d_scan_survey_station_limits(ss, &(this->station_vec[lit->leg->to.id - 1]), false);
        } else {
          thdb1d_scan_survey_station_limits(ss, &(this->station_vec[lit->leg->from.id - 1]), true);
          thdb1d_scan_survey_station_limits(ss, &(this->station_vec[lit->leg->to.id - 1]), true);
        }
      }
      ss->stat.num_shots++;
      ss = ss->fsptr;
    }
    lit++;
  }

  // prejde vsetky stations a spocita ich a nastavi limitne stations
  // pricom ak najde prvu povrchovu, tak nastvi vsetky s nou
  // ak najde podzemnu a ma niekde povrchovu -> nastavi podzemnu
  thdb1d_station_vec_type::iterator sit = this->station_vec.begin();
  while (sit != this->station_vec.end()) {
    ss = sit->survey;
    while (ss != NULL) {
      if (!thisnan(sit->explored))
        ss->stat.length_explored += sit->explored;
      if ((sit->flags & TT_STATIONFLAG_ENTRANCE) > 0) {
        ss->stat.num_entrances++;
      }
      ss->stat.num_stations++;
      ss = ss->fsptr;
    }    
    sit++;
  }

  // prejde vsetky continuations a prepocita explore length
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  thpoint * pt;
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_POINT_CMD) {
      pt = dynamic_cast<thpoint*>(obi->get());
      ss = pt->fsptr;
      if ((pt->type == TT_POINT_TYPE_CONTINUATION) && (!thisnan(pt->xsize))) {
        while (ss != NULL) {
          ss->stat.length_explored += pt->xsize;
          ss = ss->fsptr;
        }    
      }
    }
    obi++;
  }

  
#ifdef THDEBUG
    thprintf("\nend of basic statistics calculation\n\n");
#else
    thprintf("done\n");
    thtext_inline = false;
#endif
}


struct thlc_leg
{
  unsigned long from_uid, to_uid, from_id, to_id;
  thlc_leg * prev_series_leg, * next_series_leg;
  thdb1dl * dbleg;
};


struct thlc_cross
{
  unsigned long id, station_uid, narrows, dm_dist;
  bool is_fixed, dm_target;  
  thlc_cross * dm_cross, * dm_next;
  struct thlc_cross_arrow * first_arrow, * last_arrow, * dm_arrow;
  
};


struct thlc_series
{
  unsigned long from, to, numlegs, id;
  thlc_cross * from_cross, * to_cross;
  thlc_leg * first_leg, * last_leg;
  bool is_active;
};


struct thlc_cross_arrow
{
  thlc_cross * target = {};
  thlc_cross_arrow * prev_arrow = {}, * next_arrow = {};
  thlc_series * series = {};
  bool reverse = {};
};


struct thlc_loop
{
  thlc_cross * from_cross = {}, * to_cross = {};
  thlc_cross_arrow * first_arrow = {}, * last_arrow = {};
  unsigned long minid = {}, size = {}, id = {};
#ifdef THDEBUG
  bool is_new = {};
  long old_id = {};
#endif  
};


bool operator < (
  const struct thlc_loop & l1, 
  const struct thlc_loop & l2)
{
  bool direction;
  thlc_cross_arrow * l1a, * l2a;
  
  if (l1.size < l2.size)
    return true;
  else if (l1.size > l2.size)
    return false; 
    
  // neostava nam nic ine ako porovnavat po seriach
  if (l1.from_cross->id != l1.to_cross->id) {
    // l1 otovreny okruh
    if (l2.from_cross->id == l2.to_cross->id)
      return false;

    if (l1.minid < l2.minid)
      return true;
    if (l1.minid > l2.minid)
      return false;

    if (l1.to_cross->id == l2.to_cross->id)
      direction = true;
    else
      direction = false;
    // porovname seriu za seriou, vratime false ak nebude mensia
    l1a = l1.first_arrow;
    l2a = (direction ? l2.first_arrow : l2.last_arrow);
    while ((l1a != NULL) && (l2a != NULL)) {
      if (l1a->series->id > l2a->series->id)
        return false;
      if (l1a->series->id < l2a->series->id)
        return true;
      l1a = l1a->next_arrow;
      l2a = (direction ? l2a->next_arrow : l2a->prev_arrow);
      if ((l1a == NULL) && (l2a != NULL))
        return true;
      if ((l1a != NULL) && (l2a == NULL))
        return false;
    }
//#ifdef THDEBUG
//    thprintf("LOOP EQUAL: %ld == %ld\n", l1.id, l2.id);
//#endif    
    return false;  
  } else {

    // l1 uzavrety okruh
    if (l2.from_cross->id != l2.to_cross->id)
      return true;

    if (l1.minid < l2.minid)
      return true;
    if (l1.minid > l2.minid)
      return false;

    // v obidvoch najde minid hranu a na zaklade jej orientacie
    // urci direction v dvojke
    l1a = l1.first_arrow;
    while (l1a->series->id != l1.minid) {
      l1a = l1a->next_arrow;
    }
    l2a = l2.first_arrow;
    while (l2a->series->id != l2.minid) {
      l2a = l2a->next_arrow;
    }
    if (l1a->reverse != l2a->reverse)
      direction = false;
    else
      direction = true;
#define nextstep {\
    l1a = l1a->next_arrow; \
    if (l1a == NULL) l1a = l1.first_arrow; \
    l2a = (direction ? l2a->next_arrow : l2a->prev_arrow); \
    if (l2a == NULL) l2a = (direction ? l2.first_arrow : l2.last_arrow); \
    if ((l1a->series->id == l1.minid) && (l2a->series->id != l2.minid)) \
      return true; \
    if ((l1a->series->id != l1.minid) && (l2a->series->id == l2.minid)) \
      return false; \
    if (l1a->series->id < l2a->series->id) return true; \
    if (l1a->series->id > l2a->series->id) return false; \
    }
    nextstep;
    while (l1a->series->id != l1.minid) nextstep;
//#ifdef THDEBUG
//    thprintf("LOOP EQUAL: %ld == %ld\n", l1.id, l2.id);
//#endif    
    return false;      
  }
  
}



long chledala_chledala(thlc_cross * kde, unsigned long kde_size, thlc_cross * co, thlc_cross * odkial, std::list<thlc_cross_arrow> & vysledok)
{
  long result = -1;
  unsigned long i;
  thlc_cross * cc, * lastcc;  

#ifdef THDEBUG
  thprintf("\nLOOP SEARCH: %ld -> %ld\n", odkial->id, co == NULL ? -1 : long(co->id));
#endif

  // initiaze targets
  if (co == NULL) {
    for (i = 0; i < kde_size; i++) {
      cc = &(kde[i]);
      cc->dm_target = cc->is_fixed;
    }
  } else {
    for (i = 0; i < kde_size; i++)
      kde[i].dm_target = false;
    co->dm_target = true;
  }

  // reset other tree variables
  for (i = 0; i < kde_size; i++) {
    cc = &(kde[i]);
    cc->dm_arrow = NULL;
    cc->dm_dist = 0;
    cc->dm_cross = NULL;
  }
  
  lastcc = odkial;
  lastcc->dm_next = NULL;
  cc = odkial;
  
  // modified Djikstra-Moore algorithm
  thlc_cross_arrow * ca;
  thlc_cross * cmin_cross = NULL, * nc;
  bool vykasli_sa;
  unsigned long cmin = 0;
#define check_cmin(cross) {\
  if (cross->dm_target) {\
    if (cmin_cross == NULL) { \
      cmin = cross->dm_dist; \
      cmin_cross = cross; \
    } else if (cmin > cross->dm_dist) {\
      cmin = cross->dm_dist; \
      cmin_cross = cross; \
    } \
  } \
}
  while (cc != NULL) {
    ca = cc->first_arrow;
    while ((ca != NULL) && (!cc->dm_target)) {
      if ((ca->series->is_active) && (ca->target->id != odkial->id)) {
        if (ca->target->dm_cross != NULL) {
          // sme vo vrchole, v ktorom sme uz boli, takze iba skontrolujeme,
          // ci tam nie sme rychlejsie
          if (ca->target->dm_dist > (cc->dm_dist + ca->series->numlegs)) {
            ca->target->dm_arrow = ca;
            ca->target->dm_cross = cc;
            ca->target->dm_dist = cc->dm_dist + ca->series->numlegs;
            check_cmin(ca->target);
          }
        } else {
          // dostali sme sa do noveho vrcholu, nastavime mu co treba a
          // pridame si ho do vrcholov na preskumanie
          ca->target->dm_arrow = ca;
          ca->target->dm_cross = cc;
          ca->target->dm_dist = cc->dm_dist + ca->series->numlegs;
          check_cmin(ca->target);
          lastcc->dm_next = ca->target;
          lastcc = lastcc->dm_next;
          lastcc->dm_next = NULL;
        }
      }
      ca = ca->next_arrow;
    }
    // skontroluje, ci ma zmysel este hladat
    if (cmin_cross != NULL) {
      nc = cc->dm_next;
      vykasli_sa = true;
      while (nc != NULL) {
        vykasli_sa = vykasli_sa && (cmin <= nc->dm_dist);
        nc = nc->dm_next;
      }
      if (vykasli_sa)
        cc->dm_next = NULL;
    }
    cc = cc->dm_next;
    
  }
  
  if (cmin_cross != NULL) {
    result = cmin;
    cc = cmin_cross;
    ca = cc->dm_arrow;
    while (ca != NULL) {
      vysledok.insert(vysledok.begin(),*ca);
#ifdef THDEBUG
      thprintf("%d - %d : %s%d\n",
        ca->reverse ? ca->series->to : ca->series->from,
        ca->reverse ? ca->series->from : ca->series->to,
        ca->reverse ? "R" : "N",
        ca->series->id
      );
#endif
      cc = cc->dm_cross;
      ca = cc->dm_arrow;
    }
  }
    
  return result;
}


void thdb1d::find_loops()
{

  size_t nlegs = this->get_tree_size(),
    nstations = this->station_vec.size(), numseries,
    i, lastcross, nseries, nloops, loopid;
    
  thdb1dl ** legs = this->get_tree_legs(), ** curleg, * cleg;
  thdb1d_tree_node * nodes = this->get_tree_nodes(), * from_node;
  
  long * crossst = NULL, * curcrossst,
    lastlegseries, currentcross;

#ifdef THDEBUG
    thprintf("\n\nsearching for centerline loops\n");
#else
    thprintf("searching for centerline loops ... ");
    thtext_inline = true;
#endif 
    
  if (nstations > 0) 
    crossst = new long [nstations];
  thlc_leg * lclegs = NULL, * clcleg = NULL, * prevlcleg = NULL;
  if (nlegs > 0) 
    lclegs = new thlc_leg [nlegs];
  
  thlc_cross * lccrosses = NULL, * clccross;
  thlc_cross_arrow * lccrossarrows = NULL, * cca;
  thlc_series tmpseries = {}, * curseries, * prevseries;
  std::vector<thlc_series> lcseries;

  thdb1d_loop tdbloop;
#ifdef THDEBUG  
  thdb1d_loop_list_type::iterator dbli;
#endif  
  thdb1d_loop_leg tdbloopleg, * ptdbl;

  std::list<thlc_cross_arrow> from2fixv, to2fixv, closev, loopsv;
  std::list<thlc_cross_arrow>::iterator it;
  std::set<thlc_loop>::iterator lsi;
#ifdef THDEBUG
  std::list<thlc_loop> all_loop_list;
  unsigned long prev_set_size;
  std::list<thlc_loop>::iterator lli;
#endif
  std::set<thlc_loop> all_loop_set;
  long from2fixl, to2fixl, closel;
  thlc_cross_arrow tmpca;
  thlc_loop tmploop;

  
  // resetuje crossst
  lastcross = 0;
#define setcrossst(uid) {\
  currentcross = crossst[uid-1];\
  if (currentcross < 0) {\
    crossst[uid-1] = lastcross;\
    currentcross = lastcross;\
    lastcross++;\
  }}
  for (i = 0, curcrossst = crossst; i < nstations; i++, curcrossst++)
    *curcrossst = -1;
    
  curleg = legs;
  i = 0;
  lastlegseries = -1;
  curseries = NULL;
  numseries = 0;
  clcleg = lclegs;


  while ((nlegs > 0) && ((*curleg)->leg->data_type == TT_DATATYPE_NOSURVEY)) {
    nlegs--;
    curleg++;
  }
  if (nlegs == 0)
    goto LC_COORD_CALC;

  while (i < nlegs) {
  
    while ((nlegs > i) && ((*curleg)->leg->data_type == TT_DATATYPE_NOSURVEY)) {
      nlegs--;
      curleg++;
    }    
    if (nlegs == i)
      break;
      
    // nastavi clcleg
    cleg = (*curleg);
    clcleg->dbleg = cleg;
    clcleg->next_series_leg = NULL;
    if (cleg->reverse) {
      clcleg->from_id = cleg->leg->to.id;
      clcleg->to_id = cleg->leg->from.id;
    } else {
      clcleg->from_id = cleg->leg->from.id;
      clcleg->to_id = cleg->leg->to.id;
    }
    clcleg->from_uid = this->station_vec[clcleg->from_id - 1].uid;
    clcleg->to_uid = this->station_vec[clcleg->to_id - 1].uid;    
    if (clcleg->from_uid == clcleg->to_uid) {
      ththrow("shot between two equal stations -- {}@{} and {}@{}",
          this->station_vec[clcleg->from_id - 1].name,
          this->station_vec[clcleg->from_id - 1].survey->get_full_name(),
          this->station_vec[clcleg->to_id - 1].name,
          this->station_vec[clcleg->to_id - 1].survey->get_full_name());
    }
    from_node = &(nodes[clcleg->from_uid - 1]);
    if ((lastlegseries != long(cleg->series_id)) ||
      from_node->is_fixed || (from_node->narrows != 2)) {
      // zacina sa ina seria
      lcseries.push_back(tmpseries);
      curseries = &(lcseries[numseries]);
      curseries->id = numseries;
      if (numseries > 0) {
        prevseries = &(lcseries[long(numseries) - 1]);;
      } else {
        prevseries = NULL;
      }
      numseries++;
      clcleg->prev_series_leg = NULL;
      curseries->first_leg = clcleg;
      curseries->last_leg = clcleg;
      curseries->numlegs = 1;
      setcrossst(clcleg->from_uid);
      curseries->from = currentcross;
      if (prevseries != NULL) {
        setcrossst(prevlcleg->to_uid);
        prevseries->to = currentcross;
      }
    } else {
      // nezacina sa ina seria
      curseries->last_leg = clcleg;
      clcleg->prev_series_leg = prevlcleg;
      prevlcleg->next_series_leg = clcleg;
      curseries->numlegs++;
    }
    lastlegseries = long(cleg->series_id);
    curleg++;
    prevlcleg = clcleg;
    clcleg++;
    i++;
  }
  if (curseries != NULL) {
    setcrossst(prevlcleg->to_uid);
    curseries->to = currentcross;
  };

  nseries = lcseries.size();
  lccrosses = new thlc_cross [lastcross];
  for (i = 0, curcrossst = crossst; i < nstations; i++, curcrossst++) {
    if (*curcrossst > -1) {
      clccross = &(lccrosses[*curcrossst]);
      clccross->id = *curcrossst;
      clccross->station_uid = i + 1;
      clccross->narrows = 0;
      clccross->first_arrow = NULL;
      clccross->last_arrow = NULL;
      clccross->is_fixed = nodes[i].is_fixed;
    }
  }
  
  // vytvori strom krizovatiek
  i = 0;
  lccrossarrows = new thlc_cross_arrow [2 * nseries];
  cca = lccrossarrows;
  while (i < nseries) {
    curseries = &(lcseries[i]);
    curseries->from_cross = &(lccrosses[curseries->from]);
    curseries->to_cross = &(lccrosses[curseries->to]);
    
    cca->prev_arrow = NULL;
    cca->next_arrow = NULL;
    if (curseries->from_cross->last_arrow == NULL) {
      curseries->from_cross->first_arrow = cca;
    } else {
      cca->prev_arrow = curseries->from_cross->last_arrow;
      curseries->from_cross->last_arrow->next_arrow = cca;
    }
    curseries->from_cross->last_arrow = cca;
    curseries->from_cross->narrows++;
    cca->target = curseries->to_cross;
    cca->series = curseries;
    cca->reverse = false;
    cca++;

    cca->prev_arrow = NULL;
    cca->next_arrow = NULL;
    if (curseries->to_cross->last_arrow == NULL) {
      curseries->to_cross->first_arrow = cca;
    } else {
      cca->prev_arrow = curseries->to_cross->last_arrow;
      curseries->to_cross->last_arrow->next_arrow = cca;
    }
    curseries->to_cross->last_arrow = cca;
    curseries->to_cross->narrows++;
    cca->target = curseries->from_cross;
    cca->series = curseries;
    cca->reverse = true;
    cca++;

    i++;
  }

#ifdef THDEBUG
  thprintf("\n\n\nLOOP CLOSURE DEBUG\n");
  thprintf("\nCROSSES:\n");
  for (i = 0; i < lastcross; i++) {
    thprintf("%ld: %s %ld [",i, lccrosses[i].is_fixed ? "F" : "V", lccrosses[i].narrows);
    cca = lccrosses[i].first_arrow;
    while (cca != NULL) {
      thprintf("%s%ld", cca->prev_arrow == NULL ? "" : " ", cca->target->id);
      cca = cca->next_arrow;
    }
    thprintf("] %s@%s\n",
      this->station_vec[lccrosses[i].station_uid - 1].name,
      this->station_vec[lccrosses[i].station_uid - 1].survey->name);
  }
  thprintf("\nSERIES:\n");
  for (i = 0; i < nseries; i++) {
    thprintf("%ld: %ld - %ld [%ld legs",i,lcseries[i].from,lcseries[i].to,lcseries[i].numlegs);
    clcleg = lcseries[i].first_leg;
    if (clcleg != NULL) {
      thprintf(": %s@%s",
        this->station_vec[clcleg->from_uid - 1].name,
        this->station_vec[clcleg->from_uid - 1].survey->name);
    }
    while (clcleg != NULL) {
      thprintf(" - %s@%s",
        this->station_vec[clcleg->to_uid - 1].name,
        this->station_vec[clcleg->to_uid - 1].survey->name);
      clcleg = clcleg->next_series_leg;
    }
    thprintf("]\n");
  }
#endif  

  
  // najde okruhy
  nloops = 0;
  
  // urobi preprocessing serii
  for(i = 0; i < nseries; i++) {
    curseries = &(lcseries[i]);
    curseries->is_active = true;
    // ak neni fixnuty a ma len jeden arrow - deaktivujeme
    if (((!(curseries->from_cross->is_fixed)) && 
        (curseries->from_cross->narrows == 1)) ||
        ((!(curseries->to_cross->is_fixed)) && 
        (curseries->to_cross->narrows == 1)))
      curseries->is_active = false;
    // ak ma oba konce fixnute alebo totozne, spravi z nej okruh
    if ((curseries->from_cross->is_fixed && curseries->to_cross->is_fixed) ||
      (curseries->from == curseries->to)) {
      tmpca.series = curseries;
      tmpca.reverse = false;
      tmpca.prev_arrow = NULL;
      tmpca.next_arrow = NULL;
      tmploop.first_arrow = &(*loopsv.insert(loopsv.end(),tmpca));
      tmploop.last_arrow = tmploop.first_arrow;
      tmploop.from_cross = curseries->from_cross;
      tmploop.to_cross = curseries->to_cross;
      tmploop.minid = curseries->id;
      tmploop.size = curseries->numlegs;
      tmploop.id = nloops++;
#ifdef THDEBUG      
      prev_set_size = all_loop_set.size();
#endif     
      all_loop_set.insert(tmploop);
#ifdef THDEBUG
      lsi = all_loop_set.find(tmploop);
      tmploop.old_id = lsi->id;
      if (prev_set_size < all_loop_set.size())
        tmploop.is_new = true;
      else
        tmploop.is_new = false;
      all_loop_list.insert(all_loop_list.end(),tmploop);
#endif     
      curseries->is_active = false;
    }
  }
  
  // najde okruh ku kazdej serii
  for (i = 0; i < nseries; i++) {
    curseries = &(lcseries[i]);
    if (!curseries->is_active)
      continue;
    curseries->is_active = false;

//#ifdef THDEBUG
//  thprintf("\nNEW SERIES: %ld -> %ld", curseries->from, curseries->to);
//#endif

    // najde najblizsi pevny bod ku from
    from2fixv.clear();
    if (!curseries->from_cross->is_fixed)
      from2fixl = chledala_chledala(lccrosses, lastcross, NULL, curseries->from_cross, from2fixv);
    else
      from2fixl = 0;
      
    // najde najblizsti pevny bod ku to
    to2fixv.clear();
    if (!curseries->to_cross->is_fixed)
      to2fixl = chledala_chledala(lccrosses, lastcross, NULL, curseries->to_cross, to2fixv);
    else
      to2fixl = 0;
      
    // najde najkratsiu cestu z from do to
    closev.clear();
    closel = chledala_chledala(lccrosses, lastcross, curseries->from_cross, curseries->to_cross, closev);

    // vytvori okruh
    if (((from2fixl >= 0) && (to2fixl >= 0)) || (closel > 0)) {
   
      // vlozime do okruhu sucasnu hranu
      tmpca.series = curseries;
      tmpca.reverse = false;
      tmpca.prev_arrow = NULL;
      tmpca.next_arrow = NULL;
      tmploop.first_arrow = &(*loopsv.insert(loopsv.end(),tmpca));
      tmploop.last_arrow = tmploop.first_arrow;
      tmploop.from_cross = curseries->from_cross;
      tmploop.to_cross = curseries->to_cross;
      tmploop.minid = curseries->id;
      tmploop.size = curseries->numlegs;
      tmploop.id = nloops++;
#define checktmploop(ca) {\
  if (tmploop.minid > ca.series->id) \
    tmploop.minid = ca.series->id; \
  tmploop.size += ca.series->numlegs; \
}    
      if ((closel > 0) && (((from2fixl + to2fixl) >= closel) || (from2fixl < 0) || (to2fixl < 0))) {
        // vytvorime uzavrety okruh
        it = closev.begin();
        while (it != closev.end()) {
          tmpca = *it;
          tmploop.last_arrow->next_arrow = &(*loopsv.insert(loopsv.end(),tmpca));
          tmploop.last_arrow->next_arrow->prev_arrow = tmploop.last_arrow;
          tmploop.last_arrow = tmploop.last_arrow->next_arrow;
          tmploop.last_arrow->next_arrow = NULL;
          checktmploop(tmpca);
          it++;
        }
        tmploop.from_cross = curseries->from_cross;
        tmploop.to_cross = curseries->from_cross;
#ifdef THDEBUG      
        prev_set_size = all_loop_set.size();
#endif     
        all_loop_set.insert(tmploop);
#ifdef THDEBUG
        lsi = all_loop_set.find(tmploop);
        tmploop.old_id = lsi->id;
        if (prev_set_size < all_loop_set.size())
          tmploop.is_new = true;
        else
          tmploop.is_new = false;
        all_loop_list.insert(all_loop_list.end(),tmploop);
#endif     
      } else {
        // vytvorime otvoreny okruh
        it = from2fixv.begin();
        while (it != from2fixv.end()) {
          tmpca = *it;
          tmpca.reverse = !tmpca.reverse;        
          if (tmpca.reverse)
            tmploop.from_cross = tmpca.series->to_cross;
          else
            tmploop.from_cross = tmpca.series->from_cross;
          tmploop.first_arrow->prev_arrow = &(*loopsv.insert(loopsv.end(),tmpca));
          tmploop.first_arrow->prev_arrow->next_arrow = tmploop.first_arrow;
          tmploop.first_arrow = tmploop.first_arrow->prev_arrow;
          tmploop.first_arrow->prev_arrow = NULL;
          checktmploop(tmpca);
          it++;
        }
        it = to2fixv.begin();
        while (it != to2fixv.end()) {
          tmpca = *it;
          if (tmpca.reverse)
            tmploop.to_cross = tmpca.series->from_cross;
          else
            tmploop.to_cross = tmpca.series->to_cross;
          tmploop.last_arrow->next_arrow = &(*loopsv.insert(loopsv.end(),tmpca));
          tmploop.last_arrow->next_arrow->prev_arrow = tmploop.last_arrow;
          tmploop.last_arrow = tmploop.last_arrow->next_arrow;
          tmploop.last_arrow->next_arrow = NULL;
          checktmploop(tmpca);
          it++;
        }
#ifdef THDEBUG      
        prev_set_size = all_loop_set.size();
#endif     
        all_loop_set.insert(tmploop);
#ifdef THDEBUG
        lsi = all_loop_set.find(tmploop);
        tmploop.old_id = lsi->id;
        if (prev_set_size < all_loop_set.size())
          tmploop.is_new = true;
        else
          tmploop.is_new = false;
        all_loop_list.insert(all_loop_list.end(),tmploop);
#endif     
      }
    }
    curseries->is_active = true;
  }

#ifdef THDEBUG
  // vypise okruhy na zaklade serii
  lli = all_loop_list.begin();
  i = 0;
  thprintf("\nSERIES LOOPS:\n");
  while (lli != all_loop_list.end()) {
    lsi = all_loop_set.find(*lli);
    thprintf("%ld[%ld] %s %s %ld: ", lli->id, lli->old_id, lli->is_new ? "NEW" : "OLD" , lli->from_cross->id != lli->to_cross->id ? "OPN" : "CLS", lli->size);
    cca = lli->first_arrow;
    if (cca->reverse) {
      thprintf("%d(%s@%s)", cca->series->to,
        this->station_vec[cca->series->to_cross->station_uid - 1].name,
        this->station_vec[cca->series->to_cross->station_uid - 1].survey->name);
    } else {
      thprintf("%d(%s@%s)", cca->series->from,
        this->station_vec[cca->series->from_cross->station_uid - 1].name,
        this->station_vec[cca->series->from_cross->station_uid - 1].survey->name);
    }
    while (cca != NULL) {
      if (cca->reverse) {
        thprintf("[%d(%s@%s)] - %d(%s@%s)", cca->series->to,
          this->station_vec[cca->series->to_cross->station_uid - 1].name,
          this->station_vec[cca->series->to_cross->station_uid - 1].survey->name, cca->series->from,
          this->station_vec[cca->series->from_cross->station_uid - 1].name,
          this->station_vec[cca->series->from_cross->station_uid - 1].survey->name);
      } else {
        thprintf("[%d(%s@%s)] - %d(%s@%s)", cca->series->from,
          this->station_vec[cca->series->from_cross->station_uid - 1].name,
          this->station_vec[cca->series->from_cross->station_uid - 1].survey->name, cca->series->to,
          this->station_vec[cca->series->to_cross->station_uid - 1].name,
          this->station_vec[cca->series->to_cross->station_uid - 1].survey->name);
      }
      cca = cca->next_arrow;
    }
    thprintf("\n");
    lli++;
    i++;
  }
#endif  
  
  // zapise si okruhy do premennych databazy
  lsi = all_loop_set.begin();
  loopid = 0;
  while (lsi != all_loop_set.end()) {
    // vypise ci je open alebo close, nastavi
    tdbloop.from = &(this->station_vec[lsi->from_cross->station_uid - 1]);
    tdbloop.to = &(this->station_vec[lsi->to_cross->station_uid - 1]);
    if (tdbloop.from->uid == tdbloop.to->uid)
      tdbloop.open = false;
    else
      tdbloop.open = true;
    tdbloop.nlegs = 0;
    tdbloop.first_leg = NULL;
    tdbloop.last_leg = NULL;
    cca = lsi->first_arrow;
    while (cca != NULL) {      
      clcleg = cca->reverse ? cca->series->last_leg : cca->series->first_leg;
      while (clcleg != NULL) {
        tdbloopleg.leg = clcleg->dbleg->leg;
        tdbloopleg.reverse = clcleg->dbleg->reverse;
        if (cca->reverse)
          tdbloopleg.reverse = !tdbloopleg.reverse;
        ptdbl = &(* this->loop_leg_list.insert(this->loop_leg_list.end(),tdbloopleg));
        if (tdbloop.nlegs == 0) {
          tdbloop.first_leg = ptdbl;
          tdbloop.last_leg = ptdbl;
          ptdbl->next_leg = NULL;
          ptdbl->prev_leg = NULL;
        } else {
          tdbloop.last_leg->next_leg = ptdbl;
          ptdbl->prev_leg = tdbloop.last_leg;
          ptdbl->next_leg = NULL;
          tdbloop.last_leg = ptdbl;
        }
        tdbloop.nlegs++;
        clcleg = cca->reverse ? clcleg->prev_series_leg : clcleg->next_series_leg;
      }
      cca = cca->next_arrow;
    }
    tdbloop.id = ++loopid;
    this->loop_list.insert(this->loop_list.end(), tdbloop);
    lsi++;
  }


#ifdef THDEBUG
  thprintf("\nSHOT LOOPS:\n");
  dbli = this->loop_list.begin();
  i = 0;
  while (dbli != this->loop_list.end()) {
    thprintf("%ld %s %ld: %s@%s", i, dbli->open ? "OPN" : "CLS", dbli->nlegs,
      dbli->from->name, dbli->from->survey->name);
    ptdbl = dbli->first_leg;
    while (ptdbl != NULL) {
      if (ptdbl->reverse) {
        thprintf(" - %s@%s", 
          this->station_vec[this->station_vec[ptdbl->leg->from.id - 1].uid - 1].name,
          this->station_vec[this->station_vec[ptdbl->leg->from.id - 1].uid - 1].survey->name);
      } else {
        thprintf(" - %s@%s", 
          this->station_vec[this->station_vec[ptdbl->leg->to.id - 1].uid - 1].name,
          this->station_vec[this->station_vec[ptdbl->leg->to.id - 1].uid - 1].survey->name);
      }
      ptdbl = ptdbl->next_leg;
    }
    thprintf("\n");
    dbli++;
    i++;
  }
#endif  
  
  LC_COORD_CALC:
  
  delete [] crossst;
  delete [] lclegs;
  delete [] lccrosses;
  delete [] lccrossarrows;

#ifdef THDEBUG    
  thprintf("\nEND OF LOOP CLOSURE DEBUG\n");
#endif

#ifdef THDEBUG
    thprintf("\nend of loops searching\n\n");
#else
    thprintf("done\n");
    thtext_inline = false;
#endif
    
}

void thdb1d::close_loops()
{

  unsigned long nlegs = this->get_tree_size(),
    nstations = this->station_vec.size();    
    
  long i;

#ifdef THDEBUG
    thprintf("\n\ncalculating station coordinates\n");
#else
    thprintf("calculating station coordinates ... ");
    thtext_inline = true;
#endif 
  
  // * priradi suradnice UID fixnym bodom
  thdata * dp;
  bool anyfixed = false;
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  thdatafix_list::iterator fii;
  thdb1ds * ps;
  thdataobject * cx1, * cx2;
  thdb1ds * tmps;

  double avg_error = 0.0, avg_error_sum = 0.0;
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = dynamic_cast<thdata*>(obi->get());
      fii = dp->fix_list.begin();
      while(fii != dp->fix_list.end()) {
        tmps = &(this->station_vec[fii->station.id - 1]);
        cx1 = tmps->fixcontext;
        cx2 = fii->srcf.context;
        if (((cx1 == NULL) && (cx2 == NULL)) || ((cx1 != NULL) && (cx2 != NULL) && (cx1->id == cx2->id))) {
          ps = &(this->station_vec[this->station_vec[fii->station.id - 1].uid - 1]);
          ps->placed = 1;
          ps->fixed = true;
          anyfixed = true;
          ps->x = fii->x;
          ps->y = fii->y;
          ps->z = fii->z;
          if (!thisnan(fii->sdx)) {
            ps->sdx = fii->sdx;
            ps->sdy = fii->sdy;
            ps->sdz = fii->sdz;
          }
        }
        fii++;
      }
    }
    obi++;
  }
  
  // * zrata okruhy (err_(dx|dy|dz), (err|src)_length)
  // * a potom vyrovna okruhy (nastavi legs)
  double src_dx, src_dy, src_dz, dst_dx, dst_dy, dst_dz, sum_length,
    dx, dy, dz, sum_sdx, sum_sdy, sum_sdz, sumlegs, err_dx, err_dy, err_dz;
  thdb1d_loop_list_type::iterator li;
  thdb1d_loop_leg * ll; unsigned mode;

#ifdef THDEBUG
#define nmodes 3
#else
#define nmodes 2
#endif
  
  for (mode = 0; mode < nmodes; mode++) {

    li = this->loop_list.begin();
    while (li != this->loop_list.end()) {
    
      sum_sdx = 0.0;
      sum_sdy = 0.0;
      sum_sdz = 0.0;
      src_dx = 0.0;
      src_dy = 0.0;
      src_dz = 0.0;
      
      if (li->open) {
        dst_dx = li->to->x - li->from->x;
        dst_dy = li->to->y - li->from->y;
        dst_dz = li->to->z - li->from->z;
        if (!li->from->adjusted) {
          sum_sdx += li->from->sdx * li->from->sdx;
          sum_sdx += li->from->sdy * li->from->sdy;
          sum_sdx += li->from->sdz * li->from->sdz;
        }
        if (!li->to->adjusted) {
          sum_sdx += li->to->sdx * li->to->sdx;
          sum_sdx += li->to->sdy * li->to->sdy;
          sum_sdx += li->to->sdz * li->to->sdz;
        }
      } else {
        dst_dx = 0.0;
        dst_dy = 0.0;
        dst_dz = 0.0;
      }
      
      // prvy prechod - spocita chyby a sumu odchylok
      sumlegs = 0.0;
      sum_length = 0.0;
      ll = li->first_leg;
      while (ll != NULL) {
      
        if (ll->reverse) {
          dx = - ll->leg->adj_dx;
          dy = - ll->leg->adj_dy;
          dz = - ll->leg->adj_dz;
        } else {
          dx = ll->leg->adj_dx;
          dy = ll->leg->adj_dy;
          dz = ll->leg->adj_dz;
        }
        sum_length += ll->leg->total_length;
      
        if (ll->leg->adjusted) {
          dst_dx -= dx;
          dst_dy -= dy;
          dst_dz -= dz;
        } else {
          src_dx += dx;
          src_dy += dy;
          src_dz += dz;
          if (!ll->leg->plumbed) {
            sum_sdx += ll->leg->total_sdx;
            sum_sdy += ll->leg->total_sdy;
          }
          sum_sdz += ll->leg->total_sdz;
        }
        
        sumlegs += 1.0;
        ll = ll->next_leg;
      }
      
      // zapise do statistiky
#ifdef THDEBUG
      if (mode == 2) {
        thprintf("CORRECTION: %7.4f%% -> %7.4f%%\n",
          li->src_length > 0 ? 100.0 * li->err_length / li->src_length : 0.0,
          li->src_length > 0 ? 100.0 * sqrt(pow(dst_dx - src_dx,2.0) + pow(dst_dy - src_dy,2.0) + pow(dst_dz - src_dz,2.0)) / li->src_length : 0.0);
      }      
      else 
#endif      
      if (mode == 0) {
        li->src_length = sum_length;
        li->err_dx = dst_dx - src_dx;
        li->err_dy = dst_dy - src_dy;
        li->err_dz = dst_dz - src_dz;
        li->err_length = sqrt(li->err_dx * li->err_dx + li->err_dy * li->err_dy + li->err_dz * li->err_dz);
        if (sum_length > 0.0) {
          li->err = 100.0 * (li->err_length / sum_length);
          avg_error += sumlegs * li->err;
          avg_error_sum += sumlegs;
          ll = li->first_leg;
          while (ll != NULL) {
            if ((ll->leg->loop == NULL) || (ll->leg->loop->err < li->err)) {
              ll->leg->loop = &(*li);
            }
            ll = ll->next_leg;
          }
        }
      // vyrovna okruh
      } else {
        err_dx = dst_dx - src_dx;
        err_dy = dst_dy - src_dy;
        err_dz = dst_dz - src_dz;

        // ak je okruh otvoreny a std > 0, adjustne koncove body
        if (li->open) {
          if (!li->from->adjusted) {
            if (sum_sdx > 0.0)
              li->from->x -= err_dx * li->from->sdx / sum_sdx;
            if (sum_sdy > 0.0)
              li->from->y -= err_dy * li->from->sdy / sum_sdy;
            if (sum_sdz > 0.0)
              li->from->z -= err_dz * li->from->sdz / sum_sdz;
            li->from->adjusted = true;
          }
          if (!li->to->adjusted) {
            if (sum_sdx > 0.0)
              li->to->x += err_dx * li->to->sdx / sum_sdx;
            if (sum_sdy > 0.0)
              li->to->y += err_dy * li->to->sdy / sum_sdy;
            if (sum_sdz > 0.0)
              li->to->z += err_dz * li->to->sdz / sum_sdz;
            li->to->adjusted = true;
          }
        }
        
        ll = li->first_leg;
        while (ll != NULL) {
          if (!ll->leg->adjusted) {

#define ORIENT (ll->reverse ? -1.0 : 1.0)
            if (sum_sdx > 0.0)
              ll->leg->adj_dx += ORIENT * err_dx * (ll->leg->plumbed ? 0.0 : ll->leg->total_sdx) / sum_sdx;
            else
              ll->leg->adj_dx += ORIENT * err_dx / sumlegs;
            if (sum_sdy > 0.0)
              ll->leg->adj_dy += ORIENT * err_dy * (ll->leg->plumbed ? 0.0 : ll->leg->total_sdy) / sum_sdy;
            else
              ll->leg->adj_dy += ORIENT * err_dy / sumlegs;
            if (sum_sdz > 0.0)
              ll->leg->adj_dz += ORIENT * err_dz * ll->leg->total_sdz / sum_sdz;
            else
              ll->leg->adj_dz += ORIENT * err_dz / sumlegs;
            ll->leg->adjusted = true;
#ifdef THDEBUG
            thprintf("ADJLEG %s@%s - %s@%s:    %.2f,%.2f,%.2f -> %.2f,%.2f,%.2f\n", this->station_vec[ll->leg->from.id - 1].name, this->station_vec[ll->leg->from.id - 1].survey->name, this->station_vec[ll->leg->to.id - 1].name, this->station_vec[ll->leg->to.id - 1].survey->name,
						  ll->leg->total_dx, ll->leg->total_dy, ll->leg->total_dz, 
							ll->leg->adj_dx, ll->leg->adj_dy, ll->leg->adj_dz);
#endif				
          }
          ll = ll->next_leg;
        }
      } // koniec vyrovnavanie
      li++;
    } // koniec cyklu okruhov
  } // koniec modoveho cyklu
  

  // * spocita suradnice UID bodom, tak ze prejde vsetky legy a 
  // * poojde z UID na UID
  thdb1ds * froms, * tos;
  thdb1dl ** legs = this->get_tree_legs(), * cleg;
  long unrecover = -1;
  
  
  for (i = 0; (unsigned long) i < nlegs; i++) {
    legs[i]->leg->adjusted = false;
  }
  
  for (i = 0; (unsigned long) i < nlegs; i++) {

    cleg = legs[i];

    if (cleg->reverse) {
      froms = &(this->station_vec[this->station_vec[cleg->leg->to.id - 1].uid - 1]);
      tos = &(this->station_vec[this->station_vec[cleg->leg->from.id - 1].uid - 1]);
    } else {
      froms = &(this->station_vec[this->station_vec[cleg->leg->from.id - 1].uid - 1]);
      tos = &(this->station_vec[this->station_vec[cleg->leg->to.id - 1].uid - 1]);
    }


    if (cleg->leg->data_type == TT_DATATYPE_NOSURVEY) {
      // ak je no-survey, nastavi mu total statistiku
      cleg->leg->total_dx = tos->x - froms->x;
      cleg->leg->total_dy = tos->y - froms->y;
      cleg->leg->total_dz = tos->z - froms->z;
      // najprv horizontalnu dlzku
      cleg->leg->total_length = std::hypot(cleg->leg->total_dx, cleg->leg->total_dy);
      cleg->leg->total_bearing = atan2(cleg->leg->total_dx, cleg->leg->total_dy) / THPI * 180.0;
      if (cleg->leg->total_bearing < 0.0)
        cleg->leg->total_bearing += 360.0;
      cleg->leg->total_gradient = atan2(cleg->leg->total_dz, cleg->leg->total_length) / THPI * 180.0;
      // potom celkovu dlzku
      cleg->leg->total_length = std::hypot(cleg->leg->total_length, cleg->leg->total_dz);
      continue;
    }
    
    if ((i == 0) && (!anyfixed)) {
      froms->placed = 1;
      froms->x = 0.0;
      froms->y = 0.0;
      froms->z = 0.0;
    }
    
#ifdef THDEBUG
  	thprintf("LEG: %s@%s [%ld] (from %s@%s [%ld])\n", 
		  tos->name, tos->survey->name, tos->uid, froms->name, froms->survey->name, froms->uid
		);
#endif
    if (froms->placed == 0)
      ththrow("a software BUG is present (" __FILE__ ":{})", __LINE__);
    if (tos->placed == 0) {
      tos->placed += 1;
      if (cleg->reverse) {
        tos->x = froms->x - cleg->leg->adj_dx;
        tos->y = froms->y - cleg->leg->adj_dy;
        tos->z = froms->z - cleg->leg->adj_dz;
      } else {
        tos->x = froms->x + cleg->leg->adj_dx;
        tos->y = froms->y + cleg->leg->adj_dy;
        tos->z = froms->z + cleg->leg->adj_dz;
      }
    }
    else {
      tos->placed += 1;
      err_dx = (tos->x - froms->x) - (cleg->reverse ? -1.0 : 1.0) * cleg->leg->adj_dx;
      err_dy = (tos->y - froms->y) - (cleg->reverse ? -1.0 : 1.0) * cleg->leg->adj_dy;
      err_dz = (tos->z - froms->z) - (cleg->reverse ? -1.0 : 1.0) * cleg->leg->adj_dz;
			double err = std::hypot(std::hypot(err_dx, err_dy), err_dz);
      
			if ((err >= 1e-4) && (i > unrecover)) {			
#ifdef THDEBUG
		    thprintf("ERRLEG %s@%s - %s@%s: %.2f,%.2f,%.2f\n", 
				  froms->name, froms->survey->name, tos->name, tos->survey->name,
          err_dx, err_dy, err_dz);
#endif    
        // mame problem - musime spravit nasledovne
        thdb1ds * errst, * lastst;
        errst = tos;
        lastst = tos;
        long start_i, end_i;
        start_i = 0;
        end_i = i;
        sumlegs = 0.0; sum_sdx = 0.0; sum_sdy = 0.0; sum_sdz = 0.0;
                
        // 1. najst zamery, na ktore chyby rozlozime
        for (i = end_i; i >= start_i; i--) {
          cleg = legs[i];
          if (cleg->reverse) {
            froms = &(this->station_vec[this->station_vec[cleg->leg->to.id - 1].uid - 1]);
            tos = &(this->station_vec[this->station_vec[cleg->leg->from.id - 1].uid - 1]);
          } else {
            froms = &(this->station_vec[this->station_vec[cleg->leg->from.id - 1].uid - 1]);
            tos = &(this->station_vec[this->station_vec[cleg->leg->to.id - 1].uid - 1]);
          }
          
          if ((!cleg->leg->adjusted) && (tos->uid == lastst->uid)) {
            sumlegs += 1.0;
            if (!cleg->leg->plumbed) {
              sum_sdx += cleg->leg->total_sdx;
              sum_sdy += cleg->leg->total_sdy;
            }
            sum_sdz += cleg->leg->total_sdz;
            cleg->leg->to_be_adjusted = true;
          } else {
            cleg->leg->to_be_adjusted = false;
          }
          
          if (tos->uid == lastst->uid) {
            lastst = froms;
            if ((froms->placed > 1) || (froms->fixed) || (froms->uid == errst->uid)) {
              start_i = i;
            }
          }
          
          if (tos->placed > 0)
            tos->placed--;            
        }
                
        // 2. rozlozit chybu na zamery
        for (i = start_i; i <= end_i; i++) {
          cleg = legs[i];
          if (cleg->leg->to_be_adjusted) {

#define ORIENT2 (cleg->reverse ? -1.0 : 1.0)
#ifdef THDEBUG
            thprintf("ADJLEG %s@%s - %s@%s:    %.2f,%.2f,%.2f", this->station_vec[cleg->leg->from.id - 1].name, this->station_vec[cleg->leg->from.id - 1].survey->name, this->station_vec[cleg->leg->to.id - 1].name, this->station_vec[cleg->leg->to.id - 1].survey->name,
							cleg->leg->adj_dx, cleg->leg->adj_dy, cleg->leg->adj_dz);
#endif				
            if (sum_sdx > 0.0)
              cleg->leg->adj_dx += ORIENT2 * err_dx * (cleg->leg->plumbed ? 0.0 : cleg->leg->total_sdx) / sum_sdx;
            else
              cleg->leg->adj_dx += ORIENT2 * err_dx / sumlegs;
            if (sum_sdy > 0.0)
              cleg->leg->adj_dy += ORIENT2 * err_dy * (cleg->leg->plumbed ? 0.0 : cleg->leg->total_sdy) / sum_sdy;
            else
              cleg->leg->adj_dy += ORIENT2 * err_dy / sumlegs;
            if (sum_sdz > 0.0)
              cleg->leg->adj_dz += ORIENT2 * err_dz * cleg->leg->total_sdz / sum_sdz;
            else
              cleg->leg->adj_dz += ORIENT2 * err_dz / sumlegs;
              
            cleg->leg->adjusted = true;
#ifdef THDEBUG
            thprintf(" -> %.2f,%.2f,%.2f\n",
							cleg->leg->adj_dx, cleg->leg->adj_dy, cleg->leg->adj_dz);
#endif				
          }
        }
        
        // 3. ratat este raz
          i = start_i - 1; 
        if (sumlegs < 1.0) {
#ifdef THDEBUG
          thprintf("ERRLEG UNRECOVERED!\n");
#endif				
          unrecover = end_i;
        }
          
			}
		}
  }
  
  // * potom priradi suradnice nie UID bodom
  for(i = 0; (unsigned long)i < nstations; i++) {
    ps = &(this->station_vec[i]);
    froms = &(this->station_vec[ps->uid - 1]);
    if (ps->placed == 0) {
      ps->placed++;
      ps->x = froms->x;
      ps->y = froms->y;
      ps->z = froms->z;
      if (ps->placed == 0) {
//        ththrow("a software BUG is present (" __FILE__ ":{})", __LINE__);
        ththrow("can not connect {}@{} to centerline network",
          this->station_vec[i].name,
          this->station_vec[i].survey->get_full_name());
      }
    }
  }

#ifdef THDEBUG
  thprintf("\nend of station coordinates calculation\n\n");
#else
  thprintf("done\n");
  thtext_inline = false;
#endif

  if (avg_error_sum > 0.0) {
    thprintf("average loop error: %.2f%%\n", (avg_error / avg_error_sum));
  }
  
}

struct thdb1d_lpr {
  thdb1d_loop * li;
  double err;
};


void thdb1d::print_loops() {
  thdb1d_loop_list_type::iterator lii = this->loop_list.begin();
  if (lii == this->loop_list.end())
    return;
  unsigned long i, nloops;
  thdb1d_loop * li;
  nloops = this->loop_list.size();
  std::vector<thdb1d_lpr> lpr(nloops);
  i = 0; 
  while (lii != this->loop_list.end()) {
    lpr[i].li = &(*lii);
    if (lii->src_length > 0.0)
      lpr[i].err = lii->err_length / lii->src_length;
    else
      lpr[i].err = 0.0;
    i++;
    lii++;
  }  
  
  std::sort(lpr.begin(), lpr.end(), [](const auto& a, const auto& b){ return a.err >= b.err; });
  
  thdb1d_loop_leg * ll;
  thsurvey * ss;   
	int totlen = 6 - strlen(thdeflocale.format_length_units()) + 1;
  thdb1ds * ps;
  unsigned long psid, prev_psid, first_psid;
  
  thlog.printf("\n\n######################### loop errors ##########################\n");
  thlog.printf(    "REL-ERR ABS-ERR TOTAL-L STS X-ERROR Y-ERROR Z-ERROR STATIONS\n");
  for (i = 0; i < nloops; i++) {
    li = lpr[i].li;
    thlog.printf("%6.2f%% %s%s %s%s %3ld %s%s %s%s %s%s [",
      li->src_length > 0.0 ? 100.0 * li->err_length / li->src_length : 0.0,
			thdeflocale.format_length(li->err_length,1,totlen), thdeflocale.format_length_units(),			
			thdeflocale.format_length(li->src_length,1,totlen), thdeflocale.format_length_units(),			
			li->nlegs,
			thdeflocale.format_length(li->err_dx,1,totlen), thdeflocale.format_length_units(),			
			thdeflocale.format_length(li->err_dy,1,totlen), thdeflocale.format_length_units(),			
			thdeflocale.format_length(li->err_dz,1,totlen), thdeflocale.format_length_units());
    ll = li->first_leg;
    ss = NULL;
    if (ll->reverse)
      psid = ll->leg->to.id;
    else
      psid = ll->leg->from.id;
    ps = &(this->station_vec[psid - 1]);
    first_psid = psid;
    thlog.printf("%s", ps->name);
    if ((ss == NULL) || (ss->id != ps->survey->id)) {
      ss = ps->survey;
      thlog.printf("@%s", ss->get_full_name());
    }
    prev_psid = psid;
    while (ll != NULL) {
    
      if (ll->reverse)
        psid = ll->leg->to.id;
      else
        psid = ll->leg->from.id;

      if (prev_psid != psid) {
        ps = &(this->station_vec[psid - 1]);
        thlog.printf(" = %s", ps->name);
        if (ss->id != ps->survey->id) {
          ss = ps->survey;
          thlog.printf("@%s", ss->get_full_name());
        }
      }

      if (ll->reverse)
        psid = ll->leg->from.id;
      else
        psid = ll->leg->to.id;
      ps = &(this->station_vec[psid - 1]);
      thlog.printf(" - %s", ps->name);
      if (ss->id != ps->survey->id) {
        ss = ps->survey;
        thlog.printf("@%s", ss->get_full_name());
      }
      
      if ((ll->next_leg == NULL) && (!li->open)) {
        if (first_psid != psid) {
          psid = first_psid;
          ps = &(this->station_vec[psid - 1]);
          thlog.printf(" = %s", ps->name);
          if (ss->id != ps->survey->id) {
            ss = ps->survey;
            thlog.printf("@%s", ss->get_full_name());
          }
        }
      }

      prev_psid = psid;
      ll = ll->next_leg;
    }
    thlog.printf("]\n");
  }
  thlog.printf("##################### end of loop errors #######################\n");
}

thdb3ddata * thdb1d::get_3d_surface() {
  this->get_3d();
  return &(this->d3_surface);
}


thdb3ddata * thdb1d::get_3d_splay() {
  this->get_3d();
  return &(this->d3_splay);
}


thdb3ddata * thdb1d::get_3d_walls() {
  this->get_3d();
  return &(this->d3_walls);
}


thdb3ddata * thdb1d::get_3d() {
  // vrati 3D data - ale len tie, ktore su oznacene
  if (this->d3_data_parsed)
    return &(this->d3_data);

  this->d3_data_parsed = true;
  unsigned long nlegs = this->get_tree_size(),
    nstat = this->station_vec.size(), i, last_st, cur_st;
  if (nstat <= 0)
    return &(this->d3_data);
    
  // najprv tam vlozi meracske bodiky
  
  thdb3dvx ** station_in = new thdb3dvx* [nstat];

  // najprv podzemne data
  for (i = 0; i < nstat; i++) 
    station_in[i] = NULL;

#define get_3d_check_station(id) { \
      if (station_in[id] == NULL) { \
        station_in[id] = this->d3_data.insert_vertex( \
          this->station_vec[id].x, \
          this->station_vec[id].y, \
          this->station_vec[id].z); \
      } \
    }
  
  // polygony tam vlozi ako linestripy (data poojdu na thdb1dl)
  last_st = nstat;
  thdb3dfc * fc = NULL, * secfc = NULL, * endsfc = NULL;
  thdb3dvx * fsecvx[10], * tsecvx[10];
  thdb1dl ** tlegs = this->get_tree_legs();  
  double secx[10], secy[10], secXx, secXy, secXz,
    secYx, secYy, secYz;
  thdb1ds * fromst, * tost;
  int secn, j, prevj, nextj;
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected() && (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) == 0) && (((*tlegs)->leg->flags & TT_LEGFLAG_SPLAY) == 0)) {
      cur_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      get_3d_check_station(cur_st);
      if (cur_st != last_st) {
        fc = this->d3_data.insert_face(THDB3DFC_LINE_STRIP);
        fc->insert_vertex(station_in[cur_st]);
      }
      last_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      get_3d_check_station(last_st);
      fc->insert_vertex(station_in[last_st]);
      
      // tu vygeneruje LRUD obalku
      
      if ((*tlegs)->leg->walls == TT_FALSE) goto SKIP_WALLS;
      
#define secpt(N,X,Y) secx[N] = X; secy[N] = Y;      
      switch ((*tlegs)->leg->shape) {
        case TT_DATALEG_SHAPE_DIAMOND:
          secpt(0,0.5,0.0);
          secpt(1,1.0,0.5);
          secpt(2,0.5,1.0);
          secpt(3,0.0,0.5);
          secn = 4;
          break;
        case TT_DATALEG_SHAPE_OCTAGON:
          secpt(0,0.292893218813,0.0);
          secpt(1,0.707106781167,0.0);
          secpt(2,1.0,0.292893218813);
          secpt(3,1.0,0.707106781167);
          secpt(4,0.707106781167,1.0);
          secpt(5,0.292893218813,1.0);
          secpt(6,0.0,0.707106781167);
          secpt(7,0.0,0.292893218813);
          secn = 8;
          break;
        case TT_DATALEG_SHAPE_TRIANGLE:
          secpt(0,0.0,0.0);
          secpt(1,1.0,0.0);
          secpt(2,0.5,1.0);
          secn = 3;
          break;
        case TT_DATALEG_SHAPE_TUNNEL:
          secpt(0,0.0,0.0);
          secpt(1,1.0,0.0);
          secpt(2,0.904508497187,0.61803398875);
          secpt(3,0.654508497187,1.0);
          secpt(4,0.345491502823,1.0);
          secpt(5,0.095491502823,0.61803398875);
          secn = 6;
          break;
        case TT_DATALEG_SHAPE_RECTANGLE:
        default:
          secpt(0,0.0,0.0);
          secpt(1,1.0,0.0);
          secpt(2,1.0,1.0);
          secpt(3,0.0,1.0);
          secn = 4;
          break;
      }
      
      // spocitame si X a Y section vectors
      secXx = cos((*tlegs)->leg->total_bearing / 180.0 * THPI);
      secXy = -sin((*tlegs)->leg->total_bearing / 180.0 * THPI);
      secXz = 0;
      if (fabs((*tlegs)->leg->total_gradient) < (*tlegs)->leg->vtresh) {
        secYx = 0.0;
        secYy = 0.0;
        secYz = 1.0;
      } else {
        secYx = -sin((*tlegs)->leg->total_gradient / 180.0 * THPI) * sin((*tlegs)->leg->total_bearing / 180.0 * THPI);
        secYy = -sin((*tlegs)->leg->total_gradient / 180.0 * THPI) * cos((*tlegs)->leg->total_bearing / 180.0 * THPI);
        secYz = cos((*tlegs)->leg->total_gradient / 180.0 * THPI);
      }
      
      secfc = this->d3_walls.insert_face(THDB3DFC_TRIANGLE_STRIP);
      endsfc = this->d3_walls.insert_face(THDB3DFC_TRIANGLES);
      // vypocitame a vlozime body aj s normalami
      fromst = &(this->station_vec[(*tlegs)->leg->from.id - 1]);
      tost = &(this->station_vec[(*tlegs)->leg->to.id - 1]);

//#define SYMPASSAGES
      for(j = 0; j < secn; j++) {

        fsecvx[j] = this->d3_walls.insert_vertex(
#ifdef SYMPASSAGES
          fromst->x - 
          (*tlegs)->leg->from_left * secXx -
          (*tlegs)->leg->from_down * secYx + 
          secx[j] * ((*tlegs)->leg->from_left + (*tlegs)->leg->from_right) * secXx +
          secy[j] * ((*tlegs)->leg->from_up + (*tlegs)->leg->from_down) * secYx,
          fromst->y - 
          (*tlegs)->leg->from_left * secXy -
          (*tlegs)->leg->from_down * secYy + 
          secx[j] * ((*tlegs)->leg->from_left + (*tlegs)->leg->from_right) * secXy +
          secy[j] * ((*tlegs)->leg->from_up + (*tlegs)->leg->from_down) * secYy,
          fromst->z - 
          (*tlegs)->leg->from_left * secXz -
          (*tlegs)->leg->from_down * secYz + 
          secx[j] * ((*tlegs)->leg->from_left + (*tlegs)->leg->from_right) * secXz +
          secy[j] * ((*tlegs)->leg->from_up + (*tlegs)->leg->from_down) * secYz
#else
          fromst->x + 
          (secx[j] < 0.5 ? (*tlegs)->leg->from_left : (*tlegs)->leg->from_right) * (secx[j] - 0.5) / 0.5 * secXx +
          (secy[j] < 0.5 ? (*tlegs)->leg->from_down : (*tlegs)->leg->from_up) * (secy[j] - 0.5) / 0.5 * secYx,
          fromst->y + 
          (secx[j] < 0.5 ? (*tlegs)->leg->from_left : (*tlegs)->leg->from_right) * (secx[j] - 0.5) / 0.5 * secXy +
          (secy[j] < 0.5 ? (*tlegs)->leg->from_down : (*tlegs)->leg->from_up) * (secy[j] - 0.5) / 0.5 * secYy,
          fromst->z + 
          (secx[j] < 0.5 ? (*tlegs)->leg->from_left : (*tlegs)->leg->from_right) * (secx[j] - 0.5) / 0.5 * secXz +
          (secy[j] < 0.5 ? (*tlegs)->leg->from_down : (*tlegs)->leg->from_up) * (secy[j] - 0.5) / 0.5 * secYz
#endif          
          );

        tsecvx[j] = this->d3_walls.insert_vertex(
#ifdef SYMPASSAGES
          tost->x - 
          (*tlegs)->leg->to_left * secXx -
          (*tlegs)->leg->to_down * secYx + 
          secx[j] * ((*tlegs)->leg->to_left + (*tlegs)->leg->to_right) * secXx +
          secy[j] * ((*tlegs)->leg->to_up + (*tlegs)->leg->to_down) * secYx,
          tost->y - 
          (*tlegs)->leg->to_left * secXy -
          (*tlegs)->leg->to_down * secYy + 
          secx[j] * ((*tlegs)->leg->to_left + (*tlegs)->leg->to_right) * secXy +
          secy[j] * ((*tlegs)->leg->to_up + (*tlegs)->leg->to_down) * secYy,
          tost->z - 
          (*tlegs)->leg->to_left * secXz -
          (*tlegs)->leg->to_down * secYz + 
          secx[j] * ((*tlegs)->leg->to_left + (*tlegs)->leg->to_right) * secXz +
          secy[j] * ((*tlegs)->leg->to_up + (*tlegs)->leg->to_down) * secYz
#else
          tost->x + 
          (secx[j] < 0.5 ? (*tlegs)->leg->to_left : (*tlegs)->leg->to_right) * (secx[j] - 0.5) / 0.5 * secXx +
          (secy[j] < 0.5 ? (*tlegs)->leg->to_down : (*tlegs)->leg->to_up) * (secy[j] - 0.5) / 0.5 * secYx,
          tost->y + 
          (secx[j] < 0.5 ? (*tlegs)->leg->to_left : (*tlegs)->leg->to_right) * (secx[j] - 0.5) / 0.5 * secXy +
          (secy[j] < 0.5 ? (*tlegs)->leg->to_down : (*tlegs)->leg->to_up) * (secy[j] - 0.5) / 0.5 * secYy,
          tost->z + 
          (secx[j] < 0.5 ? (*tlegs)->leg->to_left : (*tlegs)->leg->to_right) * (secx[j] - 0.5) / 0.5 * secXz +
          (secy[j] < 0.5 ? (*tlegs)->leg->to_down : (*tlegs)->leg->to_up) * (secy[j] - 0.5) / 0.5 * secYz
#endif          
          );

      }

      // spocitame normaly na vrcholoch
      prevj = secn - 1;
      for(j = 0; j < secn; j++) {
       nextj = j + 1;
       if (nextj == secn) nextj = 0;
       
       // zrata normalu medzi vrcholmi prevj - j - nextj
       lxVec v1, v2, v3;
       v1 = lxVec(fsecvx[j]->x - fsecvx[prevj]->x, fsecvx[j]->y - fsecvx[prevj]->y, fsecvx[j]->z - fsecvx[prevj]->z);
       v2 = lxVec(fsecvx[j]->x - fsecvx[nextj]->x, fsecvx[j]->y - fsecvx[nextj]->y, fsecvx[j]->z - fsecvx[nextj]->z);
       v3 = lxVec(fsecvx[j]->x - tsecvx[j]->x, fsecvx[j]->y - tsecvx[j]->y, fsecvx[j]->z - tsecvx[j]->z);
       v1 = v1 ^ v3; v1.Normalize();
       v2 = v2 ^ v3; v2.Normalize();
       v3 = v1 - v2; v3.Normalize();
       fsecvx[j]->insert_normal(v3.x, v3.y, v3.z);

       v1 = lxVec(tsecvx[j]->x - tsecvx[prevj]->x, tsecvx[j]->y - tsecvx[prevj]->y, tsecvx[j]->z - tsecvx[prevj]->z);
       v2 = lxVec(tsecvx[j]->x - tsecvx[nextj]->x, tsecvx[j]->y - tsecvx[nextj]->y, tsecvx[j]->z - tsecvx[nextj]->z);
       v3 = lxVec(tsecvx[j]->x - fsecvx[j]->x, tsecvx[j]->y - fsecvx[j]->y, tsecvx[j]->z - fsecvx[j]->z);
       v1 = v1 ^ v3; v1.Normalize();
       v2 = v2 ^ v3; v2.Normalize();
       v3 = v2 - v1; v3.Normalize();
       tsecvx[j]->insert_normal(v3.x, v3.y, v3.z);
       
       if (j > 1) {
         endsfc->insert_vertex(fsecvx[0]);
         endsfc->insert_vertex(fsecvx[j - 1]);
         endsfc->insert_vertex(fsecvx[j]);
         endsfc->insert_vertex(tsecvx[0]);
         endsfc->insert_vertex(tsecvx[j]);
         endsfc->insert_vertex(tsecvx[j - 1]);
       }
       
       prevj = j;
      }      
      // vlozime triangle strip
      for(j = 0; j < secn; j++) {
        secfc->insert_vertex(fsecvx[j]);
        secfc->insert_vertex(tsecvx[j]);
      }
      secfc->insert_vertex(fsecvx[0]);
      secfc->insert_vertex(tsecvx[0]);
      
      // koniec generovania LRUD obalky
      SKIP_WALLS:;
    }
  }
  
  // potom povrchove data
  for (i = 0; i < nstat; i++) 
    station_in[i] = NULL;
#undef get_3d_check_station
#define get_3d_check_station(id) { \
      if (station_in[id] == NULL) { \
        station_in[id] = this->d3_surface.insert_vertex( \
          this->station_vec[id].x, \
          this->station_vec[id].y, \
          this->station_vec[id].z); \
      } \
    }
  
  // polygony tam vlozi ako linestripy (data poojdu na thdb1dl)
  last_st = nstat;
  fc = NULL;
  tlegs = this->get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected() && (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0) && (((*tlegs)->leg->flags & TT_LEGFLAG_SPLAY) == 0)) {
      cur_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      get_3d_check_station(cur_st);
      if (cur_st != last_st) {
        fc = this->d3_surface.insert_face(THDB3DFC_LINE_STRIP);
        fc->insert_vertex(station_in[cur_st]);
      }
      last_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      get_3d_check_station(last_st);
      fc->insert_vertex(station_in[last_st]);
    }
  }
  
  // potom splay data
  for (i = 0; i < nstat; i++) 
    station_in[i] = NULL;
#undef get_3d_check_station
#define get_3d_check_station(id) { \
      if (station_in[id] == NULL) { \
        station_in[id] = this->d3_splay.insert_vertex( \
          this->station_vec[id].x, \
          this->station_vec[id].y, \
          this->station_vec[id].z); \
      } \
    }
  
  // polygony tam vlozi ako linestripy (data poojdu na thdb1dl)
  last_st = nstat;
  fc = NULL;
  tlegs = this->get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected() && (((*tlegs)->leg->flags & TT_LEGFLAG_SPLAY) != 0)) {
      cur_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      get_3d_check_station(cur_st);
      if (cur_st != last_st) {
        fc = this->d3_splay.insert_face(THDB3DFC_LINE_STRIP);
        fc->insert_vertex(station_in[cur_st]);
      }
      last_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      get_3d_check_station(last_st);
      fc->insert_vertex(station_in[last_st]);
    }
  }

	
	// meracske body tam vlozi ako body (data na dbdb1ds)
  delete [] station_in;
  return &(this->d3_data);
  
}


void thdb1d::postprocess_objects() 
{
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    switch ((*obi)->get_class_id()) {
      case TT_SURFACE_CMD:
        dynamic_cast<thsurface*>(obi->get())->check_stations();
        break;
    }
    obi++;
  }
  // zrata priemerne dlzky shotov pre vsetky stations kvuoli interpolacii
  thdb1d_tree_arrow * arrow;
  thdb1d_tree_node * node, * nodes = this->get_tree_nodes();
  thdb1ds * s;
  double numl;
  for(unsigned long i = 0; i < this->station_vec.size(); i++) {
    s = &(this->station_vec[i]);
    if (s->uid < (i + 1)) {
      s->asl = this->station_vec[s->uid - 1].asl;
    } else {
      s->asl = 0.0;
      numl = 0;
      node = &(nodes[i]);
      arrow = node->first_arrow;
      while (arrow != NULL) {
        if ((arrow->leg->leg->flags & TT_LEGFLAG_SURFACE) == 0) {
          s->asl += arrow->leg->leg->total_length;
          numl += 1.0;
        }
        arrow = arrow->next_arrow;
      }
      if (s->asl == 0.0)
        s->asl = thnan;
      else
        s->asl /= numl;
    }
  }
}


double diffdir(double dir1, double dir2) {
  return acos(cos(dir1) * cos(dir2) + sin(dir1) * sin(dir2));
}


void thdb1d::process_xelev()
{
  unsigned long tn_legs = this->leg_vec.size();
  unsigned long tn_stations = this->station_vec.size();

#ifdef THDEBUG
    thprintf("\n\nprocessing extended elevation\n");
#else
    thprintf("processing extended elevation ... ");
    thtext_inline = true;
    if (thcfg.log_extend) {
      thprintf("\n");
    }
#endif 
    
  // PREPROCESS
  thdataextend_list::iterator xi;
  thdb1ds * st1;
  thdata * dp;
  thdb1d_tree_node * nodes = this->get_tree_nodes(), * from_node, * to_node;
  thdb1d_tree_arrow * carrow;
  thbuffer tmpbf;
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    switch ((*obi)->get_class_id()) {
      case TT_DATA_CMD:
        try {
          dp = dynamic_cast<thdata*>(obi->get());
          xi = dp->extend_list.begin();
          while(xi != dp->extend_list.end()) {
            if (!xi->to.is_empty()) {
              // find shot to apply extend to
              from_node = nodes + (nodes[xi->from.id - 1].uid - 1);
              to_node = nodes + (nodes[xi->to.id - 1].uid - 1);
              carrow = from_node->first_arrow;
              while (carrow != NULL) {
                if (carrow->end_node->uid == to_node->uid)
                  break;
                carrow = carrow->next_arrow;
              }
              if (carrow == NULL) {
                tmpbf = xi->from.name;
                if (xi->from.survey != NULL) {
                  tmpbf += "@";
                  tmpbf += xi->from.survey;
                }
                tmpbf += " - ";
                tmpbf += xi->to.name;
                if (xi->to.survey != NULL) {
                  tmpbf += "@";
                  tmpbf += xi->to.survey;
                }
                ththrow("survey shot not found -- {}", tmpbf.get_buffer());
              } else {
                // the leg is in carrow - set its extend
                if ((xi->extend & TT_EXTENDFLAG_DIRECTION) != 0) {
                  carrow->leg->leg->extend &= ~TT_EXTENDFLAG_DIRECTION;
                  carrow->extend &= ~TT_EXTENDFLAG_DIRECTION;
                }
                if (!xi->before.is_empty()) {
                	carrow->extend |= xi->extend;
                	if ((carrow->extend & TT_EXTENDFLAG_IGNORE) > 0) {
						carrow->extend |= TT_EXTENDFLAG_CNDIGNORE;
						carrow->extend &= ~TT_EXTENDFLAG_IGNORE;
						carrow->extend_ignore_before.push_back(nodes[xi->before.id - 1].uid);
                	}
                } else
                	carrow->extend |= xi->extend;
                switch (xi->extend) {
                  case TT_EXTENDFLAG_LEFT:
                    if (carrow->is_reversed)
                      carrow->leg->leg->extend |= TT_EXTENDFLAG_RIGHT;
                    else
                      carrow->leg->leg->extend |= TT_EXTENDFLAG_LEFT;
                    break;
                  case TT_EXTENDFLAG_RIGHT:
                    if (carrow->is_reversed)
                      carrow->leg->leg->extend |= TT_EXTENDFLAG_LEFT;
                    else
                      carrow->leg->leg->extend |= TT_EXTENDFLAG_RIGHT;
                    break;
                  case TT_EXTENDFLAG_VERTICAL:
                    carrow->leg->leg->extend |= TT_EXTENDFLAG_VERTICAL;
                    carrow->negative->extend |= TT_EXTENDFLAG_VERTICAL;
                    break;
                  case TT_EXTENDFLAG_UNKNOWN:
                    carrow->leg->leg->extend_ratio = xi->extend_ratio;
                    break;
                  default:
                    carrow->leg->leg->extend |= xi->extend;
                }
              }
            } else {
              // add station extend attribute
              st1 = &(this->station_vec[xi->from.id - 1]);
              from_node = nodes + (nodes[xi->from.id - 1].uid - 1);
              if ((xi->extend & TT_EXTENDFLAG_DIRECTION) != 0) {
                st1->extend &= ~TT_EXTENDFLAG_DIRECTION;
              }              
              st1->extend |= xi->extend;
              if (xi->extend == TT_EXTENDFLAG_UNKNOWN)
                st1->extend_ratio = xi->extend_ratio;
              if ((xi->extend & TT_EXTENDFLAG_IGNORE) != 0) {
                carrow = from_node->first_arrow;
                while (carrow != NULL) {
                  carrow->leg->leg->extend |= TT_EXTENDFLAG_IGNORE;
                  carrow->extend |= TT_EXTENDFLAG_IGNORE;
                  //carrow->negative->extend |= TT_EXTENDFLAG_IGNORE;
                  carrow = carrow->next_arrow;
                }
              }
              if ((xi->extend & TT_EXTENDFLAG_HIDE) != 0) {
                carrow = from_node->first_arrow;
                while (carrow != NULL) {
                  carrow->leg->leg->extend |= TT_EXTENDFLAG_HIDE;
                  carrow->extend |= TT_EXTENDFLAG_HIDE;
                  carrow->negative->extend |= TT_EXTENDFLAG_HIDE;
                  carrow = carrow->next_arrow;
                }
              }
              if (xi->extend == TT_EXTENDFLAG_BREAK) {
                carrow = from_node->first_arrow;
                while (carrow != NULL) {
                  carrow->extend |= TT_EXTENDFLAG_BREAK;
                  carrow = carrow->next_arrow;
                }
              }
            }
            xi++;
          }
        }
        catch (...) {
          threthrow("{} [{}]", xi->srcf.name, xi->srcf.line);
        }
        break;
    }
    obi++;
  }

  // PREPARE tree for new tremaux
  // is_attached to false
  unsigned long i;
  thdb1d_tree_node * n1, * current_node = NULL;
  thdb1d_tree_arrow * a1, * a2;

  // vsetky is discovery na false
  a1 = this->tree_arrows;
  for(i = 0; i < tn_legs; i++) {
    a1->is_discovery = false; a1++;
    a1->is_discovery = false; a1++;
  }

  // poojde v modoch normal/ignoreignore
  
  // FIND ROOT STATION AND FIRST SHOT
  // + traverse all stations
  // + find start in survey in highest level
  // + find top entrance
  // + find top station
  //
  // SHOT:
  // + find free start shot
  // + find free non ignore shot (ak nie ignoreignore)
  
  // EXTEND FROM ROOT
  // until everything is calculated

  unsigned long tarrows = 0;
  unsigned long prev_id = 0;
  bool component_break = true;
  bool ignorant_mode = false, just_started = true;
  int default_left(1), go_left; // -1 - left, 1 - right, 0 - vertical
  int start_level, clevel;
  double default_ratio(1.0), go_ratio;
  double cxx = 0.0;

  while (tarrows < tn_legs) {
  
    if (component_break) {
    
      for(i = 0, n1 = nodes; i < tn_stations; i++, n1++) {
        n1->is_attached = false;
      }
      a1 = this->tree_arrows;
      start_level = 0;
      a2 = NULL;

      for(i = 0; i < 2 * tn_legs; i++) {
        if (!a1->is_discovery) {
          // only ignore shots left
          clevel = 0;
          if ((a1->leg->leg->extend & TT_EXTENDFLAG_IGNORE) == 0) {
            // non ignore shots left
            clevel = 1;
            // dead end station
            if (a1->start_node->narrows == 1)
              clevel = 2;
            // entrance station left
            if ((this->station_vec[(a1->is_reversed ? (a1->leg->leg->to.id) : (a1->leg->leg->from.id)) - 1].flags & TT_STATIONFLAG_ENTRANCE) != 0)
              clevel = 3;
            // start station left
            if ((this->station_vec[a1->start_node->uid - 1].extend & TT_EXTENDFLAG_START) != 0)
              clevel = 4;
            // start shot left
            if ((a1->extend & TT_EXTENDFLAG_START) != 0)
              clevel = 5;
            if (a1->start_node->xx_touched)
              clevel = 6;
          }
            
          if (clevel > start_level) {
            start_level = clevel;
            a2 = a1;
          } else if ((clevel == start_level) && ((a2 == NULL) || (this->station_vec[a1->start_node->uid - 1].z > this->station_vec[a2->start_node->uid - 1].z))) {
            a2 = a1;
          }
          
        }
        a1++;
      }
      if (start_level == 0)
        ignorant_mode = true;
      
      // set up current_node & current_node->last_arrow
      if (a2 == NULL) {
        thwarning(("not all shots attached to extended elevation"));
        break;
      }
      current_node = a2->start_node;
      current_node->last_arrow = a2;
      current_node->is_attached = true;  
      current_node->back_arrow = NULL;
      cxx = current_node->xx;
      component_break = false;
      if (!current_node->xx_touched) {
        switch (this->station_vec[current_node->uid - 1].extend & TT_EXTENDFLAG_DIRECTION) {
          case TT_EXTENDFLAG_LEFT:
          case TT_EXTENDFLAG_REVERSE:
            current_node->xx_left = -1;
            break;
          case TT_EXTENDFLAG_VERTICAL:
            current_node->xx_left = 0;
            break;
        }
        if (!thisnan(this->station_vec[current_node->uid - 1].extend_ratio))
          current_node->xx_ratio = this->station_vec[current_node->uid - 1].extend_ratio;
      }
      default_left = current_node->xx_left;
      default_ratio = current_node->xx_ratio;
      just_started = true;

#ifdef THDEBUG
      thprintf("START -- %d (%s@%s)\n", current_node->id,
        this->station_vec[current_node->id - 1].name,
        this->station_vec[current_node->id - 1].survey->get_full_name());
#endif
      if (thcfg.log_extend) {
        thprintf("START %s@%s\n",
        this->station_vec[current_node->id - 1].name,
        this->station_vec[current_node->id - 1].survey->get_full_name());
        prev_id = current_node->id;
      }
      
    }
    
    if (just_started) {
      just_started = false;
    } else {

      // let's make move
      bool try_first, before_cnd;
      try_first = true;
      std::list<unsigned long>::iterator before_it;

      if (current_node->last_arrow == NULL) {
        try_first = false;
        current_node->last_arrow = current_node->first_arrow;
      }

      // find arrow that is not discovery and not ignored (if not ignorant mode)
      while (current_node->last_arrow != NULL) {

    	before_cnd = true;
    	if (current_node->back_arrow) {
    		before_it = std::find(current_node->last_arrow->extend_ignore_before.begin(), current_node->last_arrow->extend_ignore_before.end(), current_node->back_arrow->end_node->uid);
    		if (before_it != current_node->last_arrow->extend_ignore_before.end())
    			before_cnd = false;
    	}

        if ((!current_node->last_arrow->is_discovery) &&
           (ignorant_mode || (
        		   ((current_node->last_arrow->extend & TT_EXTENDFLAG_IGNORE) == 0) &&
				   ((current_node->last_arrow->extend & TT_EXTENDFLAG_BREAK) == 0) &&
				   (((current_node->last_arrow->extend & TT_EXTENDFLAG_CNDIGNORE) == 0) || before_cnd)
           	   )))
           break;

        current_node->last_arrow = current_node->last_arrow->next_arrow;
        if (try_first && (current_node->last_arrow == NULL)) {
          try_first = false;
          current_node->last_arrow = current_node->first_arrow;
        }
      }

    }
  
    if (current_node->last_arrow == NULL) {

      // go back
      if (current_node->back_arrow == NULL)
        component_break = true;
      else {
        current_node = current_node->pop_back_arrow()->end_node;
        cxx = current_node->xx;
        default_left = current_node->xx_left;
        default_ratio = current_node->xx_ratio;
#ifdef THDEBUG
        thprintf("%d (%s@%s) <-\n", current_node->id,
          this->station_vec[current_node->id - 1].name,
          this->station_vec[current_node->id - 1].survey->get_full_name());
#endif
        if (thcfg.log_extend) {
          if (prev_id != current_node->id) {
            thprintf("BACK %s@%s\n",
              this->station_vec[current_node->id - 1].name,
              this->station_vec[current_node->id - 1].survey->get_full_name());
            prev_id = current_node->id;
          }
        }        
      }

    } else {
    
      // go forward
      current_node->last_arrow->is_discovery = true;          
      current_node->last_arrow->negative->is_discovery = true;          
      tarrows++;
      
      // set start x
      if (current_node->last_arrow->is_reversed)
        current_node->last_arrow->leg->leg->txx = cxx;
      else
        current_node->last_arrow->leg->leg->fxx = cxx;
      
      // change cxx
      switch (this->station_vec[current_node->last_arrow->end_node->uid - 1].extend & TT_EXTENDFLAG_DIRECTION) {
        case TT_EXTENDFLAG_REVERSE:
          default_left = -default_left;
          break;
        case TT_EXTENDFLAG_LEFT:
          default_left = -1;
          break;
        case TT_EXTENDFLAG_RIGHT:
          default_left = 1;
          break;
        case TT_EXTENDFLAG_VERTICAL:
          default_left = 0;
          break;
      }
      if (!thisnan(this->station_vec[current_node->last_arrow->end_node->uid - 1].extend_ratio))
        default_ratio = this->station_vec[current_node->last_arrow->end_node->uid - 1].extend_ratio;

      current_node->last_arrow->end_node->xx_left = default_left;
      current_node->last_arrow->end_node->xx_ratio = default_ratio;
      go_left = default_left;
      go_ratio = default_ratio;
      
      if ((current_node->last_arrow->extend & 
          (TT_EXTENDFLAG_LEFT | TT_EXTENDFLAG_RIGHT
           | TT_EXTENDFLAG_REVERSE | TT_EXTENDFLAG_VERTICAL)) == 0) {
        switch (current_node->last_arrow->leg->leg->extend & TT_EXTENDFLAG_DIRECTION) {
          case TT_EXTENDFLAG_LEFT:
            go_left = -1;
            if (current_node->last_arrow->is_reversed)
              go_left = -go_left;
            break;
          case TT_EXTENDFLAG_RIGHT:
            go_left = 1;
            if (current_node->last_arrow->is_reversed)
              go_left = -go_left;
            break;
          case TT_EXTENDFLAG_REVERSE:
            go_left = -go_left;
            break;
          case TT_EXTENDFLAG_VERTICAL:
            go_left = 0;
            break;
        }
      } else {
        switch (current_node->last_arrow->extend & TT_EXTENDFLAG_DIRECTION) {
          case TT_EXTENDFLAG_REVERSE:
            go_left = -go_left;
            break;
          case TT_EXTENDFLAG_LEFT:
            go_left = -1;
            break;
          case TT_EXTENDFLAG_RIGHT:
            go_left = 1;
            break;
          case TT_EXTENDFLAG_VERTICAL:
            go_left = 0;
            break;
        }
      }
      if (!thisnan(current_node->last_arrow->leg->leg->extend_ratio))
        go_ratio = current_node->last_arrow->leg->leg->extend_ratio;
      else
        current_node->last_arrow->leg->leg->extend_ratio = go_ratio;
 
      if ((current_node->last_arrow->leg->leg->flags & TT_LEGFLAG_SPLAY) > 0) {
    	  cxx += 0;
      } else {
    	  cxx += double(go_left) * std::hypot(current_node->last_arrow->leg->leg->total_dx, current_node->last_arrow->leg->leg->total_dy) * current_node->last_arrow->leg->leg->extend_ratio;
      }
      
      // set end x
      if (current_node->last_arrow->is_reversed)
        current_node->last_arrow->leg->leg->fxx = cxx;
      else
        current_node->last_arrow->leg->leg->txx = cxx;

#ifdef THDEBUG
      thprintf("-> %d (%s@%s)\n", current_node->last_arrow->end_node->id,
        this->station_vec[current_node->last_arrow->end_node->id - 1].name,
        this->station_vec[current_node->last_arrow->end_node->id - 1].survey->get_full_name());
#endif
      if (thcfg.log_extend) {
        if ((prev_id != current_node->last_arrow->end_node->id) && !this->station_vec[current_node->last_arrow->end_node->id - 1].is_temporary()) {
          thprintf("%s %s@%s",
            (go_left == -1 ? "LEFT" : (go_left == 1 ? "RIGHT" : "VERTICAL")),
            this->station_vec[current_node->last_arrow->end_node->id - 1].name,
            this->station_vec[current_node->last_arrow->end_node->id - 1].survey->get_full_name());
          prev_id = current_node->last_arrow->end_node->id;
          if (current_node->last_arrow->leg->leg->extend_ratio != 1.0)
            thprintf(" (%.0f%%)", current_node->last_arrow->leg->leg->extend_ratio * 100.0);
          thprintf("\n");
        }
      }

      //if (!current_node->last_arrow->end_node->is_attached) {
      if (true) {
        //current_node->last_arrow->end_node->back_arrow =
        //  current_node->last_arrow->negative;
        current_node->last_arrow->end_node->push_back_arrow(current_node->last_arrow->negative);
        current_node = current_node->last_arrow->end_node;
        current_node->xx = cxx;
        if (!current_node->extendx_ok) {
          current_node->extendx = cxx;
          current_node->extendx_ok = true;
        }
        current_node->is_attached = true;
        current_node->xx_touched = true;
      } else {
        cxx = current_node->xx;
      }
      
    }
  
  } // END OF TREMAUX
  
  // postprocess splay shots
  double splay_dir, shot_dir, shot_dx, minshot_dir, minshot_x, minshot_rx, minshot_dx;
  thdb1d_tree_node * src_node;
  for(i = 0; i < tn_stations; i++) {
	  current_node = nodes + i;
	  if (this->station_vec[i].is_temporary()) {
	    minshot_dir = thnan;
	    minshot_x = current_node->first_arrow->leg->leg->fxx;
	    minshot_rx = current_node->first_arrow->leg->leg->extend_ratio;
	    minshot_dx = 1.0;
	    src_node = current_node->first_arrow->end_node;
	    splay_dir = atan2(this->station_vec[i].y - this->station_vec[src_node->id - 1].y, this->station_vec[i].x - this->station_vec[src_node->id - 1].x);
	    carrow = src_node->first_arrow;
	    while (carrow != NULL) {
	      if ((carrow->leg->leg->flags & TT_LEGFLAG_SPLAY) == 0) { // not a splay shot
          shot_dx = carrow->leg->leg->txx - carrow->leg->leg->fxx;
	        shot_dir = atan2(this->station_vec[carrow->end_node->id - 1].y - this->station_vec[src_node->id - 1].y, this->station_vec[carrow->end_node->id - 1].x - this->station_vec[src_node->id - 1].x);
	        if ((minshot_dx > 0) &&(thisnan(minshot_dir) || (diffdir(shot_dir, splay_dir) < diffdir(minshot_dir, splay_dir)))) {
	          minshot_dir = shot_dir;
	          minshot_rx = carrow->leg->leg->extend_ratio;
	          if (shot_dx > 0)
	            minshot_dx = 1.0;
	          else
	            minshot_dx = -1.0;
	          if (carrow->is_reversed) {
	            minshot_x = carrow->leg->leg->txx;
	            minshot_dx *= -1.0;
	          } else {
              minshot_x = carrow->leg->leg->fxx;
	          }
	        }
	      }
	      carrow = carrow->next_arrow;
	    }
	    if (thisnan(minshot_dir)) minshot_dir = 0.0;
        if (current_node->first_arrow->is_reversed) {
          current_node->first_arrow->leg->leg->fxx = minshot_x;
          current_node->first_arrow->leg->leg->txx = minshot_x + minshot_dx * cos(diffdir(minshot_dir, splay_dir)) * std::hypot(current_node->first_arrow->leg->leg->total_dx, current_node->first_arrow->leg->leg->total_dy) * minshot_rx;
        } else {
          current_node->first_arrow->leg->leg->txx = minshot_x;
          current_node->first_arrow->leg->leg->fxx = minshot_x + minshot_dx * cos(diffdir(minshot_dir, splay_dir)) * std::hypot(current_node->first_arrow->leg->leg->total_dx, current_node->first_arrow->leg->leg->total_dy) * minshot_rx;
        }
	  }
  }

#ifdef THDEBUG
    thprintf("\nend of extended elevation\n");
#else
    thprintf("done\n");
    thtext_inline = false;
#endif
  
}


thdb3ddata * thdb1ds::get_3d_outline() {
  if (this->d3_parsed)
    return &(this->d3_outline);
  this->d3_parsed = true;
	using namespace quickhull;
	QuickHull<double> qh; // Could be double as well
	std::vector<Vector3<double>> pointCloud, originalPointCloud;
	std::vector<thdb3dvx*> originalPointCloudUse;
  thdb1d_tree_node * n;
  thdb1d_tree_arrow * a;
  thdb1ds * tt;
  n = &(thdb.db1d.tree_nodes[this->uid - 1]);
  Vector3<double> fv(this->x, this->y, this->z), tv = {}, txv = {};

  // TODO: Add points to point cloud
	// traverse all splay shots from given station, calculate normalized position and add
  size_t splaycnt = 0, undercnt = 0;
  for(a = n->first_arrow; a != NULL; a = a->next_arrow) {
	if ((a->leg->leg->flags & TT_LEGFLAG_SURFACE) != 0) continue;
	if (!(a->leg->leg->splay_walls)) continue;
	tt = &(thdb.db1d.station_vec[a->end_node->uid - 1]);
	//if (tt->temps == TT_TEMPSTATION_FEATURE) continue;
	tv = Vector3<double>(tt->x, tt->y, tt->z);
	txv = tv - fv;
	try {
		txv.normalize();
		pointCloud.push_back(txv);
		originalPointCloud.push_back(tv);
		originalPointCloudUse.push_back(NULL);
		if ((a->leg->leg->flags & TT_LEGFLAG_SPLAY) != 0) splaycnt++;
		else undercnt++;
	} catch (...) {}
  }
  // if there are more then 1 underground shots from this station, add it
  if (undercnt > 0) {
		pointCloud.push_back(Vector3<double>(0.0, 0.0, 0.0));
		originalPointCloud.push_back(fv);
		originalPointCloudUse.push_back(NULL);
  }

	if (splaycnt > 0) {
		auto hull = qh.getConvexHull(pointCloud, true, true);
		auto indexBuffer = hull.getIndexBuffer();
		thdb3dvx * cvx;
		thdb3dfc * cfc;
		// Read vertices & triangles to d3_outline
		if (indexBuffer.size() > 2) {
			for(size_t i = 0; i < indexBuffer.size(); i++) {
				if (originalPointCloudUse[indexBuffer[i]] == 0) {
					Vector3<double> origpos = originalPointCloud[indexBuffer[i]];
					cvx = this->d3_outline.insert_vertex(origpos.x, origpos.y, origpos.z);
					originalPointCloudUse[indexBuffer[i]] = cvx;
				}
			}
			cfc = this->d3_outline.insert_face(THDB3DFC_TRIANGLES);
			for(size_t i = 0; i < indexBuffer.size(); i++) {
				cfc->insert_vertex(originalPointCloudUse[indexBuffer[i]]);
			}
		}
	}

	return &(this->d3_outline);

}

void thdb1d_tree_node::push_back_arrow(thdb1d_tree_arrow * arrow) {
	if (this->back_arrow == NULL) {
		this->back_arrow = arrow;
	} else {
		this->back_buffer.push_back(arrow);
		this->back_arrow = arrow;
	}
}

thdb1d_tree_arrow * thdb1d_tree_node::pop_back_arrow() {
	thdb1d_tree_arrow * rv = this->back_arrow;
	if (!this->back_buffer.empty()) {
		this->back_arrow = this->back_buffer.back();
		this->back_buffer.pop_back();
	} else {
		this->back_arrow = NULL;
	}
	return rv;
}

