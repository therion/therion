#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glLightfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum light, pname;
    GLfloat buffer[4];
    void *params;
    int n, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? light pname params");
        return TCL_ERROR;
    }

    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &light) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;

    n = Tom_GetLightNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 3], n * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glLightfv(light, pname, params);

    return TCL_OK;
}

int Tom_glLightiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum light, pname;
    GLint buffer[4];
    void *params;
    int n, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? light pname params");
        return TCL_ERROR;
    }

    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &light) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;

    n = Tom_GetLightNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 3], n * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glLightiv(light, pname, params);

    return TCL_OK;
}

int Tom_glLightModelfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLfloat buffer[4];
    void *params;
    int n, ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &pname) != TCL_OK)
        return TCL_ERROR;

    n = Tom_GetLightModelNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 2], buffer, n, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 2], n * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glLightModelfv(pname, params);

    return TCL_OK;
}

int Tom_glLightModeliv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLint buffer[4];
    void *params;
    int n, ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &pname) != TCL_OK) return TCL_ERROR;

    n = Tom_GetLightModelNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 2], buffer, n, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 2], n * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glLightModeliv(pname, params);

    return TCL_OK;
}
