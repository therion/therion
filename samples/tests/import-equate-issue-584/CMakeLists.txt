get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

therion_copy_files(thconfig data.3d)

set(OUTFILES database.csv map-p.xvi)

add_custom_command(
    DEPENDS therion ./thconfig ./data.3d
    OUTPUT ${OUTFILES}
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

foreach(OUTFILE IN LISTS OUTFILES)
    set(TARGET_NAME _samples-verify-ref-${SAMPLE_NAME}--${OUTFILE})

    add_custom_target(
        ${TARGET_NAME}
        DEPENDS ${OUTFILE}
        COMMAND diff -ub ${OUTFILE} ${CMAKE_CURRENT_SOURCE_DIR}/${OUTFILE}.ref
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_dependencies(samples-verify-ref ${TARGET_NAME})
endforeach()
