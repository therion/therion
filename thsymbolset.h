/** 
 * @file thsymbolset.h
 * Symbol set class.
 */
  
/* Copyright (C) 2003 Stacho Mudrak
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
 
#ifndef thsymbolset_h
#define thsymbolset_h

#include "thsymbolsetlist.h"
#include <stdio.h>

/**
 * Symbol set class.
 */
 
class thsymbolset {

  public:
  
  bool assigned[thsymbolset_size],   ///< definovane symboly
    used[thsymbolset_size];          ///< pouzite symboly
  
  thsymbolset();
  
  /**
   * Vrati macro a zaragistruje ho ako pouzite.
   */
  const char * get_mp_macro(int id);
  

  /**
   * Do MP suboru zapise mapsymbol() makro.
   */
  void export_symbol_defaults(FILE * mpf, char * symset);
  
  /**
   * Do MP suboru zapise mapsymbol() makro + zmeni hidden value.
   */
  void export_symbol_assign(FILE * mpf, int sym_id, char * symset);

  /**
   * Skryje symbol.
   */
  void export_symbol_hide(FILE * mpf, int sym_id);

  /**
   * Zobrazi symbol.
   */
  void export_symbol_show(FILE * mpf, int sym_id);

  /**
   * Do MP suboru zapise mapsymbol() makro + zmeni hidden value.
   */
  void export_symbol_assign_group(FILE * mpf, int sym_id, char * symset);

  /**
   * Skryje symbol.
   */
  void export_symbol_hide_group(FILE * mpf, int sym_id);

  /**
   * Zobrazi symbol.
   */
  void export_symbol_show_group(FILE * mpf, int sym_id);
  
  /** 
   * Exportuje znackovy kluc do pdfka.
   */
   
  void export_pdf(class thlayout * layout, FILE * mpf, unsigned & sfig);

};

/**
 * Vrati ID z popisu classu a symbolu.
 */
int thsymbolset__get_id(char * symclass, char * symbol);


/**
 * Vrati ID itemu z groupy.
 */
int thsymbolset__get_group(int group_id, int cid);


#endif


