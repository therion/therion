/**
 * @file thgrade.cxx
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
 
#include "thgrade.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdata.h"
#include "thparse.h"

thgrade::thgrade()
{
  this->data.reset_data_sd();
  this->update_sd_from_data(true);
}


thgrade::~thgrade()
{
}


int thgrade::get_class_id() 
{
  return TT_GRADE_CMD;
}


bool thgrade::is(int class_id)
{
  if (class_id == TT_GRADE_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thgrade::get_cmd_nargs() 
{
  return 1;
}


const char * thgrade::get_cmd_end()
{
  return "endgrade";
}


const char * thgrade::get_cmd_name()
{
  // insert command name here
  return "grade";
}


thcmd_option_desc thgrade::get_cmd_option_desc(const char * opts)
{
  // no options allowed
  //  int id = thmatch_token(opts, thtt_grade_opt);
  //  if (id == TT_GRADE_UNKNOWN)
  return thdataobject::get_cmd_option_desc(opts);
  //  else
  //    return thcmd_option_desc(id);
}


void thgrade::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    case 0:
      thsplit_args(& this->db->mbuff_tmp, *args);
      this->set_sd(this->db->mbuff_tmp.get_size(),
        this->db->mbuff_tmp.get_buffer());
      break;
    
    case 1:
      cod.id = TT_DATAOBJECT_NAME;
      /* FALLTHRU */
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}


void thgrade::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thgrade:\n");
  fprintf(outf,"\tlength sd: %f metres\n",thinn(this->dls_length));
  fprintf(outf,"\tbrearing sd: %f degrees\n",thinn(this->dls_bearing));
  fprintf(outf,"\tgradient sd: %f degrees\n",thinn(this->dls_gradient));
  fprintf(outf,"\tcounter sd: %f metres\n",thinn(this->dls_counter));
  fprintf(outf,"\tdepth sd: %f metres\n",thinn(this->dls_depth));
  fprintf(outf,"\tdx sd: %f metres\n",thinn(this->dls_dx));
  fprintf(outf,"\tdy sd: %f metres\n",thinn(this->dls_dy));
  fprintf(outf,"\tdz sd: %f metres\n",thinn(this->dls_dz));
  fprintf(outf,"\tx sd: %f metres\n",thinn(this->dls_x));
  fprintf(outf,"\ty sd: %f metres\n",thinn(this->dls_y));
  fprintf(outf,"\tz sd: %f metres\n",thinn(this->dls_z));
}


void thgrade::set_sd(int nargs, char ** args)
{
  this->data.set_data_sd(nargs, args);
}

int thgrade::get_context()
{
  return (THCTX_SURVEY | THCTX_NONE | THCTX_SCRAP);
}


void thgrade::update_sd_from_data(bool alsonan)
{
  if (alsonan) {
    this->dls_length = this->data.dls_length;
    this->dls_gradient = this->data.dls_gradient;
    this->dls_bearing = this->data.dls_bearing;
    this->dls_counter = this->data.dls_counter;
    this->dls_depth = this->data.dls_depth;
    this->dls_dx = this->data.dls_dx;
    this->dls_dy = this->data.dls_dy;
    this->dls_dz = this->data.dls_dz;
    this->dls_x = this->data.dls_x;
    this->dls_y = this->data.dls_y;
    this->dls_z = this->data.dls_z;
  }
  else {
    thupdate_double(this->dls_length, this->data.dls_length);
    thupdate_double(this->dls_gradient, this->data.dls_gradient);
    thupdate_double(this->dls_bearing, this->data.dls_bearing);
    thupdate_double(this->dls_counter, this->data.dls_counter);
    thupdate_double(this->dls_depth, this->data.dls_depth);
    thupdate_double(this->dls_dx, this->data.dls_dx);
    thupdate_double(this->dls_dy, this->data.dls_dy);
    thupdate_double(this->dls_dz, this->data.dls_dz);
    thupdate_double(this->dls_x, this->data.dls_x);
    thupdate_double(this->dls_y, this->data.dls_y);
    thupdate_double(this->dls_z, this->data.dls_z);
  }
}


void thgrade::update_data_sd(thdata * dp)
{
  thupdate_double(dp->dls_length, this->dls_length);
  thupdate_double(dp->dls_gradient, this->dls_gradient);
  thupdate_double(dp->dls_bearing, this->dls_bearing);
  thupdate_double(dp->dls_counter, this->dls_counter);
  thupdate_double(dp->dls_depth, this->dls_depth);
  thupdate_double(dp->dls_dx, this->dls_dx);
  thupdate_double(dp->dls_dy, this->dls_dy);
  thupdate_double(dp->dls_dz, this->dls_dz);
  thupdate_double(dp->dls_x, this->dls_x);
  thupdate_double(dp->dls_y, this->dls_y);
  thupdate_double(dp->dls_z, this->dls_z);
}

void thgrade::start_insert()
{
  this->update_sd_from_data();
}

void thgrade::self_print_library() {
  thprintf("\toname = \"%s\";\n", this->get_name());
  thprintf("\tpgrade->set(thcmd_option_desc(TT_DATAOBJECT_NAME,1),oname,0,0);\n");
  thdecode_c(&(this->db->buff_enc), this->get_title());
  thprintf("\toname = \"%s\";\n", this->db->buff_enc.get_buffer());
  thprintf("\tpgrade->set(thcmd_option_desc(TT_DATAOBJECT_TITLE,1),oname,TT_UTF_8,0);\n");
  thprintf("\tpgrade->dls_length = ");
  thprintinfnan(this->dls_length);
  thprintf(";\n\tpgrade->dls_bearing = ");
  thprintinfnan(this->dls_bearing);
  thprintf(";\n\tpgrade->dls_gradient = ");
  thprintinfnan(this->dls_gradient);
  thprintf(";\n\tpgrade->dls_counter = ");
  thprintinfnan(this->dls_counter);
  thprintf(";\n\tpgrade->dls_depth = ");
  thprintinfnan(this->dls_depth);
  thprintf(";\n\tpgrade->dls_dx = ");
  thprintinfnan(this->dls_dx);
  thprintf(";\n\tpgrade->dls_dy = ");
  thprintinfnan(this->dls_dy);
  thprintf(";\n\tpgrade->dls_dz = ");
  thprintinfnan(this->dls_dz);
  thprintf(";\n\tpgrade->dls_x = ");
  thprintinfnan(this->dls_x);
  thprintf(";\n\tpgrade->dls_y = ");
  thprintinfnan(this->dls_y);
  thprintf(";\n\tpgrade->dls_z = ");
  thprintinfnan(this->dls_z);
  thprintf(";\n");
}

thdata thgrade::data;




