/**
 * @file thsurvey.cxx
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
 
#include "thsurvey.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdate.h"
#include "thtfangle.h"
#include "thinfnan.h"

thsurvey::thsurvey()
{
  this->full_name = "";
  this->reverse_full_name = this->full_name;
  this->foptr = NULL;
  this->loptr = NULL;
  this->decdef = false;
  this->decuds = thnan;
  this->num1 = 0;
  this->person_renames.clear();
}


thsurvey::~thsurvey()
{
}


int thsurvey::get_class_id() 
{
  return TT_SURVEY_CMD;
}


bool thsurvey::is(int class_id)
{
  if (class_id == TT_SURVEY_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thsurvey::get_cmd_nargs() 
{
  return 1;
}


char * thsurvey::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


char * thsurvey::get_cmd_name()
{
  // insert endcommand if multiline command
  return "survey";
}


thcmd_option_desc thsurvey::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_survey_opt);
  if (id == TT_SURVEY_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else switch (id) {
    case TT_SURVEY_PERSON_RENAME:
     return thcmd_option_desc(id,2);
    default:
     return thcmd_option_desc(id);
  }
}


void thsurvey::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  thperson tmpp1, tmpp2;
  if (cod.id == 1)
    cod.id = TT_DATAOBJECT_NAME;
    
  switch (cod.id) {

    case TT_SURVEY_DECLINATION:
      this->parse_declination(*args);
      break;
      
    case TT_SURVEY_PERSON_RENAME:
      thencode(&(this->db->buff_enc), args[0], argenc);
      tmpp1.parse(this->db, this->db->buff_enc.get_buffer());
      thencode(&(this->db->buff_enc), args[1], argenc);
      tmpp2.parse(this->db, this->db->buff_enc.get_buffer());
      this->person_renames[tmpp1] = tmpp2;
      break;
      
    case TT_DATAOBJECT_NAME:
      if (th_is_keyword(*args))
        this->name = this->db->strstore(*args);
      else 
        ththrow(("invalid keyword -- %s", *args));
      break;
      
    default:
      thdataobject::set(cod, args, argenc, indataline);
      
  }
}


void thsurvey::self_delete()
{
  delete this;
}


void thsurvey::self_print_properties(FILE * outf)
{
  size_t nval, idx;
  thtfpwfxy * vals;
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thsurvey:\n");
  fprintf(outf,"\tfull name: \"%s\" (\"%s\")\n", this->full_name,
    this->reverse_full_name);
  if (this->decdef) {
    fprintf(outf,"\tdeclination:\n");
    nval = this->declin.get_size();
    vals = this->declin.get_values();
    for(idx = 0; idx < nval; idx++) {
      fprintf(outf,"\t\t%0.2f -> %8.5f deg\n",vals[idx].x,vals[idx].y);
    }
  }
  else
    fprintf(outf,"\tdeclination: none\n");
  fprintf(outf,"\tlength: %g\n",this->stat.length);
}


int thsurvey::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE);
}

char * thsurvey::get_full_name()
{
  return this->full_name;
}


void thsurvey::parse_declination(char * str)
{
  // splitne input string
  thtfangle du;
  thdate dd;
  double decl, //year, 
    dumdt;
  double * all_data = NULL;
  double data2 [2];
  int nid, idx, dateidx, typedt;
  
  thsplit_args(& this->db->mbuff_tmp, str);
  long nargs = this->db->mbuff_tmp.get_size();
  char ** args = this->db->mbuff_tmp.get_buffer();

  // ak 1 4 6 ... error
  if ((nargs == 1) || (((nargs % 2) == 0) && (nargs > 3)))
    ththrow(("invalid declination specification -- \"%s\"",str))

  // ak 0 tak resetuje a nastavi na 0
  if (nargs == 0) {
    this->decdef = false;
    return;
  }
  
  // ak nie, tak parsne jednotky
  nargs--;
  du.parse_units(args[nargs]);
  
  // ak 2 tak nastavi pausalne s rokom 0
  if (nargs == 1) {
    thparse_double(nid, decl, args[0]);
    if (nid != TT_SV_NUMBER)
      ththrow(("invalid declination -- %s", args[0]))
    decl = du.transform(decl);
    data2[0] = 0.0;
    data2[1] = decl;
    this->decuds = decl;
    this->declin.set(1,data2);
    this->decdef = true;
    return;
  }
  
  all_data = new double [nargs];
  nargs = nargs/2;
  dateidx = 0;
  for(idx = 0; idx < nargs; idx++) {

    // let's try if undated declination specification
    thparse_double(typedt, dumdt, args[2 * idx]);
    if (typedt == TT_SV_NAN) {
      thparse_double(nid, decl, args[2 * idx + 1]);
      if (nid != TT_SV_NUMBER) {
        delete [] all_data;
        ththrow(("invalid declination -- %s", args[2 * idx + 1]))
      }
      this->decuds = decl;
    }
    
    // if not, try dated declination specification
    else {
      try {
        dd.parse(args[2 * idx]);
      } catch (...) {
        delete [] all_data;
        threthrow(("invalid declination specification"))
      }
      all_data[2 * dateidx] = dd.get_start_year();
      // parse declination
      thparse_double(nid, decl, args[2 * idx + 1]);
      if (nid != TT_SV_NUMBER) {
        delete [] all_data;
        ththrow(("invalid declination -- %s", args[2 * idx + 1]))
      }
      all_data[2 * dateidx + 1] = du.transform(decl);
      dateidx++;
    }
  }
  
  // set declination
  if (dateidx == 0) {
    data2[0] = 0.0;
    data2[1] = this->decuds;
    this->declin.set(1,data2);
  } 
  else
    this->declin.set(dateidx,all_data);
  this->decdef = true;
  delete [] all_data;
}


void thsurvey::full_name_reverse()
{
  size_t fnln = strlen(this->full_name), i1, i2, si; //, ei;
  char * c1, * c2, *c3;
  c1 = this->full_name;
  si = 0;
  bool has_dot = false;
  for(i1 = 0; i1 <= fnln; i1++, c1++) {
    if ((*c1 == '.') || ((i1 == fnln) && has_dot)) {
      has_dot = true;
      c3 = this->full_name + si;
      c2 = this->reverse_full_name + fnln - i1;
      if (i1 < fnln) {
        c2--;
        *c2 = '.';
        c2++;
      }
      for(i2 = si; i2 < i1; i2++, c2++, c3++)
        *c2 = *c3;
      si = i1 + 1;
    }
  }
}



