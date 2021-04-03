if (NOT BUILD_THBOOK)
    return()
endif()

set(THBOOK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/thbook.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch00.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch01.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch02.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch03.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch04.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch05.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch06.tex
    ${CMAKE_CURRENT_SOURCE_DIR}/ch07.tex
    ${CMAKE_CURRENT_BINARY_DIR}/version.tex)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/etc)

set(THBOOK_FORMAT "0" CACHE STRING "thbook format selection, use values 0-3.")

find_program(PDFTEX NAMES pdftex pdftex.exe
    PATHS /home/runner/work/therion/therion-batteries/bin/win32/
          /home/runner/work/therion-batteries/therion-batteries/bin/win32/)
if (NOT PDFTEX)
    message(FATAL_ERROR "pdftex command not found")
endif()


set(PDFTEX_COMMAND
    ${CMAKE_COMMAND} -E env FORCE_SOURCE_DATE=1 TEXINPUTS="${CMAKE_CURRENT_BINARY_DIR}$<SEMICOLON>."
    ${CMAKE_CROSSCOMPILING_EMULATOR} ${PDFTEX}
    --output-dir=${CMAKE_CURRENT_BINARY_DIR} "\\\\def\\\\outputsize{${THBOOK_FORMAT}}\\\\input thbook.tex")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/thbook.pdf
  COMMAND ${PDFTEX_COMMAND}
  COMMAND ${PDFTEX_COMMAND}
  COMMAND ${PDFTEX_COMMAND}
  DEPENDS ${THBOOK_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(thbook ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thbook.pdf)
