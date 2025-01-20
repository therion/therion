/**
 * @file thexpsys.cxx
 */
  
/* Copyright (C) 2007 Stacho Mudrak
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


#include "thwarp.h"
#include "thdb1d.h"
#include "thscrap.h"
#include "thtrans.h"
#include "th2ddataobject.h"
#include "thpoint.h"
#include "thconfig.h"
#include "thdatabase.h"
#include "therion.h"
#include <cstring>

thwarp::~thwarp() {}
   
thpic * thwarp::morph(thsketch * sketch, double /*scale*/) { // TODO unused parameter scale
  this->m_sketch = sketch;
  return NULL;
}


/**
 * Sketch station structure.
 */

struct thwarplinst {
  double x, y, tx, ty;
  thdb1ds * st;
  thwarplinst() : x(0.0), y(0.0), tx(0.0), ty(0.0), st(NULL) {}
};

typedef std::list<thwarplinst> thwarplinst_list;


thpic * thwarplin::morph(thsketch * sketch, double /*scale*/) // TODO unused parameter scale
{
  if (this->morphed && this->mpic.exists())
    return &(this->mpic);

  if (!sketch->m_pic.exists())
    return NULL;

  thwarplinst_list stations;
  thwarplinst tmpst;
  thdb2dcp * ccp;
  ccp = sketch->m_scrap->fcpp;
  while (ccp != NULL) {
    if ((ccp->point != NULL) && (ccp->st != NULL)) {
      tmpst.x = ccp->pt->x - sketch->m_x;
      tmpst.y = ccp->pt->y - sketch->m_y;
      tmpst.tx = ccp->tx + sketch->m_scrap->proj->rshift_x;
      tmpst.ty = ccp->ty + sketch->m_scrap->proj->rshift_y;
      tmpst.st = ccp->st;
      stations.push_back(tmpst);
    }
    ccp = ccp->nextcp;
  }

  if (stations.size() < 2)
    return NULL;

  this->morphed = true;

  // if linear transformation, use only two stations with maximal
  // distance on output
  if ((this->method == 0) && (stations.size() > 2)) {
    thwarplinst_list::iterator sti1, sti2;
    thwarplinst tmpst2;
    tmpst2.st = NULL;
    double maxdist, cdist;
    maxdist = 0.0;
    for(sti1 = stations.begin(); sti1 != stations.end(); sti1++) {
      sti2 = sti1;
      sti2++;
      while (sti2 != stations.end()) {
        cdist = thvec2(sti1->tx - sti2->tx, sti1->ty - sti2->ty).length();
        if (cdist > maxdist) {
          tmpst = *sti1;
          tmpst2 = *sti2;
          maxdist = cdist;
        }
        sti2++;
      }
    }
    if (tmpst2.st != NULL) {
      stations.clear();
      stations.push_back(tmpst);
      stations.push_back(tmpst2);
    }
  }

  thlintrans T1, T2, T3, TLIN;
  thwarplinst_list::iterator ii;

  // Calculate transformations.
  // T1 - shift
  // T2 - rotate from TLIN
  // T3 - scale and shift from TLIN
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
  thprintf("morphing %s ", fn);
  thtext_inline = true;

  for (ii = stations.begin(); ii != stations.end(); ii++) {
    T1.m_shift -= thvec2(ii->x, ii->y);
  }
  T1.m_shift /= double(stations.size());
  T1.init_backward();

  for (ii = stations.begin(); ii != stations.end(); ii++) {
    TLIN.insert_point(T1.forward(thvec2(ii->x, ii->y)), thvec2(ii->tx, ii->ty));
  }
  TLIN.init_points();

  // create T2 and T3 from TLIN
  T2.m_fmat = TLIN.m_fmat / TLIN.m_scale;
  T2.init_backward();
  T3.m_fmat.reset();
  T3.m_fmat *= TLIN.m_scale;
  T3.m_scale = TLIN.m_scale;
  T3.m_shift = TLIN.m_shift;
  T3.init_backward();

  long mw, mh, x, y, yy,counter;
  thvec2 pmin, pmax, ptmp, mpic_origin;
  counter = 1;

  if (/*(stations.size() < 3) ||*/ this->method == 0) {

    pmin = pmax = T2.forward(T1.forward(thvec2(0.0, 0.0)));
    ptmp = T2.forward(T1.forward(thvec2(double(sketch->m_pic.width), 0.0))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    ptmp = T2.forward(T1.forward(thvec2(0.0, double(sketch->m_pic.height)))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    ptmp = T2.forward(T1.forward(thvec2(double(sketch->m_pic.width), double(sketch->m_pic.height)))); pmin.minimize(ptmp); pmax.maximize(ptmp);

    mpic_origin = thvec2(pmin.m_x, pmin.m_y);
    mw = long(pmax.m_x + 0.5) - long(pmin.m_x - 0.5);
    mh = long(pmax.m_y + 0.5) - long(pmin.m_y - 0.5);
    thprintf("(%.1f Mpix)", double(mw * mh) / 1000000.0);


    sketch->m_pic.rgba_load();
    this->mpic.rgba_init(mw, mh);
    for (x = 0; x < mw; x++) {
      for (y = 0; y < mh; y++) {
        ptmp = T1.backward(T2.backward(mpic_origin + thvec2(double(x), double(y))));
        this->mpic.rgba_set_pixel(x, y, sketch->m_pic.rgba_interpolate_pixel(ptmp.m_x, ptmp.m_y));
        if ((counter++ % 20000) == 0)
          thprintf(" [%.0f%%]", double(counter) / double(mw * mh) * 100.0);
      }
    }
  
  } else if (this->method == 1) {

    thmorphtrans TM;
    for (ii = stations.begin(); ii != stations.end(); ii++) {
      TM.insert_point(
        T2.forward(T1.forward(thvec2(ii->x, ii->y))), 
        T3.backward(thvec2(ii->tx, ii->ty)));
    }

    pmin = pmax = TM.forward(T2.forward(T1.forward(thvec2(0.0, 0.0))));
    for(x = 0; x <= double(sketch->m_pic.width); x++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), 0.0)))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), double(sketch->m_pic.height))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }
    for(y = 0; y <= double(sketch->m_pic.height); y++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(0.0, double(y))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(sketch->m_pic.width), double(y))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }

    thvec2 start;
    start = T2.forward(T1.forward(thvec2(0.0, 0.0)));
    mpic_origin = thvec2(pmin.m_x, pmin.m_y);
    mw = long(pmax.m_x + 0.5) - long(pmin.m_x - 0.5);
    mh = long(pmax.m_y + 0.5) - long(pmin.m_y - 0.5);
    thprintf("(%.1f Mpix)", double(mw * mh) / 1000000.0);
    sketch->m_pic.rgba_load();
    this->mpic.rgba_init(mw, mh);
    for (x = 0; x < mw; x++) {
      for (yy = 0; yy < mh; yy++) {
        y = yy - mh/2; if (y < 0) y += mh;
        start = TM.backward(mpic_origin + thvec2(double(x), double(y)), start);
        ptmp = T1.backward(T2.backward(start));
        this->mpic.rgba_set_pixel(x, y, sketch->m_pic.rgba_interpolate_pixel(ptmp.m_x, ptmp.m_y));
        if ((counter++ % 20000) == 0)
          thprintf(" [%.0f%%]", double(counter) / double(mw * mh) * 100.0);
      }
    }
  } else if (method == 2) {

    thmorph2trans TM;
    for (ii = stations.begin(); ii != stations.end(); ii++) {
      TM.insert_point(
        T2.forward(T1.forward(thvec2(ii->x, ii->y))), 
        T3.backward(thvec2(ii->tx, ii->ty)), ii->st->uid);
    }
    TM.insert_lines_from_db();
    TM.init();

    pmin = pmax = TM.forward(T2.forward(T1.forward(thvec2(0.0, 0.0))));
    for(x = 0; x <= double(sketch->m_pic.width); x++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), 0.0)))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), double(sketch->m_pic.height))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }
    for(y = 0; y <= double(sketch->m_pic.height); y++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(0.0, double(y))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(sketch->m_pic.width), double(y))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }

    thvec2 start;
    std::vector<thvec2> ss;
    mpic_origin = thvec2(pmin.m_x, pmin.m_y);
    mw = long(pmax.m_x + 0.5) - long(pmin.m_x - 0.5);
    mh = long(pmax.m_y + 0.5) - long(pmin.m_y - 0.5);
    if (mh > 0) ss.resize(mh);
    thprintf("(%.1f Mpix)", double(mw * mh) / 1000000.0);
    sketch->m_pic.rgba_load();
    start = T2.forward(T1.forward(thvec2(0.0, double(mh/2.0))));
    this->mpic.rgba_init(mw, mh);
    for (x = 0; x < mw; x++) {
      for (y = 0; y < mh; y++) {
        start = TM.backward(mpic_origin + thvec2(double(x), double(y)), x > 0 ? ss[y] : start);
        ss[y] = start;
        ptmp = T1.backward(T2.backward(start));
        this->mpic.rgba_set_pixel(x, y, sketch->m_pic.rgba_interpolate_pixel(ptmp.m_x, ptmp.m_y));
        if ((counter++ % 20000) == 0)
          thprintf(" [%.0f%%]", double(counter) / double(mw * mh) * 100.0);
      }
    }

  } else {

    thmorph2trans TM;
    for (ii = stations.begin(); ii != stations.end(); ii++) {
      TM.insert_point(
        T2.forward(T1.forward(thvec2(ii->x, ii->y))), 
        T3.backward(thvec2(ii->tx, ii->ty)), ii->st->uid);
    }
    TM.insert_lines_from_db();
    th2ddataobject * pobj;
    thpoint * pointp;
    pobj = sketch->m_scrap->fs2doptr;
    while (pobj != NULL) {
      if (pobj->get_class_id() == TT_POINT_CMD) {
        pointp = dynamic_cast<thpoint*>(pobj);
        if (pointp->type == TT_POINT_TYPE_EXTRA) {
          if ((T3.m_scale > 0.0) && (pointp->from_name.id > 0) && (!thisnan(pointp->xsize))) {
            TM.insert_zoom_point(
              T2.forward(T1.forward(thvec2(
              pointp->point->x - sketch->m_x,
              pointp->point->y - sketch->m_y))), 
              pointp->xsize / T3.m_scale, 
              thdb.db1d.station_vec[pointp->from_name.id - 1].uid);
          }
        }
      }
      pobj = pobj->nscrapoptr;
    }
    TM.init();

    pmin = pmax = TM.forward(T2.forward(T1.forward(thvec2(0.0, 0.0))));
    for(x = 0; x < sketch->m_pic.width; x++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), 0.0)))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), double(sketch->m_pic.height))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }
    for(y = 0; y < sketch->m_pic.height; y++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(0.0, double(y))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(sketch->m_pic.width), double(y))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }

    thvec2 start;
    std::vector<thvec2> ss;
    mpic_origin = thvec2(pmin.m_x, pmin.m_y);
    mw = long(pmax.m_x + 0.5) - long(pmin.m_x - 0.5);
    mh = long(pmax.m_y + 0.5) - long(pmin.m_y - 0.5);
    if (mh > 0) ss.resize(mh);
    thprintf("(%.1f Mpix)", double(mw * mh) / 1000000.0);
    sketch->m_pic.rgba_load();
    start = T2.forward(T1.forward(thvec2(0.0, double(mh/2.0))));
    this->mpic.rgba_init(mw, mh);
    long ox, oy, x1, y1, x2, y2;
    thvec2 newp, newpmin, newpmax;
    thline2 lAB, lBC, lCD, lDA, lCA;
    double dAB, dBC, dCD, dDA, dCA;
    std::vector<thvec2> pscan(sketch->m_pic.width);
    std::vector<thvec2> cscan(sketch->m_pic.width);
    for(oy = 0; oy < sketch->m_pic.height; oy++) {
      for(ox = 0; ox < sketch->m_pic.width; ox++) {
        cscan[ox] = TM.forward(T2.forward(T1.forward(thvec2(double(ox), double(oy))))) - mpic_origin;
        if ((ox > 0) && (oy > 0)) {

          newpmax = newpmin = pscan[ox-1];
          newpmax.maximize(pscan[ox]);newpmin.minimize(pscan[ox]);
          newpmax.maximize(cscan[ox-1]);newpmin.minimize(cscan[ox-1]);
          newpmax.maximize(cscan[ox]);newpmin.minimize(cscan[ox]);

          x1 = long(newpmin.m_x);
          x2 = long(newpmax.m_x) + 2;
          y1 = long(newpmin.m_y);
          y2 = long(newpmax.m_y) + 2;

          lAB = thline2(pscan[ox-1], cscan[ox-1]);
          lBC = thline2(cscan[ox-1], cscan[ox]);
          lCA = thline2(cscan[ox], pscan[ox-1]);
          lCD = thline2(cscan[ox], pscan[ox]);
          lDA = thline2(pscan[ox], pscan[ox-1]);

          
          // interpolate pixels within quadrilateral
          for (x = x1; x < x2; x++) {
            for (y = y1; y < y2; y++) {
              newp = thvec2(double(x), double(y));
              dAB = lAB.eval(newp);
              dBC = lBC.eval(newp);
              dCD = lCD.eval(newp);
              dCA = lCA.eval(newp);
              dDA = lDA.eval(newp);
              if (((dAB <= 0.0) && (dBC <= 0.0) && (dCA <= 0.0)) || ((dCA >= 0) && (dCD <= 0.0) && (dDA <= 0.0))) {
                dCD = fabs(dCD);
                dDA = fabs(dDA);
                dAB = fabs(dAB) + dCD;
                dBC = fabs(dBC) + dDA;
                if (dAB > 0.0) dCD /= dAB; else dCD = 0.5;
                if (dBC > 0.0) dDA /= dBC; else dDA = 0.5;
                this->mpic.rgba_set_pixel(x, y, sketch->m_pic.rgba_interpolate_pixel(double(ox) - dCD, double(oy) - 1.0 + dDA));
              }
            }
          }
          if ((counter++ % 20000) == 0)
            thprintf(" [%.0f%%]", double(counter) / double(sketch->m_pic.width * sketch->m_pic.height) * 100.0);
        }
      }
      for(ox = 0; ox < sketch->m_pic.width; ox++)
        pscan[ox] = cscan[ox];
    }

    //for (x = 0; x < mw; x++) {
    //  for (y = 0; y < mh; y++) {
    //    start = TM.backward(mpic_origin + thvec2(double(x), double(y)), x > 0 ? ss[y] : start);
    //    ss[y] = start;
    //    ptmp = T1.backward(T2.backward(start));
    //    this->mpic.rgba_set_pixel(x, y, sketch->m_pic.rgba_interpolate_pixel(ptmp.m_x, ptmp.m_y));
    //    if ((counter++ % 20000) == 0)
    //      thprintf(" [%.0f%%]", double(counter) / double(mw * mh) * 100.0);
    //  }
    //}

  }


  this->mpic.rgba_save("PNG","png",thcfg.sketch_colors);
  this->mpic.rgba_free();
  sketch->m_pic.rgba_free();
  this->mpic.scale = T3.m_scale;
  ptmp = T3.forward(mpic_origin);
  this->mpic.x = ptmp.m_x;
  this->mpic.y = ptmp.m_y;
  thprintf(" done\n");
  thtext_inline = false;
  return &(this->mpic);

}
