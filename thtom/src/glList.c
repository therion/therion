#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glGenLists(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLsizei range;
    GLuint start;
    char buffer[32];

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "range");
        return TCL_ERROR;
    }
    if (Tom_GetSizeFromObj(interp, objv[1], &range) != TCL_OK) return TCL_ERROR;

    start = glGenLists(range);

    if (start == 0) {
        Tcl_AppendResult(interp, "no group of ",
                         Tcl_GetStringFromObj(objv[1], NULL), " contiguous names available", NULL);
        return TCL_ERROR;
    }
    sprintf(buffer, "%d", start);
    Tcl_AppendResult(interp, buffer, NULL);

    return TCL_OK;
}

int Tom_glNewList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint list;
    GLenum mode;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "list mode");
        return TCL_ERROR;
    }
    if ((Tom_GetUIntFromObj(interp, objv[1], &list) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &mode) != TCL_OK))
        return TCL_ERROR;

    glNewList(list, mode);

    return TCL_OK;
}

int Tom_glEndList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glEndList();

    return TCL_OK;
}

int Tom_glCallList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint list;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "list");
        return TCL_ERROR;
    }
    if (Tom_GetUIntFromObj(interp, objv[1], &list) != TCL_OK)
        return TCL_ERROR;

    glCallList(list);

    return TCL_OK;
}

int Tom_glCallLists(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLsizei n;
    GLenum type;
    GLvoid *lists, *buffer;
    int size, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? n type lists");
        return TCL_ERROR;
    }
    if ((Tom_GetSizeFromObj(interp, objv[ns + 1], &n) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &type) != TCL_OK))
        return TCL_ERROR;

    size = n * Tom_GetTypeSize(type);
    if (size == 0) {
        Tcl_AppendResult(interp, "unknown type \"", Tcl_GetStringFromObj(objv[ns + 2], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if ((buffer = Tcl_Alloc(size)) == NULL) {
            Tcl_AppendResult(interp, "memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, type, swt) == TCL_ERROR) {
            Tcl_Free(buffer);
            return TCL_ERROR;
        }
        lists = buffer;
    } else {
        if ((lists = Tom_GetByteArrayFromObj(interp, objv[ns + 3], size, swt)) == 0)
            return TCL_ERROR;
    }

    glCallLists(n, type, lists);

    if ((swt & TOM_ASCII) != 0) Tcl_Free(buffer);

    return TCL_OK;
}

int Tom_glDeleteLists(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint list;
    GLsizei range;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "list range");
        return TCL_ERROR;
    }
    if ((Tom_GetUIntFromObj(interp, objv[1], &list) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[2], &range) != TCL_OK))
        return TCL_ERROR;

    glDeleteLists(list, range);

    return TCL_OK;
}

int Tom_glListBase(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint base;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "base");
        return TCL_ERROR;
    }
    if (Tom_GetUIntFromObj(interp, objv[1], &base) != TCL_OK)
        return TCL_ERROR;

    glListBase(base);

    return TCL_OK;
}
