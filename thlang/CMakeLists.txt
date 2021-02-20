set(THLANG_SOURCES
    process.pl
    texts.txt
    thlang_stats.pl)

set(GENERATED_SOURCES
    ${CMAKE_BINARY_DIR}/thlangdata.h
    ${CMAKE_BINARY_DIR}/thlangdatafields.h)

therion_copy_files(${THLANG_SOURCES})
therion_make_files_lists(THLANG ${THLANG_SOURCES})

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl process.pl
                   DEPENDS ${THLANG_BIN}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(generate_thlang DEPENDS ${GENERATED_SOURCES})

add_custom_target(update perl process.pl update
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
