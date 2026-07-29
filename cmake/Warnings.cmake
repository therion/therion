# link with this interface library to enable warnings, used for project code
add_library(enable-warnings INTERFACE)

# link with this interface library to disable warnings, used for extern libraries
add_library(disable-warnings INTERFACE)

if (MSVC)
    target_compile_options(enable-warnings INTERFACE /W1)
    target_compile_options(disable-warnings INTERFACE /w)
else()
    target_compile_options(enable-warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-deprecated-enum-enum-conversion
        -Wno-overlength-strings
    )
    target_compile_options(disable-warnings INTERFACE -w)
endif()

# silence Clang warning: '__COUNTER__' is a C2y extension
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
    CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 22)
    target_compile_options(enable-warnings INTERFACE -Wno-c2y-extensions)
endif()
