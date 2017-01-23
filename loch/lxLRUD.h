#ifndef lxLRUD_h
#define lxLRUD_h

// Standard libraries
#ifndef LXDEPCHECK
#endif  
//LXDEPCHECK - standard libraries

#include "lxMath.h"

enum {
  LXLRUD_OCTAGON,
  LXLRUD_TUNNEL,
  LXLRUD_DIAMOND,
  LXLRUD_SQUARE,
};


struct lxLRUDShot {
  lxVec from, to;
  long xfrom, xto;
  int profile;
  double fl, fr, fu, fd, tl, tr, tu, td, vthresh;
  void * usr_data;
  lxLRUDShot() : profile(LXLRUD_OCTAGON), vthresh(60.0) {}
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
  void InsertShot(double * from, double * to, double * fromLRUD, double * toLRUD, void * usrData);
  void Calculate();
  long GetPointsN();
  void GetPoint(long idx, double * coord, double * norm);
  long GetTrianglesN();
  void GetTriangle(long idx, long * vertices);
};

#endif


