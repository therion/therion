/**
 * @file thbuffer.cxx
 * String buffer class.
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
 
#include "thbuffer.h"

thbuffer::thbuffer()
{
  this->size = 16;
  this->buff = new char [this->size];
  this->buff[0] = 0;
}


thbuffer::~thbuffer()
{
  if (this->buff)
    delete [] this->buff;
}


void thbuffer::enlarge(size_t min_size)
{
  char * new_buff;
  size_t new_size = 4 * this->size;
  while(new_size <= min_size)
    new_size *= 4;
  new_buff = new char [new_size];
  memcpy(new_buff, this->buff, this->size);
  delete [] this->buff;
  this->buff = new_buff;
  this->size = new_size;
}


char * thbuffer::strncpy(const char * src, size_t n)
{
  if (n >= this->size)
    this->enlarge(n);
  ::strncpy(this->buff, src, n);
  this->buff[n] = 0;
  return this->buff;
}


char * thbuffer::strcpy(const char * src)
{
  size_t srclen = strlen(src);
  if (srclen >= this->size)
    this->enlarge(srclen);
  return ::strcpy(this->buff, src);
}


char * thbuffer::strcat(const char * src)
{
  size_t newlen = strlen(src) + strlen(this->buff);
  if (newlen >= this->size)
    this->enlarge(newlen);
  return ::strcat(this->buff, src);
}


char * thbuffer::strncat(const char * src, size_t n)
{
  size_t newlen = n + 1 + strlen(this->buff);
  if (newlen >= this->size)
    this->enlarge(newlen);
  return ::strncat(this->buff, src, n);
}


thbuffer::operator char* ()
{
  return this->buff;
}


char * thbuffer::get_buffer()
{
  return this->buff;
}


thbuffer & thbuffer::operator=(const char * src)
{
  this->strcpy(src);
  return *this;
}


thbuffer & thbuffer::operator=(thbuffer const & srcbf)
{
  this->strcpy(srcbf.buff);
  return *this;
}


thbuffer & thbuffer::operator+=(const char * src)
{
  this->strcat(src);
  return *this;
}

void thbuffer::guarantee(size_t bs)
{
  if (this->size <= bs)
    this->enlarge(bs);
}


