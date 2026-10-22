get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# shots3d.dbf is compared byte for byte: shapelib writes a fixed dummy date in
# the DBF header, so the file is reproducible.
therion_add_sample_with_ref(${SAMPLE_NAME} thconfig
    OUTPUT database.sql shots3d.dbf
    DEPENDS_SRC data.th
)
