#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glInitNames(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glInitNames();

    return TCL_OK;
}

int Tom_glLoadName(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint name;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "name");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &name) != TCL_OK) return TCL_ERROR;

    glLoadName(name);

    return TCL_OK;
}

int Tom_glPushName(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint name;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "name");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &name) != TCL_OK) return TCL_ERROR;

    glPushName(name);

    return TCL_OK;
}

int Tom_glPopName(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glPopName();

    return TCL_OK;
}
