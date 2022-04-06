set(LOCH_HEADERS
    lxAboutDlg.h
    lxFNT10x20_bdf.h
    lxGLC.h
    lxImgIO.h
    lxOptDlg.h
    lxRender.h
    lxR2P.h
    lxSScene.h
    lxSView.h
    lxWX.h
    lxData.h
    lxFNTFreeSans_ttf.h
    lxFNT6x13_bdf.h
    lxGUI.h
    lxLRUD.h
    lxOGLFT.h
    lxPres.h
    lxR2D.h
    lxSetup.h
    lxSTree.h
    lxSStats.h
    lxTR.h
)

set(LOCH_SOURCES
    lxAboutDlg.cxx
    lxGUI.cxx
    lxLRUD.cxx
    lxOGLFT.cxx
    lxPres.cxx
    lxSetup.cxx
    lxSTree.cxx
    lxWX.cxx
    lxData.cxx
    lxGLC.cxx
    lxImgIO.cxx
    lxOptDlg.cxx
    lxRender.cxx
    lxSScene.cxx
    lxSView.cxx
    lxSStats.cxx
    lxTR.c
)

set(LOCH_LOCALES bg cs de en_GB es fr it ru sk sl)
# we need to set the files as sources only on macOS
if (APPLE)
    foreach(LOCALE ${LOCH_LOCALES})
        set(LOCALE_FILE "${CMAKE_BINARY_DIR}/loch/locale/${LOCALE}/loch.mo")
        list(APPEND LOCH_SOURCES ${LOCALE_FILE})
        set_source_files_properties(${LOCALE_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources/${LOCALE}.lproj
            GENERATED TRUE)
    endforeach()
endif()
