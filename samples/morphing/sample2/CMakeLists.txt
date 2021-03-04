therion_copy_files(cave.png cave.th ../thconfig.layout)

foreach(I RANGE 1 3)
    therion_copy_files(thconfig.${I} cave${I}.th2)

    add_custom_command(
        OUTPUT cave${I}.pdf
        COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.${I}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.${I}
                ${CMAKE_CURRENT_BINARY_DIR}/cave.png
                ${CMAKE_CURRENT_BINARY_DIR}/cave.th
                ${CMAKE_CURRENT_BINARY_DIR}/cave${I}.th2
                ${CMAKE_CURRENT_BINARY_DIR}/../thconfig.layout
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(samples-morphing-2-${I} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cave${I}.pdf)
    add_dependencies(samples-cmake samples-morphing-2-${I})
endforeach()
