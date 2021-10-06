file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}
  DESTINATION ${CMAKE_BINARY_DIR}/loch
  PATTERN "Makefile" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE)

function(add_help DIR)
  set(FULL_PATH ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
  set(INPUT_FILES ${FULL_PATH}/loch.hhp ${FULL_PATH}/loch.hhc
                  ${FULL_PATH}/loch.hhk ${FULL_PATH}/loch.htm)

  add_custom_command(
    OUTPUT ${FULL_PATH}/loch.htb
    COMMAND ${CMAKE_COMMAND} -E tar cfv loch.htb --format=zip loch.hhp loch.hhc
            loch.hhk loch.htm
    DEPENDS ${INPUT_FILES}
    WORKING_DIRECTORY ${FULL_PATH})

  add_custom_target(hlp_${DIR} ALL DEPENDS ${FULL_PATH}/loch.htb)
  add_dependencies(loch hlp_${DIR})

  install(FILES ${FULL_PATH}/loch.htb DESTINATION share/doc/therion-viewer/help/${DIR} COMPONENT loch-docs)
endfunction()

add_help("en")
add_help("sk")
