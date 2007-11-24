/**
 * @file thexport.cxx
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
 
#include "thexport.h"
#include "thexception.h"
#include "thconfig.h"
#include "thdatabase.h"
#include "thexporter.h"
#include <stdio.h>



thexport::thexport() {
  this->outpt = "";
  this->outpt_def = false;
  this->export_mode = 0;
}

thexport::~thexport() {}

void thexport::assign_config(class thconfig * cptr) 
{
  this->cfgptr = cptr;
  this->cfgpath = cptr->cfg_file.get_cif_path();
}


void thexport::parse(int nargs, char ** args)
{
  int ax = 0, oax;
  this->parse_arguments(ax, nargs, args);
  while (ax < nargs) {
    oax = ax;
    this->parse_options(ax, nargs, args);
    if (oax == ax)
      ththrow(("unknown option -- \"%s\"", args[ax]))
  }
}


void thexport::parse_arguments(int & argx, int nargs, char ** args)
{
}

  
void thexport::parse_options(int & argx, int nargs, char ** args)
{
  int optid = thmatch_token(args[argx], thtt_exp_opt);
  int optx = argx;
  switch (optid) {
    case TT_EXP_OPT_OUTPUT:  
      argx++;
      if (argx >= nargs)
        ththrow(("missing output file name -- \"%s\"",args[optx]))
      if (strlen(args[argx]) > 0) {
        this->outpt = this->cfgptr->get_db()->strstore(args[argx]);
        outpt_def = true;
      }
      else
        ththrow(("empty file name not allowed -- \"%s\"",args[optx]))
      argx++;
      break;
    default:
      break;
  }
}


void thexport::dump(FILE * xf)
{
  this->dump_header(xf);
  this->dump_body(xf);
  fprintf(xf,"\n");
}


void thexport::dump_header(FILE * xf)
{
  fprintf(xf,"export %s", thmatch_string(this->export_mode, thtt_exporter));
}


void thexport::dump_body(FILE * xf)
{
  if (this->outpt_def) {
    thdecode_arg(&(this->cfgptr->bf1), this->outpt);
    fprintf(xf," -output %s",this->cfgptr->bf1.get_buffer());
  }  
}


const char * thexport::get_output(const char * defname)
{
  static thbuffer outptfname;
  outptfname = this->cfgpath.get_buffer();
  if (this->outpt_def) {
    if (thpath_is_absolute(this->outpt))
      return this->outpt;
    else
      outptfname += this->outpt;
  } else {
    outptfname += defname;
  }
  return outptfname.get_buffer();
}


