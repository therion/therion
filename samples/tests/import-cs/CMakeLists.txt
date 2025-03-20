get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

therion_add_sample_with_ref(${SAMPLE_NAME} thconfig
    DEPENDS_SRC data.3d
    OUTPUT out.bbox
)
