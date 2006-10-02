// Standard libraries
#ifndef LXDEPCHECK
#include <stdio.h>
#endif  
//LXDEPCHECK - standart libraries

#include "lxGLC.h"
#ifdef LXWIN32
#include "lxR2D.h"
#endif
#ifdef LXLINUX
#include "lxR2P.h"
#endif
#define LXTRCBORDER (this->m_renderData->m_scaleMode == LXRENDER_FIT_SCREEN ? 0 : 16)

// implementacia common funkcii
#define LXOFFSCREENIMPL 1
#define LXGLCX_INFILE
#include "lxGLCx.cxx"

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
    this->OpenGLInitO();

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


