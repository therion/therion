therion_make_files_lists(RABBIT rabbit-plan.pdf rabbit-xelev.pdf rabbit.lox)

add_custom_command(
    OUTPUT ${RABBIT_BIN}
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
            ${CMAKE_CURRENT_BINARY_DIR}/rabbit.th
            ${CMAKE_CURRENT_BINARY_DIR}/rabbit.th2
            ${CMAKE_CURRENT_BINARY_DIR}/surface.jpg
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-drawing-maps DEPENDS ${RABBIT_BIN})
add_dependencies(samples-cmake samples-drawing-maps)
