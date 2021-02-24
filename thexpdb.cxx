/**
 * @file thexpdb.cxx
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
 
#include "thexpdb.h"
#include "thexception.h"
#include "thscrap.h"
#include "thmap.h"
#include "thdatabase.h"
#include "thdb1d.h"
#include "th2ddataobject.h"
#include "thdata.h"
#include "thinit.h"
#include "thpoint.h"
#include "thline.h"
#include "extern/nlohmann/json.hpp"
#include "thsurvey.h"
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include "thchenc.h"
#include <map>
#ifndef THMSVC
#include <dirent.h>
#include <unistd.h>
#else
#include <direct.h>
#define mkdir _mkdir
#define S_ISDIR(v) (((v) | _S_IFDIR) != 0)
#endif
#include "thinfnan.h"

thexpdb::thexpdb() {
  this->format = TT_EXPDB_FMT_UNKNOWN;
  this->encoding = TT_UNKNOWN_ENCODING;
}


void thexpdb::parse_options(int & argx, int nargs, char ** args)
{
  int optid = thmatch_token(args[argx], thtt_expdb_opt);
  int optx = argx;
  switch (optid) {
    case TT_EXPDB_OPT_FORMAT:  
      argx++;
      if (argx >= nargs)
        ththrow("missing format -- \"{}\"",args[optx]);
      this->format = thmatch_token(args[argx], thtt_expdb_fmt);
      if (this->format == TT_EXPDB_FMT_UNKNOWN)
        ththrow("unknown format -- \"{}\"", args[argx]);
      argx++;
      break;
    case TT_EXPDB_OPT_ENCODING:  
      argx++;
      if (argx >= nargs)
        ththrow("missing encoding -- \"{}\"",args[optx]);
      this->encoding = thmatch_token(args[argx], thtt_encoding);
      if (this->encoding == TT_UNKNOWN_ENCODING)
        ththrow("unknown encoding -- \"{}\"", args[argx]);
      argx++;
      break;
    default:
      thexport::parse_options(argx, nargs, args);
      break;
  }
}

void thexpdb::dump_header(FILE * xf)
{
  thexport::dump_header(xf);
  //fprintf(xf,"export\tmodel");
}


void thexpdb::dump_body(FILE * xf)
{
  thexport::dump_body(xf);
  if (this->format != TT_EXPDB_FMT_UNKNOWN)
    fprintf(xf," -format %s", thmatch_string(this->format, thtt_expdb_fmt));
  if (this->encoding != TT_UNKNOWN_ENCODING)
    fprintf(xf," -encoding %s", thmatch_string(this->encoding, thtt_encoding));
}


void thexpdb::process_db(class thdatabase * dbp) 
{
  this->db = dbp;
  if (this->format == TT_EXPDB_FMT_UNKNOWN) {
	this->format == TT_EXPDB_FMT_QTH;
    thexp_set_ext_fmt(".sql", TT_EXPDB_FMT_SQL)
    thexp_set_ext_fmt(".csv", TT_EXPDB_FMT_CSV)
  }
  switch (this->format) {
    case TT_EXPDB_FMT_SQL:
      this->export_sql_file(dbp);
      break;
    case TT_EXPDB_FMT_CSV:
      this->export_csv_file(dbp);
      break;
    case TT_EXPDB_FMT_QTH:
      this->export_qth_file(dbp);
      break;
    default:
      ththrow("unknown database format (use .csv or .sql)");
  }
}





void thexpdb::export_sql_file(class thdatabase * dbp)
{
  
  int enc = this->encoding;
  if (enc == TT_UNKNOWN_ENCODING)
    enc = thini.get_encoding(THINIT_ENCODING_SQL);

  const char * fnm = this->get_output("cave.sql");  
  
  unsigned long i, ni;
  ni = dbp->db1d.get_tree_size();
  
#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif 
      
  FILE * sqlf;
  sqlf = fopen(fnm,"w");
  if (sqlf == NULL) {
    thwarning(("can't open %s for output",fnm))
    return;
  }
  this->register_output(fnm);
  
  thdb_object_list_type::iterator oi;
  thdataleg_list::iterator lei;
  thdata_team_set_type::iterator ti;
  std::map<thperson, long> personmap;
  std::map<thperson, long>::iterator personmapit;
  long personx, shotx; 
  //char * tmpstr;
  thdb1ds * st, * st2;
  thsurvey * sp;
  thdata * dp;
  thmap * mapp;
  thscrap * scrapp;
  int pass;
  double adx, ady, adz;
  unsigned survey_name = 1, survey_full_name = 1, centreline_title = 1,
    survey_title = 1, person_name = 1, person_surname = 1, // station_type = 4,
    station_name = 4; //station_comment = 4,


  // survey
#define IF_PRINTING if (pass == 1) 
#define ENCODESTR(str) {\
          if (str != NULL)  \
            thdecode(&(dbp->buff_enc),enc,str); \
          else \
            dbp->buff_enc = ""; \
          thdecode_sql(&(dbp->buff_tmp),dbp->buff_enc.get_buffer());}
#define ESTR (dbp->buff_tmp.get_buffer())

#define CHECK_STRLEN(var,str) {if (strlen(str) > var) var = strlen(str);}
#define INSERTPERSON \
            personmapit = personmap.find(*ti); \
            if (personmapit == personmap.end()) { \
              personmap[*ti] = ++personx; \
              ENCODESTR(ti->get_n1()); \
              IF_PRINTING fprintf(sqlf,"insert into PERSON values (%ld, %s, ", \
                personx, ESTR); \
              else CHECK_STRLEN(person_name,ESTR); \
              ENCODESTR(ti->get_n2()); \
              IF_PRINTING fprintf(sqlf,"%s);\n", ESTR); \
              else CHECK_STRLEN(person_surname, ESTR); \
              personmapit = personmap.find(*ti); \
            } 


  for (pass = 0; pass < 2; pass++) {
    oi = dbp->object_list.begin();
    personx = 0;    
    personmap.clear();
    shotx = 0;
    IF_PRINTING {
//      fprintf(sqlf,"begin transaction;\ncreate table SURVEY "
      fprintf(sqlf,"create table SURVEY "
        "(ID integer, PARENT_ID integer, NAME varchar(%d), "
        "FULL_NAME varchar(%d), TITLE varchar(%d));\n",
        survey_name, survey_full_name, survey_title);
        
      fprintf(sqlf,"create table CENTRELINE "
        "(ID integer, SURVEY_ID integer, TITLE varchar(%d), TOPO_DATE date, "
        "EXPLO_DATE date, LENGTH real, SURFACE_LENGTH real, "
        "DUPLICATE_LENGTH real);\n", centreline_title);
        
      fprintf(sqlf,"create table PERSON "
        "(ID integer, NAME varchar(%d), SURNAME varchar(%d));\n",
        person_name, person_surname);

      fprintf(sqlf,"create table EXPLO "
        "(PERSON_ID integer, CENTRELINE_ID integer);\n");
        
      fprintf(sqlf,"create table TOPO "
        "(PERSON_ID integer, CENTRELINE_ID integer);\n");

      fprintf(sqlf,"create table STATION "
        "(ID integer, NAME varchar(%d), SURVEY_ID integer, "
        "X real, Y real, Z real);\n",
        station_name);

      fprintf(sqlf,"create table STATION_FLAG "
        "(STATION_ID integer, FLAG char(3));\n");

      fprintf(sqlf,"create table SHOT "
        "(ID integer, FROM_ID integer, TO_ID integer, "
        "CENTRELINE_ID integer, "
        "LENGTH real, BEARING real, GRADIENT real, "
        "ADJ_LENGTH real, ADJ_BEARING real, ADJ_GRADIENT real, "
        "ERR_LENGTH real, ERR_BEARING real, ERR_GRADIENT real"
        ");\n");
        
      fprintf(sqlf,"create table SHOT_FLAG "
        "(SHOT_ID integer, FLAG char(3));\n");
              
	  fprintf(sqlf,"create table MAPS "
        "(ID integer, SURVEY_ID integer, NAME varchar(%d), TITLE varchar(%d), PROJID integer, LENGTH real, DEPTH real);\n",
        survey_name, survey_title);
        
	  fprintf(sqlf,"create table SCRAPS "
        "(ID integer, SURVEY_ID integer, NAME varchar(%d), PROJID integer, MAX_DISTORTION real, AVG_DISTORTION real);\n",
        survey_name);
        
	  fprintf(sqlf,"create table MAPITEMS "
        "(ID integer, TYPE integer, ITEMID integer);\n");
        
    } // END OF TABLE HEADERS
    
    // PRINTING DATA FROM OBJECTS
    while (oi != dbp->object_list.end()) {
      switch ((*oi)->get_class_id()) {

        case TT_SURVEY_CMD:
          sp = (thsurvey *)(*oi).get();
          ENCODESTR(sp->title);
          IF_PRINTING {
            fprintf(sqlf,"insert into SURVEY values "
              "(%ld, %ld, '%s', '%s', %s);\n ",
              sp->id, (sp->fsptr != NULL ? sp->fsptr->id : 0), 
              sp->name, sp->full_name, ESTR
              );
          } else {
            CHECK_STRLEN(survey_name,sp->name);
            CHECK_STRLEN(survey_full_name,sp->full_name);
            CHECK_STRLEN(survey_title,ESTR);
          }
          break;  // SURVEY

		case TT_SCRAP_CMD:
			scrapp = (thscrap *)(*oi).get();
			IF_PRINTING {
				fprintf(sqlf,"insert into SCRAPS values "
				  "(%ld, %ld, '%s', %d, %.5lf, %.5lf);\n ",
				  scrapp->id, (scrapp->fsptr != NULL ? scrapp->fsptr->id : 0), 
				  scrapp->name, scrapp->proj->id, scrapp->maxdist, scrapp->avdist);
			} else {
				CHECK_STRLEN(survey_name,scrapp->name);
			}
			break;

		case TT_MAP_CMD:
			mapp = (thmap *)(*oi).get();
			mapp->stat.scanmap(mapp);
			ENCODESTR(mapp->title);
			IF_PRINTING {
				fprintf(sqlf,"insert into MAPS values "
				  "(%ld, %ld, '%s', %s, %d, %.3lf, %.3lf);\n ",
				  mapp->id, (mapp->fsptr != NULL ? mapp->fsptr->id : 0), 
				  mapp->name, ESTR, mapp->projection_id, mapp->stat.get_length(), mapp->stat.get_depth()
				  );
				thdb2dmi * cit = mapp->first_item;
				while (cit != NULL) {
					fprintf(sqlf,"insert into MAPITEMS values "
					  "(%ld, %d, %ld);\n ",
					  mapp->id, cit->type, cit->object->id);						
					cit = cit->next_item;
				}
			} else {
				CHECK_STRLEN(survey_name,mapp->name);
				CHECK_STRLEN(survey_title,ESTR);
			}
			break;
          
        case TT_DATA_CMD:
          dp = (thdata *)(*oi).get();
          ENCODESTR(dp->title);
          IF_PRINTING {
            fprintf(sqlf,"insert into CENTRELINE values "
                         "(%ld, %ld, %s, ", dp->id, dp->fsptr->id, 
                         ESTR);
            fprintf(sqlf,"%s, ", dp->date.get_str(TT_DATE_FMT_SQL_SINGLE));
            fprintf(sqlf,"%s, ", dp->discovery_date.get_str(TT_DATE_FMT_SQL_SINGLE));
            fprintf(sqlf,"%.2f, %.2f, %.2f);\n ", dp->stat_length, 
              dp->stat_slength, dp->stat_dlength);
          } else {
            CHECK_STRLEN(centreline_title,ESTR);
          }
          
          for(ti = dp->team_set.begin(); ti != dp->team_set.end(); ti++) {
            INSERTPERSON;
            IF_PRINTING {
              fprintf(sqlf,"insert into TOPO values (%ld, %ld);\n ",
                personmapit->second, dp->id);
            }
          }
          
          for(ti = dp->discovery_team_set.begin(); ti != dp->discovery_team_set.end(); ti++) {
            INSERTPERSON;
            IF_PRINTING {
              fprintf(sqlf,"insert into EXPLO values (%ld, %ld);\n ",
                personmapit->second, dp->id);
            }
          }

          for(lei = dp->leg_list.begin(); lei != dp->leg_list.end(); lei++) {
            if (lei->is_valid) {
              IF_PRINTING {
                st = &(dbp->db1d.station_vec[lei->from.id - 1]);
                st2 = &(dbp->db1d.station_vec[lei->to.id - 1]);
                adx = st2->x - st->x;
                ady = st2->y - st->y;
                adz = st2->z - st->z;
                fprintf(sqlf,"insert into SHOT values ("
                  "%ld, %ld, %ld, %ld, %.3f, %.2f, %.2f, %.3f, %.2f, %.2f, %.3f, %.2f, %.2f);\n",
                  ++shotx, lei->from.id, lei->to.id, dp->id,
                  lei->total_length, lei->total_bearing, lei->total_gradient,
                  thdxyz2length(adx, ady, adz), thdxyz2bearing(adx, ady, adz), thdxyz2clino(adx, ady, adz),
                  thdxyz2length(adx - lei->total_dx, ady - lei->total_dy, adz - lei->total_dz), 
                  thdxyz2bearing(adx - lei->total_dx, ady - lei->total_dy, adz - lei->total_dz), 
                  thdxyz2clino(adx - lei->total_dx, ady - lei->total_dy, adz - lei->total_dz)
                  );
                if ((lei->flags & TT_LEGFLAG_SURFACE) != TT_LEGFLAG_NONE)
                  fprintf(sqlf,"insert into SHOT_FLAG values(%ld, 'srf');\n", shotx);
                if ((lei->flags & TT_LEGFLAG_DUPLICATE) != TT_LEGFLAG_NONE)
                  fprintf(sqlf,"insert into SHOT_FLAG values(%ld, 'dpl');\n", shotx);
              }
            }
          }

          break;  // DATA
          
      }
      oi++;
    }  // END PRINTING DATA FROM OBJECTS
    
    ni = dbp->db1d.station_vec.size();
    for(i = 0; i < ni; i++) {
      st = &(dbp->db1d.station_vec[i]);
      
      ENCODESTR(st->name);
      IF_PRINTING {
        fprintf(sqlf,"insert into STATION values "
          "(%ld, %s, %ld, %.2f, %.2f, %.2f);\n",
          (i+1), ESTR, st->survey->id,           
          st->x, st->y, st->z);
        if ((st->flags & TT_STATIONFLAG_ENTRANCE) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'ent');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_CONT) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'con');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_FIXED) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'fix');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_SPRING) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'spr');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_SINK) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'sin');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_DOLINE) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'dol');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_DIG) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'dig');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_AIRDRAUGHT) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'air');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_OVERHANG) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'ove');\n", (i+1));
        if ((st->flags & TT_STATIONFLAG_ARCH) != TT_STATIONFLAG_NONE)
          fprintf(sqlf,"insert into STATION_FLAG values(%ld, 'arc');\n", (i+1));
      } else {
            CHECK_STRLEN(station_name,ESTR);
      }
      
      //ENCODESTR(st->comment);
      //tmpstr = thmatch_string(st->mark,thtt_datamark);
      //IF_PRINTING {
      //  fprintf(sqlf,"'%s', %s);\n",
      //    tmpstr, ESTR);
      //} else {
      //      CHECK_STRLEN(station_type,tmpstr);
      //      CHECK_STRLEN(station_comment,ESTR);
      //}
      
    }

    
  }  // END OF PASS CYCLE
  //fprintf(sqlf,"commit transaction;\n");
  fclose(sqlf);
  
#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}


void thexpdb::export_csv_file(class thdatabase * dbp) {

  const char * fnm = this->get_output("cave.csv");

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif

  FILE * out;
  out = fopen(fnm, "w");
  if (out == NULL) {
    thwarning(("can't open %s for output", fnm))
    return;
  }
  this->register_output(fnm);

  thdb_object_list_type::iterator oi;
  thdataleg_list::iterator lei;
  thdataequate_list::iterator eqi;
  thdata * dp;

  oi = dbp->object_list.begin();

  fprintf(out, "From,To,Length(m),Azimuth(deg),Clino(deg)\n");

  while (oi != dbp->object_list.end()) {
    if ((*oi)->get_class_id() == TT_DATA_CMD) {
      dp = (thdata *) (*oi).get();

      for (lei = dp->leg_list.begin(); lei != dp->leg_list.end(); lei++) {
        if (lei->is_valid) {
          if ((lei->flags & TT_LEGFLAG_SPLAY) == TT_LEGFLAG_SPLAY)
            fprintf(out, "%s@%s,-,%.2f,%.2f,%.2f\n",
              lei->from.name, lei->from.psurvey->get_full_name(),
              lei->total_length, lei->total_bearing, lei->total_gradient);
          else
            fprintf(out, "%s@%s,%s@%s,%.2f,%.2f,%.2f\n",
              lei->from.name, lei->from.psurvey->get_full_name(), lei->to.name, lei->to.psurvey->get_full_name(),
              lei->total_length, lei->total_bearing, lei->total_gradient);
        }
      }

      // Export equate links between stations 
      int last_equate = 0;
      const long MAX_LEN = 500;
      char first_name[MAX_LEN];
      if (!dp->equate_list.empty()) {
        fprintf(out, "# Equated stations\n");
        for (eqi = dp->equate_list.begin(); eqi != dp->equate_list.end(); eqi++) {
          if (last_equate != eqi->eqid) {
            std::snprintf(first_name, MAX_LEN, "%s@%s", eqi->station.name, eqi->station.survey);
            last_equate = eqi->eqid;
          } else {
            if (last_equate != 0)
              fprintf(out, "%s,%s@%s\n", first_name, eqi->station.name, eqi->station.survey);
          }
        }
        fprintf(out, "# End equated stations\n");
      }
    }  // if TT_DATA_CMD
    oi++;
  }  // while

  fclose(out);

#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}

void thexpdb::export_qth_scrap(std::string fpath, thscrap * scr) {
	nlohmann::json drawing;
	nlohmann::json scrap;
	scrap["name"] = scr->name;
	scrap["objects"] = {};
	th2ddataobject * oo;
	for(oo = scr->fs2doptr; oo != NULL; oo = oo->nscrapoptr) {
		nlohmann::json obj;
		nlohmann::json lpts = {};
		thpoint * pt;
		thline * ln;
		thdb2dlp * lp;
		switch(oo->get_class_id()) {
		case TT_LINE_CMD:
			obj["class"] = "line";
			ln = (thline *) oo;
			obj["type"] = thmatch_string(ln->type, thtt_line_types);
			for(lp = ln->first_point; lp != NULL; lp = lp->nextlp) {
				nlohmann::json lpt;
				if (lp->cp1)
					lpt["cp1"] = {lp->cp1->x, lp->cp1->y};
				if (lp->cp2)
					lpt["cp2"] = {lp->cp2->x, lp->cp2->y};
				lpt["point"] = {lp->point->x, lp->point->y};
				lpts.push_back(lpt);
			}
			obj["points"] = lpts; 
			break;
		case TT_POINT_CMD:
			obj["class"] = "point";
			pt = (thpoint *) oo;
			obj["type"] = thmatch_string(pt->type, thtt_point_types);
			obj["point"] = {pt->point->x, pt->point->y};
			//obj["position"].push_back(pt->point->x);
			//obj["position"].push_back(pt->point->y);
			break;
		default:
			continue;
		}
		scrap["objects"].push_back(obj);
	}
	drawing["scraps"] = {};
	drawing["scraps"].push_back(scrap);
    std::ofstream o(fpath + std::string("/d_") + std::string(scr->name) + std::string(".qth"));
    o << std::setw(4) << drawing << std::endl;
}



void thexpdb::export_qth_survey(std::string fpath, thsurvey * srv) {
	
	// 1. create fpath folder
	// 2. for each object:
	// 3. create index JSON object
	// 4. traverse all objects
	// 5. save C_, D_ files, recursively call subsurveys
	// 6. save index file
	if (strlen(srv->name) > 0) {
		fpath += "/";
		fpath += srv->name;
	}
	nlohmann::json index;
	index["title"] = srv->title;
#ifdef THWIN32
    if (mkdir(fpath.c_str()) != 0) {
#else
    if (mkdir(fpath.c_str(),0046750) != 0) {
#endif

      struct 
#ifdef THMSVC
      _stat
#else
      stat 
#endif
      buf;
#ifdef THMSVC
      _stat
#else
      stat 
#endif
      (fpath.c_str(),&buf);
      if ((errno != EEXIST) || (!S_ISDIR(buf.st_mode))) {
        therror(("can't create output directory -- %s", fpath.c_str()));
      }
    }
    
    thdataobject * obj = srv->foptr;
    while (obj != NULL) {
    	switch (obj->get_class_id()) {
    	case TT_SURVEY_CMD:
    		this->export_qth_survey(fpath, (thsurvey *) obj);
    		break;
    	case TT_SCRAP_CMD:
    		this->export_qth_scrap(fpath, (thscrap *) obj);
    		break;
    	}
        obj = obj->nsptr;
    }
    
    std::ofstream o(fpath + std::string("/index.qth"));
    o << std::setw(4) << index << std::endl;
    
}


void thexpdb::export_qth_file(class thdatabase * dbp) {

  const char * fnm = this->get_output("cave");

#ifdef THDEBUG
  thprintf("\n\nwriting %s\n", fnm);
#else
  thprintf("writing %s ... ", fnm);
  thtext_inline = true;
#endif
  
  this->export_qth_survey(fnm, dbp->fsurveyptr);


#ifdef THDEBUG
#else
  thprintf("done\n");
  thtext_inline = false;
#endif
}
