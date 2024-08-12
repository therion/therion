/**
 * @file thlang.h
 * Therion language translation module.
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
 
#ifndef thlang_h
#define thlang_h

#include "thlangdata.h"
#include "thstok.h"

const char * thT(const char * txt, int lng = THLANG_UNKNOWN);

int thlang_parse(const char * str);

const char * thlang_getid(int id);

const char * thlang_getcxxid(int id);

int thlang_getlang(int lng);

const thstok * thlang_get_text_table();

void thlang_set_translation(char * lang, char * text, char * translation);

#endif


