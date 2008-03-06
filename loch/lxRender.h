/**
 * @file lxRender.h
 * Loch printing module.
 */

#ifndef lxRender_h
#define lxRender_h  

// Standard libraries
#ifndef LXDEPCHECK
#include <wx/wx.h>
#endif  
//LXDEPCHECK - standart libraries

enum {
	LXRENDER_FIT_SCREEN,
	LXRENDER_FIT_WIDTH,
	LXRENDER_FIT_HEIGHT,
	LXRENDER_FIT_SCALE,
};

struct lxRenderData {

	long m_scaleMode;
  bool m_printRenderer, 
		m_imgWhiteBg,
    m_askFName;

  wxString m_imgFileName;
  int m_imgFileType;

  double m_scale, 
		m_imgResolution,
    m_imgWidth, 
		m_imgHeight,
		m_imgPixW,
		m_imgPixH;

  lxRenderData();

  void Configure(wxWindow * parent, class lxGLCanvas * glc);
  
  void Render(wxWindow * parent, class lxGLCanvas * glc);

	void CalcImageDimensions(class lxGLCanvas * glc);

};

#endif


