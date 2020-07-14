file(COPY gentex.pl therion.tex DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(GENERATED_SOURCES
    ${CMAKE_BINARY_DIR}/thtex.h
    ${CMAKE_BINARY_DIR}/thtex.cxx
)

set_source_files_properties(
    ${GENERATED_SOURCES}
    PROPERTIES GENERATED TRUE 
)

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl
                   ARGS gentex.pl
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/therion.tex
                           ${CMAKE_CURRENT_BINARY_DIR}/gentex.pl
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(generate_tex DEPENDS ${GENERATED_SOURCES})
