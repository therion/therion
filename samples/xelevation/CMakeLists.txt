therion_copy_files(
    cave1.th
    cave1.th2
    cave1.xvi
    cave2.th
    cave3.th
    cave3.th2
    cave3.xvi
    thconfig.layout
    thconfig.1
    thconfig.2
    thconfig.3
    thconfig.4
    thconfig.5
    thconfig.6
    thconfig.7)

add_custom_command(
    OUTPUT map1p.pdf map1x.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.1
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.1
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave1.th
            ${CMAKE_CURRENT_BINARY_DIR}/cave1.th2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(samples-xelevation-1 DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/map1p.pdf ${CMAKE_CURRENT_BINARY_DIR}/map1x.pdf)
add_dependencies(samples-cmake samples-xelevation-1)

add_custom_command(
    OUTPUT map2.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.2
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.2
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave2.th
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT map3.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.3
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.3
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT map4.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.4
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.4
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT map5.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.5
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.5
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT map6.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.6
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.6
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT map7.pdf
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:therion> --reproducible-output thconfig.7
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/thconfig.7
            ${CMAKE_CURRENT_BINARY_DIR}/thconfig.layout
            ${CMAKE_CURRENT_BINARY_DIR}/cave3.th
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

foreach(I RANGE 2 7)
    add_custom_target(samples-xelevation-${I} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/map${I}.pdf)
    add_dependencies(samples-cmake samples-xelevation-${I})
endforeach()
