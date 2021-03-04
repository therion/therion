therion_copy_files(thconfig.layout cave.th cave.th2)

foreach(I RANGE 1 4)
    therion_copy_files(thconfig.${I})

    add_custom_command(
        OUTPUT map${I}.pdf
        COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.${I}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.${I}
                ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
                ${CMAKE_CURRENT_BINARY_DIR}/cave.th
                ${CMAKE_CURRENT_BINARY_DIR}/cave.th2
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(samples-map-offset-${I} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/map${I}.pdf)
    add_dependencies(samples-cmake samples-map-offset-${I})
endforeach()
