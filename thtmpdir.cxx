/**
 * @file thtmpdir.cxx
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
 
#include "thtmpdir.h"
#include "therion.h"
#include "thinit.h"

#include <fmt/core.h>

#include <cstdlib>
#include <filesystem>

#ifdef THWIN32
#include <process.h>
#define getpid _getpid
#endif

namespace fs = std::filesystem;

thtmpdir::thtmpdir()
{
  this->exist = false;
  this->tried = false;
#ifdef THDEBUG
  this->delete_all = false;
  this->debug = true;
#else
  this->delete_all = true;
  this->debug = false;
#endif
}
  
thtmpdir::~thtmpdir()
{
  this->remove();
}


void thtmpdir::create() try
{
  if (this->exist || this->tried)
    return;

  fs::path dir_path;
  this->tmp_remove_script = thini.tmp_remove_script.get_buffer();

#ifdef THDEBUG
  // the debugging temp directory
  dir_path = "thTMPDIR";
#else
  if (strlen(thini.tmp_path.get_buffer()) > 0) {
    dir_path = thini.tmp_path.get_buffer();
  } else if (this->debug) {
    dir_path = "thTMPDIR";
  } else {
    dir_path = fs::temp_directory_path() / fmt::format("th{}", getpid());
  }
#endif

  this->tried = true;
  if (!fs::create_directory(dir_path) && !this->debug) {
    thwarning(("temporary directory already exists"));
  }

  this->exist = true;
  this->name = dir_path.string();
}
catch (const std::exception& e)
{
  therror(("can't create temporary directory -- %s", e.what()));
}


void thtmpdir::remove() try
{
  if (!this->exist || !this->delete_all)
    return;

  if (!this->tmp_remove_script.empty()) {
    const auto tmpfname = fmt::format("{} {}", this->tmp_remove_script, this->name);
    if (system(tmpfname.c_str()) != 0) {
      thwarning(("delete temporary directory error -- %s not successful", tmpfname.c_str()))
    }
    if (fs::is_directory(this->name)) {
      thwarning(("error deleting temporary directory -- %s", this->name.c_str()))    
    } else {
      this->name = ".";
      this->tried = false;
      this->exist = false;
    }
    return;
  }

  fs::remove_all(this->name);
  this->name = ".";
  this->tried = false;
  this->exist = false;
} catch (const std::exception& e) {
  thwarning(("error deleting temporary directory -- %s", e.what()))
}


const char* thtmpdir::get_dir_name()
{
  if (!this->exist) this->create();
  return this->name.c_str();
}
  

const char* thtmpdir::get_file_name(const char *fname)
{
  if (!this->exist) this->create();
  this->file_name = (fs::path(this->name) / fname).string();
  return this->file_name.c_str();
}


void thtmpdir::set_delete(bool delete_id)
{
  this->delete_all = delete_id;
}


bool thtmpdir::get_delete()
{
  return(this->delete_all);
}


void thtmpdir::delete_on()
{
  this->delete_all = true;
}


void thtmpdir::delete_off()
{
  this->delete_all = false;
}

thtmpdir thtmp;


