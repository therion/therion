# Enable sanitizers.

# Fail on errors.
set(CMAKE_C_FLAGS_INIT "-fno-sanitize-recover=all")
set(CMAKE_CXX_FLAGS_INIT "-fno-sanitize-recover=all")

# Optimizations help to uncover bugs, debug symbols make reports from sanitizers readable.
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build.")

# ASan and UBSan can be run together.
set(ECM_ENABLE_SANITIZERS "address;undefined" CACHE STRING "Enable runtime sanitizers, available options: address,memory,thread,leak,undefined." )
