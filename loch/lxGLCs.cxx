/**
 * @file lxGLC.cxx
 * Loch GL canvas implementation.
 */
  
/* Copyright (C) 2004 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 */

// Standard libraries
#ifndef LXDEPCHECK
#include <math.h>
#include <stdlib.h>
#include <wx/wx.h>
#include <stdio.h>
#endif  
//LXDEPCHECK - standart libraries

#include "lxGLC.h"
#include "lxGUI.h"
#include "lxData.h"
#include "lxSetup.h"
#include "lxSView.h"
#include "lxFNT6x13_bdf.h"
#include "lxFNT10x20_bdf.h"
#include "lxFNTFreeSans_ttf.h"

// implementacia common funkcii
#define LXGLCX_INFILE
#include "lxGLCx.cxx"


BEGIN_EVENT_TABLE(lxGLCanvas, wxGLCanvas)
    EVT_LEFT_DCLICK(lxGLCanvas::OnMouseDouble)
    EVT_RIGHT_DCLICK(lxGLCanvas::OnMouseDouble)
    EVT_LEFT_DOWN(lxGLCanvas::OnMouseDown)
    EVT_LEFT_UP(lxGLCanvas::OnMouseUp)
    EVT_MIDDLE_DOWN(lxGLCanvas::OnMouseDown)
    EVT_MIDDLE_UP(lxGLCanvas::OnMouseUp)
    EVT_RIGHT_DOWN(lxGLCanvas::OnMouseDown)
    EVT_RIGHT_UP(lxGLCanvas::OnMouseUp)
    EVT_MOTION(lxGLCanvas::OnMouseMove)
		EVT_MOUSEWHEEL(lxGLCanvas::OnMouseWheel)
    EVT_SIZE(lxGLCanvas::OnSize)
    EVT_PAINT(lxGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(lxGLCanvas::OnEraseBackground)
    EVT_ENTER_WINDOW(lxGLCanvas::OnEnterWindow )
    EVT_CHAR(lxGLCanvas::OnKeyPress)
    EVT_IDLE(lxGLCanvas::OnIdle)
END_EVENT_TABLE()


static const GLubyte srf16tex[48] = {
0, 0, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
0, 0, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0};

lxGLCanvas::lxGLCanvas(struct lxSetup * stp, struct lxData * dat, 
    wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style, name)
{
  this->frame = NULL;
  this->data = dat;
  this->setup = stp;
  this->ww = 100;
  this->wh = 100;
  this->mx = 0;
  this->my = 0;

  this->m_lic = 0;

  this->m_sInit = true;
  this->m_sInitReset = true;
  this->m_isO = false;
  
  this->m_renderData = NULL;

  // fonts initialization
  FT_Library library = OGLFT::Library::instance();
  FT_New_Memory_Face(library, lxFNT10x20_bdf, lxFNT10x20_bdf_size, 0, &this->m_ftFace1);
  FT_New_Memory_Face(library, lxFNT6x13_bdf, lxFNT6x13_bdf_size, 0, &this->m_ftFace2);
  FT_New_Memory_Face(library, lxFNTFreeSans_ttf, lxFNTFreeSans_ttf_size, 0, &this->m_ftFace3);

  this->m_fntTitleS = new OGLFT::Monochrome(this->m_ftFace1);
  this->m_fntNumericS = new OGLFT::Monochrome(this->m_ftFace2);

  this->m_fntTitleO = new OGLFT::Filled(this->m_ftFace3);
  this->m_fntNumericO = new OGLFT::Filled(this->m_ftFace3);

  this->m_OSC = NULL;
  this->m_TRC = NULL;
  
  this->m_sMoveLock = LXGLCML_NONE;

	this->m_sCameraAutoRotate = false;
	this->m_sCameraLockRotation = false;
	this->m_sCameraAutoRotateAngle = 1.0;

  this->SetCurrent();
  glGenTextures(1, &this->m_idTexSurface);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->m_maxTSizeS);

  this->OSCInit(128, 128);
  this->OSCMakeCurrent();
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->m_maxTSizeO);

  this->OSCDestroy();
  this->SetCurrent();    

}


lxGLCanvas::~lxGLCanvas()
{
  this->OSCDestroy();
  this->TRCDestroy();
  delete this->m_fntTitleS;
  delete this->m_fntNumericS;
  delete this->m_fntTitleO;
  delete this->m_fntNumericO;
  FT_Done_Face(this->m_ftFace1);
  FT_Done_Face(this->m_ftFace2);
  FT_Done_Face(this->m_ftFace3);
}


void lxGLCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
  //this->SetFocus();
}


void lxGLCanvas::RenderScreen()
{

  glViewport(0, 0, (GLint) this->ww, (GLint) this->wh);
  if (this->m_sInit) this->InitializeS();
  if (this->setup->cam_anaglyph) {

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    this->setup->cam_anaglyph_left = false;
    this->SetCameraS();
    this->SetColorMask();
    glCallList(this->m_sList);  

    this->setup->cam_anaglyph_left = true;
    this->SetCameraS();
    this->SetColorMask();
    glCallList(this->m_sList);  

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  } else {
    this->SetCameraS();
    glCallList(this->m_sList);  
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
  }
  if (this->setup->m_vis_indicators)
    this->RenderIndsS();

}


void lxGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event))
{
  
  wxPaintDC dc(this);

#ifndef __WXMOTIF__
  if (!GetContext()) return;
#endif

  if (!this->m_isO) {
    SetCurrent();
    this->RenderScreen();
    SwapBuffers();
  }

}


void lxGLCanvas::ForceRefresh(bool updateTB)
{
  this->Refresh(false);
  if (updateTB)
    this->frame->SetupUpdate();
}



void lxGLCanvas::OnSize(wxSizeEvent& event)
{

  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  this->GetClientSize(&w, &h);
#ifndef __WXMOTIF__
  if (GetContext())
#endif
  {
    this->setup->cam_width = double((GLfloat) w / (GLfloat) h);
    this->ww = w;
    this->wh = h;
#ifdef __WXMSW__
    this->ForceRefresh();
#endif
  }

}


void lxGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}


void lxGLCanvas::InitializeS()
{
    wxSizeEvent tmpe;
    OnSize(tmpe);

    //SetCurrent();
    this->OpenGLInitS();

    this->SetFontColorsS();
    this->UpdateRenderContents();
    this->UpdateRenderList();
    if (this->m_sInitReset) {
      this->setup->ResetCamera();
      this->m_sInitReset = false;    
    }
    this->m_sInit = false;    
}


void lxGLCanvas::UpdateRenderList()
{
  SetCurrent();
  if (this->m_sList == 0) 
    this->m_sList = glGenLists(1);
  glNewList(this->m_sList, GL_COMPILE);
  this->RenderAllS();
  glEndList();
}

void lxGLCanvas::UpdateRenderContents()
{

  this->setup->UpdateData();
  if (this->data->m_textureSurface.image.data != NULL) {
    if (this->data->m_textureSurface.texS == NULL)
      this->data->m_textureSurface.CreateTexImages(this->m_maxTSizeS, this->m_maxTSizeO);
    glBindTexture(GL_TEXTURE_2D, this->m_idTexSurface);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (this->m_isO) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
        this->data->m_textureSurface.texSizeO, this->data->m_textureSurface.texSizeO, 0, GL_RGB, GL_UNSIGNED_BYTE, 
        (this->setup->cam_anaglyph && this->setup->cam_anaglyph_bw) ? this->data->m_textureSurface.texObw : this->data->m_textureSurface.texO);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
        this->data->m_textureSurface.texSizeS, this->data->m_textureSurface.texSizeS, 0, GL_RGB, GL_UNSIGNED_BYTE,
        (this->setup->cam_anaglyph && this->setup->cam_anaglyph_bw) ? this->data->m_textureSurface.texSbw : this->data->m_textureSurface.texS);
    }
  }
  this->shift = 
    (this->setup->data_limits.max + this->setup->data_limits.min) / 2.0;  
}



void lxGLCanvas::UpdateContents()
{
  this->UpdateRenderContents();
  this->UpdateRenderList();
  this->ForceRefresh();
}


void lxGLCanvas::OnMouseUp(wxMouseEvent& event)
{
  switch (this->m_sMoveLock) {
    case LXGLCML_ZOOM:
    case LXGLCML_ROTATE:
    case LXGLCML_ZOOM2ROTATE:
			if (event.LeftUp())
        this->m_sMoveLock = LXGLCML_NONE;
      break;
    case LXGLCML_PANX:
    case LXGLCML_PANY:
    case LXGLCML_PANX2Y:
      if ((this->mx == event.GetX()) && (this->my == event.GetY())) {
        this->PopupMenu(this->frame->m_toolMenu);
      }
			if (event.RightUp() || (event.LeftUp() && this->m_sMoveSingle))
        this->m_sMoveLock = LXGLCML_NONE;
      break;
    case LXGLCML_TILT:
			if (event.MiddleUp() || (event.LeftUp() && this->m_sMoveSingle))
        this->m_sMoveLock = LXGLCML_NONE;
      break;
  }
	if ((this->m_sMoveLock == LXGLCML_NONE) && (this->HasCapture())) {
		this->ReleaseMouse();
	}

}

void lxGLCanvas::SetColorMask()
{
  switch (this->setup->cam_anaglyph_glasses) {
    // red & cyan
    case 0:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      else
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
      break;
    // red & green
    case 1:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      else
        glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
      break;
    // red & blue
    case 2:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      else
        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
      break;
    // yellow & blue
    case 3:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE);
      else
        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
      break;
    // cyan & red
    case 4:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
      else
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      break;
    // green & red
    case 5:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
      else
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      break;
    // blue & red
    case 6:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
      else
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      break;
    // yellow & blue
    default:
      if (this->setup->cam_anaglyph_left)
        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
      else
        glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE);
      break;
  }
}


bool lxGLCanvas::CameraAutoRotate() {
	if (this->m_sCameraAutoRotate)	{
		this->setup->RotateCameraF(this->m_sCameraAutoRotateAngle);
		this->ForceRefresh();
    this->m_sCameraAutoRotateCounter++;
    if (this->m_sCameraAutoRotateSWatch.Time() > 1000) {  
      ((wxStaticText *)(this->frame->m_viewpointSetupDlg->FindWindow(LXVSTP_RENSPEED)))->SetLabel(
        wxString::Format(_("Rendering at %.1f fps."), 1000.0 * double(this->m_sCameraAutoRotateCounter) / double(this->m_sCameraAutoRotateSWatch.Time())));
      this->m_sCameraAutoRotateCounter = 0;
      this->m_sCameraAutoRotateSWatch.Start();
    }
		return true;
	} else
		return false;
}


void lxGLCanvas::OnIdle(wxIdleEvent& WXUNUSED(event))	{

  // fix bug with opening file before everything is initialized
  if (!this->frame->m_fileToOpen.IsEmpty()) {
    wxString fto;
    fto = this->frame->m_fileToOpen;
    this->frame->m_fileToOpen = _T("");
    this->frame->OpenFile(fto);
    return;
  }


	switch (this->m_sMoveLock)	{
		case LXGLCML_PANX:
		case LXGLCML_PANX2Y:
		case LXGLCML_PANY:
			break;		
		default:
			this->CameraAutoRotate();
			break;
	}
}


void lxGLCanvas::OnMouseDouble(wxMouseEvent& event)
{
  if (event.m_leftDown)
    this->frame->ToggleFullScreen();
}


void lxGLCanvas::OnMouseDown(wxMouseEvent& event)
{
  this->m_lic = 0;
  switch (this->m_sMoveLock) {

    case LXGLCML_ZOOM2ROTATE:
      if (event.RightDown()) {
        if (abs(this->mx - event.GetX()) >= abs(this->my - event.GetY())) {
          this->m_sMoveLock = LXGLCML_ROTATE;
        } else {
          this->m_sMoveLock = LXGLCML_ZOOM;
        }
      }
      break;
      
    case LXGLCML_PANX2Y:
      if (event.LeftDown()) {
        if (abs(this->mx - event.GetX()) >= abs(this->my - event.GetY())) {
          this->m_sMoveLock = LXGLCML_PANX;
        } else {
          this->m_sMoveLock = LXGLCML_PANY;
        }
      }
      break;
      
    case LXGLCML_NONE:
      this->mx = event.GetX();
      this->my = event.GetY();
			this->m_sCameraStartAutoRotateAngle = this->m_sCameraAutoRotateAngle;
      this->setup->StartCameraMovement();
      this->m_sMoveSingle = false;
      if (event.LeftDown() && event.ShiftDown()) {
        this->m_sMoveLock = LXGLCML_PANX2Y;
        this->m_sMoveSingle = true;
      } else if (event.LeftDown() && event.ControlDown()) {
        this->m_sMoveLock = LXGLCML_TILT;
        this->m_sMoveSingle = true;
      } else if (event.LeftDown()) {
        this->m_sMoveLock = LXGLCML_ZOOM2ROTATE;
      } else if (event.RightDown()) {
        this->m_sMoveLock = LXGLCML_PANX2Y;
      } else if (event.MiddleDown()) {
        this->m_sMoveLock = LXGLCML_TILT;
      }
    break;
  }

	if (this->m_sMoveLock != LXGLCML_NONE) {
		if (!this->HasCapture()) {
			this->CaptureMouse();
		}
	}

}


void lxGLCanvas::OnMouseWheel(wxMouseEvent& event)
{
  if (this->m_sMoveLock == LXGLCML_NONE) {
    this->setup->StartCameraMovement();
		this->setup->TiltCamera(-1.0 * double(event.GetWheelRotation()) / double(event.GetWheelDelta()));
    this->ForceRefresh();
	}
}


void lxGLCanvas::OnMouseMove(wxMouseEvent& event)
{
  double f, ff;
  switch (this->m_sMoveLock) {

    case LXGLCML_ZOOM:
    case LXGLCML_ROTATE:
    case LXGLCML_ZOOM2ROTATE:
      if ((this->m_sMoveLock == LXGLCML_ZOOM) || (this->m_sMoveLock == LXGLCML_ZOOM2ROTATE))
        f = pow(1.4142135623730950488016887242097, double(this->my - event.GetY()) / 20.0);
      else
        f = 1.0;
      if ((!this->m_sCameraLockRotation) && ((this->m_sMoveLock == LXGLCML_ROTATE) || (this->m_sMoveLock == LXGLCML_ZOOM2ROTATE))) {
        ff = double(event.GetX() - this->mx) / 2.0;
      } else {
        ff = 0.0;
      }
      this->setup->ZoomCamera(f);
			if (this->m_sCameraAutoRotate) {
				this->m_sCameraAutoRotateAngle = this->m_sCameraStartAutoRotateAngle + ff / 50.0;
#ifdef LXWIN32
				if (!this->CameraAutoRotate())
					this->ForceRefresh();
#endif      
			} else {
				this->setup->RotateCamera(ff);			
				this->ForceRefresh();
			}
      break;
      
    case LXGLCML_PANX:
    case LXGLCML_PANY:
    case LXGLCML_PANX2Y:
      if ((this->m_sMoveLock == LXGLCML_PANX) || (this->m_sMoveLock == LXGLCML_PANX2Y))
        f = double(event.GetX() - this->mx) / double(this->ww);
      else
        f = 0.0;
      if ((this->m_sMoveLock == LXGLCML_PANY) || (this->m_sMoveLock == LXGLCML_PANX2Y))
        ff = double(event.GetY() - this->my) / double(this->wh);
      else
        ff = 0.0;
      this->setup->PanCamera(f, ff);
      this->ForceRefresh();
      break;
      
    case LXGLCML_TILT:
      this->setup->TiltCamera(double(event.GetY() - this->my) / 2.0);
 			if (this->m_sCameraAutoRotate) {
#ifdef LXWIN32
				if (!this->CameraAutoRotate())
					this->ForceRefresh();
#else
        ;        
#endif      
			} else {
		    this->ForceRefresh();
			}
      break;
  }

  if (this->m_sMoveLock != LXGLCML_NONE) {
		if (!this->HasCapture()) {
			this->CaptureMouse();
		}
	}


}

void lxGLCanvas::OnKeyPress(wxKeyEvent& event) {
  switch (event.GetKeyCode()) {
    case WXK_ESCAPE:
			if (this->frame->IsFullScreen())
				this->frame->ToggleFullScreen();
      break;
    case WXK_LEFT:
      this->setup->StartCameraMovement();
      if (event.ShiftDown())
        this->setup->PanCamera(-0.02, 0.0);
      else
        this->setup->RotateCamera(-1.0);
      this->ForceRefresh();
      break;
    case WXK_RIGHT:
      this->setup->StartCameraMovement();
      if (event.ShiftDown())
        this->setup->PanCamera(0.02, 0.0);
      else 
        this->setup->RotateCamera(1.0);
      this->ForceRefresh();
      break;
    case WXK_UP:
      this->setup->StartCameraMovement();
      if (event.ShiftDown())
        this->setup->PanCamera(0.0, -0.02);
      else if (event.ControlDown())
        this->setup->ZoomCamera(1.02);
      else
        this->setup->TiltCamera(-1.0);
      this->ForceRefresh();
     break;
    case WXK_DOWN:
      this->setup->StartCameraMovement();
      if (event.ShiftDown())
        this->setup->PanCamera(0.0, 0.02);
      else if (event.ControlDown())
        this->setup->ZoomCamera(0.98);
      else 
        this->setup->TiltCamera(1.0);
      this->ForceRefresh();
      break;
    default:
      event.Skip();
  }
    
}


