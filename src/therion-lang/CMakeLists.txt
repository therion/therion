add_custom_command(
    COMMAND
        perl process.pl ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT 
        ${CMAKE_CURRENT_BINARY_DIR}/thlangdata.h
        ${CMAKE_CURRENT_BINARY_DIR}/thlangdatafields.h
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/process.pl
        ${CMAKE_CURRENT_SOURCE_DIR}/texts.txt
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(generate_thlang
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/thlangdata.h
        ${CMAKE_CURRENT_BINARY_DIR}/thlangdatafields.h
)

add_custom_target(update perl process.pl update ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(therion-lang INTERFACE)
target_include_directories(therion-lang INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(therion-lang generate_thlang)
