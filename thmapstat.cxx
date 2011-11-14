/**
 * @file thmapstat.cxx
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
 
#include "thmapstat.h"
#include "thscrap.h"
#include "thdata.h"
#include "thmap.h"
#include "thdataobject.h"
#include "thlayout.h"
#include "thlang.h"
#include "thtexfonts.h"
#include "thsurvey.h"
#include "thdb1d.h"
#include <string.h>

bool operator < (const thmapstat_data & c1, 
    const thmapstat_data & c2) {
  return (c1.crit < c2.crit);  
}


bool operator < (const thmapstat_copyright & c1, 
    const thmapstat_copyright & c2) {
  return (strcmp(c1.str,c2.str) < 0);
}

bool operator < (const thmapstat_dataptr & c1, 
    const thmapstat_dataptr & c2) {
  return (c1.ptr->id < c2.ptr->id);
}


thmapstat_data::thmapstat_data() {
  this->crit = 0.0;
}

thmapstat_dataptr::thmapstat_dataptr() {
  this->ptr = NULL;
}


thmapstat::thmapstat()
{
  scanned = false;
}


void thmapstat::adddata(thmapstat_datamap * dm) {

  // prejde vsetky data, prida si ich a ak ich nema, prida
  // si z nich meracov a objavitelov

  thmapstat_datamap::iterator ii;

#ifdef THDEBUG
  double sumsum = 0.0;
  thprintf("\nBEFORE:\n");
  for(ii = this->data.begin(); ii != this->data.end(); ii++) {
    sumsum += ii->first.ptr->stat_length + ii->first.ptr->stat_dlength;
    thprintf("   %d (+ %.0f = %.0f) [%s]\n",ii->first.ptr->id,
      ii->first.ptr->stat_length + ii->first.ptr->stat_dlength, sumsum, ii->first.ptr->fsptr->full_name);
  }
  thprintf("\n");
#endif  

  ii = dm->begin();
  while (ii != dm->end()) {
  
    if ((ii->second > 1) && 
        (this->data.find(ii->first) == this->data.end())) {
      this->data[ii->first] = 2;

#ifdef THDEBUG
  sumsum += ii->first.ptr->stat_length + ii->first.ptr->stat_dlength;
    thprintf(" + %d (+ %.0f = %.0f) [%s]\n",ii->first.ptr->id,
      ii->first.ptr->stat_length + ii->first.ptr->stat_dlength, sumsum, ii->first.ptr->fsptr->full_name);
#endif  

      thdata_team_set_type::iterator ti;
      
      for(ti = ii->first.ptr->team_set.begin();
        ti != ii->first.ptr->team_set.end(); ti++) {
        this->surveyed_by[*ti].crit += ii->first.ptr->stat_length + ii->first.ptr->stat_dlength;
        this->surveyed_by[*ti].date.join(ii->first.ptr->date);
      }
      this->surveyed_date.join(ii->first.ptr->date);

      for(ti = ii->first.ptr->discovery_team_set.begin();
        ti != ii->first.ptr->discovery_team_set.end(); ti++) {
        this->discovered_by[*ti].crit += ii->first.ptr->stat_length + ii->first.ptr->stat_dlength;
        this->discovered_by[*ti].date.join(ii->first.ptr->discovery_date);
      }
      this->discovered_date.join(ii->first.ptr->discovery_date);
      
    }
    
    ii++;
  }
  
}


void thmapstat::addstat(thmapstat * source) {
  // prejde vsetkych zdrojovych autorov a copyrighty a prida
  // si ich

  thmapstat_personmap::iterator aii;
  for (aii = source->drawn_by.begin();
        aii != source->drawn_by.end(); aii++) {
    this->drawn_by[aii->first].crit += aii->second.crit;
    this->drawn_by[aii->first].date.join(aii->second.date);
  }
  this->drawn_date.join(source->drawn_date);

  thmapstat_copyrightmap::iterator cii;
  for (cii = source->copyright.begin();
        cii != source->copyright.end(); cii++) {
    this->copyright[cii->first].crit += cii->second.crit;
    this->copyright[cii->first].date.join(cii->second.date);
  }
  
  this->adddata(&(source->data));
  
}

void thmapstat::addobj(thdataobject * obj) {

  // prejde vsetkych zdrojovych autorov a copyrighty a prida
  // si ich
  thdo_author_map_type::iterator aii;
  for (aii = obj->author_map.begin();
        aii != obj->author_map.end(); aii++) {
    this->drawn_by[aii->first.name].crit += 1.0;
    this->drawn_by[aii->first.name].date.join(aii->second);
    this->drawn_date.join(aii->second);
  }
  
  thdo_copyright_map_type::iterator cii;
  thmapstat_copyright tmpc;
  for (cii = obj->copyright_map.begin();
        cii != obj->copyright_map.end(); cii++) {
    tmpc.str = cii->first.name;
    this->copyright[tmpc].crit += 1.0;
    this->copyright[tmpc].date.join(cii->second);
  }
  
}


void thmapstat::scanmap(class thmap * map) {

  // upravi mape mapstat (rekurzivne), ak nebola scanovana
  if (map->stat.scanned)
    return;
    
  // prejde najprv podmapy - 
  // ak mapy - tak zavola scanmap z ich statu a prida ich do map->statu
  // ak scrapy - tak prida do map->statu manualne autorov, copyrighty a 
  // data, ktore maju viac ako 2 body
  thdb2dmi * mi = map->first_item;
  thmap * mapp;
  thscrap * scrapp;
  while (mi != NULL) {
    if (mi->type == TT_MAPITEM_NORMAL) {
      switch (mi->object->get_class_id()) {
        case TT_MAP_CMD:
          mapp = (thmap *) mi->object;
          mapp->stat.scanmap(mapp);
          map->stat.addstat(&(mapp->stat));
          break;
        case TT_SCRAP_CMD:
          scrapp = (thscrap *) mi->object;
          scrapp->get_polygon();
          map->stat.addobj(scrapp);
          map->stat.adddata(&(scrapp->adata));
          break;
      }
    }
    mi = mi->next_item;
  }
  
  // potom prida do map->statu autorov a copyrighty z map
  // map->stat.addobj(map);

  // nakoniec nastavi ze uz bola scanovana
  map->stat.scanned = true;
  
}


struct thmapstat_person_data {
  thperson person;
  double crit;
};

int thmapstat_person_data_compar(const void * d1, const void * d2) {
  thmapstat_person_data * p1 = (thmapstat_person_data *) d1, 
    * p2 = (thmapstat_person_data *) d2;
  if (p1->crit < p2->crit)
    return 1;
  else if (p1->crit > p2->crit)
    return -1;
  else if (p1->person < p2->person)
    return -1;
  else if (p1->person == p2->person)
    return 0;
  else
    return 1;
}

#define thmapstat_print_team(team_map, team_name, max_items, teamstr) \
    fprintf(f,"\\" team_name "={"); \
    pd = new thmapstat_person_data [team_map.size()]; \
    i = 0; \
    for (pi = team_map.begin(); \
          pi != team_map.end(); pi++) { \
      pd[i].person = pi->first; \
      pd[i].crit = pi->second.crit; \
      i++; \
    } \
    qsort(pd,cnt,sizeof(thmapstat_person_data),thmapstat_person_data_compar); \
    if (max_items >= 0) \
      tcnt = (unsigned long)(max_items); \
    else \
      tcnt = cnt; \
    for (i = 0; i < tcnt; i++) { \
      b = pd[i].person.get_n1(); \
      b += " "; \
      b += pd[i].person.get_n2(); \
      if (show_lengths) { \
        b += " ("; \
				b += layout->units.format_length(pd[i].crit); \
        b += "<thsp>"; \
		    b += layout->units.format_i18n_length_units(); \
        b += ")"; \
      } \
      if (i < (tcnt-1)) \
        b += ","; \
      fprintf(f,"%s%s",utf2tex(b.get_buffer()),(i < (tcnt-1) ? "\n" : "")); \
      teamstr += thutf82xhtml(b.get_buffer()); \
    } \
    fprintf(f,"}\n");
  
//      snprintf(c.get_buffer(),127,"%.0f",pd[i].crit);
//      b += c.get_buffer();
//      b += thT("units m",layout->lang);


void thmapstat::export_pdftex(FILE * f, class thlayout * layout, legenddata * ldata) {

  thbuffer b,c;
  unsigned long i, cnt, tcnt;
  thmapstat_personmap::iterator pi;
  thmapstat_copyrightmap::iterator ci;
  thmapstat_datamap::iterator ii;
  thmapstat_person_data * pd;
  bool show_lengths, z_any = false;
  double clen = 0.0, z_top = 0.0, z_bot = 0.0;
  c.guarantee(256);
  b.guarantee(256);


#define check_z(st) { \
  if (st != NULL) { \
      if (!z_any) { \
        z_top = st->z; \
        z_bot = st->z; \
        z_any = true; \
      } else { \
        if (z_top < st->z) z_top = st->z; \
        if (z_bot > st->z) z_bot = st->z; \
      } \
    } \
  }
  
  for(ii = this->data.begin(); ii != this->data.end(); ii++) {
    check_z(ii->first.ptr->stat_st_top);
    check_z(ii->first.ptr->stat_st_bottom);
    clen += ii->first.ptr->stat_length;
  }

  ldata->cavelength = "";
  ldata->cavelengthtitle = "";
  if (clen > 0) {
    //b = "";  
    //snprintf(b.get_buffer(),255,"%.0f",clen);
		b = layout->units.format_length(clen);
    b += "<thsp>";
    //b += thT("units m",layout->lang);
    b += layout->units.format_i18n_length_units();
    fprintf(f,"\\cavelengthtitle={%s}\n",utf2tex(thT("title cave length",layout->lang)));
    fprintf(f,"\\cavelength={%s}\n",utf2tex(b.get_buffer()));
    ldata->cavelength = thutf82xhtml(b.get_buffer());
    ldata->cavelengthtitle = thT("title cave length",layout->lang);
  }
  
  ldata->cavedepth = "";
  ldata->cavedepthtitle = "";

  if (z_top > z_bot) {
    //b = "";  
    //snprintf(b.get_buffer(),255,"%.0f",z_top-z_bot);
		b = layout->units.format_length(z_top-z_bot);
    b += "<thsp>";
    //b += thT("units m",layout->lang);
    b += layout->units.format_i18n_length_units();
    fprintf(f,"\\cavedepthtitle={%s}\n",utf2tex(thT("title cave depth",layout->lang)));
    fprintf(f,"\\cavedepth={%s}\n",utf2tex(b.get_buffer()));

    ldata->cavedepth = thutf82xhtml(b.get_buffer());
    ldata->cavedepthtitle = thT("title cave depth",layout->lang);

  }
  
  show_lengths = layout->explo_lens;
  
  ldata->explodate = "";
  ldata->exploteam = "";
  ldata->explotitle = "";
  if ((this->discovered_by.size() > 0) && (layout->max_explos != 0)) {
  
    cnt = this->discovered_by.size();

    if (cnt > 1) {
      fprintf(f,"\\explotitle={%s}\n",
        utf2tex(thT("title explo (plural)",layout->lang)));
      ldata->explotitle = thT("title explo (plural)",layout->lang);
    } else {
      fprintf(f,"\\explotitle={%s}\n",
        utf2tex(thT("title explo",layout->lang)));
      ldata->explotitle = thT("title explo",layout->lang);
    }

    if (discovered_date.is_defined()) {
      fprintf(f,"\\explodate={%s}\n",
        utf2tex(discovered_date.get_str(TT_DATE_FMT_UTF8_Y)));
      ldata->explodate = discovered_date.get_str(TT_DATE_FMT_UTF8_Y);
    }
  
    thmapstat_print_team(this->discovered_by,"exploteam", layout->max_explos, ldata->exploteam);
    
  }
  

  ldata->topodate = "";
  ldata->topoteam = "";
  ldata->topotitle = "";
  show_lengths = layout->topo_lens;
  if ((this->surveyed_by.size() > 0) && (layout->max_topos != 0)) {
  
    cnt = this->surveyed_by.size();

    if (cnt > 1) {
      fprintf(f,"\\topotitle={%s}\n",
        utf2tex(thT("title topo (plural)",layout->lang)));
      ldata->topotitle = thT("title topo (plural)",layout->lang);
    } else {
      fprintf(f,"\\topotitle={%s}\n",
        utf2tex(thT("title topo",layout->lang)));
      ldata->topotitle = thT("title topo",layout->lang);
    }
  
    if (surveyed_date.is_defined()) {
      fprintf(f,"\\topodate={%s}\n",
        utf2tex(surveyed_date.get_str(TT_DATE_FMT_UTF8_Y)));
      ldata->topodate = surveyed_date.get_str(TT_DATE_FMT_UTF8_Y);
    }
  
    thmapstat_print_team(this->surveyed_by,"topoteam", layout->max_topos, ldata->topoteam);
    
  }

  ldata->cartodate = "";
  ldata->cartoteam = "";
  ldata->cartotitle = "";
  show_lengths = false;
  if ((this->drawn_by.size() > 0) && (layout->max_cartos != 0)) {

    cnt = this->drawn_by.size();

    if (cnt > 1) {
      fprintf(f,"\\cartotitle={%s}\n",
        utf2tex(thT("title carto (plural)",layout->lang)));
      ldata->cartotitle = thT("title carto (plural)",layout->lang);
    } else {
      fprintf(f,"\\cartotitle={%s}\n",
        utf2tex(thT("title carto",layout->lang)));
      ldata->cartotitle = thT("title carto",layout->lang);
    }

    if (drawn_date.is_defined()) {
      fprintf(f,"\\cartodate={%s}\n",
        utf2tex(drawn_date.get_str(TT_DATE_FMT_UTF8_Y)));
      ldata->cartodate = drawn_date.get_str(TT_DATE_FMT_UTF8_Y);
    }
  
    thmapstat_print_team(this->drawn_by,"cartoteam", layout->max_cartos, ldata->cartoteam);
    
  }

  ldata->copyrights = "";
  if ((this->copyright.size() > 0) && (layout->max_copys != 0)) {
    cnt = this->copyright.size();
    fprintf(f,"\\copyrights={%s",utf2tex("(c) "));
    ldata->copyrights = "(c) ";
    i = 0;
    for (ci = this->copyright.begin();
          (ci != this->copyright.end()) && ((layout->max_copys < 0) || (i < ((unsigned long)layout->max_copys))); ci++) {
      i++;
      b = ci->first.str;
      b += " ";
      b += ci->second.date.get_str(TT_DATE_FMT_UTF8_Y);
      if (i < cnt)
        b += ", ";
      ldata->copyrights += thutf82xhtml(b.get_buffer());
      fprintf(f,"%s%s",utf2tex(b.get_buffer()),(i < cnt ? "\n" : ""));
    }
    fprintf(f,"}\n");
  }
  
}

