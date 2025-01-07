# link with this interface library to enable warnings, used for project code
add_library(enable-warnings INTERFACE)

# link with this interface library to disable warnings, used for extern libraries
add_library(disable-warnings INTERFACE)

if (MSVC)
    target_compile_options(enable-warnings INTERFACE /W4)
    target_compile_options(disable-warnings INTERFACE /w)
else()
    target_compile_options(enable-warnings INTERFACE -Wall -Wextra)
    target_compile_options(disable-warnings INTERFACE -w)
endif()