#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glMatrixMode(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glMatrixMode(mode);

    return TCL_OK;
}

int Tom_glLoadIdentity(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glLoadIdentity();

    return TCL_OK;
}

int Tom_glLoadMatrixd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble buffer[16], *matrix;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? matrix");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 16, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        matrix = buffer;
    } else {
        if ((matrix = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 16 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glLoadMatrixd(matrix);

    return TCL_OK;
}

int Tom_glLoadMatrixf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat buffer[16], *matrix;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? matrix");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 16, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        matrix = buffer;
    } else {
        if ((matrix = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 16 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glLoadMatrixf(matrix);

    return TCL_OK;
}

int Tom_glMultMatrixd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble buffer[16], *matrix;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? matrix");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 16, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        matrix = buffer;
    } else {
        if ((matrix = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 16 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glMultMatrixd(matrix);

    return TCL_OK;
}

int Tom_glMultMatrixf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat buffer[16], *matrix;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? matrix");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 16, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        matrix = buffer;
    } else {
        if ((matrix = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 16 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glMultMatrixf(matrix);

    return TCL_OK;
}

int Tom_glPushMatrix(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glPushMatrix();

    return TCL_OK;
}

int Tom_glPopMatrix(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glPopMatrix();

    return TCL_OK;
}

int Tom_glRotated(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble angle, x, y, z;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "angle x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &angle) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &z) != TCL_OK))
        return TCL_ERROR;

    glRotated(angle, x, y, z);

    return TCL_OK;
}

int Tom_glRotatef(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat angle, x, y, z;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "angle x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &angle) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &y) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[4], &z) != TCL_OK))
        return TCL_ERROR;

    glRotatef(angle, x, y, z);

    return TCL_OK;
}

int Tom_glTranslated(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }

    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glTranslated(x, y, z);

    return TCL_OK;
}

int Tom_glTranslatef(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }

    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glTranslatef(x, y, z);

    return TCL_OK;
}

int Tom_glScaled(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }

    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glScaled(x, y, z);

    return TCL_OK;
}

int Tom_glScalef(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }

    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glScalef(x, y, z);

    return TCL_OK;
}

int Tom_glFrustum(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble l, r, b, t, n, f;

    if (objc != 7) {
        Tcl_WrongNumArgs(interp, 1, objv, "left right bottom top near far");
        return TCL_ERROR;
    }

    if ((Tom_GetDoubleFromObj(interp, objv[1], &l) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &r) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &t) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[5], &n) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[6], &f) != TCL_OK))
        return TCL_ERROR;

    glFrustum(l, r, b, t, n, f);

    return TCL_OK;
}

int Tom_glOrtho(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble l, r, b, t, n, f;

    if (objc != 7) {
        Tcl_WrongNumArgs(interp, 1, objv, "left right bottom top near far");
        return TCL_ERROR;
    }

    if ((Tom_GetDoubleFromObj(interp, objv[1], &l) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &r) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &t) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[5], &n) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[6], &f) != TCL_OK))
        return TCL_ERROR;

    glOrtho(l, r, b, t, n, f);

    return TCL_OK;
}
