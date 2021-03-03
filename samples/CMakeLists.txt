therion_copy_files(samples.tcl)

add_custom_target(
  samples
  COMMAND tclsh samples.tcl ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/samples --emulator=${CMAKE_CROSSCOMPILING_EMULATOR}
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/thbook/version.tex
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target thbook
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  USES_TERMINAL)

 # subdirectories will add dependencies to this target
add_custom_target(samples-cmake)

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
