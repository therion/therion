# Find all DLL dependencies to deploy on Windows.
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES ${THERION} ${LOCH}
    RESOLVED_DEPENDENCIES_VAR DLLS
    PRE_EXCLUDE_REGEXES "^api-ms-" "^ext-ms-"
    POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    DIRECTORIES $ENV{PATH}
)

file(MAKE_DIRECTORY ${DLLS_DIR})

foreach(DLL ${DLLS})
    message("Copying dependency: ${DLL}")
    file(COPY ${DLL} DESTINATION ${DLLS_DIR})
endforeach()
