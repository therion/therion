#include <stdlib.h>
#include <string.h>
#include "tom.h"

#ifdef GLUT
int Tom_glutSolidDodecahedron(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }

    glutSolidDodecahedron();

    return TCL_OK;
}

int Tom_glutWireDodecahedron(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }

    glutWireDodecahedron();

    return TCL_OK;
}

int Tom_glutSolidCube(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble size;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }
    if (Tom_GetDoubleFromObj(interp, objv[1], &size) != TCL_OK) return TCL_ERROR;

    glutSolidCube(size);

    return TCL_OK;
}

int Tom_glutWireCube(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble size;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }
    if (Tom_GetDoubleFromObj(interp, objv[1], &size) != TCL_OK) return TCL_ERROR;

    glutWireCube(size);

    return TCL_OK;
}

int Tom_glutSolidSphere(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble radius;
    GLint slices, stacks;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "radius slices stacks");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &radius) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &slices) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &stacks) != TCL_OK))
        return TCL_ERROR;

    glutSolidSphere(radius, slices, stacks);

    return TCL_OK;
}

int Tom_glutWireSphere(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble radius;
    GLint slices, stacks;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "radius slices stacks");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &radius) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &slices) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &stacks) != TCL_OK))
        return TCL_ERROR;

    glutWireSphere(radius, slices, stacks);

    return TCL_OK;
}

int Tom_glutSolidCone(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble base, height;
    GLint slices, stacks;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "radius slices stacks");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &base) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &height) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &slices) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &stacks) != TCL_OK))
        return TCL_ERROR;

    glutSolidCone(base, height, slices, stacks);

    return TCL_OK;
}

int Tom_glutWireCone(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble base, height;
    GLint slices, stacks;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "radius slices stacks");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &base) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &height) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &slices) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &stacks) != TCL_OK))
        return TCL_ERROR;

    glutWireCone(base, height, slices, stacks);

    return TCL_OK;
}

int Tom_glutSolidTeapot(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble size;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }
    if (Tom_GetDoubleFromObj(interp, objv[1], &size) != TCL_OK)
        return TCL_ERROR;

    glutSolidTeapot(size);

    return TCL_OK;
}

int Tom_glutWireTeapot(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble size;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }
    if (Tom_GetDoubleFromObj(interp, objv[1], &size) != TCL_OK) return TCL_ERROR;

    glutWireTeapot(size);

    return TCL_OK;
}
#endif
