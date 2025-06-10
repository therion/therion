# link with this interface library to enable warnings, used for project code
add_library(enable-warnings INTERFACE)

# link with this interface library to disable warnings, used for extern libraries
add_library(disable-warnings INTERFACE)

if (MSVC)
    target_compile_options(enable-warnings INTERFACE /W4)
    target_compile_options(disable-warnings INTERFACE /w)
else()
    target_compile_options(enable-warnings INTERFACE -Wall -Wextra -Wno-deprecated-enum-enum-conversion)
    target_compile_options(disable-warnings INTERFACE -w)
endif()

# enforce warnings as errors
set(ENABLE_WERROR OFF CACHE BOOL "Report warnings as errors.")
if (ENABLE_WERROR)
    if (MSVC)
        target_compile_options(enable-warnings INTERFACE /WX)
    else()
        target_compile_options(enable-warnings INTERFACE -Werror)
    endif()
endif()
