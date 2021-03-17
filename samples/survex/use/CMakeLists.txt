therion_copy_files(thconfig use.th use-in.th2 use-out.th2 use.svx use.3d)

add_custom_command(
    OUTPUT use.3d
    COMMAND ${SURVEX_EXECUTABLE} use.svx
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/use.svx
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT use.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
            ${CMAKE_CURRENT_BINARY_DIR}/use.th
            ${CMAKE_CURRENT_BINARY_DIR}/use-in.th2
            ${CMAKE_CURRENT_BINARY_DIR}/use-out.th2
            ${CMAKE_CURRENT_BINARY_DIR}/use.3d
            ${CMAKE_CURRENT_BINARY_DIR}/../thconfig.layout
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-survex-use DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/use.pdf)
add_dependencies(samples-cmake samples-survex-use)
