therion_copy_files(thconfig create.th create.th2 create.svx create.3d)

add_custom_command(
    OUTPUT create.3d
    COMMAND ${SURVEX_EXECUTABLE} create.svx
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/create.svx
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT create.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
            ${CMAKE_CURRENT_BINARY_DIR}/create.th
            ${CMAKE_CURRENT_BINARY_DIR}/create.th2
            ${CMAKE_CURRENT_BINARY_DIR}/create.3d
            ${CMAKE_CURRENT_BINARY_DIR}/../thconfig.layout
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-survex-create DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/create.pdf)
add_dependencies(samples-cmake samples-survex-create)
