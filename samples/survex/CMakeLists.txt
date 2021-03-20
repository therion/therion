foreach(I RANGE 1 2)
    therion_add_sample(survex-${I} thconfig.${I}
                       DEPENDS_SRC cave${I}.th thconfig.layout cave.3d
                       OUTPUT cave${I}.pdf)
endforeach()

add_subdirectory(create)
add_subdirectory(ignore)
add_subdirectory(use)
