/**
 * @file thscraplp.cxx
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
 
#include "thscraplp.h"
#include "thscrap.h"
#include "thexpmap.h"
#include "extern/lxMath.h"

thscraplp::thscraplp() {

  this->next_item = NULL;
  this->prev_item = NULL;
    
  this->station_name.clear();
  this->station = NULL;
  this->ustation = NULL;
  this->arrow = NULL;

  this->lnio = false;
  this->type = 0;
}

void thscraplp::export_mp(thexpmapmpxs * out, thscrap * scrap) {

  if ((this->arrow != NULL) && (scrap->get_outline() == NULL) && (out->symset->assigned[SYMA_DIMENSIONS])) {

    lxVec vff, vf1, vf2, vtt, vt1, vt2, rvec;
    double fl, fr, tl, tr;
    bool vertical;
    lxVecLimits vlim;

    bool reverse = (thdb.db1d.station_vec[this->arrow->leg->leg->from.id - 1].uid != arrow->start_node->uid);
    //bool reverse = false;
    //if (this->arrow->is_reversed) reverse = !reverse;
    //if (this->arrow->leg->reverse) reverse = !reverse;

    thdataleg * cl = this->arrow->leg->leg;
    if (reverse) {
      vff = lxVec(this->lnx2, this->lny2, this->lnz2);
      vtt = lxVec(this->lnx1, this->lny1, this->lnz1);
    } else {
      vff = lxVec(this->lnx1, this->lny1, this->lnz1);
      vtt = lxVec(this->lnx2, this->lny2, this->lnz2);
    }

    if (cl->walls != TT_FALSE) {
      if ((vff.x == vtt.x) && (vff.y == vtt.y)) {
        vlim.valid = false;
        vlim.Add(-cl->from_left,-cl->from_down,0.0);
        vlim.Add(cl->from_right,cl->from_up  ,0.0);
        vlim.Add(-cl->to_left,-cl->to_down,0.0);
        vlim.Add(cl->to_right,cl->to_up  ,0.0);
        vf1 = vff + vlim.min;
        vf2 = vff + lxVec(vlim.min.x, vlim.max.y, 0.0);
        vt2 = vff + vlim.max;
        vt1 = vff + lxVec(vlim.max.x, vlim.min.y, 0.0);
      } else {
        vertical = false;
        switch (out->proj->type) {
          case TT_2DPROJ_ELEV:
          case TT_2DPROJ_EXTEND:
            if (fabs(cl->total_gradient) < cl->vtresh)
              vertical = true;
            if ((vertical) || (vff.x < vtt.x) || ((vff.x == vtt.x) && (vff.y < vtt.y))) {
              fl = cl->from_up; 
              fr = cl->from_down; 
              tl = cl->to_up;
              tr = cl->to_down;
            } else {
              fl = cl->from_down; 
              fr = cl->from_up; 
              tl = cl->to_down;
              tr = cl->to_up;
            }
            break;
          default:
            fl = cl->from_left; 
            fr = cl->from_right; 
            tl = cl->to_left;
            tr = cl->to_right;
        }
        if (vertical) {
          rvec = lxVec(0.0, 1.0, 0.0);
        } else {
          rvec = lxVec(vtt.x - vff.x, vtt.y - vff.y, 0.0);
          rvec = rvec.Rotated(90.0,0.0);
          rvec.Normalize();
        }
        vf1 = vff + (fl * rvec);
        vf2 = vff - (fr * rvec);
        vt1 = vtt + (tl * rvec);
        vt2 = vtt - (tr * rvec);
      }

        out->symset->export_mp_symbol_options(out->file, SYMA_DIMENSIONS);
        fprintf(out->file,"%s(((%.2f,%.2f) -- (%.2f,%.2f) -- (%.2f,%.2f) -- (%.2f,%.2f) -- cycle));\n",
        out->symset->get_mp_macro(SYMA_DIMENSIONS),
        thxmmxst(out, vf1.x, vf1.y),
        thxmmxst(out, vt1.x, vt1.y), 
        thxmmxst(out, vt2.x, vt2.y), 
        thxmmxst(out, vf2.x, vf2.y));
    }
  }
  
  // export line
  bool export_shot = true;
  if (this->arrow != NULL) {
    thdataleg * cl = this->arrow->leg->leg;
    export_shot = ((cl->flags & TT_LEGFLAG_SPLAY) == 0) || scrap->centerline_io;
  }
  
  if (export_shot) {
    out->symset->export_mp_symbol_options(out->file, this->type);
    fprintf(out->file,"%s(((%.2f,%.2f) -- (%.2f,%.2f)));\n",
      out->symset->get_mp_macro(this->type),
      thxmmxst(out, this->lnx1, this->lny1),
      thxmmxst(out, this->lnx2, this->lny2));
  }

}
