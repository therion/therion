add_library(shp dbfopen.c safileio.c sbnsearch.c shpopen.c shptree.c shapefil.h)
target_include_directories(shp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(UNIX)
    find_library(M_LIB m)
    if(M_LIB)
      target_link_libraries(shp ${M_LIB})
    endif()
endif()
