therion_add_cavern(use)
therion_add_sample(survex-use thconfig
                   DEPENDS_SRC use.th use-in.th2 use-out.th2 ../thconfig.layout
                   DEPENDS_OTHER ${CMAKE_CURRENT_BINARY_DIR}/use.3d
                   OUTPUT use.pdf)
