cmake_minimum_required(VERSION 3.10)
include(cmake/PreventInSourceBuilds.cmake)

project(therion)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_THERION "Build therion executable." ON)
option(BUILD_LOCH "Build loch executable." ON)
option(BUILD_THBOOK "Build thbook.pdf." ON)
option(BUILD_XTHERION "Build xtherion." ON)

include(GNUInstallDirs)
include(cmake/BuildType.cmake)
include(cmake/Dependencies.cmake)
include(cmake/TherionSources.cmake)
include(cmake/ECMEnableSanitizers.cmake)
include(cmake/Warnings.cmake)

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

# check if there are artifacts from the legacy Makefile in the source dir,
# which could interfere with the CMake build
add_custom_target(legacy-make 
    ${CMAKE_COMMAND} -P cmake/DetectLegacyBuild.cmake
    COMMENT "Checking for legacy build"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# generate version files
add_custom_target(version python3 set_version.py ${CMAKE_BINARY_DIR}
                  COMMENT "Updating version"
                  BYPRODUCTS ${CMAKE_BINARY_DIR}/thversion.h
                             ${CMAKE_BINARY_DIR}/thbook/version.tex
                  DEPENDS legacy-make
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# generate InnoSetup config file
if (WIN32)
    configure_file(innosetup.ini.in innosetup.ini)
endif()

# set an installation path for ini files
if (IS_ABSOLUTE ${CMAKE_INSTALL_SYSCONFDIR})
  set(TH_PATH_INI "${CMAKE_INSTALL_SYSCONFDIR}")
else()
  set(TH_PATH_INI "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")
endif()

if (BUILD_THERION)
    add_subdirectory(extern/quickhull)

    # therion lib
    add_library(therion-common STATIC ${THERION_HEADERS} ${THERION_SOURCES})
    target_include_directories(therion-common BEFORE PUBLIC "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/extern")
    target_link_libraries(therion-common PUBLIC PkgConfig::PROJ fmt::fmt shp poly2tri common-utils QuickHull)
    target_compile_definitions(therion-common PUBLIC PROJ_VER=${PROJ_MVER} "TH${THPLATFORM}" $<$<BOOL:${ENABLE_THDEBUG}>:THDEBUG>)
    add_dependencies(therion-common 
        version
        generate_thsymbolsetlist
        generate_thlang
        generate_tex
        generate_mpost
        generate_thchencdata
        generate_thcsdata
    )

    # therion executable
    add_executable(therion therion-main.cxx)
    target_link_libraries(therion therion-common)

    # therion resource file
    if (WIN32)
        add_custom_command(COMMAND ${CMAKE_RC_COMPILER} -i ${CMAKE_CURRENT_SOURCE_DIR}/therion.rc -J rc -o ${CMAKE_CURRENT_BINARY_DIR}/therion.res -O coff
                            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/therion.res
                            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/therion.rc ${CMAKE_CURRENT_SOURCE_DIR}/therion.ico
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        add_custom_target(generate_resource_therion DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/therion.res)
        add_dependencies(therion generate_resource_therion)
        target_link_libraries(therion ${CMAKE_CURRENT_BINARY_DIR}/therion.res)
    endif()

    install(TARGETS therion RUNTIME COMPONENT th-runtime)
    install(FILES therion.ini TYPE SYSCONF RENAME therion.ini.new COMPONENT th-runtime)
    install(CODE
        "if (NOT EXISTS ${TH_PATH_INI}/therion.ini)
            file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/therion.ini DESTINATION ${TH_PATH_INI})
        endif()"
        COMPONENT th-runtime)

    # unit tests
    add_executable(utest utest-proj.cxx utest-icase.cxx utest-thdouble.cxx utest-main.cxx)
    target_link_libraries(utest PUBLIC therion-common Catch2::Catch2)
    enable_testing()
    if (NOT WIN32)
        add_test(NAME utest COMMAND $<TARGET_FILE:utest> WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    endif()

    # updates thlibrarydata.cxx
    add_custom_target(library 
        ${CMAKE_COMMAND} -D THERION=$<TARGET_FILE:therion> -P cmake/Library.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_subdirectory(extern/poly2tri)
    add_subdirectory(mpost)
    add_subdirectory(thchencdata)
    add_subdirectory(tex)
    add_subdirectory(thlang)
    add_subdirectory(samples)
endif()

add_subdirectory(loch)
add_subdirectory(thbook)
add_subdirectory(xtherion)

# deployment of DLL dependencies on Windows
if (BUILD_THERION AND BUILD_LOCH AND WIN32)
    set(DLLS_DIR ${CMAKE_BINARY_DIR}/dependencies)
    add_custom_target(deploy 
        ${CMAKE_COMMAND} -D THERION=$<TARGET_FILE:therion>
                         -D LOCH=$<TARGET_FILE:loch>
                         -D DLLS_DIR=${DLLS_DIR}
                         -P cmake/Deploy.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_target_properties(deploy PROPERTIES ADDITIONAL_CLEAN_FILES ${DLLS_DIR})
endif()
