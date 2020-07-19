file(GLOB MPOST_MPS ${CMAKE_CURRENT_SOURCE_DIR}/*.mp)
file(COPY ${MPOST_MPS} genmpost.pl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(GENERATED_SOURCES 
    ${CMAKE_BINARY_DIR}/thmpost.h 
    ${CMAKE_BINARY_DIR}/thmpost.cxx
    ${CMAKE_BINARY_DIR}/thsymbolsets.h
    ${CMAKE_BINARY_DIR}/thsymbolsets.cxx
)

set_source_files_properties(
    ${GENERATED_SOURCES}
    PROPERTIES GENERATED TRUE 
)

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl
                   ARGS genmpost.pl
                   DEPENDS ${MPOST_MPS}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(generate_mpost DEPENDS ${GENERATED_SOURCES})
