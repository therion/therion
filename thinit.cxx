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
#include "thtexfonts.h"
#include "thlang.h"

#ifdef THWIN32
#include <windows.h>
#endif

const char * THCCC_INIT_FILE = "### Output character encodings ###\n"
"# encoding-default  ASCII\n"
"# encoding-sql  ASCII\n\n"
"### Default output language ###\n"
"# language  en_UK\n\n"
"### Paths to called executable files ###\n"
"# mpost-path  \"mpost\"\n"
"# pdftex-path  \"pdfetex\"\n\n"
"### Search paths for source and configuration files ###\n"
"# source-path  \"\"\n\n"
"### Tex fonts initialization ###\n"
"# tex-fonts <encoding> <roman> <italic> <bold> <sansserif> <sansserifoblique>\n"
"# tex-fonts raw cmr10 cmti10 cmbx10 cmss10 cmssi10\n"
"# tex-fonts xl2 csr10 csti10 csbx10 csss10 csssi10\n\n"
"### Path to temporary directory ###\n"
"# tmp-path  \"\"\n\n"
"### Command to remove temporary directory ###\n"
"# tmp-remove  \"\"\n\n";

thinit::thinit()
{
}


thinit::~thinit()
{
}

enum {
  TTIC_ENCODING_DEFAULT,
  TTIC_ENCODING_SQL,
  TTIC_PATH_CAVERN,
//  TTIC_PATH_3DTOPOS,
  TTIC_PATH_MPOST,
  TTIC_PATH_PDFTEX,
  TTIC_PATH_SOURCE,
  TTIC_TMP_PATH,
  TTIC_TMP_REMOVE_SCRIPT,
  TTIC_LANG,
  TTIC_TEX_FONTS,
  TTIC_UNKNOWN,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_initcmd[] = {
//  {"cavern-path", TTIC_PATH_CAVERN},
  {"encoding-default", TTIC_ENCODING_DEFAULT},
  {"encoding-sql", TTIC_ENCODING_SQL},
//  {"encoding_default", TTIC_ENCODING_DEFAULT},
//  {"path_3dtopos", TTIC_PATH_3DTOPOS},
  {"language", TTIC_LANG},
  {"mpost-path", TTIC_PATH_MPOST},
  {"pdftex-path", TTIC_PATH_PDFTEX},
  {"source-path", TTIC_PATH_SOURCE},
  {"tex-fonts",TTIC_TEX_FONTS},
  {"tmp-path",TTIC_TMP_PATH},
  {"tmp-remove",TTIC_TMP_REMOVE_SCRIPT},
  {NULL, TTIC_UNKNOWN},
};

void thinit__print_open(char * s) {
#ifdef THDEBUG
    thprintf("\ninitialization file: %s\nreading\n", s);
#else
    thprintf("initialization file: %s\n", s);
    thprintf("reading ...");
    thtext_inline = true;
#endif 
}

void thinit::load()
{

  // set encodings
  this->encoding_default = TT_ASCII;
  this->encoding_sql = TT_UNKNOWN_ENCODING;
  
  // set programs paths
  this->path_cavern = "cavern";

#ifdef THWIN32
  // set cavern path according to Windows registers
  this->path_cavern.guarantee(1024);
  thmbuffer mbf;
  DWORD type, length = 1024;
  HKEY key;
  bool loaded_ok = true;
	if (RegOpenKey(HKEY_CLASSES_ROOT,"survex.source\\shell\\Process\\command",&key) != ERROR_SUCCESS)
    loaded_ok = false;
	if (!loaded_ok || (RegQueryValueEx(key,NULL,NULL,&type,(BYTE *)this->path_cavern.get_buffer(),&length) != ERROR_SUCCESS)) {
    loaded_ok = false;
  	RegCloseKey(key);
  }
  if (loaded_ok)
  	RegCloseKey(key);
  if (type != REG_SZ)
    loaded_ok = false;
  if (loaded_ok) {
    thsplit_args(&mbf,this->path_cavern.get_buffer());
    this->path_cavern = "\"";
    this->path_cavern += *(mbf.get_buffer());
    this->path_cavern += "\"";
  } else {
    this->path_cavern = "cavern";
  }
#endif  

//  this->path_3dtopos = "3dtopos";
#ifdef THWIN32
  if (thcfg.install_tex) {
    this->path_mpost = thcfg.install_path.get_buffer();
    this->path_pdftex = thcfg.install_path.get_buffer();
    this->path_mpost += "\\bin\\win32\\mpost.exe";
    this->path_pdftex += "\\bin\\win32\\pdfetex.exe";
  } else {
#endif  
    this->path_mpost = "mpost";
    this->path_pdftex = "pdfetex";
#ifdef THWIN32
  }
#endif  
  this->tmp_path = "";
  this->tmp_remove_script = "";
  this->lang = THLANG_UNKNOWN;
  
  char * cmdln;
  char ** args;
  int nargs, argid; //, argid2;
  fontrecord frec;
  bool some_tex_fonts = false, started = false;
  this->ini_file.set_search_path(thcfg.get_initialization_path());
  this->ini_file.set_file_name("therion.ini");
  this->ini_file.sp_scan_on();
  this->ini_file.cmd_sensitivity_off();
  this->ini_file.print_if_opened(thinit__print_open, &started);
  this->ini_file.reset();
  try {
    while((cmdln = this->ini_file.read_line()) != NULL) {
      thsplit_args(& this->cmb, cmdln);
      args = this->cmb.get_buffer();
      nargs = this->cmb.get_size();
      argid = thmatch_token(args[0], thtt_initcmd);
      
      // check number of arguments:
      switch (argid) {      
        case TTIC_ENCODING_DEFAULT:
        case TTIC_PATH_CAVERN:
        case TTIC_TMP_PATH:
        case TTIC_LANG:
        case TTIC_TMP_REMOVE_SCRIPT:
        case TTIC_PATH_MPOST:
        case TTIC_PATH_PDFTEX:
        case TTIC_PATH_SOURCE:
          if (nargs != 2)
            ththrow(("invalid number of command arguments"));
          break;
        case TTIC_TEX_FONTS:
          if (nargs != 7)
            ththrow(("invalid number of command arguments"));
          break;
        default:
          ththrow(("invalid initialization command -- %s", args[0]))
      }
        
      switch(argid) {
        
        case TTIC_ENCODING_DEFAULT:
          this->encoding_default = thparse_encoding(args[1]);
          break;
          
        case TTIC_ENCODING_SQL:
          this->encoding_sql = thparse_encoding(args[1]);
          break;
          
        case TTIC_PATH_CAVERN:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_cavern.strcpy(args[1]);
          break;

        case TTIC_TMP_PATH:
          this->tmp_path.strcpy(args[1]);
          break;

        case TTIC_LANG:
          this->lang = thlang_parse(args[1]);
          if (this->lang == THLANG_UNKNOWN)
            ththrow(("language not supported -- %s",args[1]))
          break;

        case TTIC_TMP_REMOVE_SCRIPT:
          this->tmp_remove_script.strcpy(args[1]);
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
          
        case TTIC_PATH_SOURCE:
          thcfg.set_search_path(args[1]);
          break;
        
        case TTIC_TEX_FONTS:
          frec.id = get_enc_id(args[1]);
          if (frec.id < 0)
            ththrow(("tex encoding not supported -- %s", args[1]))
          frec.rm = args[2];
          frec.it = args[3];
          frec.bf = args[4];
          frec.ss = args[5];
          frec.si = args[6];
          FONTS.push_back(frec);
          some_tex_fonts = true;
          break;
          
        default:
          ththrow(("invalid initialization command -- %s", args[0]))
      }
    }
  }
  catch (...)
    threthrow(("%s [%d]", this->ini_file.get_cif_name(), this->ini_file.get_cif_line_number()))
  if (started) {
#ifdef THDEBUG
    thprintf("\n");
#else
    thprintf(" done\n");
    thtext_inline = false;
#endif 
  }
  if (!some_tex_fonts)
     init_encodings();
}


char * thinit::get_path_cavern()
{
  return this->path_cavern.get_buffer();
}

//char * thinit::get_path_3dtopos()
//{
//  return this->path_3dtopos.get_buffer();
//}

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


int thinit::get_encoding(int type) {
  int res;
  switch (type) {
    case THINIT_ENCODING_SQL:
      res = this->encoding_sql;
      break;
    default:
      res = this->encoding_default;
      break;
  }
  if (res == TT_UNKNOWN_ENCODING)
    res = this->encoding_default;
  return res;
}


thinit thini;


