#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glVertex2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glVertex2d(x, y);

    return TCL_OK;
}

int Tom_glVertex2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glVertex2f(x, y);

    return TCL_OK;
}

int Tom_glVertex2i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glVertex2i(x, y);

    return TCL_OK;
}

int Tom_glVertex2dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble buffer[2];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 2, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 2 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glVertex2dv(v);

    return TCL_OK;
}

int Tom_glVertex2fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat buffer[2];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 2, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 2 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glVertex2fv(v);

    return TCL_OK;
}

int Tom_glVertex2iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint buffer[2];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 2, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 2 * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glVertex2iv(v);

    return TCL_OK;
}

int Tom_glVertex3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glVertex3d(x, y, z);

    return TCL_OK;
}

int Tom_glVertex3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glVertex3f(x, y, z);

    return TCL_OK;
}

int Tom_glVertex3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glVertex3i(x, y, z);

    return TCL_OK;
}

int Tom_glVertex3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glVertex3dv(v);

    return TCL_OK;
}

int Tom_glVertex3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glVertex3fv(v);

    return TCL_OK;
}

int Tom_glVertex3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glVertex3iv(v);

    return TCL_OK;
}
