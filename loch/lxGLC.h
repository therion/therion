/**
 * @file lxGLC.h
 * Loch GL canvas.
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

#ifndef lx_h
#define lx_h

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/glcanvas.h>
#include <wx/timer.h>
#include <wx/image.h>
#include "lxOGLFT.h"
#endif  
//LXDEPCHECK - standart libraries

enum {
  LXGLCML_NONE,
  LXGLCML_ZOOM2ROTATE,
  LXGLCML_ZOOM,
  LXGLCML_ROTATE,
  LXGLCML_TILT,
  LXGLCML_PANX,
  LXGLCML_PANY,
  LXGLCML_PANX2Y,
  LXGLCTR_TILE,
  LXGLCTR_IMAGE
};

#include "lxMath.h"

class lxGLCanvas: public wxGLCanvas {

  public:
  
    lxVec shift;
    long ww, wh, mx, my;
    struct lxRenderData * m_renderData;
    
    struct lxData * data;
    struct lxSetup * setup;   
    class lxFrame * frame;

    void UpdateContents();
    
    lxGLCanvas(struct lxSetup * stp, struct lxData * dat, 
        wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = _T("lxGLCanvas") );

    ~lxGLCanvas();

    GLuint m_idTexSurface, m_idTexStation, m_initTextures;

    // screen funkcie a premenne
    GLuint m_sList, m_oList, m_sFixList, m_sEntList, m_sStList,
      m_oFixList, m_oEntList, m_oStList;
    int m_sMoveLock;
    double m_indRes, m_indLWidth;
    bool m_sInit, m_sInitReset;
    bool m_sMoveSingle, m_isO;

    // fonty
    FT_Face m_ftFace1, m_ftFace2, m_ftFace3;
    OGLFT::Monochrome * m_fntNumericS,
      * m_fntTitleS;
    OGLFT::Filled * m_fntNumericO,
      * m_fntTitleO;

    GLdouble m_camera_modelview[16];
    GLdouble m_camera_projection[16];
    GLint m_camera_viewport[4];

		bool m_sCameraAutoRotate, m_sCameraLockRotation;
    wxStopWatch m_sCameraAutoRotateSWatch;
    long m_sCameraAutoRotateCounter;
    double m_sCameraAutoRotateAngle, m_sCameraStartAutoRotateAngle;

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    
    void OnMouseDouble(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKeyPress(wxKeyEvent& event);
    void OnIdle(wxIdleEvent& event);
		bool CameraAutoRotate();
     
    void InitializeS();
    void ForceRefresh(bool updateTB = true);
    void UpdateRenderContents();
    void UpdateRenderList();
	  void RenderScreen();
    void SetIndicatorsTransform();
    void ProjectPoint(double src_x, double src_y, double src_z, GLdouble * x, GLdouble * y, GLdouble * z);
    void ProjectStations();
    void RenderOffList();

    // offscreen funkcie a premenne
    struct OSC * m_OSC;
    struct TRC * m_TRC;

    GLint m_maxTSizeS, m_maxTSizeO;

    bool OSCMakeCurrent();
    void OSCDestroy();
    bool OSCInit(GLint w, GLint h);

    void TRCInit(int type, GLint w, GLint h, GLint tw = 0, GLint th = 0);
    struct _TRctx * TRCGetContext();
    void TRCDestroy();
    GLint TRCGet(int param);
    GLubyte * TRCGetBuffer();
    void TRCBeginTile();
    bool TRCEndTile();

    void SetColorMask();

    long m_lic;

  DECLARE_EVENT_TABLE()
  
    public:

    bool IsRenderingOff();
    OGLFT::Face * GetFontNumeric();
    OGLFT::Face * GetFontTitle();

    void OpenGLInit();
    void SetCamera();
    void RenderAll();
    void SetFontColors();
    void RenderSurface();
    void GeomOutline();
    void RenderCenterline();
    void RenderScrapWalls();
    void RenderInds();
    void RenderICompass(double size);
    void RenderIClino(double size);
    void RenderIScalebar(double size);
    void RenderIDepthbar(double size);
    void RenderILine(double fx, double fy, double tx, double ty);
  
}; // lxGLCanvas


struct TRC {
  struct _TRctx * m_ctx;
  GLubyte * m_buff;
  TRC() {
    this->m_ctx = NULL;
    this->m_buff = NULL;
  }
};


#endif
