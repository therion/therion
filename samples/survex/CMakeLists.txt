find_program(SURVEX_EXECUTABLE cavern)
if (NOT SURVEX_EXECUTABLE)
    message("Survex not found, samples with survex will be skipped.")
    return()
endif()

therion_copy_files(thconfig.layout cave.svx)

add_custom_command(
    OUTPUT cave.3d
    COMMAND ${SURVEX_EXECUTABLE} cave.svx
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cave.svx
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

foreach(I RANGE 1 2)
    therion_copy_files(thconfig.${I} cave${I}.th)

    add_custom_command(
        OUTPUT cave${I}.pdf
        COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.${I}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.${I}
                ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
                ${CMAKE_CURRENT_BINARY_DIR}/cave${I}.th
                ${CMAKE_CURRENT_BINARY_DIR}/cave.3d
                samples-survex-create
                samples-survex-ignore
                samples-survex-use
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(samples-survex-${I} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cave${I}.pdf)
    add_dependencies(samples-cmake samples-survex-${I})
endforeach()

add_subdirectory(create)
add_subdirectory(ignore)
add_subdirectory(use)
