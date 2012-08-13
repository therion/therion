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
#include "thscrap.h"
#include "thpoint.h"
#include "therion.h"
#include "thdb2dlp.h"
#include "thdb1d.h"
#include "thscraplo.h"
#include "thline.h"
#include "thconfig.h"
#include "thtrans.h"
#include "thscraplp.h"
#include "extern/poly2tri/poly2tri.h"
#include "thscrap.h"
#include "thsurvey.h"
#include <math.h>
#ifdef THMSVC
#define hypot _hypot
#endif


struct pt2d {
  double x, y;
  pt2d(double x_, double y_) : x(x_), y(y_) {}
};


bool operator < (const struct pt2d & p1, 
                 const struct pt2d & p2)
{
  if (p1.x < p2.x)
    return true;
  else if (p1.x == p2.x)
    return (p1.y < p2.y);
  return false;
}

typedef std::map<pt2d, thscrapisolpt *> pt2d2olptmap;

#define ONLY_INVISIBLE_LINE 1
#define THSCRAPISSMP thcfg.tmp3dSMP
#define THSCRAPISWALLSMP 0.1 //thcfg.tmp3dWALLSMP
#define THSCRAPISMAXDIMD thcfg.tmp3dMAXDIMD

thscrapis::thscrapis(class thscrap * scrap) {
#ifndef THSCRAPIS_NEW3D
  this->firstbp = NULL;  
  this->firstbbp = NULL;  
  this->firstdim = NULL;  
#endif
  this->m_scrap = scrap;
  this->firstolseg = NULL;
  this->lastolseg = NULL;
  this->tri_triangles = NULL;
  this->tri_num = 0;
}



thscrapis::~thscrapis() {
  if (this->tri_triangles != NULL)
    delete [] this->tri_triangles;
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


void simplify_outline(thscrapisolpt * start, thscrapisolpt * end)
{
  if (start == end) return;
  if (start->next == end) return;
  thscrapisolpt * pt, * maxpt;
  double dx, dy, dd, maxw;
  dx = end->x - start->x,
  dy = end->y - start->y;
  dd = sqrt(dx*dx + dy*dy);
  pt = start->next;
  maxpt = NULL;
  maxw = 0;
  // prejde vsetky body, najde ten s najvacsou vzdialenostou od start/end
  //printf("\n\n\nA %6.2f %6.2f B %6.2f %6.2f\n", start->x, start->y, end->x, end->y);
  while (pt != end) {
    if (dd > 0.0) {
      pt->w = fabs(dx * (start->y - pt->y) - (start->x - pt->x) * dy)/dd;
    } else {
      pt->w = sqrt((pt->x - start->x) * (pt->x - start->x) + (pt->y - start->y) * (pt->y - start->y));
    }
    //printf("%.2f ->X %6.2f %6.2f\n", pt->w, pt->x, pt->y);
    if (pt->w >= maxw) {
      maxw = pt->w;
      maxpt = pt;
    }
    pt = pt->next;
  }
  if (maxw > THSCRAPISWALLSMP) {
    // ak vzdialenost > limit -> spusti este raz od zaciatku po ten bod a od toho po koniec
    simplify_outline(start, maxpt);
    simplify_outline(maxpt, end);
  } else {
    // ak mensia ako limit -> vsetky body medzi start/end vymaze
    start->next = end;
    end->prev = start;
  }
}


#ifdef THSCRAPIS_NEW3D

struct int3df {

  bool m_single, m_dim, m_pos, m_dir, m_setdim;
  thvec2 m_pf, m_pt;
  thline2 m_lf, m_lt, m_ln;
  double m_lnl;
  lxVec m_posf, m_post, m_direction;
  double m_upf, m_upt, m_dnf, m_dnt;

  int3df() : m_single(true), m_dim(false), m_pos(false), m_dir(false), m_setdim(false) {}
  
  void init_ln(thvec2 f, thvec2 t);
  void init_pt(thvec2 p);
  void interp(thvec2 src, double & w, double & p, thvec2 & pos);

};


void int3df::init_ln(thvec2 f, thvec2 t)
{
  m_single = false;
  m_pf = f;
  m_pt = t;
  m_ln = thline2(f, t);
  m_lf = thline2(f, f + thvec2(-m_ln.m_a, -m_ln.m_b));
  m_lt = thline2(t, t + thvec2(m_ln.m_a, m_ln.m_b));
  m_lnl = (m_pt - m_pf).length();
}

void int3df::init_pt(thvec2 p)
{
  m_single = true;
  m_pf = p;
}

void int3df::interp(thvec2 src, double & w, double & p, thvec2 & pos)
{
  if (m_single) {
    p = 0.0;
    pos = m_pf;
    w = (src - m_pf).length2();
  } else {
    double lnfd, lntd;
    lnfd = m_lf.eval(src);
    lntd = m_lt.eval(src);
    if (lnfd < 0) {
      w = (src - m_pf).length2();
      p = 0.0;
      pos = m_pf;
    } else if (lntd < 0) {
      w = (src - m_pt).length2();
      p = 1.0;
      pos = m_pt;
    } else {
      w = m_ln.eval(src);
      w *= w;    
      pos = thvec2(src.m_x - (m_ln.m_a * src.m_x + m_ln.m_b * src.m_y + m_ln.m_c) / (m_ln.m_a * m_ln.m_a + m_ln.m_b * m_ln.m_b) * m_ln.m_a, src.m_y - (m_ln.m_a * src.m_x + m_ln.m_b * src.m_y + m_ln.m_c)/(m_ln.m_a * m_ln.m_a + m_ln.m_b * m_ln.m_b) * m_ln.m_b);
      p = (pos - m_pf).length() / m_lnl;
    }
  }
}

typedef std::list<int3df> int3dl;




bool operator < (const struct thvec2 & l1, const struct thvec2 & l2)
{
  if (l1.m_x < l2.m_x)
    return true;
  if (l1.m_x > l2.m_x)
    return false;
  if (l1.m_y < l2.m_y)
    return true;
  return false;
}

bool operator > (const struct thvec2 & l1, const struct thvec2 & l2)
{
  return (l2 < l1);
}

bool operator == (const struct thvec2 & l1, const struct thvec2 & l2)
{
  return ((l1.m_x == l2.m_x) && (l1.m_y == l2.m_y));
}

struct int3dshot {
  thvec2 m_f, m_t;
  int3dshot(thvec2 f, thvec2 t) : m_f(f), m_t(t) {}
};

bool operator < (const struct int3dshot & f1, const struct int3dshot & f2)
{
  if (f1.m_f < f2.m_f)
    return true;
  if (f1.m_f > f2.m_f)
    return false;
  if (f1.m_t < f2.m_t)
    return true;
  return false;
}

typedef std::map<thvec2, int3dl::iterator> int3dpoint_map;
typedef std::map<int3dshot, int3dl::iterator> int3dshot_map;

struct int3di {

  thscrap * m_scrap;

  int3dpoint_map m_map_points;
  int3dshot_map m_map_shots;

  int3dl m_features;

  int3di(thscrap * scrap) {m_scrap = scrap;}

  void post_process();

  void insert_slp(thscraplp * slp);

  void insert_point(thvec2 p2, long id);

  void insert_dim(thvec2 p2, double up, double dn);

  void interpolate(thscrapisolpt * oline);

};

void setudlr(double asl, double & udlr, int index) {
  if (!thisnan(udlr)) return;
  if (!thisnan(asl)) {
    udlr = asl * 0.38;
    switch (index) {
      case 0:
        udlr *= 0.75;
        break;
      case 1:
        udlr *= 0.25;
        if (udlr > 1.618) udlr = 1.618;
        break;
      default:
        udlr *= .5;
    }
  } else {
    switch (index) {
      case 0:
        udlr = 1.618;
        break;
      case 1:
        udlr = 0.618;
        break;
      default:
        udlr = (1.618 + 0.618) / 2.0;
    }
  }
}

void int3di::insert_slp(thscraplp *slp) {

  thvec2 p1, p2;
  int3df f;
  int3dpoint_map::iterator pmi;
  int3dl::iterator fi;

  // ak pridavame bod
  if (!slp->lnio) {

    // skontroluje ci dany bod mame, ak ano, tak odideme
    p1 = thvec2(slp->stx, slp->sty);
    if (m_map_points.find(p1) != m_map_points.end())
      return;

    // ak nemame, tak pridame
    f.init_pt(p1);
    f.m_dim = false;
    f.m_pos = true;
    f.m_dir = false;
    f.m_posf = lxVec(slp->station->x, slp->station->y, slp->station->z);
    m_map_points[p1] = m_features.insert(m_features.end(), f);

  } else {

    p1 = thvec2(slp->lnx1, slp->lny1);
    p2 = thvec2(slp->lnx2, slp->lny2);
    lxVec pos1, pos2;
    thdb1ds * s1, * s2, * stmp;
    double udlr1[4], udlr2[4], udlrt;
    int i;
    s1 = &(thdb.db1d.station_vec[slp->arrow->leg->leg->from.id - 1]);
    s1 = &(thdb.db1d.station_vec[s1->uid - 1]);
    s2 = &(thdb.db1d.station_vec[slp->arrow->leg->leg->to.id - 1]);
    s2 = &(thdb.db1d.station_vec[s2->uid - 1]);
    udlr1[0] = slp->arrow->leg->leg->from_up;
    udlr1[1] = slp->arrow->leg->leg->from_down;
    udlr1[2] = slp->arrow->leg->leg->from_left;
    udlr1[3] = slp->arrow->leg->leg->from_right;
    udlr2[0] = slp->arrow->leg->leg->to_up;
    udlr2[1] = slp->arrow->leg->leg->to_down;
    udlr2[2] = slp->arrow->leg->leg->to_left;
    udlr2[3] = slp->arrow->leg->leg->to_right;
    if (slp->arrow->is_reversed) {
      stmp = s1; s1 = s2; s2 = stmp;
      for(i = 0; i < 4; i++) {
        udlrt = udlr1[i]; udlr1[i] = udlr2[i]; udlr2[i] = udlrt;
      }
    }
    pos1 = lxVec(s1->x, s1->y, s1->z);
    pos2 = lxVec(s2->x, s2->y, s2->z);


    // skontroluje, ci dany shot mame, ak ano, tak odideme
    if (m_map_shots.find(int3dshot(p1,p2)) != m_map_shots.end())
      return;

    // ak mame nejake body tam, kde je shot, tak ich odstranime
    pmi = m_map_points.find(p1);
    if (pmi != m_map_points.end()) {
      if (pmi->second->m_single) {
        m_features.erase(pmi->second);
        m_map_points.erase(pmi);
        pmi = m_map_points.end();
      }
    }
    pmi = m_map_points.find(p2);
    if (pmi != m_map_points.end()) {
      if (pmi->second->m_single) {
        m_features.erase(pmi->second);
        m_map_points.erase(pmi);
        pmi = m_map_points.end();
      }
    }

    if (p1 == p2) {
      if (pmi == m_map_points.end()) {
        // vlozime ako bod
        f.init_pt(p1);
        f.m_dim = false;
        f.m_pos = true;
        f.m_dir = false;
        f.m_posf = 0.5 * pos1 + 0.5 * pos2;
        m_map_points[p1] = m_features.insert(m_features.end(), f);
      } 
    } else {
      // ak nemame, vytvorime shot
      f.init_ln(p1, p2);
      f.m_pos = true;
      f.m_posf = pos1;
      f.m_post = pos2;

      // dimensions podla lrud
      double coef, pa;
      coef = 1.0;
      f.m_dim = true;
      switch (this->m_scrap->proj->type) {
        case TT_2DPROJ_PLAN:
          for(i = 0; i < 2; i++) {
            f.m_dim = f.m_dim && ((!thisnan(udlr1[i])) && (!thisnan(udlr2[i])));
            setudlr(s1->asl, udlr1[i], i);
            setudlr(s2->asl, udlr2[i], i);
          }
          if (fabs(slp->arrow->leg->leg->total_gradient) > slp->arrow->leg->leg->vtresh) {
            coef = fabs(slp->arrow->leg->leg->total_gradient);
            if (coef > 60) coef = 60;
            coef = 1 / cos(coef / 180.0 * THPI);
          }
          f.m_upf = coef * udlr1[0];
          f.m_dnf = coef * udlr1[1];
          f.m_upt = coef * udlr2[0];
          f.m_dnt = coef * udlr2[1];
          break;
        case TT_2DPROJ_ELEV:
          coef = slp->arrow->leg->leg->total_bearing;
          if (coef < 0.0) coef += 180.0;
          if (coef > 180.0) coef -= 180.0;
          pa = this->m_scrap->proj->pp1;
          if (pa < 0.0) pa += 180.0;
          if (pa > 180.0) pa -= 180.0;
          coef = fabs(coef - pa);
          if (coef > 90.0) coef = 90 - (coef - 90);
          coef = 90 - coef;
          if (coef > 60) coef = 60;
          coef = 1 / cos(coef / 180.0 * THPI);
        case TT_2DPROJ_EXTEND:
          for(i = 2; i < 4; i++) {
            f.m_dim = f.m_dim && ((!thisnan(udlr1[i])) && (!thisnan(udlr2[i])));
            setudlr(s1->asl, udlr1[i], i);
            setudlr(s2->asl, udlr2[i], i);
          }
          if (p2.m_x > p1.m_x) {
            f.m_upf = coef * udlr1[3];
            f.m_dnf = coef * udlr1[2];
            f.m_upt = coef * udlr2[3];
            f.m_dnt = coef * udlr2[2];
          } else {
            f.m_upf = coef * udlr1[2];
            f.m_dnf = coef * udlr1[3];
            f.m_upt = coef * udlr2[2];
            f.m_dnt = coef * udlr2[3];
          }
          break;
      }

      // direction podla projekcie
      if ((this->m_scrap->proj->type == TT_2DPROJ_EXTEND) && (p1.m_x != p2.m_x)) {
        f.m_dir = true;
        f.m_direction = lxVec(pos1.y - pos2.y, pos2.x - pos1.x, 0.0);
        f.m_direction.Normalize();
        if (p1.m_x > p2.m_x) f.m_direction = -1.0 * f.m_direction;
      } else {
        f.m_dir = false;
      }
      fi = m_features.insert(m_features.end(), f);
      m_map_points[p1] = fi;
      m_map_points[p2] = fi;
      m_map_shots[int3dshot(p1,p2)] = fi;
      m_map_shots[int3dshot(p2,p1)] = fi;
    }

  }
}


void int3di::insert_point(thvec2 p2, long id) {

  // skontroluje ci dany bod mame, ak ano, tak odideme
  if (m_map_points.find(p2) != m_map_points.end())
    return;

  // ak nemame, tak pridame
  int3df f;
  thdb1ds * s = &(thdb.db1d.station_vec[id - 1]);
  f.init_pt(p2);
  f.m_dim = false;
  f.m_pos = true;
  f.m_dir = false;
  f.m_posf = lxVec(s->x, s->y, s->z);
  m_map_points[p2] = m_features.insert(m_features.end(), f);

}

void int3di::insert_dim(thvec2 p2, double up, double dn) {

  int3dpoint_map::iterator mi;
  mi = m_map_points.find(p2);

  // skontroluje ci dany bod mame, ak ano, tak prepiseme ud
  if (mi != m_map_points.end()) {
    mi->second->m_dim = true;
    if (mi->second->m_single || (mi->second->m_pf == p2)) {
      mi->second->m_upf = up;
      mi->second->m_dnf = dn;
    } else {
      mi->second->m_upt = up;
      mi->second->m_dnt = dn;
    }
    return;
  }

  // ak nemame, tak pridame
  int3df f;
  f.init_pt(p2);
  f.m_dim = true;
  f.m_pos = false;
  f.m_dir = false;
  f.m_dnf = dn;
  f.m_upf = up;
  m_map_points[p2] = m_features.insert(m_features.end(), f);

}

void int3di::post_process() {
  int3dl::iterator i, ii;
  // if no dimension interpolator - use asl
  lxVecLimits vl;
  for(i = this->m_features.begin(); i != this->m_features.end(); i++) {
    i->m_setdim = false;
    vl.Add(i->m_pf.m_x, i->m_pf.m_y, 0.0);
    if (!i->m_single)
      vl.Add(i->m_pt.m_x, i->m_pt.m_y, 0.0);
  }
  if (vl.valid) {
    double diam = (vl.max - vl.min).Length() * 0.25;
    for(i = this->m_features.begin(); i != this->m_features.end(); i++) {
      if ((!i->m_dim) && (!i->m_single)) {
        bool nsetdim = true;
        for(ii = this->m_features.begin(); ii != this->m_features.end(); ii++) {
          if (ii->m_dim) {
            thvec2 dp = ii->m_pf, tmpv;
            double w, p;
            if (!ii->m_single) dp = 0.5 * dp + 0.5 * ii->m_pt;
            i->interp(dp, w, p, tmpv);
            w = sqrt(w);
            if (w < diam) 
              nsetdim = false;
          }
        }
        i->m_setdim = nsetdim;
      }
    }
    for(i = this->m_features.begin(); i != this->m_features.end(); i++) {
      if (i->m_setdim) i->m_dim = true;
    }
  }
}


void thscrapis::int3d() {


  // inicializuje 3d interpolovacie struktury
  thscraplp * slp;
  th2ddataobject * o2;
  thpoint * pp;
  int3di i3(this->m_scrap);
  bool xelev = (this->m_scrap->proj->type == TT_2DPROJ_EXTEND);

  // vlozi vsetky meracske body zo scrapu
  o2 = this->m_scrap->fs2doptr;
  while (o2 != NULL) {
    if (o2->get_class_id() == TT_POINT_CMD) {
      pp = (thpoint *) o2;
      if (pp->station_name.id != 0) {
        i3.insert_point(thvec2(pp->point->xt, pp->point->yt), pp->station_name.id);
      }
    }
    o2 = o2->nscrapoptr;
  }

  // vlozi vsetok polygon zo scrapu (ciary vklada len tie, co maju oba body v scrape!)
  slp = this->m_scrap->get_polygon();
  while (slp != NULL) {
    i3.insert_slp(slp);
    slp = slp->next_item;
  }
  
  // prejde vsetky objekty a nasackuje passage-heights
  o2 = this->m_scrap->fs2doptr;
  double cup, cdown;
  while (o2 != NULL) {
    if (o2->get_class_id() == TT_POINT_CMD) {
      pp = (thpoint *) o2;
      if ((pp->type == TT_POINT_TYPE_PASSAGE_HEIGHT) &&
          ((pp->tags & TT_POINT_TAG_HEIGHT_U) != 0) &&
          (!thisnan(pp->xsize))) {
        cdown = pp->xsize / 1.618;
        if (cdown > 1.618)
          cdown = 1.618;
        cup = pp->xsize - cdown;
        i3.insert_dim(thvec2(pp->point->xt, pp->point->yt), cup, cdown);
      } else if ((pp->type == TT_POINT_TYPE_DIMENSIONS) &&
        (!thisnan(pp->xsize)) && (!thisnan(pp->ysize))) {
        i3.insert_dim(thvec2(pp->point->xt, pp->point->yt), pp->xsize, pp->ysize);
      }
    }
    o2 = o2->nscrapoptr;
  }

  i3.post_process();

  // interpoluje
  lxVec ddir, udir, rdir;
  switch (this->m_scrap->proj->type) {
    case TT_2DPROJ_PLAN:
      ddir = lxVec(0.0, 0.0, 1.0);
      udir = lxVec(0.0, 1.0, 0.0);
      rdir = lxVec(1.0, 0.0, 0.0);
      break;
    case TT_2DPROJ_ELEV:
      ddir = lxVec(-sin(this->m_scrap->proj->pp1 / 180.0 * THPI), -cos(this->m_scrap->proj->pp1 / 180.0 * THPI), 0.0);
      udir = lxVec(0.0, 0.0, 1.0);
      rdir = lxVec(-ddir.y, ddir.x, 0.0);
      break;
    case TT_2DPROJ_EXTEND:
      ddir = lxVec(0.0, -1.0, 0.0);
      udir = lxVec(0.0, 0.0, 1.0);
      rdir = lxVec(-ddir.y, ddir.x, 0.0);
      break;
  }
  thscrapisolpt * oline, * olineln;
  int3dl::iterator ii, iii;
  lxVec sumv, pos, sumupv, sumdnv;
  thvec2 v2, pt;
  double sumw, sumdw, w, p, cw, sup, sdn;


  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline;
    while (olineln != NULL) {
      pt = thvec2(olineln->x, olineln->y);
      
      // interpolate direction if neeeded
      if (xelev) {
        ii = i3.m_features.begin();
        sumw = 0.0;
        sumv = lxVec(0.0, 0.0, 0.0);
        while (ii != i3.m_features.end()) {
          if (ii->m_dir) {
            ii->interp(pt, w, p, v2);
            if (w < 1e-6) w = 1e-6;
            cw = 1 / (w * w);
            sumv += cw * ii->m_direction;
            sumw += cw;
          }
          ii++;
        }
        if (sumv.Norm() > 0.0) {
          sumv.Normalize();
          olineln->dir = sumv;
        } else {
          olineln->dir = ddir;
        }
      } else {
        olineln->dir = ddir;
      }


      // interpolujeme position & direction pre dimension points
      iii = i3.m_features.begin();
      while (iii != i3.m_features.end()) {
        if (!iii->m_dir)
          iii->m_direction = ddir;
        if ((!iii->m_pos) && (iii->m_dim)) {
          sumw = 0.0;
          ii = i3.m_features.begin();
          sumv = lxVec(0.0, 0.0, 0.0);
          sumdw = 0.0;
          sumupv = lxVec(0.0, 0.0, 0.0);
          while (ii != i3.m_features.end()) {
            if (ii->m_pos) {
              ii->interp(iii->m_pf, w, p, v2);
              if (w < 1e-6) w = 1e-6;
              cw = 1.0 / (w * w);
              if (ii->m_pos) {
                sumw += cw;
                pos = ((1.0 - p) * ii->m_posf + p * ii->m_post);
                if (xelev) rdir = lxVec(-olineln->dir.y, olineln->dir.x, 0.0);
                v2 = pt - v2;
                pos = pos + v2.m_x * rdir + v2.m_y * udir;
                sumv += cw * pos;
              }
              if (ii->m_dir) {
                sumupv += cw * ii->m_direction;
                sumdw += cw;
              }
            }
            ii++;
          }
          iii->m_posf = (1 / sumw) * sumv;
          if (!iii->m_dir) {
            if (sumupv.Norm() > 0.0) {
              iii->m_direction = (1 / sumdw) * sumupv;
            } else {
              iii->m_direction = ddir;
            }
          }
        }
        iii++;
      }


      // interpolujeme position & dimensions
      sumdw = 0.0;
      sumw = 0.0;
      ii = i3.m_features.begin();
      sumv = lxVec(0.0, 0.0, 0.0);
      sumupv = sumv;
      sumdnv = sumv;
      sup = 0.0;
      sdn = 0.0;
      while (ii != i3.m_features.end()) {
        if (ii->m_pos || ii->m_dim) {
          if (this->m_scrap->proj->type == TT_2DPROJ_EXTEND)
            rdir = lxVec(-olineln->dir.y, olineln->dir.x, 0.0);
          ii->interp(pt, w, p, v2);
          if (w < 1e-6) w = 1e-6;
          cw = 1.0 / (w * w);
          if (ii->m_dim) {
            sumdw += cw;
            // interpolate up dimension
            sup += cw * ((1.0 - p) * ii->m_upf + p * ii->m_upt);
            // interpolate dn dimension
            sdn += cw * ((1.0 - p) * ii->m_dnf + p * ii->m_dnt);
          }
          if (ii->m_pos) {
            sumw += cw;
            v2 = pt - v2;
            pos = (1.0 - p) * ii->m_posf + p * ii->m_post;
            pos = pos + v2.m_x * rdir + v2.m_y * udir;
            sumv += cw * pos;
          }
        }
        ii++;
      }
      sumv = (1 / sumw) * sumv;
      if (sumdw == 0.0) {
        sup = 1.618;
        sdn = 0.618;
      } else {
        sup /= sumdw;
        sdn /= sumdw;
      }

      olineln->pt = sumv;
      olineln->d_up = sup;
      olineln->d_dn = sdn;
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }

}
#endif

void thscrapis::outline_scan(class thscraplo * outln) {

  // nacita
  bool first, outer, vis;
  long from, to;
  thdb2dlp * lp;
  thscraplo * lo = outln, * lo2;

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
      if (olineln->next == NULL)
        olineln->deletable = false;
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }

  
  // zjednodusi
  thscrapisolpt * simplstart;
  oline = this->firstolseg;
  while (oline != NULL) {
    simplstart = oline;
    olineln = oline->next;
    while (olineln != NULL) {
      if (!olineln->deletable) {
        simplify_outline(simplstart, olineln);
        simplstart = olineln;
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
  double orientnum, tmpx, tmpy;
#ifndef THSCRAPIS_NEW3D
  double tmpz;
#endif
  long nnodes, inode;
  long numpts = 0;

  oline = this->firstolseg;
  while (oline != NULL) {

    lastolln = oline;
    rlnode = oline;
    nnodes = 1;
    olineln = oline->next;
    numpts++;
    while (olineln != NULL) {
      if ((olineln->y < rlnode->y) || 
        ((olineln->y == rlnode->y) && (olineln->x > rlnode->x))) {
          rlnode = olineln;
      }
      lastolln = olineln;
      nnodes++;
      numpts++;
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
        tmpx = olineln->x; tmpy = olineln->y;
#ifndef THSCRAPIS_NEW3D
        tmpz = olineln->z;
#endif
        tmpvis = olineln->visible;
        olineln->x = prevolln->x; olineln->y = prevolln->y; 
#ifndef THSCRAPIS_NEW3D
        olineln->z = prevolln->z;
#endif
        olineln->visible = prevolln->visible;
        prevolln->x = tmpx; prevolln->y = tmpy;
#ifndef THSCRAPIS_NEW3D
        prevolln->z = tmpz; 
#endif
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


  // TRIANGULACIA PO NOVOM
  if (numpts > 2) {
    p2t::CDT * cdt = NULL;
    std::vector<p2t::Point *> polyline;
    pt2d2olptmap ptmap;
    bool mult_outer = false;
    //FILE * f = fopen("data.dat","w");
    double xx, yy; bool duplpts = false;
    oline = this->firstolseg;
    while (oline != NULL) {    
      olineln = oline;
      while (olineln != NULL) {
        xx = olineln->x;
        yy = olineln->y;
        while (ptmap.find(pt2d(xx, yy)) != ptmap.end()) {
          xx = xx + 0.0001;
          duplpts = true;
        }
        ptmap[pt2d(xx, yy)] = olineln;
        //fprintf(f,"%.6f\t%.6f\n", xx, yy);
        polyline.push_back(new p2t::Point(xx, yy));
        olineln = olineln->next;
        numpts++;
      }
      if (cdt == NULL) {
        cdt = new p2t::CDT(polyline);
        polyline.clear();
      } else {
        if (oline->outer) {
          mult_outer = true;
          break;
        }
        cdt->AddHole(polyline);
        polyline.clear();
      }
      oline = oline->next_segment;
    }
    
	//fclose(f);
	std::vector<p2t::Triangle *> triangles;
	try {
		cdt->Triangulate();

		// set triangulation output
		triangles = cdt->GetTriangles();

		this->tri_num = (long)triangles.size();
	} catch (...) {
        const char * errscrap = this->m_scrap->name;
        const char * errdot = "";
        const char * errsurv = "";
        if (strlen(this->m_scrap->fsptr->full_name) > 0) {
          errdot = "@";
          errsurv = this->m_scrap->fsptr->full_name;
        }
        thwarning(("%s%s%s -- scrap 3D reconstruction failed", errscrap, errdot, errsurv));
        thtext_inline = false;
		this->tri_num = 0;
	}
    long trix, newnum;
    if (this->tri_num > 0) {
      this->tri_triangles = new pthscrapisolpt[this->tri_num][3];
      for (trix = 0, newnum = 0; trix < this->tri_num; trix++) {
        p2t::Triangle& t = *triangles[trix];
        p2t::Point& b = *t.GetPoint(1);
        p2t::Point& a = *t.PointCCW(b);
        p2t::Point& c = *t.PointCW(b);
        pt2d2olptmap::iterator i = ptmap.find(pt2d(a.x, a.y));
        if (i == ptmap.end())
          continue;
        else
          this->tri_triangles[newnum][0] = i->second;
        i = ptmap.find(pt2d(b.x, b.y));
        if (i == ptmap.end())
          continue;
        else
          this->tri_triangles[newnum][1] = i->second;
        i = ptmap.find(pt2d(c.x, c.y));
        if (i == ptmap.end())
          continue;
        else
          this->tri_triangles[newnum][2] = i->second;
        newnum++;
      }
      bool old_thtext_inline = thtext_inline;
      if (mult_outer || (this->tri_num > newnum) || duplpts) {
        const char * errscrap = this->m_scrap->name;
        const char * errdot = "";
        const char * errsurv = "";
        if (strlen(this->m_scrap->fsptr->full_name) > 0) {
          errdot = "@";
          errsurv = this->m_scrap->fsptr->full_name;
        }
        if (mult_outer) {
          thwarning(("%s%s%s -- multiple scrap outer outlines not supported yet", errscrap, errdot, errsurv));
          thtext_inline = false;
        }
        if ((this->tri_num > newnum) || duplpts) {
          thwarning(("%s%s%s -- invalid scrap outline", errscrap, errdot, errsurv));
          thtext_inline = false;
        }
      }
      thtext_inline = old_thtext_inline;
      this->tri_num = newnum;
    } else {
      this->tri_triangles = NULL;
    }

    delete cdt;
  }

#ifdef THSCRAPIS_NEW3D
  this->int3d();
#endif


#ifndef THSCRAPIS_NEW3D
  // interpoluje Z
  double dum;
  oline = this->firstolseg;
  while (oline != NULL) {
    olineln = oline;
    while (olineln != NULL) {
      this->bp_interpolate(olineln->x, olineln->y, olineln->z, dum, dum, dum);
      olineln->zu = olineln->z + 1.618 * 1.618;
      olineln->zd = olineln->z - 1.618;
      olineln = olineln->next;
    }
    oline = oline->next_segment;
  }
#endif

}



/*
Input:
  * shoty s LRUD
  * meracske body bez LRUD
  * point dimenstions s UD

Ako na to:
  * zoberieme outline - HOTOVO
  * zjednodusime - HOTOVO
  * triangulacia - HOTOVO
  * potrebujeme interpolovat -> 
    x/y/z outlajnu
    direction vector
    up/down
  * mame fixne:
    PLAN - x/y + direction fixed
    ELEV - z + direction fixed
    XELEV - z
  * mame v shotoch:
    PLAN - z + up/down (treba spocitat)
    ELEV - x/y + up/down (treba spocitat)
    XELEV - x/y + direction (treba spocitat) + up/down (treba spocitat)
  * pre kazdy bod outlajnu mame ciary, ktore z neho idu
    interpolacia prebieha nasledovne:
    1. pripravime si zoznam features (shoty, body, dimensions)
    2. spocitame co treba pre features spocitat (up/down/direction)
    3. v1:
      + pre kazdy bod spocitame 1/d^4 k features a vyvazime, separatne up/down a separatne x/y/z/dir
      + v2 - selektovat features sa budu iba podla triangulacie a zvysok sa interpoluje po outlajne
*/



















// DELETE FROM HERE ON
#ifndef THSCRAPIS_NEW3D

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
  this->firstbp = NULL;
  if (it == this->bp_map.end())
    return;

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

#endif
