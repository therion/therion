#include <stdlib.h>
#include <string.h>
#include "tom.h"

static TOM *TOMCurrent = NULL;
static Tcl_HashTable TOMHashTable;

/*
 * forward declarations 
 */
static int Tom_TOMCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
static int Tom_GLCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);
static int Tom_InitContext(Tcl_Interp *interp, TOM *tom);
static int Tom_MakeCurrent(Tcl_Interp *interp, TOM *tom);
static int Tom_Configure(Tcl_Interp *interp, TOM *tom, int objc, Tcl_Obj * CONST objv[], int flags);
static void Tom_EventProc(ClientData clientData, XEvent *event);
static void Tom_Destroy(ClientData clientData);
static void Tom_DeleteCommand(ClientData clientData);
static int Tom_WidgetCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);

#ifdef DEBUG
static int Tom_Test(Tcl_Interp *interp);
#endif /* DEBUG */

typedef int Tom_GLProc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]);

typedef struct {
    char *cmdName;
    Tom_GLProc *proc;
} TomCmdDecl;

static TomCmdDecl TOM_COMMANDS[] = {
    {"::GL::glAccum", Tom_glAccum},
    {"::GL::glAlphaFunc", Tom_glAlphaFunc},
    {"::GL::glBegin", Tom_glBegin},
    {"::GL::glBitmap", Tom_glBitmap},
    {"::GL::glBlendFunc", Tom_glBlendFunc},
    {"::GL::glCallList", Tom_glCallList},
    {"::GL::glCallLists", Tom_glCallLists},
    {"::GL::glClear", Tom_glClear},
    {"::GL::glClearAccum", Tom_glClearAccum},
    {"::GL::glClearColor", Tom_glClearColor},
    {"::GL::glClearDepth", Tom_glClearDepth},
    {"::GL::glClearIndex", Tom_glClearIndex},
    {"::GL::glClearStencil", Tom_glClearStencil},
    {"::GL::glClipPlane", Tom_glClipPlane},
    {"::GL::glColor3b", Tom_glColor3b},
    {"::GL::glColor3d", Tom_glColor3d},
    {"::GL::glColor3f", Tom_glColor3f},
    {"::GL::glColor3i", Tom_glColor3i},
    {"::GL::glColor3s", Tom_glColor3s},
    {"::GL::glColor3ub", Tom_glColor3ub},
    {"::GL::glColor3ui", Tom_glColor3ui},
    {"::GL::glColor3us", Tom_glColor3us},
    {"::GL::glColor4b", Tom_glColor4b},
    {"::GL::glColor4d", Tom_glColor4d},
    {"::GL::glColor4f", Tom_glColor4f},
    {"::GL::glColor4i", Tom_glColor4i},
    {"::GL::glColor4s", Tom_glColor4s},
    {"::GL::glColor4ub", Tom_glColor4ub},
    {"::GL::glColor4ui", Tom_glColor4ui},
    {"::GL::glColor4us", Tom_glColor4us},
    {"::GL::glColor3bv", Tom_glColor3bv},
    {"::GL::glColor3dv", Tom_glColor3dv},
    {"::GL::glColor3fv", Tom_glColor3fv},
    {"::GL::glColor3iv", Tom_glColor3iv},
    {"::GL::glColor3sv", Tom_glColor3sv},
    {"::GL::glColor3ubv", Tom_glColor3ubv},
    {"::GL::glColor3uiv", Tom_glColor3uiv},
    {"::GL::glColor3usv", Tom_glColor3usv},
    {"::GL::glColor4bv", Tom_glColor4bv},
    {"::GL::glColor4dv", Tom_glColor4dv},
    {"::GL::glColor4fv", Tom_glColor4fv},
    {"::GL::glColor4iv", Tom_glColor4iv},
    {"::GL::glColor4sv", Tom_glColor4sv},
    {"::GL::glColor4ubv", Tom_glColor4ubv},
    {"::GL::glColor4uiv", Tom_glColor4uiv},
    {"::GL::glColor4usv", Tom_glColor4usv},
    {"::GL::glColorMask", Tom_glColorMask},
    {"::GL::glColorMaterial", Tom_glColorMaterial},
    {"::GL::glCopyPixels", Tom_glCopyPixels},
    {"::GL::glCullFace", Tom_glCullFace},
    {"::GL::glDeleteLists", Tom_glDeleteLists},
    {"::GL::glDepthFunc", Tom_glDepthFunc},
    {"::GL::glDepthMask", Tom_glDepthMask},
    {"::GL::glDepthRange", Tom_glDepthRange},
    {"::GL::glDisable", Tom_glDisable},
    {"::GL::glDrawBuffer", Tom_glDrawBuffer},
    {"::GL::glDrawPixels", Tom_glDrawPixels},
    {"::GL::glEdgeFlag", Tom_glEdgeFlag},
    {"::GL::glEnable", Tom_glEnable},
    {"::GL::glEnd", Tom_glEnd},
    {"::GL::glEndList", Tom_glEndList},
    {"::GL::glEvalCoord1d", Tom_glEvalCoord1d},
    {"::GL::glEvalCoord1f", Tom_glEvalCoord1f},
    {"::GL::glEvalCoord2d", Tom_glEvalCoord2d},
    {"::GL::glEvalCoord2f", Tom_glEvalCoord2f},
    {"::GL::glEvalMesh1", Tom_glEvalMesh1},
    {"::GL::glEvalMesh2", Tom_glEvalMesh2},
    {"::GL::glEvalPoint1", Tom_glEvalPoint1},
    {"::GL::glEvalPoint2", Tom_glEvalPoint2},
    {"::GL::glFinish", Tom_glFinish},
    {"::GL::glFlush", Tom_glFlush},
    {"::GL::glFogfv", Tom_glFogfv},
    {"::GL::glFogiv", Tom_glFogiv},
    {"::GL::glFrontFace", Tom_glFrontFace},
    {"::GL::glFrustum", Tom_glFrustum},
    {"::GL::glGenLists", Tom_glGenLists},
    {"::GL::glGetError", Tom_glGetError},
    {"::GL::glGetBooleanv", Tom_glGetBooleanv},
    {"::GL::glGetDoublev", Tom_glGetDoublev},
    {"::GL::glGetFloatv", Tom_glGetFloatv},
    {"::GL::glGetMaterialfv", Tom_glGetMaterialfv},
    {"::GL::glGetMaterialiv", Tom_glGetMaterialiv},
    {"::GL::glGetIntegerv", Tom_glGetIntegerv},
    {"::GL::glGetString", Tom_glGetString},
    {"::GL::glHint", Tom_glHint},
    {"::GL::glInitNames", Tom_glInitNames},
    {"::GL::glIsEnabled", Tom_glIsEnabled},
    {"::GL::glIsList", Tom_glIsList},
    {"::GL::glLightfv", Tom_glLightfv},
    {"::GL::glLightiv", Tom_glLightiv},
    {"::GL::glLightModelfv", Tom_glLightModelfv},
    {"::GL::glLightModeliv", Tom_glLightModeliv},
    {"::GL::glLineStipple", Tom_glLineStipple},
    {"::GL::glLineWidth", Tom_glLineWidth},
    {"::GL::glLoadIdentity", Tom_glLoadIdentity},
    {"::GL::glLoadMatrixd", Tom_glLoadMatrixd},
    {"::GL::glLoadMatrixf", Tom_glLoadMatrixf},
    {"::GL::glLoadName", Tom_glLoadName},
    {"::GL::glMap1d", Tom_glMap1d},
    {"::GL::glMap1f", Tom_glMap1f},
    {"::GL::glMap2d", Tom_glMap2d},
    {"::GL::glMap2f", Tom_glMap2f},
    {"::GL::glMapGrid1d", Tom_glMapGrid1d},
    {"::GL::glMapGrid1f", Tom_glMapGrid1f},
    {"::GL::glMapGrid2d", Tom_glMapGrid2d},
    {"::GL::glMapGrid2f", Tom_glMapGrid2f},
    {"::GL::glMaterialfv", Tom_glMaterialfv},
    {"::GL::glMaterialiv", Tom_glMaterialiv},
    {"::GL::glMatrixMode", Tom_glMatrixMode},
    {"::GL::glMultMatrixd", Tom_glMultMatrixd},
    {"::GL::glMultMatrixf", Tom_glMultMatrixf},
    {"::GL::glNewList", Tom_glNewList},
    {"::GL::glNormal3b", Tom_glNormal3b},
    {"::GL::glNormal3d", Tom_glNormal3d},
    {"::GL::glNormal3f", Tom_glNormal3f},
    {"::GL::glNormal3i", Tom_glNormal3i},
    {"::GL::glNormal3s", Tom_glNormal3s},
    {"::GL::glNormal3bv", Tom_glNormal3bv},
    {"::GL::glNormal3dv", Tom_glNormal3dv},
    {"::GL::glNormal3fv", Tom_glNormal3fv},
    {"::GL::glNormal3iv", Tom_glNormal3iv},
    {"::GL::glNormal3sv", Tom_glNormal3sv},
    {"::GL::glOrtho", Tom_glOrtho},
    {"::GL::glPixelStoref", Tom_glPixelStoref},
    {"::GL::glPixelStorei", Tom_glPixelStorei},
    {"::GL::glPixelTransferf", Tom_glPixelTransferf},
    {"::GL::glPixelTransferi", Tom_glPixelTransferi},
    {"::GL::glPixelZoom", Tom_glPixelZoom},
    {"::GL::glPointSize", Tom_glPointSize},
    {"::GL::glPolygonMode", Tom_glPolygonMode},
    {"::GL::glPopAttrib", Tom_glPopAttrib},
    {"::GL::glPushAttrib", Tom_glPushAttrib},
    {"::GL::glPopMatrix", Tom_glPopMatrix},
    {"::GL::glPushMatrix", Tom_glPushMatrix},
    {"::GL::glPopName", Tom_glPopName},
    {"::GL::glPushName", Tom_glPushName},
    {"::GL::glRasterPos2d", Tom_glRasterPos2d},
    {"::GL::glRasterPos2f", Tom_glRasterPos2f},
    {"::GL::glRasterPos2i", Tom_glRasterPos2i},
    {"::GL::glRasterPos2s", Tom_glRasterPos2s},
    {"::GL::glRasterPos3d", Tom_glRasterPos3d},
    {"::GL::glRasterPos3f", Tom_glRasterPos3f},
    {"::GL::glRasterPos3i", Tom_glRasterPos3i},
    {"::GL::glRasterPos3s", Tom_glRasterPos3s},
    {"::GL::glRasterPos4d", Tom_glRasterPos4d},
    {"::GL::glRasterPos4f", Tom_glRasterPos4f},
    {"::GL::glRasterPos4i", Tom_glRasterPos4i},
    {"::GL::glRasterPos4s", Tom_glRasterPos4s},
    {"::GL::glRasterPos2dv", Tom_glRasterPos2dv},
    {"::GL::glRasterPos2fv", Tom_glRasterPos2fv},
    {"::GL::glRasterPos2iv", Tom_glRasterPos2iv},
    {"::GL::glRasterPos2sv", Tom_glRasterPos2sv},
    {"::GL::glRasterPos3dv", Tom_glRasterPos3dv},
    {"::GL::glRasterPos3fv", Tom_glRasterPos3fv},
    {"::GL::glRasterPos3iv", Tom_glRasterPos3iv},
    {"::GL::glRasterPos3sv", Tom_glRasterPos3sv},
    {"::GL::glRasterPos4dv", Tom_glRasterPos4dv},
    {"::GL::glRasterPos4fv", Tom_glRasterPos4fv},
    {"::GL::glRasterPos4iv", Tom_glRasterPos4iv},
    {"::GL::glRasterPos4sv", Tom_glRasterPos4sv},
    {"::GL::glReadBuffer", Tom_glReadBuffer},
    {"::GL::glReadPixels", Tom_glReadPixels},
    {"::GL::glRectd", Tom_glRectd},
    {"::GL::glRectf", Tom_glRectf},
    {"::GL::glRecti", Tom_glRecti},
    {"::GL::glRects", Tom_glRects},
    {"::GL::glRectdv", Tom_glRectdv},
    {"::GL::glRectfv", Tom_glRectfv},
    {"::GL::glRectiv", Tom_glRectiv},
    {"::GL::glRectsv", Tom_glRectsv},
    {"::GL::glRenderMode", Tom_glRenderMode},
    {"::GL::glRotated", Tom_glRotated},
    {"::GL::glRotatef", Tom_glRotatef},
    {"::GL::glScaled", Tom_glScaled},
    {"::GL::glScalef", Tom_glScalef},
    {"::GL::glScissor", Tom_glScissor},
    {"::GL::glSelectBuffer", Tom_glSelectBuffer},
    {"::GL::glShadeModel", Tom_glShadeModel},
    {"::GL::glStencilMask", Tom_glStencilMask},
    {"::GL::glStencilFunc", Tom_glStencilFunc},
    {"::GL::glStencilOp", Tom_glStencilOp},
    {"::GL::glTexImage1D", Tom_glTexImage1D},
    {"::GL::glTexImage2D", Tom_glTexImage2D},
    {"::GL::glTexCoord2d", Tom_glTexCoord2d},
    {"::GL::glTexCoord2f", Tom_glTexCoord2f},
    {"::GL::glTexCoord2i", Tom_glTexCoord2i},
    {"::GL::glTexCoord3d", Tom_glTexCoord3d},
    {"::GL::glTexCoord3f", Tom_glTexCoord3f},
    {"::GL::glTexCoord3i", Tom_glTexCoord3i},
    {"::GL::glTexCoord4d", Tom_glTexCoord4d},
    {"::GL::glTexCoord4f", Tom_glTexCoord4f},
    {"::GL::glTexCoord4i", Tom_glTexCoord4i},
    {"::GL::glTexEnvf", Tom_glTexEnvf},
    {"::GL::glTexEnvi", Tom_glTexEnvi},
    {"::GL::glTexEnvfv", Tom_glTexEnvfv},
    {"::GL::glTexEnviv", Tom_glTexEnviv},
    {"::GL::glTexParameterf", Tom_glTexParameterf},
    {"::GL::glTexParameteri", Tom_glTexParameteri},
    {"::GL::glTranslated", Tom_glTranslated},
    {"::GL::glTranslatef", Tom_glTranslatef},
    {"::GL::glVertex2d", Tom_glVertex2d},
    {"::GL::glVertex2f", Tom_glVertex2f},
    {"::GL::glVertex2i", Tom_glVertex2i},
    {"::GL::glVertex3d", Tom_glVertex3d},
    {"::GL::glVertex3f", Tom_glVertex3f},
    {"::GL::glVertex3i", Tom_glVertex3i},
    {"::GL::glVertex2dv", Tom_glVertex2d},
    {"::GL::glVertex2fv", Tom_glVertex2f},
    {"::GL::glVertex2iv", Tom_glVertex2i},
    {"::GL::glVertex3dv", Tom_glVertex3d},
    {"::GL::glVertex3fv", Tom_glVertex3f},
    {"::GL::glVertex3iv", Tom_glVertex3i},
    {"::GL::glViewport", Tom_glViewport},

    {"::GL::gluBeginCurve", Tom_gluBeginCurve},
    {"::GL::gluBeginSurface", Tom_gluBeginSurface},
    {"::GL::gluBeginTrim", Tom_gluBeginTrim},
    {"::GL::gluDeleteNurbsRenderer", Tom_gluDeleteNurbsRenderer},
    {"::GL::gluDeleteQuadric", Tom_gluDeleteQuadric},
    {"::GL::gluEndCurve", Tom_gluEndCurve},
    {"::GL::gluEndSurface", Tom_gluEndSurface},
    {"::GL::gluEndTrim", Tom_gluEndTrim},
    {"::GL::gluLookAt", Tom_gluLookAt},
    {"::GL::gluNewNurbsRenderer", Tom_gluNewNurbsRenderer},
    {"::GL::gluNewQuadric", Tom_gluNewQuadric},
    {"::GL::gluNurbsProperty", Tom_gluNurbsProperty},
    {"::GL::gluNurbsSurface", Tom_gluNurbsSurface},
    {"::GL::gluOrtho2D", Tom_gluOrtho2D},
    {"::GL::gluPerspective", Tom_gluPerspective},
    {"::GL::gluSphere", Tom_gluSphere},
    {"::GL::gluQuadricTexture", Tom_gluQuadricTexture},
    {"::GL::gluPickMatrix", Tom_gluPickMatrix},

#ifdef GLUT
    {"::GL::glutSolidDodecahedron", Tom_glutSolidDodecahedron},
    {"::GL::glutWireDodecahedron", Tom_glutWireDodecahedron},
    {"::GL::glutSolidCube", Tom_glutSolidCube},
    {"::GL::glutWireCube", Tom_glutWireCube},
    {"::GL::glutSolidCone", Tom_glutSolidCone},
    {"::GL::glutWireCone", Tom_glutWireCone},
    {"::GL::glutSolidSphere", Tom_glutSolidSphere},
    {"::GL::glutWireSphere", Tom_glutWireSphere},
    {"::GL::glutSolidTeapot", Tom_glutSolidTeapot},
    {"::GL::glutWireTeapot", Tom_glutWireTeapot},
#endif /* GLUT */

    {NULL, NULL}
};

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BOOLEAN, "-doublebuffer", "doublebuffer", "Doublebuffer",
     "0", Tk_Offset(TOM, doublebuffer), 0, NULL},
    {TK_CONFIG_BOOLEAN, "-depth", "depth", "Depth",
     "0", Tk_Offset(TOM, depth), 0, NULL},
    {TK_CONFIG_BOOLEAN, "-alpha", "alpha", "Alpha",
     "0", Tk_Offset(TOM, alpha), 0, NULL},
    {TK_CONFIG_BOOLEAN, "-direct", "direct", "Direct",
     "1", Tk_Offset(TOM, direct), 0, NULL},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
     "0", Tk_Offset(TOM, width), 0, NULL},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
     "0", Tk_Offset(TOM, height), 0, NULL},
    {TK_CONFIG_BOOLEAN, "-current", "current", "Current",
     "1", Tk_Offset(TOM, current), 0, NULL},
    {TK_CONFIG_STRING, "-share", "share", "Share",
     NULL, Tk_Offset(TOM, share), 0, NULL},
    {TK_CONFIG_END, NULL, NULL, NULL, NULL, 0, 0, NULL}
};

int Tom_GetDoubleFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLdouble *d) {
    double tmp;
    int r;

    r = Tcl_GetDoubleFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *d = (GLfloat)tmp;

    return r;
}

int Tom_GetFloatFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLfloat *f) {
    double tmp;
    int r;

    r = Tcl_GetDoubleFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *f = (GLfloat)tmp;

    return r;
}

int Tom_GetIntFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLint *i) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *i = (GLint)tmp;

    return r;
}

int Tom_GetUIntFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLuint *i) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if ((r != TCL_OK) || (tmp < 0)) {
        Tcl_AppendResult(interp, "expected positive integer but got \"",
                         Tcl_GetStringFromObj(obj, NULL), "\"", NULL);
        return TCL_ERROR;
    }
    *i = (GLuint)tmp;

    return r;
}

int Tom_GetEnumFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLenum *e) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *e = (GLenum)tmp;

    return r;
}

int Tom_GetSizeFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLsizei *s) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *s = (GLsizei)tmp;

    return r;
}

int Tom_GetShortFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLshort *s) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *s = (GLshort)tmp;

    return r;
}

int Tom_GetUShortFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLushort *s) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if ((r != TCL_OK) || (tmp < 0)) {
        Tcl_AppendResult(interp, "expected positive short but got \"",
                         Tcl_GetStringFromObj(obj, NULL), "\"", NULL);
        return TCL_ERROR;
    }
    *s = (GLushort)tmp;

    return r;
}

int Tom_GetByteFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLbyte *b) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *b = (GLbyte)tmp;

    return r;
}

int Tom_GetUByteFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLbyte *b) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if ((r != TCL_OK) || (tmp < 0)) {
        Tcl_AppendResult(interp, "expected positive integer but got \"",
                         Tcl_GetStringFromObj(obj, NULL), "\"", NULL);
        return TCL_ERROR;
    }
    *b = (GLubyte)tmp;

    return r;
}

int Tom_GetBooleanFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLboolean *b) {
    int tmp;
    int r;

    r = Tcl_GetBooleanFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *b = (tmp != 0)? GL_TRUE : GL_FALSE;

    return r;
}

int Tom_GetBitfieldFromObj(Tcl_Interp *interp, Tcl_Obj *obj, GLbitfield *b) {
    long int tmp;
    int r;

    r = Tcl_GetLongFromObj(interp, obj, &tmp);
    if (r == TCL_OK) *b = (GLbitfield)tmp;

    return r;
}

/*
 * ! Converts an ASCII list to a binary buffer. 
 */
int Tom_FormatObj(Tcl_Interp *interp, Tcl_Obj *obj, void *buffer, int num, GLenum type, int swt) {
    Tcl_Obj *list, *v;
    char tmp[32];
    int i, len, rlen;

    if ((swt & TOM_VALUE) != 0) {
        list = obj;
    } else {
        list = Tcl_ObjGetVar2(interp, obj, NULL, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG);
        if (list == NULL) return TCL_ERROR;
    }
    if (Tcl_ListObjLength(interp, list, &len) != TCL_OK) return TCL_ERROR;
    switch (type) {
    case GL_2_BYTES:
        rlen = 2 * num;
        break;
    case GL_3_BYTES:
        rlen = 3 * num;
        break;
    case GL_4_BYTES:
        rlen = 4 * num;
        break;
    default:
        rlen = num;
    }
    if (len < rlen) {
        sprintf(tmp, "%d", rlen);
        Tcl_AppendResult(interp, "list should contain at least ", tmp, " elements", NULL);
        return TCL_ERROR;
    }

    switch (type) {
    case GL_INT:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetIntFromObj(interp, v, ((GLint *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_FLOAT:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetFloatFromObj(interp, v, ((GLfloat *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_DOUBLE:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetDoubleFromObj(interp, v, ((GLdouble *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_SHORT:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetShortFromObj(interp, v, ((GLshort *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_BYTE:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetByteFromObj(interp, v, ((GLbyte *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_UNSIGNED_INT:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetUIntFromObj(interp, v, ((GLuint *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_UNSIGNED_SHORT:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetUShortFromObj(interp, v, ((GLushort *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_UNSIGNED_BYTE:
    case GL_BITMAP:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetUByteFromObj(interp, v, ((GLubyte *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    case GL_2_BYTES:
    case GL_3_BYTES:
    case GL_4_BYTES:
        for (i = 0; i < rlen; i++) {
            if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
            if (Tom_GetUByteFromObj(interp, v, ((GLubyte *)buffer) + i) != TCL_OK) return TCL_ERROR;
        }
        break;
    default:
        sprintf(tmp, "%d", type);
        Tcl_AppendResult(interp, "unknown type ", tmp, NULL);
        return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 * ! 
 */
void *Tom_GetByteArrayFromObj(Tcl_Interp *interp, Tcl_Obj *obj, int size, int swt) {
    Tcl_Obj *list;
    char *raw;
    char tmp[32];
    int length;

    if ((swt & TOM_VALUE) != 0) {
        list = obj;
    } else {
        list = Tcl_ObjGetVar2(interp, obj, NULL, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG);
        if (list == NULL)
            return NULL;
    }

    raw = Tcl_GetByteArrayFromObj(list, &length);
    if (length < size) {
        sprintf(tmp, "%d", size);
        if ((swt & TOM_VALUE) == 0)
            Tcl_AppendResult(interp, "list \"",
                             Tcl_GetStringFromObj(obj, NULL),
                             "\" should contain at least ", tmp, " bytes", NULL);
        else Tcl_AppendResult(interp, "list should contain at least ", tmp, " bytes", NULL);
        return NULL;
    }

    return raw;
}

Tcl_Obj *Tom_NewPixels(Tcl_Interp *interp, void *pixels, int width,
                       int height, int num, GLenum type, int scan, int swt) {
    int x, y, w;
    Tcl_Obj *obj;
    char *p;

    w = width * num;
    if ((swt & TOM_ASCII) != 0) {
        obj = Tcl_NewListObj(0, NULL);
        p = pixels;
        switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_BITMAP:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLubyte *)p + x)));
                }
            }
            break;
        case GL_BYTE:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLbyte *)p + x)));
                }
            }
            break;
        case GL_FLOAT:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewDoubleObj(*((GLfloat *)p + x)));
                }
            }
            break;
        case GL_DOUBLE:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewDoubleObj(*((GLdouble *)p + x)));
                }
            }
            break;
        case GL_UNSIGNED_INT:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLuint *)p + x)));
                }
            }
            break;
        case GL_INT:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLint *)p + x)));
                }
            }
            break;
        case GL_UNSIGNED_SHORT:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLushort *)p + x)));
                }
            }
            break;
        case GL_SHORT:
            for (y = 0; y < height; y++, p += scan) {
                for (x = 0; x < w; x++) {
                    Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLshort *)p + x)));
                }
            }
            break;
#ifdef DEBUG
        default:
            Tcl_AppendResult(interp, "Tom_NewList: unhandled type ", NULL);
#endif
        }
    } else {
        obj = Tcl_NewByteArrayObj((unsigned char *)pixels, scan * height);
    }

    return obj;
}

/*
 * ! Converts an ASCII string to a binary buffer. 
 */
int
Tom_FormatPixels(Tcl_Interp *interp, Tcl_Obj *obj, void *pixels,
                 int width, int height, int num, GLenum type, int scan, int swt) {
    Tcl_Obj *list, *v;
    char tmp[32];
    int i, x, y, len, rlen, slen;
    char *p;

    if ((swt & TOM_VALUE) != 0) {
        list = obj;
    } else {
        list = Tcl_ObjGetVar2(interp, obj, NULL, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG);
        if (list == NULL)
            return TCL_ERROR;
    }
    if (Tcl_ListObjLength(interp, list, &len) != TCL_OK)
        return TCL_ERROR;
    slen = width * num;
    rlen = slen * height;
    if (len < rlen) {
        sprintf(tmp, "%d", rlen);
        Tcl_AppendResult(interp, "list should contain at least ", tmp, " elements", NULL);
        return TCL_ERROR;
    }

    p = pixels;
    i = 0;
    switch (type) {
    case GL_INT:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetIntFromObj(interp, v, ((GLint *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    case GL_FLOAT:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetFloatFromObj(interp, v, ((GLfloat *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    case GL_SHORT:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetShortFromObj(interp, v, ((GLshort *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    case GL_BYTE:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetByteFromObj(interp, v, ((GLbyte *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    case GL_UNSIGNED_INT:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetUIntFromObj(interp, v, ((GLuint *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    case GL_UNSIGNED_SHORT:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetUShortFromObj(interp, v, ((GLushort *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    case GL_UNSIGNED_BYTE:
    case GL_BITMAP:
        for (y = 0; y < height; y++, p+= scan) {
            for (x = 0; x < slen; x++, i++) {
                if (Tcl_ListObjIndex(interp, list, i, &v) != TCL_OK) return TCL_ERROR;
                if (Tom_GetUByteFromObj(interp, v, ((GLubyte *)p) + i) != TCL_OK) return TCL_ERROR;
            }
        }
        break;
    default:
        sprintf(tmp, "%d", type);
        Tcl_AppendResult(interp, "unknown type ", tmp, NULL);
        return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 * ! Creates a new list. 
 */
Tcl_Obj *Tom_NewList(Tcl_Interp *interp, void *b, int num, GLenum type, int swt) {
    int i;
    Tcl_Obj *obj;

    if ((swt & TOM_ASCII) != 0) {
        obj = Tcl_NewListObj(0, NULL);
        switch (type) {
        case GL_UNSIGNED_BYTE:
        case GL_BITMAP:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLubyte *)b + i)));
            }
            break;
        case GL_BYTE:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLbyte *)b + i)));
            }
            break;
        case GL_FLOAT:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewDoubleObj(*((GLfloat *)b + i)));
            }
            break;
        case GL_DOUBLE:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewDoubleObj(*((GLdouble *)b + i)));
            }
            break;
        case GL_INT:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLint *)b + i)));
            }
            break;
        case GL_UNSIGNED_INT:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLuint *)b + i)));
            }
            break;
        case GL_UNSIGNED_SHORT:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLushort *)b + i)));
            }
            break;
        case GL_SHORT:
            for (i = 0; i < num; i++) {
                Tcl_ListObjAppendElement(interp, obj, Tcl_NewIntObj(*((GLshort *)b + i)));
            }
            break;
#ifdef DEBUG
        default:
            Tcl_AppendResult(interp, "Tom_NewList: unhandled type ", NULL);
#endif
        }
    } else obj = Tcl_NewByteArrayObj((unsigned char *)b, num * Tom_GetTypeSize(type));

    return obj;
}

Tcl_HashEntry *Tom_CreateHashEntry(char *key, ClientData clientData) {
    Tcl_HashEntry *hentry;
    int new;

    hentry = Tcl_CreateHashEntry(&TOMHashTable, key, &new);
    Tcl_SetHashValue(hentry, clientData);
    return hentry;
}

Tcl_HashEntry *Tom_FindQEntry(Tcl_Interp *interp, int qobj) {
    Tcl_HashEntry *hentry;
    char buffer[32];

    sprintf(buffer, "q%d", qobj);
    hentry = Tcl_FindHashEntry(&TOMHashTable, buffer);
    if (hentry == NULL) {
        Tcl_AppendResult(interp, "quadric object \"", buffer, "\" doesn't exist", NULL);
    }
    return hentry;
}

Tcl_HashEntry *Tom_FindNEntry(Tcl_Interp *interp, int nobj) {
    Tcl_HashEntry *hentry;
    char buffer[32];

    sprintf(buffer, "n%d", nobj);
    hentry = Tcl_FindHashEntry(&TOMHashTable, buffer);
    if (hentry == NULL) {
        Tcl_AppendResult(interp, "nurbs object \"", buffer, "\" doesn't exist", NULL);
    }
    return hentry;
}

Tcl_HashEntry *Tom_FindWEntry(Tcl_Interp *interp, char *path) {
    Tcl_HashEntry *hentry;

    hentry = Tcl_FindHashEntry(&TOMHashTable, path);
    if (hentry == NULL) {
        Tcl_AppendResult(interp, "opengl widget \"", path, "\" doesn't exist", NULL);
    }
    return hentry;
}

int Tom_ParseSwitches(int objc, Tcl_Obj * CONST objv[], int *swt, int flags) {
    static char *SWITCHES[] = {
        "-value",               /* 0 */
        "-variable",            /* 1 */
        "-ascii",               /* 2 */
        "-binary",              /* 3 */
        "--",                   /* 4 */
        NULL
    };
    int i, ns, s;

    if (flags == 0)
        flags = TOM_VALUE | TOM_ASCII;
    for (ns = 1, s = 0; ns < objc; ns++) {
        if (Tcl_GetIndexFromObj(NULL, objv[ns], SWITCHES, NULL, 0, &i) == TCL_ERROR)
            break;
        switch (i) {
        case 0:
            if ((flags & TOM_VALUE) != 0) *swt |= TOM_VALUE;
            else s = 1;
            break;
        case 1:
            if ((flags & TOM_VALUE) != 0) *swt &= ~TOM_VALUE;
            else s = 1;
            break;
        case 2:
            if ((flags & TOM_ASCII) != 0) *swt |= TOM_ASCII;
            else s = 1;
            break;
        case 3:
            if ((flags & TOM_ASCII) != 0) *swt &= ~TOM_ASCII;
            else s = 1;
            break;
        case 4:
            ns++;
            s = 1;
            break;
        }
        if (s != 0) break;
    }

    return (ns - 1);
}

int Tom_TOMInit(Tcl_Interp *interp) {
    TomCmdDecl *cmd;
#ifdef GLUT
    char *argv[] = { "", /* "-gldebug", "-sync", "-display", "0:0", */ NULL };
    int argc = (sizeof (argv) / sizeof (char *)) - 1;
#endif

#ifdef GLUT
    glutInit(&argc, argv);
#endif


    /*
     * create ::GL namespace 
     */
    Tcl_Eval(interp, "namespace eval ::GL {}");

    Tom_GLDataInit(interp);

    Tcl_InitHashTable(&TOMHashTable, TCL_STRING_KEYS);

    /*
     * commands 
     */
    for (cmd = TOM_COMMANDS; cmd->cmdName != NULL; cmd++) {
        Tcl_CreateObjCommand(interp, cmd->cmdName, Tom_GLCmd, (ClientData)(cmd->proc), NULL);
    }

    /*
     * export all 
     */
    Tcl_Eval(interp, "namespace eval ::GL {namespace export *}");

    Tcl_CreateObjCommand(interp, "tom", Tom_TOMCmd, (ClientData)Tk_MainWindow(interp), NULL);

    return TCL_OK;
}

int Tom_GLCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (TOMCurrent == NULL) {
        Tcl_AppendResult(interp, "error: no current context", NULL);
        return TCL_ERROR;
    }

    /*
     * return ((Tcl_ObjCmdProc *)clientData)(TOMCurrent, interp, objc,
     * objv); 
     */
    return ((Tom_GLProc *)clientData)(TOMCurrent, interp, objc, objv);
}

int Tom_TOMCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tk_Window mwin, tkwin;
    TOM *tom;
    char *cmd;

    if (objc < 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "cmd ?options?");
        return TCL_ERROR;
    }
    cmd = Tcl_GetStringFromObj(objv[1], NULL);
    if (*cmd != '.') {
        if (strcmp(cmd, "current") == 0) {
            Tcl_AppendResult(interp, (TOMCurrent != NULL)? Tk_PathName(TOMCurrent->tkwin) : "", NULL);
            return TCL_OK;
        }
        Tcl_AppendResult(interp, "unknown command \"", cmd, "\"", NULL);
        return TCL_ERROR;
    }

    /*
     * new window 
     */
    mwin = (Tk_Window)clientData;
    tkwin = Tk_CreateWindowFromPath(interp, mwin, cmd, NULL);
    if (tkwin == NULL) return TCL_ERROR;
    Tk_SetClass(tkwin, "OpenGL");

    tom = (TOM *)(Tcl_Alloc(sizeof (TOM)));
    tom->tkwin = tkwin;
    tom->name = Tk_PathName(tkwin);
    tom->interp = interp;
    tom->x = 0;
    tom->y = 0;
    tom->width = 0;
    tom->height = 0;
    tom->swt = TOM_ASCII | TOM_VALUE;
    tom->display = Tk_Display(tkwin);
    tom->share = NULL;

#ifdef WIN32
    Tk_MakeWindowExist(tkwin);
    tom->hwnd = Tk_GetHWND(Tk_WindowId(tkwin));
    tom->hdc = GetDC(tom->hwnd);
    tom->hrc = NULL;
#else
    tom->gc = None;
    tom->context = NULL;
    tom->vi = NULL;
    tom->colormap = None;
#endif

    Tk_CreateEventHandler(tkwin, StructureNotifyMask, Tom_EventProc, (ClientData)tom);
    Tcl_CreateObjCommand(interp, Tk_PathName(tkwin), Tom_WidgetCmd, (ClientData)tom, Tom_DeleteCommand);

    if (Tom_Configure(interp, tom, objc - 2, objv + 2, 0) != TCL_OK) goto error;
    tom->mask = 0;
    if (tom->direct) tom->mask |= TOM_DIRECT_MASK;
    if (tom->doublebuffer) tom->mask |= TOM_DOUBLE_MASK;
    if (tom->alpha) tom->mask |= TOM_ALPHA_MASK;
    if (tom->depth) tom->mask |= TOM_DEPTH_MASK;

    if (Tom_InitContext(interp, tom) != TCL_OK) goto error;

    Tk_MapWindow(tkwin);

    Tcl_ResetResult(interp);
    Tcl_AppendElement(interp, Tk_PathName(tkwin));
    Tom_CreateHashEntry(tom->name, (ClientData)tom);

    if (tom->current) {
        if (Tom_MakeCurrent(interp, tom) != TCL_OK) goto error;
    }

    return TCL_OK;

  error:
    Tk_DestroyWindow(tkwin);

    return TCL_ERROR;
}

static int Tom_InitContext(Tcl_Interp *interp, TOM *tom) {
    TOM *share = NULL;
    Tcl_HashEntry *hentry;
#ifdef WIN32
    PIXELFORMATDESCRIPTOR pfd;
    int pixelFormat;

    if (tom->share != NULL) {
        hentry = Tom_FindWEntry(interp, tom->share);
        if (hentry == NULL) return TCL_ERROR;
        share = (TOM *)Tcl_GetHashValue(hentry);
    }

    pfd.nSize = sizeof (PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    if (TOM_DOUBLE(tom)) pfd.dwFlags |= PFD_DOUBLEBUFFER;
    pfd.cColorBits = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cAlphaBits = (TOM_ALPHA(tom))? 1 : 0;
    pfd.cAccumBits = (TOM_ACCUM(tom))? 1 : 0;
    pfd.cDepthBits = (TOM_DEPTH(tom))? 1 : 0;
    pfd.cStencilBits = (TOM_STENCIL(tom))? 1 : 0;
    pfd.cAuxBuffers = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;
    if ((pixelFormat = ChoosePixelFormat(GetDC(tom->hwnd), &pfd)) == 0) {
        Tcl_AppendResult(tom->interp, "error: couldn't choose pixel format", NULL);
        return TCL_ERROR;
    }
    if ((SetPixelFormat(GetDC(tom->hwnd), pixelFormat, &pfd) == FALSE) ||
        (DescribePixelFormat(GetDC(tom->hwnd), pixelFormat, sizeof (pfd), &pfd) == 0)) {
        Tcl_AppendResult(tom->interp, "error: couldn't choose pixel format", NULL);
        return TCL_ERROR;
    }
    tom->hrc = wglCreateContext(GetDC(tom->hwnd));
    if (tom->hrc == NULL) {
        Tcl_AppendResult(tom->interp, "error: could not create rendering context");
        return TCL_ERROR;
    }
    if (tom->share != NULL) {
        if (!wglShareLists(share->hrc, tom->hrc)) {
            Tcl_AppendResult(tom->interp, "error: could share context with \"", share->name, "\" widget");
            return TCL_ERROR;
        }
    }
#else
    int i, list[32];
    Colormap colormap = None;
    XVisualInfo *vi;
    GLXContext context = NULL;

    if (tom->share != NULL) {
        hentry = Tom_FindWEntry(interp, tom->share);
        if (hentry == NULL) return TCL_ERROR;
        share = (TOM *)Tcl_GetHashValue(hentry);
    }

    if (!glXQueryExtension(tom->display, NULL, NULL)) return TCL_ERROR;

    i = 0;
    list[i++] = GLX_LEVEL;
    list[i++] = 0;
    if (TOM_DOUBLE(tom)) list[i++] = GLX_DOUBLEBUFFER;
    list[i++] = GLX_RGBA;
    list[i++] = GLX_RED_SIZE;
    list[i++] = 1;
    list[i++] = GLX_GREEN_SIZE;
    list[i++] = 1;
    list[i++] = GLX_BLUE_SIZE;
    list[i++] = 1;
    if (TOM_ALPHA(tom)) {
        list[i++] = GLX_ALPHA_SIZE;
        list[i++] = 1;
    }
    if (TOM_ACCUM(tom)) {
        list[i++] = GLX_ACCUM_RED_SIZE;
        list[i++] = 1;
        list[i++] = GLX_ACCUM_GREEN_SIZE;
        list[i++] = 1;
        list[i++] = GLX_ACCUM_BLUE_SIZE;
        list[i++] = 1;
        if (TOM_ALPHA(tom)) {
            list[i++] = GLX_ACCUM_ALPHA_SIZE;
            list[i++] = 1;
        }
    }
    if (TOM_DEPTH(tom)) {
        list[i++] = GLX_DEPTH_SIZE;
        list[i++] = 1;
    }
    if (TOM_STENCIL(tom)) {
        list[i++] = GLX_STENCIL_SIZE;
        list[i++] = 1;
    }
    list[i] = (int)None;

    vi = glXChooseVisual(tom->display, Tk_ScreenNumber(tom->tkwin), list);
    if (vi == NULL) {
        Tcl_AppendResult(tom->interp, "error: couldn't find visual", NULL);
        return TCL_ERROR;
    }
    if (vi->class != TrueColor) {
        Tcl_AppendResult(tom->interp, "error: TrueColor visual required", NULL);
        return TCL_ERROR;
    }

    colormap = XCreateColormap(tom->display, RootWindow(tom->display, vi->screen), vi->visual, AllocNone);
    if (colormap == None) {
        Tcl_AppendResult(tom->interp, "error: couldn't create colormap", NULL);
        return TCL_ERROR;
    }

    if (Tk_SetWindowVisual(tom->tkwin, vi->visual, vi->depth, colormap) == 0) {
        Tcl_AppendResult(tom->interp, "error: couldn't set window visual", NULL);
        return TCL_ERROR;
    }

    context = glXCreateContext(tom->display, vi,
                               (share != NULL)? share->context : NULL,
			       TOM_DIRECT(tom)? GL_TRUE : GL_FALSE);
    if (context == NULL) {
        Tcl_AppendResult(tom->interp, "error: couldn't create context", NULL);
        return TCL_ERROR;
    }

    tom->context = context;
    tom->vi = vi;
    tom->colormap = colormap;
#endif

    return TCL_OK;
}

static int Tom_MakeCurrent(Tcl_Interp *interp, TOM *tom) {
    int result = TCL_OK;

    if (TOMCurrent == tom) return TCL_OK;
#ifdef WIN32
    if (tom == NULL) wglMakeCurrent(NULL, NULL);
    else if (!wglMakeCurrent(GetDC(tom->hwnd), tom->hrc)) result = TCL_ERROR;
#else
    if (tom == NULL) glXMakeCurrent(tom->display, None, NULL);
    else if (!glXMakeCurrent(tom->display, Tk_WindowId(tom->tkwin), tom->context))
        result = TCL_ERROR;
#endif
    if (result != TCL_OK) {
        Tcl_AppendResult(interp, "couldn't make ", Tk_PathName(tom->tkwin), " current Tom widget", NULL);
        return TCL_ERROR;
    }

    if (TOMCurrent != NULL) TOMCurrent->current = 0;
    if (tom != NULL) tom->current = 1;
    TOMCurrent = tom;

    return TCL_OK;
}

static int Tom_Configure(Tcl_Interp *interp, TOM *tom, int objc, Tcl_Obj * CONST objv[], int flags) {
    char **argv;
    int i;
#ifdef WIN32
#else
    XGCValues gcValues;
#endif
    argv = (char **)(Tcl_Alloc(objc * sizeof (char *)));
    for (i = 0; i < objc; i++) *(argv + i) = Tcl_GetStringFromObj(objv[i], NULL);
    i = Tk_ConfigureWidget(interp, tom->tkwin, configSpecs, objc, argv, (char *)tom, flags);
    Tcl_Free((char *)argv);
    if (i != TCL_OK) return TCL_ERROR;

#ifdef WIN32
#else
    if (tom->gc == None) {
        gcValues.function = GXcopy;
        gcValues.graphics_exposures = False;
        tom->gc = Tk_GetGC(tom->tkwin, GCFunction | GCGraphicsExposures, &gcValues);
    }
#endif

    Tk_GeometryRequest(tom->tkwin, tom->width, tom->height);

    return TCL_OK;
}

static void Tom_DeleteCommand(ClientData clientData) {
    TOM *tom;
    Tk_Window tkwin;

    tom = (TOM *)clientData;
    if (tom->tkwin != NULL) {
        tkwin = tom->tkwin;
        tom->tkwin = NULL;
        Tk_DestroyWindow(tkwin);
    }
}

static void Tom_EventProc(ClientData clientData, XEvent *event) {
    TOM *tom;
    char *cmd;

    tom = (TOM *)clientData;
    if (event->type == DestroyNotify) {
        if (tom->tkwin != NULL) {
            cmd = Tk_PathName(tom->tkwin);
            tom->tkwin = NULL;
            Tcl_DeleteCommand(tom->interp, cmd);
        }
        Tcl_EventuallyFree((ClientData) tom, (Tcl_FreeProc *)Tom_Destroy);
    }
}

static void Tom_Destroy(ClientData clientData) {
    TOM *tom;
    Tcl_HashEntry *hentry;

    tom = (TOM *)clientData;
    hentry = Tcl_FindHashEntry(&TOMHashTable, tom->name);
    if (hentry != NULL) Tcl_DeleteHashEntry(hentry);
    if (TOMCurrent == tom) {
#ifdef WIN32
        wglMakeCurrent(NULL, NULL);
#else
        glXMakeCurrent(tom->display, None, NULL);
#endif
        TOMCurrent = NULL;
    }
    Tk_FreeOptions(configSpecs, (char *)tom, tom->display, 0);
#ifdef WIN32
    if (tom->hrc != NULL) wglDeleteContext(tom->hrc);
    ReleaseDC(tom->hwnd, GetDC(tom->hwnd));
#else
    if (tom->gc != None) Tk_FreeGC(tom->display, tom->gc);
    if (tom->context != NULL) glXDestroyContext(tom->display, tom->context);
    if (tom->colormap != None) XFreeColormap(tom->display, tom->colormap);
#endif
    Tcl_Free((char *)tom);
}

static int Tom_WidgetCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    int result = TCL_OK;
    TOM *old, *tom;
#ifdef WIN32
    RECT rect;
    /*
     * PAINTSTRUCT ps; HDC hdc; 
     */
#else
    XEvent evt;
#endif
    static char *OPTIONS[] = {
        "makecurrent",
        "swapbuffers",
        "expose",
        "params",
#ifdef IMLIB2
        "load",
        "save",
#endif
#ifdef DEBUG
        "test",
#endif
        NULL
    };
    enum {
        ID_MAKECURRENT,
        ID_SWAPBUFFERS,
        ID_EXPOSE,
        ID_PARAMS,
#ifdef IMLIB2
        ID_LOAD,
        ID_SAVE,
#endif
#ifdef DEBUG
        ID_TEST,
#endif
        NUM_IDS
    };
    int i, opt, n;
    int swt, ns;
    int x, y, width, height;

    if (objc < 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "option ?arg arg ...?");
        return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(NULL, objv[1], OPTIONS, NULL, 0, &opt) == TCL_ERROR) {
        Tcl_AppendResult(interp, "bad option \"",
                         Tcl_GetStringFromObj(objv[1], NULL), "\": must be ", OPTIONS[0], NULL);
        n = (sizeof (OPTIONS) / sizeof (char *)) - 1;
        for (i = 1; i < (n - 1); i++) {
            Tcl_AppendResult(interp, ", ", OPTIONS[i], NULL);
        }
        Tcl_AppendResult(interp, " or ", OPTIONS[i], NULL);

        return TCL_ERROR;
    }

    tom = (TOM *)clientData;
    Tcl_Preserve((ClientData)tom);

    switch (opt) {
    case ID_MAKECURRENT:
        if (objc != 2) {
            Tcl_WrongNumArgs(interp, 1, objv, "makecurrent");
            result = TCL_ERROR;
            goto done;
        }
        if (Tom_MakeCurrent(interp, tom) != TCL_OK) {
            result = TCL_ERROR;
            goto done;
        }
        break;
    case ID_SWAPBUFFERS:
        if (objc != 2) {
            Tcl_WrongNumArgs(interp, 1, objv, "swapbuffers");
            result = TCL_ERROR;
            goto done;
        }
        old = TOMCurrent;
        if (Tom_MakeCurrent(interp, tom) != TCL_OK) {
            result = TCL_ERROR;
            goto done;
        }
#ifdef WIN32
        /*
         * force screen update 
         */
        /*
         * BeginPaint(tom->hwnd, &ps); 
         */
        SwapBuffers(GetDC(tom->hwnd));
        /*
         * EndPaint(tom->hwnd, &ps); 
         */
#else
        glXSwapBuffers(tom->display, Tk_WindowId(tom->tkwin));
#endif
        if (Tom_MakeCurrent(interp, old) != TCL_OK) {
            result = TCL_ERROR;
            goto done;
        }
        break;
    case ID_EXPOSE:
        if ((objc != 2) && (objc != 6)) {
            Tcl_WrongNumArgs(interp, 1, objv, "expose ?x y width height?");
            result = TCL_ERROR;
            goto done;
        }
        if (objc == 6) {
            if ((Tcl_GetIntFromObj(interp, objv[2], &x) != TCL_OK) ||
                (Tcl_GetIntFromObj(interp, objv[3], &y) != TCL_OK) ||
                (Tcl_GetIntFromObj(interp, objv[4], &width) != TCL_OK)
                || (Tcl_GetIntFromObj(interp, objv[5], &height) != TCL_OK)) {
                result = TCL_ERROR;
                goto done;
            }
        } else {
            x = y = 0;
            width = Tk_Width(tom->tkwin);
            height = Tk_Height(tom->tkwin);
        }
#ifdef WIN32
        rect.left = 0;
        rect.right = width;
        rect.top = 0;
        rect.bottom = height;
        RedrawWindow(tom->hwnd, &rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
#else
        memset(&evt, 0, sizeof (evt));
        evt.xany.type = Expose;
        evt.xany.serial = NextRequest(Tk_Display(tom->tkwin));
        evt.xany.send_event = False;
        evt.xany.display = tom->display;
        evt.xexpose.window = Tk_WindowId(tom->tkwin);
        evt.xexpose.x = x;
        evt.xexpose.y = y;
        evt.xexpose.width = width;
        evt.xexpose.height = height;
        Tk_HandleEvent(&evt);
#endif
        break;
    case ID_PARAMS:
        if (objc == 2) {
            Tcl_AppendResult(interp, ((tom->swt & TOM_ASCII) != 0)? "-ascii " : "-binary ", NULL);
            Tcl_AppendResult(interp, ((tom->swt & TOM_VALUE) != 0)? "-value" : "-variable", NULL);
            goto done;
        }
        swt = tom->swt;
        ns = Tom_ParseSwitches(objc - 1, objv + 1, &swt, 0);
        if ((objc - ns) != 2) {
            Tcl_AppendResult(interp, "unknown switch \"",
                             Tcl_GetStringFromObj(objv[ns + 2], NULL), "\"", NULL);
            result = TCL_ERROR;
            goto done;
        }
        tom->swt = swt;
        break;
#ifdef IMLIB2
    case ID_LOAD:
        result = Tom_WidgetLoadCmd(tom, interp, objc - 1, objv + 1);
        break;
    case ID_SAVE:
        result = Tom_WidgetSaveCmd(tom, interp, objc - 1, objv + 1);
        break;
#endif   /*IMLIB2*/
#ifdef DEBUG
    case ID_TEST:
        if (objc != 2) {
            Tcl_WrongNumArgs(interp, 1, objv, NULL);
            result = TCL_ERROR;
            goto done;
        }
        result = Tom_Test(interp);
        break;
#endif   /*DEBUG*/
    }

  done:
    Tcl_Release((ClientData) tom);

    return result;
}

#ifdef DEBUG
static int Tom_Test(Tcl_Interp *interp) {
    return TCL_OK;
}
#endif /*DEBUG*/

int Tom_Init(Tcl_Interp *interp) {
    return Tom_TOMInit(interp);
}
