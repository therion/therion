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


void thexporter::parse_export(int nargs, char ** args) {

  thexport * xp;
  int expmode; 

  if (nargs < 1)
    ththrow(("not enought export arguments"))
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
    case TT_EXP_MAP:
    case TT_EXP_ATLAS:
      xp = new thexpmap;
      xp->export_mode = expmode;
      ((thexpmap*)xp)->format = ((thexpmap*)xp)->get_default_format();
      break;
    default:
      ththrow(("unsupported export type -- %s", args[0]))
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
 
  thexporter_list::iterator ii, ii2;
//  thdb2dprjpr prjid1, prjid2;
  
  // najprv exportuje mapy a atlasy
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    switch ((*ii)->export_mode) {
      case TT_EXP_MAP:
      case TT_EXP_ATLAS:
        ((thexpmap*)(*ii))->parse_projection(dp);
    }
  }    
  
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    switch ((*ii)->export_mode) {
      case TT_EXP_MAP:
      case TT_EXP_ATLAS:
        if (((thexpmap*)(*ii))->projptr != NULL) {
          thexporter_quick_map_export = false;
          (*ii)->process_db(dp);
          ii2 = ii;
          // prejde ostatne a tie co maju rovnaku projekciu
          // exportuje v skratenom legislativnom konani
          for(ii2++; ii2 != this->xlist.end(); ii2++) {
            switch ((*ii2)->export_mode) {
              case TT_EXP_MAP:
              case TT_EXP_ATLAS:
                if ((((thexpmap*)(*ii2))->projptr != NULL) &&
                    (((thexpmap*)(*ii))->projptr->id == 
                    ((thexpmap*)(*ii2))->projptr->id)) {
                  thexporter_quick_map_export = true;
                  (*ii2)->process_db(dp);
                  ((thexpmap*)(*ii2))->projptr = NULL;            
                }
                break;
              default:
                break;
            }
          }
          ((thexpmap*)(*ii))->projptr = NULL;            
        }  
        break;
    }
  }
  
  // potom vsetko ostatne
  for(ii = this->xlist.begin(); ii != this->xlist.end(); ii++) {
    switch ((*ii)->export_mode) {
      case TT_EXP_MAP:
      case TT_EXP_ATLAS:
        break;
      default:
        (*ii)->process_db(dp);
        break;
    }
  }
  
}

bool thexporter_quick_map_export;



