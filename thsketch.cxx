/**
* @file thsketch.cxx
*/

/* Copyright (C) 2000	Stacho Mudrak
* 
* $Date:	$
* $RCSfile: $
* $Revision:	$
*
* --------------------------------------------------------------------	
* This	program	is free	software;	you	can	redistribute it	and/or modify
* it	under	the	terms	of the GNU General Public	License	as published by
* the Free	Software Foundation; either	version	2	of the License,	or
* any later version.
*
* This	program	is distributed in	the	hope that	it will	be useful,
* but WITHOUT ANY WARRANTY; without even	the	implied	warranty of
* MERCHANTABILITY or	FITNESS	FOR	A	PARTICULAR PURPOSE.	 See the
* GNU General Public	License	for	more details.
* 
* You should	have received	a	copy of	the	GNU	General	Public License
* along with	this program;	if not,	write	to the Free	Software
* Foundation, Inc., 59	Temple Place,	Suite	330, Boston, MA	 02111-1307	 USA
* --------------------------------------------------------------------
*/

#include "therion.h"

#include "thsketch.h"
#include "thscrap.h"
#include "thwarpp.h"
#include "thconfig.h"


thsketch::thsketch()
{
  this->m_x = 0.0;
  this->m_y = 0.0;
  this->m_warp = NULL;
}

thpic * thsketch::morph(double scale)
{
  if (this->m_warp == NULL) {
    switch (thcfg.sketch_warp) {
      case THSKETCH_WARP_LINEAR:
        this->m_warp = std::make_unique<thwarplin>();
        break;
      case THSKETCH_WARP_IDPOINT:
        this->m_warp = std::make_unique<thwarpinvdist>();
        break;
      case THSKETCH_WARP_IDLINE:
        //this->m_warp = new thwarpinvdistln;
        this->m_warp = std::make_unique<thwarpfastinvdistln>();
        break;
      default:
        this->m_warp = std::make_unique<thwarpp>();
    }
  }
  return this->m_warp->morph(this, scale);
}

