/**
 * @file lxPres.h
 * Loch scene setup.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

#ifndef lxPres_h
#define lxPres_h


// Standard libraries
#ifndef LXDEPCHECK
#include <wx/minifram.h>
#include <wx/treectrl.h>
#endif  
//LXDEPCHECK - standart libraries


#include "lxWX.h"

enum {
	LXPRES_POSLB,
};

class lxPresentDlg : public wxMiniFrame
{
public:

  lxTBoxPos m_toolBoxPosition;
  wxString m_fileName, m_fileDir;
  bool m_changed;
  class lxFrame * m_mainFrame;
  class wxListBox * m_posLBox;

  lxPresentDlg(wxWindow *parent);

  void OnCommand(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);

  void ResetPresentation(bool save = false);
  bool SavePresentation(bool saveas = false);
  void LoadPresentation();
  void UpdateList();
  void UpdateControls();

private:
  DECLARE_EVENT_TABLE()
};


#endif

