/**
 * @file lxSTree.h
 * Loch scene setup.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

#ifndef lxSTree_h
#define lxSTree_h

#include <wx/minifram.h>
#include <wx/treectrl.h>

#include "lxWX.h"



class lxModelTreeDlg : public wxMiniFrame
{
public:

  lxTBoxPos m_toolBoxPosition;
  class lxFrame * m_mainFrame;
  wxTreeCtrl * m_treeControl;

  lxModelTreeDlg(wxWindow *parent);
  void LoadData();

  void OnCommand(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);

private:
  DECLARE_EVENT_TABLE()
};


#endif

