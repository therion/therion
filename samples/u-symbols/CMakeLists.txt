foreach(I RANGE 1 2)
    therion_add_sample(u-symbols-${I} thconfig.${I}
                       DEPENDS_SRC map.th2
                       OUTPUT map${I}.pdf)
endforeach()
