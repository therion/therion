# Tiled Rendering library
add_library(TR STATIC tr.h tr.c)
target_include_directories(TR PUBLIC ${CMAKE_SOURCE_DIR}/extern)
if (APPLE)
    # On macOS OpenGL headers are in a different include directory,
    # with this workaround we don't need to modify TR source code
    target_include_directories(TR PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/macOS)
endif()
target_link_libraries(TR PUBLIC OpenGL::GL OpenGL::GLU 
                         PRIVATE disable-warnings)
