#ifndef lxLRUD_h
#define lxLRUD_h

// Standard libraries
#ifndef LXDEPCHECK
#endif  
//LXDEPCHECK - standard libraries

#include "lxTriGeom.h"

#include <any>

enum {
  LXLRUD_OCTAGON,
  LXLRUD_TUNNEL,
  LXLRUD_DIAMOND,
  LXLRUD_SQUARE,
};


struct lxLRUDShot {
  lxVec from, to;
  long xfrom = 0, xto = 0;
  int profile = LXLRUD_OCTAGON;
  double fl = 0.0, fr = 0.0, fu = 0.0, fd = 0.0, tl = 0.0, tr = 0.0, tu = 0.0, td = 0.0, vthresh = 60.0;
  std::any usr_data;
  lxLRUDShot() = default;
};


struct lxLRUDData {
  long nstations;
  std::list <lxLRUDShot> shots;
  std::map <lxVec, long> station_map;
  double currentVT;
  int currentProfile;

  lxLRUDData() : nstations(0), currentVT(60.0), currentProfile(LXLRUD_OCTAGON) {}
  long GetStationID(lxVec pos, bool insert = false);
};


struct lxLRUD {
  lxLRUDData i;
  lxTriGeom o;
  void SetVThreshold(double vt);
  void SetProfile(int sec_type);
  void InsertShot(double * from, double * to, double * fromLRUD, double * toLRUD, std::any usrData = {});
  void Calculate();
  long GetPointsN();
  void GetPoint(long idx, double * coord, double * norm);
  long GetTrianglesN();
  void GetTriangle(long idx, long * vertices);
};

#endif


