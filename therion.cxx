/**
 * @file therion.cxx
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
 
#include "therion.h"
#include "thconfig.h"
#include "thinit.h"
#include "thlayout.h"
#include "thpoint.h"
#include "thline.h"
#include "tharea.h"
#include "thlang.h"
#include "thparse.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>

int therion_exit_state = 2;

bool thverbose_mode = true;

bool thtext_inline = false;

char * thexecute_cmd = NULL;

void thprint_environment() {
  thprintf("\n\nINIT=%s\n",thcfg.get_initialization_path());
  thprintf("SOURCE=%s\n\n",thcfg.get_search_path());
  thprintf("CAVERN=%s\n",thini.get_path_cavern());
  thprintf("METAPOST=%s\n",thini.get_path_mpost());
  thprintf("PDFTEX=%s\n",thini.get_path_pdftex());
  thprintf("IDENTIFY=%s\n",thini.get_path_identify());
  thprintf("CONVERT=%s\n",thini.get_path_convert());
}


void thprint_xtherion() {
  bool already_exported;
  int i, j, l;
  const char * lngstr, *trnstr, * tsrc;
  thbuffer tdst;
  std::string tss;
  thprintf("set xth(point_types) {\n");
  for(i = 0; thtt_point_types[i].tok != TT_POINT_TYPE_UNKNOWN; i++) {
    already_exported = false;
    for(j = 0; j < i; j++) {
      if (thtt_point_types[i].tok == thtt_point_types[j].tok) {
        already_exported = true;
        break;
      }
    }
    if (!already_exported) {
      thprintf("\t%s\n",thtt_point_types[i].s);
    }
  }
  thprintf("}\n\nset xth(line_types) {\n");
  for(i = 0; thtt_line_types[i].tok != TT_LINE_TYPE_UNKNOWN; i++) {
    already_exported = false;
    for(j = 0; j < i; j++) {
      if (thtt_line_types[i].tok == thtt_line_types[j].tok) {
        already_exported = true;
        break;
      }
    }
    if (!already_exported) {
      thprintf("\t%s\n",thtt_line_types[i].s);
    }
  }
  thprintf("}\n\nset xth(area_types) {\n");
  for(i = 0; thtt_area_types[i].tok != TT_AREA_TYPE_UNKNOWN; i++) {
    already_exported = false;
    for(j = 0; j < i; j++) {
      if (thtt_area_types[i].tok == thtt_area_types[j].tok) {
        already_exported = true;
        break;
      }
    }
    if (!already_exported) {
      thprintf("\t%s\n",thtt_area_types[i].s);
    }
  }
  thprintf("}\n");

  tss = "";
  const thstok * x = thlang_get_text_table();
  while (x->s != NULL) {
    tsrc = x->s;
    l = 0;
    lngstr = thlang_getid(l);
    while (strlen(lngstr) > 0) {
      trnstr = thT(tsrc, l);
      if (strcmp(trnstr, tsrc) != 0) {
        thdecode_tcl(&tdst, trnstr);
        tss += "::msgcat::mcset ";
        tss += lngstr;
        tss += " \"";
        tss += tsrc;
        tss += "\" [encoding convertfrom utf-8 \"";
        tss += tdst.get_buffer();
        tss += "\"]\n";
      }
      l++;
      lngstr = thlang_getid(l);
    }      
    x = &(x[1]);
  }
  thprintf("\n%s", tss.c_str());
}


void thprint_xth_lines() {
}


void thpause_exit() {
#ifdef THWIN32
  if (thverbose_mode && (!(thcfg.generate_xthcfg))) {
    thprintf("Press ENTER to exit!");
    getchar();
  }
#endif
}


void thexit(int exit_code)
{
  if (exit_code == EXIT_FAILURE) thcfg.xth_save();
  exit(exit_code);
}
