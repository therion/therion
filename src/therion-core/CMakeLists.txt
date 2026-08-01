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

add_library(therion-core STATIC
    ${THERION_SOURCES} 
    ${CMAKE_CURRENT_BINARY_DIR}/thcsdata.cxx
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
