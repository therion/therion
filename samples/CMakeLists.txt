file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}
  DESTINATION ${CMAKE_BINARY_DIR}
  PATTERN "Makefile" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE)

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
