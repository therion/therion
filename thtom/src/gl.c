#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glGetError(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum err;
    int n;
    char buffer[32];

    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    err = glGetError();
    if (err == GL_NO_ERROR) {
        sprintf(buffer, "%d", err);
        Tcl_AppendResult(interp, buffer, NULL);
    } else {
        n = 0;
        while (err != GL_NO_ERROR) {
            sprintf(buffer, "%d", err);
            if (n == 0) Tcl_AppendResult(interp, buffer, NULL);
            else Tcl_AppendResult(interp, " ", buffer, NULL);
            n++;
            err = glGetError();
        }
    }

    return TCL_OK;
}

int Tom_glEnable(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum cap;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "cap");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[1], &cap) != TCL_OK)
        return TCL_ERROR;

    glEnable(cap);

    return TCL_OK;
}

int Tom_glDisable(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum cap;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "cap");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[1], &cap) != TCL_OK) return TCL_ERROR;

    glDisable(cap);

    return TCL_OK;
}

int Tom_glShadeModel(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glShadeModel(mode);

    return TCL_OK;
}

int Tom_glBegin(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glBegin(mode);

    return TCL_OK;
}

int Tom_glEnd(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glEnd();

    return TCL_OK;
}

int Tom_glViewport(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y;
    GLsizei width, height;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y width height");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[3], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[4], &height) != TCL_OK))
        return TCL_ERROR;

    glViewport(x, y, width, height);

    return TCL_OK;
}

int Tom_glClipPlane(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum plane;
    GLdouble *equation, buffer[4];
    int ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "plane equation");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &plane) != TCL_OK) return TCL_ERROR;
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 2], buffer, 4, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        equation = buffer;
    } else {
        if ((equation = Tom_GetByteArrayFromObj(interp, objv[ns + 2], 4 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glClipPlane(plane, equation);

    return TCL_OK;
}

int Tom_glFlush(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glFlush();

    return TCL_OK;
}

int Tom_glFinish(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glFinish();

    return TCL_OK;
}

int Tom_glFrontFace(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glFrontFace(mode);

    return TCL_OK;
}

int Tom_glCullFace(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum mode;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mode");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK) return TCL_ERROR;

    glCullFace(mode);

    return TCL_OK;
}

int Tom_glEdgeFlag(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLboolean flag;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "flag");
        return TCL_ERROR;
    }
    if (Tom_GetBooleanFromObj(interp, objv[1], &flag) != TCL_OK) return TCL_ERROR;

    glEdgeFlag(flag);

    return TCL_OK;
}

