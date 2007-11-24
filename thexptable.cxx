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
}


void thexptable::parse_options(int & argx, int nargs, char ** args)
{
  int optid = thmatch_token(args[argx], thtt_exptable_opt);
  int optx = argx;
  switch (optid) {
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

void thexptable::process_db(class thdatabase * dbp) 
{
  this->db = dbp;

  if (this->format == TT_EXPTABLE_FMT_UNKNOWN) {
    this->format = TT_EXPTABLE_FMT_TXT;
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

  switch (this->export_mode) {
    case TT_EXP_CONTLIST:
      {
        // check all stations and points
        unsigned long nstat = (unsigned long)dbp->db1d.station_vec.size(),
          i;
        const char * survey = NULL;
        thsurvey * srv;
        thdb1ds * st;
        thdb_object_list_type::iterator oi;
        thpoint * pt;
        double lon, lat, alt;
        for(oi = this->db->object_list.begin(); oi != this->db->object_list.end(); oi++) {
          if ((*oi)->get_class_id() == TT_POINT_CMD) {
            pt = (thpoint*)(*oi);
            if ((pt->type == TT_POINT_TYPE_CONTINUATION) && ((pt->text != NULL) || (pt->code != NULL)) && (pt->fsptr->is_selected())) {
              this->db->db2d.process_projection(pt->fscrapptr->proj);
              this->m_table.insert_object(NULL);
              this->m_table.insert_attribute("Code",pt->code);
              this->m_table.insert_attribute("Comment",pt->text);
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
         
            }
          }
        }
        for(i = 0; i < nstat; i++) {
          st = &(dbp->db1d.station_vec[i]);
          if (((st->flags & TT_STATIONFLAG_CONT) != 0) && ((st->code != NULL) || (st->comment != NULL)) && (st->survey->is_selected())) {
            this->m_table.insert_object(NULL);
            this->m_table.insert_attribute("Code",st->code);
            this->m_table.insert_attribute("Comment",st->comment);
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
          }
        }        
      }
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




