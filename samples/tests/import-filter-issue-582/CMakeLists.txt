get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

therion_copy_files(thconfig data.3d)

add_custom_command(
    DEPENDS therion ./thconfig ./data.3d
    OUTPUT database.csv
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    _samples-verify-ref-${SAMPLE_NAME}
    DEPENDS database.csv
    COMMAND diff -ub database.csv ${CMAKE_CURRENT_SOURCE_DIR}/database.csv.ref
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_dependencies(samples-verify-ref _samples-verify-ref-${SAMPLE_NAME})
