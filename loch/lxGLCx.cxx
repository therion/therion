#ifdef LXGLCX_INFILE

// Standard libraries
#ifndef LXDEPCHECK
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <math.h>
#ifdef __WXMAC__
#   ifdef __DARWIN__
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#   else
#       include <gl.h>
#       include <glu.h>


#   endif
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif
#endif  
//LXDEPCHECK - standart libraries

#include "lxSetup.h"
#include "lxRender.h"


#ifdef LXCONTEXT
#undef LXCONTEXT
#endif

#ifdef LXOFFSCREENIMPL

#define   LXCONTEXT(x) x##O
#define   LXOFFSCREEN 1
#include  "lxTR.h"

#else
// ONSCREEN

#define LXCONTEXT(x) x##S
#endif
// ONSCREEN / OFFSCREEN switch

#ifdef LXOFFSCREENIMPL
#define lxFNTSH (this->m_fntNumericO->pointSize() * this->m_fntNumericO->resolution() / 72.0)
#define lxFNTSW (0.55 * lxFNTSH)
#define lxFNTLH (this->m_fntTitleO->pointSize() * this->m_fntTitleO->resolution() / 72.0)
#define lxFNTLW (0.55 * lxFNTLH)
#else
#define lxFNTSH 13.0
#define lxFNTSW  6.0
#define lxFNTLH 20.0
#define lxFNTLW 10.0
#endif
void lxGLCanvas::LXCONTEXT(SetCamera) () {

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
#ifndef LXOFFSCREEN
    gluPerspective(this->setup->cam_lens_vfov, this->setup->cam_width, minclip, maxclip);
#else
    if (this->TRCGetContext()) {
      trPerspective(this->TRCGetContext(), this->setup->cam_lens_vfov, this->setup->cam_width, minclip, maxclip);
    } else {
      gluPerspective(this->setup->cam_lens_vfov, this->setup->cam_width, minclip, maxclip);
    }
#endif   
  } else {
    vclip = this->setup->cam_dist * this->setup->cam_lens_vfovr;
    hclip = vclip * this->setup->cam_width;
    //printf("x clip: %12.2f%12.2f\n", -hclip, hclip);
    //printf("y clip: %12.2f%12.2f\n", -vclip, vclip);
#ifndef LXOFFSCREEN
    glOrtho(-hclip, hclip, -vclip, vclip, minclip, maxclip);
#else
    if (this->TRCGetContext() != NULL) {
      trOrtho(this->TRCGetContext(), -hclip, hclip, -vclip, vclip, minclip, maxclip);
    } else {
      glOrtho(-hclip, hclip, -vclip, vclip, minclip, maxclip);
    }
#endif   
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
}

static const GLfloat mat0[] = { 0.0, 0.0, 0.0, 1.0};
static const GLfloat mat1[] = { 1.0, 1.0, 1.0, 1.0};
static const GLfloat mat2[] = { 0.5};
static const GLfloat mat3[] = { 1.0, 1.0, 1.0, 0.5};

void lxGLCanvas::LXCONTEXT(RenderScrapWalls) () {

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
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, clr);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat2);
  
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
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, clr); \
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
  
}


void lxGLCanvas::LXCONTEXT(RenderSurface) () {

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
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV,  GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, this->m_idTexSurface);
  } else {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
  }

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




void lxGLCanvas::LXCONTEXT(RenderCenterline) () {
#ifdef LXOFFSCREEN
	glLineWidth(this->m_renderData->m_scaleMode == LXRENDER_FIT_SCREEN ? 1.0 : this->m_renderData->m_imgResolution / 96.0);
#else
  glLineWidth(1.0);
#endif
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


void lxGLCanvas::LXCONTEXT(GeomOutline) () {

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


void lxGLCanvas::LXCONTEXT(RenderAll) () {

#ifdef LXOFFSCREEN
  if ((this->m_renderData != NULL) && (this->m_renderData->m_scaleMode != LXRENDER_FIT_SCREEN) && this->m_renderData->m_imgWhiteBg)
    glClearColor(1.0, 1.0, 1.0, 0.0);
  else
    glClearColor(0.0, 0.0, 0.0, 0.0);
#else
  glClearColor(0.0, 0.0, 0.0, 0.0);
#endif
  glClearDepth(1.0);
  if (this->setup->cam_anaglyph)
    glClear(GL_DEPTH_BUFFER_BIT);
  else 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // vyrendruje outline
  glDisable(GL_LIGHTING);
  glShadeModel(GL_FLAT);
  glDepthMask(GL_TRUE);
#ifdef LXOFFSCREEN
	glLineWidth(this->m_renderData->m_scaleMode == LXRENDER_FIT_SCREEN ? 1.0 : this->m_renderData->m_imgResolution / 96.0);
#else
  glLineWidth(1.0);
#endif
  glDisable(GL_BLEND);
  if (this->setup->cam_anaglyph)
    glColor3f(0.7, 0.7, 0.7);
  else
    glColor3f(1.0, 0.0, 0.0);

  if (this->setup->m_vis_bbox)
    this->LXCONTEXT(GeomOutline)();
  if (this->setup->m_vis_centerline)
    this->LXCONTEXT(RenderCenterline)();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  if (this->setup->m_vis_walls)
    this->LXCONTEXT(RenderScrapWalls)();  
  glDisable(GL_CULL_FACE);

  if (this->setup->m_vis_surface)
    this->LXCONTEXT(RenderSurface)();
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
	glShadeModel(GL_FLAT);

}


void lxGLCanvas::LXCONTEXT(OpenGLInit) () {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void lxGLCanvas::LXCONTEXT(RenderInds)()
{

#ifdef LXOFFSCREENIMPL
  this->m_indRes = this->m_renderData->m_imgResolution / 25.4; // pixels per mm
  this->m_indLWidth = 0.5;    // line width mm
  if ((this->m_indLWidth * this->m_indRes) < 1.0) {
    this->m_indLWidth = 1.0 / this->m_indRes;    // line width mm
  }
#else
  this->m_indRes = 3.7795276; // pixels per mm
  this->m_indLWidth = 1.0 / this->m_indRes;    // line width mm
  glLineWidth(1.0);
#endif

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#ifdef LXOFFSCREENIMPL
  trOrtho(this->TRCGetContext(), 0.0, this->m_renderData->m_imgPixW, 0.0, this->m_renderData->m_imgPixH, -1.0, 1.0);
  this->TRCBeginTile();
#else
  gluOrtho2D(0.0, this->ww, 0.0, this->wh);
#endif

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glLoadIdentity();
  glTranslatef(this->m_indRes * 9.0,  this->m_indRes * 18.0, 0.0);
  this->LXCONTEXT(RenderICompass)(5.0);

  glLoadIdentity();
  glTranslatef(this->m_indRes * 28.0, this->m_indRes * 13.0, 0.0);
  this->LXCONTEXT(RenderIClino)(8.0);

  glLoadIdentity();
  glTranslatef(this->m_indRes * 34.0, this->m_indRes * 9.0, 0.0);
  this->LXCONTEXT(RenderIScalebar)(50.0);

  if ((!this->setup->cam_anaglyph) && (this->setup->m_colormd != lxSETUP_COLORMD_DEFAULT)) {
    glLoadIdentity();
    glTranslatef(this->m_indRes * 4.0, this->m_indRes * 29.0, 0.0);
    this->LXCONTEXT(RenderIDepthbar)(40.0);
  }

  glLoadIdentity();
  this->LXCONTEXT(m_fntTitle)->draw(this->m_indRes * 3.0, this->m_indRes * 3.0, this->data->title);

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}


#ifdef LXOFFSCREENIMPL
#define clrTemplate(AW,AB,NL) {\
  if (this->setup->cam_anaglyph) {\
    if (this->m_renderData->m_imgWhiteBg)\
      glColor4f AW; \
    else \
      glColor4f AB; \
  } else \
    glColor4f NL; \
}
#else
#define clrTemplate(AW,AB,NL) {\
  if (this->setup->cam_anaglyph) \
    glColor4f AB; \
  else \
    glColor4f NL; \
}
#endif

#define clrOutCntr() clrTemplate((0.0, 0.0, 0.0, 1.0),(1.0, 1.0, 1.0, 1.0),(0.1, 0.1, 1.0, 1.0)) 
#define clrOutFill() clrTemplate((0.5, 0.5, 0.5, 0.5),(0.5, 0.5, 0.5, 0.5),(0.0, 0.0, 1.0, 0.5))
#define clrIndCntr() clrTemplate((0.0, 0.0, 0.0, 1.0),(1.0, 1.0, 1.0, 1.0),(1.0, 0.0, 0.0, 1.0))
#define clrIndFill() clrTemplate((0.5, 0.5, 0.5, 1.0),(0.5, 0.5, 0.5, 1.0),(1.0, 0.8, 0.0, 1.0))

#define v2r(x,y) glVertex2f(this->m_indRes * (x), this->m_indRes * (y))
#define hlw (this->m_indLWidth / 2.0)


void lxGLCanvas::LXCONTEXT(RenderICompass)(double size)
{
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
#ifdef LXOFFSCREENIMPL
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
#else
  glBegin(GL_LINE_STRIP);
  for(t = 0; t <= 360; t++) {
    v2r(
      (size - hlw) * sin(double(t)/180.0*lxPI),
      (size - hlw) * cos(double(t)/180.0*lxPI));
  }
  glEnd();
#endif

  this->LXCONTEXT(RenderILine)(0.0, -size - 0.75, 0.0, -size + 1.0);
  this->LXCONTEXT(RenderILine)(-size - 0.75, 0.0, -size + 1.0, 0.0);
  this->LXCONTEXT(RenderILine)( size - 1.0, 0.0,  size + 0.75, 0.0);

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
  this->LXCONTEXT(RenderILine)(a1.x, a1.y, a2.x, a2.y);
  this->LXCONTEXT(RenderILine)(a1.x, a1.y, a3.x, a3.y);
  this->LXCONTEXT(RenderILine)(a4.x, a4.y, a2.x, a2.y);
  this->LXCONTEXT(RenderILine)(a4.x, a4.y, a3.x, a3.y);

  glPopMatrix();

  //static char strFacing[6];
  //sprintf(&(strFacing[0]),"%03d", int(this->setup->cam_dir));
  //this->LXCONTEXT(m_fntNumeric)->draw((-2.0) * lxFNTSW, this->m_indRes * (-size - 1.0) - lxFNTSH, strFacing);
  this->LXCONTEXT(m_fntNumeric)->draw((-2.0) * lxFNTSW, this->m_indRes * (-size - 1.0) - lxFNTSH, wxString::Format(_("%03d\260"), int(this->setup->cam_dir)));
  

}


void lxGLCanvas::LXCONTEXT(RenderIClino)(double size)
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

#ifdef LXOFFSCREENIMPL
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
#else
  glBegin(GL_LINE_STRIP);
  for(t = 0; t <= 90; t += 5) {
    v2r(
      (-1.0) * (size) * cos(double(t)/180.0*lxPI),
      (neg ? -1.0 : 1.0) * (size) * sin(double(t)/180.0*lxPI));
  }
  glEnd();
#endif

  this->LXCONTEXT(RenderILine)(-size - 1.0, 0.0, 1.0, 0.0);
  this->LXCONTEXT(RenderILine)(0.0, (neg ? 1.0 : -1.0), 0.0, (neg ? -size - 1.0 : size + 1.0));
  this->LXCONTEXT(RenderILine)(-size, (neg ? 1.0 : -1.0), -size, 0.0);
  this->LXCONTEXT(RenderILine)(0.0, (neg ? -size : size), 1.0, (neg ? -size : size));

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
  this->LXCONTEXT(RenderILine)(-size - 1.0, -awn, -size - 1.0, awn);
  this->LXCONTEXT(RenderILine)(-size - 1.0, -awn, -aae * size, -awn);
  this->LXCONTEXT(RenderILine)(-size - 1.0, awn, -aae * size, awn);
  this->LXCONTEXT(RenderILine)(-aae * size, -awn, -aae * size, -awt);
  this->LXCONTEXT(RenderILine)(-aae * size,  awn, -aae * size, awt);
  this->LXCONTEXT(RenderILine)(-aae * size, -awt, 0.0, 0.0);
  this->LXCONTEXT(RenderILine)(-aae * size,  awt, 0.0, 0.0);

  glPopMatrix();
  glPopMatrix();

  //static char strClino[5];
  //sprintf(&(strClino[0]),"%+03d", int(this->setup->cam_tilt));
  //this->LXCONTEXT(m_fntNumeric)->draw(this->m_indRes * (-size) / 2.0 - 1.5 * lxFNTSW, this->m_indRes * (-1.0) - lxFNTSH, strClino);
  this->LXCONTEXT(m_fntNumeric)->draw(this->m_indRes * (-size) / 2.0 - 1.5 * lxFNTSW, this->m_indRes * (-1.0) - lxFNTSH, wxString::Format(_("%+03d\260"), int(this->setup->cam_tilt)));
}


void lxGLCanvas::LXCONTEXT(RenderIDepthbar)(double size)
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
  this->LXCONTEXT(RenderILine)(dbw, 0.0, dbw, size);

  char strCBar[10];
  for(t = 0; t <= 10; t++) {
    clrOutCntr();
    this->LXCONTEXT(RenderILine)(dbw, double(t) * size / 10.0, dbw + dbtw, double(t) * size / 10.0);
    sprintf(&(strCBar[0]), "%.0f m", clr[0] + double(t) / 10.0 * (clr[1] - clr[0]));
    this->LXCONTEXT(m_fntNumeric)->draw(this->m_indRes * dbw + lxFNTSW, this->m_indRes * (double(t) * size / 10.0) - 0.333 * lxFNTSH, strCBar);
  }

  // Depth-bar title
  this->LXCONTEXT(m_fntNumeric)->draw(0.0, this->m_indRes * (size + 1.0) + 0.5 * lxFNTSH, wxString(_T("Altitude")));


}


void lxGLCanvas::LXCONTEXT(RenderIScalebar)(double size)
{
#define sbh 2.0
#define sbt 1.5

  // upravime size tak, aby zodpovedala 1.0, 2.5 alebo 5.0
  // nasobku standardnej velkosti

  // pixel -> 1 meter???
  double sblen, scale;
  int sbtest;
  // scale = m / pixel
  scale = 2.0 * this->setup->cam_dist * this->setup->cam_lens_vfovr / 
#ifdef LXOFFSCREENIMPL
    this->m_renderData->m_imgPixH;
#else
    this->wh;
#endif
  sbtest = int(floor(log((size * this->m_indRes) * scale) / log(pow(10.0,1.0/3.0))));
  sblen = pow(10.0,floor(double(sbtest)/3.0));
  switch (sbtest % 3) {
              case 0: sblen *= 1.0; break;
     case -2: case 1: sblen *= 2.5; break;
             default: sblen *= 5.0; break;
  }
  size = sblen / scale / this->m_indRes;
  
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
  this->LXCONTEXT(RenderILine)(0.0, 0.0, size, 0.0);
  this->LXCONTEXT(RenderILine)(0.0, sbh, size, sbh);
  this->LXCONTEXT(RenderILine)(       0.0, 0.0,        0.0, sbh + sbt);
  this->LXCONTEXT(RenderILine)(0.1 * size, sbh, 0.1 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.2 * size, 0.0, 0.2 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.3 * size, sbh, 0.3 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.4 * size, 0.0, 0.4 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.5 * size, sbh, 0.5 * size, sbh + 2.0*sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.6 * size, 0.0, 0.6 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.7 * size, sbh, 0.7 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.8 * size, 0.0, 0.8 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(0.9 * size, sbh, 0.9 * size, sbh + sbt/3.0);
  this->LXCONTEXT(RenderILine)(      size, 0.0,       size, sbh + sbt);

  char strLen[32];
  if (sblen >= 10000.0)
    sprintf(&(strLen[0]),"%.0f km", sblen / 1000.0);
  else if (sblen >= 4.0)
    sprintf(&(strLen[0]),"%.0f m", sblen);
  else if (sblen >= 0.01)
    sprintf(&(strLen[0]),"%.0f mm", sblen * 1000.0);
  else
    sprintf(&(strLen[0]),"%g mm", sblen * 1000.0);
  this->LXCONTEXT(m_fntNumeric)->draw(0.5 * size * this->m_indRes - 0.5 * double(strlen(strLen)) * lxFNTSW, (sbh + sbt + 1.0) * this->m_indRes, strLen);

}

void lxGLCanvas::LXCONTEXT(RenderILine)(double fx, double fy, double tx, double ty, bool
#ifdef LXOFFSCREENIMPL
rounded
#endif
)
{
#ifdef LXOFFSCREENIMPL
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

#else

  glBegin(GL_LINES);
  v2r(fx, fy);
  v2r(tx, ty);
  glEnd();

#endif
}


void lxGLCanvas::LXCONTEXT(SetFontColors) ()
{
#ifdef LXOFFSCREENIMPL
  if (this->m_renderData->m_imgWhiteBg) {
    this->LXCONTEXT(m_fntTitle)->setForegroundColor(0.0, 0.0, 0.0, 1.0);
    this->LXCONTEXT(m_fntNumeric)->setForegroundColor(0.0, 0.0, 0.0, 1.0);
  } else {
#endif
    if (this->setup->cam_anaglyph) {
      this->LXCONTEXT(m_fntTitle)->setForegroundColor(1.0, 1.0, 1.0, 1.0);
      this->LXCONTEXT(m_fntNumeric)->setForegroundColor(1.0, 1.0, 1.0, 1.0);
    } else {
      this->LXCONTEXT(m_fntTitle)->setForegroundColor(0.0, 0.8, 1.0, 1.0);
      this->LXCONTEXT(m_fntNumeric)->setForegroundColor(0.0, 0.8, 1.0, 1.0);
    }
#ifdef LXOFFSCREENIMPL
  }
#endif
}







#endif

