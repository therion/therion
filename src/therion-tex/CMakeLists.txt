add_custom_command(
    COMMAND perl gentex.pl ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS gentex.pl therion.tex
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/thtex.cxx
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(therion-tex STATIC ${CMAKE_CURRENT_BINARY_DIR}/thtex.cxx)
target_include_directories(therion-tex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(therion-tex PUBLIC enable-warnings)
