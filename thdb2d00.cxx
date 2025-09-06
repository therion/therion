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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "thdb2d.h"
#include "thdatabase.h"
#include "thmap.h"
#include "thscrap.h"
#include "thsurvey.h"
#include "thlayout.h"
#include "thconfig.h"
#include <list>
#include <algorithm>
#include <cassert>
#include <cstring>
#include "thmapstat.h"

void thdb2d::insert_basic_maps(thdb2dxm * fmap, thmap * map, int mode, int level, thdb2dmi_shift shift) {
  thdb2dxs * xs, * txs = NULL;
  bool found = false;
  if (map->is_basic) {
    xs = fmap->first_bm;
    while ((!found) && (xs != NULL)) {
      if ((xs->bm->id == map->id) && (xs->m_shift == shift)) {
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
      xs->m_shift = shift;
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


    thdb2dxs_target_item tmp_target(map->projection_id, map->id, shift);
    thdb2dxs_target_map::iterator ti;
    ti = this->m_target_map.find(tmp_target);
    if (ti == this->m_target_map.end()) {
      this->m_target_map[tmp_target] = txs;
      txs->m_target = txs;
    } else {
      txs->m_target = ti->second;
    }
      
    

//    if (txs->mode < mode) {
    if (txs->mode != TT_MAPITEM_NORMAL) {
      txs->mode = mode;
      txs->m_shift = shift;
//      mi = map->first_item;
//      if (map->selection_mode < mode) {


//      if (map->selection_mode != TT_MAPITEM_NORMAL) {
//        map->selection_mode = mode;
//        map->selection_xs = txs;
//      }

      if (txs->m_target->mode != TT_MAPITEM_NORMAL) {
        txs->m_target = txs;
        this->m_target_map[tmp_target] = txs;
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
  for (auto mi = map->first_item; mi != nullptr; mi = mi->next_item) {
    if (auto childmap = dynamic_cast<thmap *>(mi->object)) {
      auto preview_type = mode;

      if (mode == TT_MAPITEM_NORMAL) {
        this->insert_basic_maps(fmap, childmap, mi->type, level + 1, shift.add(mi->m_shift));
        preview_type = mi->m_shift.m_preview;
      }

      if (preview_type == TT_MAPITEM_ABOVE ||
          preview_type == TT_MAPITEM_BELOW) {
        this->insert_basic_maps(fmap, childmap, preview_type, level + 1, shift);
      }
    }
  }
  
}


bool thdb2d_compxm(const thdb2dxm * e1, const thdb2dxm * e2)
{
  if (thisnan(e1->map->z))
    return true;
  if (thisnan(e2->map->z))
    return false;
  return e1->map->z < e2->map->z;
}


bool thdb2d_compscrap(const thscrap * e1, const thscrap * e2)
{
  if (thisnan(e1->z))
    return false;
  if (thisnan(e2->z))
    return true;
  return e1->z > e2->z;
}


thdb2dxm * thdb2d::insert_maps(thdb2dxm * selection,thdb2dxm * insert_after, thmap * map, 
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
          selection = this->insert_maps(selection,insert_after,dynamic_cast<thmap*>(mi->object),
            selection_level, level+1, title_level, map_level);
        else 
          selection = this->insert_maps(selection,cxm,dynamic_cast<thmap*>(mi->object),
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
  thdb2dxm * selection = NULL, * cxm, * lxm = NULL, **nsi;
  thdb2dxs * pcxs;
  unsigned long nmaps = 0, imap, nscraps = 0, iscr;
//  bool chapters = false, onemap = false;
//  thmap * pmap, * prev_pmap;
//  thdb2dmi * pmapitem;
  
  this->reset_selection();
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  
  // prejde vsetky oznacene objekty a pooznacuje mapy
  thselector_list::iterator ii = thcfg.selector.data.begin();
  cxm = selection;
  while (ii != thcfg.selector.data.end()) {
    if ((!ii->unselect) && (ii->optr != NULL) && thcfg.use_maps &&
        (ii->optr->get_class_id() == TT_MAP_CMD) &&
        dynamic_cast<thmap*>(ii->optr)->projection_id == prj->id) {
      selection = this->insert_maps(selection,cxm,dynamic_cast<thmap*>(ii->optr),
        ii->number,0,
        ((ii->map_level >= 0) && (ii->chapter_level > ii->map_level) ? ii->map_level : ii->chapter_level),ii->map_level);
      prj->stat.scanmap(dynamic_cast<thmap*>(ii->optr));  
      prj->stat.addstat(&dynamic_cast<thmap*>(ii->optr)->stat);
      // set selection color
      lxm = selection;
      while (lxm != NULL) {
    	if ((lxm->selection_level == ii->number) && (ii->m_color.defined))
    		lxm->selection_color = ii->m_color;
        lxm = lxm->next_item;
      }
      while ((cxm != NULL) && (cxm->next_item != NULL))
        cxm = cxm->next_item;
    }
    ii++;
  }
  
  lxm = NULL;
  // no map selected, let's add all basic maps
  if (selection == NULL) {
    nmaps = 0;
    obi = this->db->object_list.begin();
    while (obi != this->db->object_list.end()) {
      if (((*obi)->get_class_id() == TT_MAP_CMD) &&
		  thcfg.use_maps &&
          (dynamic_cast<thmap*>(obi->get())->projection_id == prj->id) &&
          (dynamic_cast<thmap*>(obi->get())->is_basic) &&
          (dynamic_cast<thmap*>(obi->get())->fsptr != NULL) &&
          (dynamic_cast<thmap*>(obi->get())->fsptr->is_selected())) {
        prj->stat.scanmap(dynamic_cast<thmap*>(obi->get()));  
        prj->stat.addstat(&dynamic_cast<thmap*>(obi->get())->stat);
        cxm = this->insert_xm();
        cxm->selection_color = dynamic_cast<thmap*>(obi->get())->fsptr->selected_color;
        cxm->title = false;
        cxm->expand = true;
        cxm->map = dynamic_cast<thmap*>(obi->get());
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

    // add centerline to map, even some basic maps exists
    //if (nmaps > 0) {

    //  thscrap * scrapp;
    //  thmap * mapp;
    //  thdb2dmi * xcitem;

    //  scrapp = new thscrap;
    //  scrapp->centerline_io = true;
    //  scrapp->fsptr = NULL;
    //  scrapp->db = &(thdb);
    //  scrapp->proj = prj;
    //  thdb.object_list.push_back(scrapp);

    //  mapp = new thmap;
    //  mapp->db = &(thdb);
    //  mapp->fsptr = NULL;
    //  thdb.object_list.push_back(mapp);

    //  xcitem = thdb.db2d.insert_map_item();
    //  xcitem->itm_level = mapp->last_level;
    //  xcitem->source = thdb.csrc;
    //  xcitem->psurvey = NULL;
    //  xcitem->type = TT_MAPITEM_NORMAL;
    //  xcitem->object = scrapp;
    //  mapp->first_item = xcitem;
    //  mapp->last_item = xcitem;

    //  cxm = this->insert_xm();
    //  cxm->title = false;
    //  cxm->expand = true;
    //  cxm->map = mapp;
    //  prj->stat.scanmap(mapp);  
    //  prj->stat.addstat(&(mapp->stat));

    //  cxm->next_item = selection;
    //  selection->prev_item = cxm;
    //  selection = cxm;
    //  nmaps++;

    //}

    if ((selection == NULL) && (
        (prj->type == TT_2DPROJ_PLAN) || (prj->type == TT_2DPROJ_EXTEND) ||
        (prj->type == TT_2DPROJ_ELEV))) {
      // podme vytvorit jednu mapu a vlozit do nej
      // vsetky surveye
      thscrap * scrapp;
      thmap * mapp;
      thdb2dmi * xcitem;

      // najprv spocitame scrapy
      nscraps = 0;
      obi = this->db->object_list.begin();
      while (obi != this->db->object_list.end()) {
        if (((*obi)->get_class_id() == TT_SCRAP_CMD) &&
            (dynamic_cast<thscrap*>(obi->get())->proj->id == prj->id) &&
            (dynamic_cast<thscrap*>(obi->get())->fsptr != NULL) &&
            (dynamic_cast<thscrap*>(obi->get())->fsptr->is_selected())) {
          nscraps++;
        }
        obi++;
      }  

      {
        auto unique_mapp = std::make_unique<thmap>();
        mapp = unique_mapp.get();
        mapp->id = ++this->db->objid;
        mapp->projection_id = prj->id;
        mapp->db = &(thdb);
        mapp->fsptr = NULL;
        thdb.object_list.push_back(std::move(unique_mapp));
      }

      // ak nemame ani jeden scrap, vytvorime mapu z centerlajnu
      if (nscraps == 0) {

        {
          auto unique_scrapp = std::make_unique<thscrap>();
          scrapp = unique_scrapp.get();
          scrapp->centerline_io = true;
          scrapp->fsptr = NULL;
          scrapp->db = &(thdb);
          scrapp->proj = prj;
          thdb.object_list.push_back(std::move(unique_scrapp));
        }

        xcitem = thdb.db2d.insert_map_item();
        xcitem->itm_level = mapp->last_level;
        xcitem->source = thdb.csrc;
        xcitem->psurvey = NULL;
        xcitem->type = TT_MAPITEM_NORMAL;
        xcitem->object = scrapp;

        mapp->first_item = xcitem;
        mapp->last_item = xcitem;

      }
      
      // do tej mapy povkladame vsetky scrapy, ktore
      // v danej projekcii mame zoradene podla Ztka a kazde
      // na novom levely
      nscraps = 0;
      obi = this->db->object_list.begin();
      while (obi != this->db->object_list.end()) {
        if (((*obi)->get_class_id() == TT_SCRAP_CMD) &&
            (dynamic_cast<thscrap*>(obi->get())->proj->id == prj->id) &&
            (dynamic_cast<thscrap*>(obi->get())->fsptr != NULL) &&
            (dynamic_cast<thscrap*>(obi->get())->fsptr->is_selected())) {
          xcitem = thdb.db2d.insert_map_item();          
          if (mapp->first_item == NULL) {
            mapp->first_item = xcitem;
            mapp->last_item = xcitem;
          } else {
            mapp->last_item->next_item = xcitem;
            xcitem->prev_item = mapp->last_item;
            mapp->last_item = xcitem;
          }
          xcitem->itm_level = ++mapp->last_level;
          xcitem->source = thdb.csrc;
          xcitem->psurvey = NULL;
          xcitem->type = TT_MAPITEM_NORMAL;
          xcitem->object = obi->get();
          nscraps++;
        }
        obi++;
      }  
      
      if (nscraps > 1) {

        // zoradime scrapy podla z-ka
        std::vector<thscrap*> sss(nscraps);
        xcitem = mapp->first_item; //->next_item;
        for(iscr = 0; iscr < nscraps; iscr++) {
          sss[iscr] = dynamic_cast<thscrap*>(xcitem->object);
          xcitem = xcitem->next_item;
        }
        std::sort(sss.begin(), sss.end(), thdb2d_compscrap);
        xcitem = mapp->first_item; //->next_item;
        for(iscr = 0; iscr < nscraps; iscr++) {
          xcitem->object = sss[iscr];
          xcitem = xcitem->next_item;
        }
        
        // vyhodime z mapy prvy scrap
        // mapp->first_item = mapp->first_item->next_item;
        // mapp->first_item->prev_item = NULL;
      }
      
      cxm = this->insert_xm();
      cxm->title = false;
      cxm->expand = true;
      cxm->map = mapp;
      prj->stat.scanmap(mapp);  
      prj->stat.addstat(&(mapp->stat));
      selection = cxm;
    }
    
    
    if (nmaps > 1) {
      // zoradi mapy
      std::vector<thdb2dxm*> new_selection(nmaps);
      for (imap = 0, cxm = selection, nsi = new_selection.data(); imap < nmaps; imap++, cxm = cxm->next_item, nsi++) {
        *nsi = cxm;
      }
      std::sort(new_selection.begin(), new_selection.end(), thdb2d_compxm);
      for (imap = 0, nsi = new_selection.data(); imap < nmaps; imap++, nsi++) {

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
    }
  }
  
  
//  thdb2dxs cxs, cxs2, cxs3;
  unsigned long on = 0;
  if (selection != NULL) {
    // prida do map zakladne mapy
    cxm = selection;
    while (cxm != NULL) {
      if (cxm->expand) {
        cxm->output_number = ++on;
        this->insert_basic_maps(cxm,cxm->map,TT_MAPITEM_NORMAL,0);
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

        thdb2dxs_target_item tmp_target(pcxs->bm->projection_id, pcxs->bm->id, pcxs->m_shift);
        pcxs->m_target = this->m_target_map[tmp_target];

        if (pcxs->m_target->preview_output_number == 0) {
          if ((pcxs->m_target->mode == TT_MAPITEM_NORMAL)
              && pcxs->m_target->fmap->map->is_basic)
            pcxs->m_target->preview_output_number = pcxs->m_target->fmap->output_number;
          else
            pcxs->m_target->preview_output_number = ++on;
        }


//        if (pcxs->bm->selection_xs->preview_output_number == 0) {
//          if ((pcxs->bm->selection_mode == TT_MAPITEM_NORMAL) &&
//              pcxs->bm->selection_xs->fmap->map->is_basic) 
//            pcxs->bm->selection_xs->preview_output_number =
//                pcxs->bm->selection_xs->fmap->output_number;
//          else
//            pcxs->bm->selection_xs->preview_output_number = ++on;
//        }       

        pcxs = pcxs->next_item;
      }
      cxm = cxm->next_item;
    }
  }

  return selection;
}

void thdb2d::reset_selection() {
  this->m_target_map.clear();
  thdb_object_list_type::iterator obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    switch ((*obi)->get_class_id()) {
      case TT_MAP_CMD:
        dynamic_cast<thmap*>(obi->get())->selection_mode = TT_MAPITEM_UNKNOWN;
        break;
//      case TT_SCRAP_CMD:
//        ((thscrap *)(*obi))->selection_mode = TT_MAPITEM_UNKNOWN;
//        ((thscrap *)(*obi))->selection_xs = NULL;
//        break;
    }
    obi++;
  }
}

const char * thdb2dscan_survey_title(thsurvey * fptr, long & min) {

  long newmin = 0, tmpmin;
  if (fptr == NULL) {
    return "";
  }
  const char * newname = fptr->title, * tmpname;
  if (strlen(newname) == 0)
    newname = fptr->name;
  thdataobject * o;
  thsurvey * s = fptr, * ss;
  while (s != NULL) {
  
    o = s->foptr;
    while (o != NULL) {
      while ((o != NULL) && (o->get_class_id() != TT_SURVEY_CMD))
        o = o->nsptr;
      ss = dynamic_cast<thsurvey*>(o);
      if (ss != NULL) {
        tmpname = thdb2dscan_survey_title(ss, tmpmin);
//        printf("SCAN %s: %d\n", ss->name, tmpmin);
        s->num1 += tmpmin;
        if (tmpmin > newmin) {
          newmin = tmpmin;
          newname = tmpname;
        }
        o = o->nsptr; 
      }
    }
    
//    printf("TOP %s: %d\n", s->name, s->num1);
    if (s->num1 > newmin) {
//      if (newmin != 0)
//        printf("OLD %s: %d\n", newname, newmin);
      newmin = s->num1;
      newname = s->title;
//        printf("NEW %s: %d\n", newname, newmin);
      if (strlen(newname) == 0)
        newname = s->name;
    }
    
    while ((o != NULL) && (o->get_class_id() != TT_SURVEY_CMD))
      o = o->nsptr;
    s = dynamic_cast<thsurvey*>(o);
    
  }
  
  min = newmin;
//  if (min > 0) printf("RETURN %s: %d\n", newname, newmin);
  return newname;
}


double thdb2d::get_projection_entrance_altitude(thdb2dprj * prj) {
	  thobjectname entrance;
	  thselector_list::iterator ii = thcfg.selector.data.begin();
	  while (ii != thcfg.selector.data.end()) {
	    if ((!ii->unselect) && (ii->optr != NULL) &&
	        (ii->optr->get_class_id() == TT_MAP_CMD) &&
	        (dynamic_cast<thmap*>(ii->optr)->projection_id == prj->id)) {
	      if (entrance.is_empty()) {
	    	  entrance = dynamic_cast<thmap*>(ii->optr)->fsptr->get_entrance();
	      }
	    }
	    ii++;
	  }
	  thsurvey * cs;
	  if (entrance.is_empty()) {
		  auto obi = this->db->object_list.begin();
		  while (obi != this->db->object_list.end()) {
			  if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
			        cs = dynamic_cast<thsurvey*>(obi->get());
			        if (entrance.is_empty()) {
				        if (cs->is_selected()) entrance = cs->get_entrance();
			        }
			  }
			  obi++;
		  }
	  }
	  if (entrance.is_empty()) {
		  auto obi = this->db->object_list.begin();
		  while (obi != this->db->object_list.end()) {
			  if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
			        cs = dynamic_cast<thsurvey*>(obi->get());
			        if (entrance.is_empty()) {
				        entrance = cs->get_entrance();
			        }
			  }
			  obi++;
		  }
	  }
	  if (entrance.is_empty()) return 0;
	  return this->db->db1d.station_vec[entrance.id-1].z;

}


const char * thdb2d::get_projection_title(thdb2dprj * prj) {

  // krok cislo jedna - prejde celu selection - ak najde jednu oznacenu
  // mapu - tak zoberie jej title //alebo name
  long dum;
  
  const char * rv = NULL;
  unsigned long nmaps = 0;
  thselector_list::iterator ii = thcfg.selector.data.begin();
  while (ii != thcfg.selector.data.end()) {
    if ((!ii->unselect) && (ii->optr != NULL) && 
        (ii->optr->get_class_id() == TT_MAP_CMD) &&
        (dynamic_cast<thmap*>(ii->optr)->projection_id == prj->id)) {
      nmaps++;
      if (nmaps > 1) {
        rv = NULL;
        break;
      }
      if (strlen(dynamic_cast<thmap*>(ii->optr)->title) > 0) {
        rv = dynamic_cast<thmap*>(ii->optr)->title;
      } 
      //else {
      //  rv = ((thmap*)(ii->optr))->name;
      //}
    }
    ii++;
  }
  
  if (rv != NULL)
    return rv;
  
  // prescanuje vsetky objekty a surveyom priradi pocet scrapov, ktore
  // sa z nich exportovali
  thdb_object_list_type::iterator obi = this->db->object_list.begin(), obi2;
  while (obi != this->db->object_list.end()) {
    if ((*obi)->get_class_id() == TT_SURVEY_CMD) {
      dynamic_cast<thsurvey*>(obi->get())->num1 = 0;
    }
    obi++;
  }
  
  obi = this->db->object_list.begin();
  while (obi != this->db->object_list.end()) {
    if (((*obi)->get_class_id() == TT_SCRAP_CMD) &&
        (dynamic_cast<thscrap*>(obi->get())->proj->id == prj->id) && 
        (dynamic_cast<thscrap*>(obi->get())->exported)) {
      if (dynamic_cast<thscrap*>(obi->get())->fsptr != NULL) {
        dynamic_cast<thscrap*>(obi->get())->fsptr->num1++;
      } else if (dynamic_cast<thscrap*>(obi->get())->centerline_survey != NULL) {
        dynamic_cast<thscrap*>(obi->get())->centerline_survey->num1++;
      } else {
        // prejde vsetky oznacene a da im num1 = 1
        obi2 = this->db->object_list.begin();
        while (obi2 != this->db->object_list.end()) {
          if (((*obi2)->get_class_id() == TT_SURVEY_CMD) && ((*obi2)->selected)) {
            dynamic_cast<thsurvey*>(obi2->get())->num1 = 1;
          }
          obi2++;
        }
      }
    }
    obi++;
  }
  
  // rekurzivne prejde vsetky surveye a najde najnizsie take
  // co ma v sebe najviac scrapov a zoberie jeho nazov
  dum = 0;
  rv = thdb2dscan_survey_title(this->db->fsurveyptr, dum);
  
  return rv;
  
}



