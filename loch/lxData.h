/**
 * @file lxData.h
 * Loch model data.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
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

#ifndef lxData_h
#define lxData_h

// Standard libraries
#ifndef LXDEPCHECK
#include <vtkPolyData.h>
#include <vtkLookupTable.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkDepthSortPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <wx/string.h>
#include <vector>
#endif  
//LXDEPCHECK - standart libraries


#include "lxMath.h"
#include "lxImgIO.h"
#include "lxImgIO.h"
#include "lxFile.h"


struct lxDataStation {

  lxVec pos;
  
};

struct lxDataTexture {
  double dx, dy, xx, xy, yx, yy, iw, ih;
  unsigned texSizeS, texSizeO;
  lxImageRGB image;
  unsigned char * texS, * texSbw, * texO, * texObw;
  lxDataTexture() : dx(0.0), dy(0.0), xx(1.0), xy(0.0), yx(0.0), yy(1.0), iw(1.0), ih(1.0), 
    image(), texS(NULL), texSbw(NULL), texO(NULL), texObw(NULL) {}
  ~lxDataTexture();
  bool SetImage(lxImageRGB img);
  void Clear();
  void ClearTexImages();
  void CreateTexImages(int sizeS, int sizeO);
  bool InitCalibInverse(double idx, double idy, double ixx, double ixy, double iyx, double iyy);
  bool InitCalibNormal(double idx, double idy, double ixx, double ixy, double iyx, double iyy);
};

struct lxDataShot {

  unsigned long from, to;

  bool surface, invisible;
  
};

typedef std::vector<lxDataStation> lxDataStationVec;
typedef std::vector<lxDataShot> lxDataShotVec;

struct lxData {

  lxFile m_input;

  lxDataStationVec stations;

  lxDataShotVec shots;

  wxString title;
  
  vtkPolyData * scrapWalls, * lrudWalls, * surface, * m_centerline;
  vtkTriangleFilter * allWallsTriangle, * surfaceTriangle;
  vtkDepthSortPolyData * allWallsSorted, * surfaceSorted;
  vtkAppendPolyData * allWalls;
  vtkStripper * allWallsStripped;
  vtkPolyDataNormals * scrapWallsNormals, * surfaceNormals;
  vtkLookupTable * luTable;

  lxDataTexture m_textureSurface;

  lxData();

  ~lxData();
  
  void Clear();
  void Rebuild();
  void InitTextures();
  void ExportVTK(wxString fileName);
  
};


#endif



