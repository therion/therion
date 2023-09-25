#
# Set how to find dependency shapelib.
# Option USE_BUNDLED_SHAPELIB:
# * ON - use shapelib from extern subdirectory
# * OFF - use shapelib installed in the system
#
option(USE_BUNDLED_SHAPELIB "Use bundled version of shapelib." ON)

add_library(shp-interface INTERFACE)

if (USE_BUNDLED_SHAPELIB)
    add_subdirectory(extern/shapelib)
    target_link_libraries(shp-interface INTERFACE shp)
else()
    if (NOT PKG_CONFIG_FOUND)
        find_package(PkgConfig REQUIRED)
    endif()
    pkg_check_modules(SHAPELIB REQUIRED IMPORTED_TARGET shapelib)
    target_link_libraries(shp-interface INTERFACE PkgConfig::SHAPELIB)
endif()
