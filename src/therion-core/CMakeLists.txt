add_custom_command(
    COMMAND
        tclsh ${CMAKE_CURRENT_SOURCE_DIR}/thcsdata.tcl ${PROJ_PREFIX}/share/proj
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/thcsdata.h
        ${CMAKE_CURRENT_BINARY_DIR}/thcsdata.cxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/thcsdata.tcl
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

# three.js viewer libraries embedded in HTML model export; packagers may point
# THREEJS_DIR to a system copy of three.js r147 with the same file layout
set(THREEJS_DIR ${CMAKE_SOURCE_DIR}/extern/threejs CACHE PATH
    "Directory with three.js r147 three.min.js, OrbitControls.js and LICENSE")
add_custom_command(
    COMMAND
        ${CMAKE_COMMAND} -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/thexpmodeljs.cxx
        -DFILES=thexpmodel_threejs=${THREEJS_DIR}/three.min.js,thexpmodel_orbitjs=${THREEJS_DIR}/OrbitControls.js,thexpmodel_threejs_license=${THREEJS_DIR}/LICENSE
        -P ${CMAKE_SOURCE_DIR}/cmake/EmbedFiles.cmake
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/thexpmodeljs.cxx
    DEPENDS
        ${CMAKE_SOURCE_DIR}/cmake/EmbedFiles.cmake
        ${THREEJS_DIR}/three.min.js
        ${THREEJS_DIR}/OrbitControls.js
        ${THREEJS_DIR}/LICENSE
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

add_library(therion-core STATIC
    ${THERION_SOURCES} 
    ${CMAKE_CURRENT_BINARY_DIR}/thcsdata.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/thexpmodeljs.cxx
)

target_include_directories(therion-core PUBLIC ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(therion-core SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/extern/stl_reader)
target_link_libraries(therion-core PUBLIC
    therion-chencdata
    therion-lang
    therion-mpost
    therion-tex
    proj-interface
    shp-interface
    poly2tri
    common-utils
    QuickHull
    fmt::fmt
    ImageMagick::Magick++
    $<$<PLATFORM_ID:Android>:log>
)
target_compile_definitions(therion-core PUBLIC "TH${THPLATFORM}" $<$<BOOL:${ENABLE_THDEBUG}>:THDEBUG>)
add_dependencies(therion-core
    thversion
)
