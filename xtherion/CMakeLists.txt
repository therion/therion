if (NOT BUILD_XTHERION)
    return()
endif()

if (NOT BUILD_THERION)
    message(FATAL_ERROR "xtherion depends on therion")
endif()

if(WIN32)
  set(XTHERION_EXECUTABLE "xtherion.tcl")
else()
  set(XTHERION_EXECUTABLE "xtherion")
endif()

set(TCL_SOURCES
    about.tcl
    app.tcl
    bac.tcl
    console.tcl
    cp_procs.tcl
    cp.tcl
    ctrl.tcl
    dbg.tcl
    global.tcl
    help.tcl
    init.tcl
    licence.tcl
    main.tcl
    mapcal.tcl
    me_cmds.tcl
    me_cmds2.tcl
    me_imgs.tcl
    me_import.tcl
    me_ss.tcl
    me.tcl
    mkall.tcl
    msgusr.tcl
    sbar.tcl
    scroll.tcl
    source.tcl
    svx_global.tcl
    syntax.tcl
    te_import.tcl
    te_sdata.tcl
    te_sdata2.tcl
    te.tcl
    tools_imgs.tcl
    tools.tcl)

therion_copy_files(${TCL_SOURCES} lang/process.pl lang/xtexts.txt)
therion_make_files_lists(TCL ${TCL_SOURCES})

add_custom_command(
  OUTPUT msgxth.tcl
  COMMAND perl process.pl
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lang/xtexts.txt 
          ${CMAKE_CURRENT_BINARY_DIR}/lang/process.pl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lang)

add_custom_command(
  OUTPUT therion.tcl
  COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --print-xtherion-src > therion.tcl
  DEPENDS therion
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
  OUTPUT ${XTHERION_EXECUTABLE}
  COMMAND tclsh mkall.tcl ${THPLATFORM}
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/msgxth.tcl
          ${CMAKE_CURRENT_BINARY_DIR}/therion.tcl
          ${TCL_BIN}
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

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${XTHERION_EXECUTABLE} TYPE BIN COMPONENT th-runtime)
install(FILES xtherion.ini TYPE SYSCONF RENAME xtherion.ini.new COMPONENT th-runtime)
install(CODE
        "if (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/xtherion.ini)
            file(INSTALL ${CMAKE_CURRENT_SOURCE_DIR}/xtherion.ini DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR})
        endif()"
        COMPONENT th-runtime)
