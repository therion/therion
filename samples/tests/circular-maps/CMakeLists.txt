get_filename_component(SAMPLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

function(circular_map_sample SUFFIX)
    therion_add_sample_with_ref(${SAMPLE_NAME}-${SUFFIX} thconfig-${SUFFIX}.thcfg
        DEPENDS_SRC scraps.th2
        OUTPUT out-${SUFFIX}.svg
    )
endfunction()

circular_map_sample(circular-mix)
circular_map_sample(circular-offset)
circular_map_sample(preview-basic)
circular_map_sample(preview-mix)
circular_map_sample(unused)

# no output, "error -- recursive map reference" in therion.log
# circular_map_sample(circular)
