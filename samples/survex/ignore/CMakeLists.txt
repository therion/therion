therion_add_cavern(ignore)
therion_add_sample(survex-ignore thconfig
                   DEPENDS_SRC ignore.th ignore.th2 ../thconfig.layout
                   DEPENDS_OTHER ${CMAKE_CURRENT_BINARY_DIR}/ignore.3d
                   OUTPUT ignore.pdf)
