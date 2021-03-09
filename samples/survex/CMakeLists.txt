find_program(CAVERN_EXECUTABLE cavern)
if (NOT CAVERN_EXECUTABLE)
    message("Cavern not found, samples with survex will be skipped.")
    return()
endif()

function(therion_add_cavern NAME)
    therion_copy_files(${NAME}.svx)

    add_custom_command(OUTPUT ${NAME}.3d
                       COMMAND ${CAVERN_EXECUTABLE} ${NAME}.svx
                       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.svx
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

therion_add_cavern(cave)

foreach(I RANGE 1 2)
    therion_add_sample(survex-${I} thconfig.${I}
                       DEPENDS_SRC cave${I}.th  thconfig.layout cave.svx
                       DEPENDS_OTHER ${CMAKE_CURRENT_BINARY_DIR}/cave.3d
                                     ${CMAKE_CURRENT_BINARY_DIR}/create/create.3d
                                     ${CMAKE_CURRENT_BINARY_DIR}/ignore/ignore.3d
                                     ${CMAKE_CURRENT_BINARY_DIR}/use/use.3d
                       OUTPUT cave${I}.pdf)
endforeach()

add_subdirectory(create)
add_subdirectory(ignore)
add_subdirectory(use)
