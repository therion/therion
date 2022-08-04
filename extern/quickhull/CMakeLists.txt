add_library(QuickHull QuickHull.cpp)
target_include_directories(QuickHull SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/extern)
target_link_libraries(QuickHull PRIVATE disable-warnings)
set_target_properties(QuickHull PROPERTIES CXX_CLANG_TIDY "")
