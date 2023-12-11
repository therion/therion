/**
 * @file thlookup.cxx
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
 
#include "thlookup.h"
#include "thexception.h"
#include "thchenc.h"
#include "thparse.h"
#include "thinfnan.h"
#include "thpdfdata.h"
#include "thlang.h"
#include "thscrap.h"
#include "thmap.h"
#include "thlayout.h"
#include "thtexfonts.h"
#include "thdatabase.h"
#include <string.h>


thlookup::thlookup()
{
  this->m_type = TT_LAYOUT_CCRIT_UNKNOWN;
  this->m_ascending = true;
  this->m_intervals = false;
  this->m_autoIntervals = false;
  this->m_title = "";
  this->m_depth_altitude = 0;
}


thlookup::~thlookup()
{
}


int thlookup::get_class_id() 
{
  return TT_LOOKUP_CMD;
}


bool thlookup::is(int class_id)
{
  if (class_id == TT_LOOKUP_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thlookup::get_cmd_nargs() 
{
  return 1;
}


const char * thlookup::get_cmd_end()
{
  return "endlookup";
}


const char * thlookup::get_cmd_name()
{
  // insert command name here
  return "lookup";
}


thcmd_option_desc thlookup::get_default_cod(int id) {
  return thcmd_option_desc(id);
}


thcmd_option_desc thlookup::get_cmd_option_desc(const char * opts)
{
  int id = thmatch_token(opts, thtt_lookup_opt);
  if (id == TT_LOOKUP_UNKNOWN) 
    return thdataobject::get_cmd_option_desc(opts);
  else 
    return this->get_default_cod(id);
}


void thlookup::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{

  thlookup_table_row tr;
  const char * lkpindex;
  const char * lkpnname;
  char * tmpa[1];
  thbuffer tmpb;

  thcmd_option_desc defcod = this->get_default_cod(cod.id);
  switch (cod.id) {
    case TT_DATAOBJECT_AUTHOR:
    case TT_DATAOBJECT_COPYRIGHT:
      defcod.nargs = 2;
      [[fallthrough]];
    default:
      if (cod.nargs > defcod.nargs)
        ththrow("too many arguments -- {}", args[defcod.nargs]);
  }
  
  switch (cod.id) {

    case 0:
      // add table row
      tr.parse(this, args[0]);
      m_table.push_back(tr);
      break;
    
	case TT_LOOKUP_TITLE:
      if (strlen(args[0]) > 0) {
        thencode(&(this->db->buff_enc), args[0], argenc);
        this->m_title = this->db->strstore(this->db->buff_enc.get_buffer());
      } else
        this->m_title = "";
      break;
    
    case 1:
      cod.id = TT_DATAOBJECT_NAME;
      thlookup_parse_reference(args[0], &(this->m_type), &lkpindex, &lkpnname);
      if (this->m_type == TT_LAYOUT_CCRIT_UNKNOWN)
        ththrow("invalid coloring criteria -- {}", args[0]);
      tmpb = lkpnname;
      tmpa[0] = tmpb.get_buffer();
      args = tmpa;
      [[fallthrough]];
    default:
      thdataobject::set(cod, args, argenc, indataline);
      break;
  }
}


int thlookup::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}

void thlookup_table_row::parse(class thlookup * lkp, char * args) {
  thmbuffer mbf, mbf2;
  int sv;
  char * cpar;
  thsplit_args(&mbf, args);
  if ((mbf.get_size() > 0) && (strlen(mbf.get_buffer()[0]) > 0)) {
    switch (lkp->m_type) {
      case TT_LAYOUT_CCRIT_SCRAP:
      case TT_LAYOUT_CCRIT_SURVEY:
      case TT_LAYOUT_CCRIT_MAP:
        this->m_valueString = thdb.strstore(mbf.get_buffer()[0]);
        if ((strcmp(this->m_valueString,"-") == 0) || (strcmp(this->m_valueString,".") == 0))
        	this->m_valueString = "";
        lkp->m_intervals = true;
        break;
      default:
        thsplit_args(&mbf2, mbf.get_buffer()[0]);
        if (mbf2.get_size() > 2)
          ththrow("too many values -- {}", mbf.get_buffer()[0]);
        if (mbf2.get_size() > 0) {
          cpar = mbf2.get_buffer()[0];
          if (mbf2.get_size() > 1) {
            // parse from value
            thparse_length(sv, this->m_valueDblFrom, cpar);
            if ((lkp->m_type == TT_LAYOUT_CCRIT_TOPODATE) || (lkp->m_type == TT_LAYOUT_CCRIT_EXPLODATE)) {
              if (sv != TT_SV_NAN) {
                this->m_valueDateFrom.parse(cpar);
                this->m_valueDblFrom = this->m_valueDateFrom.get_start_year();
              }
            } else {
              if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
                ththrow("invalid number -- {}", cpar);
            }
            cpar = mbf2.get_buffer()[1];
            lkp->m_intervals = true;
          }
          // parse to value
          thparse_length(sv, this->m_valueDbl, cpar);
          if ((lkp->m_type == TT_LAYOUT_CCRIT_TOPODATE) || (lkp->m_type == TT_LAYOUT_CCRIT_EXPLODATE)) {
            if (sv != TT_SV_NAN) {
              this->m_valueDate.parse(cpar);
              this->m_valueDbl = this->m_valueDate.get_start_year();
            }
          } else {
            if ((sv != TT_SV_NUMBER) && (sv != TT_SV_NAN))
              ththrow("invalid number -- {}", cpar);
          }
        }
    }
  }
  if ((mbf.get_size() > 1) && (strlen(mbf.get_buffer()[1]) > 0)) {
    // parse color
    this->m_color.parse(mbf.get_buffer()[1]);
  }
  if ((mbf.get_size() > 2) && (strlen(mbf.get_buffer()[2]) > 0)) {
    // parse label
    this->m_label = thdb.strstore(mbf.get_buffer()[2]);
  }
  if (mbf.get_size() > 3)
    ththrow("too many arguments -- {}", args);
}

void thlookup::postprocess_object_references() {
  if (this->m_table.size()  > 0) {
    thlookup_table_row * tr;
    thlookup_table_list::iterator tli;
    thobjectname on;
    thbuffer tmp;
    for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
      tr = &(*tli);
      switch (this->m_type) {
        case TT_LAYOUT_CCRIT_MAP:
        case TT_LAYOUT_CCRIT_SCRAP:
        case TT_LAYOUT_CCRIT_SURVEY:
          if ((tr->m_ref == NULL) && (strlen(tr->m_valueString) > 0)) {
            tmp = tr->m_valueString;
            thparse_objectname(on, &(thdb.buff_strings), tmp.get_buffer(), NULL);
            tr->m_ref = thdb.get_object(on, NULL);
            if (tr->m_ref == NULL) {
              thwarning(("invalid reference -- %s", tr->m_valueString));
            } else {
              if ((this->m_type == TT_LAYOUT_CCRIT_MAP) && (tr->m_ref->get_class_id() != TT_MAP_CMD))
                ththrow("not a map reference -- {}", tr->m_valueString);
              if ((this->m_type == TT_LAYOUT_CCRIT_SCRAP) && (tr->m_ref->get_class_id() != TT_SCRAP_CMD))
                ththrow("not a scrap reference -- {}", tr->m_valueString);
              if ((this->m_type == TT_LAYOUT_CCRIT_SURVEY) && (tr->m_ref->get_class_id() != TT_SURVEY_CMD))
                ththrow("not a survey reference -- {}", tr->m_valueString);
            }
          }
          break;
      }
    }
  }
}


bool scrap_in_map(thscrap * s, thmap * m) {
  thdb2dmi * mi;
  mi = m->first_item;
  while(mi != NULL) {
    if (mi->type == TT_MAPITEM_NORMAL) {
      if (m->is_basic) {
        if (s->id == mi->object->id) return true;
      } else {
        if (scrap_in_map(s, (thmap *) mi->object)) return true;
      }
    }
    mi = mi->next_item;
  }
  return false;
}


thlayout_color thlookup::value2clr(double sval) {
    double ratio;
    thlookup_table_list::iterator tli, ptli, ntli;
    thlayout_color clr;
    if ((this->m_type) == TT_LAYOUT_CCRIT_DEPTH) sval -= this->m_depth_altitude;
	if (this->m_intervals) {
	  for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
		if (thisnan(tli->m_valueDblFrom) && thisnan(tli->m_valueDbl)) {
		  clr = tli->m_color;
		}
		else if (!thisnan(sval)) {
		  if (this->m_ascending) {
			if ((thisnan(tli->m_valueDblFrom) || (tli->m_valueDblFrom < sval)) && (thisnan(tli->m_valueDbl) || (tli->m_valueDbl >= sval))) {
			  clr = tli->m_color;
			  break;
			}
		  } else {
			if ((thisnan(tli->m_valueDblFrom) || (tli->m_valueDblFrom > sval)) && (thisnan(tli->m_valueDbl) || (tli->m_valueDbl <= sval))) {
			  clr = tli->m_color;
			  break;
			}
		  }
		}
	  }
	} else {
	  ptli = this->m_table.end();
	  for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
		if (thisnan(tli->m_valueDbl) && thisnan(sval)) {
		  clr = tli->m_color;
		  break;
		} else if ((!thisnan(sval)) && (!thisnan(tli->m_valueDbl)))  {
		  ntli = tli;
		  ntli++;
		  if (this->m_ascending) {
			if (sval <= tli->m_valueDbl) {
			  clr = tli->m_color;
			  if ((ptli != this->m_table.end()) && (!thisnan(ptli->m_valueDbl)) && (ptli->m_valueDbl < sval)) {
				// interpolate value
				ratio = (sval - ptli->m_valueDbl) / (tli->m_valueDbl - ptli->m_valueDbl);
				clr = tli->m_color;
				clr.mix_with_color(ratio, ptli->m_color);
			  }
			  break;
			} else {
			  clr = tli->m_color;
			}
		  } else {
			if (sval >= tli->m_valueDbl) {
			  clr = tli->m_color;
			  if ((ptli != this->m_table.end()) && (!thisnan(ptli->m_valueDbl)) && (ptli->m_valueDbl > sval)) {
				// interpolate value
				ratio = (sval - tli->m_valueDbl) / (ptli->m_valueDbl - tli->m_valueDbl);
				clr = tli->m_color;
				clr.mix_with_color(ratio, ptli->m_color);
			  }
			  break;
			} else {
			  clr = tli->m_color;
			}
		  }
		}
		ptli = tli;
	  }
	}	
	return clr;
}


void thlookup::color_scrap(thscrap * s) {
  // set color of the scrap
  double sval;
  thlookup_table_list::iterator tli, ptli, ntli;
  thlayout_color clr;
  switch (this->m_type) {
    case TT_LAYOUT_CCRIT_MAP:
      for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
        if (strlen(tli->m_valueString) == 0) {
          clr = tli->m_color;
        }
        if (tli->m_ref == NULL) continue;
        // if map contains scrap - set color and break
        // thprintf("%s", s->name);
        if (scrap_in_map(s, (thmap *) tli->m_ref)) {
          clr = tli->m_color;
          break;
        }
      }
      break;
    case TT_LAYOUT_CCRIT_SCRAP:
      for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
        if (strlen(tli->m_valueString) == 0) {
          clr = tli->m_color;
        }
        if (tli->m_ref == NULL) continue;
        if (tli->m_ref->id == s->id) {
          clr = tli->m_color;
          break;
        }
      }
      break;
    default:
      {
        thmapstat ms;
        ms.adddata(&(s->adata));
        sval = s->a;
        if ((this->m_type) == TT_LAYOUT_CCRIT_DEPTH) sval -= this->m_depth_altitude;
        if (this->m_type == TT_LAYOUT_CCRIT_EXPLODATE) {
          sval = ms.discovered_date.get_start_year();
          if (sval < 0) sval = thnan;
        }
        if (this->m_type == TT_LAYOUT_CCRIT_TOPODATE) {
          sval = ms.surveyed_date.get_start_year();
          if (sval < 0) sval = thnan;
        }
        clr = this->value2clr(sval);
      }
  }
  if (clr.is_defined()) {
	s->clr = clr;
  }
}


void thlookup::export_color_legend(thlayout * layout) {
  layout->m_lookup = this;
  if (layout->color_legend != TT_LAYOUT_COLORLEGEND_OFF) {
    COLORLEGENDLIST.clear();
    thlookup_table_list::iterator tli;
    colorlegendrecord clrec;
    std::string title;
    for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
      tli->m_color.set_color(layout->color_model, clrec.col_legend);
      if (strlen(tli->m_label) > 0) {
        clrec.texname = ths2tex(tli->m_label, layout->lang);
        clrec.name = ths2txt(tli->m_label, layout->lang);

      } else {
        switch (this->m_type) {
          case TT_LAYOUT_CCRIT_MAP:
          case TT_LAYOUT_CCRIT_SCRAP:
          case TT_LAYOUT_CCRIT_SURVEY:
        	if (tli->m_ref) {
        		title = ths2txt(tli->m_ref->title, layout->lang);
				if (title.empty())
				  title = tli->m_ref->name;
				else
				  title = tli->m_ref->title;
        	} else
				title = "";
            clrec.texname = ths2tex(title, layout->lang);
            clrec.name = ths2txt(title, layout->lang);
            break;
          case TT_LAYOUT_CCRIT_EXPLODATE:
          case TT_LAYOUT_CCRIT_TOPODATE:
            clrec.name = "";
            if (this->m_intervals) {
              if (!thisnan(tli->m_valueDblFrom)) {
                thdate tmpdate;
                tmpdate.set_years(tli->m_valueDblFrom, -1);
                clrec.name = tmpdate.get_str(TT_DATE_FMT_UTF8_Y);
                if (!thisnan(tli->m_valueDbl))
                  clrec.name += " - ";
              }
            }
            if (!thisnan(tli->m_valueDbl))
              clrec.name += tli->m_valueDate.get_str(TT_DATE_FMT_UTF8_Y);
            if (clrec.name.size() == 0)
              clrec.name = thT("not specified",layout->lang);
            clrec.texname = utf2tex(clrec.name.c_str());
            break;
          default:
            clrec.texname = "";
            clrec.name = "";
            if (this->m_intervals) {
              if (!thisnan(tli->m_valueDblFrom)) {
                clrec.texname = utf2tex(layout->units.format_length(tli->m_valueDblFrom - layout->goz));
                clrec.texname += "\\thinspace ";
                clrec.texname += utf2tex(layout->units.format_i18n_length_units());
                clrec.name = layout->units.format_length(tli->m_valueDblFrom - layout->goz);
                clrec.name += " ";
                clrec.name += layout->units.format_i18n_length_units();
                if (!thisnan(tli->m_valueDbl)) {
                  clrec.texname += " - ";
                  clrec.name += " - ";
                }
              }
            }
            if (!thisnan(tli->m_valueDbl)) {
              clrec.texname += utf2tex(layout->units.format_length(tli->m_valueDbl - layout->goz));
              clrec.texname += "\\thinspace ";
              clrec.texname += utf2tex(layout->units.format_i18n_length_units());
              clrec.name += layout->units.format_length(tli->m_valueDbl - layout->goz);
              clrec.name += " ";
              clrec.name += layout->units.format_i18n_length_units();
            }
            if (clrec.name.size() == 0)
              clrec.name = thT("not specified",layout->lang);
            clrec.texname = utf2tex(clrec.name.c_str());
            break;
        }
      }
      tli->m_labelTeX = clrec.texname;
      tli->m_labelStr = clrec.name;
      COLORLEGENDLIST.insert(COLORLEGENDLIST.end(), clrec);
    }
  }
}




void thlookup_parse_reference(const char * arg, int * type, const char ** index, const char ** nname) {
  // parse lookup type
  thmbuffer mbf;
  thbuffer normname;
  thsplit_strings(& mbf, arg, ':');
  if ((mbf.get_size() > 2) || (mbf.get_size() < 1))
    ththrow("invalid lookup id -- {}", arg);
  if ((mbf.get_size() == 2) && (!th_is_keyword(mbf.get_buffer()[1])))
    ththrow("invalid lookup id -- {}", arg);
  *type = thmatch_token(mbf.get_buffer()[0], thtt_layout_ccrit);
  if (mbf.get_size() == 2)
    *index = thdb.strstore(mbf.get_buffer()[1]);
  else
    *index = thdb.strstore("");
  if (*type != TT_LAYOUT_CCRIT_UNKNOWN)
    normname = thmatch_string(*type, thtt_layout_ccrit);
  else
    normname = "";
  normname += "_";
  normname += *index;
  *nname = thdb.strstore(normname.get_buffer());
}

void thlookup::add_auto_item(class thdataobject * o, thlayout_color c) {
  thlookup_table_row tr;
  tr.m_ref = o;
  tr.m_color = c;
  m_table.push_back(tr);
}


void thlookup::scan_map(class thmap * m) {
  this->m_autoStat.scanmap(m);
  this->m_autoStat.addstat(&(m->stat));
}


void thlookup::auto_generate_items() {
  if (this->m_table.size() > 0) return;
  thdate interval;
  thlookup_table_row tr;
  this->m_autoIntervals = true;
  double from, to;
  int z;
  switch (this->m_type) {
    case TT_LAYOUT_CCRIT_EXPLODATE:
    case TT_LAYOUT_CCRIT_TOPODATE:
      if (this->m_type == TT_LAYOUT_CCRIT_EXPLODATE)
        interval = this->m_autoStat.discovered_date;
      else
        interval = this->m_autoStat.surveyed_date;
      from = interval.get_start_year();
      to = interval.get_end_year();
      for(z = 0; z < 7; z++) {
        tr.m_valueDate.set_years(from + double(z) * ((to - from) / 6), -1.0);
        tr.m_valueDbl = tr.m_valueDate.get_start_year();
        this->m_table.push_back(tr);
      }
      break;
    case TT_LAYOUT_CCRIT_ALTITUDE:
    case TT_LAYOUT_CCRIT_DEPTH:
      this->m_autoStat.get_min_max_alt(from, to);
      for(z = 0; z < 7; z++) {
        tr.m_valueDbl = from + double(z) * ((to - from) / 6);
        if (this->m_type == TT_LAYOUT_CCRIT_DEPTH) tr.m_valueDbl -= this->m_depth_altitude;
		this->m_table.push_front(tr);
      }
      break;
  }
}

#define tmp_alpha 0.75

void thlookup::postprocess() {
  double prevDbl;
  prevDbl = thnan;
  thlookup_table_list::iterator tli;
  for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
    if (this->m_intervals && (!thisnan(tli->m_valueDblFrom)) && (!thisnan(tli->m_valueDbl))) {
      if (tli->m_valueDblFrom < tli->m_valueDbl) {
        this->m_ascending = true;
        break;
      }
      if (tli->m_valueDblFrom > tli->m_valueDbl) {
        this->m_ascending = false;
        break;
      }
    }
    if ((!thisnan(prevDbl)) && (!thisnan(tli->m_valueDbl))) {
      if (prevDbl < tli->m_valueDbl) {
        this->m_ascending = true;
        break;
      }
      if (prevDbl > tli->m_valueDbl) {
        this->m_ascending = false;
        break;
      }
    }
    prevDbl = tli->m_valueDbl;
  }


  // doplni intervaly, ak discrete
  if (this->m_intervals) {
    prevDbl = thnan;
    for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
      if ((!thisnan(prevDbl)) && (thisnan(tli->m_valueDblFrom)))
        tli->m_valueDblFrom = prevDbl;
      prevDbl = tli->m_valueDbl;
    }
  }


  // doplni farby, ak ich nemame na zaklade value, poctu
  thlookup_table_list::iterator lvalid = this->m_table.end(), nvalid = this->m_table.end();
  double lvalidp(0.0), nvalidp(0.0), cp, totalp, ratio;
  int cnt;
  cp = 0.0;
  for(tli = this->m_table.begin(); tli != this->m_table.end(); tli++) {
    if (!tli->m_color.is_defined()) {
      // najdneme next valid
      nvalidp = cp;
      nvalid = tli;
      while (nvalid != this->m_table.end()) {
        nvalid++;
        nvalidp+=1;
        if (nvalid->m_color.is_defined()) break;
      }
      if ((lvalid == this->m_table.end()) && (nvalid == this->m_table.end())) {
        // set all colors
        if ((this->m_type == TT_LAYOUT_CCRIT_SCRAP) || (this->m_type == TT_LAYOUT_CCRIT_MAP)) {
          cnt = 0;
          for(nvalid = this->m_table.begin(); nvalid != this->m_table.end(); nvalid++) {
							switch (cnt % 16) {
								case 0:  nvalid->m_color = thlayout_color(1.0, 0.5, 0.5); break;
								case 1:  nvalid->m_color = thlayout_color(0.5, 1.0, 0.5); break;
								case 2:  nvalid->m_color = thlayout_color(0.5, 0.5, 1.0); break;
								case 3:  nvalid->m_color = thlayout_color(1.0, 1.0, 0.0); break;
								case 4:  nvalid->m_color = thlayout_color(0.0, 1.0, 1.0); break;
								case 5:  nvalid->m_color = thlayout_color(1.0, 0.0, 1.0); break;
								case 6:  nvalid->m_color = thlayout_color(0.75, 1.0, 1.0); break;
								case 7:  nvalid->m_color = thlayout_color(1.0, 0.75, 1.0); break;
								case 8:  nvalid->m_color = thlayout_color(1.0, 1.0, 0.75); break;
								case 9:  nvalid->m_color = thlayout_color(0.25, 0.75, 1.0); break;
								case 10: nvalid->m_color = thlayout_color(0.25, 1.0, 0.75); break;
								case 11: nvalid->m_color = thlayout_color(1.0, 0.75, 0.25); break;
								case 12: nvalid->m_color = thlayout_color(0.75, 1.0, 0.25); break;
                case 13: nvalid->m_color = thlayout_color(0.75, 0.25, 1.0); break;
                case 14: nvalid->m_color = thlayout_color(1.0, 0.25, 0.75); break;
                default: nvalid->m_color = thlayout_color(0.5, 0.25, 0.75); break;
							}
            nvalid->m_color.alpha_correct(tmp_alpha);
            nvalid->m_color.defined = 1;

            cnt++;
          }
        } else {
          totalp = (double) this->m_table.size() - 1.0;
          if (this->m_ascending)
        	  cp = totalp;
          else
        	  cp = 0;
          for(nvalid = this->m_table.begin(); nvalid != this->m_table.end(); nvalid++) {
            thset_color(0, cp, totalp, nvalid->m_color);
            nvalid->m_color.defined = 1;
            nvalid->m_color.alpha_correct(tmp_alpha);
            if (this->m_ascending)
            	cp -= 1.0;
            else
            	cp += 1.0;
          }
        }
        break;
      } else if (lvalid == this->m_table.end()) {
        tli->m_color = nvalid->m_color;
      } else if (nvalid == this->m_table.end()) {
        tli->m_color = lvalid->m_color;
      } else {
        ratio = (cp - lvalidp) / (nvalidp - lvalidp);
        tli->m_color = lvalid->m_color;
        tli->m_color.mix_with_color(ratio, nvalid->m_color);
        tli->m_color.defined = 1;
      }
    } else {
      lvalid = tli;
      lvalidp = cp;
    }
    cp += 1.0;
  }


}



