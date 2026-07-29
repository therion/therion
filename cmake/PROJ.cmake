#
# Set how to find dependency PROJ.
# Option ENABLE_PROJ_CMAKE:
# * ON - use CMake to find PROJ's exported targets
# * OFF - find PROJ with pkg-config
#
option(ENABLE_PROJ_CMAKE "Use CMake to find PROJ library." OFF)

add_library(proj-interface INTERFACE)

if (ENABLE_PROJ_CMAKE)
    find_package(PROJ REQUIRED)  # the version check can't be here if major versions differ
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
if (PROJ_VERSION VERSION_LESS 8.0.0)
    message(FATAL_ERROR "Unsupported proj version: ${PROJ_VERSION}")
endif()
target_compile_definitions(proj-interface INTERFACE PROJ_VER=${PROJ_MVER})
