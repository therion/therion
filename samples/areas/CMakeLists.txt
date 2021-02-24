add_custom_command(
    OUTPUT types.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
            ${CMAKE_CURRENT_BINARY_DIR}/types.th2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-area-symbols DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/types.pdf)
add_dependencies(samples-cmake samples-area-symbols)
