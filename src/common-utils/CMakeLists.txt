# Library with common utilities.
add_library(common-utils STATIC
    icase.cxx
    lxFile.cxx
    lxMath.cxx
)
target_link_libraries(common-utils PUBLIC img enable-warnings code-coverage)
target_include_directories(common-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
