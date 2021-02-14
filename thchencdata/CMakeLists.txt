set(THCHENCDATA_SOURCES
    ASCII.TXT
    CP1250.TXT
    CP1251.TXT
    CP1252.TXT
    CP1253.TXT
    8859-1.TXT
    8859-2.TXT
    8859-5.TXT
    8859-7.TXT
    generate.pl)
    
set(GENERATED_SOURCES
    ${CMAKE_BINARY_DIR}/thchencdata.h
    ${CMAKE_BINARY_DIR}/thchencdata.cxx)

therion_copy_files(${THCHENCDATA_SOURCES})
therion_make_files_lists(${THCHENCDATA_SOURCES})

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl generate.pl
                   DEPENDS ${THCHENCDATA_BIN}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(generate_thchencdata DEPENDS ${GENERATED_SOURCES})
