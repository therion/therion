add_custom_command(
    COMMAND
        perl genmpost.pl ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS 
        genmpost.pl
        thArea.mp
        therion.mp
        thLine.mp
        thPoint.mp
        thSpecial.mp
        thText.mp
        thTrans.mp
        uAUT.mp
        uSBE.mp
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/thmpost.cxx
        ${CMAKE_CURRENT_BINARY_DIR}/thsymbolsets.h
        ${CMAKE_CURRENT_BINARY_DIR}/thsymbolsets.cxx
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(therion-mpost STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/thmpost.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/thsymbolsets.cxx
)
target_include_directories(therion-mpost PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
target_link_libraries(therion-mpost PUBLIC enable-warnings)
add_dependencies(therion-mpost generate_thsymbolsetlist)
