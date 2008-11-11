/**
 * @file thexptable.cxx
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
 
#include "thexptable.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thdb1d.h"
#include "thdata.h"
#include "thexporter.h"
#include "thinit.h"
#include "thsurvey.h"
#include "thscrap.h"
#include "thdb1d.h"
#include "thpoint.h"
#include <stdio.h>
#include "thchenc.h"
#include <map>
#include "thinfnan.h"
#include "thcsdata.h"
#include "thproj.h"
#include "thconfig.h"

thexptable::thexptable() {
  this->format = TT_EXPTABLE_FMT_UNKNOWN;
  this->encoding = TT_UTF_8;
  this->expattr = true;
  this->exploc = false;
}


void thexptable::parse_options(int & argx, int nargs, char ** args)
{
  int sv;
  int optid = thmatch_token(args[argx], thtt_exptable_opt);
  int optx = argx;
  switch (optid) {
    case TT_EXPTABLE_OPT_ATTRIBUTES:
      argx++;
      if (argx >= nargs)
        ththrow(("missing attributes switch -- \"%s\"",args[optx]))
      sv = thmatch_token(args[argx], thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid attributes switch -- \"%s\"", args[argx]))
      this->expattr = (sv == TT_TRUE);
      argx++;
      break;
    case TT_EXPTABLE_OPT_LOCATION:
      argx++;
      if (argx >= nargs)
        ththrow(("missing location switch -- \"%s\"",args[optx]))
      sv = thmatch_token(args[argx], thtt_bool);
      if (sv == TT_UNKNOWN_BOOL)
        ththrow(("invalid location switch -- \"%s\"", args[argx]))
      this->exploc = (sv == TT_TRUE);
      argx++;
      break;
    case TT_EXPTABLE_OPT_FORMAT:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing format -- \"%s\"",args[optx]))
      this->format = thmatch_token(args[argx], thtt_exptable_fmt);
      if (this->format == TT_EXPTABLE_FMT_UNKNOWN)
        ththrow(("unknown format -- \"%s\"", args[argx]))
      argx++;
      break;
    case TT_EXPTABLE_OPT_ENCODING:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing encoding -- \"%s\"",args[optx]))
      this->encoding = thmatch_token(args[argx], thtt_encoding);
      if (this->encoding == TT_UNKNOWN_ENCODING)
        ththrow(("unknown encoding -- \"%s\"", args[argx]))
      argx++;
      break;
    default:
      thexport::parse_options(argx, nargs, args);
      break;
  }
}

void thexptable::dump_header(FILE * xf)
{
  thexport::dump_header(xf);
}


void thexptable::dump_body(FILE * xf)
{
  thexport::dump_body(xf);
  if (this->format != TT_EXPTABLE_FMT_UNKNOWN)
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_exptable_fmt));
  if (this->encoding != TT_UNKNOWN_ENCODING)
    fprintf(xf," -encoding %s", thmatch_string(this->encoding, thtt_encoding));
}




void thexptable::export_survey_entraces(thsurvey * survey)
{
  thdataobject * obj;
  thdb1ds * st;
  double tmpd;
  if (!survey->is_selected()) return;
  if (survey->stat.num_entrances == 0) return;
  // insert survey attributes
  if (survey->level > 1) {
    this->m_table.insert_object(NULL);          
    this->m_table.get_object()->m_tree_level = (size_t)(survey->level - 2);
    this->m_table.get_object()->m_tree_node_id = survey->get_reverse_full_name();
    this->m_table.insert_attribute("Title", ((strlen(survey->title) > 0) ? survey->title : survey->name));
    this->m_table.insert_attribute("Length",(long)(survey->stat.length + 0.5));
    if (survey->stat.station_top != NULL) {
      tmpd = survey->stat.station_top->z - survey->stat.station_bottom->z;
      if (tmpd > survey->stat.length) tmpd = survey->stat.length;
      this->m_table.insert_attribute("Depth",(long)(tmpd + 0.5));
    } else {
      this->m_table.insert_attribute("Depth","");
    }
    if (survey->stat.length_explored > 0.0) {
      this->m_table.insert_attribute("Explored", (long)(survey->stat.length_explored + 0.5));
    } else {
      this->m_table.insert_attribute("Explored", "");
    }
    if (exploc) {
      if ((survey->stat.num_entrances == 1) && (survey->entrance.id > 0)) {
        st = &(this->db->db1d.station_vec[survey->entrance.id - 1]);    
        this->m_table.insert_attribute("X",st->x);
        this->m_table.insert_attribute("Y",st->y);
      } else {
        this->m_table.insert_attribute("X","");
        this->m_table.insert_attribute("Y","");
      }
    }
    if (((survey->stat.num_entrances > 0) && ((!exploc) || (survey->stat.num_entrances == 1))) && (survey->entrance.id > 0)) {
      st = &(this->db->db1d.station_vec[survey->entrance.id - 1]);    
      this->m_table.insert_attribute("Altitude",st->z);
    } else {
      this->m_table.insert_attribute("Altitude","");
    }
    if (this->expattr) this->m_table.copy_attributes(this->db->attr.get_object(survey->id));
  }

  if (survey->entrance.id == 0) {
    for(obj = survey->foptr; obj != NULL; obj = obj->nsptr) {
      if (obj->get_class_id() == TT_SURVEY_CMD) {
        this->export_survey_entraces((thsurvey *) obj);
      }
    }
  }

  // insert entrances within this survey
  size_t nstat = this->db->db1d.station_vec.size(), i;
  for(i = 0; i < nstat; i++) {
    st = &(this->db->db1d.station_vec[i]);    
    if ((st->flags & TT_STATIONFLAG_ENTRANCE) != 0) {
      if (((exploc) && (survey->entrance.id > 0) && (survey->stat.num_entrances > 1) && (st->survey->is_in_survey(survey))) ||
        ((survey->entrance.id == 0) && (st->survey->id == survey->id))) {
          this->m_table.insert_object(NULL);          
          this->m_table.get_object()->m_tree_level = (size_t)(survey->level - 1);
          std::string * tmps = get_tmp_string();
          *tmps = survey->get_reverse_full_name();
          *tmps += ".";
          *tmps += st->name;
          this->m_table.get_object()->m_tree_node_id = tmps->c_str();
          this->m_table.insert_attribute("Title", st->comment);
          this->m_table.insert_attribute("Length","");
          this->m_table.insert_attribute("Depth","");
          if (st->explored > 0.0) {
            this->m_table.insert_attribute("Explored", (long)(st->explored + 0.5));
          } else {
            this->m_table.insert_attribute("Explored", "");
          }
          if (exploc) {
            this->m_table.insert_attribute("X",st->x);
            this->m_table.insert_attribute("Y",st->y);
          }
          this->m_table.insert_attribute("Altitude",st->z);
          if (this->expattr) this->m_table.copy_attributes(this->db->db1d.m_station_attr.get_object((long)i+1));
      }
    }
  }



}




void thexptable::process_db(class thdatabase * dbp) 
{
  this->db = dbp;

  if (this->format == TT_EXPTABLE_FMT_UNKNOWN) {
    this->format = TT_EXPTABLE_FMT_HTML;
    thexp_set_ext_fmt(".txt", TT_EXPTABLE_FMT_TXT)
    thexp_set_ext_fmt(".html", TT_EXPTABLE_FMT_HTML)
    thexp_set_ext_fmt(".htm", TT_EXPTABLE_FMT_HTML)
    thexp_set_ext_fmt(".dbf", TT_EXPTABLE_FMT_DBF)
    thexp_set_ext_fmt(".kml", TT_EXPTABLE_FMT_KML)
  }  
  const char * fname;
  switch (this->format) {
    case TT_EXPTABLE_FMT_DBF:
      fname = this->get_output("table.dbf");
      break;
    case TT_EXPTABLE_FMT_HTML:
      fname = this->get_output("table.html");
      break;
    case TT_EXPTABLE_FMT_KML:
      fname = this->get_output("table.kml");
      break;
    default:
      fname = this->get_output("table.txt");
  }
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fname);
#else
  thprintf("writing %s ... ", fname);
  thtext_inline = true;
#endif 

  const char * survey = NULL;
  thsurvey * srv;
  thdb1ds * st;
  thdb_object_list_type::iterator oi;
  thpoint * pt;
  double tmpd;

  switch (this->export_mode) {
    case TT_EXP_CAVELIST:
      this->export_survey_entraces(this->db->fsurveyptr);
      this->m_table.m_tree = true;
      break;
    case TT_EXP_CONTLIST:
      {
        // check all stations and points
        unsigned long nstat = (unsigned long)dbp->db1d.station_vec.size(),
          i;
        double lon, lat, alt;
        for(oi = this->db->object_list.begin(); oi != this->db->object_list.end(); oi++) {
          if ((*oi)->get_class_id() == TT_POINT_CMD) {
            pt = (thpoint*)(*oi);
            if ((pt->type == TT_POINT_TYPE_CONTINUATION) && (pt->text != NULL) && (pt->fsptr->is_selected())) {
              this->db->db2d.process_projection(pt->fscrapptr->proj);
              this->m_table.insert_object(NULL);
              this->m_table.insert_attribute("Comment",pt->text);
	            if (!thisnan(pt->xsize))
                      this->m_table.insert_attribute("Explored",pt->xsize);
	            else
                      this->m_table.insert_attribute("Explored",(const char *)NULL);
              if (pt->station_name.id != 0) {
                st = &(dbp->db1d.station_vec[pt->station_name.id - 1]);
              } else {
                st = pt->fscrapptr->get_nearest_station(pt->point);
              }
              if (st != NULL) {
                srv = st->survey;
              } else {
                srv = pt->fsptr;
              }
              if (strlen(srv->get_title()) > 0) {
                survey = st->survey->get_title();
              } else {
                survey = st->survey->get_full_name();
                if (strlen(survey) == 0)
                  survey = NULL;
              }
              this->m_table.insert_attribute("Survey", survey);
              this->m_table.insert_attribute("Station", st != NULL ? st->name : NULL);
              if (this->format == TT_EXPTABLE_FMT_KML) { 
                thcs2cs(thcsdata_table[thcfg.outcs].params, thcsdata_table[TTCS_LONG_LAT].params, 
                  pt->point->xt, pt->point->yt, pt->point->at, lon, lat, alt);
                this->m_table.insert_attribute("_LONGITUDE", lon / THPI * 180.0);
                this->m_table.insert_attribute("_LATITUDE",  lat / THPI * 180.0);
                this->m_table.insert_attribute("_ALTITUDE",  alt);
              }
              if (this->expattr) this->m_table.copy_attributes(this->db->attr.get_object(pt->id));
            }
          }
        }
        for(i = 0; i < nstat; i++) {
          st = &(dbp->db1d.station_vec[i]);
          if (((st->flags & TT_STATIONFLAG_CONT) != 0) && (st->comment != NULL) && (st->survey->is_selected())) {
            this->m_table.insert_object(NULL);
            this->m_table.insert_attribute("Comment",st->comment);
	          if (!thisnan(st->explored))
                    this->m_table.insert_attribute("Explored",st->explored);
	          else
                    this->m_table.insert_attribute("Explored",(const char *)NULL);
            if (strlen(st->survey->get_title()) > 0) {
              survey = st->survey->get_title();
            } else {
              survey = st->survey->get_full_name();
              if (strlen(survey) == 0)
                survey = NULL;
            }
            this->m_table.insert_attribute("Survey", survey);
            this->m_table.insert_attribute("Station", st->name);            
            if (this->format == TT_EXPTABLE_FMT_KML) { 
              thcs2cs(thcsdata_table[thcfg.outcs].params, thcsdata_table[TTCS_LONG_LAT].params, 
                st->x, st->y, st->z, lon, lat, alt);
              this->m_table.insert_attribute("_LONGITUDE", lon / THPI * 180.0);
              this->m_table.insert_attribute("_LATITUDE",  lat / THPI * 180.0);
              this->m_table.insert_attribute("_ALTITUDE",  alt);
            }
            if (this->expattr) this->m_table.copy_attributes(this->db->db1d.m_station_attr.get_object(i+1));
          }
        }        
      }
      break;
    case TT_EXP_SURVEYLIST:
      // todo add survey data
      //title
      //explored length
      //id
      //approx.
      //surface
      //duplicate
      for(oi = this->db->object_list.begin(); oi != this->db->object_list.end(); oi++) {
        if (((*oi)->get_class_id() == TT_SURVEY_CMD) && (strlen((*oi)->name) > 0)) {
          srv = (thsurvey*)(*oi);
          if (srv->is_selected()) {
            this->m_table.insert_object(NULL);          
            this->m_table.get_object()->m_tree_level = (size_t)(srv->level - 2);
            this->m_table.get_object()->m_tree_node_id = srv->get_reverse_full_name();
            this->m_table.insert_attribute("Title", ((strlen(srv->title) > 0) ? srv->title : srv->name));
            this->m_table.insert_attribute("Length",(long)(srv->stat.length + 0.5));
            if (srv->stat.station_top != NULL) {
              tmpd = srv->stat.station_top->z - srv->stat.station_bottom->z;
              if (tmpd > srv->stat.length) tmpd = srv->stat.length;
              this->m_table.insert_attribute("Depth",(long)(tmpd + 0.5));
            } else {
              this->m_table.insert_attribute("Depth",0.0);
            }
            this->m_table.insert_attribute("Explored", (long)(srv->stat.length_explored + 0.5));
            this->m_table.insert_attribute("Approx.", (long)(srv->stat.length_approx + 0.5));
            this->m_table.insert_attribute("Duplicate", (long)(srv->stat.length_duplicate + 0.5));
            this->m_table.insert_attribute("Surface", (long)(srv->stat.length_surface + 0.5));
            this->m_table.insert_attribute("Shots", (long)(srv->stat.num_shots));
            this->m_table.insert_attribute("Stations", (long)(srv->stat.num_stations));
            //this->m_table.insert_attribute("ID", srv->get_full_name());
            if (this->expattr) this->m_table.copy_attributes(this->db->attr.get_object(srv->id));
          }
        }
      }
      this->m_table.m_tree = true;
      break;
  }


  switch (this->format) {
    case TT_EXPTABLE_FMT_TXT:
      this->m_table.export_txt(fname, this->encoding);
      break;
    case TT_EXPTABLE_FMT_HTML:
      this->m_table.export_html(fname, this->encoding);
      break;
    case TT_EXPTABLE_FMT_DBF:
      this->m_table.export_dbf(fname, this->encoding);
      break;
    case TT_EXPTABLE_FMT_KML:
      this->m_table.export_kml(fname);
      break;
  }

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
 
}


std::string * thexptable::get_tmp_string() {
  return &(* this->m_str_list.insert(this->m_str_list.end(),std::string("")));
}



