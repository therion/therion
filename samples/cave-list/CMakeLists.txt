therion_copy_files(thconfig)

add_custom_command(
    OUTPUT caves.html
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-listing-caves DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/caves.html)
add_dependencies(samples-cmake samples-listing-caves)
