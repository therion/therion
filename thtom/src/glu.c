#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_gluPerspective(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble fov, a, n, f;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "fovy aspect near far");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &fov) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &a) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &n) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &f) != TCL_OK))
        return TCL_ERROR;

    gluPerspective(fov, a, n, f);

    return TCL_OK;
}

int Tom_gluLookAt(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz;

    if (objc != 10) {
        Tcl_WrongNumArgs(interp, 1, objv, "eyex eyey eyez centerx centery centerz upx upy upz");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &eyex) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &eyey) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &eyez) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &centerx) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[5], &centery) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[6], &centerz) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[7], &upx) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[8], &upy) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[9], &upz) != TCL_OK))
        return TCL_ERROR;

    gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);

    return TCL_OK;
}

int Tom_gluNewQuadric(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    static int QuadricCounter = 0;
    char buffer[32];

    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }
    QuadricCounter++;
    sprintf(buffer, "q%d", QuadricCounter);
    Tom_CreateHashEntry(buffer, (ClientData)(gluNewQuadric()));
    sprintf(buffer, "%d", QuadricCounter);

    Tcl_AppendResult(interp, buffer, NULL);

    return TCL_OK;
}

int Tom_gluDeleteQuadric(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int qobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "qobj"); 
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &qobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindQEntry(interp, qobj);
    if (hentry == NULL) {
        return TCL_ERROR;
    }

    gluDeleteQuadric((GLUquadricObj *)(Tcl_GetHashValue(hentry)));
    Tcl_DeleteHashEntry(hentry);

    return TCL_OK;
}

int Tom_gluSphere(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int qobj;
    GLdouble radius;
    GLint slices, stacks;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "qobj radius slices stacks");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &qobj) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &radius) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &slices) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &stacks) != TCL_OK)) {
        return TCL_ERROR;
    }
    hentry = Tom_FindQEntry(interp, qobj);
    if (hentry == NULL) return TCL_ERROR;

    gluSphere((GLUquadricObj *)(Tcl_GetHashValue(hentry)), radius, slices, stacks);

    return TCL_OK;
}

int Tom_gluNewNurbsRenderer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    static int NurbsCounter = 0;
    char buffer[32];

    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    NurbsCounter++;
    sprintf(buffer, "n%d", NurbsCounter);
    Tom_CreateHashEntry(buffer, (ClientData)(gluNewNurbsRenderer()));
    sprintf(buffer, "%d", NurbsCounter);
    Tcl_AppendResult(interp, buffer, NULL);

    return TCL_OK;
}

int Tom_gluDeleteNurbsRenderer(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;
 
    gluDeleteNurbsRenderer((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));
    Tcl_DeleteHashEntry(hentry);

    return TCL_OK;
}

int Tom_gluNurbsProperty(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;
    GLenum property;
    GLfloat value;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj property value");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &property) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &value) != TCL_OK))
        return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluNurbsProperty((GLUnurbsObj *)(Tcl_GetHashValue(hentry)), property, value);

    return TCL_OK;
}

int Tom_gluBeginCurve(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluBeginCurve((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));

    return TCL_OK;
}

int Tom_gluBeginSurface(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluBeginSurface((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));

    return TCL_OK;
}

int Tom_gluBeginTrim(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluBeginTrim((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));

    return TCL_OK;
}

int Tom_gluEndCurve(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluEndCurve((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));

    return TCL_OK;
}

int Tom_gluEndSurface(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluEndSurface((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));

    return TCL_OK;
}

int Tom_gluEndTrim(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int nobj;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj");
        return TCL_ERROR;
    }
    if (Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;

    gluEndTrim((GLUnurbsObj *)(Tcl_GetHashValue(hentry)));

    return TCL_OK;
}

int Tom_gluNurbsSurface(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    GLUnurbsObj *nurbs;
    GLint nobj, nuknot, nvknot, ustride, vstride, uorder, vorder;
    GLfloat *uknot, *vknot, *ctrl;
    GLenum type;

    if (objc != 12) {
        Tcl_WrongNumArgs(interp, 1, objv, "nobj nuknot uknot nvknot vknot "
                         "ustride vstride ctrl uorder vorder type");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &nobj) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &nuknot) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &nvknot) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[6], &ustride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[7], &vstride) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[9], &uorder) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[10], &vorder) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[11], &type) != TCL_OK))
        return TCL_ERROR;
    hentry = Tom_FindNEntry(interp, nobj);
    if (hentry == NULL) return TCL_ERROR;
    nurbs = (GLUnurbsObj *)(Tcl_GetHashValue(hentry));
    uknot = (GLfloat *)(Tcl_GetByteArrayFromObj(objv[3], NULL));
    vknot = (GLfloat *)(Tcl_GetByteArrayFromObj(objv[5], NULL));
    ctrl = (GLfloat *)(Tcl_GetByteArrayFromObj(objv[8], NULL));

    gluNurbsSurface(nurbs, nuknot, uknot, nvknot, vknot, ustride, vstride, ctrl, uorder, vorder, type);

    return TCL_OK;
}

int Tom_gluOrtho2D(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble left, right, bottom, top;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "left right bottom top");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &left) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &right) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &bottom) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &top) != TCL_OK))
        return TCL_ERROR;

    gluOrtho2D(left, right, bottom, top);

    return TCL_OK;
}

int Tom_gluPickMatrix(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y, width, height;
    GLint viewport[4];
    int i, length;
    Tcl_Obj *obj;

    if (objc != 6) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y width height viewport");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &width) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &height) != TCL_OK))
        return TCL_ERROR;
    if (Tcl_ListObjLength(interp, objv[5], &length) != TCL_OK) return TCL_ERROR;
    if (length < 4) {
        Tcl_AppendResult(interp, "list should contain at least 4 elements", NULL);
        return TCL_ERROR;
    }
    for (i = 0; i < 4; i++) {
        if (Tcl_ListObjIndex(interp, objv[5], i, &obj) != TCL_OK) return TCL_ERROR;
        if (Tom_GetIntFromObj(interp, obj, viewport + i) != TCL_OK) return TCL_ERROR;
    }

    gluPickMatrix(x, y, width, height, viewport);

    return TCL_OK;
}

int Tom_gluQuadricTexture(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    Tcl_HashEntry *hentry;
    int qobj;
    GLboolean textureCoords;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "qobj textureCoords");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &qobj) != TCL_OK) ||
        (Tom_GetBooleanFromObj(interp, objv[2], &textureCoords) != TCL_OK)) {
        return TCL_ERROR;
    }
    hentry = Tom_FindQEntry(interp, qobj);
    if (hentry == NULL) return TCL_ERROR;

    gluQuadricTexture((GLUquadricObj *)(Tcl_GetHashValue(hentry)), textureCoords);

    return TCL_OK;
}
