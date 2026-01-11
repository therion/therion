add_library(therion-core STATIC ${THERION_SOURCES})

target_include_directories(therion-core PUBLIC ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(therion-core SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/extern/stl_reader)
target_link_libraries(therion-core PUBLIC proj-interface shp-interface poly2tri common-utils QuickHull fmt::fmt $<$<PLATFORM_ID:Android>:log>)
target_compile_definitions(therion-core PUBLIC "TH${THPLATFORM}" $<$<BOOL:${ENABLE_THDEBUG}>:THDEBUG>)
add_dependencies(therion-core
    thversion
    generate_thsymbolsetlist
    generate_thlang
    generate_tex
    generate_mpost
    generate_thchencdata
    generate_thcsdata
)
