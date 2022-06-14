# set default build type if missing
if(NOT CMAKE_BUILD_TYPE)
    set(DEFAULT_BUILD_TYPE Release)
    message(STATUS "Build type not specified, using ${DEFAULT_BUILD_TYPE}.")
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Choose the type of build." FORCE)
endif()

# convenient choice between standard CMake build types
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
