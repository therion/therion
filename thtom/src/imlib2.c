#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
//#include <Imlib2.h>
#include "tom.h"

int Tom_WidgetLoadCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc < 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? load name ?width? height?");
        return TCL_ERROR;
    }
    return TCL_ERROR;
}

int Tom_WidgetSaveCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc < 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? load name ?width? height?");
        return TCL_ERROR;
    }
    return TCL_ERROR;
}
