cmake_minimum_required(VERSION 3.10)

project(Therion)
set(CMAKE_CXX_STANDARD 14)

include(cmake/Dependencies.cmake)
include(cmake/TherionSources.cmake)

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
    ${CMAKE_BINARY_DIR}/thlangdata.cxx
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

set(CROSS "")

# copy files needed by some build steps
file(COPY thcsdata.tcl thsymbolsetfont.txt DESTINATION ${CMAKE_BINARY_DIR})

# generate thcsdata sources
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/thcsdata.h
                          ${CMAKE_BINARY_DIR}/thcsdata.cxx
                   COMMAND tclsh 
                   ARGS thcsdata.tcl ${CROSS}
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

# therion lib
add_library(therion-common STATIC ${THERION_HEADERS} ${THERION_SOURCES})
target_include_directories(therion-common BEFORE PUBLIC "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/extern" ${PROJ_INCLUDE_DIRS})
target_link_libraries(therion-common PUBLIC ${PROJ4_LIBRARIES} loch-common)
target_compile_definitions(therion-common PUBLIC PROJ_VER=${PROJ_MVER})
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

# unit tests
add_executable(utest utest-proj.cxx utest-main.cxx)
target_link_libraries(utest PUBLIC therion-common)
enable_testing()
add_test(NAME utest COMMAND ${CMAKE_BINARY_DIR}/utest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# add subdirectories to the build
add_subdirectory(mpost)
add_subdirectory(thchencdata)
add_subdirectory(tex)
add_subdirectory(thlang)
add_subdirectory(loch)
add_subdirectory(thbook)
add_subdirectory(xtherion)
