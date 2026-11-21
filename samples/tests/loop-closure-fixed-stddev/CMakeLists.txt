get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

therion_copy_files(thconfig therion.ini)

add_custom_command(
    OUTPUT database.sql
    DEPENDS therion thconfig therion.ini
    COMMAND ${CMAKE_COMMAND} -E env "THERION=${CMAKE_CURRENT_BINARY_DIR}"
            ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion>
            --reproducible-output thconfig
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(_samples-verify-ref-${SAMPLE_NAME}--database.sql
    DEPENDS database.sql
    COMMAND diff -ub database.sql ${CMAKE_CURRENT_SOURCE_DIR}/database.sql.ref
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_dependencies(samples-verify-ref
    _samples-verify-ref-${SAMPLE_NAME}--database.sql)
