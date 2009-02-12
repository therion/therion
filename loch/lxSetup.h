/**
 * @file lxSetup.h
 * Loch model setup.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

#ifndef lxSetup_h
#define lxSetup_h

// Standard libraries
#ifndef LXDEPCHECK
#endif  
//LXDEPCHECK - standart libraries


#include "lxData.h"
#include "lxMath.h"

enum {
  lxSETUP_COLORMD_DEFAULT,
  lxSETUP_COLORMD_ALTITUDE,
};

struct lxSetup {

  // data settings
  lxData * data;
  lxVecLimits data_limits;

  // camera settings
  lxVec cam_center, cam_pos,
    cam_orig_center, cam_orig_pos;
  int cam_anaglyph_glasses, m_colormd;
  double cam_dist, cam_dir, cam_tilt, cam_width,
    cam_orig_dist, cam_orig_dir, cam_orig_tilt, cam_lens, cam_lens_vfov, cam_lens_vfovr, cam_anaglyph_eyesep,
    data_limits_diam;
  bool cam_persp, cam_anaglyph, cam_anaglyph_bw, cam_anaglyph_left;

  bool m_vis_centerline, m_vis_walls, m_vis_surface, 
       m_vis_labels, m_vis_bbox, m_vis_grid, m_vis_indicators,
       m_vis_centerline_cave, m_vis_centerline_surface, m_vis_centerline_splay,
       m_vis_centerline_station, m_vis_centerline_entrance, m_vis_centerline_fix, m_vis_centerline_duplicate,
       m_stlabel_comment, m_stlabel_name, m_stlabel_altitude, m_stlabel_survey;
  bool m_srf_transparency, m_srf_texture;
  bool m_walls_transparency;
  bool m_colormd_app_centerline,
    m_colormd_app_walls;
  double m_srf_opacity, m_walls_opacity;
  
  lxSetup(struct lxData * dat);

  //  Upravi data na zaklade settings a potom updatne
  //  sam seba.
  void UpdateData();

  //  Restne pohlad podla prave nastaveneho dir a tilt,
  //  ostatne da tak, aby data_limits vyplnali obrazovku
  void ResetCamera();
  
  void StartCameraMovement();
  void ZoomCamera(double zoom);
  void PanCamera(double panx, double pany);
  void TiltCamera(double tilt);
  void RotateCamera(double rot);
	void RotateCameraF(double rot);
  void SetLens(double lens);
  void UpdatePos();
  
};


#endif
