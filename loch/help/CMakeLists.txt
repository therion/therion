file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}
  DESTINATION ${CMAKE_BINARY_DIR}/loch
  PATTERN "Makefile" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE)

find_program(HHC_COMMAND hhc)

function(add_help DIR)
  set(FULL_PATH ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
  set(INPUT_FILES ${FULL_PATH}/loch.hhp ${FULL_PATH}/loch.hhc
                  ${FULL_PATH}/loch.hhk ${FULL_PATH}/loch.htm)

  if(HHC_COMMAND)
    add_custom_command(
      OUTPUT ${FULL_PATH}/loch.chm
      COMMAND ${HHC_COMMAND} loch.hhp
      DEPENDS ${INPUT_FILES}
      WORKING_DIRECTORY ${FULL_PATH})
  endif()

  add_custom_command(
    OUTPUT ${FULL_PATH}/loch.htb
    COMMAND ${CMAKE_COMMAND} -E tar cfv loch.htb --format=zip loch.hhp loch.hhc
            loch.hhk loch.htm
    DEPENDS ${INPUT_FILES}
    WORKING_DIRECTORY ${FULL_PATH})

  add_custom_target(hlp_${DIR} ALL DEPENDS ${FULL_PATH}/loch.chm
                                           ${FULL_PATH}/loch.htb)
endfunction()

add_help("en")
add_help("sk")
