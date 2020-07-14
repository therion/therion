set(LOCH_HEADERS
getline.h  lxAboutDlg.h  lxFile.h             lxFNT10x20_bdf.h  lxGLC.h  lxImgIO.h  lxMath.h   lxOptDlg.h  lxRender.h  lxR2P.h    lxSScene.h  lxSView.h  lxWX.h
img.h      lxData.h      lxFNTFreeSans_ttf.h  lxFNT6x13_bdf.h   lxGUI.h  lxLRUD.h   lxOGLFT.h  lxPres.h    lxR2D.h     lxSetup.h  lxSTree.h   lxTR.h
)
set(LOCH_SOURCES
lxAboutDlg.cxx  lxFile.cxx  lxGUI.cxx    lxLRUD.cxx  lxOGLFT.cxx   lxPres.cxx    lxSetup.cxx   lxSTree.cxx  lxWX.cxx
lxData.cxx      lxGLC.cxx   lxImgIO.cxx  lxMath.cxx  lxOptDlg.cxx  lxRender.cxx  lxSScene.cxx  lxSView.cxx
)

add_library(loch-common STATIC ${LOCH_HEADERS} ${LOCH_SOURCES} img.c lxTR.c
# loch/lxR2D.c loch/getline.c TODO WIN only
)
target_link_libraries(loch-common PUBLIC
    ${wxWidgets_LIBRARIES}
    ${VTK_LIBRARIES} 
    Freetype::Freetype
    PNG::PNG
    JPEG::JPEG
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${X11_LIBRARIES}
)
target_compile_definitions(loch-common PUBLIC LOCH LXLINUX)
target_include_directories(loch-common BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(loch lxR2P.c)
target_link_libraries(loch PUBLIC loch-common)
