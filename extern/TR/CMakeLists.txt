# Tiled Rendering library
add_library(TR STATIC tr.h tr.c)
target_include_directories(TR PUBLIC ${CMAKE_SOURCE_DIR}/extern)
target_link_libraries(TR PUBLIC OpenGL::GL OpenGL::GLU 
                         PRIVATE disable-warnings)
