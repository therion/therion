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
#include "thlayoutclr.h"
#include <stdio.h>


/**
 * Symbol set class.
 */
 
struct thsymbolset {
  
  bool assigned[thsymbolset_size],        ///< definovane symboly
    used[thsymbolset_size];               ///< pouzite symboly
  thlayout_color color[thsymbolset_size];    ///< Symbol colors.
    
  bool group_symbols; ///< Ci zgrupuje podobne symboly alebo nie.
  
  thsymbolset();
  
  /**
   * Vrati macro a zaragistruje ho ako pouzite.
   */
  const char * get_mp_macro(int id);
  
  /**
   * Return whether symbol is assigned.
   */
  bool is_assigned(int symbol);

  /**
   * Do MP suboru zapise mapsymbol() makro.
   */
  void export_symbol_defaults(FILE * mpf, const char * symset);
  
  /**
   * Do MP suboru zapise mapsymbol() makro + zmeni hidden value.
   */
  void export_symbol_assign(FILE * mpf, int sym_id, const char * symset);

  /**
   * Skryje symbol.
   */
  void export_symbol_hide(FILE * mpf, int sym_id);

  /**
   * Zobrazi symbol.
   */
  void export_symbol_show(FILE * mpf, int sym_id);

  /**
   * Set symbol color.
   */
  void export_symbol_color(FILE * mpf, int sym_id, thlayout_color * clr);

  /**
   * Do MP suboru zapise mapsymbol() makro + zmeni hidden value.
   */
  void export_symbol_assign_group(FILE * mpf, int sym_id, const char * symset);

  /**
   * Skryje symbol.
   */
  void export_symbol_hide_group(FILE * mpf, int sym_id);

  /**
   * Zobrazi symbol.
   */
  void export_symbol_show_group(FILE * mpf, int sym_id);

  /**
   * Set symbol color.
   */
  void export_symbol_color_group(FILE * mpf, int sym_id, thlayout_color * clr);
  
  /** 
   * Exportuje znackovy kluc do pdfka.
   */
   
  void export_pdf(class thlayout * layout, FILE * mpf, unsigned & sfig);


  /**
   * Export symbol options to metapost.
   */

  void export_mp_symbol_options(FILE * mpf, int sym_id);
  void export_mp_symbol_options(class thexception * x, int sym_id);


};

/**
 * Vrati ID z popisu classu a symbolu.
 */
int thsymbolset__get_id(const char * symclass, const char * symbol);


/**
 * Vrati ID itemu z groupy.
 */
int thsymbolset__get_group(int group_id, int cid);


/**
 * Export all symbols.
 */
void export_all_symbols();


#endif


