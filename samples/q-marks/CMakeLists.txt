therion_add_sample(q-marks-1 thconfig.1
                   DEPENDS_SRC thconfig.1.layout centerline.th
                   OUTPUT map1.pdf map2.pdf map.xvi)

therion_add_sample(q-marks-2 thconfig.2
                   DEPENDS_SRC thconfig.1.layout centerline.th map.th2
                   OUTPUT map3.pdf map4.pdf questions.html)
