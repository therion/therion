#
# Generate code coverage report for unit tests.
#
# How to use:
#   1. Use Clang compiler.
#   2. Set already configured build folder as a working directory.
#   3. Enable code coverage: cmake -DENABLE_CODE_COVERAGE=ON .
#   4. Build the project:    cmake --build .
#   5. Run unit tests:       cmake --build . --target test
#   6. Generate report:      cmake --build . --target ccov-report
#   7. View HTML report at ./coverage/html/index.html.
#

# interface library for setting compiler and linker flags
add_library(code-coverage INTERFACE)

if (ENABLE_CODE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Code coverage requires Clang compiler.")
    endif()

    set(COVERAGE_FLAGS -fprofile-instr-generate -fcoverage-mapping)
    target_compile_options(code-coverage INTERFACE ${COVERAGE_FLAGS})
    target_link_options(code-coverage INTERFACE ${COVERAGE_FLAGS})
endif()

set(COVERAGE_FOLDER ${CMAKE_BINARY_DIR}/coverage)

add_custom_target(ccov-report
    # process raw coverage profile
    COMMAND llvm-profdata merge -sparse ${COVERAGE_FOLDER}/utest.profraw -o ${COVERAGE_FOLDER}/utest.profdata
    # generate HTML report
    COMMAND llvm-cov show 
        $<TARGET_FILE:utest>
        -instr-profile=${COVERAGE_FOLDER}/utest.profdata
        -output-dir=${COVERAGE_FOLDER}/html
        -format=html
        -show-line-counts-or-regions
        -show-expansions
        -Xdemangler=c++filt
    # Generate console summary
    COMMAND llvm-cov report 
        $<TARGET_FILE:utest> 
        -instr-profile=${COVERAGE_FOLDER}/utest.profdata
        -show-region-summary=false
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating coverage report for utest"
)
