/**
 * @file thdatareader.cxx
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
 
#include "thdatareader.h"
#include "thexception.h"
#include "thobjectsrc.h"

unsigned long thdatareader_get_opos(bool inlineid, bool cfgid)
{
  unsigned long opos = 0;
  
  if (inlineid)
    opos |= THOP_INLINE;
  else
    opos &= ~THOP_INLINE;
  
  if (cfgid)
    opos |= THOP_CONFIGURE;
  else
    opos &= ~THOP_CONFIGURE;
  
  return opos;
}


void thdatareader::read(const char * ifname, long lnstart, long lnend, const char * spath, thdatabase * dbptr)
{
  std::unique_ptr<thdataobject> unique_objptr;
  thdataobject * objptr = NULL;  // pointer to the newly created object
  thcmd_option_desc optd;  // option descriptor
  bool inside_cmd = false;
  bool configure_cmd = false;
  bool advanced_end_search = false;
  char * ln, * opt, ** opts;
  const char * endlnopt = NULL;
  int ai, ait, ant;

  bool special_lines_only;
  unsigned long lnn;
  special_lines_only = ((lnend >= lnstart) && (lnstart > 0));
  
  this->inp.report_missing = true;
  this->inp.set_file_name(ifname);
  this->inp.cmd_sensitivity_on();
  this->inp.set_search_path(spath);
  this->inp.set_file_suffix(".th:.th2");
  this->inp.sp_scan_on();
  this->inp.reset();
  if ((special_lines_only) && (lnstart > 1))
    this->inp.set_input_sensitivity(false);
  
  thobjectsrc osrc;

#ifndef THMSVC
  try {
#endif
    while ((ln = this->inp.read_line()) != NULL) {

      lnn = this->inp.get_cif_line_number();
      if (this->inp.is_first_file() && (special_lines_only) && (((long)lnn < lnstart) || ((long)lnn > lnend))) {
        if ((long)(lnn + 1) == lnstart)
          this->inp.set_input_sensitivity(true);
        if ((long)lnn > lnend)
          this->inp.set_input_sensitivity(false);
        continue;
      }
    
      // let's test source fname
      osrc.line = this->inp.get_cif_line_number();
      if (strcmp(osrc.name, this->inp.get_cif_name()) != 0)
        osrc.name = dbptr->strstore(this->inp.get_cif_name(), true);
      dbptr->csrc.line = osrc.line;
      dbptr->csrc.name = osrc.name;
    
      // now different behavoiour whether we're inside or outside command
      if (inside_cmd) {
        thsplit_word(&this->bf1, &this->bf2, ln);
             
        // if end_command option, set turn off inside_cmd
        // and insert object into database
        if ((advanced_end_search && objptr->get_cmd_ends_match(this->bf1.get_buffer())) || 
            (strcmp(this->bf1.get_buffer(), endlnopt) == 0)) {
          inside_cmd = false;
          //this->inp.cmd_sensitivity_on();
          if (!configure_cmd)
            dbptr->insert(std::move(unique_objptr));
          else {
            objptr->start_insert();
            configure_cmd = false;
          }
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
            this->inp.get_cif_encoding(),
            thdatareader_get_opos(inside_cmd,configure_cmd));
          continue;
        }
        
        // if data line (!) set data option      
        optd.id = 0;
        optd.nargs = 1;
        opt = ln;
        while(strcmp(opt,"!") < 0) opt++;
        if (*opt == '!') opt++;
        objptr->set(optd, & opt, this->inp.get_cif_encoding(),
          thdatareader_get_opos(inside_cmd,configure_cmd));

      }
      else {
        
        
        // first, let's parse arguments
        thsplit_args(&this->mbf1, this->inp.get_value());
        ant = this->mbf1.get_size();
        opts = this->mbf1.get_buffer();
        
        // check if command
        // if no => throw exception        
        if (strcmp(this->inp.get_cmd(),"revise") == 0)
          configure_cmd = true;
        else if (strcmp(this->inp.get_cmd(),"equate") == 0) {
          dbptr->insert_equate(ant, opts);
          continue;
        } 
        else {
          unique_objptr = dbptr->create(this->inp.get_cmd(), osrc);
          objptr = unique_objptr.get();
          if (objptr == NULL)
            ththrow(("unknown command -- %s", this->inp.get_cmd()))
          else
            dbptr->check_context(objptr);
            
	        switch(objptr->get_class_id()) {
            case TT_LAYOUT_CMD:
              ththrow(("layout definition not allowed in input files"))
              break;
          }
        }

        // analyze the commands options

        if (configure_cmd) {
          // let's find an object
          objptr = dbptr->revise(*opts, dbptr->get_current_survey(), osrc);
          if (objptr == NULL)
            ththrow(("object does not exist -- %s", *opts))
          ai = 1;
          opts++;
        }
        else {
          if (ant < objptr->get_cmd_nargs())
            ththrow(("not enough command arguments -- must be %d",
              objptr->get_cmd_nargs()));
          optd.nargs = 1;

          // set obligatory arguments
          for (ai = 0; ai < objptr->get_cmd_nargs(); ai++, opts++) {
            optd.id = ai + 1;
            objptr->set(optd, opts, this->inp.get_cif_encoding(),
              thdatareader_get_opos(inside_cmd,configure_cmd));

          }
        }

        ait = ai;
        while (ait < ant) {
          optd = objptr->get_cmd_option_desc(*opts + 1);
          if (configure_cmd && (optd.id == TT_DATAOBJECT_NAME))
            ththrow(("object name configuration not allowed"))
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
 
          objptr->set(optd, opts, this->inp.get_cif_encoding(),
            thdatareader_get_opos(inside_cmd,configure_cmd));
          opts += optd.nargs;
          ait += optd.nargs;
        }
           
        // if multi line, set that we're inside the command
        // and switch of sensitivity       
        // else insert object into database
        if ((endlnopt = objptr->get_cmd_end()) == NULL)
          if (!configure_cmd)
            dbptr->insert(std::move(unique_objptr));
          else {
            objptr->start_insert();
            configure_cmd = false;
          }
        else {
          if (configure_cmd)
            endlnopt = "endrevise";
          advanced_end_search = objptr->get_cmd_ends_state();
          inside_cmd = true;
          //this->inp.cmd_sensitivity_off();
        }
      }    
    }

#ifndef THMSVC
  }
// put everything into try block and throw exception, if error
  catch (...)
    threthrow(("%s [%d]", this->inp.get_cif_name(), this->inp.get_cif_line_number()))
#endif

  dbptr->end_insert();  

}


thdatareader thdbreader;

