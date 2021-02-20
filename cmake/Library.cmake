# run therion to generate source code for thlibrarydata.cxx
execute_process(
    COMMAND ${THERION} --print-library-src thlibrarydata.thcfg
    OUTPUT_VARIABLE LIBRARY_LOG
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# find start of source file in the therion log
string(FIND "${LIBRARY_LOG}" "/**" CXX_START)
if (${CXX_START} EQUAL -1)
    message(FATAL_ERROR "thlibrarydata.cxx generation failed")
endif()
string(SUBSTRING "${LIBRARY_LOG}" ${CXX_START} -1 LIBRARY_LOG)

# write generated CXX source to the source file
file(WRITE ${CMAKE_SOURCE_DIR}/thlibrarydata.cxx "${LIBRARY_LOG}")
