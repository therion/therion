/**
 * @file lxSStats.h
 * Survey statistics.
 */
  
/* Copyright (C) 2022 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

#ifndef lxSStats_h
#define lxSStats_h


// Standard libraries
#ifndef LXDEPCHECK
#include <wx/minifram.h>
#include <wx/textctrl.h>
#endif  
//LXDEPCHECK - standard libraries


#include "lxWX.h"



class lxSurveyStatsDlg : public wxMiniFrame
{
public:

  lxTBoxPos m_toolBoxPosition;
  class lxFrame * m_mainFrame;
  wxTextCtrl * m_statsControl;

  lxSurveyStatsDlg(wxWindow *parent);
  void LoadData();

  void OnCommand(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);

private:
  DECLARE_EVENT_TABLE()
};


#endif

