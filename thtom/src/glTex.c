#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glTexCoord2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble s, t;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &t) != TCL_OK))
        return TCL_ERROR;

    glTexCoord2d(s, t);

    return TCL_OK;
}

int Tom_glTexCoord2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat s, t;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &t) != TCL_OK))
        return TCL_ERROR;

    glTexCoord2f(s, t);

    return TCL_OK;
}

int Tom_glTexCoord2i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint s, t;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &t) != TCL_OK))
        return TCL_ERROR;

    glTexCoord2i(s, t);

    return TCL_OK;
}

int Tom_glTexCoord3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble s, t, r;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t r");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &t) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &r) != TCL_OK))
        return TCL_ERROR;

    glTexCoord3d(s, t, r);

    return TCL_OK;
}

int Tom_glTexCoord3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat s, t, r;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t r");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &t) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &r) != TCL_OK))
        return TCL_ERROR;

    glTexCoord3f(s, t, r);

    return TCL_OK;
}

int Tom_glTexCoord3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint s, t, r;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t r");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &t) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &r) != TCL_OK))
        return TCL_ERROR;

    glTexCoord3i(s, t, r);

    return TCL_OK;
}

int Tom_glTexCoord4d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble s, t, r, q;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t r q");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &t) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &r) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &q) != TCL_OK))
        return TCL_ERROR;

    glTexCoord4d(s, t, r, q);

    return TCL_OK;
}

int Tom_glTexCoord4f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat s, t, r, q;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t r q");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &t) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &r) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[4], &q) != TCL_OK))
        return TCL_ERROR;

    glTexCoord4f(s, t, r, q);

    return TCL_OK;
}

int Tom_glTexCoord4i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint s, t, r, q;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "s t r q");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &s) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &t) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &r) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &q) != TCL_OK))
        return TCL_ERROR;

    glTexCoord4i(s, t, r, q);

    return TCL_OK;
}
