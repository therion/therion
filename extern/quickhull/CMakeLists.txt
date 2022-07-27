add_library(QuickHull QuickHull.cpp)
target_link_libraries(QuickHull PRIVATE disable-warnings)
set_target_properties(QuickHull PROPERTIES CXX_CLANG_TIDY "")
