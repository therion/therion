/**
 * @file thdb2dxs.cxx
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#include "thdb2dxs.h"
#include "thdb2dmi.h"

thdb2dxs::thdb2dxs()
{
  this->prev_item = NULL;
  this->next_item = NULL;

  this->bm = NULL;
  this->fmap = NULL;
  this->mode = TT_MAPITEM_UNKNOWN;
  this->m_target = this;
  
  this->previewed = false;
  this->preview_output_number = 0;

}

bool operator < (const thdb2dxs_target_item & i1, const thdb2dxs_target_item & i2)
{
  if (i1.m_proj_id < i2.m_proj_id)
    return true;
  if (i1.m_proj_id > i2.m_proj_id)
    return false;

  if (i1.m_map_id < i2.m_map_id)
    return true;
  if (i1.m_map_id > i2.m_map_id)
    return false;

  if (i1.m_shift < i2.m_shift)
    return true;
  return false;
}
  
