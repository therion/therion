/**
 * @file thconfig.cxx
 * Configuration module.
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

#include "thconfig.h"
#include "therion.h"
#include "thparse.h"
#include "thlang.h"
#include <stdlib.h>
#include <stdio.h>
#include "thchenc.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thdatareader.h"
#include "thdataobject.h"
#include "thcsdata.h"
#include "thproj.h"
#include "thlogfile.h"
#include "thgeomag.h"
#include "thgeomagdata.h"
#include "thlayout.h"
#include "thsketch.h"
#ifdef THWIN32
#include <windows.h>
#endif


enum {
  TT_UNKNOWN_CFG, 
  TT_SOURCE, 
  TT_ENDSOURCE, 
  TT_SELECT, 
  TT_UNSELECT, 
  TT_EXPORT, 
  TT_OUTCS,
  TT_SETUP3D,
  TT_AUTO_JOIN,
  TT_SYSTEM,
  TT_SKETCH_WARP,
  TT_TEXT,
};


static const thstok thtt_cfg[] = {
//  {"auto-join", TT_AUTO_JOIN},
  {"cs", TT_OUTCS},
  {"endsource", TT_ENDSOURCE},
  {"export", TT_EXPORT},
  {"select", TT_SELECT},
  {"setup3d", TT_SETUP3D},
  {"sketch-warp", TT_SKETCH_WARP},
  {"source", TT_SOURCE},
  {"system", TT_SYSTEM},
  {"text", TT_TEXT},
  {"unselect", TT_UNSELECT},
  {NULL, TT_UNKNOWN_CFG}
};


//const char * THCCC_ENCODING = "# Character encoding of this configuration "//  "file.\n";
const char * THCCC_ENCODING = "";
const char * THCCC_SOURCE = "# Name of the source file.\n";


thconfig::thconfig()
{

  this->install_path.strcpy("");
  this->install_tex = false;
  this->install_tcltk = false;
  this->install_im = false;

  this->auto_join = true;
  this->outcs = TTCS_LOCAL;
  this->outcs_sumx = 0.0;
  this->outcs_sumy = 0.0;
  this->outcs_sumn = 0.0;

  this->tmp3dSMP = 1.0;
  this->tmp3dWALLSMP = 1.0;
  this->tmp3dMAXDIMD = 5.0;
  this->sketch_warp = THSKETCH_WARP_IDLINE;

#ifdef THWIN32
  thbuffer * tmpbf = &(this->bf1);
  // set search path according to Windows registers
  tmpbf->guarantee(1024);
  DWORD type, length = 1024;
  HKEY key;
  bool loaded_ok = true;
	if (RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Therion",&key) != ERROR_SUCCESS) {
    loaded_ok = false;
  	RegCloseKey(key);
  }
  if (!loaded_ok) {
    loaded_ok = true;
	  if (RegOpenKey(HKEY_CURRENT_USER,"SOFTWARE\\Therion",&key) != ERROR_SUCCESS) {
      loaded_ok = false;
  	  RegCloseKey(key);
    }
  }
  if (loaded_ok) {
    if (RegQueryValueEx(key,"InstallDir",NULL,&type,(BYTE *)tmpbf->get_buffer(),&length) != ERROR_SUCCESS) {
      tmpbf->strcpy("");
      this->install_path = "";
    } else {
      this->install_path = tmpbf->get_buffer();
      if (RegQueryValueEx(key,"TeX",NULL,&type,(BYTE *)tmpbf->get_buffer(),&length) == ERROR_SUCCESS)
        this->install_tex = true;
      if (RegQueryValueEx(key,"TclTk",NULL,&type,(BYTE *)tmpbf->get_buffer(),&length) == ERROR_SUCCESS)
        this->install_tcltk = true;
      if (RegQueryValueEx(key,"ImageMagick",NULL,&type,(BYTE *)tmpbf->get_buffer(),&length) == ERROR_SUCCESS)
        this->install_im = true;
    }
  	RegCloseKey(key);
  }
  if (type != REG_SZ)
    loaded_ok = false;
  const char * winini = "C:/WINDOWS;C:/WINNT;C:/Program files/Therion";
  const char * wincfg = "C:/Program files/Therion";
#else
  const char * unixini = "/etc:/usr/etc:/usr/local/etc";
  const char * unixcfg = "/usr/share/therion:/usr/local/share/therion";
#endif
  
  this->fname = "thconfig";
  this->skip_comments = false;
  this->generate_xthcfg = false;
  this->cfg_fenc = TT_UTF_8;
  this->fstate = THCFG_READ;

  char * sp = NULL;
  if (sp != NULL)
    this->search_path = sp;
  else {
    sp = getenv("HOME");
#ifdef THWIN32
    if (sp != NULL) {
      this->search_path = sp;
    } else {
      sp = getenv("HOMEDRIVE");
      if (sp != NULL) {
        this->search_path = sp;
        sp = getenv("HOMEPATH");
        if (sp != NULL) {
          this->search_path += sp;
        }
      }
      sp = getenv("HOMEDRIVE");
    }
#endif    
    if (sp != NULL) {
#ifdef THWIN32
      this->search_path += "\\.therion;";
      if (strlen(this->install_path.get_buffer()) > 0) {
        this->search_path += this->install_path.get_buffer();
      } else {
        this->search_path += wincfg;
      }
#else
      this->search_path = sp;
      this->search_path += "/.therion:";
      this->search_path += unixcfg;
#endif
    }
    else {
#ifdef THWIN32
      if (strlen(this->install_path.get_buffer()) > 0) {
        this->search_path += this->install_path.get_buffer();
      } else {
        this->search_path += wincfg;
      }
#else
      this->search_path += unixcfg;
#endif
    }
  }


  sp = getenv("THERION");
  if (sp != NULL)
    this->init_path = sp;
  else {
    sp = getenv("HOME");
#ifdef THWIN32
    if (sp != NULL) {
      this->init_path = sp;
    } else {
      sp = getenv("HOMEDRIVE");
      if (sp != NULL) {
        this->init_path = sp;
        sp = getenv("HOMEPATH");
        if (sp != NULL) {
          this->init_path += sp;
        }
      }
      sp = getenv("HOMEDRIVE");
    }
#endif    
    if (sp != NULL) {
#ifdef THWIN32
      this->init_path += "\\.therion;";
      if (strlen(this->install_path.get_buffer()) > 0) {
        this->init_path += this->install_path.get_buffer();
      } else {
        this->init_path += winini;
      }
#else
      this->init_path = sp;
      this->init_path += "/.therion:";
      this->init_path += unixini;
#endif
    }
    else {
#ifdef THWIN32
      if (strlen(this->install_path.get_buffer()) > 0) {
        this->init_path += this->install_path.get_buffer();
      } else {
        this->init_path += winini;
      }
#else
      this->init_path += unixini;
#endif
    }
  }

  this->exporter.assign_config(this);
  this->selector.assign_config(this);
}


thconfig::~thconfig()
{
}


void thconfig::set_file_name(char * fn)
{
  this->fname = fn;
  this->cfg_file.report_missing = true;
}

   
char * thconfig::get_file_name()
{
  return this->fname;
}
  

void thconfig::append_source(char * fname, long startln, long endln)
{
  thconfig_src xsrc;
  xsrc.fname = this->src_fnames.append(fname);
  xsrc.startln = startln;
  xsrc.endln = endln;
  this->src.push_back(xsrc);
}

  
thconfig_src_list * thconfig::get_sources()
{
  return & this->src;
}  


void thconfig::set_comments_skip(bool state)
{
  this->skip_comments = state;
}
  

void thconfig::comments_skip_on()
{
  this->skip_comments = true;
}
  

void thconfig::comments_skip_off()
{
  this->skip_comments = false;
}
  

bool thconfig::get_comments_skip()
{
  return this->skip_comments;
}


void thconfig::set_file_state(thcfg_fstate fs)
{
  this->fstate = fs;
}
 
 
thcfg_fstate thconfig::get_file_state()
{
  return this->fstate;
}


void thconfig::set_search_path(char * pth)
{
  this->search_path = pth;
}

  
char * thconfig::get_search_path()
{
  return this->search_path;
}

char * thconfig::get_initialization_path()
{
  return this->init_path;
}


void thconfig__pifo(char * s) {
#ifdef THDEBUG
  thprintf("\nconfiguration file: %s\nreading\n",s);
#else
  thprintf("configuration file: %s\n",s);
  thprintf("reading ...");
  thtext_inline = true;
#endif 
}

 
static const thstok thtt_sketchwarp[] = {
  {"line", THSKETCH_WARP_IDLINE},
  {"linear", THSKETCH_WARP_LINEAR},
  {"plaquette", THSKETCH_WARP_PLAQUETTE},
  {"point", THSKETCH_WARP_IDPOINT},
  {NULL, THSKETCH_WARP_UNKNOWN},
};



void thconfig::load() 
{
  thmbuffer valuemb;
  int sv;
  long srcstart(0), srcend(-1);
  bool fstarted  = false;
  bool source_mode = false;
  if ((this->fstate == THCFG_UPDATE) || (this->fstate == THCFG_READ)) {
    this->cfg_file.cmd_sensitivity_on();
    this->cfg_file.sp_scan_off();
    this->cfg_file.set_file_name(this->fname);
    this->cfg_file.print_if_opened(thconfig__pifo, &fstarted);
    this->cfg_file.reset();
    try {
      char * cfgln = this->cfg_file.read_line();
      while(cfgln != NULL) {
        this->cfg_fenc = this->cfg_file.get_cif_encoding();
        thsplit_args(&valuemb, this->cfg_file.get_value());
        sv = thmatch_token(this->cfg_file.get_cmd(),thtt_cfg);

        if (source_mode) {
          if (sv == TT_ENDSOURCE) {
            source_mode = false;
            srcend = this->cfg_file.get_cif_line_number() - 1;
            if (srcend >= srcstart) {
              this->append_source(this->cfg_file.get_cif_name(), srcstart, srcend);
            }

            this->cfg_file.set_input_sensitivity(true);
          }
        } else switch (sv) {
  
          case TT_SOURCE:
            if (valuemb.get_size() == 0) {
              source_mode = true;
              srcstart = this->cfg_file.get_cif_line_number() + 1;
              this->cfg_file.set_input_sensitivity(false);
            } else {
              if (valuemb.get_size() > 1)
                ththrow(("one file name expected"))            
              this->append_source(valuemb.get_buffer()[0]);
#ifdef THWIN32
              this->search_path.strcat(";");
#else
              this->search_path.strcat(":");
#endif
              this->search_path.strcat(this->cfg_file.get_cif_path());
            }
            break;
            
          case TT_SKETCH_WARP:
            if (valuemb.get_size() != 1)
              ththrow(("single sketch-warp switch expected"));
            sv = thmatch_token(valuemb.get_buffer()[0],thtt_sketchwarp);
            if (sv == THSKETCH_WARP_UNKNOWN)
              ththrow(("invalid sketch-warp switch -- %s", valuemb.get_buffer()[0]));
            this->sketch_warp = sv;
            break;

          case TT_SETUP3D:
            if (valuemb.get_size() > 0) {
              thparse_double(sv, this->tmp3dSMP, valuemb.get_buffer()[0]);
              if ((sv != TT_SV_NUMBER) || (this->tmp3dSMP <= 0.0))
                ththrow(("invalid number -- %s", valuemb.get_buffer()[0]))
              this->tmp3dWALLSMP = this->tmp3dSMP;
            }
            if (valuemb.get_size() > 1) {
              thparse_double(sv, this->tmp3dWALLSMP, valuemb.get_buffer()[1]);
              if ((sv != TT_SV_NUMBER) || (this->tmp3dWALLSMP <= 0.0))
                ththrow(("invalid number -- %s", valuemb.get_buffer()[1]))
            }
            if (valuemb.get_size() > 2) {
              thparse_double(sv, this->tmp3dMAXDIMD, valuemb.get_buffer()[2]);
              if ((sv != TT_SV_NUMBER) || (this->tmp3dMAXDIMD <= 0.0))
                ththrow(("invalid number -- %s", valuemb.get_buffer()[2]))
            }
            break;

          case TT_AUTO_JOIN:
            if (valuemb.get_size() > 0) {
              sv = thmatch_token(valuemb.get_buffer()[0], thtt_bool);
              if (sv == TT_UNKNOWN_BOOL)
                ththrow(("invalid auto-join sqitch -- %s", valuemb.get_buffer()[0]))
              this->auto_join = (sv == TT_TRUE);
            } else {
              ththrow(("missing auto-join switch"))
            }
            break;
  
          case TT_SELECT:
            this->selector.parse_selection(false, valuemb.get_size(), valuemb.get_buffer());
            break;

          case TT_SYSTEM:
            if (valuemb.get_size() == 0)
              ththrow(("missing system command"))
            if (valuemb.get_size() > 1)
              ththrow(("single system command expected"))
            this->exporter.parse_system(valuemb.get_buffer()[0]);
            break;

          case TT_TEXT:
            if ((valuemb.get_size() < 3) || (valuemb.get_size() > 3))
              ththrow(("invalid text syntax -- should be: text <language> <text> <translation>"))
            thlang_set_translation(valuemb.get_buffer()[0], valuemb.get_buffer()[1], valuemb.get_buffer()[2]);
            break;

          case TT_OUTCS:
            if (valuemb.get_size() == 1) {
              sv = thcasematch_token(valuemb.get_buffer()[0], thtt_cs);
              if (sv == TTCS_UNKNOWN)
                ththrow(("unknown coordinate system -- %s", valuemb.get_buffer()[0]))
              if (!thcsdata_table[sv].output)
                ththrow(("%s coordinate system not supported for output", valuemb.get_buffer()[0]))
              this->outcs = sv;
              this->outcs_def.name = this->get_db()->strstore(this->cfg_file.get_cif_name(), true);
              this->outcs_def.line = this->cfg_file.get_cif_line_number();
            } else {
              ththrow(("output coordinate system specification requires single parameter"))
            }
            break;
            
          case TT_UNSELECT:
            this->selector.parse_selection(true, valuemb.get_size(), valuemb.get_buffer());
            break;
            
          case TT_EXPORT:
            this->exporter.parse_export(valuemb.get_size(), valuemb.get_buffer());
            break;
  
          default:
	          // skusi povolene databazove prikazy
            switch (thmatch_token(this->cfg_file.get_cmd(),thtt_commands)) {
            
              case TT_LAYOUT_CMD:
                this->load_dbcommand(&valuemb);
                break;

              default:
                ththrow(("unknown configuration command -- %s", this->cfg_file.get_cmd()));
                
	          }
        }
        cfgln = this->cfg_file.read_line(); 
      }
      if (source_mode) {
        ththrow(("endsource expected"));
      }
    }
    catch (...)
      threthrow(("%s [%d]", this->cfg_file.get_cif_name(), this->cfg_file.get_cif_line_number()));
  }
  
  if (fstarted) {
#ifdef THDEBUG
    thprintf("\n");
#else
    thprintf(" done\n");
    thtext_inline = false;
#endif    
  }
}


void thconfig::load_dbcommand(thmbuffer * valmb) {

  // vytvori objekt
  // analyzuje jeho argumenty a prida riadok do dblines
  // nacita ostatne riadky, ak je to potrebne a analyzuje ich
  // vlozi ho do databazy
  // kazdy riadok prida do prikazov na ulozenie

  thdataobject * objptr;  // pointer to the newly created object
  thcmd_option_desc optd;  // option descriptor
  char * ln, * opt;
  char ** opts;
  const char * endlnopt = NULL;
  int ai, ait, ant;
  thobjectsrc osrc;
  bool inside_cmd = false;
  
  try {

    // command source
    osrc.line = this->cfg_file.get_cif_line_number();
    if (strcmp(osrc.name, this->cfg_file.get_cif_name()) != 0)
      osrc.name = dbptr->strstore(this->cfg_file.get_cif_name(), true);
    dbptr->csrc.line = osrc.line;
    dbptr->csrc.name = osrc.name;


    objptr = dbptr->create(this->cfg_file.get_cmd(), osrc);
    if (objptr == NULL)
      ththrow(("unknown command -- %s", this->cfg_file.get_cmd()));

    if (objptr->get_class_id() == TT_LAYOUT_CMD) {
      ((thlayout*)objptr)->m_pconfig = this;
    }

    thencode(&this->bf1, this->cfg_file.get_line(), this->cfg_file.get_cif_encoding());  
    this->cfg_dblines.append(this->bf1.get_buffer());  

    // analyze the commands options

    // first, let's parse arguments
    ant = valmb->get_size();
    opts = valmb->get_buffer();
    if (ant < objptr->get_cmd_nargs())
      ththrow(("not enough command arguments -- must be %d",
        objptr->get_cmd_nargs()));
    optd.nargs = 1;

    // set obligatory arguments
    for (ai = 0; ai < objptr->get_cmd_nargs(); ai++, opts++) {
      optd.id = ai + 1;
      objptr->set(optd, opts, this->cfg_file.get_cif_encoding(),
        thdatareader__get_opos(false,false));
    }

    // set options
    ait = ai;
    while (ait < ant) {
      optd = objptr->get_cmd_option_desc(*opts + 1);
      if (optd.id == TT_DATAOBJECT_UNKNOWN) {
        optd.id = ++ai;
        optd.nargs = 1;
      }
      else {
        if ((ait + optd.nargs) >= ant)
          ththrow(("not enough option arguments -- %s -- must be %d", *opts, optd.nargs));
        opts++;
        ait++;
      }
 
      objptr->set(optd, opts, this->cfg_file.get_cif_encoding(),
        thdatareader__get_opos(false,false));
      opts += optd.nargs;
      ait += optd.nargs;
    }       
    
    // if multi line, set that we're inside the command
    // and switch of sensitivity       
    // else insert object into database
    if ((endlnopt = objptr->get_cmd_end()) != NULL) {
      inside_cmd = true;
      this->cfg_file.cmd_sensitivity_off();
  
      while (inside_cmd) {
      
        if ((ln = this->cfg_file.read_line()) == NULL)
          ththrow(("%s [%d] -- %s is missing",this->cfg_file.get_cif_name(),
            this->cfg_file.get_cif_line_number(),endlnopt))
                      
        thencode(&this->bf1, ln, this->cfg_file.get_cif_encoding());  
        this->cfg_dblines.append(this->bf1.get_buffer());  

        thsplit_word(&this->bf1, &this->bf2, ln);
             
        // if end_command option, set turn off inside_cmd
        // and insert object into database
        if (strcmp(this->bf1.get_buffer(), endlnopt) == 0) {
          inside_cmd = false;
          this->cfg_file.cmd_sensitivity_on();
          continue;   
        }
  
        // let's parse if an option line
        optd = objptr->get_cmd_option_desc(this->bf1.get_buffer());
        if (optd.id != TT_DATAOBJECT_UNKNOWN) {
          thsplit_args(&this->mbf1, this->bf2.get_buffer());
          if (this->mbf1.get_size() < optd.nargs)
            ththrow(("not enough option arguments -- %s -- must be %d",
              this->bf1.get_buffer(), optd.nargs));
          optd.nargs = this->mbf1.get_size();
          objptr->set(optd, this->mbf1.get_buffer(), 
            this->cfg_file.get_cif_encoding(),
            thdatareader__get_opos(inside_cmd,false));
          continue;
        }
        
        // if data line (!) set data option      
        optd.id = 0;
        optd.nargs = 1;
        opt = ln;
        while(strcmp(opt,"!") < 0) opt++;
        if (*opt == '!') opt++;
        objptr->set(optd, & opt, this->cfg_file.get_cif_encoding(),
          thdatareader__get_opos(inside_cmd,false));
      }  
    }
    
    // vlozi objekt do databazy
    dbptr->insert(objptr);
  }
    
  // put everything into try block and throw exception, if error
  catch (...)
    threthrow(("%s [%d]", this->cfg_file.get_cif_name(), this->cfg_file.get_cif_line_number()))

}


void thconfig::save()
{
  if ((this->fstate == THCFG_UPDATE) || (this->fstate == THCFG_GENERATE)) {
  
#ifdef THDEBUG
    thprintf("\nwriting configuration file -- %s\n", this->fname.get_buffer());
#else
    thprintf("writing configuration file ... ");
    thtext_inline = true;
#endif 

    // OK, let's open configuration file for output
    FILE * cf;
    cf = fopen(this->fname.get_buffer(),"w");
    if (cf == NULL) {
      thwarning(("can't open configuration file for output -- %s",           this->fname.get_buffer()));
      return;
    }
  
    // first, let's write file encoding
    if (!this->skip_comments)
      fprintf(cf,"%s",THCCC_ENCODING);
    fprintf(cf,"encoding %s\n\n",thmatch_string(this->cfg_fenc, thtt_encoding));
    
    // source file
    long sid;
    thconfig_src_list::iterator srcit;
    bool some_src = false;
    for(srcit = this->src.begin(); srcit != this->src.end(); srcit++) {
      if (!some_src) {
        if (!this->skip_comments)
          fprintf(cf,"%s",THCCC_SOURCE);
      }
      fprintf(cf,"source \"%s\"\n", srcit->fname);
      some_src = true;
    }
    if (some_src)
      fprintf(cf,"\n");

    // layouts etc.
    char ** srcn = this->cfg_dblines.get_buffer();
    bool some_layout = false;
    for(sid = 0; sid < this->cfg_dblines.get_size(); sid++, srcn++) {
      thdecode(&(this->bf1), this->cfg_fenc, *srcn);
      fprintf(cf, "%s\n", this->bf1.get_buffer());
      some_layout = true;
    }
    if (some_layout)
      fprintf(cf,"\n");
    
    // selected objects
    this->selector.dump_selection(cf);    
    
    // dump readed export
    this->exporter.dump_export(cf);
    
    // dump possibilities objects
    // this->selector.dump_selection_db(cf, this->dbptr);
    
    // close config file
    fclose(cf);
    
#ifdef THDEBUG
    thprintf("\n");
#else
    thprintf("done\n");
    thtext_inline = false;
#endif 

  }
}
  

void thconfig::assign_db(class thdatabase * dp)
{
  this->dbptr = dp;
}


void thconfig::select_data()
{
  this->selector.select_db(this->dbptr);
}


void thconfig::export_data()
{
  this->exporter.export_db(this->dbptr);
}



void thconfig::xth_save()
{
  if (this->generate_xthcfg) {

    thbuffer nfn;
    nfn = this->fname.get_buffer();
    char * xbf = nfn.get_buffer();
    size_t x, xl = strlen(xbf);
    for (x = 0; x < xl; x++) {
      if (xbf[x] == '.')
        xbf[x] = '_';
    }
  
#ifdef THDEBUG
    thprintf("\nwriting xtherion file -- .xth_%s\n", xbf);
#else
    thprintf("writing xtherion file ... ");
    thtext_inline = true;
#endif 

    // OK, let's open configuration file for output
    FILE * cf;
    cf = fopen(".xtherion.dat","w");
    if (cf == NULL) {
      thwarning(("can't open xtherion file for output -- %s.xth", this->fname.get_buffer()));
      return;
    }
  
    // dump possibilities objects
    this->selector.dump_selection_db(cf, this->dbptr);
    fprintf(cf,"xth_cp_comp_stat %.0f %ld\n", thdb.db1d.nlength, thdb.nscraps);
    // close config file
    fclose(cf);
    
#ifdef THDEBUG
    thprintf("\n");
#else
    thprintf("done\n");
    thtext_inline = false;
#endif 

  }
}
  


  
double thconfig::get_outcs_convergence()
{
  double x, y, z;
  if (this->get_outcs_center(x, y, z)) {
    return thcsconverg(thcsdata_table[this->outcs].params, x, y);
  } else {
    return 0.0;
  }
}


bool thconfig::get_outcs_mag_decl(double year, double & decl)
{
  double x, y, z, lat, lon, alt;
  if (!this->get_outcs_center(x, y, z))
    return false;
  if ((year < double(thgeomag_minyear)) || (year > double(thgeomag_minyear + thgeomag_step * (thgeomag_maxmindex + 1))))
    return false;
  thcs2cs(thcsdata_table[this->outcs].params, "+proj=latlong +datum=WGS84", x, y, z, lon, lat, alt);
  decl = thgeomag(lat, lon, alt, year);
  return true;
}



bool thconfig::get_outcs_center(double & x, double & y, double & z)
{
  if ((this->outcs == TTCS_LOCAL) || (!this->outcs_def.is_valid()) || (this->outcs_sumn < 0.5)) {
    return false;
  } else {
    x = this->outcs_sumx / this->outcs_sumn;
    y = this->outcs_sumy / this->outcs_sumn;
    z = this->outcs_sumz / this->outcs_sumn;
    return true;
  }
}


void thconfig::log_outcs(double decsyear, double deceyear) {
  double x, y, z, dec;
  bool firstdec = true;
  if (this->get_outcs_center(x, y, z)) {
    size_t tab_size = ((sizeof(thtt_cs)/sizeof(thstok)) - 1);
    const char * csstr;
    long i, ii;
    i = 0;
    ii = -1;
    while (i < (long)tab_size) {
      if (thtt_cs[i].tok == this->outcs) {
        ii = i;
        if (strlen(thtt_cs[ii].s) < 6)
          break;
        if (!(((thtt_cs[ii].s[0] == 'e' || thtt_cs[ii].s[0] == 'E') && 
          (thtt_cs[ii].s[1] == 's' || thtt_cs[ii].s[1] == 'S') && 
          (thtt_cs[ii].s[2] == 'r' || thtt_cs[ii].s[2] == 'R') && 
          (thtt_cs[ii].s[3] == 'i' || thtt_cs[ii].s[3] == 'I')) || 
          ((thtt_cs[ii].s[0] == 'e' || thtt_cs[ii].s[0] == 'E') && 
          (thtt_cs[ii].s[1] == 'p' || thtt_cs[ii].s[1] == 'P') && 
          (thtt_cs[ii].s[2] == 's' || thtt_cs[ii].s[2] == 'S') && 
          (thtt_cs[ii].s[3] == 'g' || thtt_cs[ii].s[3] == 'G'))))
          break;
      }
      i++;
    }
    if (ii > -1)
      csstr = thtt_cs[ii].s;
    else
      csstr = "unknown";
    thlog.printf("output coordinate system: %s\n", csstr);
    thlog.printf("meridian convergence (deg): %.4f\n", this->get_outcs_convergence());
    if (!thisnan(decsyear)) {
      long min = long(decsyear), max = long(deceyear + 1.0), yyy;
      for(yyy = min; yyy <= max; yyy++) {
        if (firstdec) {
          thlog.printf("geomag declinations (deg):\n");
          firstdec = false;
        }
        if (this->get_outcs_mag_decl(double(yyy), dec)) {
          thlog.printf("  %4d.1.1  %.4f\n", yyy, dec);
        }
      }
    }
  }
}



thconfig thcfg;




