function(process_locale DIR)
  set(FULL_PATH ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
  file(MAKE_DIRECTORY ${FULL_PATH})
  add_custom_command(
      OUTPUT  ${FULL_PATH}/loch.mo
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${FULL_PATH}/loch.mo ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/loch.po
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/loch.po
  )
  add_custom_target(locale_${DIR} DEPENDS ${FULL_PATH}/loch.mo)
  add_dependencies(loch locale_${DIR})

  # on macOS install locale files are part of an application bundle
  if (NOT APPLE)
    install(FILES ${FULL_PATH}/loch.mo DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${DIR}/LC_MESSAGES COMPONENT loch-runtime)
  endif()
endfunction()

foreach(LOCALE ${LOCH_LOCALES})
  process_locale(${LOCALE})
endforeach()
