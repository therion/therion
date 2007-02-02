/**
 * @file thscrapis.cxx
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
 
#include "thscrapis.h"
#include "therion.h"
#include "thdb2dlp.h"
#include "thscraplo.h"
#include "thline.h"
#include "thconfig.h"
#include "thcdt.h"
#include "thscraplp.h"
#include <math.h>
#ifdef THMSVC
#define hypot _hypot
#endif

#define ONLY_INVISIBLE_LINE 1
#define THSCRAPISSMP thcfg.tmp3dSMP
#define THSCRAPISWALLSMP thcfg.tmp3dWALLSMP
#define THSCRAPISMAXDIMD thcfg.tmp3dMAXDIMD

thscrapis::thscrapis() {
  this->track_bottom = true;
  this->firstbp = NULL;  
  this->firstbbp = NULL;  
  this->firstdim = NULL;  
  this->firstolseg = NULL;
  this->lastolseg = NULL;
  this->tri_triangles = NULL;
  this->tri_opts = NULL;
  this->maxol = NULL;
  this->tri_num = 0;
  
  // interpolation variables
  this->first1ip = NULL;
  this->last1ip = NULL;
  this->first1il = NULL;
  this->last1il = NULL;
  this->first2ip = NULL;
  this->last2ip = NULL;
  
}



thscrapis::~thscrapis() {
  if (this->tri_opts != NULL)
    delete [] this->tri_opts;
  if (this->tri_triangles != NULL)
    delete [] this->tri_triangles;
}
 

 
bool operator < (const struct thscrapisloc & l1, 
  const struct thscrapisloc & l2)
{
  if (l1.x + THSCRAPISRES < l2.x)
    return true;
  else if (l2.x + THSCRAPISRES < l1.x)
    return false;
  else if (l1.y + THSCRAPISRES < l2.y)
    return true;
  else 
    return false;
}



void thscrapis::insert_bp(double x, double y, double z)
{
  thscrapisbp * mi = &(this->bp_map[thscrapisloc(x,y)]);
  if (mi->sums < 0) {
    mi->x = x;
    mi->y = y;
    mi->z = z;
    mi->sums = 0.0;
  } else {
    if (z < mi->z) {
      mi->z = z;
    }
  }
}



void thscrapis::end_bp()
{
  // vytvori odkazy
  std::map <thscrapisloc, thscrapisbp> ::iterator it = this->bp_map.begin();
  thscrapisbp * cbp = &(it->second);
  this->firstbp = cbp;
  it++;
  while (it != this->bp_map.end()) {
//    thprintf("BP: %.2f %.2f %.2f\n", cbp->x, cbp->y, cbp->z);
    cbp->next = &(it->second);
    cbp = cbp->next;
    it++;
  }
}
  


void thscrapis::insert_bp_direction(double x, double y, double z, double tx, double ty, double tz, thscraplp * slp)
{
  // ak najde (x,y), tak prida direction
  std::map <thscrapisloc, thscrapisbp> ::iterator 
    it = this->bp_map.find(thscrapisloc(x,y));
  if (it == this->bp_map.end())
    return;
  double dx = tx - x, 
    dy = ty - y,
    dz = tz - z,
    dl = hypot(dx, dy), d, tmpup, tmpdown;
  it->second.suml += hypot(dl,dz);
  it->second.sumsl += 1.0;
  if (dl < THSCRAPISRES) {
    if ((dz > 1.618) && (dz > it->second.up))
      it->second.up = dz;
    if ((dz < -1.618) && (dz < -(it->second.down)))
      it->second.down = -dz;
    return;
  }
  if (slp->arrow != NULL) {
    tmpup = (slp->arrow->is_reversed ? slp->arrow->leg->leg->to_up : slp->arrow->leg->leg->from_up);
    tmpdown = (slp->arrow->is_reversed ? slp->arrow->leg->leg->to_down : slp->arrow->leg->leg->from_down);
    if (fabs(slp->arrow->leg->leg->total_gradient) >= slp->arrow->leg->leg->vtresh) {
      d = cos(slp->arrow->leg->leg->total_gradient / 180.0 * THPI);     
    } else {
      d = 1.0;
    }
    if (!thisnan(tmpup)) {
      tmpup *= d;
      if (tmpup > it->second.up)
        it->second.up = tmpup;
    }
    if (!thisnan(tmpdown)) {
      tmpdown *= d;
      if (tmpdown > it->second.down)
        it->second.down = tmpdown;
    }
  }
  if (dy < 0) {
    dx *= -1.0;
    dy *= -1.0;
  }
  d = atan2(dx, dy);
  it->second.sumd += d;
  it->second.sums += 1.0;
}
  


void thscrapis::end_bp_direction()
{
  // predeli directions
  this->firstbbp = this->firstbp;
  thscrapisbp * cbp = this->firstbp;
  while (cbp != NULL) {
    if (cbp->sums > 0.0) {
      cbp->sumd /= cbp->sums;
      cbp->dx = sin(cbp->sumd);
      cbp->dy = cos(cbp->sumd);
      cbp->dd = true;
    } else {
      cbp->dx = 0.0;
      cbp->dy = 0.0;
    }
    if (cbp->sumsl > 0.0) {
      cbp->suml /= cbp->sumsl;
      cbp->suml *= 0.618;
      if ((cbp->up < 0.0) && (cbp->down < 0.0)) {
        cbp->down = cbp->suml / 1.618;
        if (cbp->down > 0.618)
          cbp->down = 0.618;
        cbp->up = cbp->suml - cbp->down;
        if (cbp->up > 1.618)
          cbp->up = 1.618;
      } else if (cbp->up < 0.0) {
        cbp->up = 1.618;
      } else if (cbp->down < 0.0) {
        cbp->down = 1.618;
      }
    } 
    if (cbp->up < 0.0) {
      cbp->up *= -1.0;
      if (cbp->up > 1.618)
        cbp->up = 1.618;
    }
    if (cbp->down < 0.0) {
      cbp->down *= -1.0;
      if (cbp->down > 0.618)
        cbp->down = 0.618;
    }
    cbp = cbp->next;
  }
}
  
void thscrapis::bp_interpolate(double x, double y, double & iz, double & id, double & idx, double & idy)
{
  // prejde vsetky body a spocita interpolovane z a direction
  double sumw = 0.0, sumdw = 0.0, cw, dx, dy, dd;
  bool anyd = false;
  iz = 0.0;
  id = 0.0;
  thscrapisbp * cbp = this->firstbp;
//  thprintf("interpolation %.2f %.2f\n", x, y);
  while (cbp != NULL) {
    dx = x - cbp->x;
    dy = y - cbp->y;
    cw = hypot(dx, dy);
    if (cw < THSCRAPISRES) {
      iz = cbp->z;
      id = cbp->sumd;
      idx = cbp->dx;
      idy = cbp->dy;
      return;
    }
    //cw = pow(cw,-2.0) * exp(-2.0 * pow(dx * cbp->dx + dy * cbp->dy, 2.0));
    dd = dx * cbp->dx + dy * cbp->dy;
    cw = 1 / (cw * cw) * exp(-2.0 * dd * dd);
    if (cbp->dd) {
      id += cw * cbp->sumd;
      sumdw += cw;
      anyd = true;
    }
//    thprintf("inserting z: %.2f %.2f -> %.2f x %.2f\n", cbp->x, cbp->y, cw, cbp->z);
    iz += cw * cbp->z;
    sumw += cw;
    cbp = cbp->next;
  }
  if (sumw == 0.0)
    return;
  iz /= sumw;
  if (anyd) {
    id /= sumdw;
    idx = sin(id);
    idy = cos(id);
  } else {
    id = 0.0;
    idx = 0.0;
    idy = 0.0;
  }
}


void thscrapis::insert_bp_shot(double fx, double fy, double fz, double tx, double ty, double tz)
{

  // spocita dlzku
  double dx = tx - fx,
    dy = ty - fy,
    dz = tz - fz,
    dl = hypot(dx, ty), ddx, ddy, 
    dirf, dirt, smp, frc, cx, cy;
  long i, n;
  if (dl < THSCRAPISRES)
    return;
  if (dy < 0.0) {
    ddx = -1.0 * dx / dl;
    ddy = -1.0 * dy / dl;
  } else {
    ddx = dx / dl;
    ddy = dy / dl;
  }

  // skusi najst from shot a to shot a ich smery
  std::map <thscrapisloc, thscrapisbp> ::iterator 
    fit = this->bp_map.find(thscrapisloc(fx,fy)),
    tit = this->bp_map.find(thscrapisloc(tx,ty));
  thscrapisbp * cbp;
  if ((fit == this->bp_map.end()) && (tit == this->bp_map.end())) {
    dirf = atan2(ddx, ddy);
    dirt = dirf;
  } else if (fit == this->bp_map.end()) {
    dirt = tit->second.sumd;
    dirf = dirt;
  } else if (tit == this->bp_map.end()) {
    dirf = fit->second.sumd;
    dirt = dirf;
  } else {
    dirf = fit->second.sumd;
    dirt = tit->second.sumd;
  }
  
  // zacne vkladat body (na zaciatok), ale iba ak ich nemame
  n = long(dl / THSCRAPISSMP);
  smp = dl / (n + 1);
  for (i = 1; i <= n; i++) {
    frc = double(i) * smp / dl;
    cx = fx + frc * dx;
    cy = fy + frc * dy;
    cbp = &(this->bp_map[thscrapisloc(cx, cy)]);
    if (cbp->sums < 0.0) {
      cbp->x = cx;
      cbp->y = cy;
      cbp->z = fz + frc * dz;
      cbp->sumd = dirf + (dirt - dirf) * frc;
      cbp->dx = sin(cbp->sumd);
      cbp->dy = cos(cbp->sumd);
      cbp->sums = 0.0;
      cbp->next = this->firstbp;
      this->firstbp = cbp;
    } 
  }  
}

struct thscrapislns {
  double x, y;
  thscrapislns(double xx, double yy) : x(xx), y(yy) {}
};

void thscrapis::insert_outline_lnsegment(bool newsegment, thline * line, 
    bool outer, bool reverse, bool visible, long from, long to) 
{
  if (from == to)
    return;
  thdb2dlp * lp = line->first_point;
  thdb2dpt * prev_pt = NULL;
  std::list <thscrapislns> tmplist;
  tmplist.clear();
  double t,t2,t3,t_,t_2,t_3,x1,y1,c1x,c1y,c2x,c2y,x2,y2;
  long last = 0,i,nb = 20;
  bool dnu = false;
  while ((lp != NULL) && ((long(last) <= long(to)) || (to < 0))) {
    if (last == from) {
      dnu = true;
      tmplist.push_back(thscrapislns(lp->point->xt, lp->point->yt));
      prev_pt = lp->point;
    } 
    else if (dnu) {
      if ((lp->cp1 != NULL) && (lp->cp2 != NULL)) {
        x1 = prev_pt->xt;
        y1 = prev_pt->yt;
        c1x = lp->cp1->xt;
        c1y = lp->cp1->yt;
        c2x = lp->cp2->xt;
        c2y = lp->cp2->yt;
        x2 = lp->point->xt;
        y2 = lp->point->yt;
        for(i = 1; i < nb; i++) {
          t = double(i) / double(nb);
          t2 = t * t;
          t3 = t * t * t;
          t_ = 1.0 - t;
          t_2 = t_ * t_;
          t_3 = t_ * t_ * t_;
          tmplist.push_back(thscrapislns(
            t_3 * x1 + 3.0 * t * t_2 * c1x + 3 * t2 * t_ * c2x + t3 * x2
            , 
            t_3 * y1 + 3.0 * t * t_2 * c1y + 3 * t2 * t_ * c2y + t3 * y2
          ));
        }
      } 
      tmplist.push_back(thscrapislns(lp->point->xt, lp->point->yt));
    }
    prev_pt = lp->point;
    lp = lp->nextlp;
    last++;
  }
  
  // vlozi tmplist do zoznamu
  bool first_point = true;
  std::list <thscrapislns> :: iterator it;
  thscrapisolpt * cpt;
  if (tmplist.size() == 0)
    return;
  if (reverse) {
    it = tmplist.end();
    it--;
  } else {
    it = tmplist.begin();
  }  
  while (it != tmplist.end()) {
    // vlozime bod - osobitne prvy
    cpt = &(*(this->ol_list.insert(this->ol_list.end(),thscrapisolpt())));
    if (newsegment && first_point) {
      if (this->firstolseg == NULL) {
        this->firstolseg = cpt;
        this->lastolseg = cpt;
      } else {
        if (outer) {
          cpt->next_segment = this->firstolseg;
          this->firstolseg = cpt;
        } else {
          this->lastolseg->next_segment = cpt;
          this->lastolseg = cpt;
        }
      }
    } else {
      // pokracovanie segmentu
      this->lastolcseg->next = cpt;
    }
    
    if (first_point) {
       cpt->visible = false;
    } else {
      cpt->visible = visible;
    }
    cpt->x = it->x;
    cpt->y = it->y;
    cpt->outer = outer;
    this->lastolcseg = cpt;
    first_point = false;
    
    if (reverse) {
      if (it == tmplist.begin())
        it = tmplist.end();
      else
        it--;
    } else {
      it++;
    }
  }
}

void thscrapis::outline_scan(class thscraplo * outln) {
  // nacita
  bool first, outer, vis;
  long from, to;
  thdb2dlp * lp;
  thscraplo * lo = outln, * lo2;
  if (true) {  /// ;-) uz mame podporovanych viac vonkajsich a viac vnut. obrysov 
    while (lo != NULL) {
      outer = (lo->line->outline == TT_LINE_OUTLINE_OUT);
      // prejde celu krivku a rozlozi ju na viditelne a neviditelne casti
      first = true;
      lo2 = lo;
      while (lo2 != NULL) {              
        if (lo2->mode == TT_OUTLINE_REVERSED) {
          lp = lo2->line->first_point;
          from = 0;
          while (lp != NULL) {
            lp = lp->nextlp;
            from++;
          }
          from = from - 1;
          lp = lo2->line->last_point;
          to = from;
        } else {
          lp = lo2->line->first_point;
          from = 0;
          to = from;
        }
        
        bool cvis = false;
        while (lp != NULL) {

          // najde viditelnost prveho bodu
          if (lo2->mode == TT_OUTLINE_REVERSED) {
            if (lp->prevlp != NULL) {
#ifdef ONLY_INVISIBLE_LINE              
              vis = (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#else
              vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#endif                
            } else
              vis = cvis;
          } else {
#ifdef ONLY_INVISIBLE_LINE              
            vis = (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#else
            vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
              (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#endif                
          }
          cvis = vis;
          
          // pokracuje pokial nemame bod z inou viditelnostou
          while (lp != NULL) {
            
            if (lo2->mode == TT_OUTLINE_REVERSED) {
              if (lp->prevlp != NULL) {
#ifdef ONLY_INVISIBLE_LINE              
                vis = (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#else
                vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->prevlp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#endif                
              } else
                vis = cvis;
            } else {
#ifdef ONLY_INVISIBLE_LINE              
              vis = (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#else
              vis = (lo2->line->type == TT_LINE_TYPE_WALL) &&
                (lp->subtype != TT_LINE_SUBTYPE_INVISIBLE);
#endif                
            }
            
            if (vis != cvis)
              break;
            
            if (lo2->mode == TT_OUTLINE_REVERSED) {
              lp = lp->prevlp;
              if (lp != NULL)
                from--;
            } else {
              lp = lp->nextlp;
              if (lp != NULL)
                to++;
            }
          }
          
          this->insert_outline_lnsegment(first, lo2->line, outer, 
            (lo2->mode == TT_OUTLINE_REVERSED), cvis, from, to);
          first = false;
          if (lo2->mode == TT_OUTLINE_REVERSED)
            to = from;
          else
            from = to;
        }
        
        lo2 = lo2->next_line;
      }
      lo = lo->next_outline;
    }
  }
  
  // ZJEDNODUSENIE
#define print_outline(ARG) \
  oline = this->firstolseg; \
  thprintf("\n" ARG ":\n"); \
  while (oline != NULL) { \
    thprintf("------------------\n"); \
    olineln = oline; \
    while (olineln != NULL) { \
      thprintf("%s %6.2f %6.2f\n", olineln->visible ? "SHOW" : "HIDE", olineln->x, olineln->y); \
      olineln = olineln->next; \
    } \
    oline = oline->next_segment; \
  }
    
  
  // vyplni prev
  thscrapisolpt * oline, * olineln, * prevolln, * rlnode, * lastolln;

//  print_outline("BEFORE ALL");

  oline = this->firstolseg;
  while (oline != NULL) {
    oline->deletable = false;
    prevolln = oline;
    olineln = prevolln->next;
    while (olineln != NULL) {
      olineln->prev = prevolln;
      prevolln = olineln;
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }

  // odstrani duplikaty
  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline->next;
    prevolln = NULL;
    while (olineln != NULL) {
      if (hypot(olineln->x - olineln->prev->x, 
        olineln->y - olineln->prev->y) < THSCRAPISRES) {
        olineln->prev->next = olineln->next;
        if (olineln->next != NULL) {
          olineln->next->prev = olineln->prev;
        }
      } else {
        prevolln = olineln;
      }
      olineln = olineln->next;
    }
    
    // odstrani duplikat z konca  
    if ((prevolln != NULL) && (hypot(oline->x - prevolln->x, 
        oline->y - prevolln->y) < THSCRAPISRES)) {
      if (prevolln->prev != NULL) {
        prevolln->prev->next = NULL;
        oline->visible = prevolln->visible;
      }
    }
    
    oline = oline->next_segment;
  }

//  print_outline("AFTER DUPLICATES REMOVAL");

  // nastavi tie co sa nebudu mazat
  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline->next;
    while (olineln != NULL) {
      if ((olineln->prev != NULL) && (olineln->visible != olineln->prev->visible)) {
        olineln->prev->deletable = false;
      }
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }
  
  // zjednodusi
  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline->next;
    while (olineln != NULL) {
      if (olineln->deletable && (hypot(olineln->x - olineln->prev->x, 
        olineln->y - olineln->prev->y) < THSCRAPISWALLSMP)) {
        olineln->prev->next = olineln->next;
        if (olineln->next != NULL) {
          olineln->next->prev = olineln->prev;
        }
      }
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }

//  print_outline("BEFORE REORIENT");
#define orient_number(v1,v2,v3) ((v2->x - v1->x)*(v3->y - v1->y) \
    - (v3->x - v1->x)*(v2->y - v1->y))    
  
  // preorientuje
  bool reorient = false, tmpvis, tmptmpvis;
  double orientnum, tmpx, tmpy, tmpz;
  long nnodes, inode;
  
  oline = this->firstolseg;
  while (oline != NULL) {

    lastolln = oline;
    rlnode = oline;
    nnodes = 1;
    olineln = oline->next;
    while (olineln != NULL) {
      if ((olineln->y < rlnode->y) || 
          ((olineln->y == rlnode->y) && (olineln->x > rlnode->x))) {
        rlnode = olineln;
      }
      lastolln = olineln;
      nnodes++;
      olineln = olineln->next;
    }
    
    // usporiadame vrcholy - prevolln, rlnode, olineln
    if (rlnode->prev != NULL)
      prevolln = rlnode->prev;
    else
      prevolln = lastolln;
    if (rlnode->next != NULL)
      olineln = rlnode->next;
    else
      olineln = oline;

    // zistime ci je orientacia OK  
    if ((prevolln != NULL) && (olineln != NULL)) {
      orientnum = orient_number(prevolln, rlnode, olineln);
//      orientnum = (rlnode->x - prevolln->x) * (olineln->y - prevolln->y) -
//        (olineln->x - prevolln->x) * (rlnode->y - prevolln->y);
      reorient = (oline->outer != (orientnum > 0)); // outer == anti-clockwise
    }
    
    if (reorient) {
      // finta FEE - pointre nechame tak a vymenime data
      olineln = oline;
      prevolln = lastolln;
      nnodes = nnodes/2;
      inode = 0;
      while ((olineln != NULL) && (inode < nnodes)) {
        inode++;
        tmpx = olineln->x; tmpy = olineln->y; tmpz = olineln->z;
        tmpvis = olineln->visible;
        olineln->x = prevolln->x; olineln->y = prevolln->y; olineln->z = prevolln->z;
        olineln->visible = prevolln->visible;
        prevolln->x = tmpx; prevolln->y = tmpy; prevolln->z = tmpz; 
        prevolln->visible = tmpvis; 
        olineln = olineln->next;
        prevolln = prevolln->prev;
      }
      
      tmpvis = lastolln->visible;
      olineln = oline;
      while (olineln != NULL) {
        tmptmpvis = tmpvis;
        tmpvis = olineln->visible;
        olineln->visible = tmptmpvis;
        olineln = olineln->next;
      }
      
    }
    
    oline = oline->next_segment;    
  }
  
//  print_outline("AFTER ALL");
  
    
  // interpoluje a ponastavuje nnext a pprev
  long numpts = 0, numsegs = 0, cnumpts, cmaxnumpts = 0;
  // long totalnumpts;
  double dum;
  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline;
    numsegs++;
    cnumpts = 0;
    while (olineln != NULL) {
      numpts++;
      cnumpts++;
      this->bp_interpolate(olineln->x, olineln->y, olineln->z, dum, dum, dum);
      olineln->zu = olineln->z + 1.618 * 1.618;
      olineln->zd = olineln->z - 1.618;
      if (olineln->next == NULL) {
        olineln->nnext = oline;
        oline->pprev = olineln;
      } else {
        olineln->next->pprev = olineln;
        olineln->nnext = olineln->next;
      }
      olineln = olineln->next;
    }
    if (oline->outer && (cnumpts > cmaxnumpts)) {
      this->maxol = oline;
      cmaxnumpts = cnumpts;
    }
    oline = oline->next_segment;
  }


  // TRIANGULACIA PO NOVOM
  if (numpts > 2) {

    thcdtpoly.clear();
    this->tri_opts = new thscrapisolpt* [numpts];
    oline = this->firstolseg;
    numpts = 0;
    while (oline != NULL) {    
      olineln = oline;
      while (olineln != NULL) {
        this->tri_opts[numpts] = olineln;
        thcdtpoly.insert_vertex(olineln->x, olineln->y);
        olineln = olineln->next;
        numpts++;
      }
      thcdtpoly.end_contour();
      oline = oline->next_segment;
    }
    
    thcdtpoly.triangulate();
    
    // set triangulation output
    this->tri_num = (int)thcdtpoly.triangles.size();
    std::set<thcdttri>::iterator trii;
    long trix, newnum;
    thscrapisolpt tmpop, * ptmpop = &(tmpop);
    bool o1, o2, o3;
    if (this->tri_num > 0) {
      this->tri_triangles = new int [this->tri_num][3];
      for (trix = 0, newnum = 0, trii = thcdtpoly.triangles.begin();
          trix < this->tri_num; trii++, trix++) {
        // zisti ci je orientovany OK  
        o1 = (orient_number(this->tri_opts[trii->n1],this->tri_opts[trii->n2],this->tri_opts[trii->n3]) > 0.0);
        if (o1) {
          this->tri_triangles[newnum][0] = trii->n1;
          this->tri_triangles[newnum][1] = trii->n2;
          this->tri_triangles[newnum][2] = trii->n3;
        } else {
          this->tri_triangles[newnum][0] = trii->n1;
          this->tri_triangles[newnum][1] = trii->n3;
          this->tri_triangles[newnum][2] = trii->n2;
        }
        
        // ak je vnutri, tak zvysi 
        tmpop.x = this->tri_opts[trii->n2]->x + this->tri_opts[trii->n3]->x - this->tri_opts[trii->n1]->x;
        tmpop.y = this->tri_opts[trii->n2]->y + this->tri_opts[trii->n3]->y - this->tri_opts[trii->n1]->y; 
        o1 = (orient_number(this->tri_opts[trii->n1]->pprev, this->tri_opts[trii->n1], this->tri_opts[trii->n1]->nnext) > 0.0);
        o2 = (orient_number(this->tri_opts[trii->n1]->pprev, this->tri_opts[trii->n1], ptmpop) > 0.0);
        o3 = (orient_number(ptmpop, this->tri_opts[trii->n1], this->tri_opts[trii->n1]->nnext) > 0.0);
        if ((o1 && o2 && o3) || ((!o1) && (o2 || o3)))
          newnum++;
      }
      this->tri_num = newnum;
    } else {
      this->tri_triangles = NULL;
    }
  
  }

}


void thscrapis::insert_dim(double x, double y, double up, double down) {
  thscrapisdim * cd = &(*(this->dim_list.insert(this->dim_list.end(),thscrapisdim())));
  this->bp_interpolate(x, y, cd->z, cd->dd, cd->dx, cd->dy);
  cd->x = x;
  cd->y = y;
  cd->up = up;
  cd->down = down;
  cd->zup = cd->z + up;
  cd->zdown = cd->z - down;
  if (this->firstdim == NULL) {
    this->firstdim = cd;
  } else {
    cd->next = this->firstdim;
    this->firstdim = cd;
  }
}
  
void thscrapis::insert_bp_dim() {
  // prejde vsetky existujuce,
  // ak je minimum vzdialenosti > THSCRAPISMAXDIMD
  // prida ho do listu
  // prejde list este raz a nastavi nexty
  thscrapisbp * bp = this->firstbbp;
  thscrapisdim * cd;
  double mindist, cdist;
  bool toinsert;
  while (bp != NULL) {
    cd = this->firstdim;
    toinsert = true;
    if (cd != NULL) {
      mindist = hypot(cd->x - bp->x, cd->y - bp->y);
      toinsert = false;
      cd = cd->next;
      while (cd != NULL) {
        cdist = hypot(cd->x - bp->x, cd->y - bp->y);
        if (cdist < mindist)
          mindist = cdist;
        cd = cd->next;
      }
      if (mindist > THSCRAPISMAXDIMD)
        toinsert = true;
    }
    if (toinsert) {
      cd = &(*(this->dim_list.insert(this->dim_list.end(),thscrapisdim())));
      cd->x = bp->x;
      cd->y = bp->y;
      cd->z = bp->z;
      cd->dd = bp->sumd;
      cd->dx = bp->dx;
      cd->dy = bp->dy;
      cd->up = bp->up;
      cd->down = bp->down;
      cd->zup = cd->z + cd->up;
      cd->zdown = cd->z - cd->down;
    }
    bp = bp->next;
  } 
  
  std::list <thscrapisdim> :: iterator it = this->dim_list.begin();
  if (it != this->dim_list.end()) {
    this->firstdim = &(*it);
    this->firstdim->next = NULL;
    it++;
    while (it != this->dim_list.end()) {
      cd = &(*it);
      cd->next = this->firstdim;
      this->firstdim = cd;
      it++;
    }
  }
  
}
  
void thscrapis::outline_interpolate_dims()
{
  // poojde bod za bodom a spravi interpolaciu
  thscrapisolpt * oline, * olineln;
  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline;
    while (olineln != NULL) {
      this->dim_interpolate(olineln->x, olineln->y, olineln->z, olineln->zu, olineln->zd);
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }
  
}

void thscrapis::dim_interpolate(double x, double y, double z, double & zu, double & zd)
{
  thscrapisdim * cd = this->firstdim;
  if (cd == NULL)
    return;

  // prejde vsetky body a spocita interpolovane up, down, zup, zdown
  double sumw = 0.0, up = 0.0, down = 0.0, zup = 0.0, zdown = 0.0,
    dx, dy, cw;
  while (cd != NULL) {
    dx = x - cd->x;
    dy = y - cd->y;
    cw = hypot(dx, dy);
    if (cw < THSCRAPISRES) {
      zup = cd->zup;
      zdown = cd->zdown;
      up = cd->up;
      down = cd->down;
      sumw = 1.0;
      break;
    }
    cw = 1.0 / (cw * cw); //pow(cw,-2.0); // * exp(-2.0 * pow(dx * cd->dx + dy * cd->dy, 2.0));
    zup += cw * cd->zup;
    zdown += cw * cd->zdown;
    up += cw * cd->up;
    down += cw * cd->down;
    sumw += cw;
    cd = cd->next;
  }
  
  if (sumw == 0.0) {
    zu = z + 1.0;
    zd = z - 1.0;
    return;
  }
    
  if (sumw > 0) {
    zup /= sumw;
    zdown /= sumw;
    up /= sumw;
    down /= sumw;
  }
  
  if (zup < z + up) 
    zu = z + up;
  else 
    zu = zup;

  if (zdown > z - down) 
    zd = z - down;
  else 
    zd = zdown;
  
}


thscrapis1ip * thscrapis::i1insertpt (double x, double y, double z)
{
  thscrapis1ip * result = &(this->i1pt_map[thscrapisloc(x, y)]);
  if (result->newone) {
    result->x = x;
    result->y = y;
    result->z = z;    
    result->newone = false;
    if (this->first1ip == NULL) {
      this->first1ip = result;
      this->last1ip = result;
    } else {
      this->last1ip->next = result;
      this->last1ip = result;
    }
  } else {
    if (result->z > z) {
      result->z = z;
    }
  }
  return result;
}


void thscrapis::i1insert(double x1, double y1, double z1, double x2, double y2, double z2)
{
  thscrapis1ip * fp = this->i1insertpt(x1, y1, z1),
     * tp = this->i1insertpt(x2, y2, z2);
  thscrapis1il * lg;
  double dir;
  if (fp != tp) {
    lg = &(*(this->i1_legs.insert(this->i1_legs.end(), thscrapis1il())));
    dir = atan2(y2 - y1, x2 - x1);
    lg->a1.sp = fp;
    lg->a1.tp = tp;
    lg->a1.dd = dir;
    lg->a2.sp = tp;
    lg->a2.tp = fp;
    lg->a2.dd = -dir;
    if (this->last1il == NULL) {
      this->first1il = lg;
      this->last1il = lg;
    } else {
      this->last1il->next = lg;
      this->last1il = lg;
    }

    // vlozi arrows do 1ip
    if (fp->lasta == NULL) {
      fp->firsta = &(lg->a1);
      fp->lasta = &(lg->a1);
    } else {
      fp->lasta->next = &(lg->a1);
      fp->lasta = fp->lasta->next;
    }
    if (tp->lasta == NULL) {
      tp->firsta = &(lg->a2);
      tp->lasta = &(lg->a2);
    } else {
      tp->lasta->next = &(lg->a2);
      tp->lasta = tp->lasta->next;
    }
  }  // end of insertion
}


void thscrapis::i1interp()
{

  thscrapis1ip * pt;
  thscrapis1ila * pa, * pa2;
  thscrapis1il * lg;
  double langl, rangl;

#define thscrais_i1inpterp_dir(td,sd) if ((td < 0.0) || (sd < td)) td = sd;
    
  // prejde kazdy bod a spocita lnd a rnd
  for(pt = this->first1ip; pt != NULL; pt = pt->next) {
    for(pa = pt->firsta; pa != NULL; pa = pa-> next) {
      for(pa2 = pa->next; pa2 != NULL; pa2 = pa2->next) {
        langl = pa2->dd - pa->dd;
        if (langl < 0.0)
          langl += 6.28318530718;
        langl /= 2.0;
        rangl = 3.14159265359 - langl;
        thscrais_i1inpterp_dir(pa->lnd,langl);
        thscrais_i1inpterp_dir(pa->rnd,rangl);
        thscrais_i1inpterp_dir(pa2->lnd,rangl);
        thscrais_i1inpterp_dir(pa2->rnd,langl);
      }
    }
  }
  
#define thscrais_i1inpterp_resetdir(td) if (td < 0.0) td = 1.57079632679;
#define thscrais_i1inpterp_setnorm(xx,yy,dd,dplus) \
  xx = cos((dd) + (dplus)); \
  yy = sin((dd) + (dplus));
  
  // potom prejde kazdy shot a spocita normalam x a y
  for(lg = this->first1il; lg != NULL; lg = lg->next) {
    thscrais_i1inpterp_resetdir(lg->a1.lnd);
    thscrais_i1inpterp_resetdir(lg->a1.rnd);
    thscrais_i1inpterp_resetdir(lg->a2.lnd);
    thscrais_i1inpterp_resetdir(lg->a2.rnd);
    thscrais_i1inpterp_setnorm(lg->a1.lnx, lg->a1.lny, lg->a1.dd, lg->a1.lnd);
    thscrais_i1inpterp_setnorm(lg->a1.rnx, lg->a1.rny, lg->a1.dd, -lg->a1.rnd);
    thscrais_i1inpterp_setnorm(lg->a2.lnx, lg->a2.lny, lg->a2.dd, lg->a1.lnd);
    thscrais_i1inpterp_setnorm(lg->a2.rnx, lg->a2.rny, lg->a2.dd, -lg->a1.rnd);
  }
  
  // interpoluje outline (ako presne, to si treba rozkreslit)
  
}

void thscrapis::i2insert(thscrapis2ip ipt)
{
  // vlozi i2 bod
}

void thscrapis::i2interp()
{
  // najde najblizsi i2 bod
  // ak je lokalny - vezme z a cau parky
  // ak je globalny - interpoluje (ako to je este zahada :)))
}



