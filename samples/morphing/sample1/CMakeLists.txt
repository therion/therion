foreach(I RANGE 1 2)
    therion_add_sample(morphing-1-${I} thconfig.${I}
                       DEPENDS_SRC cave1.jpg cave1.th cave${I}.th2 ../thconfig.layout
                       OUTPUT cave${I}.pdf)
endforeach()
