/**
 * @file thdb1d.cxx
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
#include "thlogfile.h"
#include "thsurface.h"

//#define THUSESVX

thdb1d::thdb1d()
{
  this->db = NULL;
  this->tree_legs = NULL;
  this->num_tree_legs = 0;
  this->lsid = 0;
  this->nlength = 0.0;
  
  this->tree_arrows = NULL;
  this->tree_nodes = NULL;
  
  this->d3_data_parsed = false;
}


thdb1d::~thdb1d()
{
  if (this->tree_legs != NULL)
    delete [] this->tree_legs;
  if (this->tree_arrows != NULL)
    delete [] this->tree_arrows;
  if (this->tree_nodes != NULL)
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
  thdatass_list::iterator ssi;
  thdataequate_list::iterator eqi;
  thdata * dp;
  double dcc, sindecl, cosdecl, tmpx, tmpy;
  thdb1ds * tsp1, * tsp2;  // Temporary stations.
  while (obi != this->db->object_list.end()) {
  
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      
      dp = (thdata *)(*obi);
      
      // scan data shots
      lei = dp->leg_list.begin();
      try {
        while(lei != dp->leg_list.end()) {
          if (lei->is_valid) {
            lei->from.id = this->insert_station(lei->from, lei->psurvey, dp, 3);
            lei->to.id = this->insert_station(lei->to, lei->psurvey, dp, 3);
            
            this->leg_vec.push_back(thdb1dl(&(*lei),dp,lei->psurvey));
            
            // check station marks
            tsp1 = &(this->station_vec[lei->from.id - 1]);
            tsp2 = &(this->station_vec[lei->to.id - 1]);
            if (lei->s_mark > tsp1->mark)
              tsp1->mark = lei->s_mark;
            if (lei->s_mark > tsp2->mark)
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
              if (fabs(dcc) > lei->length)
                ththrow(("length reading is less than change in depth"))
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
                  lei->bearing += lei->backbearing;
                  lei->bearing = lei->bearing / 2.0;
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
                    lei->gradient += lei->backgradient;
                    lei->gradient = lei->gradient / 2.0;
                  }
                  else {
                    if (thisinf(lei->gradient) != -thisinf(lei->backgradient))
                      ththrow(("invalid plumbed shot"))
                  }
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
                break;
                
              case TT_DATATYPE_CARTESIAN:
                lei->total_dx = (lei->direction ? 1.0 : -1.0) * lei->dx;
                lei->total_dy = (lei->direction ? 1.0 : -1.0) * lei->dy;
                lei->total_dz = (lei->direction ? 1.0 : -1.0) * lei->dz;
                lei->total_length = thdxyz2length(lei->total_dx,lei->total_dy,lei->total_dz);
                lei->total_bearing = thdxyz2bearing(lei->total_dx,lei->total_dy,lei->total_dz);
                lei->total_gradient = thdxyz2clino(lei->total_dx,lei->total_dy,lei->total_dz);
                break;
            }

            if (!thisnan(lei->declination)) {
              lei->total_bearing += lei->declination;
              if (lei->total_bearing >= 360.0)
                lei->total_bearing -= 360.0;
              if (lei->total_bearing < 0.0)
                lei->total_bearing += 360.0;
              cosdecl = cos(lei->declination/180*THPI);
              sindecl = sin(lei->declination/180*THPI);
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
      catch (...)
        threthrow(("%s [%d]", lei->srcf.name, lei->srcf.line));
          
      // scan data fixes
      fii = dp->fix_list.begin();
      try {
        while(fii != dp->fix_list.end()) {
          fii->station.id = this->insert_station(fii->station, fii->psurvey, dp, 2);
          this->station_vec[fii->station.id - 1].flags |= TT_STATIONFLAG_FIXED;
          fii++;
        }
      }
      catch (...)
        threthrow(("%s [%d]", fii->srcf.name, fii->srcf.line));
  
      // scan data equates
      eqi = dp->equate_list.begin();
      try {
        while(eqi != dp->equate_list.end()) {
          eqi->station.id = this->insert_station(eqi->station, eqi->psurvey, dp, 1);
          eqi++;
        }
      }
      catch (...)
        threthrow(("%s [%d]", eqi->srcf.name, eqi->srcf.line));
    }
  
    obi++;
  }

  // scan data stations
  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = (thdata *)(*obi);
      ssi = dp->ss_list.begin();
      try {
        while(ssi != dp->ss_list.end()) {
          ssi->station.id = this->get_station_id(ssi->station, ssi->psurvey);
          if (ssi->station.id == 0) {
            if (ssi->station.survey == NULL)
              ththrow(("station doesn't exist -- %s", ssi->station.name))
            else
              ththrow(("station doesn't exist -- %s@%s", ssi->station.name, ssi->station.survey))
          }
          // set station flags and comment
          else {
            if (ssi->comment != NULL)
              this->station_vec[ssi->station.id-1].comment = ssi->comment;
            this->station_vec[ssi->station.id-1].flags |= ssi->flags;
          }
          ssi++;
        }
      }
      catch (...)
        threthrow(("%s [%d]", ssi->srcf.name, ssi->srcf.line));
    }  
    obi++;
  }
}


void thdb1d::process_data()
{
  this->scan_data();
#ifndef THUSESVX
  this->find_loops();
  this->close_loops();
#else  
  thsvxctrl survex;
  survex.process_survey_data(this->db);
#endif
  this->process_survey_stat();
  this->postprocess_objects();
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

void thdb1ds::set_parent_data(class thdata * pd, unsigned pd_priority, unsigned pd_slength) {
  if ((pd_slength > this->data_slength)) 
    this->data = pd;
  else if ((pd_slength == this->data_slength) && (pd_priority >= this->data_priority))
    this->data = pd;
}


unsigned long thdb1d::insert_station(class thobjectname on, class thsurvey * ps, class thdata * pd, unsigned pd_priority)
{
  // first insert object into database
  unsigned pd_slength = strlen(ps->full_name);
  ps = this->db->get_survey(on.survey, ps);
  on.survey = NULL;
  unsigned long csurvey_id = (ps == NULL) ? 0 : ps->id;
  
  thdb1d_station_map_type::iterator sti;
  sti = this->station_map.find(thobjectid(on.name, csurvey_id)); 
  if (sti != this->station_map.end()) {
    this->station_vec[sti->second - 1].set_parent_data(pd,pd_priority,pd_slength);
    return sti->second;
  }
  
  if (!(this->db->insert_datastation(on, ps))) {
    if (on.survey != NULL)
      ththrow(("object already exist -- %s@%s", on.name, on.survey))
    else
      ththrow(("object already exist -- %s", on.name))
  }
  
  this->station_map[thobjectid(on.name, csurvey_id)] = ++this->lsid;
  this->station_vec.push_back(thdb1ds(on.name, ps));
  this->station_vec[this->lsid - 1].set_parent_data(pd,pd_priority,pd_slength);
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
    ththrow(("equate of two fixed stations -- %s@%s and %s@%s",
          thisdb->station_vec[n1->id - 1].name,
          thisdb->station_vec[n1->id - 1].survey->get_full_name(),
          thisdb->station_vec[n2->id - 1].name,
          thisdb->station_vec[n2->id - 1].survey->get_full_name()));
  }
    
  if (n1->id == n2->id) {
    ththrow(("equate of two identical stations -- %s@%s and %s@%s",
          thisdb->station_vec[n1->id - 1].name,
          thisdb->station_vec[n1->id - 1].survey->get_full_name(),
          thisdb->station_vec[n2->id - 1].name,
          thisdb->station_vec[n2->id - 1].survey->get_full_name()));
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

  unsigned long tn_legs = this->leg_vec.size();
  unsigned long tn_stations = this->station_vec.size();
  
  
  if ((tn_legs < 0) || (tn_stations < 0))
    return;
  
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
      dp = (thdata *)(*obi);
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
    
    a2->negative = a1;
    a2->leg = a1->leg;
    a2->start_node = a1->end_node;
    a2->end_node = a1->start_node;
    a2->is_reversed = true;
    
    // assign nodes
    a1->next_arrow = a1->start_node->first_arrow;
    a1->start_node->first_arrow = a1;
    a1->start_node->narrows++;
    a2->next_arrow = a2->start_node->first_arrow;
    a2->start_node->first_arrow = a2;
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
        ththrow(("a software BUG is present (" __FILE__ ":%d)", __LINE__));
//#ifdef THDEBUG
//        thprintf("warning -- not all stations connected to the network\n");
//#endif
        break;
      }

      if ((!n2->is_fixed) && (any_fixed || (component > 0))) {
        ththrow(("can not connect %s@%s to centerline network",
          this->station_vec[n3->id - 1].name,
          this->station_vec[n3->id - 1].survey->get_full_name()));
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
      
    while ((current_node->last_arrow != NULL) && 
        (current_node->last_arrow->is_discovery))
        current_node->last_arrow = current_node->last_arrow->next_arrow;
    
    if (current_node->last_arrow == NULL) {

      // go back
      if (current_node->back_arrow == NULL)
        component_break = true;
      else {
        current_node = current_node->back_arrow->end_node;
//#ifdef THDEBUG
//        thprintf("%d (%s@%s) <-\n", current_node->id,
//          this->station_vec[current_node->id - 1].name,
//          this->station_vec[current_node->id - 1].survey->get_full_name());
//#endif
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



void thdb1d__scan_survey_station_limits(thsurvey * ss, thdb1ds * st, bool is_under) {
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
    ss->stat.station_state = 2;
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


void thdb1d__scan_data_station_limits(thdata * ss, thdb1ds * st, bool is_under) {
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
    ss->stat_st_state = 2;
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
    if ((lit->leg->flags & TT_LEGFLAG_DUPLICATE) != 0)
      lit->data->stat_dlength += lit->leg->total_length;
    // ak nie skusi ci je surface
    else if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0)
      lit->data->stat_slength += lit->leg->total_length;
    // inak prida do length
    else {
      lit->data->stat_length += lit->leg->total_length;
      this->nlength += lit->leg->total_length;
    }
    // stations
    if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0) {
      thdb1d__scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->from.id - 1]), false);
      thdb1d__scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->to.id - 1]), false);
    } else {
      thdb1d__scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->from.id - 1]), true);
      thdb1d__scan_data_station_limits(lit->data, &(this->station_vec[lit->leg->to.id - 1]), true);
    }


    ss = lit->survey;
    while (ss != NULL) {
      // skusi ci je duplikovane
      if ((lit->leg->flags & TT_LEGFLAG_DUPLICATE) != 0)
        ss->stat.length_duplicate += lit->leg->total_length;
      // ak nie skusi ci je surface
      else if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0)
        ss->stat.length_surface += lit->leg->total_length;
      // inak prida do length
      else
        ss->stat.length += lit->leg->total_length;
      if ((lit->leg->flags & TT_LEGFLAG_SURFACE) != 0) {
        thdb1d__scan_survey_station_limits(ss, &(this->station_vec[lit->leg->from.id - 1]), false);
        thdb1d__scan_survey_station_limits(ss, &(this->station_vec[lit->leg->to.id - 1]), false);
      } else {
        thdb1d__scan_survey_station_limits(ss, &(this->station_vec[lit->leg->from.id - 1]), true);
        thdb1d__scan_survey_station_limits(ss, &(this->station_vec[lit->leg->to.id - 1]), true);
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
      ss->stat.num_stations++;
      ss = ss->fsptr;
    }    
    sit++;
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
  class thlc_cross_arrow * first_arrow, * last_arrow, * dm_arrow;
  
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
  thlc_cross * target;
  thlc_cross_arrow * prev_arrow, * next_arrow;
  thlc_series * series;
  bool reverse;
};


struct thlc_loop
{
  thlc_cross * from_cross, * to_cross;
  thlc_cross_arrow * first_arrow, * last_arrow;
  unsigned long minid, size, id;
#ifdef THDEBUG
  bool is_new;
  long old_id;
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

  unsigned long nlegs = this->get_tree_size(),
    nstations = this->station_vec.size(), numseries,
    i, lastcross, nseries, nloops;
    
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
  thlc_series tmpseries, * curseries, * prevseries;
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

  if (nlegs == 0)
    goto LC_COORD_CALC;

  while (i < nlegs) {
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
      ththrow(("shot between two equal stations -- %s@%s and %s@%s",
          this->station_vec[clcleg->from_id - 1].name,
          this->station_vec[clcleg->from_id - 1].survey->get_full_name(),
          this->station_vec[clcleg->to_id - 1].name,
          this->station_vec[clcleg->to_id - 1].survey->get_full_name()));
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
  
  if (crossst != NULL)
    delete [] crossst;
  if (lclegs != NULL)
    delete [] lclegs;
  if (lccrosses != NULL)
    delete [] lccrosses;
  if (lccrossarrows != NULL)
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
    nstations = this->station_vec.size(), i;    

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
  double avg_error = 0.0, avg_error_sum = 0.0;
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      dp = (thdata *)(*obi);
      fii = dp->fix_list.begin();
      while(fii != dp->fix_list.end()) {
        ps = &(this->station_vec[this->station_vec[fii->station.id - 1].uid - 1]);
        ps->placed = true;
        anyfixed = true;
        ps->x = fii->x;
        ps->y = fii->y;
        ps->z = fii->z;
        if (!thisnan(fii->sdx)) {
          ps->sdx = fii->sdx;
          ps->sdy = fii->sdy;
          ps->sdz = fii->sdz;
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
          sum_sdx += ll->leg->total_sdx;
          sum_sdy += ll->leg->total_sdy;
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
          avg_error += 100.0 * sumlegs * (li->err_length / sum_length);
          avg_error_sum += sumlegs;
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
            li->from->adjusted = true;
          }
        }
        
        ll = li->first_leg;
        while (ll != NULL) {
          if (!ll->leg->adjusted) {

#define ORIENT (ll->reverse ? -1.0 : 1.0)
            if (sum_sdx > 0.0)
              ll->leg->adj_dx += ORIENT * err_dx * ll->leg->total_sdx / sum_sdx;
            else
              ll->leg->adj_dx += ORIENT * err_dx / sumlegs;
            if (sum_sdy > 0.0)
              ll->leg->adj_dy += ORIENT * err_dy * ll->leg->total_sdy / sum_sdy;
            else
              ll->leg->adj_dy += ORIENT * err_dy / sumlegs;
            if (sum_sdz > 0.0)
              ll->leg->adj_dz += ORIENT * err_dz * ll->leg->total_sdz / sum_sdz;
            else
              ll->leg->adj_dz += ORIENT * err_dz / sumlegs;
            ll->leg->adjusted = true;
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
  for (i = 0; i < nlegs; i++) {
  
    cleg = legs[i];
    if (cleg->reverse) {
      froms = &(this->station_vec[this->station_vec[cleg->leg->to.id - 1].uid - 1]);
      tos = &(this->station_vec[this->station_vec[cleg->leg->from.id - 1].uid - 1]);
    } else {
      froms = &(this->station_vec[this->station_vec[cleg->leg->from.id - 1].uid - 1]);
      tos = &(this->station_vec[this->station_vec[cleg->leg->to.id - 1].uid - 1]);
    }
    if ((i == 0) && (!anyfixed)) {
      froms->placed = true;
      froms->x = 0.0;
      froms->y = 0.0;
      froms->z = 0.0;
    }
    
    if (!froms->placed)
      ththrow(("a software BUG is present (" __FILE__ ":%d)", __LINE__));
    if (!tos->placed) {
      if (cleg->reverse) {
        tos->x = froms->x - cleg->leg->adj_dx;
        tos->y = froms->y - cleg->leg->adj_dy;
        tos->z = froms->z - cleg->leg->adj_dz;
      } else {
        tos->x = froms->x + cleg->leg->adj_dx;
        tos->y = froms->y + cleg->leg->adj_dy;
        tos->z = froms->z + cleg->leg->adj_dz;
      }
      tos->placed = true;
    }
    
  }
  
  // * potom priradi suradnice nie UID bodom
  for(i = 0; i < nstations; i++) {
    ps = &(this->station_vec[i]);
    froms = &(this->station_vec[ps->uid - 1]);
    if (!ps->placed) {
      ps->placed = froms->placed;
      ps->x = froms->x;
      ps->y = froms->y;
      ps->z = froms->z;
      if (!ps->placed) {
//        ththrow(("a software BUG is present (" __FILE__ ":%d)", __LINE__));
        ththrow(("can not connect %s@%s to centerline network",
          this->station_vec[i].name,
          this->station_vec[i].survey->get_full_name()));
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

int comp_lpr(const void * l1, const void * l2) {
  if (((thdb1d_lpr*)l1)->err > ((thdb1d_lpr*)l2)->err)
    return -1;
  else if (((thdb1d_lpr*)l1)->err < ((thdb1d_lpr*)l2)->err)
    return 1;
  else return 0;
}



void thdb1d::print_loops() {
  thdb1d_loop_list_type::iterator lii = this->loop_list.begin();
  if (lii == this->loop_list.end())
    return;
  unsigned long i, nloops;
  thdb1d_loop * li;
  nloops = this->loop_list.size();
  thdb1d_lpr * lpr = new thdb1d_lpr [nloops];
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
  
  qsort(lpr,nloops,sizeof(thdb1d_lpr),comp_lpr);
  
  thdb1d_loop_leg * ll;
  thsurvey * ss;   
  thdb1ds * ps, * prev_ps, * first_ps;
  unsigned long psid, prev_psid, first_psid;
  
  thlog.printf("\n\n######################### loop errors ##########################\n");
  thlog.printf(    "REL-ERR ABS-ERR TOTAL-L STS X-ERROR Y-ERROR Z-ERROR STATIONS\n");
  for (i = 0; i < nloops; i++) {
    li = lpr[i].li;
    thlog.printf("%6.2f%% %6.1fm %6.1fm %3ld %6.1fm %6.1fm %6.1fm [",
      li->src_length > 0.0 ? 100.0 * li->err_length / li->src_length : 0.0,
      li->err_length, li->src_length, li->nlegs, li->err_dx, li->err_dy, li->err_dz);
    ll = li->first_leg;
    ss = NULL;
    if (ll->reverse)
      psid = ll->leg->to.id;
    else
      psid = ll->leg->from.id;
    ps = &(this->station_vec[psid - 1]);
    first_ps = ps;
    first_psid = psid;
    thlog.printf("%s", ps->name);
    if ((ss == NULL) || (ss->id != ps->survey->id)) {
      ss = ps->survey;
      thlog.printf("@%s", ss->get_full_name());
    }
    prev_ps = ps;
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

      prev_ps = ps;
      prev_psid = psid;
      ll = ll->next_leg;
    }
    thlog.printf("]\n");
  }
  thlog.printf("##################### end of loop errors #######################\n");
  delete [] lpr;
}

thdb3ddata * thdb1d::get_3d_surface() {
  this->get_3d();
  return &(this->d3_surface);
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
          this->station_vec[id].z, \
          (void *) &(this->station_vec[id])); \
      } \
    }
  
  // polygony tam vlozi ako linestripy (data poojdu na thdb1dl)
  last_st = nstat;
  thdb3dfc * fc = NULL;
  thdb1dl ** tlegs = this->get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected() && (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) == 0)) {
      cur_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      get_3d_check_station(cur_st);
      if (cur_st != last_st) {
        fc = this->d3_data.insert_face(THDB3DFC_LINE_STRIP);
        fc->insert_vertex(station_in[cur_st], (void *) *tlegs);
      }
      last_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      get_3d_check_station(last_st);
      fc->insert_vertex(station_in[last_st], (void *) *tlegs);
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
          this->station_vec[id].z, \
          (void *) &(this->station_vec[id])); \
      } \
    }
  
  // polygony tam vlozi ako linestripy (data poojdu na thdb1dl)
  last_st = nstat;
  fc = NULL;
  tlegs = this->get_tree_legs();  
  for(i = 0; i < nlegs; i++, tlegs++) {
    if ((*tlegs)->survey->is_selected() && (((*tlegs)->leg->flags & TT_LEGFLAG_SURFACE) != 0)) {
      cur_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->to.id : (*tlegs)->leg->from.id) - 1].uid - 1;
      get_3d_check_station(cur_st);
      if (cur_st != last_st) {
        fc = this->d3_surface.insert_face(THDB3DFC_LINE_STRIP);
        fc->insert_vertex(station_in[cur_st], (void *) *tlegs);
      }
      last_st = this->station_vec[((*tlegs)->reverse ? (*tlegs)->leg->from.id : (*tlegs)->leg->to.id) - 1].uid - 1;
      get_3d_check_station(last_st);
      fc->insert_vertex(station_in[last_st], (void *) *tlegs);
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
        ((thsurface*)(*obi))->check_stations();
        break;
    }
    obi++;
  }
}







