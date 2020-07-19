file(COPY process.pl texts.txt thlang_stats.pl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(GENERATED_SOURCES
    ${CMAKE_BINARY_DIR}/thlangdata.h
    ${CMAKE_BINARY_DIR}/thlangdata.cxx
)

set_source_files_properties(
    ${GENERATED_SOURCES}
    PROPERTIES GENERATED TRUE 
)

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl 
                   ARGS process.pl
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/texts.txt
                           ${CMAKE_CURRENT_BINARY_DIR}/process.pl
                           ${CMAKE_CURRENT_BINARY_DIR}/thlang_stats.pl
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/thlang
)

add_custom_target(generate_thlang DEPENDS ${GENERATED_SOURCES})

add_custom_target(update perl process.pl update
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
