foreach(I RANGE 1 3)
    therion_add_sample(morphing-2-${I} thconfig.${I}
                       DEPENDS_SRC cave.png cave.th cave${I}.th2 ../thconfig.layout
                       OUTPUT cave${I}.pdf)
endforeach()
