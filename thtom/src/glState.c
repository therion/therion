#include "tom.h"

int Tom_GetNParams(GLenum pname) {
    switch (pname) {
        /* one value */
    case GL_ACCUM_ALPHA_BITS:
    case GL_ACCUM_BLUE_BITS:
    case GL_ACCUM_GREEN_BITS:
    case GL_ACCUM_RED_BITS:
    case GL_ALPHA_BIAS:
    case GL_ALPHA_BITS:
    case GL_ALPHA_SCALE:
    case GL_ALPHA_TEST:
    case GL_ALPHA_TEST_FUNC:
    case GL_ALPHA_TEST_REF:
    case GL_ATTRIB_STACK_DEPTH:
    case GL_AUTO_NORMAL:
    case GL_AUX_BUFFERS:
    case GL_BLEND:
    case GL_BLEND_DST:
    case GL_BLEND_SRC:
    case GL_BLUE_BIAS:
    case GL_BLUE_BITS:
    case GL_BLUE_SCALE:
    case GL_COLOR_ARRAY_EXT:
    case GL_COLOR_ARRAY_COUNT_EXT:
    case GL_COLOR_ARRAY_SIZE_EXT:
    case GL_COLOR_ARRAY_TYPE_EXT:
    case GL_COLOR_MATERIAL:
    case GL_COLOR_MATERIAL_FACE:
    case GL_CULL_FACE:
    case GL_CULL_FACE_MODE:
    case GL_CURRENT_INDEX:
    case GL_CURRENT_RASTER_INDEX:
    case GL_DEPTH_BIAS:
    case GL_DEPTH_BITS:
    case GL_DEPTH_CLEAR_VALUE:
    case GL_DEPTH_FUNC:
    case GL_DEPTH_SCALE:
    case GL_DEPTH_TEST:
    case GL_DEPTH_WRITEMASK:
    case GL_DITHER:
    case GL_DOUBLEBUFFER:
    case GL_DRAW_BUFFER:
    case GL_EDGE_FLAG:
    case GL_EDGE_FLAG_ARRAY_EXT:
    case GL_FOG:
    case GL_FOG_DENSITY:
    case GL_FOG_END:
    case GL_FOG_HINT:
    case GL_FOG_INDEX:
    case GL_FOG_MODE:
    case GL_FOG_START:
    case GL_FRONT_FACE:
    case GL_GREEN_BIAS:
    case GL_GREEN_BITS:
    case GL_GREEN_SCALE:
    case GL_INDEX_ARRAY_EXT:
    case GL_INDEX_ARRAY_COUNT_EXT:
    case GL_INDEX_ARRAY_TYPE_EXT:
    case GL_INDEX_BITS:
    case GL_INDEX_CLEAR_VALUE:
    case GL_INDEX_MODE:
    case GL_INDEX_OFFSET:
    case GL_INDEX_SHIFT:
    case GL_INDEX_WRITEMASK:
    case GL_LIGHTING:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_LINE_SMOOTH:
    case GL_LINE_SMOOTH_HINT:
    case GL_LINE_STIPPLE:
    case GL_LINE_STIPPLE_PATTERN:
    case GL_LINE_STIPPLE_REPEAT:
    case GL_LINE_WIDTH:
    case GL_LIST_BASE:
    case GL_LIST_INDEX:
    case GL_LIST_MODE:
    case GL_LOGIC_OP:
    case GL_LOGIC_OP_MODE:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_GRID_SEGMENTS:
    case GL_MAP1_INDEX:
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_1:
    case GL_MAP1_TEXTURE_COORD_2:
    case GL_MAP1_TEXTURE_COORD_3:
    case GL_MAP1_TEXTURE_COORD_4:
    case GL_MAP1_VERTEX_3:
    case GL_MAP1_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_INDEX:
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_1:
    case GL_MAP2_TEXTURE_COORD_2:
    case GL_MAP2_TEXTURE_COORD_3:
    case GL_MAP2_TEXTURE_COORD_4:
    case GL_MAP2_VERTEX_3:
    case GL_MAP2_VERTEX_4:
    case GL_MAP_COLOR:
    case GL_MAP_STENCIL:
    case GL_MATRIX_MODE:
    case GL_MAX_CLIP_PLANES:
    case GL_MAX_EVAL_ORDER:
    case GL_MAX_LIGHTS:
    case GL_MAX_LIST_NESTING:
    case GL_MAX_NAME_STACK_DEPTH:
    case GL_MAX_PIXEL_MAP_TABLE:
    case GL_MAX_TEXTURE_SIZE:
    case GL_MODELVIEW_STACK_DEPTH:
    case GL_NAME_STACK_DEPTH:
    case GL_NORMAL_ARRAY_EXT:
    case GL_NORMAL_ARRAY_TYPE_EXT:
    case GL_NORMALIZE:
    case GL_PACK_ALIGNMENT:
    case GL_PACK_LSB_FIRST:
    case GL_PACK_ROW_LENGTH:
    case GL_PACK_SKIP_PIXELS:
    case GL_PACK_SKIP_ROWS:
    case GL_PACK_SWAP_BYTES:
    case GL_PIXEL_MAP_A_TO_A_SIZE:
    case GL_PIXEL_MAP_B_TO_B_SIZE:
    case GL_PIXEL_MAP_G_TO_G_SIZE:
    case GL_PIXEL_MAP_I_TO_A_SIZE:
    case GL_PIXEL_MAP_I_TO_B_SIZE:
    case GL_PIXEL_MAP_I_TO_G_SIZE:
    case GL_PIXEL_MAP_I_TO_I_SIZE:
    case GL_PIXEL_MAP_I_TO_R_SIZE:
    case GL_PIXEL_MAP_R_TO_R_SIZE:
    case GL_PIXEL_MAP_S_TO_S_SIZE:
    case GL_POINT_SIZE:
    case GL_POINT_SMOOTH:
    case GL_POINT_SMOOTH_HINT:
    case GL_POLYGON_SMOOTH:
    case GL_POLYGON_SMOOTH_HINT:
    case GL_POLYGON_STIPPLE:
    case GL_READ_BUFFER:
    case GL_RED_BIAS:
    case GL_RED_BITS:
    case GL_RED_SCALE:
    case GL_RENDER_MODE:
    case GL_RGBA_MODE:
    case GL_SCISSOR_TEST:
    case GL_SHADE_MODEL:
    case GL_STENCIL_BITS:
    case GL_STENCIL_CLEAR_VALUE:
    case GL_STENCIL_FAIL:
    case GL_STENCIL_FUNC:
    case GL_STENCIL_REF:
    case GL_STENCIL_TEST:
    case GL_STENCIL_VALUE_MASK:
    case GL_STENCIL_WRITEMASK:
    case GL_STEREO:
    case GL_SUBPIXEL_BITS:
    case GL_TEXTURE_1D:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_GEN_S:
    case GL_TEXTURE_GEN_T:
    case GL_TEXTURE_GEN_R:
    case GL_TEXTURE_GEN_Q:
    case GL_TEXTURE_STACK_DEPTH:
    case GL_UNPACK_ALIGNMENT:
    case GL_UNPACK_LSB_FIRST:
    case GL_UNPACK_ROW_LENGTH:
    case GL_UNPACK_SKIP_PIXELS:
    case GL_UNPACK_SKIP_ROWS:
    case GL_UNPACK_SWAP_BYTES:
    case GL_VERTEX_ARRAY_EXT:
    case GL_VERTEX_ARRAY_SIZE_EXT:
    case GL_VERTEX_ARRAY_TYPE_EXT:
    case GL_ZOOM_X:
    case GL_ZOOM_Y:
        return 1;
        /* two values */
    case GL_DEPTH_RANGE:
    case GL_LINE_WIDTH_RANGE:
    case GL_MAP1_GRID_DOMAIN:
    case GL_MAP2_GRID_SEGMENTS:
    case GL_MAX_VIEWPORT_DIMS:
    case GL_POINT_SIZE_RANGE:
    case GL_POLYGON_MODE:
        return 2;
        /* three values */
    case GL_CURRENT_NORMAL:
        return 3;
        /* four values */
    case GL_ACCUM_CLEAR_VALUE:
    case GL_COLOR_CLEAR_VALUE:
    case GL_COLOR_WRITEMASK:
    case GL_CURRENT_COLOR:
    case GL_CURRENT_RASTER_COLOR:
    case GL_FOG_COLOR:
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_MAP2_GRID_DOMAIN:
    case GL_SCISSOR_BOX:
    case GL_VIEWPORT:
        return 4;
        /* sixteen values */
    case GL_MODELVIEW_MATRIX:
    case GL_PROJECTION_MATRIX:
    case GL_TEXTURE_MATRIX:
        return 16;
    default:
        return -1;
    }
}

int Tom_GetMaterialNParams(GLenum pname) {
    switch (pname) {
        /* four values */
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_AMBIENT_AND_DIFFUSE:
    case GL_EMISSION:
    case GL_SPECULAR:
        return 4;
        /* one value */
    case GL_SHININESS:
        return 1;
        /* three values */
    case GL_COLOR_INDEXES:
        return 3;
    default:
        return -1;
    }
}

int Tom_GetLightNParams(GLenum pname) {
    switch (pname) {
        /* four values */
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_POSITION:
        return 4;
        /* three values */
    case GL_SPOT_DIRECTION:
        return 3;
        /* one value */
    case GL_SPOT_EXPONENT:
    case GL_SPOT_CUTOFF:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
        return 1;
    default:
        return -1;
    }
}

int Tom_GetLightModelNParams(GLenum pname) {
    switch (pname) {
    case GL_LIGHT_MODEL_AMBIENT:
        return 4;
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
    case GL_LIGHT_MODEL_TWO_SIDE:
        return 1;
    default:
        return -1;
    }
}

int Tom_GetFogNParams(GLenum pname) {
    switch (pname) {
    case GL_FOG_MODE:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
    case GL_FOG_INDEX:
        return 1;
    case GL_FOG_COLOR:
        return 4;
    default:
        return -1;
    }
}

int Tom_GetTexNParams(GLenum pname) {
    switch (pname) {
    case GL_TEXTURE_ENV_MODE:
        return 1;
    case GL_TEXTURE_ENV_COLOR:
        return 4;
    default:
        return -1;
    }
}

int Tom_glGetBooleanv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLboolean params[16];
    GLubyte buffer[16];
    Tcl_Obj *list;
    int i, n = 0, ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &pname) != TCL_OK) return TCL_ERROR;

    glGetBooleanv(pname, params);
    n = Tom_GetNParams(pname);
    for (i = 0; i < n; i++) buffer[i] = ((GLubyte *)params)[i];

    if (((list = Tom_NewList(interp, params, n, GL_UNSIGNED_BYTE, swt)) == NULL) ||
	(Tcl_ObjSetVar2(interp, objv[2], NULL, list, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glGetDoublev(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLdouble params[16];
    Tcl_Obj *list;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &pname) != TCL_OK) return TCL_ERROR;

    glGetDoublev(pname, params);
    n = Tom_GetNParams(pname);

    if (((list = Tom_NewList(interp, params, n, GL_DOUBLE, swt)) == NULL) ||
        (Tcl_ObjSetVar2(interp, objv[2], NULL, list, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glGetFloatv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLfloat params[16];
    Tcl_Obj *list;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &pname) != TCL_OK) return TCL_ERROR;

    glGetFloatv(pname, params);
    n = Tom_GetNParams(pname);

    if (((list = Tom_NewList(interp, params, n, GL_FLOAT, swt)) == NULL) ||
        (Tcl_ObjSetVar2(interp, objv[2], NULL, list, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glGetIntegerv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum pname;
    GLint params[16];
    Tcl_Obj *list;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 3)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &pname) != TCL_OK) return TCL_ERROR;

    glGetIntegerv(pname, params);
    n = Tom_GetNParams(pname);

    if (((list = Tom_NewList(interp, params, n, GL_INT, swt)) == NULL) ||
        (Tcl_ObjSetVar2(interp, objv[2], NULL, list, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glGetMaterialfv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum face, pname;
    GLfloat params[4];
    Tcl_Obj *list;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? face pname params");
        return TCL_ERROR;
    }

    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &face) != TCL_OK) return TCL_ERROR;
    if (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK) return TCL_ERROR;

    glGetMaterialfv(face, pname, params);
    n = Tom_GetMaterialNParams(pname);

    if (((list = Tom_NewList(interp, params, n, GL_FLOAT, swt)) == NULL) ||
        (Tcl_ObjSetVar2(interp, objv[2], NULL, list, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glGetMaterialiv(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLenum face, pname;
    GLint params[4];
    Tcl_Obj *list;
    int n = 0, ns, swt = tom->swt;

    ns = (objc > 4)? Tom_ParseSwitches(objc, objv, &swt, TOM_ASCII) : 0;
    if ((objc - ns) != 4) {
        Tcl_WrongNumArgs(interp, 1, objv, "?switches? face pname params");
        return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[ns + 1], &face) != TCL_OK) return TCL_ERROR;
    if (Tom_GetEnumFromObj(interp, objv[ns + 2], &pname) != TCL_OK) return TCL_ERROR;

    glGetMaterialiv(face, pname, params);
    n = Tom_GetMaterialNParams(pname);

    if (((list = Tom_NewList(interp, params, n, GL_INT, swt)) == NULL) ||
        (Tcl_ObjSetVar2(interp, objv[2], NULL, list, TCL_PARSE_PART1 | TCL_LEAVE_ERR_MSG) == NULL))
        return TCL_ERROR;

    return TCL_OK;
}

int Tom_glPushAttrib(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLbitfield mask;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "mask");
        return TCL_ERROR;
    }
    if (Tom_GetBitfieldFromObj(interp, objv[1], &mask) != TCL_OK) return TCL_ERROR;

    glPushAttrib(mask);

    return TCL_OK;
}

int Tom_glPopAttrib(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    if (objc != 1) {
        Tcl_WrongNumArgs(interp, 1, objv, NULL);
        return TCL_ERROR;
    }

    glPopAttrib();

    return TCL_OK;
}

int Tom_glIsList(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj * CONST objv[]) {
    GLuint list;

    if (objc != 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "list");
        return TCL_ERROR;
    }
    if (Tom_GetUIntFromObj(interp, objv[1], &list) != TCL_OK) return TCL_ERROR;

    Tcl_AppendResult(interp, glIsList(list)? "1" : "0", NULL);

    return TCL_OK;
}

int Tom_glIsEnabled(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
    GLenum cap;

    if (objc != 2) {
      Tcl_WrongNumArgs(interp, 1, objv, "cap");
      return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &cap) != TCL_OK) return TCL_ERROR;

    Tcl_AppendResult(interp, glIsEnabled(cap)? "1" : "0", NULL);

    return TCL_OK;
}

int Tom_glHint(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
    GLenum target, mode;

    if (objc != 3) {
      Tcl_WrongNumArgs(interp, 1, objv, "taget mode");
      return TCL_ERROR;
    }
    if ((Tom_GetEnumFromObj(interp, objv[1], &target) != TCL_OK) ||
	(Tom_GetEnumFromObj(interp, objv[1], &mode) != TCL_OK))
      return TCL_ERROR;

    glHint(target, mode);

    return TCL_OK;
}

int Tom_glGetString(TOM *tom, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
    GLenum name;
    const GLubyte *str;

    if (objc != 2) {
      Tcl_WrongNumArgs(interp, 1, objv, "name");
      return TCL_ERROR;
    }
    if (Tom_GetEnumFromObj(interp, objv[1], &name) != TCL_OK) return TCL_ERROR;

    str = glGetString(name);

    if (str == 0) {
        Tcl_AppendResult(interp, "error: unknown name \"", Tcl_GetString(objv[1]), "\"", NULL); 
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, (char *)str, NULL); 

    return TCL_OK;
}
