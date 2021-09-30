# get the relative path to generated samples to ensure reproducible builds,
# as the path is written to index.tex and also in the metadata (PTEX.FileName)
# of PDF images included into thbook.pdf
file(RELATIVE_PATH SAMPLES_REL_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
string(REGEX REPLACE "/$" "" SAMPLES_REL_DIR ${SAMPLES_REL_DIR})

# Build samples and add them to the thbook.
therion_copy_files(samples.tcl)
add_custom_target(
  samples
  COMMAND tclsh samples.tcl ${SAMPLES_REL_DIR} --generate-tex-only
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/thbook/version.tex
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target thbook
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/samples.tcl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(samples samples-build)

# build samples in html format, including the conversion of PDF to PNG images
add_custom_target(
  samples-html
  COMMAND tclsh samples.tcl ${SAMPLES_REL_DIR} --generate-html-images
  USES_TERMINAL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/samples.tcl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(samples-html therion)

 # subdirectories will add dependencies to these targets
add_custom_target(samples-build)
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
    add_custom_target(_samples-${SAMPLE_NAME}
                      COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output ${THCONFIG}
                      DEPENDS ${SAMPLE_DEPENDS_OTHER}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND_EXPAND_LISTS)
    add_dependencies(_samples-${SAMPLE_NAME} therion)

    # CRC verification: Copy .crc files from source dir and then build the sample with verification.
    add_custom_target(_samples-verify-crc-${SAMPLE_NAME}
                      ${CMAKE_COMMAND} -E copy "$<JOIN:${OUTPUTS_SRC},.crc;>.crc" ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output --verify-output-crc ${THCONFIG}
                      DEPENDS ${SAMPLE_DEPENDS_OTHER}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND_EXPAND_LISTS)
    add_dependencies(_samples-verify-crc-${SAMPLE_NAME} therion)

    # CRC generation: Build sample with checksum generation and copy resulting .crc files to the source dir.
    add_custom_target(_samples-generate-crc-${SAMPLE_NAME}
                      ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output --generate-output-crc ${THCONFIG}
                      COMMAND ${CMAKE_COMMAND} -E copy "$<JOIN:${OUTPUTS_BIN},.crc;>.crc" ${CMAKE_CURRENT_SOURCE_DIR}
                      DEPENDS ${SAMPLE_DEPENDS_OTHER}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMAND_EXPAND_LISTS)
    add_dependencies(_samples-generate-crc-${SAMPLE_NAME} therion)

    # Make targets with all the samples together.
    add_dependencies(samples-build _samples-${SAMPLE_NAME})
    add_dependencies(samples-verify-crc _samples-verify-crc-${SAMPLE_NAME})
    add_dependencies(samples-generate-crc _samples-generate-crc-${SAMPLE_NAME})
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
