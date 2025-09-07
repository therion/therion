foreach(I RANGE 1 6)
    therion_add_sample(map-offset-${I} thconfig.${I}
                       DEPENDS_SRC thconfig.layout cave.th cave.th2
                       OUTPUT map${I}.pdf)
endforeach()
