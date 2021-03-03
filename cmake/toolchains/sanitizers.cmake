# Enable sanitizers.

# Fail on errors.
set(CMAKE_C_FLAGS_INIT "-fno-sanitize-recover=all")
set(CMAKE_CXX_FLAGS_INIT "-fno-sanitize-recover=all")
# ASan and UBSan can be run together.
set(ECM_ENABLE_SANITIZERS "address;undefined" CACHE STRING "Enable runtime sanitizers, available options: address,memory,thread,leak,undefined." )
