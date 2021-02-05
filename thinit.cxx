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
#include "thlocale.h"
#include "thtmpdir.h"
#include "thcs.h"
#include "thproj.h"
#include "thpdfdbg.h"

#ifdef THWIN32
#include <windows.h>
#endif

const char * THCCC_INIT_FILE = "### Output character encodings ###\n"
"# encoding-default  ASCII\n"
"# encoding-sql  ASCII\n\n"
"### Default regional settings ###\n"
"# language  en_GB\n"
"# units metric\n\n"
"### Preferred loop closure method\n"
"# loop-closure survex\n\n"
"### Paths to called executable files ###\n"
"# mpost-path  \"mpost\"\n"
"# mpost-options  \"-tex=etex\"\n"
"# pdftex-path  \"pdftex\"\n"
"# cavern-path  \"cavern\"\n"
"# convert-path  \"convert\"\n"
"# identify-path  \"identify\"\n\n"
"### Search paths for source and configuration files ###\n"
"# source-path  \"\"\n\n"
"### Tex initialization ###\n"
"# tex-env off\n"
"# tex-fonts <encoding> <roman> <italic> <bold> <sansserif> <sansserifoblique>\n"
"# tex-fonts raw cmr10 cmti10 cmbx10 cmss10 cmssi10\n"
"# tex-fonts-optional xl2 csr10 csti10 csbx10 csss10 csssi10\n\n"
"# tex-fonts-optional cmcyr cmcyr10 cmcti10 cmcbx10 cmcss10 cmcssi10\n\n"
"### PDF fonts initialization\n"
"# otf2pfb off\n"
"# pdf-fonts <roman> <italic> <bold> <sansserif> <sansserifoblique>\n\n"
"### Path to temporary directory ###\n"
"# tmp-path  \"\"\n\n"
"### User defined coordinate system ###\n"
"# cs-def <id> <proj4id> [other options]\n\n"
"### Let PROJ v6+ find the optimal transformation ###\n"
"# proj-auto off\n\n"
"### PROJ v6+ handling of missing transformation grids if proj-auto is on ###\n"
"# proj-missing-grid warn\n\n"
"### Use count registers in TeX to store references to scraps; otherwise define control sequences ###\n"
"# tex-refs-registers on\n\n"
"### Command to remove temporary directory ###\n"
"# tmp-remove  \"\"\n\n";

thinit::thinit()
{
  this->fonts_ok = false;
  this->tex_env = false;
  this->lang = THLANG_UNKNOWN;
	this->loopc = THINIT_LOOPC_UNKNOWN;
}


thinit::~thinit()
{
}


enum {
  TTIC_ENCODING_DEFAULT,
  TTIC_ENCODING_SQL,
  TTIC_PATH_CAVERN,
//  TTIC_PATH_3DTOPOS,
  TTIC_PATH_CONVERT,
  TTIC_PATH_IDENTIFY,
  TTIC_PATH_MPOST,
  TTIC_OPT_MPOST,
  TTIC_PATH_PDFTEX,
  TTIC_PATH_SOURCE,
  TTIC_TMP_PATH,
  TTIC_TMP_REMOVE_SCRIPT,
  TTIC_LANG,
  TTIC_TEX_FONTS,
  TTIC_TEX_FONTS_OPTIONAL,
  TTIC_TEX_ENV,
  TTIC_UNITS,
  TTIC_LOOPC,
  TTIC_TEXT,	
  TTIC_PDF_FONTS,
  TTIC_PROJ_AUTO,
  TTIC_PROJ_MISSING_GRID,
  TTIC_OTF2PFB,
  TTIC_TEX_REFS_REGISTERS,
  TTIC_CS_DEF,
  TTIC_UNKNOWN,
};


/**
 * Data types parsing table.
 */
 
static const thstok thtt_initcmd[] = {
  {"cavern-path", TTIC_PATH_CAVERN},
  {"convert-path", TTIC_PATH_CONVERT},
  {"cs-def", TTIC_CS_DEF},
  {"encoding-default", TTIC_ENCODING_DEFAULT},
  {"encoding-sql", TTIC_ENCODING_SQL},
//  {"encoding_default", TTIC_ENCODING_DEFAULT},
//  {"path_3dtopos", TTIC_PATH_3DTOPOS},
  {"identify-path", TTIC_PATH_IDENTIFY},
  {"language", TTIC_LANG},
  {"loop-closure", TTIC_LOOPC},
  {"mpost-options", TTIC_OPT_MPOST},
  {"mpost-path", TTIC_PATH_MPOST},
  {"otf2pfb", TTIC_OTF2PFB},
  {"pdf-fonts", TTIC_PDF_FONTS},
  {"pdftex-path", TTIC_PATH_PDFTEX},
  {"proj-auto", TTIC_PROJ_AUTO},
  {"proj-missing-grid", TTIC_PROJ_MISSING_GRID},
  {"source-path", TTIC_PATH_SOURCE},
  {"tex-env",TTIC_TEX_ENV},
  {"tex-fonts",TTIC_TEX_FONTS},
  {"tex-fonts-optional",TTIC_TEX_FONTS_OPTIONAL},
  {"tex-refs-registers",TTIC_TEX_REFS_REGISTERS},
  {"text",TTIC_TEXT},
  {"tmp-path",TTIC_TMP_PATH},
  {"tmp-remove",TTIC_TMP_REMOVE_SCRIPT},
  {"units",TTIC_UNITS},
  {NULL, TTIC_UNKNOWN},
};

void thinit_print_open(char * s) {
#ifdef THDEBUG
    thprintf("\ninitialization file: %s\nreading\n", s);
#else
    thprintf("initialization file: %s\n", s);
    thprintf("reading ...");
    thtext_inline = true;
#endif 
}


static const thstok thtt_loopc[] = {
  {"survex", THINIT_LOOPC_SURVEX},
  {"therion", THINIT_LOOPC_THERION},
  {NULL, THINIT_LOOPC_UNKNOWN},
};


#ifdef THWIN32
thbuffer short_path_buffer;
void thinit__make_short_path(thbuffer * bf) {
  DWORD rv;
  size_t sl;
  sl = strlen(bf->get_buffer());
  if (sl == 0)
    return;
  short_path_buffer.guarantee(2 * sl);
  rv = GetShortPathName(bf->get_buffer(), short_path_buffer.get_buffer(), (DWORD) short_path_buffer.size);
  if ((rv > 0) && (rv < (DWORD) short_path_buffer.size)) {
    bf->strcpy(short_path_buffer.get_buffer());
  }
}
#endif


void thinit::copy_fonts() {

  if (ENC_NEW.NFSS == 0) return;

  static thbuffer tmpb;
  int retcode;

  if (fonts_ok) return;

  thprintf("copying_fonts ...\n");

  for(int index = 0; index < 5; index++) {
    thprintf("%s\n", font_dst[index].c_str());
  // skopiruje font subor
#ifdef THWIN32
    tmpb = "copy \"";
#else
    tmpb = "cp \"";
#endif
    tmpb += font_src[index].c_str();
    tmpb += "\" \"";
    tmpb += thtmp.get_file_name(font_dst[index].c_str());
    tmpb += "\"";

#ifdef THWIN32
    char * cpcmd;
    size_t  cpch;
    cpcmd = tmpb.get_buffer();
    for(cpch = 0; cpch < strlen(cpcmd); cpch++) {
      if (cpcmd[cpch] == '/') {
        cpcmd[cpch] = '\\';
      }
    }
#endif

#ifdef THDEBUG
    thprintf("copying font\n");
#endif
    retcode = system(tmpb.get_buffer());
    if (retcode != EXIT_SUCCESS)
      ththrow(("unable to copy font file -- %s", font_src[index].c_str()))
  }

#ifdef THWIN32
  FILE * f = fopen(thtmp.get_file_name("pltotf.bat"),"w");
  fprintf(f,"@\"%s\\bin\\win32\\pltotf.exe\" %%1 %%2\n",thcfg.install_path.get_buffer());
  fclose(f);
  f = fopen(thtmp.get_file_name("cfftot1.bat"),"w");
  fprintf(f,"@\"%s\\bin\\win32\\cfftot1.exe\" %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9\n",thcfg.install_path.get_buffer());
  fclose(f);
#endif
  thprintf("done.\n");
  fonts_ok = true;
}


int thinit::get_lang() {
  return thlang_getlang(this->lang);
}



void thinit::check_font_path(const char * fname, int index) {

  static thbuffer pfull, pshort, tmpb;

  pshort.strcpy("");
  pfull.strcpy("");
  long i, l;
  tmpb = fname;
  char * buff = tmpb.get_buffer();
  l = (long) strlen(buff);
  bool search_sn = true;
  if (l == 0) ththrow(("missing font file name"));
  for(i = (l-1); i >= 0; i--) {
    if ((buff[i] == '/') || (buff[i] == '\\')) {
      if (search_sn) {
        pshort.strcpy(&(buff[i+1]));
        search_sn = false;
      }
      buff[i] = '/';
    }
  }
  
  if (strlen(pshort.get_buffer()) == 0) ththrow(("invalid font name -- %s", fname))

  if ( 
#ifdef THWIN32
      ((l > 1) && (buff[1] == ':')) ||
#endif
      buff[0] == '/') {
    pfull.strcpy(buff);
  } else {
    if (strlen(this->ini_file.get_cif_path()) > 0) {
      pfull.strcpy(this->ini_file.get_cif_path());
      pfull += "/";
    } else {
      pfull = "";
    }
    pfull += buff;
  }

  // checkne ci TTF
  if ((l > 3) && icase_equals(&(buff[l-4]), ".ttf")) ENC_NEW.t1_convert = 0;

  font_src[index] = pfull.get_buffer();
  font_dst[index] = pshort.get_buffer();
  ENC_NEW.otf_file[index] = pshort.get_buffer();

}



void thinit::load()
{

  // set encodings
  this->encoding_default = TT_ASCII;
  this->encoding_sql = TT_UNKNOWN_ENCODING;

  ENC_NEW.NFSS = 0;
  ENC_NEW.t1_convert = 1;
  this->opt_mpost = "";

  
  // set programs paths
  this->path_cavern = "cavern";

#ifdef THWIN32
  // set cavern path according to Windows registers
  this->path_cavern.guarantee(1024);
  thmbuffer mbf;
  thbuffer bf;
  DWORD type(0), length = 1024;
  HKEY key;
  bool loaded_ok = true;
  if (RegOpenKey(HKEY_CLASSES_ROOT,"survex.source\\shell\\Process\\command",&key) != ERROR_SUCCESS)
    loaded_ok = false;
  if (!loaded_ok) {  // VG120416: search also the HKCU in case an old Survex versions was installed as non-admin
    if (RegOpenKey(HKEY_CURRENT_USER,"survex.source\\shell\\Process\\command",&key) != ERROR_SUCCESS)
      loaded_ok = false;
  }
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
    this->path_cavern = *(mbf.get_buffer());
    // VG 120416: Replace "aven.exe" with "cavern.exe". New Survex versions write up aven, but therion needs cavern
    // See http://mailman.speleo.sk/pipermail/therion/2015-September/006072.html
    int pathlen = strlen(this->path_cavern.get_buffer());
    int suflen = strlen("aven.exe");
    if ((pathlen > suflen) && (strncmp(this->path_cavern.get_buffer() + pathlen - suflen, "aven.exe", suflen ) == 0)) {
      this->path_cavern.get_buffer()[pathlen - strlen("aven.exe")] = 0;
      this->path_cavern.strcat("cavern.exe");
    }
  } else {
    this->path_cavern = "cavern";
  }
#endif  

	// try running survex
	if (this->loopc == THINIT_LOOPC_UNKNOWN) {
	  thbuffer svxcom;
	  svxcom = "\"";
	  svxcom += thini.get_path_cavern();
	  svxcom += "\" --version";  
#ifdef THDEBUG
	  thprintf("testing cavern\n");
#endif
	  if (system(svxcom.get_buffer()) == EXIT_SUCCESS) {
			this->loopc = THINIT_LOOPC_SURVEX;
		} else {
			this->loopc = THINIT_LOOPC_THERION;
		}
	}

//  this->path_3dtopos = "3dtopos";
#ifdef THWIN32
  if (thcfg.install_tex) {
    this->path_mpost = thcfg.install_path.get_buffer();
    this->path_pdftex = thcfg.install_path.get_buffer();
    this->path_otftotfm = thcfg.install_path.get_buffer();
    this->path_mpost += "\\bin\\win32\\mpost.exe";
    this->path_pdftex += "\\bin\\win32\\pdftex.exe";
    this->path_otftotfm += "\\bin\\win32\\otftotfm.exe";
  } else {
#endif  
    this->path_mpost = "mpost";
    this->path_pdftex = "pdftex";
    this->path_otftotfm = "otftotfm";
#ifdef THWIN32
  }
#endif  

#ifdef THWIN32
  if (thcfg.install_im) {
    this->path_convert = thcfg.install_path.get_buffer();
    this->path_convert += "\\bin\\convert.exe";
    this->path_identify = thcfg.install_path.get_buffer();
    this->path_identify += "\\bin\\identify.exe";
  } else {
#endif  
    this->path_convert = "convert";
    this->path_identify = "identify";
#ifdef THWIN32
  }
#endif  

  set_proj_lib_path();

  this->tmp_path = "";
  this->tmp_remove_script = "";
  this->lang = THLANG_UNKNOWN;
  
  char * cmdln;
  char ** args;
  int nargs, argid, sv; //, argid2;
  fontrecord frec;
  bool some_tex_fonts = false, started = false;
  this->ini_file.set_search_path(thcfg.get_initialization_path());
  this->ini_file.set_file_name("therion.ini");
  this->ini_file.sp_scan_on();
  this->ini_file.cmd_sensitivity_off();
  this->ini_file.print_if_opened(thinit_print_open, &started);
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
        case TTIC_PATH_CONVERT:
        case TTIC_PATH_IDENTIFY:
        case TTIC_LOOPC:
        case TTIC_TMP_PATH:
        case TTIC_LANG:
        case TTIC_UNITS:
        case TTIC_TMP_REMOVE_SCRIPT:
        case TTIC_PATH_MPOST:
        case TTIC_OPT_MPOST:
        case TTIC_PATH_PDFTEX:
        case TTIC_PATH_SOURCE:
        case TTIC_OTF2PFB:
        case TTIC_TEX_REFS_REGISTERS:
        case TTIC_TEX_ENV:
        case TTIC_PROJ_AUTO:
        case TTIC_PROJ_MISSING_GRID:
          if (nargs != 2)
            ththrow(("invalid number of command arguments"));
          break;
        case TTIC_TEXT:
          if (nargs != 4)
            ththrow(("invalid text syntax -- should be: text <language> <text> <translation>"))
          break;
        case TTIC_PDF_FONTS:
          if (nargs != 6)
            ththrow(("invalid number of command arguments"));
          break;
        case TTIC_TEX_FONTS:
        case TTIC_TEX_FONTS_OPTIONAL:
          if (nargs != 7)
            ththrow(("invalid number of command arguments"));
          break;
        case TTIC_CS_DEF:
          if (nargs < 2)
            ththrow(("invalid cs-def syntax -- should be: cs-def <id> <proj4id> [other options]"))
          break;
        default:
          ththrow(("invalid initialization command -- %s", args[0]))
      }
        
      switch(argid) {

        case TTIC_CS_DEF:
          thcs_add_cs(args[1], args[2], nargs - 2, &(args[3]));
          break;
        
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
          
        case TTIC_PATH_CONVERT:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_convert.strcpy(args[1]);
          break;
          
        case TTIC_PATH_IDENTIFY:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_identify.strcpy(args[1]);
          break;

        case TTIC_TMP_PATH:
          this->tmp_path.strcpy(args[1]);
          break;

        case TTIC_LANG:
          this->lang = thlang_parse(args[1]);
          if (this->lang == THLANG_UNKNOWN)
            ththrow(("language not supported -- %s",args[1]))
          break;

        case TTIC_UNITS:
          thdeflocale.parse_units(args[1]);
          break;

        case TTIC_OTF2PFB:
          sv = thmatch_token(args[1], thtt_bool);
          if (sv == TT_UNKNOWN_BOOL)
            ththrow(("invalid otf2pfb switch -- %s", args[1]))
          ENC_NEW.t1_convert = (sv == TT_TRUE);
          break;

        case TTIC_TEX_REFS_REGISTERS:
          sv = thmatch_token(args[1], thtt_bool);
          if (sv == TT_UNKNOWN_BOOL)
            ththrow(("invalid tex-refs-registers switch -- %s", args[1]))
          tex_refs_registers = (sv == TT_TRUE);
          break;

        case TTIC_PROJ_AUTO:
          sv = thmatch_token(args[1], thtt_bool);
          if (sv == TT_UNKNOWN_BOOL)
            ththrow(("invalid proj-auto switch -- %s", args[1]))
          thcs_cfg.proj_auto = (sv == TT_TRUE);
          break;

        case TTIC_PROJ_MISSING_GRID:
          sv = thcs_parse_gridhandling(args[1]);
          if (sv == GRID_INVALID)
            ththrow(("invalid proj-missing-grid switch -- %s", args[1]))
          thcs_cfg.proj_auto_grid = sv;
          break;

        case TTIC_TEXT:
            thlang_set_translation(args[1], args[2], args[3]);
            break;

        case TTIC_TMP_REMOVE_SCRIPT:
          this->tmp_remove_script.strcpy(args[1]);
          break;

        case TTIC_PATH_MPOST:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_mpost.strcpy(args[1]);
          break;

        case TTIC_OPT_MPOST:
          this->opt_mpost.strcpy(args[1]);
          break;

        case TTIC_PATH_PDFTEX:
          if (strlen(args[1]) < 1)
            ththrow(("invalid path"))
          this->path_pdftex.strcpy(args[1]);
          break;
          
        case TTIC_PATH_SOURCE:
          thcfg.set_search_path(args[1]);
          break;
        
        case TTIC_TEX_FONTS_OPTIONAL:
        case TTIC_TEX_FONTS:
          frec.id = get_enc_id(args[1]);
          if (frec.id < 0)
            ththrow(("tex encoding not supported -- %s", args[1]))
          frec.rm = args[2];
          frec.it = args[3];
          frec.bf = args[4];
          frec.ss = args[5];
          frec.si = args[6];
          frec.opt = (argid == TTIC_TEX_FONTS_OPTIONAL);
          FONTS.push_back(frec);
          some_tex_fonts = true;
          break;

        case TTIC_PDF_FONTS:
          // TODO: parsne nazvy suborov, nastavi t1_convert, 
          ENC_NEW.NFSS = 1;
          check_font_path(args[1], 0);
          check_font_path(args[2], 1);
          check_font_path(args[3], 2);
          check_font_path(args[4], 3);
          check_font_path(args[5], 4);
          break;
          
        case TTIC_TEX_ENV:
          sv = thmatch_token(args[1], thtt_bool);
          if (sv == TT_UNKNOWN_BOOL)
            ththrow(("invalid tex-env switch -- %s", args[1]))
          this->tex_env = (sv == TT_TRUE);
          break;
          
        case TTIC_LOOPC:
          sv = thmatch_token(args[1], thtt_loopc);
          if (sv == THINIT_LOOPC_UNKNOWN)
            ththrow(("invalid loop-closure switch -- %s", args[1]))
          this->loopc = sv;
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

#ifdef THWIN32
  thinit__make_short_path(&this->path_cavern);
  thinit__make_short_path(&this->path_convert);
  thinit__make_short_path(&this->path_identify);
  thinit__make_short_path(&this->path_mpost);
  thinit__make_short_path(&this->path_pdftex);
#endif

  // check if optional fonts are in the system, remove them if not
#ifdef THWIN32
  if (!thini.tex_env) {
    putenv("TEXMFCNF=");
    putenv("DVIPSHEADERS=");
    putenv("GFFONTS=");
    putenv("GLYPHFONTS=");
    putenv("MFBASES=");
    putenv("MFINPUTS=");
    putenv("MFPOOL=");
    putenv("MPINPUTS=");
    putenv("MPMEMS=");
    putenv("MPPOOL=");
    putenv("MPSUPPORT=");
    putenv("PKFONTS=");
    putenv("PSHEADERS=");
    putenv("T1FONTS=");
    putenv("T1INPUTS=");
    putenv("T42FONTS=");
    putenv("TEXCONFIG=");
    putenv("TEXDOCS=");
    putenv("TEXFONTMAPS=");
    putenv("TEXFONTS=");
    putenv("TEXFORMATS=");
    putenv("TEXINPUTS=");
    putenv("TEXMFDBS=");
    putenv("TEXMFINI=");
    putenv("TEXPICTS=");
    putenv("TEXPKS=");
    putenv("TEXPOOL=");
    putenv("TEXPSHEADERS=");
    putenv("TEXSOURCES=");
    putenv("TFMFONTS=");
    putenv("TTFONTS=");
    putenv("VFFONTS=");
    putenv("WEB2C=");
#ifdef THMSVC
    putenv("TEXINPUTS=../tex;../../therion.prj/Setup/texmf/tex;.");
    putenv("MPINPUTS=../mpost;.");
    if (ENC_NEW.NFSS == 1) {
      putenv("TEXFONTS=.");
      putenv("T1FONTS=.");
      putenv("TTFFONTS=.");
    }
#endif
  }
#endif

  list<fontrecord> TMPFONTS;

  for (list<fontrecord>::iterator J = FONTS.begin(); J != FONTS.end(); J++) {
    if (J->opt) {
      FILE * ff = fopen(thtmp.get_file_name("fonttest.tex"),"w");
      fprintf(ff,"\\nopagenumbers\n\\batchmode\n\\def\\fonttest#1{\\font\\a=#1\\a}\n\\fonttest{%s}\n\\fonttest{%s}\n\\fonttest{%s}\n\\fonttest{%s}\n\\fonttest{%s}\n\\end", J->rm.c_str(), J->it.c_str(), J->bf.c_str(), J->ss.c_str(), J->si.c_str());
      fclose(ff);

      thbuffer com, wdir;
      wdir.guarantee(1024);
      thassert(getcwd(wdir.get_buffer(),1024) != NULL);
      thassert(chdir(thtmp.get_dir_name()) == 0);
      int retcode;

      com = "\"";
      com += this->get_path_pdftex();
      com += "\"";
    //  com += " --interaction nonstopmode data.tex";
      com += " --no-mktex=tfm fonttest.tex";
      retcode = system(com.get_buffer());
      thprintf("checking optional fonts %s %s %s %s %s ...", J->rm.c_str(), J->it.c_str(), J->bf.c_str(), J->ss.c_str(), J->si.c_str());
      if (retcode != EXIT_SUCCESS) {
        thprintf(" NOT INSTALLED\n");
      } else {
        TMPFONTS.push_back(*J);
        thprintf(" OK\n");
      }
      thassert(chdir(wdir.get_buffer()) == 0);
    } else {
      TMPFONTS.push_back(*J);
    }
  }

  FONTS.clear();
  for (list<fontrecord>::iterator J = TMPFONTS.begin(); J != TMPFONTS.end(); J++) {
    FONTS.push_back(*J);
  }

}


char * thinit::get_path_cavern()
{
  return this->path_cavern.get_buffer();
}


char * thinit::get_path_convert()
{
  return this->path_convert.get_buffer();
}


char * thinit::get_path_identify()
{
  return this->path_identify.get_buffer();
}

//char * thinit::get_path_3dtopos()
//{
//  return this->path_3dtopos.get_buffer();
//}

char * thinit::get_path_mpost()
{
  return this->path_mpost.get_buffer();
}

char * thinit::get_opt_mpost()
{
  return this->opt_mpost.get_buffer();
}

char * thinit::get_path_pdftex()
{
  return this->path_pdftex.get_buffer();
}

char * thinit::get_path_otftotfm()
{
  return this->path_otftotfm.get_buffer();
}


void thinit::set_proj_lib_path() {  // set PROJ library resources path
#ifdef THWIN32
  putenv((std::string("PROJ_LIB=")+thcfg.install_path.get_buffer()+"\\lib\\proj-" + std::to_string(PROJ_VER)).c_str());
#endif
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


