add_custom_command(
    COMMAND
        perl generate.pl ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/thchencdata.h
        ${CMAKE_CURRENT_BINARY_DIR}/thchencdatatable.h
    DEPENDS
        ASCII.TXT
        CP1250.TXT
        CP1251.TXT
        CP1252.TXT
        CP1253.TXT
        8859-1.TXT
        8859-2.TXT
        8859-5.TXT
        8859-7.TXT
        generate.pl
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(generate_thchencdata
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/thchencdata.h
        ${CMAKE_CURRENT_BINARY_DIR}/thchencdatatable.h
)

add_library(therion-chencdata INTERFACE)
target_include_directories(therion-chencdata INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(therion-chencdata generate_thchencdata)
