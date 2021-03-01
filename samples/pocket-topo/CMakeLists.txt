therion_copy_files(thconfig demo.th)
therion_make_files_lists(POCKET_TOPO_SAMPLES cave.3d cave.plt cave.lox cave.xvi)

add_custom_command(
    OUTPUT ${POCKET_TOPO_SAMPLES_BIN}
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig
            ${CMAKE_CURRENT_BINARY_DIR}/demo.th
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-pocket-topo DEPENDS ${POCKET_TOPO_SAMPLES_BIN})
add_dependencies(samples-cmake samples-pocket-topo)
