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

#include <wx/xml/xml.h>

#include "lxMath.h"
#include <set>

struct lxData;

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
  double cam_dist = 0.0, cam_dir, cam_tilt, cam_width = 0.0,
    cam_orig_dist = 0.0, cam_orig_dir = 0.0, cam_orig_tilt = 0.0, cam_lens = 0.0, cam_lens_vfov = 0.0, cam_lens_vfovr = 0.0, cam_anaglyph_eyesep,
    data_limits_diam = 0.0;
  bool cam_persp, cam_anaglyph, cam_anaglyph_bw, cam_anaglyph_left;

  bool m_vis_centerline = false, m_vis_walls, m_vis_surface, 
       m_vis_labels, m_vis_bbox, m_vis_grid, m_vis_indicators,
       m_vis_centerline_cave, m_vis_centerline_surface, m_vis_centerline_splay,
       m_vis_centerline_station, m_vis_centerline_entrance, m_vis_centerline_fix, m_vis_centerline_duplicate,
       m_stlabel_comment, m_stlabel_name, m_stlabel_altitude, m_stlabel_survey;
  bool m_srf_transparency, m_srf_texture, m_srf_lighting;
  bool m_walls_transparency;
  bool m_colormd_app_centerline,
    m_colormd_app_walls;
  double m_srf_opacity, m_walls_opacity;

  std::set<std::string> m_survey_selection;
  
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

  void SelectSurvey(const char * survey);
  bool IsSurveySelected(const char * survey);
  void ClearSurveySelection();
  
  void SaveToXMLNode(wxXmlNode * n);
  void LoadFromXMLNode(wxXmlNode * n, wxXmlNode * nn = NULL, double t = 0.0);

};


#endif
