# Clang + ASan + UBSan + optimizations + debug symbols

include(${CMAKE_CURRENT_LIST_DIR}/clang.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sanitizers.cmake)

# Optimizations help to uncover bugs, debug symbols make reports from sanitizers readable.
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build.")
