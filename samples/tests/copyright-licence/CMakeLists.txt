get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

therion_add_sample_with_ref(${SAMPLE_NAME} thconfig
    OUTPUT map.th2
    DEPENDS_SRC data.th data.th2
)
