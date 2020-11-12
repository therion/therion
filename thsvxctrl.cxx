/**
 * @file thsvxctrl.cxx
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
 
#include "thsvxctrl.h"
#include "thdatabase.h"
#include "thdataleg.h"
#include "thtmpdir.h"
#include "thdata.h"
#include "thexception.h"
#include "thdatabase.h"
#include "thinfnan.h"
#include "thinit.h"
#include "thconfig.h"
#include <iostream>
#include <fstream>
#include "thsurvey.h"
#include "thcs.h"
#include "thlogfile.h"
#include "extern/img.h"
#include <math.h>
#include <string>
#include <fstream>

#ifdef THMSVC
#include <direct.h>
#define getcwd _getcwd
#endif

#define THPI 3.1415926535898

thsvxctrl::thsvxctrl()
{
  this->pdl.clear();
  this->svxf_ln = 1;
}


thsvxctrl::~thsvxctrl()
{
}


void thsvxctrl::write_double(double dd)
{
  if (thisnan(dd)) {
    fprintf(this->svxf,"\t-");
    return;
  }
  switch (thisinf(dd)) {
    case -1:
      fprintf(this->svxf,"\tdown");
      break;
    case 1:
      fprintf(this->svxf,"\tup");
      break;
    default:
      fprintf(this->svxf,"\t%f",dd);
      break;
  }
}



void thsvxctrl::write_survey_leg(thdataleg * legp)
{
  // first let's check infer
  if (this->pdl.infer_plumbs != legp->infer_plumbs) {
    fprintf(this->svxf,"*infer plumbs %s\n",(legp->infer_plumbs ? "on" : "off"));
    this->svxf_ln++;
  }
  if (this->pdl.infer_equates != legp->infer_equates) {
    fprintf(this->svxf,"*infer equates %s\n",(legp->infer_equates ? "on" : "off"));
    this->svxf_ln++;
  }
    
  // let's check standard deviations
  if (
      (((!thisnan(this->pdl.length_sd)) || (!thisnan(legp->length_sd))) &&
      (this->pdl.length_sd != legp->length_sd))
      ||
//      (((!thisnan(this->pdl.counter_sd)) || (!thisnan(legp->counter_sd))) &&
//      (this->pdl.counter_sd != legp->counter_sd)) 
//      ||
      (((!thisnan(this->pdl.depth_sd)) || (!thisnan(legp->depth_sd))) &&
      (this->pdl.depth_sd != legp->depth_sd)) 
      ||
      (((!thisnan(this->pdl.bearing_sd)) || (!thisnan(legp->bearing_sd))) &&
      (this->pdl.bearing_sd != legp->bearing_sd)) 
      ||
      (((!thisnan(this->pdl.gradient_sd)) || (!thisnan(legp->gradient_sd))) &&
      (this->pdl.gradient_sd != legp->gradient_sd)) 
      ||
      (((!thisnan(this->pdl.dx_sd)) || (!thisnan(legp->dx_sd))) &&
      (this->pdl.dx_sd != legp->dx_sd)) 
      ||
      (((!thisnan(this->pdl.dy_sd)) || (!thisnan(legp->dy_sd))) &&
      (this->pdl.dy_sd != legp->dy_sd)) 
      ||
      (((!thisnan(this->pdl.dz_sd)) || (!thisnan(legp->dz_sd))) &&
      (this->pdl.dz_sd != legp->dz_sd))
      ) {
      fprintf(this->svxf,"*sd default\n");
      this->svxf_ln++;
      if (!(thisnan(legp->length_sd))) {
        fprintf(this->svxf,"*sd tape %f metres\n", legp->length_sd);
        this->svxf_ln++;
      }        
//      if (!(thisnan(legp->counter_sd))) {
//        fprintf(this->svxf,"*sd counter %f metres\n", legp->counter_sd);
//        this->svxf_ln++;
//      }
      if (!(thisnan(legp->depth_sd))) {
        fprintf(this->svxf,"*sd depth %f metres\n", legp->depth_sd);
        this->svxf_ln++;
      }        
      if (!(thisnan(legp->bearing_sd))) {
        fprintf(this->svxf,"*sd compass %f degrees\n", legp->bearing_sd);
        this->svxf_ln++;
      } 
      if (!(thisnan(legp->gradient_sd))) {
        fprintf(this->svxf,"*sd clino %f degrees\n", legp->gradient_sd);
        this->svxf_ln++;
      }
      if (!(thisnan(legp->dx_sd))) {
        fprintf(this->svxf,"*sd dx %f metres\n", legp->dx_sd);
        this->svxf_ln++;
      }                
      if (!(thisnan(legp->dy_sd))) {
        fprintf(this->svxf,"*sd dy %f metres\n", legp->dy_sd);
        this->svxf_ln++;
      }
      if (!(thisnan(legp->dz_sd))) {
        fprintf(this->svxf,"*sd dz %f metres\n", legp->dz_sd);
        this->svxf_ln++;
      }
   }
   
  if (this->pdl.data_type != legp->data_type) {
    switch (legp->data_type) {
      case TT_DATATYPE_NORMAL:
        fprintf(this->svxf,"*data\tnormal\tfrom\tto\ttape\tcompass\tclino\n");
        this->svxf_ln++;
        break;
      case TT_DATATYPE_DIVING:
        fprintf(this->svxf,"*data\tdiving");
      case TT_DATATYPE_CYLPOLAR:
        if (legp->data_type != TT_DATATYPE_DIVING)
          fprintf(this->svxf,"*data\tcylpolar");
        fprintf(this->svxf,"\tfrom\tto\ttape\tcompass\tdepthchange\n");
        this->svxf_ln++;
        break;
      case TT_DATATYPE_CARTESIAN:
        fprintf(this->svxf,"*data\tcartesian\tfrom\tto\tnorthing\teasting\taltitude\n");
        this->svxf_ln++;
        break;
      case TT_DATATYPE_NOSURVEY:
        fprintf(this->svxf,"*data\tnosurvey\tfrom\tto\n");
        this->svxf_ln++;
        break;
    }
  }

  double leggridmc;
  leggridmc = 0.0;
  if (legp->gridcs != TTCS_LOCAL) {
    if (legp->gridcs != this->lastleggridmccs) {
      if (legp->gridcs == thcfg.outcs) {
        this->lastleggridmc = this->meridian_convergence;
      } else {
        this->lastleggridmc = thcfg.get_cs_convergence(legp->gridcs);
      }
      this->lastleggridmccs = legp->gridcs;
    }
    leggridmc = this->lastleggridmc;
  }


  // recalculate declination if necessary
  double legp_bearing = legp->bearing;
  double legp_dx = legp->dx;
  double legp_dy = legp->dy;
  double legp_declin = (this->meridian_convergence - leggridmc + (!thisnan(legp->declination) ? legp->declination : legp->implicit_declination));
  if (legp_declin != 0.0) {
    switch (legp->data_type) {
      case TT_DATATYPE_NORMAL:
      case TT_DATATYPE_DIVING:
      case TT_DATATYPE_CYLPOLAR:
        if (!thisnan(legp_bearing)) {
          legp_bearing += legp_declin;
          if (legp_bearing >= 360.0)
            legp_bearing -= 360.0;
          if (legp_bearing < 0.0)
            legp_bearing += 360.0;
        }
        break;
      case TT_DATATYPE_CARTESIAN:
        double cosdecl = cos(legp_declin/180*THPI), 
          sindecl = sin(legp_declin/180*THPI);
        legp_dx = (cosdecl * legp->dx) + (sindecl * legp->dy);
        legp_dy = (cosdecl * legp->dy) - (sindecl * legp->dx);
        break;
    }
  }
  
  if (legp->flags != this->pdl.flags) {
    fprintf(this->svxf,"*flags");
    if ((legp->flags & TT_LEGFLAG_DUPLICATE) == TT_LEGFLAG_NONE) fprintf(this->svxf," not");
    fprintf(this->svxf," duplicate");
    if ((legp->flags & TT_LEGFLAG_SURFACE) == TT_LEGFLAG_NONE) fprintf(this->svxf," not");
    fprintf(this->svxf," surface");
    if ((legp->flags & TT_LEGFLAG_SPLAY) == TT_LEGFLAG_NONE) fprintf(this->svxf," not");
    fprintf(this->svxf," splay");
    fprintf(this->svxf,"\n");
    this->svxf_ln++;
  }

  // now print the data
  // first stations
  if (legp->direction)
    fprintf(this->svxf, "\t%ld\t%ld", legp->from.id, legp->to.id);
  else
    fprintf(this->svxf, "\t%ld\t%ld", legp->to.id, legp->from.id);

  // then data
  switch (legp->data_type) {
    case TT_DATATYPE_NORMAL:
      this->write_double(legp->length);
      if (legp->infer_plumbs && 
          ((legp->gradient == 90.0) || (legp->gradient == -90.0)) &&
          (!thisnan(legp_bearing)))
        fprintf(this->svxf,"\t-");
      else
        this->write_double(legp_bearing);
      this->write_double(legp->gradient);
      break;
    case TT_DATATYPE_DIVING:
    case TT_DATATYPE_CYLPOLAR:
      this->write_double(legp->length);
      this->write_double(legp_bearing);
      if (!thisnan(legp->depthchange))
        this->write_double(legp->depthchange);
      else
        this->write_double(legp->todepth - legp->fromdepth);
      break;
    case TT_DATATYPE_CARTESIAN:
      this->write_double(legp_dy);
      this->write_double(legp_dx);
      this->write_double(legp->dz);
      break;
  }
  this->src_map[this->svxf_ln] = &(legp->srcf);
  fprintf(this->svxf, "\n");
  this->svxf_ln++;
  this->pdl = *legp;
}

void thsvxctrl::write_survey_fix(thdatafix * fixp)
{
  if (!(thisnan(fixp->x))) {
    fprintf(this->svxf,"*fix\t%ld\t%f\t%f\t%f",fixp->station.id,
      fixp->x,fixp->y,fixp->z);
    if (!(thisnan(fixp->sdx))) {
      fprintf(this->svxf,"\t%f\t%f\t%f",fixp->sdx,fixp->sdy,fixp->sdz);
      if (!(thisnan(fixp->cxy)))
        fprintf(this->svxf,"\t%f\t%f\t%f",fixp->cxy,fixp->cyz,fixp->cxz);
    }
    this->src_map[this->svxf_ln] = &(fixp->srcf);
    fprintf(this->svxf,"\n");
    this->svxf_ln++;
  }
}

void thsvxctrl::process_survey_data(class thdatabase * dbp)
{
#ifdef THDEBUG
    thprintf("\n\nprocessing survey data\n");
#else
    thprintf("processing survey data ... ");
    thtext_inline = true;
#endif 
  // first check if any survey data
  if (dbp->db1d.station_vec.size() == 0) {
#ifdef THDEBUG
    thprintf("no survey data\n");
#else
    thprintf("no data.\n");
    thtext_inline = false;
#endif 
    return;
  }
    
  // first create survex file
#ifdef THDEBUG
#else
//   thprintf("writing ... ");
#endif
  const char * svxfn = thtmp.get_file_name("data.svx");
  this->svxf = fopen(svxfn,"w");
  if (svxf == NULL)
    ththrow(("can't open survex file for output -- %s", svxfn))

  this->meridian_convergence = thcfg.get_outcs_convergence();
  this->lastleggridmccs = TTCS_LOCAL;
  this->lastleggridmc = 0.0;


  fprintf(this->svxf,"*units declination clino compass degrees\n");
  fprintf(this->svxf,"*units tape depth counter northing easting altitude metres\n");
  fprintf(this->svxf,"*infer plumbs off\n");
  fprintf(this->svxf,"*infer equates off\n");
  this->svxf_ln += 4;
  
  // write data to file
  thdb_object_list_type::iterator obi = dbp->object_list.begin();
  thdataleg_list::iterator lei;
  thdatafix_list::iterator fii;
  //thdatass_list::iterator ssi;
  thdataequate_list::iterator eqi;
  thdata * dp;
  //thdataobject * cx1, * cx2;
  //thdb1ds * tmps;
  
  while (obi != dbp->object_list.end()) {
  
    if ((*obi)->get_class_id() == TT_DATA_CMD) {
      
      dp = (thdata *)(*obi).get();
      
      // scan data shots
      lei = dp->leg_list.begin();
      while(lei != dp->leg_list.end()) {
        if (lei->is_valid)
          this->write_survey_leg(&(*lei));
        lei++;
      }
          
      // scan data fixes
      fii = dp->fix_list.begin();
      while(fii != dp->fix_list.end()) {
        //tmps = &(dbp->db1d.station_vec[fii->station.id - 1]);
        //cx1 = tmps->fixcontext;
        //cx2 = fii->srcf.context;
        //if (((cx1 == NULL) && (cx2 == NULL)) || ((cx1 != NULL) && (cx2 != NULL) && (cx1->id == cx2->id))) {
          this->write_survey_fix(&(*fii));
        //}
        fii++;
      }
  
      // export data equates
      int last_e = 0;
      for(eqi = dp->equate_list.begin(); eqi != dp->equate_list.end(); eqi++) {
        if (last_e != eqi->eqid) {
          if (last_e > 0) {
            fprintf(this->svxf,"\n");
            this->svxf_ln++;
          }
          fprintf(this->svxf,"*equate");
          this->src_map[this->svxf_ln] = &(eqi->srcf);
          last_e = eqi->eqid;
        }
        fprintf(this->svxf,"\t%ld",eqi->station.id);
      }
      if (last_e > 0) {
        fprintf(this->svxf,"\n");        
        this->svxf_ln++;
      }
        
    }
    obi++;
  }
  
  fclose(svxf);
  
  // run survex
  thbuffer svxcom, wdir;
  wdir.guarantee(1024);
  thassert(getcwd(wdir.get_buffer(),1024) != NULL);
  int retcode;
  svxcom = "\"";
  svxcom += thini.get_path_cavern();
  svxcom += "\" --quiet --log --output=";
  svxcom += thtmp.get_dir_name();
  svxcom += " ";
  svxcom += svxfn;
  
#ifdef THDEBUG
  thprintf("running cavern\n");
#else
//    thprintf("processing ... ");
#endif
  retcode = system(svxcom.get_buffer());

  this->transcript_log_file(dbp, thtmp.get_file_name("data.log"));

  if (retcode != EXIT_SUCCESS)
    ththrow(("cavern exit code -- %d", retcode))
  else
    this->load_err_file(dbp, thtmp.get_file_name("data.err"));


  // read coordinates from 3D file
  img_point imgpt;
  int result;
  unsigned long ss;
  thdb1ds * stp;
  img* pimg = img_open(thtmp.get_file_name("data.3d"));
  if (pimg == NULL)
    ththrow(("can't open cavern output"))    
  do {
    result = img_read_item(pimg, &imgpt);
    switch (result) {
      case img_MOVE:
      case img_LINE:
        break;
      case img_LABEL:
        if (sscanf(pimg->label,"%lu",&ss) != 0) {
          stp = & (dbp->db1d.station_vec[ss-1]);
          stp->x = imgpt.x;
          stp->y = imgpt.y;
          stp->z = imgpt.z;
        }
        break;
      case img_BAD:
        img_close(pimg);
        ththrow(("error reading cavern output"))
        break;
    }
  } while (result != img_STOP);
  img_close(pimg);

    
//  svxcom = "\"";
//   svxcom += thini.get_path_3dtopos();
//   svxcom += "\" ";
//   svxcom += thtmp.get_file_name("data.3d");
//   svxcom += " ";
//   svxcom += thtmp.get_file_name("data.pos");
// 
// #ifdef THDEBUG
//   thprintf("running 3dtopos\n");
// #endif
//   retcode = system(svxcom.get_buffer());
// 
//   if (retcode != EXIT_SUCCESS)
//     ththrow(("3dtopos exit code -- %d", retcode))
// 
// // Let's copy results and log-file to working directory
// // #ifdef THWIN32
// //   svxcom = "copy ";
// // #else
// //   svxcom = "cp ";
// // #endif
// //   svxcom += thtmp.get_file_name("data.*");
// //   svxcom += " ";
// //   svxcom += wdir.get_buffer();
// // #ifdef THDEBUG
// //   thprintf("copying results\n");
// // #endif
// // 
// //   retcode = system(svxcom.get_buffer());
// // 
// //   if (retcode != EXIT_SUCCESS)
// //     ththrow(("cp exit code -- %d", retcode))
//   
//   
// #ifdef THDEBUG
// #else
// //    thprintf("reading ... ");
// #endif 
//   // import survex export
//   std::ifstream posf;
//   thdb1ds * stp;
//   double sx, sy, sz;
//   unsigned long ss;
//   size_t lnsize = 4096, pix = 0, ppx = 0, clns;
//   svxcom.guarantee(lnsize);
//   char * lnbuff = svxcom.get_buffer(),
//     * p[4], * cps = lnbuff;
//   posf.open(thtmp.get_file_name("data.pos"));
//   if (!posf.is_open())
//     ththrow(("can't open survex output"))
//   while (!posf.eof()) {
//     posf.getline(lnbuff, lnsize);
//     clns = strlen(lnbuff);
//     ppx = 0;
//     pix = 0;
//     cps = lnbuff;
//     bool scan_num = true;
//     while ((pix < clns) && (ppx < 4)) {
//       if (scan_num && 
//           (((((unsigned char) *cps) > 47) && 
//           (((unsigned char) *cps) < 58)) || (*cps == 46) || 
//           (*cps == 45) || (*cps == 43))) {
//         p[ppx] = cps;
//         ppx++;
//         scan_num = false;
//       }
//       if (((unsigned char)*cps) < 33)
//         scan_num = true;
//       if ((*cps == ',') || (((unsigned char)*cps) < 33))
//         *cps = 0;
//       cps++;
//       pix++;
//     }
//     if (ppx == 4) {
//       if (sscanf(p[0],"%lf",&sx) == 0)
//         ppx = 0;
//       if (sscanf(p[1],"%lf",&sy) == 0)
//         ppx = 0;
//       if (sscanf(p[2],"%lf",&sz) == 0)
//         ppx = 0;
//       if (sscanf(p[3],"%lu",&ss) == 0)
//         ppx = 0;
//       // if inport OK, write coordinates into database
//       if (ppx == 4) {
//         stp = & (dbp->db1d.station_vec[ss-1]);
//         stp->x = sx;
//         stp->y = sy;
//         stp->z = sz;
//       }
//     }
//   }
//   posf.close();
// 

#ifdef THDEBUG
#else
    thprintf("done\n");
    thtext_inline = false;
#endif 

//  FILE * ptsf;
//  ptsf = fopen("data.pts","w");  
//  if (ptsf == NULL)
//    ththrow(("can't open data.pts for output"))
//  unsigned int sid, lsid;
//  lsid = dbp->db1d.station_vec.size();
//  for (sid = 0; sid < lsid; sid++) {
//    stp = & (dbp->db1d.station_vec[sid]);
//    fprintf(ptsf,"%6d -> %s@%s\n",sid+1,stp->name,stp->survey->get_full_name());
//  }
//  fclose(ptsf);

}


enum {THSVXLOGNUM_NONE, THSVXLOGNUM_LINE, THSVXLOGNUM_STATION};

void thsvxctrl::transcript_log_file(class thdatabase * dbp, const char * lfnm)
{
  thbuffer tsbuff;
  thdb1ds * stp;
  char * lnbuff = new char [4097];
  char * numbuff = &(lnbuff[2049]);
  unsigned long lnum = 0;
  thlog.printf("\n####################### cavern log file ########################\n");
  std::ifstream clf(lfnm);
  if (!(clf.is_open()))
    ththrow(("can't open cavern log file for input"))
  // let's read line by line and print to log file
  size_t chidx, nchs;
  char * chch;
  bool onnum, ondig, fonline;
  long csn;
  size_t lsid;
  lsid = dbp->db1d.station_vec.size();
  while (!(clf.eof())) {
    lnum++;
    clf.getline(lnbuff,2048);
    thlog.printf("%2d> %s\n",lnum,lnbuff);
    // let's scan the line
    chch = lnbuff;
    nchs = strlen(chch);
    onnum = false;
    fonline = true;
    csn = 0;
    chidx = 0;
    char * start_ch = NULL; //, * test_ch;
    int num_type;
    thsvxctrl_src_maptype::iterator srcmi;
    const auto prev_char_is = [&lnbuff, &start_ch](const char c) { return (start_ch - 1) >= lnbuff && start_ch[-1] == c; };
//    if (*lnbuff == 13) lnbuff++;
//    if (strncmp(lnbuff,"There were",10) == 0)
//      chidx = 2049;
//    else if (strncmp(lnbuff,"There are",9) == 0)
//      chidx = 2049;
//    else if (strncmp(lnbuff,"Survey has",10) == 0)
//      chidx = 2049;
//    else if (strncmp(lnbuff,"Survey contains",15) == 0)
//      chidx = 2049;
//    else if (nchs > 6) {
//      if (strcmp(&(lnbuff[nchs-6]),"nodes.") == 0)
//        chidx = 2049;
//      else if (strcmp(&(lnbuff[nchs-5]),"node.") == 0)
//        chidx = 2049;
//    }
    while ((chidx <= nchs) && (chidx <= 2048)) {
    
      if ((chidx < nchs) && (chidx < 2048) &&
          (((unsigned char) *chch) > 47) && (((unsigned char) *chch) < 58)) {
        if (!onnum)
          start_ch = chch;
        ondig = true;
        onnum = true;
      }
      else
        ondig = false;
        
      if (onnum) {
        if (ondig) {
          csn *= 10;
          csn += ((unsigned char) *chch) - 48;
        }
        else {
          num_type = THSVXLOGNUM_STATION;
          if ((chidx > 0) && (chidx <= nchs)) {
            if (prev_char_is(':') && (*chch == ':'))
              num_type = THSVXLOGNUM_LINE;
          }
          if (num_type == THSVXLOGNUM_STATION) {
            if ((chidx > 0) && (chidx <= nchs)) {
              if ((*chch == '.') || (*chch == ',') || (*chch == ')') || (*chch == 'm')
                || (*chch == '-') || (*chch == '/'))
                num_type = THSVXLOGNUM_NONE;
              if (prev_char_is('.') || prev_char_is('-'))
                num_type = THSVXLOGNUM_NONE;
            }
          }
      
          switch (num_type) {
            case THSVXLOGNUM_LINE:
              srcmi = this->src_map.find((unsigned long)csn);
              if (srcmi != this->src_map.end()) {
                if (!fonline) {
                  fonline = true;
                  tsbuff.strcat("\n");
                }
                sprintf(numbuff,"%2ld> input:%ld -- %s [%ld]\n",lnum,csn,srcmi->second->name,srcmi->second->line);
                tsbuff.strcat(numbuff);
              }
              break;
            case THSVXLOGNUM_STATION:
              csn--;
              if ((csn >= 0) && (csn < long(lsid))) {
                if (fonline) {
                  sprintf(numbuff,"%2ld> ",lnum);
                  tsbuff.strcat(numbuff);
                  fonline = false;
                }
                else {
                  tsbuff.strcat(" -- ");
                }
                sprintf(numbuff,"%ld : ",(csn+1));
                stp = & (dbp->db1d.station_vec[(unsigned int)csn]);
                tsbuff.strcat(numbuff);
                tsbuff.strcat(stp->name);
                tsbuff.strcat("@");
                tsbuff.strcat(stp->survey->get_full_name());
              }
              break;
            default:
              break;
          } // end of switch
          onnum = false;
          csn = 0;
        }
      }
      
      chch++;
      chidx++;
    }
    
    if (!fonline)
        tsbuff.strcat("\n");

  }
  clf.close();
  thlog.printf("######################### transcription ########################\n%s",tsbuff.get_buffer());
  thlog.printf("#################### end of cavern log file ####################\n");
  delete [] lnbuff;
}

void thsvxctrl::load_err_file(class thdatabase * dbp, const char * lfnm) {

	std::string line;
	std::ifstream infile(lfnm);
	thmbuffer b;
	unsigned long sf, st, prev_st, nst;
	long i;

	while(std::getline(infile, line)) {

//		thprintf("SCANNING: %s\n", line.c_str());
		thsplit_args(&b, line.c_str());
		thdb1d_traverse t, * ct;
		thdb1d_loop_leg l;
		thdb1d_loop_leg * cl;
		thdb1d_tree_node * cn;
		thdb1d_tree_arrow * ca;
		bool has_legs;
		has_legs = false;
		nst = 0;
		cl = NULL;
		prev_st = 0;

		i = 0;
		if (b.get_size() > 2) {
			i = 0;
			while (i < b.get_size()) {
//				thprintf("%s\n", b.get_buffer()[i]);
			  st = 0;
			  sscanf(b.get_buffer()[i], "%lu", &st);
				if (st > 0) {
					nst++;
					if (nst == 1) {
						sf = st;
//						thprintf("\nFROM: %lu\n", sf);
						t.first_leg = NULL;
						t.last_leg = NULL;
						t.from = & (dbp->db1d.station_vec[sf-1]);
						t.nlegs = 0;
						prev_st = sf;
						cl = NULL;
					} else {
						t.to = & (dbp->db1d.station_vec[st-1]);
						if (strcmp(b.get_buffer()[i-1],"-") == 0) {
							has_legs = true;
							l.prev_leg = cl;
							l.next_leg = NULL;
							l.leg = NULL;
							l.reverse = false; //TODO
							// find appropriate leg
							cn = &(dbp->db1d.tree_nodes[dbp->db1d.station_vec[prev_st-1].uid - 1]);
							for(ca = cn->first_arrow; ca != NULL; ca = ca->next_arrow) {
								if (ca->end_node->uid == dbp->db1d.station_vec[st-1].uid) {
									l.leg = ca->leg->leg;
									l.reverse = ca->leg->reverse;
									break;
								}
							}
							if (l.leg != NULL) {
								cl = &(*dbp->db1d.loop_leg_list.insert(dbp->db1d.loop_leg_list.end(), l));
								if (t.nlegs == 0) {
									t.first_leg = cl;
								}
								if (cl->prev_leg != NULL) {
									cl->prev_leg->next_leg = cl;
								}
								t.last_leg = cl;
								t.nlegs++;
							}
							else {
//								thprintf("LEG not found!!!\n");
							}
//							thprintf("TO: %s %lu\n", b.get_buffer()[i], st);
						}
						else if (strcmp(b.get_buffer()[i-1],"=") == 0) {
//							thprintf("EQ: %s %lu\n", b.get_buffer()[i], st);
						}
						prev_st = st;
					}
				}
				i++;
			}
		}
		if (has_legs) {
			t.id = dbp->db1d.traverse_list.size() + 1;
			// TODO: read original length
			std::getline(infile, line);
			// read E
			std::getline(infile, line);
			sscanf(line.c_str(), "%lf", &t.E);
			// TODO: read H, V
			std::getline(infile, line);
//			thprintf("E: %.2lf\n", t.E);
			ct = &(*dbp->db1d.traverse_list.insert(dbp->db1d.traverse_list.end(), t));
			for(cl = ct->first_leg; cl != NULL; cl = cl->next_leg) {
				cl->leg->traverse = ct;
			}
		}
	}


}
