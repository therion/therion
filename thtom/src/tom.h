#ifndef _TOM_H_
#define _TOM_H_

#include <tcl.h>
#include <tk.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tkPlatDecls.h>
#undef WIN32_LEAN_AND_MEAN
#else
#include <GL/glx.h>
#endif /* WIN32 */
#include <GL/gl.h>
#include <GL/glu.h>
#ifdef GLUT
#include <GL/glut.h>
#endif
#ifdef MINGW
#include <GL/glext.h>
#endif

#define TOM_VALUE 0x01
#define TOM_ASCII 0x02

#define TOM_ALPHA_MASK (1 << 0)
#define TOM_ACCUM_MASK (1 << 1)
#define TOM_DEPTH_MASK (1 << 2)
#define TOM_STENCIL_MASK (1 << 3)
#define TOM_DOUBLE_MASK (1 << 4)
#define TOM_DIRECT_MASK (1 << 5)

#define TOM_ALPHA(tom) ((tom)->mask & TOM_ALPHA_MASK)
#define TOM_ACCUM(tom) ((tom)->mask & TOM_ACCUM_MASK)
#define TOM_DEPTH(tom) ((tom)->mask & TOM_DEPTH_MASK)
#define TOM_STENCIL(tom) ((tom)->mask & TOM_STENCIL_MASK)
#define TOM_DOUBLE(tom) ((tom)->mask & TOM_DOUBLE_MASK)
#define TOM_DIRECT(tom) ((tom)->mask & TOM_DIRECT_MASK)

typedef struct {
    Tk_Window tkwin;
    char *name;
    Tcl_Interp *interp;
    int x, y;
    int width, height;
    Display *display;
#ifdef WIN32
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
#else
    GC gc;
    Colormap colormap;
    XVisualInfo *vi;
    GLXContext context;
#endif                          /* WIN32 */
    int mask;
    int doublebuffer;
    int alpha;
    int depth;
    int direct;
    int current;
    int swt;
    char *share;
} TOM;

extern int Tom_TOMInit(Tcl_Interp *interp);
extern int Tom_GLDataInit(Tcl_Interp *interp);
extern int Tom_GetNParams(GLenum pname);
extern int Tom_GetMaterialNParams(GLenum pname);
extern int Tom_GetLightNParams(GLenum pname);
extern int Tom_GetLightModelNParams(GLenum pname);
extern int Tom_GetFogNParams(GLenum pname);
extern int Tom_GetTexNParams(GLenum pname);
extern int Tom_GetTypeSize(GLenum type);
extern int Tom_GetFormatSize(GLenum format);
extern Tcl_HashEntry *Tom_CreateHashEntry(char *key, ClientData clientData);
extern Tcl_HashEntry *Tom_FindQEntry(Tcl_Interp *interp, int qobj);
extern Tcl_HashEntry *Tom_FindNEntry(Tcl_Interp *interp, int nobj);
extern Tcl_HashEntry *Tom_FindWEntry(Tcl_Interp *interp, char *path);
extern int Tom_ParseSwitches(int objc, Tcl_Obj * CONST objv[], int *swt, int flags);

extern int Tom_GetDoubleFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLdouble *d);
extern int Tom_GetFloatFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLfloat *f);
extern int Tom_GetIntFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLint *i);
extern int Tom_GetUIntFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLuint *i);
extern int Tom_GetEnumFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLenum *e);
extern int Tom_GetSizeFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLsizei *s);
extern int Tom_GetShortFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLshort *s);
extern int Tom_GetUShortFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLushort *s);
extern int Tom_GetByteFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLbyte *b);
extern int Tom_GetUByteFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLbyte *b);
extern int Tom_GetBooleanFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLboolean *b);
extern int Tom_GetBitfieldFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLbitfield *b);
extern void *Tom_GetByteArrayFromObj(Tcl_Interp *interp, Tcl_Obj *obj, int size, int swt);
extern int Tom_FormatObj(Tcl_Interp *interp, Tcl_Obj *obj, void *buffer, int num, GLenum type, int swt);
extern int Tom_FormatPixels(Tcl_Interp *interp, Tcl_Obj *obj, void *pixels,
                            int width, int height, int num, GLenum type, int scan, int swt);
extern Tcl_Obj *Tom_NewList(Tcl_Interp *interp, void *b, int num, GLenum type, int swt);
extern Tcl_Obj *Tom_NewPixels(Tcl_Interp *interp, void *pixels, int width,
                              int height, int num, GLenum type, int scan, int swt);

/* if the BUILD_TOM macro is defined, the assumption is that we are
 * building the dynamic library */
#ifdef WIN32
#ifdef BUILD_TOM
#define TOM_API __declspec(dllexport)
#else
#define TOM_API __declspec(dllimport)
#endif
#else
#define TOM_API
#endif

/* Function prototypes for this module */
extern TOM_API int Tom_Init(Tcl_Interp *interp);

/* Commands functions */
extern int Tom_WidgetLoadCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_WidgetLoadCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);

/* OpenGL functions */
extern int Tom_glAccum(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glAlphaFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glBegin(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glBitmap(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glBlendFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glCallList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glCallLists(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClear(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClearAccum(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClearColor(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClearDepth(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClearIndex(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClearStencil(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glClipPlane(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3b(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3ub(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3ui(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3us(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4b(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4ub(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4ui(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4us(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3bv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3ubv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3uiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor3usv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4bv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4ubv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4uiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColor4usv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColorMask(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glColorMaterial(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glCopyPixels(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glCullFace(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDeleteLists(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDepthFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDepthMask(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDepthRange(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDrawBuffer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDrawPixels(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glDisable(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEdgeFlag(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEnable(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEnd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEndList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalCoord1d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalCoord1f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalCoord2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalCoord2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalMesh1(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalMesh2(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalPoint1(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glEvalPoint2(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glFinish(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glFlush(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glFogfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glFogiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glFrontFace(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glFrustum(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGenLists(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetError(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetBooleanv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetDoublev(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetFloatv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetIntegerv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetMaterialfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetMaterialiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glGetString(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glHint(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glIsList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glIsEnabled(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLightfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLightiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLightModelfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLightModeliv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLineStipple(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLineWidth(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLoadIdentity(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLoadMatrixd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLoadMatrixf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMaterialfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMap1d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMap1f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMap2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMap2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMapGrid1d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMapGrid1f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMapGrid2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMapGrid2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMaterialiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMatrixMode(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMultMatrixd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glMultMatrixf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNewList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3b(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3bv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glNormal3sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glOrtho(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPixelStoref(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPixelStorei(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPixelTransferf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPixelTransferi(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPixelZoom(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPointSize(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPolygonMode(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPopMatrix(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPushMatrix(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPopAttrib(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPushAttrib(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRotated(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRotatef(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos2sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos3sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRasterPos4sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glReadPixels(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glScaled(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glScalef(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glShadeModel(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glStencilMask(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glStencilFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTranslated(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTranslatef(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord2i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord4d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord4f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexCoord4i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexEnvf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexEnvi(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexEnvfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexEnviv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexImage1D(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexImage2D(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexParameterf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glTexParameteri(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex2i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex2dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex2fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex2iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glVertex3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glViewport(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glSelectBuffer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRenderMode(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glInitNames(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glLoadName(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPushName(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glPopName(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRectd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRectf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRecti(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRects(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRectdv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRectfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRectiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glRectsv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glReadBuffer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glScissor(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glStencilOp(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);

/* GLU functions */
extern int Tom_gluBeginCurve(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluBeginSurface(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluBeginTrim(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluDeleteNurbsRenderer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluDeleteQuadric(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluEndCurve(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluEndSurface(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluEndTrim(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluLookAt(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluNewNurbsRenderer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluNewQuadric(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluNurbsProperty(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluNurbsSurface(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluOrtho2D(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluPerspective(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluPickMatrix(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluQuadricTexture(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_gluSphere(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);

#ifdef GLUT
/* GLUT functions */
extern int Tom_glutSolidCone(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutWireCone(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutSolidDodecahedron(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutWireDodecahedron(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutSolidCube(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutWireCube(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutSolidSphere(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutWireSphere(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutSolidTeapot(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
extern int Tom_glutWireTeapot(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
#endif /* GLUT */

#endif
