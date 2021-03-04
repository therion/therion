# Set GCC as a compiler.

find_program(CMAKE_C_COMPILER gcc)
find_program(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}" CACHE STRING "C++ compiler" FORCE)
