#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glNormal3b(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbyte nx, ny, nz;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "nx ny nz");
        return TCL_ERROR;
    }
    if ((Tom_GetByteFromObj(interp, objv[1], &nx) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[2], &ny) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[3], &nz) != TCL_OK))
        return TCL_ERROR;

    glNormal3b(nx, ny, nz);

    return TCL_OK;
}

int Tom_glNormal3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble nx, ny, nz;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "nx ny nz");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &nx) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &ny) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &nz) != TCL_OK))
        return TCL_ERROR;

    glNormal3d(nx, ny, nz);

    return TCL_OK;
}

int Tom_glNormal3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat nx, ny, nz;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "nx ny nz");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &nx) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &ny) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &nz) != TCL_OK))
        return TCL_ERROR;

    glNormal3f(nx, ny, nz);

    return TCL_OK;
}

int Tom_glNormal3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint nx, ny, nz;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "nx ny nz");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &nx) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &ny) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &nz) != TCL_OK))
        return TCL_ERROR;

    glNormal3i(nx, ny, nz);

    return TCL_OK;
}

int Tom_glNormal3s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort nx, ny, nz;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "nx ny nz");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &nx) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &ny) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[3], &nz) != TCL_OK))
        return TCL_ERROR;

    glNormal3s(nx, ny, nz);

    return TCL_OK;
}

int Tom_glNormal3bv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbyte buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_BYTE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLbyte), swt)) == 0)
            return TCL_ERROR;
    }

    glNormal3bv(v);

    return TCL_OK;
}

int Tom_glNormal3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glNormal3dv(v);

    return TCL_OK;
}

int Tom_glNormal3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glNormal3fv(v);

    return TCL_OK;
}

int Tom_glNormal3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glNormal3iv(v);

    return TCL_OK;
}

int Tom_glNormal3sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLshort), swt)) == 0)
            return TCL_ERROR;
    }

    glNormal3sv(v);

    return TCL_OK;
}
