#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glRectd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x1, y1, x2, y2;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y1) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &x2) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &y2) != TCL_OK))
        return TCL_ERROR;

    glRectd(x1, y1, x2, y2);

    return TCL_OK;
}

int Tom_glRectf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x1, y1, x2, y2;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &x1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y1) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &x2) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[4], &y2) != TCL_OK))
        return TCL_ERROR;

    glRectf(x1, y1, x2, y2);

    return TCL_OK;
}

int Tom_glRecti(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x1, y1, x2, y2;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x1) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y1) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &x2) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &y2) != TCL_OK))
        return TCL_ERROR;

    glRecti(x1, y1, x2, y2);

    return TCL_OK;
}

int Tom_glRects(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort x1, y1, x2, y2;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x1 y1 x2 y2");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &x1) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &y1) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[3], &x2) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[4], &y2) != TCL_OK))
        return TCL_ERROR;

    glRects(x1, y1, x2, y2);

    return TCL_OK;
}

int Tom_glRectdv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble bv1[2], bv2[2];
    void *v1, *v2;
    int ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v1 v2");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], bv1, 2, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        if (Tom_FormatObj(interp, objv[ns + 2], bv2, 2, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v1 = bv1;
        v2 = bv2;
    } else {
        if ((v1 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
        if ((v2 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glRectdv(v1, v2);

    return TCL_OK;
}

int Tom_glRectfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat bv1[2], bv2[2];
    void *v1, *v2;
    int ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v1 v2");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], bv1, 2, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        if (Tom_FormatObj(interp, objv[ns + 2], bv2, 2, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v1 = bv1;
        v2 = bv2;
    } else {
        if ((v1 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
        if ((v2 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glRectiv(v1, v2);

    return TCL_OK;
}

int Tom_glRectiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint bv1[2], bv2[2];
    void *v1, *v2;
    int ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v1 v2");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], bv1, 2, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        if (Tom_FormatObj(interp, objv[ns + 2], bv2, 2, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v1 = bv1;
        v2 = bv2;
    } else {
        if ((v1 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
        if ((v2 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glRectiv(v1, v2);

    return TCL_OK;
}

int Tom_glRectsv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort bv1[2], bv2[2];
    void *v1, *v2;
    int ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v1 v2");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], bv1, 2, GL_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        if (Tom_FormatObj(interp, objv[ns + 2], bv2, 2, GL_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v1 = bv1;
        v2 = bv2;
    } else {
        if ((v1 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLshort), swt)) == 0)
            return TCL_ERROR;
        if ((v2 = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 2 * sizeof (GLshort), swt)) == 0)
            return TCL_ERROR;
    }

    glRectiv(v1, v2);

    return TCL_OK;
}
