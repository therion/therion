set(LOCH_HEADERS
lxAboutDlg.h  lxFNT10x20_bdf.h  lxGLC.h  lxImgIO.h  lxOptDlg.h  lxRender.h  lxR2P.h    lxSScene.h  lxSView.h  lxWX.h
lxData.h      lxFNTFreeSans_ttf.h  lxFNT6x13_bdf.h   lxGUI.h  lxLRUD.h   lxOGLFT.h  lxPres.h    lxR2D.h     lxSetup.h  lxSTree.h   lxTR.h
)
set(LOCH_SOURCES
lxAboutDlg.cxx  lxGUI.cxx    lxLRUD.cxx  lxOGLFT.cxx   lxPres.cxx    lxSetup.cxx   lxSTree.cxx  lxWX.cxx
lxData.cxx      lxGLC.cxx   lxImgIO.cxx  lxOptDlg.cxx  lxRender.cxx  lxSScene.cxx  lxSView.cxx  lxTR.c
)

set(LOCH_DEFINITIONS _GNU_SOURCE LOCH "LX${THPLATFORM}" GL_SILENCE_DEPRECATION TBB_SUPPRESS_DEPRECATED_MESSAGES=1)

# library common with therion
add_library(loch-common STATIC icase.h lxFile.h lxFile.cxx lxMath.h lxMath.cxx img.h img.c)
target_compile_definitions(loch-common PRIVATE ${LOCH_DEFINITIONS})

# loch resource file
if (WIN32)
    add_custom_command(COMMAND ${CMAKE_RC_COMPILER} -i ${CMAKE_CURRENT_SOURCE_DIR}/loch.rc "-I$<JOIN:${wxWidgets_INCLUDE_DIRS},;-I>" -J rc -o ${CMAKE_CURRENT_BINARY_DIR}/loch.res -O coff
                        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/loch.res
                        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/loch.rc ${CMAKE_CURRENT_SOURCE_DIR}/loch.ico
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMAND_EXPAND_LISTS)
    add_custom_target(generate_resource DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/loch.res)
endif()

# loch executable
add_executable(loch WIN32 ${LOCH_HEADERS} ${LOCH_SOURCES} $<$<BOOL:${WIN32}>:lxR2D.c> $<$<STREQUAL:${THPLATFORM},LINUX>:lxR2P.c>)
# we need to filter out include directories from wxWidgets cxx flags list, because we have to set them only as system headers to ignore warnings
set(CXX_FLAGS_WITHOUT_INCLUDE_DIRS ${wxWidgets_CXX_FLAGS_LIST})
list(FILTER CXX_FLAGS_WITHOUT_INCLUDE_DIRS EXCLUDE REGEX "-I/")
target_compile_options(loch PRIVATE ${CXX_FLAGS_WITHOUT_INCLUDE_DIRS})
target_compile_definitions(loch PRIVATE ${LOCH_DEFINITIONS})
target_include_directories(loch PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(loch SYSTEM PRIVATE ${FREETYPE_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS} ${VTK_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
target_link_libraries(loch PRIVATE
    loch-common
    ${wxWidgets_LIBRARIES}
    ${VTK_LIBRARIES} 
    ${FREETYPE_LIBRARIES}
    OpenGL::GL
    OpenGL::GLU
    ${X11_LIBRARIES}
    $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_BINARY_DIR}/loch.res>
)
if (WIN32)
    add_dependencies(loch generate_resource)
endif()

install(TARGETS loch RUNTIME DESTINATION bin)

add_subdirectory(help)
