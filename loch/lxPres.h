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

#include <wx/minifram.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/xml/xml.h>

#include "lxWX.h"

enum {
	LXPRES_POSLB,
};

class lxPresentDlg : public wxMiniFrame
{
public:

  lxTBoxPos m_toolBoxPosition;
  wxString m_fileName, m_fileDir;
  bool m_changed = {};
  class lxFrame * m_mainFrame;
  class wxListCtrl * m_posLBox;
  class wxButton * m_playButton = NULL;

  lxPresentDlg(wxWindow *parent);

  void OnCommand(wxCommandEvent& event);
  void OnListItemSelected(wxListEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);

  void ResetPresentation(bool save = false);
  bool SavePresentation(bool saveas = false);
  void LoadPresentation();
  void UpdateList();
  void UpdateControls();
  void MarkCurrentView();
  void SelectScene(long index);
  void ExportPresentation();
  void EditOptions();

private:
  long GetSelection();
  wxXmlNode * GetScene(long index);
  wxString GetSceneLabel(wxXmlNode * n, long index);
  wxString GetSceneDuration(wxXmlNode * n);
  wxString GetSceneRotations(wxXmlNode * n);
  wxString GetSceneRotationDuration(wxXmlNode * n);
  bool GetLoopAnimation();
  bool GetSceneChanges();
  void SetLoopAnimation(bool value);
  void SetSceneChanges(bool value);
  void ApplySceneChanges(wxXmlNode * n);
  void EditSelected();

  DECLARE_EVENT_TABLE()
};


#endif
