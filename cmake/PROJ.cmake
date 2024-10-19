#
# Set how to find dependency PROJ.
# Option ENABLE_PROJ_CMAKE:
# * ON - use CMake to find PROJ's exported targets
# * OFF - find PROJ with pkg-config
#
option(ENABLE_PROJ_CMAKE "Use CMake to find PROJ library." OFF)

add_library(proj-interface INTERFACE)

if (ENABLE_PROJ_CMAKE)
    find_package(PROJ REQUIRED)
    target_link_libraries(proj-interface INTERFACE PROJ::proj)
else()
    if (NOT PKG_CONFIG_FOUND)
        find_package(PkgConfig REQUIRED)
    endif()
    pkg_check_modules(PROJ REQUIRED IMPORTED_TARGET proj)
    target_link_libraries(proj-interface INTERFACE PkgConfig::PROJ)
endif()

string(FIND ${PROJ_VERSION} "." MVER_SEP)
string(SUBSTRING ${PROJ_VERSION} 0 ${MVER_SEP} PROJ_MVER)
set(PROJ_UNSUPPORTED 7.0.0)
if (PROJ_VERSION IN_LIST PROJ_UNSUPPORTED OR PROJ_VERSION VERSION_LESS 6.2.1) # check if proj version is unsupported
    message(FATAL_ERROR "Unsupported proj version: ${PROJ_VERSION}")
endif()
target_compile_definitions(proj-interface INTERFACE PROJ_VER=${PROJ_MVER})
