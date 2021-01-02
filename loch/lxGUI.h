/**
 * @file lxGUI.h
 * Loch user interface.
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

#ifndef lxGUI_h
#define lxGUI_h

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/wx.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/docview.h>
#include <wx/xml/xml.h>
#endif  
//LXDEPCHECK - standard libraries


#include "lxGLC.h"

enum {
  LXMENU_CAMERA_ADJUST = 1000,
  LXMENU_CAMERA_ROTCW,
  LXMENU_CAMERA_ROTCCW,
  LXMENU_CAMERA_UPPER,
  LXMENU_CAMERA_LOWER,
  LXMENU_CAMERA_UP,
  LXMENU_CAMERA_DOWN,
  LXMENU_CAMERA_LEFT,
  LXMENU_CAMERA_RIGHT,
  LXMENU_CAMERA_ZOOMIN,
  LXMENU_CAMERA_ZOOMOUT,
  LXMENU_CAMERA_ORIENT,
  LXMENU_CAMERA_ORIENT_HOME,
  LXMENU_CAMERA_ORIENT_PLAN,
  LXMENU_CAMERA_ORIENT_PROFILE,
  LXMENU_CAMERA_ORIENT_NORTH,
  LXMENU_CAMERA_ORIENT_SOUTH,
  LXMENU_CAMERA_ORIENT_EAST,
  LXMENU_CAMERA_ORIENT_WEST,
  LXMENU_CAMERA_EXTENDS,
  LXMENU_CAMERA_DEFAULTS,
  LXMENU_CAMERA_PERSP,
  LXMENU_CAMERA_AUTOROTATE,
  LXMENU_CAMERA_LOCKROT,
	LXMENU_VIEW,
  LXMENU_VIEW_FULLSCREEN,
  LXMENU_VIEW_MODELSTP,
  LXMENU_VIEW_SELECTIONSTP,
  LXMENU_VIEW_VIEWPOINTSTP,
  LXMENU_VIEW_PRESENTDLG,
  LXMENU_TOOLS_OPTIONS,
	LXMENU_VIEWEND,
  LXMENU_EDIT_UNDO,
  LXMENU_EDIT_REDO,
  LXMENU_FILE_OPEN,
  LXMENU_FILE_RELOAD,
  LXMENU_FILE_RENDER,
  LXMENU_FILE_RENDER_SETUP,
  LXMENU_FILE_EXPORT,
  LXMENU_FILE_IMPORT,
  LXMENU_HELP_CONTENTS,
  LXMENU_HELP_CONTROL,
  LXMENU_HELP_RENDERING,
  LXMENU_HELP_BUGS,
  LXMENU_HELP_ABOUT,
  LXMENU_EXPFIT,
  LXMENU_EXPROT,
  LXMENU_PRES,
  LXMENU_PRESMARK,
  LXMENU_PRESUPDATE,
  LXMENU_PRESMOVEUP,
  LXMENU_PRESMOVEDOWN,
  LXMENU_PRESDELETE,
  LXMENU_PRESNEW,
  LXMENU_PRESOPEN,
  LXMENU_PRESSAVE,
  LXMENU_PRESSAVEAS,
  LXMENU_PRESACTIVATE,
  LXMENU_PRESEDIT,
  LXMENU_PRES_END,
	LXTB,
	LXTB_OPEN,
	LXTB_RELOAD,
	LXTB_RENDER,
	LXTB_RENDER_SETUP,
	LXTB_VIEWPOINT,
	LXTB_PERSP,
	LXTB_ROTATION,
	LXTB_LOCKROT,
  LXTB_PLAN,
  LXTB_PROFILE,
	LXTB_FIT,
  LXTB_HOME,
	LXTB_FULLSCREEN,
	LXTB_STEREO,
	LXTB_VIEWSTP,
	LXTB_SCENESTP,
  LXTB_VISCENTERLINE,
  LXTB_VISWALLS,
  LXTB_VISSURFACE,
  LXTB_VISBBOX,
  LXTB_VISINDS,
  LXTB_VISENTRANCE,
  LXTB_VISFIX,
  LXTB_VISSTATION,
  LXTB_VISLABEL,
	LXTBEND,
};


enum {
  LXWALLS_INTERP_NONE,
  LXWALLS_INTERP_ALL_ONLY,
  LXWALLS_INTERP_MISSING,
};

enum {
  LXUNITS_METRIC,
  LXUNITS_IMPERIAL,
};


class lxFrame: public wxFrame
{

  public:

    lxGLCanvas * canvas;
    wxString m_fileDir, m_fileToOpen, m_fileName;
    int m_fileType;
    
    struct lxData * data;
    struct lxSetup * setup;
    struct lxRenderData * m_renderData;
    wxXmlDocument * m_pres;


    class lxModelSetupDlg * m_modelSetupDlg;
    bool m_modelSetupDlgOn;

    class lxModelTreeDlg * m_selectionSetupDlg;
    bool m_selectionSetupDlgOn;
    class lxPresentDlg * m_presentationDlg;
    bool m_presentationDlgOn;

		class lxViewpointSetupDlg * m_viewpointSetupDlg;
    bool m_viewpointSetupDlgOn;

    wxMenuBar * m_menuBar;
		wxToolBar * m_toolBar;
		wxMenu * m_viewpointMenu, * m_toolMenu;
    wxAcceleratorTable m_menuAccelTable;

    wxFileConfig * m_fileConfig;
    wxFileHistory * m_fileHistory;
    wxString m_iniDirectory;

    long m_iniUnits;
    int m_iniStereoGlasses;
    int m_iniStereoGlassesLast;
    long m_iniWallsInterpolate;

    class lxApp * m_app;

    class wxHelpController * m_helpController;

    lxFrame(class lxApp * app, const wxString& title, const wxPoint& pos,
      const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
      
    virtual ~lxFrame(); 
    void OnExit(wxCommandEvent& event);

    void OnAll(wxCommandEvent& event);
    void OnMenuCameraMove(wxCommandEvent& event);
    void OnMenuCameraOrient(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
		
    void TogglePerspective();
    void ToggleStereo();
    void ToggleStereoBW();
		void ToggleRotation();
		void ToggleRotLock();
    void ToggleFullScreen();
    void ToggleModelSetup();
    void ToggleSelectionSetup();
    void TogglePresentationDlg();
    void ToggleViewpointSetup();

    void ToggleVisibilityCenterline();
    void ToggleVisibilityCenterlineCave();
    void ToggleVisibilityCenterlineSurface();
    void ToggleVisibilityCenterlineSplay();
    void ToggleVisibilityCenterlineDuplicate();
    void ToggleVisibilityCenterlineFix();
    void ToggleVisibilityCenterlineStation();
    void ToggleVisibilityCenterlineEntrance();
    void ToggleVisibilitySurface();
    void ToggleVisibilityWalls();
    void ToggleVisibilityLabels();
    void ToggleVisibilityBBox();
    void ToggleVisibilityGrid();
    void ToggleVisibilityIndicators();
    void ToggleVisibilityStLabelName();
    void ToggleVisibilityStLabelComment();
    void ToggleVisibilityStLabelAltitude();
    void ToggleVisibilityStLabelSurvey();
    void ExportRotationPictures();

    void SetColorMode(int);
    void DetectFileType();
    int GetFileType(wxString fName);
    void ToggleColorsApplyCenterline();
    void ToggleColorsApplyWalls();

    void ToggleSurfaceTexture();
    void ToggleSurfaceTransparency();
    void ToggleSurfaceLighting();

    void ToggleWallsTransparency();

    void SetupUpdate();
    void SetupApply();
    void OpenFile(const wxString & fName);
    void ImportFile(const wxString fName, int fType);

    void LoadData(wxString fName, int fType);
    void ReloadData();
		void UpdateM2TB();

    DECLARE_EVENT_TABLE()
    
}; // lxFrame



class lxApp: public wxGLApp
{

  public:
  
    class lxFrame * frame;

    wxLocale m_locale;
    wxFileName m_path;
  
    bool OnInit() override;
    
#ifdef LXMACOSX
    virtual void MacOpenFile(const wxString &fileName);
#endif    
    
}; // lxApp

#endif


