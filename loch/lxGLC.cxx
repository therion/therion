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
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <math.h>
#ifdef LXMACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
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
#include "lxSetup.h"
#include "lxRender.h"
#include "lxGUI.h"
#include "lxTR.h"

#ifdef LXWIN32
#include "lxR2D.h"
#endif
#ifdef LXLINUX
#include "lxR2P.h"
#endif

#define LXTRCBORDER (this->m_renderData->m_scaleMode == LXRENDER_FIT_SCREEN ? 0 : 16)

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
    0, 0, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0
};



lxGLCanvas::lxGLCanvas(struct lxSetup * stp, struct lxData * dat, 
                       wxWindow *parent, wxWindowID id,
                       const wxPoint& pos, const wxSize& size, long style, const wxString& name)
                       : wxGLCanvas(parent, id, NULL, pos, size, style, name),
                         ctx(this)
{
  this->frame = NULL;
  this->data = dat;
  this->setup = stp;
  this->ww = 100;
  this->wh = 100;
  this->mx = 0;
  this->my = 0;

  this->m_sList = 0;
  this->m_sFixList = 0;
  this->m_sEntList = 0;
  this->m_sStList = 0;
  this->m_oList = 0;
  this->m_oFixList = 0;
  this->m_oEntList = 0;
  this->m_oStList = 0;

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

  this->m_maxTSizeO = 0;
  this->m_maxTSizeS = 0;

  this->m_initTextures = true;

}


bool lxGLCanvas::IsRenderingOff()
{
  return (m_isO && (this->m_renderData->m_scaleMode != LXRENDER_FIT_SCREEN));
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
    this->SetCamera();
    this->SetColorMask();
    glCallList(this->m_sList);  

    this->setup->cam_anaglyph_left = true;
    this->SetCamera();
    this->SetColorMask();
    glCallList(this->m_sList);  

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  } else {

    this->SetCamera();
    glCallList(this->m_sList);  

  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  this->RenderOffList();

  if (this->setup->m_vis_indicators)
    this->RenderInds();

}


void lxGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event))
{

  wxPaintDC dc(this);

  if (!this->m_isO) {
    this->ctx.SetCurrent(*this);
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
  this->setup->cam_width = double((GLfloat) w / (GLfloat) h);
  this->ww = w;
  this->wh = h;
#ifdef __WXMSW__
  this->ForceRefresh();
#endif

}


void lxGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}


void lxGLCanvas::InitializeS()
{
  wxSizeEvent tmpe;
  OnSize(tmpe);
  this->OpenGLInit();
  //this->SetFontColors();
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
  this->ctx.SetCurrent(*this);
  if (this->m_sList == 0) 
    this->m_sList = glGenLists(1);
  glNewList(this->m_sList, GL_COMPILE);
  this->RenderAll();
  glEndList();
}



void lxGLCanvas::UpdateRenderContents()
{
  this->setup->UpdateData();
  if (this->data->m_textureSurface.image.data != NULL) {
    GLint newTSizeO, newTSizeS;
    newTSizeO = this->m_maxTSizeO;
    newTSizeS = this->m_maxTSizeS;
    if (this->m_isO) {
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &newTSizeO);
      if (newTSizeO > 4096) newTSizeO = 4096;
      //printf("MaxTSizeO: %d\n", newTSizeO);
      if (newTSizeS == 0) {
        newTSizeS = newTSizeO;
      }
    } else {
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &newTSizeS);
      if (newTSizeS > 4096) newTSizeS = 4096;
      //printf("MaxTSizeS: %d\n", newTSizeS);
      if (newTSizeO == 0) {
        newTSizeO = newTSizeS;
      }
    }  
    if ((this->data->m_textureSurface.texS == NULL) || (this->m_maxTSizeO != newTSizeO) || (this->m_maxTSizeS != newTSizeS)) {
      this->m_maxTSizeS = newTSizeS;
      this->m_maxTSizeO = newTSizeO;
      this->ctx.SetCurrent(*this);
      this->data->m_textureSurface.CreateTexImages(this->m_maxTSizeS, this->m_maxTSizeO);
      if (this->m_isO)
        this->OSCMakeCurrent();      
    }
    if (this->m_initTextures) {
      glGenTextures(1, & this->m_idTexSurface);
      glGenTextures(1, & this->m_idTexStation);
	  this->m_initTextures = false;
    }
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


#define lxFNTSH (this->IsRenderingOff() ? (this->m_fntNumericO->pointSize() * this->m_fntNumericO->resolution() / 72.0) : 13.0)
#define lxFNTSW (this->IsRenderingOff() ? (0.55 * lxFNTSH) : 6.0)
#define lxFNTLH (this->IsRenderingOff() ? (this->m_fntTitleO->pointSize() * this->m_fntTitleO->resolution() / 72.0) : 20.0)
#define lxFNTLW (this->IsRenderingOff() ? (0.55 * lxFNTLH) : 10.0)

OGLFT::Face * lxGLCanvas::GetFontNumeric() {
  if (this->IsRenderingOff())
    return this->m_fntNumericO;
  else
    return this->m_fntNumericS;
}

OGLFT::Face * lxGLCanvas::GetFontTitle() {
  if (this->IsRenderingOff())
    return this->m_fntTitleO;
  else
    return this->m_fntTitleS;
}


void lxGLCanvas::SetCamera() {

  double minclip, maxclip, vclip, hclip;
  lxVecLimits ml = this->setup->data_limits.Rotate(this->setup->cam_dir, this->setup->cam_tilt, this->setup->cam_center);

  //printf("\n\n\nCAMERA\n");
  //printf("   dir: %12.2f\n", this->setup->cam_dir);
  //printf("  tilt: %12.2f\n", this->setup->cam_tilt);
  //printf(" ext x: %12.2f%12.2f\n", ml.min.x, ml.max.x);
  //printf("     y: %12.2f%12.2f\n", ml.min.y, ml.max.y);
  //printf("     z: %12.2f%12.2f\n", ml.min.z, ml.max.z);
  //printf("center: %12.2f%12.2f%12.2f\n", lxVecXYZ(this->setup->cam_center));
  //printf("   pos: %12.2f%12.2f%12.2f\n", lxVecXYZ(this->setup->cam_pos));
  //printf("  dist: %12.2f\n", this->setup->cam_dist);

  minclip = this->setup->cam_dist + ml.min.y;
  if (minclip < 0.0) minclip = 1.0;
  minclip = 0.9 * minclip;
  maxclip = this->setup->cam_dist + ml.max.y;
  maxclip = 1.1 * maxclip;
  if (maxclip < minclip) maxclip = minclip + 1.0;
  if (minclip < (maxclip / 100.0)) minclip = maxclip / 100.0;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (this->setup->cam_persp) {
    if (!this->m_isO) {
      gluPerspective(this->setup->cam_lens_vfov, this->setup->cam_width, minclip, maxclip);
    } else {
      if (this->TRCGetContext()) {
        trPerspective(this->TRCGetContext(), this->setup->cam_lens_vfov, this->setup->cam_width, minclip, maxclip);
      } else {
        gluPerspective(this->setup->cam_lens_vfov, this->setup->cam_width, minclip, maxclip);
      }
    }   
  } else {
    vclip = this->setup->cam_dist * this->setup->cam_lens_vfovr;
    hclip = vclip * this->setup->cam_width;
    //printf("x clip: %12.2f%12.2f\n", -hclip, hclip);
    //printf("y clip: %12.2f%12.2f\n", -vclip, vclip);
    if (!this->m_isO) {
      glOrtho(-hclip, hclip, -vclip, vclip, minclip, maxclip);
    } else {
      if (this->TRCGetContext() != NULL) {
        trOrtho(this->TRCGetContext(), -hclip, hclip, -vclip, vclip, minclip, maxclip);
      } else {
        glOrtho(-hclip, hclip, -vclip, vclip, minclip, maxclip);
      }
    } 
  }
  //printf("z clip: %12.2f%12.2f\n", minclip, maxclip);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  lxVec up = lxVec(
    sin(this->setup->cam_tilt / 180.0 * lxPI) * sin(this->setup->cam_dir / 180.0 * lxPI),
    sin(this->setup->cam_tilt / 180.0 * lxPI) * cos(this->setup->cam_dir / 180.0 * lxPI),
    cos(this->setup->cam_tilt / 180.0 * lxPI)
    );
  //printf("LOOK FROM: %12.2f%12.2f%12.f\n", lxShiftVecXYZ(this->setup->cam_pos,this->shift));
  //printf("       TO: %12.2f%12.2f%12.f\n", lxShiftVecXYZ(this->setup->cam_center,this->shift));
  //printf("       UP: %12.2f%12.2f%12.f\n", lxVecXYZ(up));  
  if (this->setup->cam_anaglyph) {
    lxVec aRPosShift = (this->setup->cam_anaglyph_left ? -1.0 : 1.0) * (this->setup->cam_anaglyph_eyesep * this->setup->cam_dist) * lxVec(
      -cos(this->setup->cam_dir / 180.0 * lxPI),
      sin(this->setup->cam_dir / 180.0 * lxPI),
      0
      );
    gluLookAt(lxShiftVecXYZ(this->setup->cam_pos,this->shift + aRPosShift),
      lxShiftVecXYZ(this->setup->cam_center, this->shift),
      lxVecXYZ(up));
  } else {
    gluLookAt(lxShiftVecXYZ(this->setup->cam_pos,this->shift),
      lxShiftVecXYZ(this->setup->cam_center,this->shift),
      lxVecXYZ(up));
  }

  // Save view coordinates
  glGetDoublev(GL_MODELVIEW_MATRIX, m_camera_modelview);
  glGetDoublev(GL_PROJECTION_MATRIX, m_camera_projection);
  glGetIntegerv(GL_VIEWPORT, m_camera_viewport);

}

static const GLfloat mat0[] = { 0.0, 0.0, 0.0, 1.0};
static const GLfloat mat1[] = { 1.0, 1.0, 1.0, 1.0};
static const GLfloat mat2[] = { 0.5};
static const GLfloat mat3[] = { 1.0, 1.0, 1.0, 0.5};

void lxGLCanvas::RenderScrapWalls() {

  GLfloat clr[4];
  clr[0] = 1.0; clr[1] = 1.0; clr[2] = 1.0;
  clr[3] = this->setup->m_walls_opacity;
  glColor4f(1.0,1.0,1.0,this->setup->m_walls_opacity);

  glShadeModel(GL_SMOOTH);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_LIGHTING);

  bool useTransparency;
  useTransparency = false;
  if (this->setup->m_walls_transparency && (clr[3] < 1.0)) {
    lxVec viewDir;
    viewDir = this->setup->cam_center - this->setup->cam_pos;
    viewDir.Normalize();
    this->data->allWallsSorted->SetVector(viewDir.x, viewDir.y, viewDir.z);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    useTransparency = true;
  } else {
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  glEnable(GL_COLOR_MATERIAL);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat2);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glColor4fv(clr);

  vtkIdType * cPts, nPts, xP;
  double * nmv, * ptc, nmvv[3];

  this->data->scrapWallsNormals->Update();

  vtkPolyData * pdt;
  if (useTransparency) {
    pdt = this->data->allWallsSorted->GetOutput();
  } else {
    pdt = this->data->allWallsStripped->GetOutput();
  }
  vtkCellArray * tgs = pdt->GetPolys();
  vtkCellArray * tss = pdt->GetStrips();
  vtkDataArray * nms = pdt->GetPointData()->GetNormals();

#define draw3vert(N) \
  ptc = pdt->GetPoint(cPts[N]); \
  nmv = nms->GetTuple(cPts[N]); \
  glNormal3f(nmv[0],nmv[1],nmv[2]); \
  if ((!this->setup->cam_anaglyph) && (this->setup->m_colormd != lxSETUP_COLORMD_DEFAULT) && (this->setup->m_colormd_app_walls)) { \
  this->data->luTable->GetColor(ptc[2], nmvv); \
  clr[0] = nmvv[0]; clr[1] = nmvv[1]; clr[2] = nmvv[2]; \
  glColor4fv(clr); \
  } \
  glVertex3f(lxShiftVecX3(ptc, this->shift));

  tgs->InitTraversal();
  glBegin(GL_TRIANGLES);
  while (tgs->GetNextCell(nPts, cPts) != 0) {
    if (nPts == 3) {      
      draw3vert(0);
      draw3vert(1);
      draw3vert(2);
    }
  }
  glEnd();

  tss->InitTraversal();
  while (tss->GetNextCell(nPts, cPts) != 0) {
    glBegin(GL_TRIANGLE_STRIP);
    for(xP = 0; xP < nPts; xP++) {
      draw3vert(xP);
    }
    glEnd();
  }

  glDisable(GL_COLOR_MATERIAL);

}


void lxGLCanvas::RenderSurface() {

  GLfloat clr[4];
  clr[0] = 1.0;
  clr[1] = 1.0;
  clr[2] = 1.0;
  clr[3] = this->setup->m_srf_opacity;

  bool srf_tex = (this->data->m_textureSurface.image.data != NULL) && (this->setup->m_srf_texture);
  glShadeModel(GL_SMOOTH);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glColor4f(1.0,1.0,1.0,this->setup->m_srf_opacity);
  if (this->setup->m_srf_transparency && (clr[3] < 1.0)) {  

    lxVec viewDir;
    viewDir = this->setup->cam_center - this->setup->cam_pos;
    viewDir.Normalize();
    this->data->surfaceSorted->SetVector(viewDir.x, viewDir.y, viewDir.z);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
  } else {
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, clr);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat2);
  if (srf_tex) {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,  GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, this->m_idTexSurface);
  } else {
    glDisable(GL_TEXTURE_2D);
  }

  if (this->setup->m_srf_lighting)
    glEnable(GL_LIGHTING);
  else 
    glDisable(GL_LIGHTING);

  vtkIdType * cPts, nPts;
  double * nmv, * ptc;

  this->data->surfaceSorted->Update();

  vtkPolyData * pdt = this->data->surfaceSorted->GetOutput();
  vtkCellArray * tgs = pdt->GetPolys();
  vtkDataArray * nms = pdt->GetPointData()->GetNormals();
  tgs->InitTraversal();
  glBegin(GL_TRIANGLES);
  while (tgs->GetNextCell(nPts, cPts) != 0) {
    if (nPts == 3) {

      //      nmv = this->data->luTable->GetColor(ptc[2]); 
      //      clr[0] = nmv[0]; clr[1] = nmv[1]; clr[2] = nmv[2]; 
      //      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, clr); 

#undef draw3vert
#define draw3vert(N) \
  ptc = pdt->GetPoint(cPts[N]); \
  nmv = nms->GetTuple(cPts[N]); \
  glNormal3f(nmv[0],nmv[1],nmv[2]); \
  if (srf_tex) \
  glTexCoord2f(\
  (ptc[0] * this->data->m_textureSurface.xx + ptc[1] * this->data->m_textureSurface.xy + this->data->m_textureSurface.dx) / this->data->m_textureSurface.iw, \
  1.0 - (ptc[0] * this->data->m_textureSurface.yx + ptc[1] * this->data->m_textureSurface.yy + this->data->m_textureSurface.dy) / this->data->m_textureSurface.ih); \
  glVertex3f(lxShiftVecX3(ptc, this->shift)); \

      draw3vert(0);
      draw3vert(1);
      draw3vert(2);

    }
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
}




void lxGLCanvas::RenderCenterline() {
  if (this->IsRenderingOff()) {
    glLineWidth(this->m_renderData->m_imgResolution / 96.0);
  } else {
    glLineWidth(1.0);
  }
  double clrs[3], clrc[3];
  if (this->setup->cam_anaglyph) {
    if ((this->m_renderData != NULL) && (this->m_renderData->m_scaleMode != LXRENDER_FIT_SCREEN) && this->m_renderData->m_imgWhiteBg) {
      clrc[0] = 0.0;clrc[1] = 0.0;clrc[2] = 0.0; 
    } else {
      clrc[0] = 1.0;clrc[1] = 1.0;clrc[2] = 1.0; 
    }
  } else {
    clrc[0] = 0.0;clrc[1] = 1.0;clrc[2] = 1.0; 
  }

  clrs[0] = 0.5; clrs[1] = 0.5; clrs[2] = 0.5; 

  bool prevsurface = false;
  lxDataShot * psh;
  lxVec * stv;
  double clr[3];
  unsigned long id, nid;
  nid = this->data->shots.size();
  glColor3f(clrc[0],clrc[1],clrc[2]);
  glBegin(GL_LINES);
  for(id = 0; id < nid; id++) {

    psh = &(this->data->shots[id]);
    if (psh->invisible)
      continue;
    if (psh->splay && (!this->setup->m_vis_centerline_splay))
      continue;
    if (psh->duplicate && (!this->setup->m_vis_centerline_duplicate))
      continue;


#define drawLvert(N) \
  stv = &(this->data->stations[N].pos); \
  if ((!psh->surface) && (!this->setup->cam_anaglyph) && (this->setup->m_colormd != lxSETUP_COLORMD_DEFAULT) && (this->setup->m_colormd_app_centerline)) { \
  this->data->luTable->GetColor(stv->z, clr); \
  glColor3f(clr[0],clr[1],clr[2]); \
  } \
  glVertex3f(lxShiftVecPXYZ(stv, this->shift));

    if ((psh->surface && this->setup->m_vis_centerline_surface) 
      || ((!psh->surface) && this->setup->m_vis_centerline_cave)) {
        if (prevsurface != psh->surface) {
          if (psh->surface) {
            glColor3f(clrs[0],clrs[1],clrs[2]);
          } else {
            glColor3f(clrc[0],clrc[1],clrc[2]);
          }
        }
        drawLvert(psh->from);
        drawLvert(psh->to);
      }

      prevsurface = psh->surface;
  } 
  glEnd();

}

void lxGLCanvas::ProjectPoint(double src_x, double src_y, double src_z, GLdouble * x, GLdouble * y, GLdouble * z)
{
   gluProject(src_x, src_y, src_z,
            m_camera_modelview, m_camera_projection, m_camera_viewport,
            x, y, z);
}


void lxGLCanvas::ProjectStations() {
  unsigned long id, nid;
  nid = this->data->stations.size();
  lxDataStation * st;
  for(id = 0; id < nid; id++) {
    st = &(this->data->stations[id]);
    this->ProjectPoint(lxShiftVecPXYZ(&(st->pos), this->shift), &(st->m_screen_x), &(st->m_screen_y), &(st->m_screen_z));
  }
}


void lxGLCanvas::RenderOffList()
{

  if (!(this->setup->m_vis_centerline_fix || 
        this->setup->m_vis_centerline_station || 
        this->setup->m_vis_centerline_entrance))
    return;

  this->ProjectStations();  
  this->SetIndicatorsTransform();

  // Render stations
  unsigned long id, nid;
  nid = this->data->stations.size();
  lxDataStation * st;
  GLdouble x, y, z;
  GLuint fxList, enList, stList;

  glShadeModel(GL_FLAT);
  glDepthMask(GL_TRUE);
  glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (this->IsRenderingOff()) {
    if (this->m_oStList == 0) {
      this->m_oStList = glGenLists(3);
      this->m_oEntList = this->m_oStList + 1;
      this->m_oFixList = this->m_oStList + 2;
    }
    stList = this->m_oStList;
    enList = this->m_oEntList;
    fxList = this->m_oFixList;
  } else {
    if (this->m_sStList == 0) {
      this->m_sStList = glGenLists(3);
      this->m_sEntList = this->m_sStList + 1;
      this->m_sFixList = this->m_sStList + 2;
    }
    stList = this->m_sStList;
    enList = this->m_sEntList;
    fxList = this->m_sFixList;
  }

  glNewList(fxList, GL_COMPILE);
  glColor3f(0.2,0.0,0.0);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3d( 0.0,-2.4,0.0);
  glVertex3d(+2.4, 0.0,0.0);
  glVertex3d( 0.0,+2.4,0.0);
  glVertex3d(-2.4, 0.0,0.0);
  glEnd();
  glColor3f(1.0,0.0,0.0);
  glBegin(GL_LINE_STRIP);
  glVertex3d( 0.0,-2.4,0.0);
  glVertex3d(+2.4, 0.0,0.0);
  glVertex3d( 0.0,+2.4,0.0);
  glVertex3d(-2.4, 0.0,0.0);
  glVertex3d( 0.0,-2.4,0.0);
  glEnd();
  glBegin(GL_LINES);
  glVertex3d( 0.0,-2.4,0.0);
  glVertex3d( 0.0,-3.4,0.0);
  glVertex3d(+2.4, 0.0,0.0);
  glVertex3d(+3.4, 0.0,0.0);
  glVertex3d( 0.0,+2.4,0.0);
  glVertex3d( 0.0,+3.4,0.0);
  glVertex3d(-2.4, 0.0,0.0);
  glVertex3d(-3.4, 0.0,0.0);

  glEnd();
  glEndList();

  glNewList(enList, GL_COMPILE);
  glColor3f(0.0,0.0,0.0); //glColor3f(0.1,0.3,0.1);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3d(-4.4,-4.4,0.0);
  glVertex3d(-4.4, 2.4,0.0);
  glVertex3d(-2.4, 4.4,0.0);
  glVertex3d( 2.4, 4.4,0.0);
  glVertex3d( 4.4, 2.4,0.0);
  glVertex3d( 4.4,-4.4,0.0);
  glEnd();
  glColor3f(0.3,1.0,0.3);
  glBegin(GL_LINE_STRIP);
  glVertex3d(-3.4,-3.4,0.0);
  glVertex3d(-0.4,-3.4,0.0);
  glVertex3d(-3.4,-0.4,0.0);
  glVertex3d(-3.4, 0.4,0.0);
  glVertex3d(-0.4, 3.4,0.0);
  glVertex3d( 0.4, 3.4,0.0);
  glVertex3d( 3.4, 0.4,0.0);
  glVertex3d( 3.4,-0.4,0.0);
  glVertex3d( 0.4,-3.4,0.0);
  glVertex3d( 3.4,-3.4,0.0);
  glEnd();
  glEndList();

  glNewList(stList, GL_COMPILE);
  glColor3f(1.0,1.0,0.0);
  glBegin(GL_LINES);
  glVertex3d(-2.4,   0,0);
  glVertex3d( 2.4,   0,0);
  glVertex3d(   0,-2.4,0);
  glVertex3d(   0,+2.4,0);
  glEnd();
  glEndList();


  if (this->setup->m_vis_centerline_station) {
    for(id = 0; id < nid; id++) {
      st = &(this->data->stations[id]);
      if (st->m_screen_z < 0.0) continue;
      x = (GLdouble)long(st->m_screen_x) + 0.5;
      y = (GLdouble)long(st->m_screen_y) + 0.5;
      z = - st->m_screen_z;
      if (st->m_temporary && (!this->setup->m_vis_centerline_splay))
        continue;
      if (st->m_surface && (!this->setup->m_vis_centerline_surface))
        continue;
      if ((!st->m_surface) && (!this->setup->m_vis_centerline_cave))
        continue;
      glLoadIdentity();
      glTranslated(x, y, z);
      glCallList(stList);
    }
  }

  glDisable(GL_DEPTH_TEST);

  if (this->setup->m_vis_centerline_fix) {
    for(id = 0; id < nid; id++) {
      st = &(this->data->stations[id]);
      if (st->m_fix) {
        if (st->m_screen_z < 0.0) continue;
        x = (GLdouble)long(st->m_screen_x) + 0.5;
        y = (GLdouble)long(st->m_screen_y) + 0.5;
        z = - st->m_screen_z;
        glLoadIdentity();
        glTranslated(x, y, z);
        glCallList(fxList);
      }
    }
  }

  if (this->setup->m_vis_centerline_entrance) {
    for(id = 0; id < nid; id++) {
      st = &(this->data->stations[id]);
      if (st->m_entrance) {
        if (st->m_screen_z < 0.0) continue;
        x = (GLdouble)long(st->m_screen_x) + 0.5;
        y = (GLdouble)long(st->m_screen_y) + 0.5;
        z = - st->m_screen_z;
        glLoadIdentity();
        glTranslated(x, y, z);
        glCallList(enList);
      }
    }
  }

  glLoadIdentity();
  std::string cmnt;
  const char * csurvey;
  char strCBar[10];
  bool show_label;
  this->GetFontNumeric()->setForegroundColor(1.0, 1.0, 0.5, 1.0);
  if (this->setup->m_stlabel_name || this->setup->m_stlabel_comment || this->setup->m_stlabel_altitude || this->setup->m_stlabel_survey) {
    for(id = 0; id < nid; id++) {
      show_label = false;
      st = &(this->data->stations[id]);
      if (st->m_screen_z < 0.0) continue;
      if (st->m_temporary) continue;
      if (this->setup->m_vis_centerline_entrance && st->m_entrance) show_label = true;
      if (this->setup->m_vis_centerline_fix && st->m_fix) show_label = true;
      if (st->m_surface && this->setup->m_vis_centerline_surface && this->setup->m_vis_centerline_station) show_label = true;
      if ((!st->m_surface) && this->setup->m_vis_centerline_cave && this->setup->m_vis_centerline_station) show_label = true;
      if (!show_label) continue;

      // name
      cmnt = "";
      if (this->setup->m_stlabel_name && (strlen(st->m_name) > 0))
        cmnt = st->m_name;

      // survey
      if (this->setup->m_stlabel_survey) {
        csurvey = this->data->surveys[st->m_survey_idx].m_full_name.c_str();
        if (strlen(csurvey) > 0) {
          if (cmnt.length() > 0) cmnt += "@";
          cmnt += csurvey;
        }
      }

      // altitude
      if (this->setup->m_stlabel_altitude) {
        if (this->frame->m_iniUnits == 1) {
          sprintf(&(strCBar[0]), "%.0f ft", st->pos.z / 0.3048);
        } else {
          sprintf(&(strCBar[0]), "%.0f m", st->pos.z);
        }
        if (cmnt.length() > 0) cmnt += ":";
        cmnt += strCBar;
      }

      // comment
      if (this->setup->m_stlabel_comment && (strlen(st->m_comment) > 0)) {
        if (cmnt.length() > 0) cmnt += ":";
        cmnt += st->m_comment;
      }


      x = (GLdouble)long(st->m_screen_x+0.5);
      y = (GLdouble)long(st->m_screen_y+0.5);
      z = - st->m_screen_z;

      if (cmnt.length() > 0) {
        this->GetFontNumeric()->draw(x + 5, y - 0.25 * lxFNTSH, z, wxString(wxConvUTF8.cMB2WX(cmnt.c_str())));
      }
    }
  }

}


void lxGLCanvas::GeomOutline() {

  glBegin(GL_LINE_STRIP);
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(0), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(4), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(0), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(1), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(5), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(1), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(2), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(6), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(2), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(3), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(7), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(3), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(0), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(4), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(5), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(6), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(7), this->shift));
  glVertex3f(lxShiftVecXYZ(this->setup->data_limits.Vertex(4), this->shift));
  glEnd();

}


void lxGLCanvas::RenderAll() {

  if (this->m_isO) {
    if ((this->m_renderData != NULL) && (this->m_renderData->m_scaleMode != LXRENDER_FIT_SCREEN) && this->m_renderData->m_imgWhiteBg)
      glClearColor(1.0, 1.0, 1.0, 0.0);
    else
      glClearColor(0.0, 0.0, 0.0, 0.0);
  } else {
    glClearColor(0.0, 0.0, 0.0, 0.0);
  }
  glClearDepth(1.0);
  if (this->setup->cam_anaglyph)
    glClear(GL_DEPTH_BUFFER_BIT);
  else 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // vyrendruje outline
  glDisable(GL_LIGHTING);
  glShadeModel(GL_FLAT);
  glDepthMask(GL_TRUE);
  if (this->m_isO) {
    glLineWidth(this->m_renderData->m_scaleMode == LXRENDER_FIT_SCREEN ? 1.0 : this->m_renderData->m_imgResolution / 96.0);
  } else {
    glLineWidth(1.0);
  }
  glDisable(GL_BLEND);
  if (this->setup->cam_anaglyph)
    glColor3f(0.7, 0.7, 0.7);
  else
    glColor3f(1.0, 0.0, 0.0);

  if (this->setup->m_vis_bbox)
    this->GeomOutline();

  glShadeModel(GL_SMOOTH);
  if (this->setup->m_vis_centerline)
    this->RenderCenterline();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  if (this->setup->m_vis_walls)
    this->RenderScrapWalls();  
  glDisable(GL_CULL_FACE);

  if (this->setup->m_vis_surface)
    this->RenderSurface();
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glShadeModel(GL_FLAT);

}


void lxGLCanvas::OpenGLInit() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void lxGLCanvas::SetIndicatorsTransform()
{
  if (this->m_isO) {
    this->m_indRes = this->m_renderData->m_imgResolution / 25.4; // pixels per mm
    this->m_indLWidth = 0.5;    // line width mm
    if ((this->m_indLWidth * this->m_indRes) < 1.0) {
      this->m_indLWidth = 1.0 / this->m_indRes;    // line width mm
    }
  } else {
    this->m_indRes = 3.7795276; // pixels per mm
    this->m_indLWidth = 1.0 / this->m_indRes;    // line width mm
    glLineWidth(1.0);
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (this->m_isO) {
    trOrtho(this->TRCGetContext(), 0.0, this->m_renderData->m_imgPixW, 0.0, this->m_renderData->m_imgPixH, -1.0, 1.0);
    this->TRCBeginTile();
  } else {
    glOrtho(0.0, this->ww, 0.0, this->wh, 0.0, 1.0);
    //gluOrtho2D(0.0, this->ww, 0.0, this->wh);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}



void lxGLCanvas::RenderInds()
{

  this->SetFontColors();
  if (this->m_isO) {
    this->m_indRes = this->m_renderData->m_imgResolution / 25.4; // pixels per mm
    this->m_indLWidth = 0.5;    // line width mm
    if ((this->m_indLWidth * this->m_indRes) < 1.0) {
      this->m_indLWidth = 1.0 / this->m_indRes;    // line width mm
    }
  } else {
    this->m_indRes = 3.7795276; // pixels per mm
    this->m_indLWidth = 1.0 / this->m_indRes;    // line width mm
    glLineWidth(1.0);
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (this->m_isO) {
    trOrtho(this->TRCGetContext(), 0.0, this->m_renderData->m_imgPixW, 0.0, this->m_renderData->m_imgPixH, -1.0, 1.0);
    this->TRCBeginTile();
  } else {
    gluOrtho2D(0.0, this->ww, 0.0, this->wh);
  }

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glShadeModel(GL_FLAT);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glLoadIdentity();
  glTranslatef(this->m_indRes * 9.0,  this->m_indRes * 18.0, 0.0);
  this->RenderICompass(5.0);

  glLoadIdentity();
  glTranslatef(this->m_indRes * 28.0, this->m_indRes * 13.0, 0.0);
  this->RenderIClino(8.0);

  glLoadIdentity();
  glTranslatef(this->m_indRes * 34.0, this->m_indRes * 9.0, 0.0);
  this->RenderIScalebar(50.0);

  if ((!this->setup->cam_anaglyph) && (this->setup->m_colormd != lxSETUP_COLORMD_DEFAULT)) {
    glLoadIdentity();
    glTranslatef(this->m_indRes * 4.0, this->m_indRes * 29.0, 0.0);
    this->RenderIDepthbar(40.0);
  }

  glLoadIdentity();
  this->GetFontTitle()->draw(this->m_indRes * 3.0, this->m_indRes * 3.0, this->data->title);
//  if (this->m_isO)
//    this->m_fntTitleO->draw(this->m_indRes * 3.0, this->m_indRes * 3.0, this->data->title);
//  else
//    this->m_fntTitleS->draw(this->m_indRes * 3.0, this->m_indRes * 3.0, this->data->title);

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}


#define clrTemplate(AW,AB,NL) {\
  if (this->m_isO) {\
    if (this->setup->cam_anaglyph) {\
       if (this->m_renderData->m_imgWhiteBg)\
         glColor4f AW; \
       else \
         glColor4f AB; \
    } else \
      glColor4f NL; \
  } else { \
  if (this->setup->cam_anaglyph) \
    glColor4f AB; \
  else \
    glColor4f NL; \
  } \
}

#define clrOutCntr() clrTemplate((0.0, 0.0, 0.0, 1.0),(1.0, 1.0, 1.0, 1.0),(0.1, 0.1, 1.0, 1.0)) 
#define clrOutFill() clrTemplate((0.5, 0.5, 0.5, 0.5),(0.5, 0.5, 0.5, 0.5),(0.0, 0.0, 1.0, 0.5))
#define clrIndCntr() clrTemplate((0.0, 0.0, 0.0, 1.0),(1.0, 1.0, 1.0, 1.0),(1.0, 0.0, 0.0, 1.0))
#define clrIndFill() clrTemplate((0.5, 0.5, 0.5, 1.0),(0.5, 0.5, 0.5, 1.0),(1.0, 0.8, 0.0, 1.0))

#define v2r(x,y) glVertex2f(this->m_indRes * (x), this->m_indRes * (y))
#define hlw (this->m_indLWidth / 2.0)


void lxGLCanvas::RenderICompass(double size) {
  int t;

  glPushMatrix();
  glRotatef(this->setup->cam_dir, 0.0, 0.0, 1.0);
  clrOutFill();

  // draw circle aroung
  glBegin(GL_TRIANGLE_FAN);
  for(t = 0; t < 360; t++) {
    v2r(
      (size - hlw) * sin(double(t)/180.0*lxPI),
      (size - hlw) * cos(double(t)/180.0*lxPI));
  }
  glEnd();

  clrOutCntr();
  if (this->m_isO) {
    glBegin(GL_TRIANGLE_STRIP);
    for(t = 0; t <= 360; t++) {
      v2r(
        (size - hlw) * sin(double(t)/180.0*lxPI),
        (size - hlw) * cos(double(t)/180.0*lxPI));
      v2r(
        (size + hlw) * sin(double(t)/180.0*lxPI),
        (size + hlw) * cos(double(t)/180.0*lxPI));
    }
    glEnd();
  } else {
    glBegin(GL_LINE_STRIP);
    for(t = 0; t <= 360; t++) {
      v2r(
        (size - hlw) * sin(double(t)/180.0*lxPI),
        (size - hlw) * cos(double(t)/180.0*lxPI));
    }
    glEnd();
  }

  this->RenderILine(0.0, -size - 0.75, 0.0, -size + 1.0);
  this->RenderILine(-size - 0.75, 0.0, -size + 1.0, 0.0);
  this->RenderILine( size - 1.0, 0.0,  size + 0.75, 0.0);

  lxVec a1, a2, a3, a4;

  a1 = lxVec(0.0, size + 1.0, 0.0);
  a2 = a1.Rotated(150.0, 0.0);
  a3 = a1.Rotated(-150.0, 0.0);
  a4 = lxVec(0.0, -0.333 * size, 0.0);

  clrIndFill();
  glBegin(GL_TRIANGLE_STRIP);
  v2r(a2.x, a2.y);
  v2r(a1.x, a1.y);
  v2r(a4.x, a4.y);
  v2r(a3.x, a3.y);
  glEnd();

  clrIndCntr();
  this->RenderILine(a1.x, a1.y, a2.x, a2.y);
  this->RenderILine(a1.x, a1.y, a3.x, a3.y);
  this->RenderILine(a4.x, a4.y, a2.x, a2.y);
  this->RenderILine(a4.x, a4.y, a3.x, a3.y);

  glPopMatrix();

#if wxCHECK_VERSION(3,0,0)
  this->GetFontNumeric()->draw((-2.0) * lxFNTSW, this->m_indRes * (-size - 1.0) - lxFNTSH, wxString::Format(_("%03d\xc2\xb0"), int(this->setup->cam_dir)));
#else
  this->GetFontNumeric()->draw((-2.0) * lxFNTSW, this->m_indRes * (-size - 1.0) - lxFNTSH, wxString::Format(_("%03d\260"), int(this->setup->cam_dir)));
#endif  

}


void lxGLCanvas::RenderIClino(double size)
{

#define awn 0.75
#define awt 1.5
#define aae 0.33
  int t;
  double tilt = this->setup->cam_tilt;
  bool neg = tilt < 0.0;

  glPushMatrix();
  if (neg)
    glTranslatef(0.0, this->m_indRes * size, 0.0);

  clrOutFill();
  if (fabs(tilt) > 0.0) {
    glBegin(GL_TRIANGLE_FAN);
    v2r(0.0, 0.0);
    for(t = 0; t <= 20; t++) {
      v2r(
        (-size) * cos(double(t) * (tilt / 20.0) / 180.0 * lxPI),
        (size) *  sin(double(t) * (tilt / 20.0) / 180.0 * lxPI));
    }
    glEnd();
  }

  clrOutCntr();

  if (this->m_isO) {
    glBegin(GL_TRIANGLE_STRIP);
    for(t = 0; t <= 90; t += 5) {
      v2r(
        (-1.0) * (size - hlw) * cos(double(t)/180.0*lxPI),
        (neg ? -1.0 : 1.0) * (size - hlw) * sin(double(t)/180.0*lxPI));
      v2r(      
        (-1.0) * (size + hlw) * cos(double(t)/180.0*lxPI),
        (neg ? -1.0 : 1.0) * (size + hlw) * sin(double(t)/180.0*lxPI));
    }
    glEnd();
  } else {
    glBegin(GL_LINE_STRIP);
    for(t = 0; t <= 90; t += 5) {
      v2r(
        (-1.0) * (size) * cos(double(t)/180.0*lxPI),
        (neg ? -1.0 : 1.0) * (size) * sin(double(t)/180.0*lxPI));
    }
    glEnd();
  }

  this->RenderILine(-size - 1.0, 0.0, 1.0, 0.0);
  this->RenderILine(0.0, (neg ? 1.0 : -1.0), 0.0, (neg ? -size - 1.0 : size + 1.0));
  this->RenderILine(-size, (neg ? 1.0 : -1.0), -size, 0.0);
  this->RenderILine(0.0, (neg ? -size : size), 1.0, (neg ? -size : size));

  glPushMatrix();
  glRotatef(-tilt, 0.0, 0.0, 1.0);

  clrIndFill();
  glBegin(GL_QUADS);
  v2r(-size - 1.0, -awn);
  v2r(-size - 1.0, awn);
  v2r(-aae * size, awn);
  v2r(-aae * size, -awn);
  glEnd();
  glBegin(GL_TRIANGLES);
  v2r(-aae * size, awt);
  v2r(-aae * size, -awt);
  v2r(0.0, 0.0);
  glEnd();

  clrIndCntr();
  this->RenderILine(-size - 1.0, -awn, -size - 1.0, awn);
  this->RenderILine(-size - 1.0, -awn, -aae * size, -awn);
  this->RenderILine(-size - 1.0, awn, -aae * size, awn);
  this->RenderILine(-aae * size, -awn, -aae * size, -awt);
  this->RenderILine(-aae * size,  awn, -aae * size, awt);
  this->RenderILine(-aae * size, -awt, 0.0, 0.0);
  this->RenderILine(-aae * size,  awt, 0.0, 0.0);

  glPopMatrix();
  glPopMatrix();

#if wxCHECK_VERSION(3,0,0)
  this->GetFontNumeric()->draw(this->m_indRes * (-size) / 2.0 - 1.5 * lxFNTSW, this->m_indRes * (-1.0) - lxFNTSH, wxString::Format(_("%+03d\xc2\xb0"), int(this->setup->cam_tilt)));
#else
  this->GetFontNumeric()->draw(this->m_indRes * (-size) / 2.0 - 1.5 * lxFNTSW, this->m_indRes * (-1.0) - lxFNTSH, wxString::Format(_("%+03d\260"), int(this->setup->cam_tilt)));
#endif  
}


void lxGLCanvas::RenderIDepthbar(double size)
{

#define dbw 3.0
#define dbtw 0.8
#define dbnc 32

  int t;

  double clv[3], * clr;
  clr = this->data->luTable->GetRange();

  glShadeModel(GL_SMOOTH);
  glBegin(GL_QUAD_STRIP);
  for(t = 0; t <= dbnc; t++) {
    this->data->luTable->GetColor(clr[0] + t / double(dbnc) * (clr[1] - clr[0]), clv);
    glColor4f(clv[0], clv[1], clv[2], 1.0);
    v2r(0.0, double(t) * size / double(dbnc));
    v2r(dbw, double(t) * size / double(dbnc));
  }
  glEnd();
  glShadeModel(GL_FLAT);


  clrOutCntr();
  this->RenderILine(dbw, 0.0, dbw, size);

  char strCBar[10];
  for(t = 0; t <= 10; t++) {
    clrOutCntr();
    this->RenderILine(dbw, double(t) * size / 10.0, dbw + dbtw, double(t) * size / 10.0);
    if (this->frame->m_iniUnits == 1) {
      sprintf(&(strCBar[0]), "%.0f ft", (clr[0] + double(t) / 10.0 * (clr[1] - clr[0])) / 0.3048);
    } else {
      sprintf(&(strCBar[0]), "%.0f m", (clr[0] + double(t) / 10.0 * (clr[1] - clr[0])));
    }
    this->GetFontNumeric()->draw(this->m_indRes * dbw + lxFNTSW, this->m_indRes * (double(t) * size / 10.0) - 0.333 * lxFNTSH, strCBar);
  }

  // Depth-bar title
  this->GetFontNumeric()->draw(0.0, this->m_indRes * (size + 1.0) + 0.5 * lxFNTSH, wxString(_("Altitude")));

}


void lxGLCanvas::RenderIScalebar(double size)
{
#define sbh 2.0
#define sbt 1.5

  // upravime size tak, aby zodpovedala 1.0, 2.5 alebo 5.0
  // nasobku standardnej velkosti

  // pixel -> 1 meter???
  char strLen[32];
  double sblen, scale;
  int sbtest;
  // scale = m / pixel
  scale = 2.0 * this->setup->cam_dist * this->setup->cam_lens_vfovr / 
    (this->m_isO ? this->m_renderData->m_imgPixH : this->wh);


  if (this->frame->m_iniUnits == 1) {

    bool miles;
    miles = false;

    sbtest = int(floor(log((size * this->m_indRes) * scale / 0.3048) / log(pow(10.0,1.0/3.0))));
    sblen = pow(10.0,floor(double(sbtest)/3.0));
    switch (sbtest % 3) {
                case 0: sblen *= 1.0; break;
                case -2: case 1: sblen *= 2.5; break;
                default: sblen *= 5.0; break;
    }
    size = sblen * 0.3048 / scale / this->m_indRes;

    if (sbtest > 12) {
      size *= 1.6;
      miles = true;
      sbtest = int(floor(log((size * this->m_indRes) * scale / 1609.344) / log(pow(10.0,1.0/3.0))));
      if (sbtest < 0) sbtest = 0;
      sblen = pow(10.0,floor(double(sbtest)/3.0));
      switch (sbtest % 3) {
                  case 0: sblen *= 1.0; break;
                  case -2: case 1: sblen *= 2.5; break;
                  default: sblen *= 5.0; break;
      }
      size = sblen * 1609.344 / scale / this->m_indRes;
    }


    if (miles)
      sprintf(&(strLen[0]),"%.0f mi", sblen);
    else if (sblen > 4.0)
      sprintf(&(strLen[0]),"%.0f ft", sblen);
    else
      sprintf(&(strLen[0]),"%g in", 12.0 * sblen);

  } else {

    sbtest = int(floor(log((size * this->m_indRes) * scale) / log(pow(10.0,1.0/3.0))));
    sblen = pow(10.0,floor(double(sbtest)/3.0));
    switch (sbtest % 3) {
                case 0: sblen *= 1.0; break;
                case -2: case 1: sblen *= 2.5; break;
                default: sblen *= 5.0; break;
    }
    size = sblen / scale / this->m_indRes;

    if (sblen >= 10000.0)
      sprintf(&(strLen[0]),"%.0f km", sblen / 1000.0);
    else if (sblen >= 4.0)
      sprintf(&(strLen[0]),"%.0f m", sblen);
    else if (sblen >= 0.01)
      sprintf(&(strLen[0]),"%.0f mm", sblen * 1000.0);
    else
      sprintf(&(strLen[0]),"%g mm", sblen * 1000.0);
  }


  clrOutFill();
  glBegin(GL_QUADS);
  v2r(0.0 * size, 0.0);
  v2r(0.2 * size, 0.0);
  v2r(0.2 * size, sbh);
  v2r(0.0 * size, sbh);
  v2r(0.4 * size, 0.0);
  v2r(0.6 * size, 0.0);
  v2r(0.6 * size, sbh);
  v2r(0.4 * size, sbh);
  v2r(0.8 * size, 0.0);
  v2r(1.0 * size, 0.0);
  v2r(1.0 * size, sbh);
  v2r(0.8 * size, sbh);
  glEnd();


  clrOutCntr();
  this->RenderILine(0.0, 0.0, size, 0.0);
  this->RenderILine(0.0, sbh, size, sbh);
  this->RenderILine(       0.0, 0.0,        0.0, sbh + sbt);
  this->RenderILine(0.1 * size, sbh, 0.1 * size, sbh + sbt/3.0);
  this->RenderILine(0.2 * size, 0.0, 0.2 * size, sbh + sbt/3.0);
  this->RenderILine(0.3 * size, sbh, 0.3 * size, sbh + sbt/3.0);
  this->RenderILine(0.4 * size, 0.0, 0.4 * size, sbh + sbt/3.0);
  this->RenderILine(0.5 * size, sbh, 0.5 * size, sbh + 2.0*sbt/3.0);
  this->RenderILine(0.6 * size, 0.0, 0.6 * size, sbh + sbt/3.0);
  this->RenderILine(0.7 * size, sbh, 0.7 * size, sbh + sbt/3.0);
  this->RenderILine(0.8 * size, 0.0, 0.8 * size, sbh + sbt/3.0);
  this->RenderILine(0.9 * size, sbh, 0.9 * size, sbh + sbt/3.0);
  this->RenderILine(      size, 0.0,       size, sbh + sbt);

  this->GetFontNumeric()->draw(0.5 * size * this->m_indRes - 0.5 * double(strlen(strLen)) * lxFNTSW, (sbh + sbt + 1.0) * this->m_indRes, strLen);

}


void lxGLCanvas::RenderILine(double fx, double fy, double tx, double ty)
{

  bool rounded;
  if (this->m_isO) rounded = true;
  else rounded = false;

  if (this->m_isO) {
    lxVec n, nr;
    int t;
    n = lxVec(tx, ty, 0.0) - lxVec(fx, fy, 0.0);
    n.Normalize();
    n = hlw * lxVec(n.y, -n.x, n.z);

    glBegin(GL_QUADS);
    v2r(fx + n.x, fy + n.y);
    v2r(tx + n.x, ty + n.y);
    v2r(tx - n.x, ty - n.y);
    v2r(fx - n.x, fy - n.y);
    glEnd();

    if (rounded) {
      glBegin(GL_TRIANGLE_FAN);
      v2r(fx - n.x, fy - n.y);
      for(t = 0; t > -180; t -= 30) {
        nr = n.Rotated(double(t), 0.0);
        v2r(fx + nr.x, fy + nr.y);
      }
      glEnd();

      glBegin(GL_TRIANGLE_FAN);
      v2r(tx - n.x, ty - n.y);
      for(t = 0; t < 180; t += 30) {
        nr = n.Rotated(double(t), 0.0);
        v2r(tx + nr.x, ty + nr.y);
      }
      glEnd();

    }

  } else {

    glBegin(GL_LINES);
    v2r(fx, fy);
    v2r(tx, ty);
    glEnd();

  }
}



void lxGLCanvas::SetFontColors()
{
  if (this->m_isO && this->m_renderData->m_imgWhiteBg) {
    this->GetFontTitle()->setForegroundColor(0.0, 0.0, 0.0, 1.0);
    this->GetFontNumeric()->setForegroundColor(0.0, 0.0, 0.0, 1.0);
  } else {
    if (this->setup->cam_anaglyph) {
      this->GetFontTitle()->setForegroundColor(1.0, 1.0, 1.0, 1.0);
      this->GetFontNumeric()->setForegroundColor(1.0, 1.0, 1.0, 1.0);
    } else {
      this->GetFontTitle()->setForegroundColor(0.0, 0.8, 1.0, 1.0);
      this->GetFontNumeric()->setForegroundColor(0.0, 0.8, 1.0, 1.0);
    }
  }
}




struct OSC {

  int m_Width, m_Height;
  bool m_OK;

#ifdef LXWIN32
  R2DContext * m_r2d;
#endif
#ifdef LXLINUX
  R2PContext * m_r2p;
#endif


  OSC() {
#ifdef LXWIN32
    this->m_r2d = NULL;
#endif
#ifdef LXLINUX
    this->m_r2p = NULL;
#endif
    this->m_Width = 0;
    this->m_Height = 0;
    this->m_OK = false;
  }
};

bool lxGLCanvas::OSCMakeCurrent()
{

  // urobime context current
#ifdef LXWIN32  
  if (this->m_OSC->m_r2d) {
    R2DMakeCurrent(this->m_OSC->m_r2d);
    return true;
  }
#endif
#ifdef LXLINUX  
  if (this->m_OSC->m_r2p) {
    R2PMakeCurrent(this->m_OSC->m_r2p);
    return true;
  }
#endif
  return false;
}


bool lxGLCanvas::OSCInit(GLint w, GLint h)
{
  // vytvorime dib
  // vytvorime context
  this->m_OSC = new OSC();
  this->m_OSC->m_Width = w;
  this->m_OSC->m_Height = h;
  this->m_OSC->m_OK = true;

#ifdef LXWIN32
  this->m_OSC->m_r2d = R2DCreate(w, h);
#endif
#ifdef LXLINUX
  this->m_OSC->m_r2p = R2PCreate(w, h);
#endif
  return this->m_OSC->m_OK;
}


void lxGLCanvas::OSCDestroy()
{
  if (this->m_OSC != NULL) {
#ifdef LXWIN32
    if (this->m_OSC->m_r2d)
      R2DDestroy(this->m_OSC->m_r2d);
#endif
#ifdef LXLINUX
    if (this->m_OSC->m_r2p)
      R2PDestroy(this->m_OSC->m_r2p);
#endif
    delete this->m_OSC;
  }
  this->m_OSC = NULL;
}


struct _TRctx * lxGLCanvas::TRCGetContext()
{
  if (this->m_TRC != NULL)
    return this->m_TRC->m_ctx;
  else
    return NULL;
}



void lxGLCanvas::TRCInit(int type, GLint w, GLint h, GLint tw, GLint th)
{
  if (this->m_TRC == NULL) {

    this->m_TRC = new TRC();

    if (th < 64)
      th = 64;
    if (tw < 64)
      tw = 64;

    if (th > this->m_OSC->m_Height)
      th = this->m_OSC->m_Height;
    if (tw > this->m_OSC->m_Width)
      tw = this->m_OSC->m_Width;

    tw = tw - tw % 8;
    th = th - th % 8;

    TRcontext * ctx = this->m_TRC->m_ctx = trNew();
    GLubyte * buff = this->m_TRC->m_buff = new GLubyte [3 * sizeof(GLubyte) * (type == LXGLCTR_IMAGE ? w : tw) * (type == LXGLCTR_IMAGE ? h : th)];

    this->OSCMakeCurrent();
    this->OpenGLInit();

    trTileSize(ctx, tw, th, LXTRCBORDER);
    trImageSize(ctx, w, h);
    if (type == LXGLCTR_IMAGE)
      trImageBuffer(ctx, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) buff);
    else
      trTileBuffer(ctx, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) buff);
  }

}

GLint lxGLCanvas::TRCGet(int param)
{
  if (this->m_TRC != NULL)
    return trGet(this->m_TRC->m_ctx, (TRenum) param);
  else
    return -1;
}

GLubyte * lxGLCanvas::TRCGetBuffer()
{
  if (this->m_TRC != NULL)
    return this->m_TRC->m_buff;
  else
    return (GLubyte *) NULL;
}

void lxGLCanvas::TRCDestroy()
{
  if (this->m_TRC != NULL) {
    delete [] this->m_TRC->m_buff;
    trDelete(this->m_TRC->m_ctx);
    delete this->m_TRC;
  }
  this->m_TRC = NULL;
}

void lxGLCanvas::TRCBeginTile()
{
  if (this->m_TRC != NULL) {
    this->OSCMakeCurrent();
    trBeginTile(this->m_TRC->m_ctx);
  }
}

bool lxGLCanvas::TRCEndTile()
{
  if (this->m_TRC != NULL)
    return (trEndTile(this->m_TRC->m_ctx) > 0);
  else
    return false;
}





