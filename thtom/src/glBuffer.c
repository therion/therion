#include <stdlib.h>
#include <string.h>
#include "tom.h"

static Tcl_Obj *TOMSelectBuffer = NULL;
static GLuint *GLSelectBuffer = NULL;
static GLsizei GLSelectSize = 0;

int Tom_glDepthFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum func;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "func");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &func) != TCL_OK) return TCL_ERROR;

    glDepthFunc(func);

    return TCL_OK;
}

int Tom_glBlendFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum sfactor, dfactor;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "sfactor dfactor");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &sfactor) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &dfactor) != TCL_OK))
        return TCL_ERROR;

    glBlendFunc(sfactor, dfactor);

    return TCL_OK;
}

int Tom_glClear(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    int mask;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mask");
        return TCL_ERROR;
    }
    if (Tcl_GetIntFromObj(interp, objv[1], &mask) != TCL_OK) return TCL_ERROR;

    glClear((GLbitfield) mask);

    return TCL_OK;
}

int Tom_glClearColor(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    double red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tcl_GetDoubleFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tcl_GetDoubleFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tcl_GetDoubleFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tcl_GetDoubleFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glClearColor((GLclampf)red, (GLclampf)green, (GLclampf)blue, (GLclampf)alpha);

    return TCL_OK;
}

int Tom_glClearAccum(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glClearAccum(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glClearDepth(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble depth;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "depth");
        return TCL_ERROR;
    }
    if (Tom_GetDoubleFromObj(interp, objv[1], &depth) != TCL_OK) return TCL_ERROR;

    glClearDepth((GLclampf)depth);

    return TCL_OK;
}

int Tom_glClearIndex(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat c;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "c");
        return TCL_ERROR;
    }
    if (Tom_GetFloatFromObj(interp, objv[1], &c) != TCL_OK) return TCL_ERROR;

    glClearIndex(c);

    return TCL_OK;
}

int Tom_glClearStencil(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint s;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "s");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &s) != TCL_OK) return TCL_ERROR;

    glClearStencil(s);

    return TCL_OK;
}

int Tom_glSelectBuffer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "size buffer");
        return TCL_ERROR;
    }
    if (Tom_GetSizeFromObj(interp, objv[1], &GLSelectSize) != TCL_OK) return TCL_ERROR;
    if (TOMSelectBuffer != NULL) Tcl_DecrRefCount(TOMSelectBuffer);
    TOMSelectBuffer = Tcl_DuplicateObj(objv[2]);
    if (GLSelectBuffer != NULL) Tcl_Free((char *)GLSelectBuffer);
    GLSelectBuffer = (GLuint *)Tcl_Alloc(GLSelectSize * sizeof (GLuint));
    if (GLSelectBuffer == NULL) return TCL_ERROR;

    glSelectBuffer(GLSelectSize, GLSelectBuffer);

    return TCL_OK;
}

int Tom_glRenderMode(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint r;
    char buffer[32];
    GLenum mode;
    Tcl_Obj *str;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    r = glRenderMode(mode);

    if (GLSelectBuffer != NULL) {
        str = Tcl_NewStringObj((char *)GLSelectBuffer, GLSelectSize * sizeof (GLint));
        if (Tcl_ObjSetVar2(interp, TOMSelectBuffer, NULL, str,
			   TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL)
            return TCL_ERROR;
    }
    sprintf(buffer, "%d", r);
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, buffer, NULL);

    return TCL_OK;
}

int Tom_glReadBuffer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glReadBuffer(mode);

    return TCL_OK;
}

int Tom_glDrawBuffer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glDrawBuffer(mode);

    return TCL_OK;
}

int Tom_glColorMask(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLboolean red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetBooleanFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetBooleanFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetBooleanFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetBooleanFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColorMask(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glDepthMask(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLboolean flag;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "flag");
        return TCL_ERROR;
    }
    if (Tom_GetBooleanFromObj(interp, objv[1], &flag) != TCL_OK) return TCL_ERROR;

    glDepthMask(flag);

    return TCL_OK;
}

int Tom_glStencilMask(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint mask;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mask");
        return TCL_ERROR;
    }
    if (Tom_GetUIntFromObj(interp, objv[1], &mask) != TCL_OK) return TCL_ERROR;

    glStencilMask(mask);

    return TCL_OK;
}

int Tom_glStencilFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum func;
    GLint ref;
    GLuint mask;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "func ref mask");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &func) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &ref) != TCL_OK) ||
        (Tom_GetUIntFromObj(interp, objv[3], &mask) != TCL_OK))
        return TCL_ERROR;

    glStencilFunc(func, ref, mask);

    return TCL_OK;
}

int Tom_glAlphaFunc(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum func;
    GLfloat ref;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "func ref");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &func) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &ref) != TCL_OK))
        return TCL_ERROR;

    glAlphaFunc(func, ref);

    return TCL_OK;
}

int Tom_glAccum(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum op;
    GLfloat value;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "op value");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &op) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &value) != TCL_OK))
        return TCL_ERROR;

    glAccum(op, value);

    return TCL_OK;
}

int Tom_glDepthRange(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    double znear, zfar;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "near far");
        return TCL_ERROR;
    }
    if ((Tcl_GetDoubleFromObj(interp, objv[1], &znear) != TCL_OK) ||
        (Tcl_GetDoubleFromObj(interp, objv[2], &zfar) != TCL_OK))
        return TCL_ERROR;

    glDepthRange((GLclampf)znear, (GLclampf)zfar);

    return TCL_OK;
}

int Tom_glScissor(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y;
    GLsizei width, height;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y width height");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &y) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &x) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[3], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[4], &height) != TCL_OK))
        return TCL_ERROR;

    glScissor(x, y, width, height);

    return TCL_OK;
}

int Tom_glStencilOp(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum fail, zfail, zpass;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "fail zfail zpass");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &fail) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &zfail) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[3], &zpass) != TCL_OK))
        return TCL_ERROR;

    glStencilOp(fail, zfail, zpass);

    return TCL_OK;
}
