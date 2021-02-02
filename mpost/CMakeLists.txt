set(MPOST_SOURCES
    thArea.mp
    therion.mp
    thLine.mp
    thPoint.mp
    thSpecial.mp
    thText.mp
    thTrans.mp
    uAUT.mp
    uSBE.mp
    genmpost.pl)
    
set(GENERATED_SOURCES 
    ${CMAKE_BINARY_DIR}/thmpost.h 
    ${CMAKE_BINARY_DIR}/thmpost.cxx
    ${CMAKE_BINARY_DIR}/thsymbolsets.h
    ${CMAKE_BINARY_DIR}/thsymbolsets.cxx)

therion_copy_files(${MPOST_SOURCES})
therion_make_files_lists(MPOST ${MPOST_SOURCES})

add_custom_command(OUTPUT ${GENERATED_SOURCES}
                   COMMAND perl genmpost.pl
                   DEPENDS ${MPOST_BIN}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(generate_mpost DEPENDS ${GENERATED_SOURCES})
