/**
 * @file thdb2d00.cxx
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
 
#include "thdb2d.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thparse.h"
#include "thtfangle.h"
#include "tharea.h"
#include "thmap.h"
#include "thjoin.h"
#include "thpoint.h"
#include "thline.h"
#include "thscrap.h"
#include "thsurvey.h"
#include "thlogfile.h"
#include <math.h>
#include "thlayout.h"
#include "thconfig.h"
#include <list>


void thdb2d::insert_basic_maps(thdb2dxm * fmap, thmap * map, int mode) {
  thdb2dxs * xs, * txs;
  thdb2dmi * mi;
  bool found = false;
  if (map->is_basic) {
    xs = fmap->first_bm;
    while ((!found) && (xs != NULL)) {
      if (xs->bm->id == map->id) {
        found = true;
        txs = xs;
      }
      xs = xs->next_item;
    }
    if (!found) {
      xs = this->insert_xs();
      txs = xs;
      xs->bm = map;
      xs->fmap = fmap;
      if (fmap->first_bm == NULL) {
        fmap->first_bm = xs;
        fmap->last_bm = xs;
      }
      else {
        fmap->last_bm->next_item = xs;
        xs->prev_item = fmap->last_bm;
        fmap->last_bm = xs;
      }
    }
    if (txs->mode < mode) {
      txs->mode = mode;
      mi = map->first_item;
      if (map->selection_mode < mode) {
        map->selection_mode = mode;
        map->selection_xs = txs;
      }
//      while (mi != NULL) {
//        if ((mi->type == TT_MAPITEM_NORMAL) && (((thscrap*)(mi->object))->selection_mode < mode)) {
//          ((thscrap*)(mi->object))->selection_mode = mode;
//          ((thscrap*)(mi->object))->selection_xs = txs;
//        }
//        mi = mi->next_item;
//      }
    }
  }
  
  // prejde vsetky referencie
  mi = map->first_item;
  while (mi != NULL) {
    if (mode == TT_MAPITEM_NORMAL) {
      if ((mi->type != TT_MAPITEM_NORMAL) || (!map->is_basic))
        this->insert_basic_maps(fmap,(thmap *)mi->object,mi->type);
    } else {
      if ((mi->type == TT_MAPITEM_NORMAL) && (!map->is_basic))
        this->insert_basic_maps(fmap,(thmap *)mi->object,mode);
    }
    mi = mi->next_item;
  }
  
}


int thdb2d_compxm(const void * ee1, const void * ee2)
{
  thdb2dxm ** e1 = (thdb2dxm **) ee1, ** e2 = (thdb2dxm **) ee2;
  if ((*e1)->map->z < (*e2)->map->z) {
    return -1;
  } else if ((*e1)->map->z == (*e2)->map->z)
    return 0;
  else
    return 1;
}


thdb2dxm * thdb2d::insert_maps(thdb2dxm * selection,thdb2dxm * insert_after,thmap * map, 
    unsigned long selection_level, int level, int title_level, int map_level) {
  thdb2dxm * cxm = NULL;
  thdb2dmi * mi;
  if ((level == title_level) || (level == map_level) || (map->is_basic)) {
    cxm = this->insert_xm();
    cxm->map = map;
    cxm->selection_level = selection_level;
    if (insert_after == NULL) {
      cxm->next_item = selection;
      if (selection != NULL)
        selection->prev_item = cxm;
      selection = cxm;
    } else {
      cxm->next_item = insert_after->next_item;
      if (insert_after->next_item != NULL)
        insert_after->prev_item = cxm;
      insert_after->next_item = cxm;
      cxm->prev_item = insert_after;
    }
    if (level == title_level)
      cxm->title = true;
    if ((level == map_level) || (map->is_basic))
      cxm->expand = true;
  }
  
  // ak treba, preskuma pod nim
  if ((!map->is_basic) && ((level < map_level) || (map_level < 0))) {
    mi = map->first_item;
    while (mi != NULL) {
      if (mi->type == TT_MAPITEM_NORMAL) {
        if (cxm == NULL)
          selection = this->insert_maps(selection,insert_after,(thmap *) mi->object, 
            selection_level, level+1, title_level, map_level);
        else 
          selection = this->insert_maps(selection,cxm,(thmap *) mi->object, 
            selection_level, level+1, title_level, map_level);
      }
      mi = mi->next_item;
    }
  }
  
  return selection;
}

/*


  goto DEBUG_SKIP_SELECTION;
  
  while (obi != this->db->object_list.end()) {
    if (((*obi)->get_class_id() == TT_MAP_CMD) &&
        (((thmap*)(*obi))->projection_id == prj->id) &&
        ((*obi)->is_selected())) {
      nmaps++;
      pmap = (thmap*)(*obi);
      if (nmaps == 2) {
        onemap = false;
        if ((map_level > 0) && (!prev_pmap->is_basic))
          chapters = true;
        else
          chapters = false;
      }
      if (!chapters) {
        if (nmaps == 1) {
          onemap = true;
          if ((map_level > 1) && (!pmap->is_basic)) {
            // prejde vsetky pod a zisti, ci je nejaka nie basic
            pmapitem = pmap->first_item;
            while ((!chapters) && (pmapitem != NULL)) {
              if ((!((thmap *)(pmapitem->object))->is_basic) && 
                  (pmapitem->type == TT_MAPITEM_NORMAL)) {
                chapters = true;
              }
              pmapitem = pmapitem->next_item;
            }
          }
        } else {
          if ((map_level > 0) && (!pmap->is_basic))
            chapters = true;
        }
      }
      prev_pmap = pmap;
    }
    obi++;
  }
  
  // prida kapitoly a podkapitoly
  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if (((*obi)->get_class_id() == TT_MAP_CMD) &&
        (((thmap*)(*obi))->projection_id == prj->id) &&
        ((*obi)->is_selected())) {
      cxm = selection;
      if (cxm != 0) {
        while ((cxm != NULL) && (cxm->selection_level <= (*obi)->selected_number)) {
          cxm = cxm->next_item;
        }
      }
      if (chapters && onemap) {
        selection = this->insert_maps(selection,cxm,(thmap*)(*obi),
          (*obi)->selected_number,0,1,map_level);
      } 
      else if (chapters) {
        selection = this->insert_maps(selection,cxm,(thmap*)(*obi),
          (*obi)->selected_number,0,0,map_level);
      }
      else {
        selection = this->insert_maps(selection,cxm,(thmap*)(*obi),
          (*obi)->selected_number,0,-1,map_level);
      }
    }
    obi++;
  }
  
  DEBUG_SKIP_SELECTION:
  */
  



thdb2dxm * thdb2d::select_projection(thdb2dprj * prj)
{

  // najde vsetky mapy ktore mame oznacene, resp. vyberie vsetky zakladne  
  thdb2dxm * selection = NULL, * cxm, * lxm, * * new_selection, **nsi;
  thdb2dxs * pcxs;
  unsigned long nmaps = 0, imap;
//  bool chapters = false, onemap = false;
//  thmap * pmap, * prev_pmap;
//  thdb2dmi * pmapitem;
  
  this->reset_selection();
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  
  // prejde vsetky oznacene objekty a pooznacuje mapy
  thselector_list::iterator ii = thcfg.selector.data.begin();
  cxm = selection;
  while (ii != thcfg.selector.data.end()) {
    if ((!ii->unselect) && (ii->optr != NULL) && 
        (ii->optr->get_class_id() == TT_MAP_CMD)) {
      selection = this->insert_maps(selection,cxm,(thmap*)(ii->optr),
        ii->number,0,
        ((ii->map_level >= 0) && (ii->chapter_level > ii->map_level) ? ii->map_level : ii->chapter_level),ii->map_level);
      while ((cxm != NULL) && (cxm->next_item != NULL))
        cxm = cxm->next_item;
    }
    ii++;
  }
  
  // no map selected, let's add all basic maps
  if (selection == NULL) {
    nmaps = 0;
    obi = this->db->object_list.begin();
    while (obi != this->db->object_list.end()) {
      if (((*obi)->get_class_id() == TT_MAP_CMD) &&
          (((thmap*)(*obi))->projection_id == prj->id) &&
          (((thmap*)(*obi))->is_basic)) {
        cxm = this->insert_xm();
        cxm->title = false;
        cxm->expand = true;
        cxm->map = (thmap*)(*obi);
        cxm->map->calc_z();
        nmaps++;
        if (selection == NULL) {
          selection = cxm;
          lxm = cxm;
        } else {
          lxm->next_item = cxm;
          cxm->prev_item = lxm;
          lxm = cxm;
        }
      }
      obi++;
    }
    
    
    if (nmaps > 1) {
      // zoradi mapy
      new_selection = new (thdb2dxm*) [nmaps];
      for (imap = 0, cxm = selection, nsi = new_selection; imap < nmaps; imap++, cxm = cxm->next_item, nsi++) {
        *nsi = cxm;
      }
      qsort(new_selection,nmaps,sizeof(thdb2dxm*),thdb2d_compxm);
      for (imap = 0, nsi = new_selection; imap < nmaps; imap++, nsi++) {

        if (imap == 0) {
          selection = *nsi;
          (*nsi)->prev_item = NULL;
        }
        else { 
          lxm->next_item = *nsi;
          (*nsi)->prev_item = lxm;
        }

        (*nsi)->next_item = NULL;
        lxm = *nsi;
        
      }
      
      delete [] new_selection;
    }
  }
  
//  thdb2dxs cxs, cxs2, cxs3;
  unsigned long on = 0;
  if (selection != NULL) {
    // prida do map zakladne mapy
    cxm = selection;
    while (cxm != NULL) {
      if (cxm->expand) {
        this->insert_basic_maps(cxm,cxm->map,TT_MAPITEM_NORMAL);
        cxm->output_number = ++on;
      }
      cxm = cxm->next_item;
    }
  }
  
  if (selection != NULL) {
    cxm = selection;
    while (cxm != NULL) {
      pcxs = cxm->first_bm;
      while (pcxs != NULL) {
        // ak je fmap zakladna, tak OK a PON = ON
        // inak PON = ++on
        if (pcxs->bm->selection_xs->preview_output_number == 0) {
          if ((pcxs->bm->selection_mode == TT_MAPITEM_NORMAL) &&
              pcxs->bm->selection_xs->fmap->map->is_basic) 
            pcxs->bm->selection_xs->preview_output_number =
                pcxs->bm->selection_xs->fmap->output_number;
          else
            pcxs->bm->selection_xs->preview_output_number = ++on;
//            pcxs->bm->selection_xs->preview_output_number = 0;
        }
        pcxs = pcxs->next_item;
      }
      cxm = cxm->next_item;
    }
  }

  return selection;
}

void thdb2d::reset_selection() {
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    switch ((*obi)->get_class_id()) {
      case TT_MAP_CMD:
        ((thmap *)(*obi))->selection_mode = TT_MAPITEM_UNKNOWN;
        break;
//      case TT_SCRAP_CMD:
//        ((thscrap *)(*obi))->selection_mode = TT_MAPITEM_UNKNOWN;
//        ((thscrap *)(*obi))->selection_xs = NULL;
//        break;
    }
    obi++;
  }
}






