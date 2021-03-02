therion_copy_files(thconfig.1 thconfig.2 thconfig.1.layout centerline.th map.th2)
therion_make_files_lists(OUTPUT1 map1.pdf map2.pdf map.xvi)

add_custom_command(
    OUTPUT ${OUTPUT1_BIN}
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.1
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.1
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.1.layout
            ${CMAKE_CURRENT_BINARY_DIR}/centerline.th
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-q-marks-1 DEPENDS ${OUTPUT1_BIN})
add_dependencies(samples-cmake samples-q-marks-1)

therion_make_files_lists(OUTPUT2 map3.pdf map4.pdf questions.html)

add_custom_command(
    OUTPUT ${OUTPUT2_BIN}
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.2
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.2
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.1.layout
            ${CMAKE_CURRENT_BINARY_DIR}/centerline.th
            ${CMAKE_CURRENT_BINARY_DIR}/map.th2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-q-marks-2 DEPENDS ${OUTPUT2_BIN})
add_dependencies(samples-cmake samples-q-marks-2)
