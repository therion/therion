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
#include "thversion.h"
#include "thtexfonts.h"
#include "thsurvey.h"
#include "thdb1d.h"
#include "thconfig.h"
#include "thcs.h"
#include <vector>
#include <algorithm>
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

void thmapstat::resetdata() {
	this->data.clear();
	this->surveyed_by.clear();
	this->discovered_by.clear();
	this->surveyed_date.reset();
	this->discovered_date.reset();
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
      ii->first.ptr->stat_length + ii->first.ptr->stat_dlength, 
      sumsum, ii->first.ptr->fsptr->full_name);
  }
  thprintf("\n");
#endif  

  ii = dm->begin();
  while (ii != dm->end()) {
  
    if (this->data.find(ii->first) == this->data.end()) {
      this->data[ii->first] = 2;

#ifdef THDEBUG
  sumsum += ii->first.ptr->stat_length + ii->first.ptr->stat_dlength;
    thprintf(" + %d (+ %.0f = %.0f) [%s]\n",ii->first.ptr->id,
      ii->first.ptr->stat_length + ii->first.ptr->stat_dlength, 
      sumsum, ii->first.ptr->fsptr->full_name);
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
        this->discovered_by[*ti].crit += ii->first.ptr->stat_length; // + ii->first.ptr->stat_dlength;
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

	if (map->asoc_survey.psurvey != NULL) {
		map->stat.resetdata();
		thmapstat_datamap dm;
		thmapstat_dataptr dp;
		thdataobject * obj;
		for(thdb_object_list_type::iterator it = thdb.object_list.begin(); it != thdb.object_list.end(); it++) {
			obj = it->get();
			if ((obj->get_class_id() == TT_DATA_CMD) && (obj->is_in_survey(map->asoc_survey.psurvey))) {
				dp.ptr = (thdata*)obj;
				dm[dp] = 1;
			}
		}
		map->stat.adddata(&dm);
	}

  
  // potom prida do map->statu autorov a copyrighty z map
  // map->stat.addobj(map);

  // nakoniec nastavi ze uz bola scanovana
  map->stat.scanned = true;
  
}


struct thmapstat_person_data {
  thperson person;
  double crit;
  bool operator < (const thmapstat_person_data& x) const
  {
	  if (crit > x.crit)
	    return true;
	  else if (crit < x.crit)
	    return false;
	  else if (person < x.person)
	    return true;
	  else
	    return false;
  }
};

struct thmapstat_copyright_data {
  thmapstat_copyright copy;
  thmapstat_data data;
  double crit;
  bool operator < (const thmapstat_copyright_data& x) const
  {
	  if (crit > x.crit)
	    return true;
	  else if (crit < x.crit)
	    return false;
	  else if (strcmp(copy.str, x.copy.str) < 0)
	    return true;
	  else
	    return false;
  }
};

void thmapstat_print_team(FILE * f, thmapstat_personmap & team_map, const char * team_name, int max_items, bool alphasort, std::string & teamstr, bool show_lengths, bool show_count, thlayout * layout, thbuffer & c){
    fprintf(f, "\\%s={", team_name);
    std::vector<thmapstat_person_data> pd;
    for (auto pi = team_map.begin(); pi != team_map.end(); pi++) {
		thmapstat_person_data ditem;
		ditem.person = pi->first;
		if (alphasort)
			ditem.crit = 0.0;
		else
			ditem.crit = pi->second.crit;
		pd.push_back(ditem);
    }
    if ((pd.size() > 0) && (max_items != 0)) {
    	std::sort(pd.begin(), pd.end());
    	unsigned long maxcnt(pd.size());
    	if (max_items > 0)
    		maxcnt = (unsigned long) max_items;
    	if (maxcnt > pd.size())
    		maxcnt = pd.size();
    	std::string b;
    	size_t i;
        for (i = 0; i < maxcnt; i++) {
          b = pd[i].person.get_n1();
          if (b.length() > 0) b += " ";
          b += pd[i].person.get_n2();
          if (show_lengths) {
            b += " (";
    		if (!show_count) {
    				b += layout->units.format_length(pd[i].crit);
    				b += "<thsp>";
    				b += layout->units.format_i18n_length_units();
    		} else {
          	  	    std::snprintf(c.get_buffer(),127,"%.0f",pd[i].crit);
          	  	    b += c.get_buffer();
    		}
            b += ")";
          }
          if (i < (maxcnt-1))
            b += ", ";
          fprintf(f,"%s%s",utf2tex(b).c_str(),(i < (maxcnt-1) ? "\n" : ""));
          teamstr += thutf82xhtml(b.c_str());
        }
    }
    fprintf(f,"}\n");
}


void thmapstat_print_copy(FILE * f, thmapstat_copyrightmap & copy_map, const char * team_name, int max_items, bool alphasort, std::string & teamstr, bool show_lengths, thlayout * layout, thbuffer & c){
    fprintf(f, "\\%s={", team_name);
    std::vector<thmapstat_copyright_data> pd;
    for (auto pi = copy_map.begin(); pi != copy_map.end(); pi++) {
		thmapstat_copyright_data ditem;
		ditem.copy = pi->first;
		ditem.data = pi->second;
		if (alphasort)
			ditem.crit = 0.0;
		else
			ditem.crit = pi->second.crit;
		pd.push_back(ditem);
    }
    if ((pd.size() > 0) && (max_items != 0)) {
        fprintf(f, "%s", utf2tex(teamstr).c_str());
    	std::sort(pd.begin(), pd.end());
    	unsigned long maxcnt(pd.size());
    	if (max_items > 0)
    		maxcnt = (unsigned long) max_items;
    	if (maxcnt > pd.size())
    		maxcnt = pd.size();
    	std::string b;
    	size_t i;
        for (i = 0; i < maxcnt; i++) {
			b = pd[i].copy.str;
			b += " ";
			b += pd[i].data.date.get_str(TT_DATE_FMT_UTF8_Y);
			if (show_lengths) {
			  b += " (";
			  std::snprintf(c.get_buffer(),127,"%.0f",pd[i].crit);
			  b += c.get_buffer();
			  b += ")";
			}
            if (i < (maxcnt-1))
              b += ", ";
            fprintf(f,"%s%s",utf2tex(b).c_str(),(i < (maxcnt-1) ? "\n" : ""));
            teamstr += thutf82xhtml(b.c_str());
        }
    } else {
    	teamstr = "";
    }
    fprintf(f,"}\n");
}


double thmapstat::get_length() {
	double clen = 0.0;
	thmapstat_datamap::iterator ii;
	for(ii = this->data.begin(); ii != this->data.end(); ii++) {
		clen += ii->first.ptr->stat_length;
	}
	return clen;
}

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

double thmapstat::get_depth() {
	double z_top = 0.0, z_bot = 0.0;
	this->get_min_max_alt(z_bot, z_top);
	if (thisnan(z_top)) return thnan;
	return (z_bot - z_top);
}


void thmapstat::get_min_max_alt(double & min, double & max) {
    double z_top = 0.0, z_bot = 0.0;
    bool z_any = false;
    thmapstat_datamap::iterator ii;
    for(ii = this->data.begin(); ii != this->data.end(); ii++) {
    	if (ii->first.ptr->stat_st_state == 2) {
			check_z(ii->first.ptr->stat_st_top);
			check_z(ii->first.ptr->stat_st_bottom);
    	}
    }
    if (z_any) {
		min = z_bot;
		max = z_top;
    } else {
    	min = thnan;
    	max = thnan;
    }
}



void thmapstat::export_pdftex(FILE * f, class thlayout * layout, legenddata * ldata) {

  thbuffer b,c;
  unsigned long cnt;
  thmapstat_personmap::iterator pi;
  thmapstat_copyrightmap::iterator ci;
  thmapstat_datamap::iterator ii;
  std::unique_ptr<thmapstat_person_data[]> pd;
  std::unique_ptr<thmapstat_copyright_data[]> pdc;
  bool show_lengths;
  bool show_count;
  show_count = false;
  double clen, z_top, z_bot;
  c.guarantee(256);
  b.guarantee(256);

  if (!thcfg.reproducible_output)
    fprintf(f,"\\thversion={%s}\n",utf2tex(THVERSION));
  else
    fprintf(f,"\\thversion={%s}\n",utf2tex("..."));
  thdate dt;
  dt.reset_current();
  double cy = dt.get_start_year();
  dt.shour = -1;
  if (!thcfg.reproducible_output)
    fprintf(f,"\\currentdate={%s}\n",utf2tex(dt.get_str(TT_DATE_FMT_LOCALE)));
  else
    fprintf(f,"\\currentdate={%s}\n",utf2tex("1981-09-27"));
  fprintf(f,"\\northdir={%s}\n",layout->north == TT_LAYOUT_NORTH_GRID ? "grid" : "true");


  if (thcfg.outcs == TTCS_LOCAL) {
	  fprintf(f,"\\outcscode={local}\n");
	  fprintf(f,"\\outcsname={Local coordinate system}\n");
	  fprintf(f,"\\magdecl={N/A}\n");
	  fprintf(f,"\\gridconv={N/A}\n");
  } else {
	  fprintf(f,"\\outcscode={%s}\n",utf2tex(thcs_get_name(thcfg.outcs)));
	  fprintf(f,"\\outcsname={%s}\n",utf2tex(thcs_get_data(thcfg.outcs)->prjname));
	  double md;
	  thcfg.get_outcs_mag_decl(cy, md);
	  fprintf(f,"\\magdecl={%.2f}\n", md);
	  fprintf(f,"\\gridconv={%.2f}\n", thcfg.get_outcs_convergence());
  }
  

  clen = this->get_length();
  this->get_min_max_alt(z_bot, z_top);

  ldata->cavelength = "";
  ldata->cavelengthtitle = "";
  if (clen > 0) {
    //b = "";  
    //std::snprintf(b.get_buffer(),255,"%.0f",clen);
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

  if (!thisnan(z_top)) {
    //b = "";  
    //std::snprintf(b.get_buffer(),255,"%.0f",z_top-z_bot);
	b = layout->units.format_length(z_top - z_bot);
    b += "<thsp>";
    //b += thT("units m",layout->lang);
    b += layout->units.format_i18n_length_units();
    fprintf(f,"\\cavedepthtitle={%s}\n",utf2tex(thT("title cave depth",layout->lang)));
    fprintf(f,"\\cavedepth={%s}\n",utf2tex(b.get_buffer()));
    ldata->cavedepth = thutf82xhtml(b.get_buffer());
    ldata->cavedepthtitle = thT("title cave depth",layout->lang);
	b = layout->units.format_length(z_top);
    //b += "<thsp>";
    //b += layout->units.format_i18n_length_units();
    fprintf(f,"\\cavemaxz={%s}\n",utf2tex(b.get_buffer()));
	b = layout->units.format_length(z_bot);
    //b += "<thsp>";
    //b += layout->units.format_i18n_length_units();
    fprintf(f,"\\caveminz={%s}\n",utf2tex(b.get_buffer()));

  }
  
  show_lengths = (layout->explo_lens == TT_LAYOUT_LENSTAT_ON);
  
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
  
    thmapstat_print_team(f, this->discovered_by, "exploteam", layout->max_explos, (layout->explo_lens == TT_LAYOUT_LENSTAT_OFF) , ldata->exploteam, show_lengths, show_count, layout, c);
  }
  

  ldata->topodate = "";
  ldata->topoteam = "";
  ldata->topotitle = "";
  show_lengths = (layout->topo_lens == TT_LAYOUT_LENSTAT_ON);
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
  
    thmapstat_print_team(f, this->surveyed_by, "topoteam", layout->max_topos, (layout->topo_lens == TT_LAYOUT_LENSTAT_OFF), ldata->topoteam, show_lengths, show_count, layout, c);
  }

  ldata->cartodate = "";
  ldata->cartoteam = "";
  ldata->cartotitle = "";
  show_count = true;
  show_lengths = (layout->carto_lens == TT_LAYOUT_LENSTAT_ON);;
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
  
    thmapstat_print_team(f, this->drawn_by,"cartoteam", layout->max_cartos, (layout->carto_lens == TT_LAYOUT_LENSTAT_OFF), ldata->cartoteam, show_lengths, show_count, layout, c);
  }

  ldata->copyrights = "";
  show_lengths = (layout->copy_lens == TT_LAYOUT_LENSTAT_ON);;
  if ((this->copyright.size() > 0) && (layout->max_copys != 0)) {
    cnt = this->copyright.size();
    ldata->copyrights = "(c) ";
    thmapstat_print_copy(f, this->copyright, "copyrights", layout->max_copys, (layout->copy_lens == TT_LAYOUT_LENSTAT_OFF), ldata->copyrights, show_lengths, layout, c);
  }
  
}

