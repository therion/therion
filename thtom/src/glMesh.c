#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glMap1d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target;
    GLdouble u1, u2, *points;
    GLint stride, order;
    int size, ns, swt = tom->swt;

    ns = (objc > 7)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 7) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? target u1 u2 stride order points");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[ns + 2], &u1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[ns + 3], &u2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 4], &stride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 5], &order) != TCL_OK))
        return TCL_ERROR;
    size = order * stride;
    if ((swt & TOM_ASCII) != 0) {
        if ((points = (GLdouble *)Tcl_Alloc(size * sizeof (GLdouble))) == NULL) {
            Tcl_AppendResult(interp, "memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 6], points, size, GL_DOUBLE, swt)
            == TCL_ERROR) {
            Tcl_Free((char *)points);
            return TCL_ERROR;
        }
    } else {
        if ((points = Tom_GetByteArrayFromObj(interp, objv[ns + 6], size, swt)) == 0)
            return TCL_ERROR;
    }

    glMap1d(target, u1, u2, stride, order, points);

    if ((swt & TOM_ASCII) != 0)
        Tcl_Free((char *)points);

    return TCL_OK;
}

int Tom_glMap1f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target;
    GLfloat u1, u2, *points;
    GLint stride, order;
    int size, ns, swt = tom->swt;

    ns = (objc > 7)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 7) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? target u1 u2 stride order points");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 2], &u1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 3], &u2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 4], &stride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 5], &order) != TCL_OK))
        return TCL_ERROR;
    size = order * stride;
    if ((swt & TOM_ASCII) != 0) {
        if ((points = (GLfloat *)Tcl_Alloc(size * sizeof (GLfloat))) == NULL) {
            Tcl_AppendResult(interp, "memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 6], points, size, GL_FLOAT, swt)
            == TCL_ERROR) {
            Tcl_Free((char *)points);
            return TCL_ERROR;
        }
    } else {
        if ((points = Tom_GetByteArrayFromObj(interp, objv[ns + 6], size, swt)) == 0)
            return TCL_ERROR;
    }

    glMap1f(target, u1, u2, stride, order, points);

    if ((swt & TOM_ASCII) != 0) Tcl_Free((char *)points);

    return TCL_OK;
}

int Tom_glMap2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target;
    GLdouble u1, u2, v1, v2, *points;
    GLint ustride, uorder, vstride, vorder;
    int size, ns, swt = tom->swt;

    ns = (objc > 11)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 11) {
        Tcl_WrongNumArgs(interp, 1, objv,
                         "?switches? target u1 u2 ustride uorder v1 v2 vstride vorder points");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[ns + 2], &u1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[ns + 3], &u2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 4], &ustride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 5], &uorder) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[ns + 6], &v1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[ns + 7], &v2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 8], &vstride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 9], &vorder) != TCL_OK))
        return TCL_ERROR;
    size = vorder * vstride;
    if ((swt & TOM_ASCII) != 0) {
        if ((points = (GLdouble *)Tcl_Alloc(size * sizeof (GLdouble))) == NULL) {
            Tcl_AppendResult(interp, "memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 10], points, size, GL_DOUBLE, swt)
            == TCL_ERROR) {
            Tcl_Free((char *)points);
            return TCL_ERROR;
        }
    } else {
        if ((points = Tom_GetByteArrayFromObj(interp, objv[ns + 10], size, swt)) == 0)
            return TCL_ERROR;
    }

    glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);

    if ((swt & TOM_ASCII) != 0) Tcl_Free((char *)points);

    return TCL_OK;
}

int Tom_glMap2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target;
    GLfloat u1, u2, v1, v2, *points;
    GLint ustride, uorder, vstride, vorder;
    int size, ns, swt = tom->swt;

    ns = (objc > 11)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 11) {
        Tcl_WrongNumArgs(interp, 1, objv,
                         "?switches? target u1 u2 ustride uorder v1 v2 vstride vorder points");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 2], &u1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 3], &u2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 4], &ustride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 5], &uorder) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 6], &v1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 7], &v2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 8], &vstride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 9], &vorder) != TCL_OK))
        return TCL_ERROR;
    size = vorder * vstride;
    if ((swt & TOM_ASCII) != 0) {
        if ((points = (GLfloat *)Tcl_Alloc(size * sizeof (GLfloat))) == NULL) {
            Tcl_AppendResult(interp, "memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 10], points, size, GL_FLOAT, swt)
            == TCL_ERROR) {
            Tcl_Free((char *)points);
            return TCL_ERROR;
        }
    } else {
        if ((points = Tom_GetByteArrayFromObj(interp, objv[ns + 10], size, swt)) == 0)
            return TCL_ERROR;
    }

    glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);

    if ((swt & TOM_ASCII) != 0) Tcl_Free((char *)points);

    return TCL_OK;
}

int Tom_glMapGrid1d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint un;
    GLdouble u1, u2;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "un u1 u2");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &un) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &u1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &u2) != TCL_OK))
        return TCL_ERROR;

    glMapGrid1d(un, u1, u2);

    return TCL_OK;
}

int Tom_glMapGrid1f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint un;
    GLfloat u1, u2;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "un u1 u2");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &un) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &u1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &u2) != TCL_OK))
        return TCL_ERROR;

    glMapGrid1f(un, u1, u2);

    return TCL_OK;
}

int Tom_glMapGrid2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint un, vn;
    GLdouble u1, u2, v1, v2;

    if (objc != 7) {
        Tcl_WrongNumArgs(interp, 1, objv, "un u1 u2 vn v1 v2");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &un) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &u1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &u2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &vn) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[5], &v1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[6], &v2) != TCL_OK))
        return TCL_ERROR;

    glMapGrid2d(un, u1, u2, vn, v1, v2);

    return TCL_OK;
}

int Tom_glMapGrid2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint un, vn;
    GLfloat u1, u2, v1, v2;

    if (objc != 7) {
        Tcl_WrongNumArgs(interp, 1, objv, "un u1 u2 vn v1 v2");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &un) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &u1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &u2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &vn) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[5], &v1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[6], &v2) != TCL_OK))
        return TCL_ERROR;

    glMapGrid2f(un, u1, u2, vn, v1, v2);

    return TCL_OK;
}

int Tom_glEvalCoord1d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble u;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "u");
        return TCL_ERROR;
    }
    if (Tom_GetDoubleFromObj(interp, objv[1], &u) != TCL_OK) return TCL_ERROR;

    glEvalCoord1d(u);

    return TCL_OK;
}

int Tom_glEvalCoord1f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat u;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "u");
        return TCL_ERROR;
    }
    if (Tom_GetFloatFromObj(interp, objv[1], &u) != TCL_OK) return TCL_ERROR;

    glEvalCoord1f(u);

    return TCL_OK;
}

int Tom_glEvalCoord2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble u, v;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "u v");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &u) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &v) != TCL_OK))
        return TCL_ERROR;

    glEvalCoord2d(u, v);

    return TCL_OK;
}

int Tom_glEvalCoord2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat u, v;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "u v");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &u) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &v) != TCL_OK))
        return TCL_ERROR;

    glEvalCoord2f(u, v);

    return TCL_OK;
}

int Tom_glEvalMesh1(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;
    GLint i1, i2;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode i1 i2");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &i1) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &i2) != TCL_OK))
        return TCL_ERROR;

    glEvalMesh1(mode, i1, i2);

    return TCL_OK;
}

int Tom_glEvalMesh2(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;
    GLint i1, i2, j1, j2;

    if (objc != 6) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode i1 i2 j1 j2");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &i1) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &i2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &j1) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[5], &j2) != TCL_OK))
        return TCL_ERROR;

    glEvalMesh2(mode, i1, i2, j1, j2);

    return TCL_OK;
}

int Tom_glEvalPoint1(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint i;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "i");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &i) != TCL_OK) return TCL_ERROR;

    glEvalPoint1(i);

    return TCL_OK;
}

int Tom_glEvalPoint2(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint i1, i2;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "i1 i2");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &i1) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &i2) != TCL_OK))
        return TCL_ERROR;

    glEvalPoint2(i1, i2);

    return TCL_OK;
}
