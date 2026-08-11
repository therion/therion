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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "thexpdb.h"
#include "thexception.h"
#include "thscrap.h"
#include "thmap.h"
#include "thdatabase.h"
#include "thdb1d.h"
#include "thdata.h"
#include "thinit.h"
#include "thsurvey.h"
#include <stdio.h>
#include "thchenc.h"
#include <map>
#include "thinfnan.h"
#include "thparse.h"
#include "therion.h"

#include <fmt/format.h>

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
        throw thexception(fmt::format("missing format -- \"{}\"",args[optx]));
      this->format = thmatch_token(args[argx], thtt_expdb_fmt);
      if (this->format == TT_EXPDB_FMT_UNKNOWN)
        throw thexception(fmt::format("unknown format -- \"{}\"", args[argx]));
      argx++;
      break;
    case TT_EXPDB_OPT_ENCODING:  
      argx++;
      if (argx >= nargs)
        throw thexception(fmt::format("missing encoding -- \"{}\"",args[optx]));
      this->encoding = thmatch_token(args[argx], thtt_encoding);
      if (this->encoding == TT_UNKNOWN_ENCODING)
        throw thexception(fmt::format("unknown encoding -- \"{}\"", args[argx]));
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
    set_format_by_extension(".sql", TT_EXPDB_FMT_SQL);
    set_format_by_extension(".csv", TT_EXPDB_FMT_CSV);
  }
  switch (this->format) {
    case TT_EXPDB_FMT_SQL:
      this->export_sql_file(dbp);
      break;
    case TT_EXPDB_FMT_CSV:
      this->export_csv_file(dbp);
      break;
    default:
      throw thexception("unknown database format (use .csv or .sql)");
  }
}


double sql_double(double x) {
	if (thisnan(x)) return 0.0;
	if (thisinf(x) == 1) return 9e99;
	if (thisinf(x) == -1) return -9e99;
	return x;
}



void thexpdb::export_sql_file(class thdatabase * dbp)
{
  const int enc = (this->encoding != TT_UNKNOWN_ENCODING) ? this->encoding
                                                          : thini.get_encoding(THINIT_ENCODING_SQL);

  const char * fnm = this->get_output("cave.sql");  
  
  unsigned long i, ni;
  ni = dbp->db1d.get_tree_size();
  
#ifdef THDEBUG
  thprint(fmt::format("\n\nwriting {}\n", fnm));
#else
  thprint(fmt::format("writing {} ... ", fnm));
  thtext_inline = true;
#endif 
      
  FILE * sqlf;
  sqlf = fopen(fnm,"wb");
  if (sqlf == NULL) {
    thwarning(fmt::format("can't open {} for output",fnm));
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
  double adx, ady, adz;
  unsigned survey_name = 1, survey_full_name = 1, centreline_title = 1,
    survey_title = 1, person_name = 1, person_surname = 1, // station_type = 4,
    station_name = 4; //station_comment = 4,


  // survey
  const auto encodestr = [enc, dbp](const char* str) {
    if (str)
      thdecode(&(dbp->buff_enc),enc,str);
    else
      dbp->buff_enc = "";
    return thdecode_sql(dbp->buff_enc);
  };

  const auto check_strlen = [](unsigned int& var, std::string_view str) {
    if (str.length() > var)
      var = str.length();
  };

  const auto insert_person = [&](const thperson& person, const bool printing) {
    personmapit = personmap.find(person);
    if (personmapit == personmap.end()) {
      personmap[person] = ++personx;
      auto str = encodestr(person.get_n1());
      if (printing) fprintf(sqlf,"insert into PERSON values (%ld, %s, ",
        personx, str.c_str());
      else check_strlen(person_name, str);
      str = encodestr(person.get_n2());
      if (printing) fprintf(sqlf,"%s);\n", str.c_str());
      else check_strlen(person_surname, str);
      personmapit = personmap.find(person);
    } 
  };

  for (int pass = 0; pass < 2; pass++) {
    const bool printing = pass == 1;
    oi = dbp->object_list.begin();
    personx = 0;    
    personmap.clear();
    shotx = 0;
    if (printing) {
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
        {
          sp = dynamic_cast<thsurvey*>(oi->get());
          const auto str = encodestr(sp->title);
          if (printing) {
            fprintf(sqlf,"insert into SURVEY values "
              "(%ld, %ld, '%s', '%s', %s);\n ",
              sp->id, (sp->fsptr != NULL ? sp->fsptr->id : 0), 
              sp->name, sp->full_name, str.c_str()
              );
          } else {
            check_strlen(survey_name,sp->name);
            check_strlen(survey_full_name,sp->full_name);
            check_strlen(survey_title, str);
          }
          break;  // SURVEY
        }

		case TT_SCRAP_CMD:
			scrapp = dynamic_cast<thscrap*>(oi->get());
			if (printing) {
				fprintf(sqlf,"insert into SCRAPS values "
				  "(%ld, %ld, '%s', %d, %.5lf, %.5lf);\n ",
				  scrapp->id, (scrapp->fsptr != NULL ? scrapp->fsptr->id : 0), 
				  scrapp->name, scrapp->proj->id, sql_double(scrapp->maxdist), sql_double(scrapp->avdist));
			} else {
				check_strlen(survey_name,scrapp->name);
			}
			break;

		case TT_MAP_CMD:
    {
			mapp = dynamic_cast<thmap*>(oi->get());
			mapp->stat.scanmap(mapp);
			const auto str = encodestr(mapp->title);
			if (printing) {
				fprintf(sqlf,"insert into MAPS values "
				  "(%ld, %ld, '%s', %s, %d, %.3lf, %.3lf);\n ",
				  mapp->id, (mapp->fsptr != NULL ? mapp->fsptr->id : 0), 
				  mapp->name, str.c_str(), mapp->projection_id, sql_double(mapp->stat.get_length()), sql_double(mapp->stat.get_depth())
				  );
				thdb2dmi * cit = mapp->first_item;
				while (cit != NULL) {
					fprintf(sqlf,"insert into MAPITEMS values "
					  "(%ld, %d, %ld);\n ",
					  mapp->id, cit->type, cit->object->id);						
					cit = cit->next_item;
				}
			} else {
				check_strlen(survey_name,mapp->name);
				check_strlen(survey_title, str);
			}
			break;
    }
          
        case TT_DATA_CMD:
        {
          dp = dynamic_cast<thdata*>(oi->get());
          const auto str = encodestr(dp->title);
          if (printing) {
            fprintf(sqlf,"insert into CENTRELINE values "
                         "(%ld, %ld, %s, ", dp->id, dp->fsptr->id, 
                         str.c_str());
            fprintf(sqlf,"%s, ", dp->date.get_str(TT_DATE_FMT_SQL_SINGLE));
            fprintf(sqlf,"%s, ", dp->discovery_date.get_str(TT_DATE_FMT_SQL_SINGLE));
            fprintf(sqlf,"%.2f, %.2f, %.2f);\n ", dp->stat_length, 
              dp->stat_slength, dp->stat_dlength);
          } else {
            check_strlen(centreline_title, str);
          }
          
          for(ti = dp->team_set.begin(); ti != dp->team_set.end(); ti++) {
            insert_person(*ti, printing);
            if (printing) {
              fprintf(sqlf,"insert into TOPO values (%ld, %ld);\n ",
                personmapit->second, dp->id);
            }
          }
          
          for(ti = dp->discovery_team_set.begin(); ti != dp->discovery_team_set.end(); ti++) {
            insert_person(*ti, printing);
            if (printing) {
              fprintf(sqlf,"insert into EXPLO values (%ld, %ld);\n ",
                personmapit->second, dp->id);
            }
          }

          for(lei = dp->leg_list.begin(); lei != dp->leg_list.end(); lei++) {
            if (lei->is_valid) {
              if (printing) {
                st = &(dbp->db1d.station_vec[lei->from.id - 1]);
                st2 = &(dbp->db1d.station_vec[lei->to.id - 1]);
                adx = st2->x - st->x;
                ady = st2->y - st->y;
                adz = st2->z - st->z;
                fprintf(sqlf,"insert into SHOT values ("
                  "%ld, %ld, %ld, %ld, %.3f, %.2f, %.2f, %.3f, %.2f, %.2f, %.3f, %.2f, %.2f);\n",
                  ++shotx, lei->from.id, lei->to.id, dp->id,
                  sql_double(lei->total_length), sql_double(lei->total_bearing), sql_double(lei->total_gradient),
				  sql_double(thdxyz2length(adx, ady, adz)), sql_double(thdxyz2bearing(adx, ady)), sql_double(thdxyz2clino(adx, ady, adz)),
				  sql_double(thdxyz2length(adx - lei->total_dx, ady - lei->total_dy, adz - lei->total_dz)),
				  sql_double(thdxyz2bearing(adx - lei->total_dx, ady - lei->total_dy)),
				  sql_double(thdxyz2clino(adx - lei->total_dx, ady - lei->total_dy, adz - lei->total_dz))
                  );
                if ((lei->flags & TT_LEGFLAG_SURFACE) != TT_LEGFLAG_NONE)
                  fprintf(sqlf,"insert into SHOT_FLAG values(%ld, 'srf');\n", shotx);
                if ((lei->flags & TT_LEGFLAG_DUPLICATE) != TT_LEGFLAG_NONE)
                  fprintf(sqlf,"insert into SHOT_FLAG values(%ld, 'dpl');\n", shotx);
                if ((lei->flags & TT_LEGFLAG_APPROXIMATE) != TT_LEGFLAG_NONE)
                  fprintf(sqlf,"insert into SHOT_FLAG values(%ld, 'apx');\n", shotx);
                if ((lei->flags & TT_LEGFLAG_SPLAY) != TT_LEGFLAG_NONE)
                  fprintf(sqlf,"insert into SHOT_FLAG values(%ld, 'spl');\n", shotx);
              }
            }
          }

          break;  // DATA
        }
          
      }
      oi++;
    }  // END PRINTING DATA FROM OBJECTS
    
    ni = dbp->db1d.station_vec.size();
    for(i = 0; i < ni; i++) {
      st = &(dbp->db1d.station_vec[i]);
      
      const auto str = encodestr(st->name);
      if (printing) {
        fprintf(sqlf,"insert into STATION values "
          "(%ld, %s, %ld, %.2f, %.2f, %.2f);\n",
          (i+1), str.c_str(), st->survey->id,           
		  sql_double(st->x), sql_double(st->y), sql_double(st->z));
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
            check_strlen(station_name, str);
      }
    }

    
  }  // END OF PASS CYCLE
  //fprintf(sqlf,"commit transaction;\n");
  fclose(sqlf);
  
#ifdef THDEBUG
#else
  thprint("done\n");
  thtext_inline = false;
#endif
}


void thexpdb::export_csv_file(class thdatabase * dbp) {

  const char * fnm = this->get_output("cave.csv");

#ifdef THDEBUG
  thprint(fmt::format("\n\nwriting {}\n", fnm));
#else
  thprint(fmt::format("writing {} ... ", fnm));
  thtext_inline = true;
#endif

  FILE * out;
  out = fopen(fnm, "wb");
  if (out == NULL) {
    thwarning(fmt::format("can't open {} for output", fnm));
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
      dp = dynamic_cast<thdata*>(oi->get());

      for (lei = dp->leg_list.begin(); lei != dp->leg_list.end(); lei++) {
        if (lei->is_valid) {
          bool is_splay = (lei->flags & TT_LEGFLAG_SPLAY) == TT_LEGFLAG_SPLAY;
          fprintf(out, "%s,%s,%.2f,%.2f,%.2f\n",
              lei->from.print_full_name().c_str(),
              is_splay ? "-" : lei->to.print_full_name().c_str(),
              lei->total_length, lei->total_bearing, lei->total_gradient);
        }
      }

      // Export equate links between stations 
      int last_equate = 0;
      std::string first_name;
      if (!dp->equate_list.empty()) {
        fprintf(out, "# Equated stations\n");
        for (eqi = dp->equate_list.begin(); eqi != dp->equate_list.end(); eqi++) {
          if (last_equate != eqi->eqid) {
            first_name = eqi->station.print_full_name();
            last_equate = eqi->eqid;
          } else {
            if (last_equate != 0)
              fprintf(out, "%s,%s\n", first_name.c_str(), eqi->station.print_full_name().c_str());
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
  thprint("done\n");
  thtext_inline = false;
#endif
}
