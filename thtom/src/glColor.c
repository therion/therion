#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glColor3b(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbyte red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetByteFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3b(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3ub(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLubyte red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetUByteFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetUByteFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetUByteFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3ub(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3d(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3f(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3i(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3ui(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetUIntFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetUIntFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetUIntFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3ui(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3s(red, green, blue);

    return TCL_OK;
}

int Tom_glColor3us(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLushort red, green, blue;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue");
        return TCL_ERROR;
    }
    if ((Tom_GetUShortFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetUShortFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetUShortFromObj(interp, objv[3], &blue) != TCL_OK))
        return TCL_ERROR;

    glColor3us(red, green, blue);

    return TCL_OK;
}

int Tom_glColor4b(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbyte red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetByteFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetByteFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4b(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4ub(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLubyte red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetUByteFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetUByteFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetUByteFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetUByteFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4ub(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4d(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4f(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4i(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4ui(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetUIntFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetUIntFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetUIntFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetUIntFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4ui(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4s(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor4us(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLushort red, green, blue, alpha;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "red green blue alpha");
        return TCL_ERROR;
    }
    if ((Tom_GetUShortFromObj(interp, objv[1], &red) != TCL_OK) ||
        (Tom_GetUShortFromObj(interp, objv[2], &green) != TCL_OK) ||
        (Tom_GetUShortFromObj(interp, objv[3], &blue) != TCL_OK) ||
        (Tom_GetUShortFromObj(interp, objv[4], &alpha) != TCL_OK))
        return TCL_ERROR;

    glColor4us(red, green, blue, alpha);

    return TCL_OK;
}

int Tom_glColor3bv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glColor3bv(v);

    return TCL_OK;
}

int Tom_glColor3ubv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbyte buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_UNSIGNED_BYTE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLubyte), swt)) == 0)
            return TCL_ERROR;
    }

    glColor3ubv(v);

    return TCL_OK;
}

int Tom_glColor3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glColor3dv(v);

    return TCL_OK;
}

int Tom_glColor3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glColor3fv(v);

    return TCL_OK;
}

int Tom_glColor3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glColor3iv(v);

    return TCL_OK;
}

int Tom_glColor3uiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_UNSIGNED_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLuint), swt)) == 0)
            return TCL_ERROR;
    }

    glColor3uiv(v);

    return TCL_OK;
}

int Tom_glColor3sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glColor3sv(v);

    return TCL_OK;
}

int Tom_glColor3usv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLushort buffer[3];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 3, GL_UNSIGNED_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 3 * sizeof (GLushort), swt)) == 0)
            return TCL_ERROR;
    }

    glColor3usv(v);

    return TCL_OK;
}

int Tom_glColor4bv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbyte buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_BYTE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLbyte), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4bv(v);

    return TCL_OK;
}

int Tom_glColor4ubv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLubyte buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_UNSIGNED_BYTE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLubyte), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4ubv(v);

    return TCL_OK;
}

int Tom_glColor4dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_DOUBLE, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLdouble), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4dv(v);

    return TCL_OK;
}

int Tom_glColor4fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4fv(v);

    return TCL_OK;
}

int Tom_glColor4iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4iv(v);

    return TCL_OK;
}

int Tom_glColor4uiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_UNSIGNED_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLuint), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4uiv(v);

    return TCL_OK;
}

int Tom_glColor4sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLshort), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4sv(v);

    return TCL_OK;
}

int Tom_glColor4usv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLushort buffer[4];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }

    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 4, GL_UNSIGNED_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 4 * sizeof (GLushort), swt)) == 0)
            return TCL_ERROR;
    }

    glColor4usv(v);

    return TCL_OK;
}
