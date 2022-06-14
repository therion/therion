set(TEX_SOURCES gentex.pl therion.tex)

set(GENERATED_SOURCES
    ${CMAKE_BINARY_DIR}/thtex.h
    ${CMAKE_BINARY_DIR}/thtex.cxx)

therion_copy_files(${TEX_SOURCES})
therion_make_files_lists(TEX ${TEX_SOURCES})

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl gentex.pl
                   DEPENDS ${TEX_BIN}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(generate_tex DEPENDS ${GENERATED_SOURCES})
