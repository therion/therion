# link with this interface library to enable warnings, used for project code
add_library(enable-warnings INTERFACE)
target_compile_options(enable-warnings INTERFACE -Wall)

# link with this interface library to disable warnings, used for extern libraries
add_library(disable-warnings INTERFACE)
target_compile_options(disable-warnings INTERFACE -w)
