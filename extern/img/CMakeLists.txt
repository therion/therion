add_library(img img.h img.c)
target_compile_definitions(img PUBLIC IMG_API_VERSION=1)
target_include_directories(img SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(img PRIVATE disable-warnings)
