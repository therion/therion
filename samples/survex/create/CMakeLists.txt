therion_add_cavern(create)
therion_add_sample(survex-create thconfig
                   OUTPUT create.pdf
                   DEPENDS_SRC create.th create.th2 ../thconfig.layout
                   DEPENDS_OTHER ${CMAKE_CURRENT_BINARY_DIR}/create.3d)
