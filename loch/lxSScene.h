/**
 * @file lxSScene.h
 * Loch scene setup.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

#ifndef lxSScene_h
#define lxSScene_h


// Standard libraries
#ifndef LXDEPCHECK
#include <wx/minifram.h>
#endif  
//LXDEPCHECK - standard libraries


#include "lxWX.h"



enum {
	LXMSTP_CONTROLLB,
};


class lxModelSetupDlg : public wxMiniFrame
{
public:

  lxTBoxPos m_toolBoxPosition;
  class lxFrame * m_mainFrame;
	wxBoxSizer * m_controlSizer, 
		* m_controlSizer_ColorMode,
		* m_controlSizer_Visibility,
		* m_controlSizer_Centerline,
		* m_controlSizer_Walls,
		* m_controlSizer_Surface,
		* m_controlSizer_Labels,
		* m_controlSizer_BBox2Grid = nullptr,
		* m_controlSizer_Indicators,
		* m_controlSizer_ColorSetup = nullptr;

	lxModelSetupDlg(wxWindow *parent);

  void LoadSetup();
  void InitSetup();

  void OnSlider(wxScrollEvent& event);
  void OnCommand(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
	void OnMove(wxMoveEvent& event);
	void OnControlSelect(wxCommandEvent& event);

private:
  DECLARE_EVENT_TABLE()
};


#endif

