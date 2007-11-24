/**
* @file warpp.cxx
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


#include "thwarpp.h"
#include "thscrap.h"
#include "th2ddataobject.h"
#include "thpoint.h"


thwarpp::~thwarpp() {}


void thwarpp::self_delete()
{
  delete this;
}


thscrap * thwarpp::get_scrap()
{
  if (this->m_sketch != NULL)
    return this->m_sketch->m_scrap;
  return NULL;
}


void thwarpp::process_station_vector(	thwarptrans	&	tw,	double ph	)
{
  thsketchst_list::iterator	ii;
  for	(ii	=	this->stations.begin();	ii !=	this->stations.end();	ii++)	{
    thdb1ds	&	st = thdb.db1d.station_vec[ii->station.id	-	1];
    std::string	name(	st.name	);
    tw.insert_point( ii->code, name, thvec2(ii->x, ph	-	ii->y),
      thvec2(st.x,	-	st.y)	);
    // thprintf("insert	%s from	%.2f %.2f	to %.2f	%.2f \n",
    //					st.name, ii->x,	ph - ii->y,	st.x,	-	st.y );
  }
}

void thwarpp::process_leg_vector(	thwarptrans	&	tw )
{
  for	(	thsketchlg_list::iterator	jj = this->legs.begin(); jj	!= this->legs.end(); ++jj	)	{
    //	thprintf("insert line	%s %s\n",	jj->from.name, jj->to.name );
    std::string	f( jj->from.name );
    std::string	t( jj->to.name );
    tw.insert_line(	jj->code,	f, t );
  }
}


thpic * thwarpp::morph(thsketch * sketch, double scale) {

  thwarp::morph(sketch, scale);
  double sf = scale;

  if (!this->m_sketch->m_pic.exists())
    return NULL;

  const char * fn;
  size_t fnx, fnl;
  fn = sketch->m_pic.fname;
  fnl = strlen(sketch->m_pic.fname);
  for(fnx = 0; fnx < fnl; fnx++) {
    if (((sketch->m_pic.fname[fnx] == '/') || (sketch->m_pic.fname[fnx] == '\\')) && (fnx < fnl - 1)) {
      fn = &(sketch->m_pic.fname[fnx + 1]);
    }
  }
  if (thtext_inline) thprintf("\n");
  thprintf("morphing %s ...", fn);
  thtext_inline = true;

  this->m_sketch->m_pic.rgba_load();
  long pw	=	this->m_sketch->m_pic.width;
  long ph	=	this->m_sketch->m_pic.height;

  // FIXME could use bounds	(defaults	1.5	1.5	2.0)
  // thwarptrans TW( b2, b3, b4	);
  thwarptrans	TW;

  thscrap	*	scrap	=	this->get_scrap();
  thdb2dcp *ccp;
  thbuffer n2sb, n2sb2;
  n2sb.guarantee(128);
  n2sb2.guarantee(128);
  std::string s, s2;
  std::map <unsigned long, thdb1ds*> ssm;
  //std::map <unsigned long, thdb1ds*>::iterator ssmi;
  ccp = scrap->fcpp;
  while (ccp != NULL) {
    if ((ccp->point != NULL) && (ccp->st != NULL)) {
      sprintf(n2sb.get_buffer(),"%ld",ccp->st->uid);
      s = n2sb.get_buffer();
      TW.insert_point( THMORPH_STATION,	s,	
        thvec2(ccp->pt->x - sketch->m_x, sketch->m_y + double(sketch->m_pic.height) - ccp->pt->y),
        thvec2(ccp->tx + sketch->m_scrap->proj->rshift_x,- (ccp->ty + sketch->m_scrap->proj->rshift_y)));

      ssm[ccp->st->uid] = &(thdb.db1d.station_vec[ccp->st->uid - 1]);
    }
    ccp = ccp->nextcp;
  }

  thdb1d_leg_vec_type::iterator	lg = thdb.db1d.leg_vec.begin();
  unsigned long fuid, tuid;
  while	(	lg !=	thdb.db1d.leg_vec.end()	)	{
    thdataleg	*	dlg	=	lg->leg;
    fuid = thdb.db1d.station_vec[dlg->from.id - 1].uid;
    tuid = thdb.db1d.station_vec[dlg->to.id - 1].uid;
    sprintf(n2sb.get_buffer(),"%ld",fuid);
    sprintf(n2sb2.get_buffer(),"%ld",tuid);
    s = n2sb.get_buffer();
    s2 = n2sb2.get_buffer();
    if ((ssm.find(fuid) != ssm.end()) && (ssm.find(tuid) != ssm.end())) {
      // thprintf("insert	leg	%s %s\n",	dlg->from.name,	dlg->to.name );
      TW.insert_line(	THMORPH_STATION, s, s2);
    }
    ++ lg;
  }

  th2ddataobject * pobj = scrap->fs2doptr;
  int n_extra = 0;
  while (pobj != NULL) {
    if (pobj->get_class_id() == TT_POINT_CMD) {
      thpoint * pointp = (thpoint *) pobj;
      if (pointp->type == TT_POINT_TYPE_EXTRA) {
        pointp->check_extra();
	if ((pointp->from_name.id > 0) && (!thisnan(pointp->xsize))) {
	  unsigned long fuid = thdb.db1d.station_vec[pointp->from_name.id - 1].uid;
	  if ( ssm.find(fuid) == ssm.end() ) {
	    thprintf("warning: extra point from %s but no station\n",
	      pointp->from_name.name );
	  } else {
            sprintf(n2sb2.get_buffer(),"%ld",fuid);
            sprintf(n2sb.get_buffer(),"%ld_E_%d",fuid, ++n_extra);
            s  = n2sb.get_buffer(); 
            s2 = n2sb2.get_buffer();
	    thdb2dpt * pt = pointp->point;
	    // assert( pt != NULL );
	    double x = pt->x - sketch->m_x;
	    double y = sketch->m_y + double(sketch->m_pic.height) - pt->y;
         
            TW.insert_zoom_point( THMORPH_EXTRA, s, thvec2(x, y), s2, pointp->xsize );
            // ssm[ccp->st->uid] = &(thdb.db1d.station_vec[ccp->st->uid - 1]);
	  }
	}
      }
    }
    pobj = pobj->nscrapoptr;
  }


  TW.initialize();

  // the factor	of 4 is	good compromise	between	quality	and	speed
  double myscale = 4.0 * TW.to_unit()	/	TW.from_unit();

  long mw	=	(long)(	pw * myscale * sf	/	4.0	);
  long mh	=	(long)(	ph * myscale * sf	/	4.0	);
  this->mpic.rgba_init(mw, mh);

  thvec2 origin( mw/2.0, mh/2.0	);
  double unit	=	TW.to_unit() * sf;
  TW.map_image(	(const unsigned	char*)this->m_sketch->m_pic.rgba,	pw,	ph,
    (unsigned	char*)this->mpic.rgba, mw, mh, 
    origin,	unit,
    4	);

  this->mpic.rgba_save("PNG","png");
  this->mpic.rgba_free();
  this->m_sketch->m_pic.rgba_free();
#if	0
  this->mpic.scale = T3.m_scale;
  ptmp = T3.forward(mpic_origin);
  this->mpic.x = ptmp.m_x;
  this->mpic.y = ptmp.m_y;
#else
  this->mpic.scale = TW.to_unit()	/	TW.warped_unit();
  this->m_origin = TW.to_origin();
  this->mpic.x = - origin.m_x	*	this->mpic.scale;
  this->mpic.y = - origin.m_y	*	this->mpic.scale;
#endif

  this->mpic.x += this->m_origin.m_x;
  this->mpic.y -= this->m_origin.m_y;

  thprintf(" done\n");
  thtext_inline = false;

  return &(this->mpic);
}



