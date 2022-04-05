function(process_locale DIR)
  set(FULL_PATH ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
  if (APPLE)
    string(APPEND FULL_PATH ".lproj")
  endif()
  file(MAKE_DIRECTORY ${FULL_PATH})
  add_custom_command(
      OUTPUT  ${FULL_PATH}/loch.mo
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${FULL_PATH}/loch.mo ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/loch.po
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/loch.po
  )
  add_custom_target(locale_${DIR} DEPENDS ${FULL_PATH}/loch.mo)
  add_dependencies(loch locale_${DIR})

  # on macOS install locale files as part of an application bundle
  if (APPLE)
    target_sources(loch ${FULL_PATH})
    set_source_files_properties(${FULL_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  else()
    install(FILES ${FULL_PATH}/loch.mo DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${DIR}/LC_MESSAGES COMPONENT loch-runtime)
  endif()
endfunction()

process_locale("bg")
process_locale("cs")
process_locale("de")
process_locale("en_GB")
process_locale("es")
process_locale("fr")
process_locale("it")
process_locale("ru")
process_locale("sk")
process_locale("sl")
