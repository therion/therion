# Find all DLL dependencies to deploy on Windows.
include(GetPrerequisites)
get_prerequisites(${THERION} DLLS 1 1 "" "")
get_prerequisites(${LOCH}    DLLS 1 1 "" "")

file(MAKE_DIRECTORY ${DLLS_DIR})

foreach(DLL ${DLLS})
    gp_resolve_item("" ${DLL} "" "" RESOLVED)
    message("Copying dependency: ${RESOLVED}")
    file(COPY ${RESOLVED} DESTINATION ${DLLS_DIR})
endforeach()
