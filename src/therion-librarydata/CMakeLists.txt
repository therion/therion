add_library(therion-librarydata STATIC thlibrarydata.cxx)
target_include_directories(therion-librarydata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(therion-librarydata PUBLIC therion-core)

# update thlibrarydata.cxx
add_custom_target(library
    $<TARGET_FILE:therion> --print-library-src ${CMAKE_CURRENT_SOURCE_DIR}/thlibrarydata.thcfg
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
