therion_copy_files(samples.tcl)

# legacy samples target
add_custom_target(
  samples
  COMMAND tclsh samples.tcl ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/samples --emulator=${CMAKE_CROSSCOMPILING_EMULATOR}
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/thbook/version.tex
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target thbook
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  USES_TERMINAL)

 # subdirectories will add dependencies to these targets
add_custom_target(samples-cmake)
add_custom_target(samples-verify-crc)
add_custom_target(samples-generate-crc)

# Function for registration of therion samples to the build.
# Usage:
# therion_add_sample(<unique_name> <thconfig_file>
#                    DEPENDS_SRC <source_files_dependencies>
#                    DEPENDS_OTHER <other_dependencies>)
# DEPENDS_SRC - relative names of source files needed to build the sample
# DEPENDS_OTHER - absolute paths of dependency files or targets
function(therion_add_sample SAMPLE_NAME THCONFIG)
    # CMake will parse these optional arguments
    set(MULTI_ARGS OUTPUT DEPENDS_SRC DEPENDS_OTHER)
    cmake_parse_arguments(SAMPLE "" "" "${MULTI_ARGS}" ${ARGN})

    therion_copy_files(${THCONFIG} ${SAMPLE_DEPENDS_SRC}) # copy dependency files to the build dir
    therion_make_files_lists(OUTPUTS ${SAMPLE_OUTPUT}) # make helper lists of files in src and build dirs

    # Build samples with therion.
    add_custom_target(samples-cmake-${SAMPLE_NAME}
                      COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output ${THCONFIG}
                      DEPENDS ${SAMPLE_DEPENDS_OTHER}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND_EXPAND_LISTS)

    # CRC verification: Copy .crc files from source dir and then build the sample with verification.
    add_custom_target(samples-verify-crc-${SAMPLE_NAME}
                      ${CMAKE_COMMAND} -E copy "$<JOIN:${OUTPUTS_SRC},.crc;>.crc" ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output --verify-output-crc ${THCONFIG}
                      DEPENDS ${SAMPLE_DEPENDS_OTHER}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND_EXPAND_LISTS)

    # CRC generation: Build sample with checksum generation and copy resulting .crc files to the source dir.
    add_custom_target(samples-generate-crc-${SAMPLE_NAME}
                      ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output --generate-output-crc ${THCONFIG}
                      COMMAND ${CMAKE_COMMAND} -E copy "$<JOIN:${OUTPUTS_BIN},.crc;>.crc" ${CMAKE_CURRENT_SOURCE_DIR}
                      DEPENDS ${SAMPLE_DEPENDS_OTHER}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND_EXPAND_LISTS)

    # Make targets with all the samples together.
    add_dependencies(samples-cmake samples-cmake-${SAMPLE_NAME})
    add_dependencies(samples-verify-crc samples-verify-crc-${SAMPLE_NAME})
    add_dependencies(samples-generate-crc samples-generate-crc-${SAMPLE_NAME})
endfunction()

add_subdirectory(areas)
add_subdirectory(basics)
add_subdirectory(cave-list)
add_subdirectory(map-offset)
add_subdirectory(morphing/sample1)
add_subdirectory(morphing/sample2)
add_subdirectory(pocket-topo)
add_subdirectory(q-marks)
add_subdirectory(u-symbols)
add_subdirectory(survex)
add_subdirectory(xelevation)
