/**
 * @file thsketch.cxx
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
 
#include "thsketch.h"
#include "thexception.h"
#include "thchenc.h"
#include "thdb2d.h"
#include "thtrans.h"

thsketch::thsketch()
{

  this->proj = thdb.db2d.get_default_projection();
  this->morphed = false;

}


thsketch::~thsketch()
{
}


int thsketch::get_class_id() 
{
  return TT_SKETCH_CMD;
}


bool thsketch::is(int class_id)
{
  if (class_id == TT_SKETCH_CMD)
    return true;
  else
    return thdataobject::is(class_id);
}

int thsketch::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 0;
}


char * thsketch::get_cmd_end()
{
  // insert endcommand if multiline command
  return "endsketch";
}


char * thsketch::get_cmd_name()
{
  // insert command name here
  return "sketch";
}


thcmd_option_desc thsketch::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_sketch_opt);
  if (id == TT_SKETCH_UNKNOWN)
    return thdataobject::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void thsketch::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  thdb2dprjpr projection;

  switch (cod.id) {

    case 0:
      this->parse_station(args);
      break;

    case TT_SKETCH_PICTURE:
      this->pic.init(args[0], thdb.csrc.name);
      break;

    case TT_SKETCH_PROJECTION:
      projection = this->db->db2d.parse_projection(*args);
      if (!projection.parok)
        ththrow(("invalid parameters of projection"));
      this->proj = projection.prj;
      break;
    
    // if not found, try to set fathers properties  
    default:
      thdataobject::set(cod, args, argenc, indataline);
  }
}


void thsketch::self_delete()
{
  delete this;
}

void thsketch::self_print_properties(FILE * outf)
{
  thdataobject::self_print_properties(outf);
  fprintf(outf,"thsketch:\n");
  if (this->pic.fname != NULL) fprintf(outf,"\tpicture: %s", this->pic.fname);
  thsketchst_list::iterator ii;
  fprintf(outf,"\tstations:\n");
  for(ii = this->stations.begin(); ii != this->stations.end(); ii++) {
    fprintf(outf, "\t\t%.2f\t%.2f\t", ii->x, ii->y);
    fprintf(outf, ii->station);
    fprintf(outf, "\n");
  }
}


void thsketch::parse_picture(char ** args)
{
}


void thsketch::parse_station(char ** args) 
{
  thsplit_args(&(thdb.mbuff_tmp), args[0]);
  long na;
  int sv; 
  char ** aa;
  thsketchst st;
  na = thdb.mbuff_tmp.get_size();
  aa = thdb.mbuff_tmp.get_buffer();

  if (na == 0)
    return;
  
  if (na != 3)
    ththrow(("invalid sketch station specification - should be \"<x> <y> <station>\""));

  thparse_double(sv, st.x, aa[0]);
  if (sv != TT_SV_NUMBER) 
    ththrow(("invalid station pixel X coordinate"));

  thparse_double(sv, st.y, aa[1]);
  if (sv != TT_SV_NUMBER) 
    ththrow(("invalid station pixel Y coordinate"));

  thparse_objectname(st.station, &(this->db->buff_stations), aa[2], this);
  st.source = this->db->csrc;

  this->stations.push_back(st);
  
}

void thsketch::check_stations()
{
  thsketchst_list::iterator i;
  for (i = this->stations.begin(); i != this->stations.end(); i++) {
    i->station.id = thdb.db1d.get_station_id(i->station, i->station.psurvey);
    if (i->station.id == 0) {
      this->throw_source();
      if (i->station.survey == NULL)
        threthrow2(("station doesn't exist -- %s", i->station.name))
      else
        threthrow2(("station doesn't exist -- %s@%s", i->station.name, i->station.survey))
    }
  }
}


bool thsketch::morph()
{
  if (this->morphed)
    return this->mpic.exists();

  this->morphed = true;

  if (!this->pic.exists())
    return false;

  if (this->stations.size() < 2)
    return false;

  thlintrans T1, T2, T3, TLIN;
  thsketchst_list::iterator ii;

  // Calculate transformations.
  // T1 - shift
  // T2 - rotate from TLIN
  // T3 - scale and shift from TLIN
  char * fn;
  size_t fnx, fnl;
  fn = this->pic.fname;
  fnl = strlen(this->pic.fname);
  for(fnx = 0; fnx < fnl; fnx++) {
    if (((this->pic.fname[fnx] == '/') || (this->pic.fname[fnx] == '\\')) && (fnx < fnl - 1)) {
      fn = &(this->pic.fname[fnx + 1]);
    }
  }

  if (thtext_inline) thprintf("\n");
  thprintf("morphing %s ", fn);
  thtext_inline = true;

  for (ii = this->stations.begin(); ii != this->stations.end(); ii++) {
    T1.m_shift -= thvec2(ii->x, ii->y);
  }
  T1.m_shift /= double(this->stations.size());
  T1.init_backward();

  for (ii = this->stations.begin(); ii != this->stations.end(); ii++) {
    TLIN.insert_point(T1.forward(thvec2(ii->x, ii->y)), 
      thvec2(thdb.db1d.station_vec[ii->station.id - 1].x, thdb.db1d.station_vec[ii->station.id - 1].y));
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

  long mw, mh, x, y, counter;
  thvec2 pmin, pmax, ptmp, mpic_origin;
  counter = 1;

#define TRANS_MORPH 0

  if ((this->stations.size() < 3) || (!TRANS_MORPH)) {

    pmin = pmax = T2.forward(T1.forward(thvec2(0.0, 0.0)));
    ptmp = T2.forward(T1.forward(thvec2(double(this->pic.width), 0.0))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    ptmp = T2.forward(T1.forward(thvec2(0.0, double(this->pic.height)))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    ptmp = T2.forward(T1.forward(thvec2(double(this->pic.width), double(this->pic.height)))); pmin.minimize(ptmp); pmax.maximize(ptmp);

    mpic_origin = thvec2(pmin.m_x, pmin.m_y);
    mw = long(pmax.m_x + 0.5) - long(pmin.m_x - 0.5);
    mh = long(pmax.m_y + 0.5) - long(pmin.m_y - 0.5);
    thprintf("(%.1f Mpix) ...", double(mw * mh) / 1000000.0);


    this->pic.rgba_load();
    this->mpic.rgba_init(mw, mh);
    for (x = 0; x < mw; x++) {
      for (y = 0; y < mh; y++) {
        ptmp = T1.backward(T2.backward(mpic_origin + thvec2(double(x), double(y))));
        this->mpic.rgba_set_pixel(x, y, this->pic.rgba_interpolate_pixel(ptmp.m_x, ptmp.m_y));
        if ((counter++ % 100000) == 0)
          thprintf(".");
      }
    }
  
  } else {

    thmorphtrans TM;
    for (ii = this->stations.begin(); ii != this->stations.end(); ii++) {
      TM.insert_point(
        T2.forward(T1.forward(thvec2(ii->x, ii->y))), 
        T3.backward(thvec2(thdb.db1d.station_vec[ii->station.id - 1].x, thdb.db1d.station_vec[ii->station.id - 1].y)));
    }

    pmin = pmax = TM.forward(T2.forward(T1.forward(thvec2(0.0, 0.0))));
    for(x = 0; x <= double(this->pic.width); x++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), 0.0)))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(x), double(this->pic.height))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }
    for(y = 0; y <= double(this->pic.height); y++) {
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(0.0, double(x))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
      ptmp = TM.forward(T2.forward(T1.forward(thvec2(double(this->pic.width), double(x))))); pmin.minimize(ptmp); pmax.maximize(ptmp);
    }

    thvec2 start;
    start = T2.forward(T1.forward(thvec2(0.0, 0.0)));
    mpic_origin = thvec2(pmin.m_x, pmin.m_y);
    mw = long(pmax.m_x + 0.5) - long(pmin.m_x - 0.5);
    mh = long(pmax.m_y + 0.5) - long(pmin.m_y - 0.5);
    thprintf("(%.1f Mpix) ...", double(mw * mh) / 1000000.0);

    this->pic.rgba_load();
    this->mpic.rgba_init(mw, mh);
    for (x = 0; x < mw; x++) {
      for (y = 0; y < mh; y++) {
        start = TM.backward(mpic_origin + thvec2(double(x), double(y)), start);
        ptmp = T1.backward(T2.backward(start));
        this->mpic.rgba_set_pixel(x, y, this->pic.rgba_interpolate_pixel(ptmp.m_x, ptmp.m_y));
        if ((counter++ % 100000) == 0)
          thprintf(".");
      }
    }
  }

  this->mpic.rgba_save("PNG","png");
  this->mpic.rgba_free();
  this->pic.rgba_free();
  this->mpic.scale = T3.m_scale;
  ptmp = T3.forward(mpic_origin);
  this->mpic.x = ptmp.m_x;
  this->mpic.y = ptmp.m_y;
  thprintf(" done\n");
  thtext_inline = false;
  return true;

}






