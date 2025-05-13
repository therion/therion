/**
 * @file thdb2dmi.cxx
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
 
#include "thdb2dmi.h"

thdb2dmi::thdb2dmi()
{
  this->prev_item = NULL;
  this->next_item = NULL;

  this->name.clear();
  this->object = NULL;
  this->itm_level = 1;
  
  this->psurvey = NULL;
  this->type = TT_MAPITEM_UNKNOWN;

}


thdb2dmi_shift thdb2dmi_shift::add(const thdb2dmi_shift & s)
{
  thdb2dmi_shift x;
  if (s.is_active()) {
    x.m_preview = s.m_preview;
    x.m_prev_x = this->m_x;
    x.m_prev_y = this->m_y;
  } else {
    x.m_preview = this->m_preview;
  }
  x.m_x = this->m_x + s.m_x;
  x.m_y = this->m_y + s.m_y;
  return x;
}


bool operator == (const thdb2dmi_shift & s1, const thdb2dmi_shift & s2)
{
  if ((!s1.is_active()) && (!s2.is_active()))
    return true;
  if (s1.is_active() && s2.is_active() && (s1.m_x == s2.m_x) && (s1.m_y == s2.m_y))
    return true;
  return false;
}


bool operator < (const thdb2dmi_shift & s1, const thdb2dmi_shift & s2)
{
  if (s1.is_active() && (!s2.is_active()))
    return false;
  if (s2.is_active() && (!s1.is_active()))
    return true;
  if ((!s1.is_active()) && (!s2.is_active()))
    return false;
  if (s1.m_x < s2.m_x)
    return true;
  if (s1.m_x > s2.m_x)
    return false;
  if (s1.m_y < s2.m_y)
    return true;
  return false;
}


bool thdb2dmi_shift::is_active() const {
  return (this->m_preview != TT_MAPITEM_UNKNOWN);
} 


