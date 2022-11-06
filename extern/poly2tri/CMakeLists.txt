add_library(poly2tri 
    ${CMAKE_CURRENT_SOURCE_DIR}/common/shapes.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/shapes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/advancing_front.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/advancing_front.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/cdt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/cdt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/sweep_context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/sweep_context.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/sweep.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/sweep.h)

target_link_libraries(poly2tri PRIVATE disable-warnings)
set_target_properties(poly2tri PROPERTIES CXX_CLANG_TIDY "")
