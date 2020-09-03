if(${THPLATFORM} EQUAL "WIN32")
  set(XTHERION_EXECUTABLE "xtherion.tcl")
else()
  set(XTHERION_EXECUTABLE "xtherion")
endif()

file(GLOB TCL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.tcl)
file(COPY ${TCL_SOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY lang/process.pl lang/xtexts.txt DESTINATION lang/)

add_custom_command(
  OUTPUT msgxth.tcl
  COMMAND perl ARGS process.pl
  DEPENDS lang/xtexts.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lang)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/therion.tcl
  COMMAND ${CMAKE_BINARY_DIR}/therion ARGS --print-xtherion-src > therion.tcl
  DEPENDS therion
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${XTHERION_EXECUTABLE}
  COMMAND tclsh ARGS mkall.tcl ${THPLATFORM}
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/msgxth.tcl
          ${CMAKE_CURRENT_BINARY_DIR}/therion.tcl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(
  update-xtherion
  perl process.pl update
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lang)

add_custom_target(
  survex
  COMMAND tclsh mkall.tcl survex
  COMMAND ${CMAKE_COMMAND} tar -cvzf ../../../svxedit.tar.gz svxedit)

add_custom_target(xtherion-tcl ALL
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${XTHERION_EXECUTABLE})
