/**
 * @file lxSView.h
 * Loch scene setup.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

#ifndef lxSView_h
#define lxSView_h

#include <wx/minifram.h>

#include "lxWX.h"

enum {
  LXVSTP_CONTROLLB = 10000,
	LXVSTP_PERSP,
	LXVSTP_ZOOM,
	LXVSTP_ZOOMSLIDE,
	LXVSTP_FACING,
	LXVSTP_FACINGSLIDE,
	LXVSTP_TILT,
	LXVSTP_TILTSLIDE,
	LXVSTP_DIST,
	LXVSTP_DISTSLIDE,
	LXVSTP_CENTERX,
	LXVSTP_CENTERY,
	LXVSTP_CENTERZ,
  LXVSTP_ROTATION,
  LXVSTP_ROTLOCK,
  LXVSTP_ROTSPEED,
  LXVSTP_STEREO,
  LXVSTP_STEREOBW,
  LXVSTP_3DGLASSES,
  LXVSTP_EYESEP,
  LXVSTP_RENSPEED,
};


class lxViewpointSetupDlg : public wxMiniFrame
{
public:

  lxTBoxPos m_toolBoxPosition;
  lxCRC32 m_toolBoxCRC;
  class lxFrame * m_mainFrame;
	wxBoxSizer * m_controlSizer, 
		* m_controlSizer_Orientation,
		* m_controlSizer_Center,
		* m_controlSizer_Camera,
    * m_controlSizer_Rotation,
    * m_controlSizer_Stereo;

  bool m_uicFacing,
    m_uicTilt,
    m_uicCenterX,
    m_uicCenterY,
    m_uicCenterZ,
    m_uicDist,
    m_uicZoom;
	
	lxViewpointSetupDlg(wxWindow *parent);

  bool IsChanged();
  void LoadSetup();
  void SetSliders();

  void OnCommand(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);
	void OnControlSelect(wxCommandEvent& event);
  void OnSlider(wxScrollEvent& event);
	void OnText(wxCommandEvent& event);

private:
  DECLARE_EVENT_TABLE()
};





#endif

