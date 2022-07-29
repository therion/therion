/**
 * @file thmbuffer.cxx
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
 
#include "thmbuffer.h"


thmbuffer::mblock::mblock(size_t min_size, size_t last_size)
{
  this->next_ptr = NULL;
  this->size = 4 * last_size;
  while (this->size <= min_size)
    this->size *= 4;
  this->data = new char [this->size];
}


thmbuffer::mblock::~mblock()
{
  delete [] this->data;
  delete this->next_ptr;
}


thmbuffer::thmbuffer()
{
  this->max_size = 4;
  this->size = 0;
  this->buf = new char* [this->max_size];
  this->first_ptr = new mblock(1000, 256);
  this->last_ptr = this->first_ptr;
  this->free_size = this->last_ptr->size;
  this->free_ptr = this->last_ptr->data;
}


thmbuffer::~thmbuffer()
{
  delete [] this->buf;
  delete this->first_ptr;
}


char * thmbuffer::appendn(const char * src, size_t n)
{
  char * dest;
  while (this->free_size <= long(n)) {
    if (this->last_ptr->next_ptr != NULL)
      this->last_ptr = this->last_ptr->next_ptr;
    else {
      this->last_ptr->next_ptr = new mblock(n, this->last_ptr->size);
      this->last_ptr = this->last_ptr->next_ptr;
    }
    this->free_size = this->last_ptr->size;
    this->free_ptr = this->last_ptr->data;
  }
  dest = memcpy(this->free_ptr, src, n);
  dest[n] = 0;
  this->free_ptr += n + 1;
  this->free_size -= n + 1;

  if (this->size == this->max_size) {
    char ** new_buf;
    this->max_size *= 4;
    new_buf = new char* [this->max_size];
    memcpy(new_buf, this->buf, this->size * sizeof(char*));
    delete [] this->buf;
    this->buf = new_buf;
  }
  this->buf[this->size] = dest;
  this->size++;
  return dest;
}


char * thmbuffer::append(const char * src)
{
  return this->appendn(src, strlen(src));
}


void thmbuffer::clear()
{
  this->last_ptr = this->first_ptr;
  this->free_size = this->last_ptr->size;
  this->free_ptr = this->last_ptr->data;
  this->size = 0;
}


long thmbuffer::get_size()
{
  return this->size;
}


char ** thmbuffer::get_buffer()
{
  return this->buf;
}




