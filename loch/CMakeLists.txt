if (NOT BUILD_THERION AND NOT BUILD_LOCH)
    return()
endif()

set(LOCH_DEFINITIONS _GNU_SOURCE LOCH "LX${THPLATFORM}" GL_SILENCE_DEPRECATION TBB_SUPPRESS_DEPRECATED_MESSAGES=1)

# extern library img
add_library(img img.h img.c)
target_compile_definitions(img PUBLIC IMG_API_VERSION=1)
target_link_libraries(img PRIVATE disable-warnings)

# library common with therion
add_library(common-utils STATIC icase.h lxFile.h lxFile.cxx lxMath.h lxMath.cxx)
target_compile_definitions(common-utils PRIVATE ${LOCH_DEFINITIONS})
target_link_libraries(common-utils PUBLIC img enable-warnings)

if (NOT BUILD_LOCH)
    return()
endif()

# loch resource file
if (WIN32)
    add_custom_command(COMMAND ${CMAKE_RC_COMPILER} -i ${CMAKE_CURRENT_SOURCE_DIR}/loch.rc "-I$<JOIN:${wxWidgets_INCLUDE_DIRS},;-I>" -J rc -o ${CMAKE_CURRENT_BINARY_DIR}/loch.res -O coff
                        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/loch.res
                        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/loch.rc ${CMAKE_CURRENT_SOURCE_DIR}/loch.ico
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        COMMAND_EXPAND_LISTS)
    add_custom_target(generate_resource DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/loch.res)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/LochSources.cmake)
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
    common-utils
    ${wxWidgets_LIBRARIES}
    ${VTK_LIBRARIES} 
    ${FREETYPE_LIBRARIES}
    OpenGL::GL
    OpenGL::GLU
    ${X11_LIBRARIES}
    $<$<BOOL:${WIN32}>:${CMAKE_CURRENT_BINARY_DIR}/loch.res>
)
add_dependencies(loch version)
if (WIN32)
    add_dependencies(loch generate_resource)
endif()

if (APPLE)
    set(ICONS "loch.app/Contents/Resources/loch.icns")
    target_sources(loch PRIVATE ${ICONS})
    set_target_properties(loch PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/loch.app/Contents/Info.plist"
    )
    set_source_files_properties(${ICONS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

install(TARGETS loch 
    RUNTIME 
        COMPONENT loch-runtime
    BUNDLE
        DESTINATION "/Applications"
        COMPONENT loch-runtime
)

add_subdirectory(help)
add_subdirectory(locale)
