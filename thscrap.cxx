/**
 * @file thscrap.cxx
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
 
#include "thscrap.h"
#include "thexception.h"
#include "thchenc.h"
#include "thtflength.h"
#include "thinfnan.h"
#include "thdb2d.h"
#include "thdb2dmi.h"
#include "th2ddataobject.h"
#include "thline.h"
#include "thpoint.h"

thscrap::thscrap()
{
  this->fs2doptr = NULL;
  this->ls2doptr = NULL;
  this->fcpp = NULL;
  this->lcpp = NULL;
  this->ncp = 0;

  this->proj = thdb.db2d.get_default_projection();
  this->proj_next_scrap = NULL;
  this->proj_prev_scrap = NULL;
  this->xscrap = NULL;
  
  this->scale = 1.0;
  this->scale_r1x = 0.0;
  this->scale_r1y = 0.0;
  this->scale_r2x = 1.0;
  this->scale_r2y = 0.0;
  this->scale_p1x = 0.0;
  this->scale_p1y = 0.0;
  this->scale_p2x = 1.0;
  this->scale_p2y = 0.0;
  this->scale_p9 = false;
  this->reset_limits();

  this->reset_transformation();

  this->outline_parsed = false;
  this->outline_first = NULL;
  this->ends_parsed = false;
  this->ends_first = NULL;
  this->polygon_parsed = false;
  this->polygon_first = NULL;
  this->polygon_last = NULL;
  
}


thscrap::~thscrap()
{
}


int thscrap::get_class_id() 
{
  return TT_SCRAP_CMD;
}


bool thscrap::is(int class_id)
{
  if (class_id == TT_SCRAP_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thscrap::get_cmd_nargs() 
{
  return 1;
}


char * thscrap::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


char * thscrap::get_cmd_name()
{
  // insert command name here
  return "scrap";
}


thcmd_option_desc thscrap::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_scrap_opt);
  if (id == TT_SCRAP_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thscrap::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  
  thdb2dprjpr projection;
  if (cod.id == 1)
    cod.id = TT_DATAOBJECT_NAME;
    
  switch (cod.id) {

//    case TT_DATAOBJECT_NAME:
//      if (th_is_keyword(*args))
//        this->name = this->db->strstore(*args);
//      else 
//        ththrow(("invalid keyword -- %s", *args));
//      break;
      
    case TT_SCRAP_PROJECTION:
      projection = this->db->db2d.parse_projection(*args);
      if (!projection.parok)
        ththrow(("invalid parameters of projection"));
      this->proj = projection.prj;
      break;
    
    case TT_SCRAP_SCALE:
      this->parse_scale(*args);
      break;
    
    case TT_SCRAP_STATIONS:
      this->parse_stations(*args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}

void thscrap::self_delete()
{
  delete this;
}


void thscrap::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thscrap:\n");
  fprintf(outf,"\tprojection: %d\n",this->proj->id);
  fprintf(outf,"\tscale: %f\n",this->scale);
  fprintf(outf,"\tz: %f\n",this->z);
  // insert intended print of object properties here
}


void thscrap::parse_scale(char * ss)
{
  thsplit_words(& this->db->db2d.mbf,ss);
  int npar = this->db->db2d.mbf.get_size();
  char ** pars = this->db->db2d.mbf.get_buffer();
  if ((npar < 1) || ((npar > 3) && (npar != 8) && (npar != 9)))
    ththrow(("invalid number of scale arguments -- %d",npar))
  double n1, n2;
  bool p9 = false;
  int sv, ux = 0, n2x = 0;
  thtflength lentf;
  switch (npar) {
    case 2:
      ux = 1;
      break;
    case 3:
      ux = 2;
      n2x = 1;
      break;
    case 9:
      ux = 8;
    case 8:
      p9 = true;
      break;
  }
  this->scale_p9 = p9;

#define parse_scalep9(var,parn) \
    thparse_double(sv,var,pars[parn]); \
    if (sv != TT_SV_NUMBER) \
      ththrow(("real number required -- %s", pars[parn]));

  if (p9) {
//    thparse_double(sv,this->scale_r1x,pars[0]);
//    if ((sv != TT_SV_NUMBER) || (n1 <= 0.0))
//      ththrow(("real positive number required -- %s", pars[0]));
      parse_scalep9(this->scale_p1x,0)
      parse_scalep9(this->scale_p1y,1)
      parse_scalep9(this->scale_p2x,2)
      parse_scalep9(this->scale_p2y,3)
      parse_scalep9(this->scale_r1x,4)
      parse_scalep9(this->scale_r1y,5)
      parse_scalep9(this->scale_r2x,6)
      parse_scalep9(this->scale_r2y,7)
      if (hypot(this->scale_r1x - this->scale_r2x,
        this->scale_r1y - this->scale_r2y) == 0.0)
        ththrow(("zero scale real length"));
      if (hypot(this->scale_p1x - this->scale_p2x,
        this->scale_p1y - this->scale_p2y) == 0.0)
        ththrow(("zero scale picture length"));        
  }
  else {
    // let's parse first number
    thparse_double(sv,n1,pars[0]);
    if ((sv != TT_SV_NUMBER) || (n1 <= 0.0))
      ththrow(("real positive number required -- %s", pars[0]));
      
    // let's parse second number
    if (n2x > 0) {
      thparse_double(sv,n2,pars[n2x]);
      if ((sv != TT_SV_NUMBER) || (n2 <= 0.0))
        ththrow(("real positive number required -- %s", pars[0]));
      n1 = n2 / n1;
    }
  }    
  // let's parse units
  if (ux > 0) {
    lentf.parse_units(pars[ux]);
    if (p9) {
      this->scale_r1x = lentf.transform(this->scale_r1x);
      this->scale_r1y = lentf.transform(this->scale_r1y);
      this->scale_r2x = lentf.transform(this->scale_r2x);
      this->scale_r2y = lentf.transform(this->scale_r2y);
    } 
    else {
      n1 = lentf.transform(n1);
    }
  }
  
  this->scale = n1;
}

thdb2dcp * thscrap::insert_control_point()
{
  thdb2dcp * cp = this->db->db2d.insert_control_point();
  if (this->fcpp == NULL) {
    this->fcpp = cp;
    this->lcpp = cp;
    this->ncp = 1;
  }
  else {
    cp->prevcp = this->lcpp;
    this->lcpp->nextcp = cp;
    this->lcpp = cp;
    this->ncp++;
  }
  return cp;
}

void thscrap::reset_limits() {
  this->lxmin = thnan;
  this->lxmax = thnan;
  this->lymin = thnan;
  this->lymax = thnan;
}


void thscrap::reset_transformation() {
  this->mx = 0.0;
  this->my = 0.0;
  this->mxx = 1.0;
  this->mxy = 0.0;
  this->myx = 0.0;
  this->myy = 1.0;
  this->mr = 0.0;
  this->ms = 1.0;
}


thscraplo * thscrap::get_outline() {
  if (this->outline_parsed)
    return this->outline_first;
  //if (!this->proj->processed)
  //  this->db->db2d.process_projection(this->proj);
  this->outline_parsed = true;

#ifdef THDEBUG
  thprintf("\n\nscanning outline: scrap %s\n",this->name);
#endif
  
  th2ddataobject * obj = this->fs2doptr;
//  thline * pln;
  thscraplo * co, * co2, * co3, * co3last, * lo = NULL, ** pco3;
  double cdist, mindist;

  while (obj != NULL) {
    if ((obj->get_class_id() == TT_LINE_CMD) &&
        (((thline *)obj)->first_point != NULL) &&
        ((((thline *)obj)->outline == TT_LINE_OUTLINE_IN) ||
        (((thline *)obj)->outline == TT_LINE_OUTLINE_OUT))) {
      co = this->db->db2d.insert_scraplo();
      co->line = (thline *) obj;
      if (this->outline_first == NULL) {
        this->outline_first = co;
        lo = co;
      } else {
			  // vzdialenost moojho konca od zaciatku
			  mindist = hypot(co->line->last_point->point->x - 
					co->line->first_point->point->x, 
					co->line->last_point->point->y - 
					co->line->first_point->point->y);
        if ((this->outline_first->line->outline == TT_LINE_OUTLINE_IN) &&
            (((thline *)obj)->outline == TT_LINE_OUTLINE_OUT)) {
          co->next_scrap_line = this->outline_first;
          this->outline_first = co;
        } else {
					// poojdeme odpredu
					co3 = this->outline_first;
					pco3 = &(this->outline_first);
					while (co3 != NULL) {
						if ((co3->line->outline == co->line->outline) &&
								(hypot(co3->line->last_point->point->x - 
										co3->line->first_point->point->x, 
										co3->line->last_point->point->y - 
										co3->line->first_point->point->y) < mindist)) {
							co->next_scrap_line = co3;
							*pco3	= co;
							break;
						}
						pco3 = &(co3->next_scrap_line);	
					  co3 = co3->next_scrap_line;
						if (co3 == NULL) {
	          	lo->next_scrap_line = co;
  	        	lo = co;
						}
					}
        }
      }
    }
    obj = obj->nscrapoptr;
  }
  
  bool still_in_line, co3_normal = false;
  thdb2dpt * first_pt, * last_pt;
  if (this->outline_first != NULL) {
    co = this->outline_first;
    while (co != NULL) {
      first_pt = co->line->first_point->point;
      last_pt = co->line->last_point->point;
      co->mode = TT_OUTLINE_NORMAL;
#ifdef THDEBUG
      thprintf("\tfirst line: %d - mode %d\n",co->line->id,co->line->outline);
#endif
      still_in_line = !(co->line->is_closed);
      co3 = co;
      while (still_in_line) {
        mindist = hypot(last_pt->x - first_pt->x, last_pt->y - first_pt->y);
        co2 = co->next_scrap_line;
        co3last = co3;
        co3 = NULL;
        while (co2 != NULL) {
          if ((co2->mode == TT_OUTLINE_NO) &&
              (co2->line->outline == co->line->outline)) {
            cdist = hypot(co2->line->last_point->point->x - last_pt->x,
                          co2->line->last_point->point->y - last_pt->y);
            if (cdist <= mindist) {
              co3_normal = false;
              co3 = co2;
              mindist = cdist;
            }
            cdist = hypot(co2->line->first_point->point->x - last_pt->x,
                          co2->line->first_point->point->y - last_pt->y);
            if (cdist <= mindist) {
              co3_normal = true;
              co3 = co2;
              mindist = cdist;
            }
          }
          co2 = co2->next_scrap_line;
        }
        if (co3 == NULL)
          still_in_line = false;
        else {
          co3last->next_line = co3;
#ifdef THDEBUG
          thprintf("\tnext line: %d %s\n",co3->line->id,(co3_normal ? "normal" : "reversed"));
#endif
          if (co3_normal) {
            co3->mode = TT_OUTLINE_NORMAL;
            last_pt = co3->line->last_point->point;
          }
          else {
            co3->mode = TT_OUTLINE_REVERSED;
            last_pt = co3->line->first_point->point;
          }
        }
      }
      
      // podme na dalsi
      co2 = co->next_scrap_line;
      while ((co2 != NULL) && (co2->mode != TT_OUTLINE_NO))
        co2 = co2->next_scrap_line;
      co->next_outline = co2;
      co = co2;
    }
  }
  
  return this->outline_first;
  
} // get_outline


thscraplp * thscrap::polygon_insert() {
  thscraplp * rv = this->db->db2d.insert_scraplp();
  if (this->polygon_last == NULL) {
    this->polygon_first = rv;
    this->polygon_last = rv;
  } else {
    this->polygon_last->next_item = rv;
    rv->prev_item = this->polygon_last;
    this->polygon_last = rv;
  }
  return rv;
}


void thscrap::parse_stations(char * ss) {
  thsplit_words(& this->db->db2d.mbf,ss);
  int npar = this->db->db2d.mbf.get_size(), ipar = 0;
  char ** pars = this->db->db2d.mbf.get_buffer();
  thscraplp * lp;
  while (ipar < npar) {
    lp = this->polygon_insert();
    thparse_objectname(lp->station_name,&(this->db->buff_stations),*pars);
    pars++;
    ipar++;
  }
}


thscraplp * thscrap::get_polygon() {

  if (this->polygon_parsed)
    return this->polygon_first;
  if (!this->proj->processed)
    this->db->db2d.process_projection(this->proj);

  double sina = 0.0, cosa = 0.0;
  switch (this->proj->type) {
    case TT_2DPROJ_ELEV:
      sina = sin(this->proj->pp1 / 180.0 * 3.14159265358);
      cosa = cos(this->proj->pp1 / 180.0 * 3.14159265358);
      break;
  }
  bool incp;
  thdb2dcp * cp;
  thscraplp * lp, * nlp;
  thdb1ds * st, * st1, * st2;
  thdb1d_tree_arrow * arrow;
  thdb1d_tree_node * nodes = this->db->db1d.get_tree_nodes();

  switch (this->proj->type) {
    case TT_2DPROJ_PLAN:
    case TT_2DPROJ_ELEV:
      incp = true;
      lp = NULL;
      cp = this->fcpp;
      while (incp || (lp != NULL)) {
        if (incp && (cp == NULL)) {
          incp = false;
          lp = this->polygon_first;
        }
        if ((cp != NULL) || (lp != NULL)) {
          if (lp != NULL) {
            st = lp->ustation;
            if (st != NULL) {
              switch (this->proj->type) {
                case TT_2DPROJ_PLAN:
                  lp->stx = st->x - this->proj->shift_x;
                  lp->sty = st->y - this->proj->shift_y;
                  break;
                case TT_2DPROJ_ELEV:
                  lp->stx = cosa * (st->x - this->proj->shift_x) 
                            - sina * (st->y - this->proj->shift_y);
                  lp->sty = st->z - this->proj->shift_z;
                  break;
              }
            }
          } else
            st = cp->st;
            
          if ((st != NULL) && (nodes != NULL)) {
            // let's process each arrow from this point
            arrow = nodes[st->uid - 1].first_arrow;
            while (arrow != NULL) {
              // process arrow here
              nlp = this->polygon_insert();
              nlp->lnio = true;
              st1 = &(this->db->db1d.station_vec[arrow->start_node->uid - 1]);
              st2 = &(this->db->db1d.station_vec[arrow->end_node->uid - 1]);
              switch (this->proj->type) {
                case TT_2DPROJ_PLAN:
                  nlp->lnx1 = st1->x - this->proj->shift_x;
                  nlp->lny1 = st1->y - this->proj->shift_y;
                  nlp->lnx2 = st2->x - this->proj->shift_x;
                  nlp->lny2 = st2->y - this->proj->shift_y;
                  break;
                case TT_2DPROJ_ELEV:
                  nlp->lnx1 = cosa * (st1->x - this->proj->shift_x) 
                            - sina * (st1->y - this->proj->shift_y);
                  nlp->lny1 = st1->z - this->proj->shift_z;
                  nlp->lnx2 = cosa * (st2->x - this->proj->shift_x) 
                            - sina * (st2->y - this->proj->shift_y);
                  nlp->lny2 = st2->z - this->proj->shift_z;
                  break;
              }
              arrow = arrow->next_arrow;
            }
          }
        }
        if (cp != NULL) {
          cp = cp->nextcp;
        }
        if (lp != NULL) {
          lp = lp->next_item;
        }
      }
      break;
    
    
    case TT_2DPROJ_EXTEND:
      cp = this->fcpp;
      while (cp != NULL) {
        if (cp->used_in_attachement) {
          nlp = this->polygon_insert();
          nlp->lnio = true;
          nlp->lnx1 = cp->tx;
          nlp->lny1 = cp->ty;
          nlp->lnx2 = cp->point->extend_point->cpoint->tx;
          nlp->lny2 = cp->point->extend_point->cpoint->ty;
        }
        cp = cp->nextcp;
      }
      break;
      
  }
  
  this->polygon_parsed = true;
  return this->polygon_first;
}

void thscrap::calc_z()
{
  thdb2dcp * cp = this->fcpp;
  this->z = 0.0;
  unsigned long numcp = 0;
  while(cp != NULL) {
    if (cp->st != NULL) {
      this->z += cp->tz;
      numcp++;
    }
    cp = cp->nextcp;
  }
  this->z /= double(numcp);
}


thdb1ds * thscrap::get_nearest_station(thdb2dpt * pt) {

  double mindist, cdist;
  thdb1ds * res = NULL;
  mindist = -1.0;

  // skusi kontrolne body
  thdb2dcp * cp = this->fcpp;
  while(cp != NULL) {
    if (cp->st != NULL) {
      cdist = hypot(cp->pt->xt - pt->xt,cp->pt->yt - pt->yt);
      if ((cdist < mindist) || (mindist < 0.0)) {
        res = cp->st;
        mindist = cdist;
      }
    }
    cp = cp->nextcp;
  }
  
  // skusi pridany polygon
  thscraplp * lp = this->polygon_first;
  while(lp != NULL) {
    if (lp->station != NULL) {
      cdist = hypot(lp->stx - pt->xt,lp->sty - pt->yt);
      if ((cdist < mindist) || (mindist < 0.0)) {
        res = lp->station;
        mindist = cdist;
      }
    }
    lp = lp->next_item;
  }
  
  return res;
}

thscrapen * thscrap::get_ends() {
  if (this->ends_parsed)
    return this->ends_first;
  bool search_inv, cont, rev, lastvis, cvis = false, try_next_line;
  thdb2dlp * first_point, * clp;
  thscrapen * res = NULL, * cres;
  thscraplo * lo = this->get_outline(), * lo2, * nextlo;
  while (lo != NULL) {
    if (lo->line->outline == TT_LINE_OUTLINE_OUT) {
      lo2 = lo;
      search_inv = true;
      cont = true;
      first_point = NULL;
      lastvis = (lo->mode == TT_OUTLINE_REVERSED ?
        ((lo->line->last_point->subtype != TT_LINE_SUBTYPE_INVISIBLE) &&
        (lo->line->type == TT_LINE_TYPE_WALL)) :
        ((lo->line->first_point->subtype != TT_LINE_SUBTYPE_INVISIBLE) &&
        (lo->line->type == TT_LINE_TYPE_WALL)));
      while (cont) {
        if ((lo2 == NULL) && (first_point != NULL))
          lo2 = lo;
        if (lo2 == NULL)
          break;  
        // teraz podme po bodoch krivky
        rev = (lo2->mode == TT_OUTLINE_REVERSED);
        if (rev)
          clp = lo2->line->last_point;
        else
          clp = lo2->line->first_point;

        while (cont && (clp != NULL)) {

          
          // urcime sucanu viditelnost
          try_next_line = false;
          if (rev) {
            if (clp->prevlp != NULL)
              cvis = ((clp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE) &&
                (lo2->line->type == TT_LINE_TYPE_WALL));
            else 
              try_next_line = true;
          } else {
            if (clp->nextlp != NULL) 
              cvis = ((clp->subtype != TT_LINE_SUBTYPE_INVISIBLE) &&
                (lo2->line->type == TT_LINE_TYPE_WALL));
            else 
              try_next_line = true;
          }
          
          // skusi nasledovnu ciaru       
          if (try_next_line) {
          
            cvis = true;

            if (lo2->next_line != NULL)
              nextlo = lo2->next_line;
            else
              nextlo = lo;
            
            // ak je dalsia normalne natocena
            if (nextlo->mode == TT_OUTLINE_REVERSED) {
              if ((nextlo->line->last_point->point->x != clp->point->x) || 
                  (nextlo->line->last_point->point->y != clp->point->y))  
                cvis = false;  
              else if ((nextlo->line->last_point->prevlp != NULL) &&
                  ((nextlo->line->last_point->prevlp->subtype == TT_LINE_SUBTYPE_INVISIBLE) || 
                  (nextlo->line->type != TT_LINE_TYPE_WALL)))
                cvis = false;  
            } else {  
              if ((nextlo->line->first_point->point->x != clp->point->x) || 
                  (nextlo->line->first_point->point->y != clp->point->y))
                cvis = false;
              else if ((nextlo->line->first_point->subtype == TT_LINE_SUBTYPE_INVISIBLE) ||
                (nextlo->line->type != TT_LINE_TYPE_WALL))
                cvis = false;  
            }              
            
          }
      
          // zistime ci mame bod
          if ((cvis != lastvis) && (search_inv != cvis)) {
          
            // mame bod, teraz ho priradime            
            if (search_inv) {
              cres = this->db->db2d.insert_scrapen();
              if (first_point != clp) {
                if (res == NULL)
                  res = cres;
                else {
                  cres->next_end = res;
                  res = cres;
                }
                res->l1 = lo2->line;
                res->lp1 = clp;
              } else
                cont = false;
              if (first_point == NULL)
                first_point = clp; 
            } else {
              res->l2 = lo2->line;
              res->lp2 = clp;
            }
            search_inv = !search_inv;
          
          }
          
          // podme na dalsi bod
          if (rev)
            clp = clp->prevlp;
          else
            clp = clp->nextlp;
          lastvis = cvis;
        }
        lo2 = lo2->next_line;
      }
    }
    lo = lo->next_outline;
  }
  this->ends_parsed = true;
  this->ends_first = res;
  return res;
}

