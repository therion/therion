set(LOCH_HEADERS
lxAboutDlg.h  lxFNT10x20_bdf.h  lxGLC.h  lxImgIO.h  lxOptDlg.h  lxRender.h  lxR2P.h    lxSScene.h  lxSView.h  lxWX.h
lxData.h      lxFNTFreeSans_ttf.h  lxFNT6x13_bdf.h   lxGUI.h  lxLRUD.h   lxOGLFT.h  lxPres.h    lxR2D.h     lxSetup.h  lxSTree.h   lxTR.h
)
set(LOCH_SOURCES
lxAboutDlg.cxx  lxGUI.cxx    lxLRUD.cxx  lxOGLFT.cxx   lxPres.cxx    lxSetup.cxx   lxSTree.cxx  lxWX.cxx
lxData.cxx      lxGLC.cxx   lxImgIO.cxx  lxOptDlg.cxx  lxRender.cxx  lxSScene.cxx  lxSView.cxx  lxTR.c
)

set(LOCH_DEFINITIONS _GNU_SOURCE LOCH "LX${THPLATFORM}")
set(IS_WINDOWS FALSE)
if (${THPLATFORM} STREQUAL WIN32)
    set(IS_WINDOWS TRUE)
endif()

# library common with therion
add_library(loch-common STATIC lxFile.h lxFile.cxx lxMath.h lxMath.cxx img.h img.c $<$<BOOL:${IS_WINDOWS}>:getline.h getline.c>)
target_compile_definitions(loch-common PRIVATE ${LOCH_DEFINITIONS})

# loch resource file
if (${IS_WINDOWS})
    add_custom_command(COMMAND ${CMAKE_RC_COMPILER} -i ${CMAKE_CURRENT_SOURCE_DIR}/loch.rc "-I$<JOIN:${wxWidgets_INCLUDE_DIRS},;-I>" -J rc -o ${CMAKE_CURRENT_BINARY_DIR}/loch.res -O coff
                        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/loch.res
                        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/loch.rc ${CMAKE_CURRENT_SOURCE_DIR}/loch.ico
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMAND_EXPAND_LISTS)
    add_custom_target(generate_resource DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/loch.res)
endif()

# loch executable
add_executable(loch WIN32 ${LOCH_HEADERS} ${LOCH_SOURCES} $<IF:$<BOOL:${IS_WINDOWS}>,lxR2D.c,lxR2P.c>)
target_compile_options(loch PRIVATE ${wxWidgets_CXX_FLAGS_LIST})
target_compile_definitions(loch PRIVATE ${LOCH_DEFINITIONS})
target_include_directories(loch PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${FREETYPE_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS} ${VTK_INCLUDE_DIRS})
target_link_libraries(loch PRIVATE
    loch-common
    ${wxWidgets_LIBRARIES}
    ${VTK_LIBRARIES} 
    ${FREETYPE_LIBRARIES}
    PNG::PNG
    JPEG::JPEG
    OpenGL::GL
    OpenGL::GLU
    ${X11_LIBRARIES}
    $<$<BOOL:${IS_WINDOWS}>:${CMAKE_CURRENT_BINARY_DIR}/loch.res>
)
if (${IS_WINDOWS})
    add_dependencies(loch generate_resource)
endif()

install(TARGETS loch RUNTIME DESTINATION bin)

add_subdirectory(help)
