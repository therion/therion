/**
 * @file thperson.cxx
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
 
#include "thperson.h"
#include "thdatabase.h"
#include "thparse.h"
#include "thexception.h"
#include <string.h>


void thperson::reset()
{
  this->n1 = "";
  this->n2 = "";
  this->nn = "/";
}
  

void thperson::identify(thdatabase * dbp)
{
  dbp->buff_tmp = this->n2;
  dbp->buff_tmp += "/";
  dbp->buff_tmp += this->n1;
  this->nn = dbp->strstore(dbp->buff_tmp.get_buffer(), true);
}


thperson::thperson()
{
  this->reset();
}


#ifdef THWIN32
char * index(char * str, char znk)
{
	size_t sln = strlen(str), idx;
  for(idx = 0; idx < sln; idx++, str++) 
  	if(*str == znk) return str;
  return NULL;
}
#endif


void thperson::parse(thdatabase * dbp, char * src)
{
  this->reset();
  bool has_sep = (index(src,'/') != NULL);
  thsplit_strings(&(dbp->mbuff_tmp), src, '/');
  if (has_sep) {
    if (dbp->mbuff_tmp.get_size() == 1) {
      this->n2 = dbp->strstore((dbp->mbuff_tmp.get_buffer())[0], true);
    }
    else if (dbp->mbuff_tmp.get_size() == 2) {
      this->n1 = dbp->strstore((dbp->mbuff_tmp.get_buffer())[0], true);
      this->n2 = dbp->strstore((dbp->mbuff_tmp.get_buffer())[1], true);
    }
    else
      ththrow(("invalid name format -- \"%s\"", src))
  }
  else {
    thsplit_args(&(dbp->mbuff_tmp), src);
    if (dbp->mbuff_tmp.get_size() == 1) {
      this->n2 = dbp->strstore((dbp->mbuff_tmp.get_buffer())[0], true);
    }
    else if (dbp->mbuff_tmp.get_size() == 2) {
      this->n1 = dbp->strstore((dbp->mbuff_tmp.get_buffer())[0], true);
      this->n2 = dbp->strstore((dbp->mbuff_tmp.get_buffer())[1], true);
    }
    else
      ththrow(("invalid name format -- \"%s\"", src))
  }
  
  this->identify(dbp);
}


bool operator < (const class thperson & p1, const class thperson & p2)
{
  if (strcmp(p1.nn, p2.nn) < 0)
    return true;
  else
    return false;
}


bool operator == (const class thperson & p1, const class thperson & p2)
{
  if (strcmp(p1.nn, p2.nn) == 0)
    return true;
  else
    return false;
}


char * thperson::get_n1() const
{
  return this->n1;
}

char * thperson::get_n2() const
{
  return this->n2;
}



