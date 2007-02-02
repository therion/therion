#include "lxLRUD.h"

// Standard libraries
#ifndef LXDEPCHECK
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>
#endif  
//LXDEPCHECK - standart libraries


long lxLRUDData::GetStationID(lxVec pos, bool insert)
{
  std::map<lxVec,long>::iterator smi;
  smi = this->station_map.find(pos);
  if (smi == this->station_map.end()) {
    if (insert) {
      this->station_map[pos] = this->nstations;
      nstations++;
      return this->nstations;
    } else {
      return -1;
    }
  } else {
    return smi->second;
  }
}



void lxLRUD::SetVThreshold(double vt)
{
  this->i.currentVT = vt;
}


void lxLRUD::SetProfile(int sec_type)
{
  this->i.currentProfile = sec_type;
}


void lxLRUD::InsertShot(double * from, double * to, double * fromLRUD, double * toLRUD, void * usrData)
{

  lxLRUDShot shot;

  shot.fl = fromLRUD[0];
  shot.fr = fromLRUD[1];
  shot.fu = fromLRUD[2];
  shot.fd = fromLRUD[3];

  shot.tl = toLRUD[0];
  shot.tr = toLRUD[1];
  shot.tu = toLRUD[2];
  shot.td = toLRUD[3];

  shot.vthresh = this->i.currentVT;
  shot.profile = this->i.currentProfile;

  shot.from = lxVec(from[0], from[1], from[2]);
  shot.to   = lxVec(to[0], to[1], to[2]);
  shot.xfrom = this->i.GetStationID(shot.from, true);
  shot.xto = this->i.GetStationID(shot.to, true);
  
  shot.usr_data = usrData;

  this->i.shots.insert(this->i.shots.end(), shot);

}


struct xcEdge {
  size_t fx, tx;
  lxLRUDShot * shot;
  lxVec dir;
  bool used;
  xcEdge() : fx(0), tx(0), shot(NULL), used(false) {}
};

struct xcNodeArrow {
  xcEdge * edge;
  bool orient;
  xcNodeArrow() : edge(NULL), orient(true) {}
  xcNodeArrow(xcEdge * e, bool o) : edge(e), orient(o) {}
  lxVec GetDir();
  size_t GetFX();
  double GetFU();
  double GetFD();
  double GetFL();
  double GetFR();
  size_t GetTX();
  double GetTU();
  double GetTD();
  double GetTL();
  double GetTR();
};

lxVec xcNodeArrow::GetDir() {
  return (this->orient ? 1.0 : -1.0) * this->edge->dir;
}

size_t xcNodeArrow::GetFX() {
  return (this->orient ? this->edge->fx : this->edge->tx);
}

size_t xcNodeArrow::GetTX() {
  return (this->orient ? this->edge->tx : this->edge->fx);
}

#define xcNAGet(d,e,f) double xcNodeArrow::Get##d() { \
  return (this->orient ? lxMax(this->edge->shot->e, lxVecPrec) : lxMax(this->edge->shot->f, lxVecPrec)); \
}

xcNAGet(TU,tu,fu)
xcNAGet(TD,td,fd)
xcNAGet(TL,tl,fl)
xcNAGet(TR,tr,fr)
xcNAGet(FU,fu,tu)
xcNAGet(FD,fd,td)
xcNAGet(FL,fl,tl)
xcNAGet(FR,fr,tr)


struct xcNode {
  size_t id;
  lxVec p;
  std::list<xcNodeArrow> arrows;
};

struct xcSeriesStart {
  double prob;
  xcNodeArrow arrow;
};

bool operator < (const xcSeriesStart & s, const xcSeriesStart & t)
{
  return s.prob < t.prob;
}

enum {
  LXPE_FLAT,
  LXPE_ROUND,
  LXPE_CONT,
};

struct xcSeriesArrow {
  xcNodeArrow arrow;
  xcSeriesArrow * next, * prev;
  int fType, tType, fXMin, tXMin;
  lxVec f, t, ft, sDir, uDir, rDir, fDir, nfDir, tDir, ntDir;
  double fDist, tDist;
  lxTriGeomPoint fp[9], tp[9], nfp[9], ntp[9];
  lxPlane fPlane, tPlane, nfPlane, ntPlane;
  xcSeriesArrow() : next(NULL), prev(NULL) {}
};


void lxLRUD::Calculate()
{
  // TODO - more sophisticated LRUD modelling :)

  std::list<lxLRUDShot>::iterator shi;
  lxVec shotDir, uDir, rDir;

  double pX[8], pY[8];
  int prevProfile, i;
  prevProfile = -1;
  double ca, ci;

  // 1. traverse all shots and create a list of nodes
  size_t csx, cex, nedg, cnx, nnod;
  std::map<lxVec, size_t> stMap;
  std::map<lxVec, size_t>::iterator stMapIter;
  nedg = this->i.shots.size();
  nnod = 0;
  if (nedg == 0)
    return;
  xcEdge * edges = new xcEdge [nedg], * cedge;
  for(csx = 0, shi = this->i.shots.begin(); shi != this->i.shots.end(); shi++) {

    cedge = &(edges[csx]);
    cedge->shot = &(*shi);

    stMapIter = stMap.find(shi->from);
    if (stMapIter == stMap.end()) {
      cedge->fx = nnod;
      stMap[shi->from] = nnod++;
    } else {
      cedge->fx = stMapIter->second;
    }

    stMapIter = stMap.find(shi->to);
    if (stMapIter == stMap.end()) {
      cedge->tx = nnod;
      stMap[shi->to] = nnod++;
    } else {
      cedge->tx = stMapIter->second;
    }

    if (cedge->fx != cedge->tx)
      csx++;
    else
      nedg--;
    
  }

  if (nedg == 0) {
    delete [] edges;
    return;
  }

  xcNode * nodes = new xcNode [nnod], * cnode;
  for(stMapIter = stMap.begin(); stMapIter != stMap.end(); stMapIter++) {
    nodes[stMapIter->second].id = stMapIter->second;
    nodes[stMapIter->second].p = stMapIter->first;
  }

  // 2. for each node, create a list of edges (with edge orientation)
  for(cex = 0; cex < nedg; cex++) {
    cedge = &(edges[cex]);
    cedge->dir = nodes[cedge->tx].p - nodes[cedge->fx].p;
    cedge->dir.Normalize();
    nodes[cedge->fx].arrows.push_back(xcNodeArrow(cedge, true));
    nodes[cedge->tx].arrows.push_back(xcNodeArrow(cedge, false));
  }

  // 3. calculate series starting arrow probabilities
  std::vector<xcSeriesStart> sstarts;
  size_t narrows;
  xcSeriesStart sstart;
  std::list<xcNodeArrow>::iterator nait;
  for (cnx = 0; cnx < nnod; cnx++) {
    cnode = &(nodes[cnx]);
    narrows = cnode->arrows.size();
    for (nait = cnode->arrows.begin(); nait != cnode->arrows.end(); nait++) {
      sstart.arrow = *nait;
      sstart.prob = ((narrows % 2 == 0) ? 1.0 : -1.0) / double (narrows);
      sstarts.push_back(sstart);
    }
  }

  std::sort(sstarts.begin(), sstarts.end());

  // 4. create series
  bool nextarrow = true;
  xcSeriesArrow sarrow;
  xcNodeArrow parrow, narrow;
  xcNode * lastN;
  std::list<xcSeriesArrow> series;
  std::list<xcSeriesArrow>::iterator si;
  xcSeriesArrow * psi;
  std::vector<xcSeriesStart>::iterator ssi;
  sarrow.arrow = sstarts[0].arrow;
  sarrow.arrow.edge->used = true;
  series.push_back(sarrow);
  parrow = sarrow.arrow;

  while (nextarrow) {

    // find next arrow from parrow
    nextarrow = false;
    lastN = &(nodes[parrow.GetTX()]);
    for(nait = lastN->arrows.begin(); nait != lastN->arrows.end(); nait++) {
      if (!nait->edge->used) {
        if (nextarrow) {
          if ((nait->GetDir() * parrow.GetDir()) < (narrow.GetDir() * parrow.GetDir())) {
            narrow = *nait;
          }
        } else {
          nextarrow = true;
          narrow = *nait;
        }
      }
    }

    // if not found, find new start arrow
    if (!nextarrow) {
      for(ssi = sstarts.begin(); ssi != sstarts.end(); ssi++) {
        if (!ssi->arrow.edge->used) {
          narrow = ssi->arrow;
          nextarrow = true;
          break;
        }
      }
    }

    // add new series item
    if (nextarrow) {
      narrow.edge->used = true;
      sarrow.arrow = narrow;
      series.push_back(sarrow);
    }
    parrow = narrow;

  }

  xcSeriesArrow * first, * prev;
  first = NULL;
  prev = NULL;
  for(si = series.begin(); si != series.end(); si++) {
    if (prev != NULL) {
      if (si->arrow.GetFX() == prev->arrow.GetTX()) {
        si->prev = prev;
        prev->next = &(*si);
      } else {
        if ((first != NULL) && (first->arrow.GetFX() == prev->arrow.GetTX())) {
          prev->next = first;
          first->prev = prev;
        }
        first = NULL;
      }
    }
    prev = &(*si);
    if (first == NULL)
      first = prev;
  }
  if ((first != NULL) && (first->arrow.GetFX() == prev->arrow.GetTX())) {
    prev->next = first;
    first->prev = prev;
  }

  for(si = series.begin(); si != series.end(); si++) {
    si->fType = LXPE_CONT;
    si->tType = LXPE_CONT;
    if (si->prev == NULL) {
      if (nodes[si->arrow.GetFX()].arrows.size() == 1) {
        si->fType = LXPE_FLAT;
      } else {
        si->fType = LXPE_ROUND;
      }
    }
    if (si->next == NULL) {
      if (nodes[si->arrow.GetTX()].arrows.size() == 1) {
        si->tType = LXPE_FLAT;
      } else {
        si->tType = LXPE_ROUND;
      }
    }
  }



  for(si = series.begin(); si != series.end(); si++) {

    psi = &(*si);
    
#define pXY(i,x,y) pX[i] = x; pY[i] = y;

    if (shi->profile != prevProfile) {
      switch (shi->profile) {
        case LXLRUD_TUNNEL:
          pXY(0,0.0,1.0)
          pXY(1,0.5,0.75)
          pXY(2,0.8,0.0)
          pXY(3,1.0,-1.0)
          pXY(4,0.0,-1.0)
          pXY(5,-1.0,-1.0)
          pXY(6,-0.8,0.0)
          pXY(7,-0.5,0.75)
          break;
        case LXLRUD_DIAMOND:
          pXY(0,0.0,1.0)
          pXY(1,0.5,0.5)
          pXY(2,1.0,0.0)
          pXY(3,0.5,-0.5)
          pXY(4,0.0,-1.0)
          pXY(5,-0.5,-0.5)
          pXY(6,-1.0,0.0)
          pXY(7,-0.5,0.5)
          break;
        case LXLRUD_SQUARE:
          pXY(0,0.0,1.0)
          pXY(1,1.0,1.0)
          pXY(2,1.0,0.0)
          pXY(3,1.0,-1.0)
          pXY(4,0.0,-1.0)
          pXY(5,-1.0,-1.0)
          pXY(6,-1.0,0.0)
          pXY(7,-1.0,1.0)
          break;
        default:
          pXY(0,0.0,1.0)
          pXY(1,0.707,0.707)
          pXY(2,1.0,0.0)
          pXY(3,0.707,-0.707)
          pXY(4,0.0,-1.0)
          pXY(5,-0.707,-0.707)
          pXY(6,-1.0,0.0)
          pXY(7,-0.707,0.707)
          break;
      }
    }
    prevProfile = shi->profile;

    shotDir = nodes[si->arrow.GetTX()].p - nodes[si->arrow.GetFX()].p;

    rDir = lxVec(shotDir.y, -shotDir.x, 0.0);
    if (rDir.Norm() == 0.0) {
      rDir = lxVec(1.0, 0.0, 0.0);
      uDir = lxVec(0.0, (shotDir.z < 0 ? 1.0 : -1.0), 0.0);
    } else {
      rDir.Normalize();
      ci = shotDir.Inclination();
      if  (fabs(ci) < si->arrow.edge->shot->vthresh)
        uDir = lxVec(0.0, 0.0, 1.0);
      else {
        uDir = lxVec(0.0, 1.0, 0.0);
        ca = shotDir.Azimuth();
        uDir = lxPol2Vec(1.0, (ci >= 0.0 ? ca + 180.0 : ca), (ci >= 0.0 ? 90 - ci : 90 + ci));
      }
    }

    si->uDir = uDir;
    si->rDir = rDir;
    si->sDir = uDir ^ rDir;
    si->sDir.Normalize();

    si->f = nodes[si->arrow.GetFX()].p;
    si->t = nodes[si->arrow.GetTX()].p;
    si->ft = si->t - si->f;

    si->tDir = si->sDir;
    si->fDir = si->sDir;
    si->fPlane.Init(si->f, si->fDir);
    si->tPlane.Init(si->t, si->tDir);
    si->fXMin = -1;
    si->tXMin = -1;
    si->fDist = -1.0;
    si->tDist = -1.0;

    for(i = 0; i < 8; i++) {
      si->fp[i].p = si->f + pX[i] * (pX[i] > 0 ? si->arrow.GetFR() : si->arrow.GetFL()) * rDir + pY[i] * (pY[i] > 0 ? si->arrow.GetFU() : si->arrow.GetFD()) * uDir;
      si->tp[i].p = si->t + pX[i] * (pX[i] > 0 ? si->arrow.GetTR() : si->arrow.GetTL()) * rDir + pY[i] * (pY[i] > 0 ? si->arrow.GetTU() : si->arrow.GetTD()) * uDir;
    }

  }

  // spocitajme fDir a tDir, fPlane a tPlane
  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);
    if (si->next != NULL) {
      si->tDir = si->sDir + si->next->sDir;
      if (si->tDir.Norm() < 1E-6) {
        if (si->tType == LXPE_CONT) {
          si->tType = LXPE_ROUND;
          si->next->fType = LXPE_ROUND;
        }
      } else {
        si->tDir.Normalize();
        si->next->fDir = si->tDir;
        si->tPlane.Init(si->t, si->tDir);
        si->next->fPlane = si->tPlane;
      }
    }
  }

  // spocitajme body na fPlane a tPlane
  lxVec ffp[8], ttp[8], nsDir;
  lxPlane sPlane;
  double sDistMax(0.0), sDistMin(0.0), sDistCur;
  int sXMax(0), sXMin(0);
  bool allOK;
  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);
    
    // spocitajme tPlane
    allOK = true;
    sPlane.Init(si->t, si->sDir);
    for(i = 0; i < 8; i++) {
      allOK = allOK && (si->tPlane.CalcIntersection(si->fp[i], si->tp[i], ttp[i]));
      sDistCur = sPlane.CalcPosition(ttp[i]);
      if (i == 0) {
        sDistMax = sDistCur;
        sXMax = 0;
        sDistMin = sDistCur;
        sXMin = 0;
      } else {
        if (sDistCur > sDistMax) {
          sDistMax = sDistCur;
          sXMax = i;
        }
        if (sDistCur < sDistMin) {
          sDistMin = sDistCur;
          sXMin = i;
        }
      }
    }
    if (sXMin != sXMax) {
      nsDir = ttp[sXMin] - si->tp[sXMax];
      if (nsDir.Norm() > 0)
        nsDir.Normalize();
      else
        allOK = false;
      si->ntDir = nsDir ^ si->tDir;
      si->ntDir = nsDir ^ si->ntDir;
      if ((si->ntDir * si->sDir) < 0)
        si->ntDir *= -1.0;
      else if ((si->ntDir * si->sDir) == 0.0)
        allOK = false;
      
      si->ntDir.Normalize();
    } else {
      si->ntDir = si->sDir;
    }
    si->ntPlane.Init(ttp[sXMin], si->ntDir);
    for(i = 0; i < 8; i++) {
      allOK = allOK && si->ntPlane.CalcIntersection(si->fp[i], si->tp[i], si->ntp[i]);
    }
    
    if (si->tType == LXPE_CONT) {
      si->tXMin = sXMin;
      si->tDist = (si->tp[7 - sXMin].p - si->t).Length();
      if (!allOK) {
        si->tType = LXPE_ROUND;
      }
    }



    // spocitajme fPlane
    allOK = true;
    sPlane.Init(si->f, si->sDir);
    for(i = 0; i < 8; i++) {
      allOK = allOK && (si->fPlane.CalcIntersection(si->tp[i], si->fp[i], ffp[i]));
      sDistCur = sPlane.CalcPosition(ffp[i]);
      if (i == 0) {
        sDistMax = sDistCur;
        sXMax = 0;
        sDistMin = sDistCur;
        sXMin = 0;
      } else {
        if (sDistCur > sDistMax) {
          sDistMax = sDistCur;
          sXMax = i;
        }
        if (sDistCur < sDistMin) {
          sDistMin = sDistCur;
          sXMin = i;
        }
      }
    }
    if (sXMax != sXMin) {
      nsDir = ffp[sXMax] - si->fp[sXMin];
      if (nsDir.Norm() > 0)
        nsDir.Normalize();
      else
        allOK = false;
      si->nfDir = nsDir ^ si->fDir;
      si->nfDir = nsDir ^ si->nfDir;
      if ((si->nfDir * si->sDir) < 0)
        si->nfDir *= -1.0;
      else if ((si->nfDir * si->sDir) == 0.0)
        allOK = false;

      si->nfDir.Normalize();
    } else {
      si->nfDir = si->sDir;
    }
    si->nfPlane.Init(ffp[sXMax], si->nfDir);
    for(i = 0; i < 8; i++) {
      allOK = allOK && si->nfPlane.CalcIntersection(si->tp[i], si->fp[i], si->nfp[i]);
    }
    
    if (si->fType == LXPE_CONT) {
      si->fXMin = sXMax;
      si->fDist = (si->fp[7 - sXMax].p - si->f).Length();
      if (!allOK) {
        si->fType = LXPE_ROUND;
      }
    }
  }

  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);
    if ((si->fType == LXPE_ROUND) && (si->prev != NULL)) {
      si->prev->tType = LXPE_ROUND;
    }
    if ((si->tType == LXPE_ROUND) && (si->next != NULL)) {
      si->next->fType = LXPE_ROUND;
    }
  }


  for(si = series.begin(); si != series.end(); si++) {

    // teraz nastavime body tak, ako ich mame vo vystupe
    allOK = false;
    while (((si->tType == LXPE_CONT) || (si->fType == LXPE_CONT)) && (!allOK)) {

      // 1. Nastavime body
      for(i = 0; i < 8; i++) {
        if (si->fType == LXPE_CONT) 
          ffp[i] = si->nfp[i];
        else
          ffp[i] = si->fp[i];
        if (si->tType == LXPE_CONT) 
          ttp[i] = si->ntp[i];
        else
          ttp[i] = si->tp[i];
      }

      // 2. Skontrolujeme ci sedia smery a to, ci body nelezia mimo
      //    koncovych rovin
      allOK = true;
      for (i = 0; i < 8; i++) {
        if (((ttp[i] - ffp[i]) * si->ft) < 0.0) {
          allOK = false;
          break;
        }
      }

      if (!allOK) {
        if ((si->fType == LXPE_CONT) && (si->tType == LXPE_CONT)) {
          if ((si->sDir * si->nfDir) < (si->sDir * si->ntDir)) {
            si->fType = LXPE_ROUND;
            si->prev->tType = LXPE_ROUND;
          } else {
            si->tType = LXPE_ROUND;
            si->next->fType = LXPE_ROUND;
          }
        } else if (si->fType == LXPE_CONT) {
          si->fType = LXPE_ROUND;
          si->prev->tType = LXPE_ROUND;
        } else if (si->tType == LXPE_CONT) {
          si->tType = LXPE_ROUND;
          si->next->fType = LXPE_ROUND;
        }
      }

    } // koniec checkovania

  }

  // calculate points
  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);

    if (si->tType == LXPE_CONT) {
      for(i = 0; i < 8; i++) {
        si->tp[i] = si->ntp[i];
      }
    }
    if (si->fType == LXPE_CONT) {
      for(i = 0; i < 8; i++) {
        si->fp[i] = si->nfp[i];
      }
    }

  }

  // if points are too close to each other, reduce geometry
  double avDist;
  int ti, fi;
  for(si = series.begin(); si != series.end(); si++) {
    if (si->tType == LXPE_CONT) {
      for(i = 0; i < 8; i++) {
        ti = (i + si->tXMin) % 8;
        fi = (i + si->next->fXMin) % 8;
        avDist = (si->tp[ti].p - si->tp[(ti + 7) % 8].p).Length() + (si->tp[ti].p - si->tp[(ti + 1) % 8].p).Length();
        avDist += (si->next->fp[fi].p - si->next->fp[(fi + 7) % 8].p).Length() + (si->next->fp[fi].p - si->next->fp[(fi + 1) % 8].p).Length();
        avDist *= 0.25;
        if ((avDist > 0.0) && ((si->next->fp[fi].p - si->tp[ti].p).Length() / avDist < 0.25)) {
          si->tp[ti].p = 0.5 * (si->next->fp[fi].p + si->tp[ti].p);
          si->next->fp[fi].p = si->tp[ti].p;
        }
      }
    }
  }

  // calculate normals
  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);
    for(i = 0; i < 8; i++) {
      si->tp[i].n = lxCalcNormal4(si->tp[(i + 1) % 8].p, si->fp[i].p, si->tp[(i + 7) % 8].p, si->tp[i].p);
      si->fp[i].n = lxCalcNormal4(si->fp[(i + 7) % 8].p, si->tp[i].p, si->fp[(i + 1) % 8].p, si->fp[i].p);
    }
  }

  lxVec nnorm;
  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);
    if (si->tType == LXPE_CONT) {
      for(i = 0; i < 8; i++) {
        ti = (i + si->tXMin) % 8;
        fi = (i + si->next->fXMin) % 8;
        nnorm = si->tp[i].n + si->next->fp[i].n;
        nnorm.Normalize();
        si->tp[i].n = nnorm;
        si->next->fp[i].n = nnorm;
      }
    }
  }


  // insert geometry
  // COUNTER NEEDED
  lxTriGeom so;
  for(si = series.begin(); si != series.end(); si++) {
    psi = &(*si);

    if (si->fType != LXPE_CONT) {
      this->o.Append(&so);
      so.Clear();
    }

    si->fp[8].p = lxVec(0.0, 0.0, 0.0);
    si->tp[8].p = lxVec(0.0, 0.0, 0.0);
    for(i = 0; i < 8; i++) {    
      si->fp[8].p += si->fp[i];
      si->tp[8].p += si->tp[i];
    }
    si->fp[8].p *= 0.125;
    si->fp[8].n = -1.0 * si->sDir;
    si->tp[8].p *= 0.125;
    si->tp[8].n = si->sDir;

    double dl;
    lxTriGeomPoint ddt, ddp[8];
    double diam;
    switch (si->tType) {
      case LXPE_ROUND:
        //if (si->tDist <= 0.0) {
          diam = (si->tp[0].p - si->tp[8].p).Length();
          for(i = 1; i < 8; i++) {
            dl = (si->tp[i].p - si->tp[8].p).Length();
            if (diam > dl) diam = dl;
          }
        //} else {
        //  diam = si->tDist;
        //}
        ddt = si->tp[8].p + diam * si->sDir;
        ddt.n = si->tp[8].n;
        for (i = 0; i < 8; i++) {
          ddp[i] = si->tp[8] + 0.70710678 * (si->tp[i] - si->tp[8]) + 0.70710678 * diam * si->sDir;
        }
        for (i = 0; i < 8; i++) {
          nnorm = lxCalcNormal4(si->tp[(i + 7) % 8].p, ddp[i].p, si->tp[(i + 1) % 8].p, si->tp[i].p);
          nnorm += si->tp[i].n;
          nnorm.Normalize();
          si->tp[i].n = nnorm;
          ddp[i].n = lxCalcNormal5(ddp[(i+1) % 8], si->tp[i], ddp[(i+7) % 8], ddt, ddp[i]);
        }
        for (i = 0; i < 8; i++) {
          so.Insert3Angle(ddt, ddp[i], ddp[(i + 1) % 8]);
          so.Insert4Angle(ddp[i], si->tp[i], ddp[(i + 1) % 8], si->tp[(i + 1) % 8]);
        }
        break;
      case LXPE_FLAT:
        for(i = 0; i < 8; i++) {
          ddp[i].p = si->tp[i].p;
          ddp[i].n = si->tp[8].n;
        }
        for(i = 0; i < 8; i++) {
          so.Insert3Angle(si->tp[8], ddp[i], ddp[(i + 1) % 8]);
        }
        break;
    }
    switch (si->fType) {
      case LXPE_ROUND:
        //if (si->fDist <= 0.0) {
          diam = (si->fp[0].p - si->fp[8].p).Length();
          for(i = 1; i < 8; i++) {
            dl = (si->fp[i].p - si->fp[8].p).Length();
            if (diam > dl) diam = dl;
          }
        //} else {
        //  diam = si->fDist;
        //}
        ddt = si->fp[8].p - diam * si->sDir;
        ddt.n = si->fp[8].n;
        for (i = 0; i < 8; i++) {
          ddp[i].n = ddp[i].p - si->fp[8].p;
          ddp[i].n.Normalize();
        }
        for (i = 0; i < 8; i++) {
          ddp[i] = si->fp[8] + 0.70710678 * (si->fp[i] - si->fp[8]) - 0.70710678 * diam * si->sDir;
        }
        for (i = 0; i < 8; i++) {
          nnorm = lxCalcNormal4(si->fp[(i + 1) % 8].p, ddp[i].p, si->fp[(i + 7) % 8].p, si->fp[i].p);
          nnorm += si->fp[i].n;
          nnorm.Normalize();
          si->fp[i].n = nnorm;
          ddp[i].n = lxCalcNormal5(ddp[(i+1) % 8], ddt, ddp[(i+7) % 8], si->fp[i], ddp[i]);
        }
        for (i = 0; i < 8; i++) {
          so.Insert3Angle(ddt, ddp[(i + 1) % 8], ddp[i]);
          so.Insert4Angle(si->fp[i], ddp[i], si->fp[(i + 1) % 8], ddp[(i + 1) % 8]);
        }
        break;
      case LXPE_FLAT:
        for(i = 0; i < 8; i++) {
          ddp[i].p = si->fp[i].p;
          ddp[i].n = si->fp[8].n;
        }
        for(i = 0; i < 8; i++) {
          so.Insert3Angle(si->fp[8], ddp[(i + 1) % 8], ddp[i]);
        }
        break;
    }

    if (si->tType == LXPE_CONT) {
      for(i = 0; i < 8; i++) {
        ti = (i + si->tXMin) % 8;
        fi = (i + si->next->fXMin) % 8;
        so.Insert4Angle(si->next->fp[fi], si->tp[ti], si->next->fp[(fi + 1) % 8], si->tp[(ti + 1) % 8]);
      }
    }

    for(i = 0; i < 8; i++) {
      so.Insert4Angle(si->tp[i], si->fp[i], si->tp[(i + 1) % 8], si->fp[(i + 1) % 8]);
    }

  }

  this->o.Append(&so);

  delete [] edges;
  delete [] nodes;

  //size_t nT = this->o.GetNTriangles();
  //size_t nP = this->o.GetNPoints();

}


long lxLRUD::GetPointsN() 
{
  return long(this->o.GetNPoints());
}

void lxLRUD::GetPoint(long idx, double * coord, double * norm)
{
  lxTriGeomPoint pt = this->o.GetPoint(idx);
  coord[0] = pt.p.x;
  coord[1] = pt.p.y;
  coord[2] = pt.p.z;
  norm[0] = pt.n.x;
  norm[1] = pt.n.y;
  norm[2] = pt.n.z;
}

long lxLRUD::GetTrianglesN()
{
  return long(this->o.GetNTriangles());
}

void lxLRUD::GetTriangle(long idx, long * vertices)
{
  lxTriGeom3Angle tg = this->o.GetTriangle(idx);
  vertices[0] = long(tg.v1);
  vertices[1] = long(tg.v2);
  vertices[2] = long(tg.v3);
}
