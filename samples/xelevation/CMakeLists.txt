therion_add_sample(xelevation-1 thconfig.1
                   DEPENDS_SRC thconfig.layout cave1.th cave1.th2
                   OUTPUT map1p.pdf map1x.pdf)

therion_add_sample(xelevation-2 thconfig.2
                   DEPENDS_SRC thconfig.layout cave2.th
                   OUTPUT map2.pdf)

therion_add_sample(xelevation-3 thconfig.3
                   DEPENDS_SRC thconfig.layout cave3.th
                   OUTPUT map3.pdf)

therion_add_sample(xelevation-4 thconfig.4
                   DEPENDS_SRC thconfig.layout cave3.th cave3.th2
                   OUTPUT map4.pdf)

therion_add_sample(xelevation-5 thconfig.5
                   DEPENDS_SRC thconfig.layout cave3.th cave3.th2
                   OUTPUT map5.pdf)

therion_add_sample(xelevation-6 thconfig.6
                   DEPENDS_SRC thconfig.layout cave3.th
                   OUTPUT map6.pdf)

therion_add_sample(xelevation-7 thconfig.7
                   DEPENDS_SRC thconfig.layout cave3.th
                   OUTPUT map7.pdf)
