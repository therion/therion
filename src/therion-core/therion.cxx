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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
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
#include "thlog.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>

int therion_exit_state = 2;

bool thverbose_mode = true;

bool thtext_inline = false;

char * thexecute_cmd = NULL;

static void thprint_impl(const bool verbose, FILE* f, std::string_view msg)
{
  thlog(msg);
  if (verbose) {
    fmt::print(f, "{}", msg);
  }
}

void thprint(std::string_view msg)
{
  thprint_impl(thverbose_mode, stdout, msg);
}

void thprint2err(std::string_view msg) noexcept
{
  try
  {
    thprint_impl(true, stderr, msg);
  }
  catch(const std::exception& e)
  {
    std::fprintf(stderr, "error occurred while reporting another error: %s\n", e.what());
  }
  catch (...)
  {
    std::fprintf(stderr, "unknown error occurred while reporting another error\n");
  }
}

void thprint_environment() {
  thprint(fmt::format("\n\nINIT={}\n",thcfg.get_initialization_path()));
  thprint(fmt::format("SOURCE={}\n\n",thcfg.get_search_path()));
  thprint(fmt::format("CAVERN={}\n",thini.get_path_cavern()));
  thprint(fmt::format("METAPOST={}\n",thini.get_path_mpost()));
  thprint(fmt::format("PDFTEX={}\n",thini.get_path_pdftex()));
  thprint(fmt::format("IDENTIFY={}\n",thini.get_path_identify()));
  thprint(fmt::format("CONVERT={}\n",thini.get_path_convert()));
}


void thprint_xtherion() {
  bool already_exported;
  int i, j, l;
  const char * lngstr, *trnstr, * tsrc;
  thbuffer tdst;
  std::string tss;
  thprint("set xth(point_types) {\n");
  for(i = 0; thtt_point_types[i].tok != TT_POINT_TYPE_UNKNOWN; i++) {
    already_exported = false;
    for(j = 0; j < i; j++) {
      if (thtt_point_types[i].tok == thtt_point_types[j].tok) {
        already_exported = true;
        break;
      }
    }
    if (!already_exported) {
      thprint(fmt::format("\t{}\n",thtt_point_types[i].s));
    }
  }
  thprint("}\n\nset xth(line_types) {\n");
  for(i = 0; thtt_line_types[i].tok != TT_LINE_TYPE_UNKNOWN; i++) {
    already_exported = false;
    for(j = 0; j < i; j++) {
      if (thtt_line_types[i].tok == thtt_line_types[j].tok) {
        already_exported = true;
        break;
      }
    }
    if (!already_exported) {
      thprint(fmt::format("\t{}\n",thtt_line_types[i].s));
    }
  }
  thprint("}\n\nset xth(area_types) {\n");
  for(i = 0; thtt_area_types[i].tok != TT_AREA_TYPE_UNKNOWN; i++) {
    already_exported = false;
    for(j = 0; j < i; j++) {
      if (thtt_area_types[i].tok == thtt_area_types[j].tok) {
        already_exported = true;
        break;
      }
    }
    if (!already_exported) {
      thprint(fmt::format("\t{}\n",thtt_area_types[i].s));
    }
  }
  thprint("}\n");

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
  thprint(fmt::format("\n{}", tss));
}


void thprint_xth_lines() {
}


void thpause_exit() {
#ifdef THWIN32
  if (thverbose_mode && (!(thcfg.generate_xthcfg))) {
    thprint("Press ENTER to exit!");
    getchar();
  }
#endif
}


void thexit(int exit_code)
{
  if (exit_code == EXIT_FAILURE) thcfg.xth_save();
  exit(exit_code);
}
