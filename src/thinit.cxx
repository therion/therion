/**
 * @file thinit.cxx
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
 
#include "thinit.h"
#include "thparse.h"
#include "thchenc.h"
#include "therion.h"
#include "thconfig.h"
#include "thexception.h"

const char * THCCC_INIT_FILE = "# Default output encoding.\n"\
"encoding_default  ISO8859-1\n\n"\
"# Paths to called executable files.\n"\
"path_cavern  \"cavern\"\n"\
"path_3dtopos  \"3dtopos\"\n"\
"path_mpost  \"mpost\"\n"\
"path_pdftex  \"pdfetex\"\n\n";

thinit::thinit()
{
  // set encodings
  this->encoding_default = TT_ISO8859_1;
  
  // set programs paths
  this->path_cavern = "cavern";
  this->path_3dtopos = "3dtopos";
  this->path_mpost = "mpost";
  this->path_pdftex = "pdfetex";
  
}


thinit::~thinit()
{
}

enum {
  TTIC_ENCODING_DEFAULT,
  TTIC_PATH_CAVERN,
  TTIC_PATH_3DTOPOS,
  TTIC_PATH_MPOST,
  TTIC_PATH_PDFTEX,
  TTIC_UNKNOWN,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_initcmd[] = {
  {"encoding_default", TTIC_ENCODING_DEFAULT},
  {"path_3dtopos", TTIC_PATH_3DTOPOS},
  {"path_cavern", TTIC_PATH_CAVERN},
  {"path_mpost", TTIC_PATH_MPOST},
  {"path_pdftex", TTIC_PATH_PDFTEX},
  {NULL, TTIC_UNKNOWN},
};



void thinit::load()
{
  this->ini_file.set_search_path(thcfg.get_search_path());
  this->ini_file.set_file_name("therion.ini");
  this->ini_file.sp_scan_on();
  this->ini_file.cmd_sensitivity_off();
  this->ini_file.reset();
  char * cmdln;
  char ** args;
  int nargs, argid; //, argid2;
  try {
    while((cmdln = this->ini_file.read_line()) != NULL) {
      thsplit_args(& this->cmb, cmdln);
      args = this->cmb.get_buffer();
      nargs = this->cmb.get_size();
      argid = thmatch_token(args[0], thtt_initcmd);
      
      // check number of arguments:
      if (nargs != 2)
        ththrow(("invalid number of command arguments"));
        
      switch(argid) {
        
        case TTIC_ENCODING_DEFAULT:
          this->encoding_default = thparse_encoding(args[1]);
          break;
          
        case TTIC_PATH_CAVERN:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_cavern.strcpy(args[1]);
          break;

        case TTIC_PATH_3DTOPOS:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_3dtopos.strcpy(args[1]);
          break;
          
        case TTIC_PATH_MPOST:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_mpost.strcpy(args[1]);
          break;

        case TTIC_PATH_PDFTEX:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_pdftex.strcpy(args[1]);
          break;
          
        default:
          ththrow(("invalid initialization command -- %s", args[0]))
      }
    }
  }
  catch (...)
    threthrow(("%s [%d]", this->ini_file.get_cif_name(), this->ini_file.get_cif_line_number()))
}


char * thinit::get_path_cavern()
{
  return this->path_cavern.get_buffer();
}

char * thinit::get_path_3dtopos()
{
  return this->path_3dtopos.get_buffer();
}

char * thinit::get_path_mpost()
{
  return this->path_mpost.get_buffer();
}

char * thinit::get_path_pdftex()
{
  return this->path_pdftex.get_buffer();
}


void thprint_init_file()
{
  thprintf(THCCC_INIT_FILE);
}


thinit thini;


