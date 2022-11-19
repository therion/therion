/**
 * @file thinput.cxx
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
 
#include "thinput.h"
#include "thparse.h"
#include "thchenc.h"
#include "therion.h"
#include "thexception.h"

#include <filesystem>

namespace fs = std::filesystem;

#define THMAXFREC 64

const long thinput::max_line_size = 65535;


enum {TT_UNKNOWN_INPUT, TT_INPUT, TT_ENCODING};


static const thstok thtt_input[] = {
  {"encoding", TT_ENCODING},
  {"input", TT_INPUT},
  {NULL, TT_UNKNOWN_INPUT}
};


thinput::ifile::ifile(ifile * fp)
{
  this->lnumber = 0;
  this->encoding = TT_UTF_8;
  this->prev_ptr = fp;
}

void thinput::ifile::close()
{
  if (this->sh.is_open()) {
    this->sh.close();
    this->sh.clear();
    this->lnumber = 0;
  }
}

bool thinput::ifile::is_equal(ifile* f)
{
  try {
    return fs::equivalent(name.get_buffer(), f->name.get_buffer());
  } catch(const std::exception& e) {
    thwarning(("unable to compare files -- %s", e.what()))
    return false;
  }
}


thinput::thinput()
{
  this->cmd_sensitivity = true;
  this->input_sensitivity = true;
  this->scan_search_path = false;
  this->first_ptr = std::unique_ptr<ifile>(new ifile(nullptr));
  this->last_ptr = this->first_ptr.get();
  this->lnbuffer = std::unique_ptr<char[]>(new char [thinput::max_line_size]);
  this->pifo = false;
  this->pifoid = NULL;
  this->pifoproc = NULL;
  this->report_missing = false;
}

void thinput::set_cmd_sensitivity(bool cs)
{
  this->cmd_sensitivity = cs;
  if (!cs) {
    this->cmdbf = "";
    this->valuebf = "";
  }
}
  

void thinput::cmd_sensitivity_on()
{
  this->cmd_sensitivity = true;
}
   

void thinput::cmd_sensitivity_off()
{
  this->cmd_sensitivity = false;
  this->cmdbf = "";
  this->valuebf = "";
}
   

bool thinput::get_cmd_sensitivity()
{
  return this->cmd_sensitivity;
}

   
void thinput::set_sp_scan(bool spss)
{
  this->scan_search_path = spss;
}

   
void thinput::sp_scan_on()
{
  this->scan_search_path = true;
}
   

void thinput::sp_scan_off()
{
  this->scan_search_path = false;
}

  
bool thinput::get_sp_scan()
{
  return this->scan_search_path;
}


void thinput::set_file_name(const char * fname)
{
  this->file_name = fname;
}


char * thinput::get_file_name()
{
  return this->file_name;
}


void thinput::set_search_path(const char * spath)
{
  this->search_path.clear();
#ifdef THWIN32
  thsplit_paths(& this->search_path, spath, ';'); 
#else
  thsplit_paths(& this->search_path, spath, ':'); 
#endif
}

void thinput::set_file_suffix(const char * fsx)
{
  this->file_suffix.clear();
  thsplit_strings(& this->file_suffix, fsx, ':'); 
}


void thinput::open_file(char * fname)
{
  char * srcfile = NULL;
  if (this->last_ptr->sh.is_open())
    srcfile = this->last_ptr->name;
  
  // as first, let's find appropriate ifile
  ifile * ifptr;
  ifptr = this->last_ptr;
  while (ifptr->sh.is_open() && (ifptr->next_ptr != NULL))
    ifptr = ifptr->next_ptr.get();
  if (ifptr->sh.is_open() && (ifptr->next_ptr == NULL)) {
    ifptr->next_ptr = std::unique_ptr<ifile>(new ifile(ifptr));
    ifptr = ifptr->next_ptr.get();
  }
  
  // now, let's open the file
  // first let's set the path to source file path
  if (srcfile != NULL)
    thsplit_fpath(&ifptr->path, srcfile);
  else
    ifptr->path = "";
  
  
  int ix = -1;
  long paths = this->search_path.get_size(), sfxid,
    sfxs = this->file_suffix.get_size();
  char ** sfx = this->file_suffix.get_buffer();
  char ** pths = this->search_path.get_buffer();
  size_t ifptr_psz;
  char ifptr_plc;

  while ((!ifptr->sh.is_open()) && (ix < paths)) {

    // let's try the name without suffixes
    ifptr_psz = strlen(ifptr->path.get_buffer());
    ifptr_plc = (ifptr_psz > 0 ? ifptr->path.get_buffer()[ifptr_psz-1] : 0);
    if (ifptr_psz > 0) {
      ifptr->name = ifptr->path;
      if (ifptr_plc != '/')
        ifptr->name += "/";
      ifptr->name += fname;
    }
    else
      ifptr->name = fname;
    ifptr->sh.clear();
    ifptr->sh.open(ifptr->name); //, ios::in | ios::nocreate
    
    // if not successful, try with suffixes
    sfxid = 0;
    while ((!ifptr->sh.is_open()) && (sfxid < sfxs)) {
      if (ifptr_psz > 0) {
        ifptr->name = ifptr->path;
        if (ifptr_plc != '/')
          ifptr->name += "/";
        ifptr->name += fname;
      }
      else
        ifptr->name = fname;
      ifptr->name += sfx[sfxid];
      ifptr->sh.clear();
      ifptr->sh.open(ifptr->name); //, ios::in | ios::nocreate
      sfxid++;
    }
    
    // if still no success, try with search path
    ix++;
    if (ix < paths)
      ifptr->path = pths[ix];
          
  }
  
  
  // if file was open, now let's try if not recursive inclusion
  ifile * tmptr;
  bool is_rec = false;
  unsigned int n_rec = 0;
  
  if (!ifptr->sh.is_open()) {
    if ((srcfile != NULL) || (this->report_missing)) {
      ththrow("can't open file for input -- {}", fname);
    }
  }
  else {
    tmptr = ifptr->prev_ptr;
#ifdef THWIN32
    while ((tmptr != NULL) && (n_rec < THMAXFREC)) {
#else
    while ((tmptr != NULL) && !is_rec) {
#endif
      is_rec = is_rec || tmptr->is_equal(ifptr);
      tmptr = tmptr->prev_ptr;
      n_rec++;
    }
#ifdef THWIN32
    if (n_rec == THMAXFREC) {
      therror(("%s [%d] -- too many file inclusions -- probably input recursion -- %s", \
      this->get_cif_name(), this->get_cif_line_number(), fname));
    }
#else
    if (is_rec) {
      therror(("%s [%d] -- recursive file inclusion -- %s", \
      this->get_cif_name(), this->get_cif_line_number(), fname));
    }
#endif
    else {
      this->last_ptr = ifptr;
      if (this->pifo) {
        if (this->pifoproc != NULL)
          this->pifoproc(ifptr->name.get_buffer());
        if (this->pifoid != NULL)
          *(this->pifoid) = true;
        this->pifo = false;
        this->pifoid = NULL;
        this->pifoproc = NULL;
      }
#ifdef THDEBUG
      thprintf("open file -- %s\n", this->last_ptr->name.get_buffer());
#endif
    }
  }
  
}


void thinput::close_file()
{
  if (this->last_ptr->sh.is_open()) {
#ifdef THDEBUG
    thprintf("close file -- %s\n", this->last_ptr->name.get_buffer());
#endif
    this->last_ptr->close();
    if (this->last_ptr->prev_ptr != NULL) {
      this->last_ptr = this->last_ptr->prev_ptr; 
      // Is next cmd it really like this ?????
      this->last_ptr->next_ptr = NULL;
    }
  }
}

void thinput::reset()
{
  // first, let's close all open files
  ifile * iptr = this->first_ptr.get();
  while(iptr != NULL) {
    iptr->close();
    iptr = iptr->next_ptr.get();
  }
  
  // set pointer to the first ifile
  this->last_ptr = this->first_ptr.get();
  this->open_file(this->file_name);  
}


char * thinput::read_line()
{
  int ln_state = 0;
  bool mline = false;
  long lnlen; //, idx;
  char * idxptr;
  
  while (ln_state == 0) {

    // first, check if any input available
    if (!this->last_ptr->sh.is_open()) {
      ln_state = -1;
      break;
    }
      
    // check eof state of last open file
    if (this->last_ptr->sh.eof()) {
      this->close_file();
      continue;
    }

    // OK, we can read the line
    this->last_ptr->sh.getline(this->lnbuffer.get(), thinput::max_line_size);
    this->last_ptr->lnumber++;

    // Check, if reading was OK.
    
    if (this->last_ptr->sh.fail() && (!(this->last_ptr->sh.eof()))) {
      therror(("%s [%d] -- line too long", \
        this->get_cif_name(), this->get_cif_line_number()));
    }
    
    // now let's find last non white character
    lnlen = (long)strlen(this->lnbuffer.get());
    idxptr = this->lnbuffer.get() + lnlen - 1;
    while ((lnlen > 0) && (*idxptr < 33)) {
      idxptr--;
      lnlen--;
    }
    
    // empty line read
    if ((lnlen == 0) && !mline)
      continue;
    
    // join backslash ended lines together
    if (*idxptr == '\\') {
      if (mline) {
        this->linebf.strncat(this->lnbuffer.get(), lnlen - 1);
      }
      else {
        this->linebf.strncpy(this->lnbuffer.get(), lnlen - 1);
        mline = true;
      }
      continue;
    }
    else {
      if (mline)
        this->linebf.strncat(this->lnbuffer.get(), lnlen);
      else
        this->linebf.strncpy(this->lnbuffer.get(), lnlen);
    }
    
    mline = false;
    
    // we've something regular in the linebf
    // split into cmd & value & interpret commands 
    if (this->cmd_sensitivity) {
      thsplit_word(&this->cmdbf, &this->valuebf, this->linebf.get_buffer());
      thsplit_args(&this->tmpmb, this->valuebf.get_buffer());
      
      // check if comment
      if (*(this->cmdbf.get_buffer()) == '#')
        continue;
        
      // interpret commands
      switch (thmatch_token(this->cmdbf.get_buffer(), thtt_input)) {
      
        case TT_INPUT:
          if (this->input_sensitivity) {
            if (this->tmpmb.get_size() != 1)
              therror(("%s [%d] -- one input file name expected -- %s", \
                this->get_cif_name(), this->get_cif_line_number(), \
                this->valuebf.get_buffer()))
            else
              this->open_file(*(this->tmpmb.get_buffer()));
          }
          continue;
          
        case TT_ENCODING:
          if (this->tmpmb.get_size() != 1)
            therror(("%s [%d] -- encoding name expected -- %s", \
              this->get_cif_name(), this->get_cif_line_number(), \
              this->valuebf.get_buffer()));
          this->last_ptr->encoding = \
            thmatch_token(*(this->tmpmb.get_buffer()), thtt_encoding);
          if (this->last_ptr->encoding == TT_UNKNOWN_ENCODING) {
            therror(("%s [%d] -- unknown encoding -- %s", \
              this->get_cif_name(), this->get_cif_line_number(), \
              this->valuebf.get_buffer()));
            this->last_ptr->encoding = TT_UTF_8;
          }
          continue;
          
      } // interpret commands
          
    }
    // otherwise check if comment
    else {
      idxptr = this->linebf.get_buffer();
      lnlen = (long)strlen(idxptr);
      while ((lnlen > 0) && (*idxptr < 33)) {
        lnlen--;
        idxptr++;
      }
      if (*idxptr == '#')
        continue;
    }
 
    // no comment, no command
    ln_state = 1;
 
  }
  
  if (ln_state == 1)
    return this->linebf.get_buffer();
  else
    return NULL;
}


char * thinput::get_cmd()
{
  return this->cmdbf.get_buffer();
}

char * thinput::get_line()
{
  return this->linebf.get_buffer();
}


char * thinput::get_value()
{
  return this->valuebf.get_buffer();
}

char * thinput::get_cif_name()
{
  return this->last_ptr->name.get_buffer();
}



char * thinput::get_cif_path()
{
  static thbuffer cifpath;
  thsplit_fpath(&cifpath, this->last_ptr->name.get_buffer());
  return cifpath.get_buffer();
}

std::string thinput::get_cif_abspath(const char * fname)
{
  std::error_code ec;
  auto pict_path = std::filesystem::current_path(ec);
  thassert(!ec);

  const auto& last_name = this->last_ptr->name.get_buffer();
  if (fs::path(last_name).is_absolute())
    pict_path = last_name;
  else 
    pict_path /= last_name;
  
  pict_path = pict_path.parent_path();

  if (fname != NULL && strlen(fname) > 0) {
    if (fs::path(fname).is_absolute())
      pict_path = fname;
    else
      pict_path /= fname;
  }

  return pict_path.string();
}

unsigned long thinput::get_cif_line_number()
{
  return this->last_ptr->lnumber;
}


int thinput::get_cif_encoding()
{
  return this->last_ptr->encoding;
}

void thinput::print_if_opened(void (* pifop)(char *), bool * printed) {
  this->pifo = true;
  if (printed != NULL)
    this->pifoid = printed;
  this->pifoproc = pifop;
}


void thinput::set_input_sensitivity(bool s)
{
  this->input_sensitivity = s;
}


bool thinput::get_input_sensitivity()
{
  return this->input_sensitivity;
}

bool thinput::is_first_file()
{
  return ((this->first_ptr == NULL) || (this->first_ptr->next_ptr == NULL));
}

