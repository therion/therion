/**
 * @file lxGUI.cxx
 * Loch user interface implementation.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

// Standard libraries
#ifndef LXDEPCHECK

#include <wx/wx.h>
#include <wx/utils.h>
#include <wx/filedlg.h>
#include <wx/toolbar.h>
#include <wx/filename.h>
#include <wx/busyinfo.h>
#include <wx/help.h>
#include <wx/fs_zip.h>
#include <wx/dnd.h>
#include <wx/display.h>

#include <vtkObject.h>

#endif  
//LXDEPCHECK - standart libraries

#include "lxGUI.h"
#include "lxData.h"
#include "lxSetup.h"
#include "lxRender.h"
#include "lxOptDlg.h"
#include "lxAboutDlg.h"
#include "lxSView.h"
#include "lxSScene.h"

#include "icons/open.xpm"
#include "icons/render.xpm"
#include "icons/reload.xpm"
#include "icons/stereo.xpm"
#include "icons/rotation.xpm"
#include "icons/lockrot.xpm"
#include "icons/fit.xpm"
#include "icons/home.xpm"
#include "icons/rendersetup.xpm"
#include "icons/plan.xpm"
#include "icons/profile.xpm"
#include "icons/fullscreen.xpm"
#include "icons/orto.xpm"
#include "icons/camera.xpm"
#include "icons/scene.xpm"
#include "icons/viscline.xpm"
#include "icons/vissurface.xpm"
#include "icons/visbbox.xpm"
#include "icons/viswalls.xpm"
#include "icons/visinds.xpm"

#ifndef LXGNUMSW
#include "loch.xpm"
#endif




////////////////////////////////////////////////////////////////////////
// Main frame
////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(lxFrame, wxFrame)
    EVT_MENU(wxID_EXIT, lxFrame::OnExit)
    EVT_MENU(LXMENU_CAMERA_ROTCW, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_ROTCCW, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_UPPER, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_LOWER, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_UP, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_DOWN, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_LEFT, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_RIGHT, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_ZOOMIN, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_ZOOMOUT, lxFrame::OnMenuCameraMove)
    EVT_MENU(LXMENU_CAMERA_PERSP, lxFrame::OnAll)
    EVT_MENU(LXMENU_CAMERA_AUTOROTATE, lxFrame::OnAll)
    EVT_MENU(LXMENU_CAMERA_LOCKROT, lxFrame::OnAll)
    EVT_MENU(LXMENU_CAMERA_ORIENT_HOME, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_ORIENT_PLAN, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_ORIENT_PROFILE, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_ORIENT_NORTH, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_ORIENT_WEST, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_ORIENT_EAST, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_ORIENT_SOUTH, lxFrame::OnMenuCameraOrient)
    EVT_MENU(LXMENU_CAMERA_EXTENDS, lxFrame::OnAll)
		EVT_MENU_RANGE(LXMENU_VIEW, LXMENU_VIEWEND, lxFrame::OnAll)
    EVT_MENU(LXMENU_FILE_OPEN, lxFrame::OnAll)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, lxFrame::OnAll)
    EVT_MENU(LXMENU_FILE_RELOAD, lxFrame::OnAll)
    EVT_MENU(LXMENU_FILE_RENDER, lxFrame::OnAll)
    EVT_MENU(LXMENU_FILE_RENDER_SETUP, lxFrame::OnAll)
    EVT_MENU_RANGE(LXMENU_HELP_CONTENTS, LXMENU_HELP_ABOUT, lxFrame::OnAll)
		EVT_TOOL_RANGE(LXTB, LXTBEND, lxFrame::OnAll)
    EVT_SIZE(lxFrame::OnSize)
    EVT_MOVE(lxFrame::OnMove)
END_EVENT_TABLE()





#if wxUSE_DRAG_AND_DROP
class DnDFile : public wxFileDropTarget {

    lxFrame * m_Parent;

  public:
	
    DnDFile(lxFrame *parent) : m_Parent(parent) { }
	
    virtual bool OnDropFiles(wxCoord, wxCoord, const wxArrayString &filenames);

};

bool
DnDFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString &filenames)
{
    assert(filenames.GetCount() > 0);
    m_Parent->OpenFile(filenames[0]);
    return TRUE;
}
#endif

lxFrame::lxFrame(class lxApp * app, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style),
    m_modelSetupDlg(NULL), m_modelSetupDlgOn(false),
    m_viewpointSetupDlg(NULL), m_viewpointSetupDlgOn(false)
{

//    // Give it an icon
#ifdef LXGNUMSW
    this->SetIcon(wxIcon(_T("LOCHICON")));
#else
		this->SetIcon(wxIcon(loch_xpm));
#endif

    this->m_app = app;
    this->data = new lxData;
    this->setup = new lxSetup(data);

    this->m_fileToOpen = _T("");

    this->m_renderData = new lxRenderData();
    this->m_fileName = _("noname");

    this->m_fileConfig = new wxFileConfig(_T("loch"));
    this->m_fileHistory = new wxFileHistory;

    this->m_helpController = new wxHelpController;
    wxString hlpFName = _T("loch"), tmpHlpFName;
#ifdef LXWIN32
    hlpFName += _T(".chm");
#else
    hlpFName += _T(".htb");
#endif
    wxFileName hlpFN;
    hlpFN = wxFileName(this->m_app->m_path);
    hlpFN.AppendDir(this->m_app->m_locale.GetCanonicalName());
    hlpFN.SetFullName(hlpFName);
    if (hlpFN.FileExists()) {
      hlpFName = hlpFN.GetFullPath();
    } else {
      hlpFN = wxFileName(this->m_app->m_path);
      hlpFN.AppendDir(this->m_app->m_locale.GetCanonicalName().Left(2));
      hlpFN.SetFullName(hlpFName);
      if (hlpFN.FileExists()) {
        hlpFName = hlpFN.GetFullPath();
      } else {
        hlpFName = _T("en/loch");
      }
    }
    this->m_helpController->Initialize(hlpFName);

    

    this->m_fileConfig->Read(_T("startup_dir"), &this->m_iniDirectory, wxEmptyString);
    this->m_fileConfig->Read(_T("startup_dir_last"), &this->m_fileDir, ::wxGetCwd());
    if (this->m_iniDirectory.Length() > 0) {
      this->m_fileDir = this->m_iniDirectory;
    }

    this->m_fileConfig->Read(_T("stereo_channels_last"), &this->m_iniStereoGlassesLast, 0);
    this->m_fileConfig->Read(_T("stereo_channels"), &this->m_iniStereoGlasses, 8);
    if (this->m_iniStereoGlasses == 8) 
      this->setup->cam_anaglyph_glasses = this->m_iniStereoGlassesLast;
    else 
      this->setup->cam_anaglyph_glasses = this->m_iniStereoGlasses;

    this->m_fileConfig->Read(_T("wall_extra"), &this->m_iniWallsInterpolate, LXWALLS_INTERP_NONE);




    this->m_toolBar = CreateToolBar();
		this->m_toolBar->AddTool(LXTB_OPEN, _("Open"), wxBitmap(open_xpm), _("Open file"));
		this->m_toolBar->AddTool(LXTB_RELOAD, _("Reload"), wxBitmap(reload_xpm),  _("Reload file"));
		this->m_toolBar->AddSeparator();		
		this->m_toolBar->AddTool(LXTB_RENDER, _("Render"), wxBitmap(render_xpm),  _("Render to file"));
		this->m_toolBar->AddTool(LXTB_RENDER_SETUP, _("Rendering setup"), wxBitmap(rendersetup_xpm),  _("Rendering setup"));
		this->m_toolBar->AddSeparator();		
    this->m_toolBar->AddTool(LXTB_ROTATION, _("Rotation"), wxBitmap(rotation_xpm), _("Rotation"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_LOCKROT, _("Lock rotation"), wxBitmap(lockrot_xpm), _("Lock rotation"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_PERSP, _("Ortho"), wxBitmap(orto_xpm), _("Orthogonal view"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_STEREO, _("Stereo"), wxBitmap(stereo_xpm), _("Stereo mode"), wxITEM_CHECK);
		this->m_toolBar->AddSeparator();		
		this->m_toolBar->AddTool(LXMENU_CAMERA_ORIENT_PLAN, _("Plan"), wxBitmap(plan_xpm),  _("Plan view"));
		this->m_toolBar->AddTool(LXMENU_CAMERA_ORIENT_PROFILE, _("Profile"), wxBitmap(profile_xpm),  _("Profile view"));
		this->m_toolBar->AddTool(LXTB_FIT, _("Fit zoom"), wxBitmap(fit_xpm),  _("Zoom to fit"));
		this->m_toolBar->AddTool(LXMENU_CAMERA_ORIENT_HOME, _("Reset"), wxBitmap(home_xpm), _("Reset viewpoint"));
		this->m_toolBar->AddSeparator();		
		this->m_toolBar->AddTool(LXTB_VISCENTERLINE, _("Centerline"), wxBitmap(viscline_xpm), _("Show centerline"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_VISWALLS, _("Walls"), wxBitmap(viswalls_xpm), _("Show walls"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_VISSURFACE, _("Surface"), wxBitmap(vissurface_xpm), _("Show surface"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_VISBBOX, _("Bounding box"), wxBitmap(visbbox_xpm), _("Show bounding box"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_VISINDS, _("Indicators"), wxBitmap(visinds_xpm), _("Show indicators"), wxITEM_CHECK);
		this->m_toolBar->AddSeparator();		
		this->m_toolBar->AddTool(LXTB_VIEWSTP, _("Camera setup"), wxBitmap(camera_xpm), _("Camera setup"), wxITEM_CHECK);
		this->m_toolBar->AddTool(LXTB_SCENESTP, _("Scene setup"), wxBitmap(scene_xpm), _("Scene setup"), wxITEM_CHECK);
		this->m_toolBar->AddSeparator();		
		this->m_toolBar->AddTool(LXTB_FULLSCREEN, _("Full screen"), wxBitmap(fullscreen_xpm), _("Full screen"), wxITEM_CHECK);
		this->m_toolBar->Realize();

    // Make a menubar
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(LXMENU_FILE_OPEN, _("&Open...\tCtrl+O"));
    fileMenu->Append(LXMENU_FILE_RELOAD, _("&Reload\tCtrl+R"));
    fileMenu->AppendSeparator();
    fileMenu->Append(LXMENU_FILE_RENDER, _("&Render to file\tCtrl+P"));
    fileMenu->Append(LXMENU_FILE_RENDER_SETUP, _("Rendering &setup..."));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _("E&xit\tCtrl+Q"));
    this->m_fileHistory->UseMenu(fileMenu);
    this->m_fileHistory->Load(*this->m_fileConfig);

    wxMenu *cameraAdjustMenu = new wxMenu;
    cameraAdjustMenu->Append(LXMENU_CAMERA_ROTCW, _("Rotate left\tRight"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_ROTCCW, _("Rotate right\tLeft"));
    cameraAdjustMenu->AppendSeparator();
    cameraAdjustMenu->Append(LXMENU_CAMERA_UPPER, _("Upper viewpoint\tDown"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_LOWER, _("Lower viewpoint\tUp"));
    cameraAdjustMenu->AppendSeparator();
    cameraAdjustMenu->Append(LXMENU_CAMERA_UP, _("Move up\tShift+Down"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_DOWN, _("Move down\tShift+Up"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_LEFT, _("Move left\tShift+Right"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_RIGHT, _("Move right\tShift+Left"));
    cameraAdjustMenu->AppendSeparator();
    cameraAdjustMenu->Append(LXMENU_CAMERA_ZOOMIN, _("Zoom in\tCtrl+Up"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_ZOOMOUT, _("Zoom out\tCtrl+Down"));
    cameraAdjustMenu->Append(LXMENU_CAMERA_EXTENDS,_("Zoom to fit\tSpace"));

    wxMenu *cameraOrientMenu = new wxMenu;
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_PLAN,_("&Plan"));
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_PROFILE,_("Pro&file"));
    cameraOrientMenu->AppendSeparator();
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_NORTH, _("&North"));
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_WEST, _("&West"));
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_EAST, _("&East"));
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_SOUTH, _("&South"));
    cameraOrientMenu->AppendSeparator();
    cameraOrientMenu->Append(LXMENU_CAMERA_ORIENT_HOME,_("&Reset"));
		this->m_viewpointMenu = cameraOrientMenu;

    this->m_toolMenu = new wxMenu;
    this->m_toolMenu->AppendCheckItem(LXMENU_CAMERA_AUTOROTATE, _("Rotation"));
    this->m_toolMenu->AppendCheckItem(LXMENU_CAMERA_LOCKROT, _("Lock rotation"));
    this->m_toolMenu->AppendSeparator();
    this->m_toolMenu->AppendCheckItem(LXMENU_VIEW_VIEWPOINTSTP, _("Camera"));
    this->m_toolMenu->AppendCheckItem(LXMENU_VIEW_MODELSTP, _("Scene"));

    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(LXMENU_CAMERA_ADJUST, _("Action"), cameraAdjustMenu);
    viewMenu->AppendCheckItem(LXMENU_CAMERA_AUTOROTATE, _("Rotation"));
    viewMenu->AppendCheckItem(LXMENU_CAMERA_LOCKROT, _("Lock rotation"));
    viewMenu->Append(LXMENU_CAMERA_ORIENT, _("Orientation"), cameraOrientMenu);
    viewMenu->AppendCheckItem(LXMENU_CAMERA_PERSP, _("Orthogonal"));
    viewMenu->AppendSeparator();
    viewMenu->Append(LXMENU_VIEW_FULLSCREEN, _("Full screen\tF11"));

    wxMenu *winMenu = new wxMenu;
    winMenu->AppendCheckItem(LXMENU_VIEW_VIEWPOINTSTP, _("&Camera"));
    winMenu->AppendCheckItem(LXMENU_VIEW_MODELSTP, _("&Scene"));
    winMenu->AppendSeparator();
    winMenu->Append(LXMENU_TOOLS_OPTIONS, _("&Options..."));

    wxMenu * hlpMenu = new wxMenu;
    hlpMenu->Append(LXMENU_HELP_CONTENTS, _("&Contents...\tF1"));
    hlpMenu->Append(LXMENU_HELP_CONTROL, _("&Control"));
    hlpMenu->Append(LXMENU_HELP_RENDERING, _("&Rendering"));
    hlpMenu->Append(LXMENU_HELP_BUGS, _("&Bugs"));
    hlpMenu->AppendSeparator();
    hlpMenu->Append(LXMENU_HELP_ABOUT, _("&About..."));
    
    m_menuBar = new wxMenuBar;
    m_menuBar->Append(fileMenu, _("&File"));
    m_menuBar->Append(viewMenu, _("&View"));
    m_menuBar->Append(winMenu, _("&Tools"));
    m_menuBar->Append(hlpMenu, _("&Help"));

    this->SetMenuBar(m_menuBar);

    this->canvas = new lxGLCanvas(this->setup, this->data, this, wxID_ANY, 
      wxDefaultPosition, wxDefaultSize);  
    

    this->canvas->frame = this;
    
    wxBoxSizer * ms = new wxBoxSizer(wxVERTICAL);
    ms->SetMinSize(192, 128);
    ms->Add(this->canvas, 1, wxEXPAND, 0);

    SetSizer(ms);
    ms->SetSizeHints(this);
    this->SetPosition(pos);
    this->SetSize(size);

    this->m_modelSetupDlg = new lxModelSetupDlg(this);
    this->m_modelSetupDlgOn = false;

    wxSize csize = this->GetClientSize();
    this->m_viewpointSetupDlg = new lxViewpointSetupDlg(this);
#ifdef __WXMSW__
    lxTBoxPos::m_fsOffset = size.y - csize.y;
    this->m_viewpointSetupDlg->m_toolBoxPosition.Set(1, size.x - csize.x, size.y - csize.y);
#else		
    wxSize tbsize = this->m_toolBar->GetSize();
    lxTBoxPos::m_fsOffset = size.y - csize.y + tbsize.y;
    this->m_viewpointSetupDlg->m_toolBoxPosition.Set(1, size.x - csize.x, size.y - csize.y + tbsize.y);
#endif		
    this->m_viewpointSetupDlgOn = false;

		this->UpdateM2TB();

#if wxUSE_DRAG_AND_DROP
    SetDropTarget(new DnDFile(this));
#endif

		// Show the frame
    this->Show(true);
}


void lxFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}



lxFrame::~lxFrame()
{
  this->m_fileHistory->Save(*this->m_fileConfig);
  this->m_fileConfig->Write(_T("startup_dir"), this->m_iniDirectory);
  this->m_fileConfig->Write(_T("startup_dir_last"), this->m_fileDir);
  this->m_fileConfig->Write(_T("stereo_channels_last"), this->m_iniStereoGlassesLast);
  this->m_fileConfig->Write(_T("stereo_channels"), this->m_iniStereoGlasses);
  this->m_fileConfig->Write(_T("wall_extra"), this->m_iniWallsInterpolate);

#ifdef LXVCPPMSW
	SetToolBar(NULL);
#endif
  delete this->m_fileHistory;
  delete this->m_fileConfig;
  delete this->m_toolMenu;
  delete this->setup;
  delete this->data;
  delete this->m_renderData;
  delete this->m_helpController;
}


void lxFrame::OnMenuCameraMove(wxCommandEvent& event){
  wxKeyEvent ke;
  ke.m_keyCode = 0;
  ke.m_shiftDown = false;
  ke.m_controlDown = false;
  switch (event.GetId()) {
    case LXMENU_CAMERA_ROTCW:
      ke.m_keyCode = WXK_RIGHT;
      break;
    case LXMENU_CAMERA_ROTCCW:
      ke.m_keyCode = WXK_LEFT;
      break;
    case LXMENU_CAMERA_UPPER:
      ke.m_keyCode = WXK_DOWN;
      break;
    case LXMENU_CAMERA_LOWER:
      ke.m_keyCode = WXK_UP;
      break;
    case LXMENU_CAMERA_UP:
      ke.m_keyCode = WXK_DOWN;
      ke.m_shiftDown = true;
      break;
    case LXMENU_CAMERA_DOWN:
      ke.m_keyCode = WXK_UP;
      ke.m_shiftDown = true;
      break;
    case LXMENU_CAMERA_LEFT:
      ke.m_keyCode = WXK_RIGHT;
      ke.m_shiftDown = true;
      break;
    case LXMENU_CAMERA_RIGHT:
      ke.m_keyCode = WXK_LEFT;
      ke.m_shiftDown = true;
      break;
    case LXMENU_CAMERA_ZOOMIN:
      ke.m_keyCode = WXK_UP;
      ke.m_controlDown = true;
      break;
    case LXMENU_CAMERA_ZOOMOUT:
      ke.m_keyCode = WXK_DOWN;
      ke.m_controlDown = true;
      break;
  }
  if (ke.GetKeyCode() != 0)
    this->canvas->OnKeyPress(ke);
}


void lxFrame::OnMenuCameraOrient(wxCommandEvent& event) {
  switch (event.GetId()) {
    case LXMENU_CAMERA_ORIENT_HOME:
      this->setup->cam_tilt = 90.0;
      this->setup->cam_dir = 0.0;
      this->setup->ResetCamera();
      break;
    case LXMENU_CAMERA_ORIENT_PLAN:
      this->setup->cam_tilt = 90.0;
      this->setup->UpdatePos();
      break;
    case LXMENU_CAMERA_ORIENT_PROFILE:
      this->setup->cam_tilt = 0.0;
      this->setup->UpdatePos();
      break;
    case LXMENU_CAMERA_ORIENT_NORTH:
      this->setup->cam_dir = 0.0;
      this->setup->UpdatePos();
      break;
    case LXMENU_CAMERA_ORIENT_EAST:
      this->setup->cam_dir = 90.0;
      this->setup->UpdatePos();
      break;
    case LXMENU_CAMERA_ORIENT_SOUTH:
      this->setup->cam_dir = 180.0;
      this->setup->UpdatePos();
      break;
    case LXMENU_CAMERA_ORIENT_WEST:
      this->setup->cam_dir = 270.0;
      this->setup->UpdatePos();
      break;
  }
  this->canvas->ForceRefresh();
}


void lxFrame::ToggleFullScreen() {
  this->ShowFullScreen(!this->IsFullScreen());
	this->UpdateM2TB();
}


void lxFrame::DetectFileType()
{
#define matchtype(w,t) if (wxMatchWild(_T(w), this->m_fileName, false)) this->m_fileType = t;
  if (this->m_fileType == 0) {
    matchtype("*.lox",1);
    matchtype("*.LOX",1);
    matchtype("*.plt",2);
    matchtype("*.PLT",2);
    matchtype("*.3d",3);
    matchtype("*.3D",3);
  }
}




void lxFrame::OnAll(wxCommandEvent& event)
{
  switch (event.GetId()) {

    case LXMENU_HELP_CONTENTS:
      this->m_helpController->DisplayContents();
      break;

    case LXMENU_HELP_CONTROL:
      this->m_helpController->DisplaySection(_T("Control"));
      break;

    case LXMENU_HELP_RENDERING:
      this->m_helpController->DisplaySection(_T("Rendering"));
      break;

    case LXMENU_HELP_BUGS:
      this->m_helpController->DisplaySection(_T("Bugs"));
      break;

    case LXMENU_HELP_ABOUT:
      lxShowAboutDialog(this);
      break;

		case LXTB_VIEWPOINT:
      lxSize = this->m_toolBar->GetSize();
      lxPoint = wxGetMousePosition() - this->GetPosition();
#ifdef __WXMSW__
			this->PopupMenu(this->m_viewpointMenu, lxPoint.x - 8, lxSize.y);
#else
			this->PopupMenu(this->m_viewpointMenu, lxPoint.x - 8, 0);
#endif			
      this->m_toolBar->ToggleTool(LXTB_VIEWPOINT, false);
			break;

		case LXTB_LOCKROT:
		case LXMENU_CAMERA_LOCKROT:
			this->ToggleRotLock();
			break;

		case LXTB_FULLSCREEN:
    case LXMENU_VIEW_FULLSCREEN:
      this->ToggleFullScreen();
      break;
		
		case LXTB_STEREO:
      this->ToggleStereo();
      break;
		
		case LXTB_VIEWSTP:
      this->ToggleViewpointSetup();
      break;
		
		case LXTB_SCENESTP:
      this->ToggleModelSetup();
      break;

    case LXMENU_TOOLS_OPTIONS:
      lxShowOptionsDialog(this);
      break;


    case wxID_FILE1:
    case wxID_FILE2:
    case wxID_FILE3:
    case wxID_FILE4:
    case wxID_FILE5:
    case wxID_FILE6:
    case wxID_FILE7:
    case wxID_FILE8:
    case wxID_FILE9: 
      this->m_fileName = this->m_fileHistory->GetHistoryFile(event.GetId() - wxID_FILE1);
      this->m_fileDir = wxFileName(this->m_fileName).GetPath();
      this->m_fileType = 0;
      this->DetectFileType();
      this->ReloadData();
      this->setup->ResetCamera();
      this->canvas->ForceRefresh();
      break;

		case LXTB_OPEN:
    case LXMENU_FILE_OPEN:
      {
        wxFileDialog dialog
                 (
                    this,
                    _("Open"),
                    wxEmptyString,
                    wxEmptyString,
                    _("All supported files (*.lox;*.plt;*.3d)|*.lox;*.plt;*.3d|Loch files (*.lox)|*.lox|Compass PLT files (*.plt)|*.plt|Survex 3D files (*.3d)|*.3d")
                  );

       dialog.SetDirectory(this->m_fileDir);
			 dialog.CentreOnParent();

       if (dialog.ShowModal() == wxID_OK) {
          this->m_fileName = dialog.GetPath();
          this->m_fileDir  = dialog.GetDirectory();
          this->m_fileType = dialog.GetFilterIndex();
          this->DetectFileType();
          this->ReloadData();
          this->setup->ResetCamera();
          this->canvas->ForceRefresh();
        }
      }
      break;
      
		case LXTB_RELOAD:
    case LXMENU_FILE_RELOAD:
      this->ReloadData();
      this->canvas->ForceRefresh();
      break;
      
		case LXTB_RENDER_SETUP:
    case LXMENU_FILE_RENDER_SETUP:
      this->m_renderData->Configure(this, this->canvas);
      break;

		case LXTB_RENDER:
    case LXMENU_FILE_RENDER:
      this->m_renderData->Render(this, this->canvas);
      break;

		case LXTB_ROTATION:
		case LXMENU_CAMERA_AUTOROTATE:
			this->ToggleRotation();
			break;

		case LXTB_PERSP:
		case LXMENU_CAMERA_PERSP:
			this->TogglePerspective();
			break;

		case LXTB_FIT:
		case LXMENU_CAMERA_EXTENDS:
		  this->setup->ResetCamera();
		  this->canvas->ForceRefresh();
			break;

    case LXMENU_VIEW_MODELSTP:
      this->ToggleModelSetup();
      break;

    case LXMENU_VIEW_VIEWPOINTSTP:
      this->ToggleViewpointSetup();
      break;

    case LXTB_VISBBOX:
      this->ToggleVisibilityBBox();
      break;

    case LXTB_VISCENTERLINE:
      this->ToggleVisibilityCenterline();
      break;

    case LXTB_VISWALLS:
      this->ToggleVisibilityWalls();
      break;

    case LXTB_VISSURFACE:
      this->ToggleVisibilitySurface();
      break;

    case LXTB_VISINDS:
      this->ToggleVisibilityIndicators();
      break;

    default:
      event.Skip();
  }
}


void lxFrame::OnSize(wxSizeEvent& event)
{

  wxFrame::OnSize(event);
  if (this->m_viewpointSetupDlg != NULL) {
    this->m_viewpointSetupDlg->m_toolBoxPosition.Restore();
  }
  if (this->m_modelSetupDlg != NULL) {
    this->m_modelSetupDlg->m_toolBoxPosition.Restore();
  }

}

void lxFrame::OnMove(wxMoveEvent& event)
{
  if ((event.GetPosition().x > 0) || (event.GetPosition().y > 0)) {
    if (this->m_viewpointSetupDlg != NULL) {
      this->m_viewpointSetupDlg->m_toolBoxPosition.Restore();
    }
    if (this->m_modelSetupDlg != NULL) {
      this->m_modelSetupDlg->m_toolBoxPosition.Restore();
    }
  }
}


void lxFrame::UpdateM2TB() {
  
	// perspective
	this->m_menuBar->Check(LXMENU_CAMERA_PERSP, !this->setup->cam_persp);    
	this->m_menuBar->Check(LXMENU_VIEW_MODELSTP, this->m_modelSetupDlgOn);    
	this->m_menuBar->Check(LXMENU_VIEW_VIEWPOINTSTP, this->m_viewpointSetupDlgOn);    
	this->m_toolBar->ToggleTool(LXTB_PERSP, !this->setup->cam_persp); 

  // visibility
	this->m_toolBar->ToggleTool(LXTB_VISCENTERLINE, this->setup->m_vis_centerline); 
	this->m_toolBar->ToggleTool(LXTB_VISWALLS, this->setup->m_vis_walls); 
	this->m_toolBar->ToggleTool(LXTB_VISSURFACE, this->setup->m_vis_surface); 
	this->m_toolBar->ToggleTool(LXTB_VISBBOX, this->setup->m_vis_bbox); 
	this->m_toolBar->ToggleTool(LXTB_VISINDS, this->setup->m_vis_indicators); 

	// rotation
	this->m_menuBar->Check(LXMENU_CAMERA_AUTOROTATE, this->canvas->m_sCameraAutoRotate);
  this->m_toolMenu->Check(LXMENU_CAMERA_AUTOROTATE, this->canvas->m_sCameraAutoRotate);
	this->m_toolBar->ToggleTool(LXTB_ROTATION, this->canvas->m_sCameraAutoRotate); 

	// lock rotation
	this->m_menuBar->Check(LXMENU_CAMERA_LOCKROT, this->canvas->m_sCameraLockRotation);
  this->m_toolMenu->Check(LXMENU_CAMERA_LOCKROT, this->canvas->m_sCameraLockRotation);
	this->m_toolBar->ToggleTool(LXTB_LOCKROT, this->canvas->m_sCameraLockRotation); 	

  // stereo
  this->m_toolBar->ToggleTool(LXTB_STEREO, this->setup->cam_anaglyph); 

  // dialogs
  this->m_toolBar->ToggleTool(LXTB_VIEWSTP, this->m_viewpointSetupDlgOn); 
  this->m_toolMenu->Check(LXMENU_VIEW_VIEWPOINTSTP, this->m_viewpointSetupDlgOn); 
  this->m_toolBar->ToggleTool(LXTB_SCENESTP, this->m_modelSetupDlgOn); 
  this->m_toolMenu->Check(LXMENU_VIEW_MODELSTP, this->m_modelSetupDlgOn); 

	// full screen	
	this->m_toolBar->ToggleTool(LXTB_FULLSCREEN, this->IsFullScreen());
#ifdef LXLINUX	
	this->m_toolBar->Show(!this->IsFullScreen());
#endif	

  this->SetupUpdate();
	
}


void lxFrame::TogglePerspective() {
  this->setup->cam_persp = !this->setup->cam_persp;
  this->canvas->ForceRefresh();
	this->UpdateM2TB();
}

#define  lxFrameToggle(proc,item) void lxFrame::Toggle##proc() { \
  this->setup->item = !this->setup->item; \
  this->canvas->UpdateContents(); \
  this->UpdateM2TB(); \
}


#define  lxFrameFastToggle(proc,item) void lxFrame::Toggle##proc() { \
  this->setup->item = !this->setup->item; \
  this->canvas->UpdateRenderList(); \
  this->canvas->ForceRefresh(); \
  this->UpdateM2TB(); \
}


#define  lxFrameFastSet(proc,item) void lxFrame::Set##proc(int value) { \
  this->setup->item = value; \
  this->canvas->UpdateRenderList(); \
  this->canvas->ForceRefresh(); \
  this->UpdateM2TB(); \
}

lxFrameToggle(VisibilityCenterline, m_vis_centerline)
lxFrameToggle(VisibilityCenterlineCave, m_vis_centerline_cave)
lxFrameToggle(VisibilityCenterlineSurface, m_vis_centerline_surface)
lxFrameToggle(VisibilityWalls, m_vis_walls)
lxFrameToggle(VisibilitySurface, m_vis_surface)
lxFrameToggle(VisibilityLabels, m_vis_labels)
lxFrameToggle(VisibilityBBox, m_vis_bbox)
lxFrameToggle(VisibilityGrid, m_vis_grid)
lxFrameToggle(VisibilityIndicators, m_vis_indicators)

lxFrameFastSet(ColorMode, m_colormd) 

lxFrameFastToggle(ColorsApplyCenterline, m_colormd_app_centerline)
lxFrameFastToggle(ColorsApplyWalls, m_colormd_app_walls)

lxFrameToggle(WallsTransparency, m_walls_transparency)

lxFrameToggle(SurfaceTexture, m_srf_texture)
lxFrameToggle(SurfaceTransparency, m_srf_transparency)


void lxFrame::ToggleStereo() {
  this->setup->cam_anaglyph = !this->setup->cam_anaglyph;
  this->canvas->SetFontColorsS();
  this->canvas->UpdateRenderContents();
  this->canvas->UpdateRenderList();
  this->canvas->ForceRefresh();
  this->UpdateM2TB();
}



void lxFrame::ToggleStereoBW() {
  this->setup->cam_anaglyph_bw = !this->setup->cam_anaglyph_bw;
  this->canvas->UpdateRenderContents();
  this->canvas->UpdateRenderList();
  this->canvas->ForceRefresh();
  this->UpdateM2TB();
}


void lxFrame::ToggleRotation() {
	this->canvas->m_sCameraAutoRotate = !this->canvas->m_sCameraAutoRotate;
  if (this->canvas->m_sCameraAutoRotate) {
    this->canvas->m_sCameraAutoRotateCounter = 0;
    this->canvas->m_sCameraAutoRotateSWatch.Start();
    this->setup->StartCameraMovement();
  } else {
    ((wxStaticText *)(this->m_viewpointSetupDlg->FindWindow(LXVSTP_RENSPEED)))->SetLabel(_T(""));
  }
	this->UpdateM2TB();
}

void lxFrame::ToggleRotLock() {
	this->canvas->m_sCameraLockRotation = !this->canvas->m_sCameraLockRotation;
	this->UpdateM2TB();
}


void lxFrame::ToggleModelSetup()
{
  if (this->m_modelSetupDlgOn) {
    this->m_modelSetupDlg->m_toolBoxPosition.Save();
    this->m_modelSetupDlg->Show(false);
  } else {
    this->m_modelSetupDlg->m_toolBoxPosition.Restore();
    this->m_modelSetupDlg->Show(true);
  }
  this->m_modelSetupDlgOn = !this->m_modelSetupDlgOn;
	this->UpdateM2TB();
}


void lxFrame::ToggleViewpointSetup()
{
  if (this->m_viewpointSetupDlgOn) {
    this->m_viewpointSetupDlg->m_toolBoxPosition.Save();
    this->m_viewpointSetupDlg->Show(false);
  } else {
    this->m_viewpointSetupDlg->m_toolBoxPosition.Restore();
    this->m_viewpointSetupDlg->Show(true);
  }
  this->m_viewpointSetupDlgOn = !this->m_viewpointSetupDlgOn;
	this->UpdateM2TB();
}



void lxFrame::SetupUpdate()
{
  if (this->m_viewpointSetupDlgOn)
    this->m_viewpointSetupDlg->LoadSetup();
  if (this->m_modelSetupDlgOn)
    this->m_modelSetupDlg->LoadSetup();
}


void lxFrame::SetupApply()
{
}



void lxFrame::ReloadData()
{
  this->canvas->Refresh(false);
  wxBusyInfo info(_("Building 3D model, please wait..."));
  this->data->m_input.Clear();
  switch (this->m_fileType) {
    case 1:
      this->data->m_input.ImportLOX(this->m_fileName.mbc_str());
      break;
    case 2:
      this->data->m_input.ImportPLT(this->m_fileName.mbc_str());
      break;
    case 3:
      this->data->m_input.Import3D(this->m_fileName.mbc_str());
      break;
    default:
      this->data->m_input.m_error = "unable to detect file format";
      break;
  }
  switch (this->m_iniWallsInterpolate) {
    case LXWALLS_INTERP_ALL_ONLY:
      if (!this->data->m_input.HasAnyWalls())
        this->data->m_input.InterpolateMissingLRUD();
      break;
    case LXWALLS_INTERP_MISSING:
      if (this->m_fileType != 1) {
        this->data->m_input.InterpolateMissingLRUD();
      }
      break;
  }

  if (this->data->m_input.m_error.length() > 0) {
    wxMessageBox(_("Error reading input file"), _("Error"), wxICON_ERROR | wxOK);
  } else {
    this->m_fileHistory->AddFileToHistory(this->m_fileName);
  }
  this->data->Rebuild();
  this->canvas->UpdateRenderContents();
  this->canvas->UpdateRenderList();
  this->m_modelSetupDlg->InitSetup();
}




////////////////////////////////////////////////////////////////////////
// Application
////////////////////////////////////////////////////////////////////////

IMPLEMENT_APP(lxApp)

bool lxApp::OnInit()
{

    m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);
    m_locale.AddCatalog(wxT("loch"));
    m_path = wxFileName(this->argv[0]);

    vtkObject::GlobalWarningDisplayOff();

#if wxUSE_DISPLAY
    //multi screen support
    wxDisplay display(0); //Get the settings for screen 0
    wxRect displayRect = display.GetGeometry();
#else
    wxSize scr = wxGetDisplaySize();
#endif

    wxFileSystem::AddHandler(new wxZipFSHandler);

#if wxUSE_DISPLAY
    this->frame = new lxFrame(this, _T("Loch"),
      wxPoint(displayRect.width / 8, displayRect.height / 8), 
      wxSize(3 * displayRect.width / 4, 3 * displayRect.height / 4));
#else
    this->frame = new lxFrame(this, _T("Loch"), wxPoint(scr.x / 8, scr.y / 8), 
      wxSize(3 * scr.x / 4, 3 * scr.y / 4));
#endif
  
    this->frame->Show(true);
    this->frame->canvas->SetFocus();

#if wxUSE_DISPLAY
    this->frame->canvas->OSCInit(displayRect.width, displayRect.height);
#else
    this->frame->canvas->OSCInit(wxGetDisplaySize().x, wxGetDisplaySize().y);
#endif

    if (this->argc > 1) {
      this->frame->m_fileToOpen = wxString(this->argv[1]);
    }

    return true;
}


void lxFrame::OpenFile(const wxString & fName)
{
      wxFileName tmpFN(fName);
      this->m_fileName = tmpFN.GetFullPath();
      this->m_fileDir  = tmpFN.GetPath();
      this->m_fileType = 0;
      this->DetectFileType();
      this->ReloadData();
      this->setup->ResetCamera();
      this->canvas->ForceRefresh();
}








