/**
 * @file thexporter.cxx
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
 
#include "thexporter.h"
#include "thexception.h"
#include "thconfig.h"
#include "thdatabase.h"
#include "thexpmodel.h"
#include "thexpmap.h"
#include "thexpdb.h"
#include "thexpsys.h"
#include "thexptable.h"
#include <stdio.h>


thexporter::thexporter()
{
  this->cfgptr = NULL;
}


thexporter::~thexporter()
{
  thexporter_list::iterator ii;
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    delete (*ii);
  }
}


void thexporter::assign_config(class thconfig * cptr) {
  this->cfgptr = cptr;
}



void thexporter::parse_system(char * system_cmd)
{
  if (strlen(system_cmd) == 0)
    ththrow("empty system command not allowed");
  thexpsys * xp;
  xp = new thexpsys;
  xp->src.name = thdb.strstore(thcfg.get_cfg_file()->get_cif_name(),true);
  xp->src.line = thcfg.get_cfg_file()->get_cif_line_number();  
  xp->assign_config(this->cfgptr);
  xp->cmd = thdb.strstore(system_cmd, false);
  this->xlist.push_back(xp);
}


void thexporter::parse_export(int nargs, char ** args) {

  thexport * xp;
  int expmode; 

  if (nargs < 1)
    ththrow("not enough export arguments");
  expmode = thmatch_token(args[0], thtt_exporter);
  switch (expmode) {
    case TT_EXP_MODEL:
      xp = new thexpmodel;
      xp->export_mode = expmode;
      break;
    case TT_EXP_DATABASE:
      xp = new thexpdb;
      xp->export_mode = expmode;
      break;
    case TT_EXP_CONTLIST:
    case TT_EXP_SURVEYLIST:
    case TT_EXP_CAVELIST:
      xp = new thexptable;
      xp->export_mode = expmode;
      break;
    case TT_EXP_MAP:
    case TT_EXP_ATLAS:
      xp = new thexpmap;
      xp->export_mode = expmode;
      break;
    default:
      ththrow("unsupported export type -- {}", args[0]);
  }
  
  xp->src.name = thdb.strstore(thcfg.get_cfg_file()->get_cif_name(),true);
  xp->src.line = thcfg.get_cfg_file()->get_cif_line_number();  
  
  // take care of layout revision
  switch (expmode) {
    case TT_EXP_MAP:
    case TT_EXP_ATLAS:
      thdb.revision_set.insert(threvision(thdb.objid,0,xp->src));
      break;
  }

  nargs--;
  args++;
  xp->assign_config(this->cfgptr);
  xp->parse(nargs, args);
  this->xlist.push_back(xp);
  
}

   
void thexporter::dump_export(FILE * xf)
{
  thexporter_list::iterator ii;
  if ((!this->xlist.empty()) && (!this->cfgptr->get_comments_skip()))
    fprintf(xf,"# Export settings.\n");
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    (*ii)->dump(xf);
  }
  if (!this->xlist.empty())
    fprintf(xf,"\n");
}

   
void thexporter::export_db(class thdatabase * dp)
{ 
 
  thexporter_list::iterator ii;
  
  // najprv spracujeme projekcie
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    switch ((*ii)->export_mode) {
      case TT_EXP_MAP:
      case TT_EXP_ATLAS:
        ((thexpmap*)(*ii))->parse_projection(dp);
    }
  }    

  // exportujeme  
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    switch ((*ii)->export_mode) {
      case TT_EXP_MAP:
      case TT_EXP_ATLAS:
        thexporter_quick_map_export = false;
        (*ii)->process_db(dp);
        break;

      default:
        (*ii)->process_db(dp);
        break;
    }
  }  

  // check crc if approrpiate
  if (thcfg.crc_generate || thcfg.crc_verify) {
	  bool ok(true), cok(true);
	  if (thcfg.crc_generate)
		  thprintf("generating");
	  else
		  thprintf("checking");
	  thprintf(" CRC32 of all output files ... \n");
	  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
		  cok = (*ii)->check_crc();
		  ok = ok && cok;
		  for(auto fi = (*ii)->output_files.begin(); fi != (*ii)->output_files.end(); fi++) {
			  thprintf("%s ... %s\n", fi->fnm.c_str(), fi->res.c_str());
		  }

	  }
	  thprintf("done.\n");
	  if (!ok) therror(("CRC32 checks not passed.\n"));
  }

  
}

bool thexporter_quick_map_export;



