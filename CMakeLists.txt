cmake_minimum_required(VERSION 3.10)
include(cmake/PreventInSourceBuilds.cmake)

project(Therion)
set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include(cmake/BuildType.cmake)
include(cmake/Dependencies.cmake)
include(cmake/TherionSources.cmake)
include(cmake/ECMEnableSanitizers.cmake)

# strip binaries in release build
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")

set(ECM_ENABLE_SANITIZERS "" CACHE STRING 
    "Enable runtime sanitizers, available options: address,memory,thread,leak,undefined." )
set(ENABLE_THDEBUG OFF CACHE BOOL "Print debug information.")

option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy." OFF)
if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXECUTABLE clang-tidy)
    if (CLANG_TIDY_EXECUTABLE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};--quiet")
    endif()
endif()

option(ENABLE_CCACHE "Enable compiler cache." OFF)
if (ENABLE_CCACHE)
    find_program(CCACHE_EXECUTABLE ccache)
    mark_as_advanced(CCACHE_EXECUTABLE)
    if(CCACHE_EXECUTABLE)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_EXECUTABLE})
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_EXECUTABLE})
    endif()
endif()

# determine platform
if(UNIX AND NOT APPLE)
    set(THPLATFORM LINUX)
elseif(APPLE)
    set(THPLATFORM MACOSX)
else()
    set(THPLATFORM WIN32)
endif()

# tell CMake which files must be generated now, doing so in subfolders is too late
set_source_files_properties(
    ${CMAKE_BINARY_DIR}/thchencdata.h
    ${CMAKE_BINARY_DIR}/thchencdata.cxx
    ${CMAKE_BINARY_DIR}/thlangdata.h
    ${CMAKE_BINARY_DIR}/thlangdatafields.h
    ${CMAKE_BINARY_DIR}/thmpost.h 
    ${CMAKE_BINARY_DIR}/thmpost.cxx
    ${CMAKE_BINARY_DIR}/thsymbolsets.h
    ${CMAKE_BINARY_DIR}/thsymbolsets.cxx
    ${CMAKE_BINARY_DIR}/thversion.h
    ${CMAKE_BINARY_DIR}/thcsdata.h
    ${CMAKE_BINARY_DIR}/thcsdata.cxx
    ${CMAKE_BINARY_DIR}/thsymbolsetlist.h
    ${CMAKE_BINARY_DIR}/thtex.h
    ${CMAKE_BINARY_DIR}/thtex.cxx
    PROPERTIES GENERATED TRUE 
)

# copy given files to the build dir
function(therion_copy_files)
    foreach(FILE_NAME ${ARGV})
        configure_file(${FILE_NAME} ${FILE_NAME} COPYONLY)
    endforeach()
endfunction()

# make lists of names with source and binary dir prefixes
macro(therion_make_files_lists LIST_NAME)
    foreach(FILE_NAME ${ARGN})
        list(APPEND ${LIST_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME})
        list(APPEND ${LIST_NAME}_BIN ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME})
    endforeach()
endmacro()

# copy files needed by some build steps
therion_copy_files(thcsdata.tcl thsymbolsetfont.txt .clang-tidy)

# generate thcsdata sources
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/thcsdata.h
                          ${CMAKE_BINARY_DIR}/thcsdata.cxx
                   COMMAND tclsh thcsdata.tcl ${PROJ_PREFIX}/share/proj
                   DEPENDS ${CMAKE_BINARY_DIR}/thcsdata.tcl
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(generate_thcsdata DEPENDS
    ${CMAKE_BINARY_DIR}/thcsdata.h
    ${CMAKE_BINARY_DIR}/thcsdata.cxx
)

# generate thsymbolsetlist sources
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/thsymbolsetlist.h
    COMMAND perl
    ARGS ${CMAKE_SOURCE_DIR}/thsymbolsetlist.pl
    DEPENDS ${CMAKE_SOURCE_DIR}/thsymbolsetlist.pl 
            ${CMAKE_BINARY_DIR}/mpost/thTrans.mp
            ${CMAKE_BINARY_DIR}/thsymbolsetfont.txt
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(generate_thsymbolsetlist DEPENDS ${CMAKE_BINARY_DIR}/thsymbolsetlist.h)

# generate version files
execute_process(COMMAND python3 set_version.py ${CMAKE_BINARY_DIR}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# generate InnoSetup config file
if (WIN32)
    configure_file(innosetup.ini.in innosetup.ini)
endif()

# updates thlibrarydata.cxx
add_custom_target(library 
    ${CMAKE_COMMAND} -D THERION=$<TARGET_FILE:therion> -P cmake/Library.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# therion lib
add_library(therion-common STATIC ${THERION_HEADERS} ${THERION_SOURCES})
target_include_directories(therion-common BEFORE PUBLIC "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/extern")
target_link_libraries(therion-common PUBLIC PkgConfig::PROJ fmt::fmt shp poly2tri loch-common)
target_compile_definitions(therion-common PUBLIC PROJ_VER=${PROJ_MVER} "TH${THPLATFORM}" $<$<BOOL:${ENABLE_THDEBUG}>:THDEBUG>)
add_dependencies(therion-common 
    generate_thsymbolsetlist
    generate_thlang
    generate_tex
    generate_mpost
    generate_thchencdata
    generate_thcsdata
)

# therion executable
add_executable(therion therion-main.cxx)
target_link_libraries(therion PUBLIC therion-common)

install(TARGETS therion RUNTIME DESTINATION bin)

# unit tests
add_executable(utest utest-proj.cxx utest-icase.cxx utest-thdouble.cxx utest-main.cxx)
target_link_libraries(utest PUBLIC therion-common Catch2::Catch2)
enable_testing()
if (NOT WIN32)
    add_test(NAME utest COMMAND $<TARGET_FILE:utest> WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# add subdirectories to the build
add_subdirectory(extern/poly2tri)
add_subdirectory(mpost)
add_subdirectory(thchencdata)
add_subdirectory(tex)
add_subdirectory(thlang)
add_subdirectory(loch)
add_subdirectory(thbook)
add_subdirectory(xtherion)
add_subdirectory(samples)
