therion_copy_files(thconfig ignore.th ignore.th2 ignore.svx ignore.3d)

add_custom_command(
    OUTPUT ignore.3d
    COMMAND ${SURVEX_EXECUTABLE} ignore.svx
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ignore.svx
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT ignore.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
            ${CMAKE_CURRENT_BINARY_DIR}/ignore.th
            ${CMAKE_CURRENT_BINARY_DIR}/ignore.th2
            ${CMAKE_CURRENT_BINARY_DIR}/ignore.3d
            ${CMAKE_CURRENT_BINARY_DIR}/../thconfig.layout
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-survex-ignore DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ignore.pdf)
add_dependencies(samples-cmake samples-survex-ignore)
