#include <stdlib.h>
#include <string.h>
#include "tom.h"

int Tom_glPointSize(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat size;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "size");
        return TCL_ERROR;
    }
    if (Tom_GetFloatFromObj(interp, objv[1], &size) != TCL_OK) return TCL_ERROR;

    glPointSize(size);

    return TCL_OK;
}

int Tom_glLineWidth(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat width;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "width");
        return TCL_ERROR;
    }
    if (Tom_GetFloatFromObj(interp, objv[1], &width) != TCL_OK) return TCL_ERROR;

    glLineWidth(width);

    return TCL_OK;
}

int Tom_glLineStipple(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint factor;
    GLushort pattern;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "factor pattern");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &factor) != TCL_OK) ||
        (Tom_GetUShortFromObj(interp, objv[2], &pattern) != TCL_OK))
        return TCL_ERROR;

    glLineStipple(factor, pattern);

    return TCL_OK;
}

int Tom_glPolygonMode(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum face, mode;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "face mode");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &face) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &mode) != TCL_OK))
        return TCL_ERROR;

    glPolygonMode(face, mode);

    return TCL_OK;
}

int Tom_glPixelStoref(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLfloat param;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "pname param");
        return TCL_ERROR;
    }

    if ((Tom_GetEnumFromObj(interp, objv[1], &pname) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &param) != TCL_OK)) {
        return TCL_ERROR;
    }

    glPixelStoref(pname, param);

    return TCL_OK;
}

int Tom_glPixelStorei(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLint param;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "pname param");
        return TCL_ERROR;
    }

    if ((Tom_GetEnumFromObj(interp, objv[1], &pname) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &param) != TCL_OK)) {
        return TCL_ERROR;
    }

    glPixelStorei(pname, param);

    return TCL_OK;
}

int Tom_glTexImage1D(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, format, type;
    GLint level, components, border;
    GLsizei width;
    int size, bsize;
    GLvoid *buffer, *pixels;
    int ns, swt = tom->swt;

    ns = (objc > 9)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 9) {
        Tcl_WrongNumArgs(interp, 1, objv,
                         "?switches? target level components " "width border format type pixels");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 2], &level) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 3], &components) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 4], &width) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 5], &border) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 6], &format) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 7], &type) != TCL_OK))
        return TCL_ERROR;

    size = width * components;
    bsize = size * Tom_GetTypeSize(type);
    if (bsize == 0) {
        Tcl_AppendResult(interp, "error: invalid type \"",
                         Tcl_GetStringFromObj(objv[ns + 7], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if ((buffer = Tcl_Alloc(bsize)) == NULL) {
            Tcl_AppendResult(interp, "error: memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 8], buffer, size, type, swt) == TCL_ERROR) {
            Tcl_Free(buffer);
            return TCL_ERROR;
        }
        pixels = buffer;
    } else {
        if ((pixels = Tom_GetByteArrayFromObj(interp, objv[ns + 8], bsize, swt)) == 0)
            return TCL_ERROR;
    }

    glTexImage1D(target, level, components, width, border, format, type, pixels);

    if ((swt & TOM_ASCII) != 0) Tcl_Free(buffer);

    return TCL_OK;
}

int Tom_glTexImage2D(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, format, type;
    GLint level, components, border;
    GLsizei width, height;
    int size, bsize;
    GLvoid *buffer, *pixels;
    int ns, swt = tom->swt;

    ns = (objc > 10)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 10) {
        Tcl_WrongNumArgs(interp, 1, objv,
                         "?switches? target level components "
			 "width height border format type pixels");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 2], &level) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 3], &components) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 4], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 5], &height) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 6], &border) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 7], &format) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 8], &type) != TCL_OK))
        return TCL_ERROR;

    size = width * height * components;
    bsize = size * Tom_GetTypeSize(type);
    if (bsize == 0) {
        Tcl_AppendResult(interp, "error: invalid type \"",
                         Tcl_GetStringFromObj(objv[ns + 8], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if ((buffer = Tcl_Alloc(bsize)) == NULL) {
            Tcl_AppendResult(interp, "error: memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 9], buffer, size, type, swt) == TCL_ERROR) {
            Tcl_Free(buffer);
            return TCL_ERROR;
        }
        pixels = buffer;
    } else {
        if ((pixels = Tom_GetByteArrayFromObj(interp, objv[ns + 9], bsize, swt)) == 0)
            return TCL_ERROR;
    }

    glTexImage2D(target, level, components, width, height, border, format, type, pixels);

    if ((swt & TOM_ASCII) != 0) Tcl_Free(buffer);

    return TCL_OK;
}

int Tom_glTexEnvf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, pname;
    GLfloat param;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "target pname param");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &pname) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &param) != TCL_OK))
        return TCL_ERROR;

    glTexEnvf(target, pname, param);

    return TCL_OK;
}

int Tom_glTexEnvi(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, pname;
    GLint param;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "target pname param");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &pname) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &param) != TCL_OK))
        return TCL_ERROR;

    glTexEnvi(target, pname, param);

    return TCL_OK;
}

int Tom_glTexEnvfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, pname;
    GLfloat buffer[4];
    void *params;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? target pname params");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;
    n = Tom_GetTexNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 3], n * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glTexEnvfv(target, pname, params);

    return TCL_OK;
}

int Tom_glTexEnviv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, pname;
    GLint buffer[4];
    void *params;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? target pname params");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;
    n = Tom_GetTexNParams(pname);
    if ((Tom_GetEnumFromObj(interp, objv[ns + 1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK))
        return TCL_ERROR;
    n = Tom_GetTexNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 3], buffer, n, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 3], n * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glTexEnviv(target, pname, params);

    return TCL_OK;
}

int Tom_glTexParameterf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, pname;
    GLfloat param;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "target pname param");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &pname) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &param) != TCL_OK))
        return TCL_ERROR;

    glTexParameterf(target, pname, param);

    return TCL_OK;
}

int Tom_glTexParameteri(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum target, pname;
    GLint param;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "target pname param");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &target) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[2], &pname) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &param) != TCL_OK))
        return TCL_ERROR;

    glTexParameteri(target, pname, param);

    return TCL_OK;
}

int Tom_glFogfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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
    n = Tom_GetFogNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 2], buffer, n, GL_FLOAT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 2], n * sizeof (GLfloat), swt)) == 0)
            return TCL_ERROR;
    }

    glFogfv(pname, params);

    return TCL_OK;
}

int Tom_glFogiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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
    n = Tom_GetFogNParams(pname);
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 2], buffer, n, GL_INT, swt) == TCL_ERROR)
            return TCL_ERROR;
        params = buffer;
    } else {
        if ((params = Tom_GetByteArrayFromObj(interp, objv[ns + 2], n * sizeof (GLint), swt)) == 0)
            return TCL_ERROR;
    }

    glFogiv(pname, params);

    return TCL_OK;
}

int Tom_glDrawPixels(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum format, type;
    GLsizei width, height;
    int bsize, fsize, ssize, tsize;
    GLint align;
    GLvoid *buffer, *pixels;
    int ns, swt = tom->swt;

    ns = (objc > 6)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 6) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? width height format type pixels");
        return TCL_ERROR;
    }
    if ((Tom_GetSizeFromObj(interp, objv[ns + 1], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 2], &height) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 3], &format) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 4], &type) != TCL_OK))
        return TCL_ERROR;
    if ((fsize = Tom_GetFormatSize(format)) == 0) {
        Tcl_AppendResult(interp, "error: invalid format \"",
                         Tcl_GetStringFromObj(objv[ns + 3], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    if ((tsize = Tom_GetTypeSize(type)) == 0) {
        Tcl_AppendResult(interp, "error: invalid type \"",
                         Tcl_GetStringFromObj(objv[ns + 4], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    glGetIntegerv(GL_PACK_ALIGNMENT, &align);
    ssize = width * fsize * tsize;
    ssize = (ssize + (align - 1)) & (-align);
    bsize = ssize * height;
    if ((swt & TOM_ASCII) != 0) {
        if ((buffer = Tcl_Alloc(bsize)) == NULL) {
            Tcl_AppendResult(interp, "error: memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatPixels
            (interp, objv[ns + 5], buffer, width, height, fsize, type, ssize, swt) == TCL_ERROR) {
            Tcl_Free(buffer);
            return TCL_ERROR;
        }
        pixels = buffer;
    } else {
        if ((pixels = Tom_GetByteArrayFromObj(interp, objv[ns + 5], bsize, swt)) == 0)
            return TCL_ERROR;
    }

    glDrawPixels(width, height, format, type, pixels);

    if ((swt & TOM_ASCII) != 0) Tcl_Free(buffer);

    return TCL_OK;
}

int Tom_glReadPixels(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum format, type;
    GLint x, y;
    GLsizei width, height;
    int bsize, fsize, ssize, tsize;
    GLint align;
    GLvoid *buffer;
    Tcl_Obj *list;
    int ns, swt = tom->swt;

    ns = (objc > 8)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 8) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? x y width height format type pixels");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[ns + 1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[ns + 2], &y) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 3], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 4], &height) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 5], &format) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[ns + 6], &type) != TCL_OK))
        return TCL_ERROR;
    if ((fsize = Tom_GetFormatSize(format)) == 0) {
        Tcl_AppendResult(interp, "error: invalid format \"",
                         Tcl_GetStringFromObj(objv[ns + 5], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    if ((tsize = Tom_GetTypeSize(type)) == 0) {
        Tcl_AppendResult(interp, "error: invalid type \"",
                         Tcl_GetStringFromObj(objv[ns + 6], NULL), "\"", NULL);
        return TCL_ERROR;
    }
    glGetIntegerv(GL_PACK_ALIGNMENT, &align);
    ssize = width * fsize * tsize;
    ssize = (ssize + (align - 1)) & (-align);
    bsize = ssize * height;
    if ((buffer = Tcl_Alloc(bsize)) == NULL) {
        Tcl_AppendResult(interp, "error: memory allocation failure", NULL);
        return TCL_ERROR;
    }

    glReadPixels(x, y, width, height, format, type, buffer);

    list = Tom_NewPixels(interp, buffer, width, height, fsize, type, ssize, swt);
    Tcl_Free(buffer);
    if ((list == NULL) ||
        (Tcl_ObjSetVar2(interp, objv[7], NULL, list,
			TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glRasterPos2d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glRasterPos2d(x, y);

    return TCL_OK;
}

int Tom_glRasterPos2f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glRasterPos2f(x, y);

    return TCL_OK;
}

int Tom_glRasterPos2i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glRasterPos2i(x, y);

    return TCL_OK;
}

int Tom_glRasterPos2s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort x, y;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &y) != TCL_OK))
        return TCL_ERROR;

    glRasterPos2s(x, y);

    return TCL_OK;
}

int Tom_glRasterPos3d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glRasterPos3d(x, y, z);

    return TCL_OK;
}

int Tom_glRasterPos3f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glRasterPos3f(x, y, z);

    return TCL_OK;
}

int Tom_glRasterPos3i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glRasterPos3i(x, y, z);

    return TCL_OK;
}

int Tom_glRasterPos3s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort x, y, z;

    if (objc != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[3], &z) != TCL_OK))
        return TCL_ERROR;

    glRasterPos3s(x, y, z);

    return TCL_OK;
}

int Tom_glRasterPos4d(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLdouble x, y, z, w;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z w");
        return TCL_ERROR;
    }
    if ((Tom_GetDoubleFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[3], &z) != TCL_OK) ||
        (Tom_GetDoubleFromObj(interp, objv[4], &w) != TCL_OK))
        return TCL_ERROR;

    glRasterPos4d(x, y, z, w);

    return TCL_OK;
}

int Tom_glRasterPos4f(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat x, y, z, w;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z w");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[3], &z) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[4], &w) != TCL_OK))
        return TCL_ERROR;

    glRasterPos4f(x, y, z, w);

    return TCL_OK;
}

int Tom_glRasterPos4i(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y, z, w;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z w");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[3], &z) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[4], &w) != TCL_OK))
        return TCL_ERROR;

    glRasterPos4i(x, y, z, w);

    return TCL_OK;
}

int Tom_glRasterPos4s(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort x, y, z, w;

    if (objc != 5) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y z w");
        return TCL_ERROR;
    }
    if ((Tom_GetShortFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[3], &z) != TCL_OK) ||
        (Tom_GetShortFromObj(interp, objv[4], &w) != TCL_OK))
        return TCL_ERROR;

    glRasterPos4s(x, y, z, w);

    return TCL_OK;
}

int Tom_glRasterPos2dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos2dv(v);

    return TCL_OK;
}

int Tom_glRasterPos2fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos2fv(v);

    return TCL_OK;
}

int Tom_glRasterPos2iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos2iv(v);

    return TCL_OK;
}

int Tom_glRasterPos2sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLshort buffer[2];
    void *v;
    int ns, swt = tom->swt;

    ns = (objc > 2)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? v");
        return TCL_ERROR;
    }
    if ((swt & TOM_ASCII) != 0) {
        if (Tom_FormatObj(interp, objv[ns + 1], buffer, 2, GL_SHORT, swt) == TCL_ERROR)
            return TCL_ERROR;
        v = buffer;
    } else {
        if ((v = Tom_GetByteArrayFromObj(interp, objv[ns + 1], 2 * sizeof (GLshort), swt)) == 0)
            return TCL_ERROR;
    }

    glRasterPos2sv(v);

    return TCL_OK;
}

int Tom_glRasterPos3dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos3dv(v);

    return TCL_OK;
}

int Tom_glRasterPos3fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos3fv(v);

    return TCL_OK;
}

int Tom_glRasterPos3iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos3iv(v);

    return TCL_OK;
}

int Tom_glRasterPos3sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos3sv(v);

    return TCL_OK;
}

int Tom_glRasterPos4dv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos4dv(v);

    return TCL_OK;
}

int Tom_glRasterPos4fv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos4fv(v);

    return TCL_OK;
}

int Tom_glRasterPos4iv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos4iv(v);

    return TCL_OK;
}

int Tom_glRasterPos4sv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
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

    glRasterPos4sv(v);

    return TCL_OK;
}

int Tom_glBitmap(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLsizei width, height;
    GLfloat xorig, yorig;
    GLfloat xmove, ymove;
    GLubyte *buffer, *bitmap;
    int size;
    int ns, swt = tom->swt;

    ns = (objc > 8)? Tom_ParseSwitches(objc, objv, &swt, 0) : 0;
    if ((objc - ns) != 8) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? width height xorig yorig xmove ymove bitmap");
        return TCL_ERROR;
    }
    if ((Tom_GetSizeFromObj(interp, objv[ns + 1], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[ns + 2], &height) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 3], &xorig) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 4], &yorig) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 5], &xmove) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[ns + 6], &ymove) != TCL_OK))
        return TCL_ERROR;
    size = width * height;
    if ((swt & TOM_ASCII) != 0) {
        if ((buffer = (GLubyte *)Tcl_Alloc(size * sizeof (GLubyte))) == NULL) {
            Tcl_AppendResult(interp, "memory allocation failure", NULL);
            return TCL_ERROR;
        }
        if (Tom_FormatObj(interp, objv[ns + 7], buffer, size, GL_UNSIGNED_BYTE, swt) == TCL_ERROR) {
            Tcl_Free(buffer);
            return TCL_ERROR;
        }
        bitmap = buffer;
    } else {
        if ((bitmap = Tom_GetByteArrayFromObj(interp, objv[ns + 7], size, swt)) == 0)
            return TCL_ERROR;
    }

    glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);

    if ((swt & TOM_ASCII) != 0) Tcl_Free(buffer);

    return TCL_OK;
}

int Tom_glPixelTransferf(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLfloat param;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "pname param");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &pname) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &param) != TCL_OK))
        return TCL_ERROR;

    glPixelTransferf(pname, param);

    return TCL_OK;
}

int Tom_glPixelTransferi(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLint param;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "pname param");
        return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &pname) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &param) != TCL_OK))
        return TCL_ERROR;

    glPixelTransferi(pname, param);

    return TCL_OK;
}

int Tom_glCopyPixels(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLint x, y;
    GLsizei width, height;
    GLenum type;

    if (objc != 6) {
        Tcl_WrongNumArgs(interp, 1, objv, "x y width height type");
        return TCL_ERROR;
    }
    if ((Tom_GetIntFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Tom_GetIntFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[3], &width) != TCL_OK) ||
        (Tom_GetSizeFromObj(interp, objv[4], &height) != TCL_OK) ||
        (Tom_GetEnumFromObj(interp, objv[5], &type) != TCL_OK))
        return TCL_ERROR;

    glCopyPixels(x, y, width, height, type);

    return TCL_OK;
}

int Tom_glPixelZoom(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLfloat xfactor, yfactor;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "xfactor yfactor");
        return TCL_ERROR;
    }
    if ((Tom_GetFloatFromObj(interp, objv[1], &xfactor) != TCL_OK) ||
        (Tom_GetFloatFromObj(interp, objv[2], &yfactor) != TCL_OK))
        return TCL_ERROR;

    glPixelZoom(xfactor, yfactor);

    return TCL_OK;
}
