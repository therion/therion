file(GLOB THCHENCDATA_TXTS *.TXT)
file(COPY ${THCHENCDATA_TXTS} generate.pl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(GENERATED_SOURCES
    ${CMAKE_BINARY_DIR}/thchencdata.h
    ${CMAKE_BINARY_DIR}/thchencdata.cxx
)

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl 
                   ARGS generate.pl
                   DEPENDS generate.pl ${THCHENCDATA_TXTS}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(generate_thchencdata DEPENDS ${GENERATED_SOURCES})
