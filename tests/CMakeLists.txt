if (NOT BUILD_THERION OR NOT BUILD_TESTING)
    return()
endif()

include(Catch2)

add_executable(utest 
    utest-main.cxx
    utest-proj.cxx
    utest-str.cxx
    utest-thdatastation.cxx
    utest-thdb2dab.cxx
    utest-thdouble.cxx
    utest-thexception.cxx
    utest-thlayoutln.cxx
    utest-thlogfile.cxx
    utest-thobjectid.cxx
    utest-thobjectsrc.cxx
    utest-thscrapen.cxx
    utest-thscraplo.cxx
)
target_link_libraries(utest PUBLIC therion-core catch2-interface)

add_test(NAME utest COMMAND $<TARGET_FILE:utest> WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
if (ENABLE_CODE_COVERAGE)
    set_property(TEST utest PROPERTY ENVIRONMENT LLVM_PROFILE_FILE=${COVERAGE_FOLDER}/utest.profraw)
endif()
