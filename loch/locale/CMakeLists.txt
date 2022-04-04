file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}
  DESTINATION ${CMAKE_BINARY_DIR}/loch
  PATTERN "*.mo" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE)

function(process_locale DIR)
  set(FULL_PATH ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
  add_custom_command(
      OUTPUT  ${FULL_PATH}/loch.mo
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${FULL_PATH}/loch.mo ${FULL_PATH}/loch.po
      DEPENDS ${FULL_PATH}/loch.po
  )
  add_custom_target(locale_${DIR} ALL DEPENDS ${FULL_PATH}/loch.mo)
  add_dependencies(loch locale_${DIR})

  # on macOS install locale files as part of an application bundle
  if (APPLE)
    target_sources(loch PRIVATE ${FULL_PATH}/loch.mo)
    set_source_files_properties(${FULL_PATH}/loch.mo PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${DIR}.lproj/LC_MESSAGES)
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
