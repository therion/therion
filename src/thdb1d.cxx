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
#include <math.h>

#define thnanpow2(cislo) ((thisnan(cislo) ? 0.0 : cislo) * (thisnan(cislo) ? 0.0 : cislo))

thdb1d::thdb1d()
{
  this->db = NULL;
  this->tree_legs = NULL;
  this->num_tree_legs = 0;
  this->lsid = 0;
  
  this->tree_arrows = NULL;
  this->tree_nodes = NULL;
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
  double dcc;
  thdb1ds * tsp1, * tsp2;  // Temporary stations.
  while (obi != this->db->object_list.end()) {
  
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      
      dp = (thdata *)(*obi);
      
      // scan data shots
      lei = dp->leg_list.begin();
      try {
        while(lei != dp->leg_list.end()) {
          if (lei->is_valid) {
            lei->from.id = this->insert_station(lei->from, lei->psurvey);
            lei->to.id = this->insert_station(lei->to, lei->psurvey);
            
            this->leg_vec.push_back(thdb1dl(&(*lei),dp,lei->psurvey));
            
            // check station marks
            tsp1 = &(this->station_vec[lei->from.id - 1]);
            tsp2 = &(this->station_vec[lei->to.id - 1]);
            if (lei->s_mark > tsp1->mark)
              tsp1->mark = lei->s_mark;
            if (lei->s_mark > tsp2->mark)
              tsp2->mark = lei->s_mark;
            

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
            if ((lei->data_type == TT_DATATYPE_DIVING) ||
                (lei->data_type == TT_DATATYPE_CYLPOLAR)) {
              if (!thisnan(lei->depthchange))
                dcc = lei->depthchange;
              else
                dcc = lei->todepth - lei->fromdepth;
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
            
            // calculate leg total length
            switch (lei->data_type) {
              case TT_DATATYPE_NORMAL:
              case TT_DATATYPE_DIVING:
                lei->total_length = lei->length;
                break;
              case TT_DATATYPE_CYLPOLAR:
                lei->total_length = sqrt(thnanpow2(lei->length) + thnanpow2(lei->depthchange));
                break;
              case TT_DATATYPE_CARTESIAN:
                lei->total_length = sqrt(thnanpow2(lei->dx) + thnanpow2(lei->dy) + thnanpow2(lei->dz));
                break;
            }
            
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
          fii->station.id = this->insert_station(fii->station, fii->psurvey);
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
          eqi->station.id = this->insert_station(eqi->station, eqi->psurvey);
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
  thsvxctrl survex;
  this->scan_data();
  survex.process_survey_data(this->db);
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


unsigned long thdb1d::insert_station(class thobjectname on, class thsurvey * ps)
{
  // first insert object into database
  ps = this->db->get_survey(on.survey, ps);
  on.survey = NULL;
  unsigned long csurvey_id = (ps == NULL) ? 0 : ps->id;
  
  thdb1d_station_map_type::iterator sti;
  sti = this->station_map.find(thobjectid(on.name, csurvey_id)); 
  if (sti != this->station_map.end())
    return sti->second;
  
  if (!(this->db->insert_datastation(on, ps))) {
    if (on.survey != NULL)
      ththrow(("object already exist -- %s@%s", on.name, on.survey))
    else
      ththrow(("object already exist -- %s", on.name))
  }
  
  this->station_map[thobjectid(on.name, csurvey_id)] = ++this->lsid;
  this->station_vec.push_back(thdb1ds(on.name, ps));
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


void thdb1d_equate_nodes(thdb1d_tree_node * n1, thdb1d_tree_node * n2)
{

  if (n1->uid == n2->uid)
    return;

  thdb1d_tree_node * n3;
    
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
    thprintf("\n\nscanning data tree\n");
#else
    thprintf("scanning data tree ... ");
    thtext_inline = true;
#endif 

  thdb1d_tree_node * nodes = new thdb1d_tree_node [tn_stations];
  this->tree_nodes = nodes;
  thdb1d_tree_arrow * arrows = new thdb1d_tree_arrow [2 * tn_legs];
  this->tree_arrows = arrows;
  thdb1d_tree_node * n1, * n2, * current_node;
  thdb1d_tree_arrow * a1, * a2;
  unsigned long i, ii;
  
  // let's parse all nodes
  for(i = 0, ii = 1, n1 = nodes; i < tn_stations; i++, n1++, ii++) {
    n1->id = ii;
    n1->uid = ii;
    n1->is_fixed = ((this->station_vec[i].flags & TT_STATIONFLAG_FIXED) != 0);
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
          thdb1d_equate_nodes(n1,n2);
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
        thdb1d_equate_nodes(nodes + (iil->leg->from.id - 1),
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
    if (n1->first_arrow == NULL)
      n1->is_attached = true;
    else if (n1->id != n1->uid)
      n1->is_attached = true;
  }
  
  unsigned long series = 0, component = 0, tarrows = 0, last_series = 0;
  bool component_break = true;
  this->tree_legs = new (thdb1dl *) [tn_legs];
  thdb1dl ** current_leg = this->tree_legs;

  while (tarrows < tn_legs) {
  
    if (component_break) {
      
      // let's find starting node
      n2 = NULL;
      bool n2null = true;
      for(i = 0, n1 = nodes; i < tn_stations; i++, n1++) {
        if (!n1->is_attached) {
          if (n2null) {
            n2 = n1;
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
//#ifdef THDEBUG
//        thprintf("warning -- not all stations connected to the network\n");
//#endif
        break;
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
    thprintf("done.\n");
    thtext_inline = false;
#endif
}




