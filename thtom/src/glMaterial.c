#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glColorMaterial(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum face, mode;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "face mode");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &face) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &mode) != TCL_OK))
        return TCL_ERROR;

    glColorMaterial(face, mode);

    return TCL_OK;
}

int Tom_glMaterialfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum face, pname;
    GLfloat buffer[4];
    void *params;
    int n, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? face pname params");
        return TCL_ERROR;
    }

    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &face) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;

    n = Tom_GetMaterialNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 3], n * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glMaterialfv(face, pname, params);

    return TCL_OK;
}

int Tom_glMaterialiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum face, pname;
    GLint buffer[4];
    void *params;
    int n, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? face pname params");
        return TCL_ERROR;
    }

    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &face) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;

    n = Tom_GetMaterialNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 3], n * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glMaterialiv(face, pname, params);

    return TCL_OK;
}
