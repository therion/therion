#
# Set how to find dependency Catch2.
# Option USE_BUNDLED_CATCH2:
# * ON - use Catch2 header from extern subdirectory
# * OFF - use Catch2 installed in the system
#
option(USE_BUNDLED_CATCH2 "Use bundled version of Catch2." ON)

add_library(catch2-interface INTERFACE)

if (USE_BUNDLED_CATCH2)
    target_include_directories(catch2-interface INTERFACE ${CMAKE_SOURCE_DIR}/extern)
else()
    find_package(Catch2 REQUIRED)
    target_link_libraries(catch2-interface INTERFACE Catch2::Catch2)
endif()
