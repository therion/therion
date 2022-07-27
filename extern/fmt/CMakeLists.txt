set(FMT_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/args.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/color.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/compile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/core.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/format.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/format-inl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/chrono.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/os.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/ostream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/printf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/ranges.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/std.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/xchar.h)

set(FMT_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os.cc)

add_library(fmt ${FMT_HEADERS} ${FMT_SOURCES})
add_library(fmt::fmt ALIAS fmt)
target_link_libraries(fmt PRIVATE disable-warnings)
set_target_properties(fmt PROPERTIES CXX_CLANG_TIDY "")
