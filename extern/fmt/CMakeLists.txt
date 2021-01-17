set(CMAKE_CXX_STANDARD 14)
set(FMT_HEADERS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/color.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/compile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/core.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/format.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/format-inl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/chrono.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/locale.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/os.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/ostream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/printf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt/ranges.h)

set(FMT_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os.cc)

add_library(fmt ${FMT_HEADERS} ${FMT_SOURCES})
add_library(fmt::fmt ALIAS fmt)
target_include_directories(fmt SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(fmt PROPERTIES CXX_CLANG_TIDY "")
