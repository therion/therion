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
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#ifdef THWIN32
#include <process.h>
#define snprintf _snprintf
#define getpid _getpid
#define THPATHSEPARATOR "\\"
#else
#define THPATHSEPARATOR "/"
#endif

thtmpdir::thtmpdir()
{
  this->exist = false;
  this->tried = false;
  this->name.strcpy(".");
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


void thtmpdir::create()
{
  if ((!this->exist) && (!this->tried)) {
    thbuffer dir_path;
#ifdef THDEBUG
    // the debugging temp directory
    dir_path = "thTMPDIR";
#else
    char dn[16];
    char *envtmp;
    // release temp directory
    envtmp = getenv("TEMP");
    if (envtmp != NULL) {
      dir_path = envtmp;
    } else {
      envtmp = getenv("TMP");
      if (envtmp != NULL) {
        dir_path = envtmp;
      } else {
#ifdef THWIN32
        dir_path = ".";
#else
        dir_path = "/tmp";
#endif
      }
    }
    dir_path += THPATHSEPARATOR;
    if (this->debug) {
      dir_path += "thTMPDIR";
    } else {
      dir_path += "th";
      snprintf(&(dn[0]),16,"%d",getpid());
      dir_path += &(dn[0]);
    }
#endif
    this->tried = true;
#ifdef THWIN32
    if (mkdir(dir_path) != 0) {
#else
    if (mkdir(dir_path,0046750) != 0) {
#endif
      struct stat buf;
      stat(dir_path,&buf);
      if ((errno == EEXIST) && (S_ISDIR(buf.st_mode))) {
          if (!this->debug) {
            thwarning(("temporary directory already exists"));
          }
          this->exist = true;
          this->name = dir_path;
      }
      else {
        therror(("can't create temporary directory -- %s", dir_path.get_buffer()));
      }
    }
    else {
      this->exist = true;
      this->name = dir_path;
    }
  }
}


void thtmpdir::remove()
{
  if (this->exist && this->delete_all) {
    // remove directory contents
#ifdef THMACOSX
    thbuffer tmpfname;
    tmpfname = "rm -f -R ";
    tmpfname += this->name;
    system(tmpfname.get_buffer());
    DIR *tmpdir = opendir(this->name);
    if (tmpdir != NULL) {
      thwarning(("error deleting temporary directory -- %s",this->name.get_buffer()))      
      closedir(tmpdir);
    } else {
      this->name = ".";
      this->tried = false;
      this->exist = false;
    }
#else
    DIR *tmpdir = opendir(this->name);
    struct dirent *tmpf;
    thbuffer tmpfname;
    if (tmpdir != NULL) {
      tmpf = readdir(tmpdir);
      while (tmpf != NULL) {
        tmpfname = this->name;
        tmpfname += THPATHSEPARATOR;
        tmpfname += tmpf->d_name;
        unlink(tmpfname);
        tmpf = readdir(tmpdir);
      }
      closedir(tmpdir);
    }

    // remove directory
    if (rmdir(this->name) != 0)
      thwarning(("error deleting temporary directory -- %s",this->name.get_buffer()))
    else {
      this->name = ".";
      this->tried = false;
      this->exist = false;
    }
#endif    
  }
}


const char* thtmpdir::get_dir_name()
{
  if (!this->exist) this->create();
  return(this->name);
}
  

const char* thtmpdir::get_file_name(const char *fname)
{
  if (!this->exist) this->create();
  this->file_name = this->name;
  this->file_name += THPATHSEPARATOR;
  this->file_name += fname;
  return(this->file_name);
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


