#! /usr/bin/tclsh

# <identifiers> <options> <libPROJ definition> <.PRJ definition>
set proj_specs {
  {{long-lat} {dms} "+init=epsg:4326" {} {}}
  {{lat-long} {dms swap} "+init=epsg:4326 " {} {}}
  {{etrs} {dms} "+init=epsg:4258" {} {}}
  {{jtsk}   {} "+proj=krovak +axis=wsu +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {} {}}
  {{jtsk03} {} "+proj=krovak +axis=wsu +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {} {}}
  {{ijtsk}   {output} "+proj=krovak +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {BOUNDCRS[SOURCECRS[PROJCRS["unknown",BASEGEOGCRS["unknown",DATUM["Unknown based on Bessel 1841 ellipsoid",ELLIPSOID["Bessel 1841",6377397.155,299.1528128,LENGTHUNIT["metre",1,ID["EPSG",9001]]]],PRIMEM["Greenwich",0,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8901]]],CONVERSION["unknown",METHOD["Krovak (North Orientated)",ID["EPSG",1041]],PARAMETER["Latitude of projection centre",49.5,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8811]],PARAMETER["Longitude of origin",24.8333333333333,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8833]],PARAMETER["Co-latitude of cone axis",30.2881397527778,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",1036]],PARAMETER["Latitude of pseudo standard parallel",78.5,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8818]],PARAMETER["Scale factor on pseudo standard parallel",0.9999,SCALEUNIT["unity",1],ID["EPSG",8819]],PARAMETER["False easting",0,LENGTHUNIT["metre",1],ID["EPSG",8806]],PARAMETER["False northing",0,LENGTHUNIT["metre",1],ID["EPSG",8807]]],CS[Cartesian,2],AXIS["(E)",east,ORDER[1],LENGTHUNIT["metre",1,ID["EPSG",9001]]],AXIS["(N)",north,ORDER[2],LENGTHUNIT["metre",1,ID["EPSG",9001]]]]],TARGETCRS[GEOGCRS["WGS 84",DATUM["World Geodetic System 1984",ELLIPSOID["WGS 84",6378137,298.257223563,LENGTHUNIT["metre",1]]],PRIMEM["Greenwich",0,ANGLEUNIT["degree",0.0174532925199433]],CS[ellipsoidal,2],AXIS["latitude",north,ORDER[1],ANGLEUNIT["degree",0.0174532925199433]],AXIS["longitude",east,ORDER[2],ANGLEUNIT["degree",0.0174532925199433]],ID["EPSG",4326]]],ABRIDGEDTRANSFORMATION["Transformation from unknown to WGS84",METHOD["Position Vector transformation (geog2D domain)",ID["EPSG",9606]],PARAMETER["X-axis translation",570.8285,ID["EPSG",8605]],PARAMETER["Y-axis translation",85.6769,ID["EPSG",8606]],PARAMETER["Z-axis translation",462.842,ID["EPSG",8607]],PARAMETER["X-axis rotation",4.9984,ID["EPSG",8608]],PARAMETER["Y-axis rotation",1.5867,ID["EPSG",8609]],PARAMETER["Z-axis rotation",5.2611,ID["EPSG",8610]],PARAMETER["Scale difference",1.0000035623,ID["EPSG",8611]]]]} {}}
  {{ijtsk03} {output} "+proj=krovak +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {PROJCRS["S-JTSK [JTSK03] / Krovak East North",BASEGEOGCRS["S-JTSK [JTSK03]",DATUM["System of the Unified Trigonometrical Cadastral Network [JTSK03]",ELLIPSOID["Bessel 1841",6377397.155,299.1528128,LENGTHUNIT["metre",1]]],PRIMEM["Greenwich",0,ANGLEUNIT["degree",0.0174532925199433]],ID["EPSG",8351]],CONVERSION["Krovak East North (Greenwich)",METHOD["Krovak (North Orientated)",ID["EPSG",1041]],PARAMETER["Latitude of projection centre",49.5,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8811]],PARAMETER["Longitude of origin",24.8333333333333,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8833]],PARAMETER["Co-latitude of cone axis",30.2881397527778,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",1036]],PARAMETER["Latitude of pseudo standard parallel",78.5,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8818]],PARAMETER["Scale factor on pseudo standard parallel",0.9999,SCALEUNIT["unity",1],ID["EPSG",8819]],PARAMETER["False easting",0,LENGTHUNIT["metre",1],ID["EPSG",8806]],PARAMETER["False northing",0,LENGTHUNIT["metre",1],ID["EPSG",8807]]],CS[Cartesian,2],AXIS["easting (X)",east,ORDER[1],LENGTHUNIT["metre",1]],AXIS["northing (Y)",north,ORDER[2],LENGTHUNIT["metre",1]],USAGE[SCOPE["unknown"],AREA["Slovakia"],BBOX[47.73,16.84,49.61,22.56]],ID["EPSG",8353]]} {}}
  {{s-merc} {output} "+init=epsg:3857" {PROJCRS["WGS 84 / Pseudo-Mercator",BASEGEOGCRS["WGS 84",DATUM["World Geodetic System 1984",ELLIPSOID["WGS 84",6378137,298.257223563,LENGTHUNIT["metre",1]]],PRIMEM["Greenwich",0,ANGLEUNIT["degree",0.0174532925199433]],ID["EPSG",4326]],CONVERSION["Popular Visualisation Pseudo-Mercator",METHOD["Popular Visualisation Pseudo Mercator",ID["EPSG",1024]],PARAMETER["Latitude of natural origin",0,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8801]],PARAMETER["Longitude of natural origin",0,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8802]],PARAMETER["False easting",0,LENGTHUNIT["metre",1],ID["EPSG",8806]],PARAMETER["False northing",0,LENGTHUNIT["metre",1],ID["EPSG",8807]]],CS[Cartesian,2],AXIS["easting (X)",east,ORDER[1],LENGTHUNIT["metre",1]],AXIS["northing (Y)",north,ORDER[2],LENGTHUNIT["metre",1]],USAGE[SCOPE["unknown"],AREA["World - 85°S to 85°N"],BBOX[-85.06,-180,85.06,180]],ID["EPSG",3857]]} {}}
  {{eur79z30} {output} "+proj=utm +zone=30 +ellps=intl +towgs84=-86,-98,-119,0,0,0,0 +no_defs" {BOUNDCRS[SOURCECRS[PROJCRS["unknown",BASEGEOGCRS["unknown",DATUM["Unknown based on International 1909 (Hayford) ellipsoid",ELLIPSOID["International 1909 (Hayford)",6378388,297,LENGTHUNIT["metre",1,ID["EPSG",9001]]]],PRIMEM["Greenwich",0,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8901]]],CONVERSION["UTM zone 30N",METHOD["Transverse Mercator",ID["EPSG",9807]],PARAMETER["Latitude of natural origin",0,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8801]],PARAMETER["Longitude of natural origin",-3,ANGLEUNIT["degree",0.0174532925199433],ID["EPSG",8802]],PARAMETER["Scale factor at natural origin",0.9996,SCALEUNIT["unity",1],ID["EPSG",8805]],PARAMETER["False easting",500000,LENGTHUNIT["metre",1],ID["EPSG",8806]],PARAMETER["False northing",0,LENGTHUNIT["metre",1],ID["EPSG",8807]],ID["EPSG",16030]],CS[Cartesian,2],AXIS["(E)",east,ORDER[1],LENGTHUNIT["metre",1,ID["EPSG",9001]]],AXIS["(N)",north,ORDER[2],LENGTHUNIT["metre",1,ID["EPSG",9001]]]]],TARGETCRS[GEOGCRS["WGS 84",DATUM["World Geodetic System 1984",ELLIPSOID["WGS 84",6378137,298.257223563,LENGTHUNIT["metre",1]]],PRIMEM["Greenwich",0,ANGLEUNIT["degree",0.0174532925199433]],CS[ellipsoidal,2],AXIS["latitude",north,ORDER[1],ANGLEUNIT["degree",0.0174532925199433]],AXIS["longitude",east,ORDER[2],ANGLEUNIT["degree",0.0174532925199433]],ID["EPSG",4326]]],ABRIDGEDTRANSFORMATION["Transformation from unknown to WGS84",METHOD["Position Vector transformation (geog2D domain)",ID["EPSG",9606]],PARAMETER["X-axis translation",-86,ID["EPSG",8605]],PARAMETER["Y-axis translation",-98,ID["EPSG",8606]],PARAMETER["Z-axis translation",-119,ID["EPSG",8607]],PARAMETER["X-axis rotation",0,ID["EPSG",8608]],PARAMETER["Y-axis rotation",0,ID["EPSG",8609]],PARAMETER["Z-axis rotation",0,ID["EPSG",8610]],PARAMETER["Scale difference",1,ID["EPSG",8611]]]]} {}}
}

#  {{OSGB:ST} {output} {+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=100000 +y_0=-200000 +ellps=airy +datum=OSGB36 +units=m +no_defs} {} {}}
#  {{OSGB:SN} {output} {+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=200000 +y_0=-300000 +ellps=airy +datum=OSGB36 +units=m +no_defs} {} {}}

# add UTM projections
for {set zone 1} {$zone <= 60} {incr zone} {
  lappend proj_specs [list [format "utm%dn utm%d" $zone $zone $zone $zone $zone] {output} [format "+init=epsg:%d" [expr $zone+32600]] [format {PROJCS["WGS_1984_UTM_Zone_%dN",GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["False_Easting",500000.0],PARAMETER["False_Northing",0.0],PARAMETER["Central_Meridian",%.1f],PARAMETER["Scale_Factor",0.9996],PARAMETER["Latitude_Of_Origin",0.0],UNIT["Meter",1.0]]} $zone [expr double($zone * 6 - 183)]] [format "WGS84 / UTM zone %dN" $zone]]
  lappend proj_specs [list [format "utm%ds" $zone $zone $zone] {output} [format "+init=epsg:%d" [expr $zone+32700]] [format {PROJCS["WGS_1984_UTM_Zone_%dS",GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["False_Easting",500000.0],PARAMETER["False_Northing",10000000.0],PARAMETER["Central_Meridian",%.1f],PARAMETER["Scale_Factor",0.9996],PARAMETER["Latitude_Of_Origin",0.0],UNIT["Meter",1.0]]} $zone [expr double($zone * 6 - 183)]] [format "WGS84 / UTM zone %dS" $zone]]
}

# ETRS zones can't be mixed with UTM zones as therion (thdataobject.cxx) uses offsets between UTM zone positions in the list
for {set zone 28} {$zone <= 37} {incr zone} {
  lappend proj_specs [list [format "etrs%d" $zone $zone $zone $zone $zone] {output} [format "+init=epsg:%d" [expr $zone+25800]] [format {PROJCS["ETRS89_UTM_zone_%dN",GEOGCS["GCS_ETRS_1989",DATUM["D_ETRS_1989",SPHEROID["GRS_1980",6378137,298.257222101]],PRIMEM["Greenwich",0],UNIT["Degree",0.017453292519943295]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",%.1f],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["Meter",1]]} $zone [expr double($zone * 6 - 183)]] [format "ETRS89 / UTM zone %dN" $zone]]
}

# <from-ids> <to-ids> <transformation>
set proj_transformations {
  # adapted from https://www.geoportal.sk/files/qgis/qgis_trans_proj8.zip
  # with all deg/rad steps removed
  {{etrs epsg:4258} {ijtsk epsg:5514} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs epsg:4258} {ijtsk03 epsg:8353} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{long-lat lat-long epsg:4326} {ijtsk03 epsg:8353} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {etrs epsg:4258} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3}}
  {{ijtsk epsg:5514} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk03 epsg:8353} {etrs epsg:4258} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3}}
  {{ijtsk03 epsg:8353} {long-lat lat-long epsg:4326} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3}}
  {{ijtsk03 epsg:8353} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3046} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3046} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{utm34 epsg:32634} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{s-merc epsg:3857} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {epsg:3046} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {epsg:3046} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {utm34 epsg:32634} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=WGS84}}
  {{ijtsk03 epsg:8353} {s-merc epsg:3857} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84}}
  {{ijtsk epsg:5514} {long-lat lat-long epsg:4326} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3}}
  {{ijtsk epsg:5514} {utm34 epsg:32634} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=WGS84}}
  {{ijtsk epsg:5514} {s-merc epsg:3857} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84}}
  {{long-lat lat-long epsg:4326} {ijtsk epsg:5514} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{utm34 epsg:32634} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{s-merc epsg:3857} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs34 epsg:25834} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +inv +proj=helmert +x=570.8 +y=85.7 +z=462.8 +rx=4.998 +ry=1.587 +rz=5.261 +s=3.56 +convention=position_vector +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs34 epsg:25834} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3034} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +inv +proj=helmert +x=570.8 +y=85.7 +z=462.8 +rx=4.998 +ry=1.587 +rz=5.261 +s=3.56 +convention=position_vector +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3034} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3035} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3035} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {etrs34 epsg:25834} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{ijtsk epsg:5514} {epsg:3035} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {etrs34 epsg:25834} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{ijtsk epsg:5514} {epsg:3034} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {epsg:3035} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {epsg:3034} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80}}
  {{epsg:3045} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3045} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {epsg:3045} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {epsg:3045} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{etrs33 epsg:25833} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs33 epsg:25833} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {etrs33 epsg:25833} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{ijtsk03 epsg:8353} {etrs33 epsg:25833} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{utm33 epsg:32633} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk03 epsg:8353} {utm33 epsg:32633} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=WGS84}}
  {{utm33 epsg:32633} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {utm33 epsg:32633} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=WGS84}}
  {{etrs epsg:4258} {long-lat lat-long epsg:4326} {+proj=noop}}
  {{long-lat lat-long epsg:4326} {etrs epsg:4258} {+proj=noop}}
  # jtsk variants added
  {{etrs epsg:4258} {jtsk epsg:5513} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs epsg:4258} {jtsk03 epsg:8352} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{long-lat lat-long epsg:4326} {jtsk03 epsg:8352} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk epsg:5513} {etrs epsg:4258} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3}}
  {{jtsk epsg:5513} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk03 epsg:8352} {etrs epsg:4258} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3}}
  {{jtsk03 epsg:8352} {long-lat lat-long epsg:4326} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3}}
  {{jtsk03 epsg:8352} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3046} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3046} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{utm34 epsg:32634} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{s-merc epsg:3857} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk epsg:5513} {epsg:3046} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {epsg:3046} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {utm34 epsg:32634} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=WGS84}}
  {{jtsk03 epsg:8352} {s-merc epsg:3857} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84}}
  {{jtsk epsg:5513} {long-lat lat-long epsg:4326} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3}}
  {{jtsk epsg:5513} {utm34 epsg:32634} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=WGS84}}
  {{jtsk epsg:5513} {s-merc epsg:3857} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84}}
  {{long-lat lat-long epsg:4326} {jtsk epsg:5513} {+proj=pipeline +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{utm34 epsg:32634} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{s-merc epsg:3857} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=webmerc +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs34 epsg:25834} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +inv +proj=helmert +x=570.8 +y=85.7 +z=462.8 +rx=4.998 +ry=1.587 +rz=5.261 +s=3.56 +convention=position_vector +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs34 epsg:25834} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=utm +zone=34 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3034} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +inv +proj=helmert +x=570.8 +y=85.7 +z=462.8 +rx=4.998 +ry=1.587 +rz=5.261 +s=3.56 +convention=position_vector +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3034} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3035} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3035} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk epsg:5513} {etrs34 epsg:25834} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{jtsk epsg:5513} {epsg:3035} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {etrs34 epsg:25834} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=34 +ellps=GRS80}}
  {{jtsk epsg:5513} {epsg:3034} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {epsg:3035} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=laea +lat_0=52 +lon_0=10 +x_0=4321000 +y_0=3210000 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {epsg:3034} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=lcc +lat_0=52 +lon_0=10 +lat_1=35 +lat_2=65 +x_0=4000000 +y_0=2800000 +ellps=GRS80}}
  {{epsg:3045} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{epsg:3045} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk epsg:5513} {epsg:3045} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {epsg:3045} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{etrs33 epsg:25833} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{etrs33 epsg:25833} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=GRS80 +step +proj=push +v_3 +step +proj=cart +ellps=GRS80 +step +proj=helmert +x=-485.014055 +y=-169.473618 +z=-483.842943 +rx=7.78625453 +ry=4.39770887 +rz=4.10248899 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk epsg:5513} {etrs33 epsg:25833} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{jtsk03 epsg:8352} {etrs33 epsg:25833} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=GRS80 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=GRS80}}
  {{utm33 epsg:32633} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk03 epsg:8352} {utm33 epsg:32633} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=WGS84}}
  {{utm33 epsg:32633} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=utm +zone=33 +ellps=WGS84 +step +proj=push +v_3 +step +proj=cart +ellps=WGS84 +step +inv +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=bessel +step +proj=pop +v_3 +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk epsg:5513} {utm33 epsg:32633} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=push +v_3 +step +proj=cart +ellps=bessel +step +proj=helmert +x=485.021 +y=169.465 +z=483.839 +rx=-7.786342 +ry=-4.397554 +rz=-4.102655 +s=0 +convention=coordinate_frame +step +inv +proj=cart +ellps=WGS84 +step +proj=pop +v_3 +step +proj=utm +zone=33 +ellps=WGS84}}
  # combined jtsk/ijtsk
  {{jtsk epsg:5513} {ijtsk03 epsg:8353} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{jtsk03 epsg:8352} {ijtsk epsg:5514} {+proj=pipeline +step +inv +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk epsg:5514} {jtsk03 epsg:8352} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +inv +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
  {{ijtsk03 epsg:8353} {jtsk epsg:5513} {+proj=pipeline +step +inv +proj=krovak +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel +step +proj=hgridshift +grids=sk_gku_JTSK03_to_JTSK.tif +step +proj=krovak +axis=wsu +lat_0=49.5 +lon_0=24.8333333333333 +alpha=30.2881397527778 +k=0.9999 +x_0=0 +y_0=0 +ellps=bessel}}
}

# remove comments from the list
set proj_transformations [regsub -all {#.*?\n} $proj_transformations \n]

set osgb1 {
	{S T}
	{N O}
    {H}
}
set osgb2 {
	{V W X Y Z}
	{Q R S T U}
	{L M N O P}
	{F G H J K}
	{A B C D E}
}


set yy -1
foreach al $osgb1 {
	set xx 4
	foreach a $al {
		set y $yy
		foreach bl $osgb2 {
			set x $xx
			foreach b $bl {
				lappend proj_specs [list "OSGB:$a$b" {output} "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=[expr $x * 100000] +y_0=[expr $y * 100000] +ellps=airy +datum=OSGB36 +units=m +no_defs" {} "OSGB:$a$b"]
				set x [expr $x - 1]
			}
			set y [expr $y - 1]
		}
		set xx [expr $xx - 5]
	}
	set yy [expr $yy - 5]
}


set osgbspecs {{ST 1 -2} {SN 2 -3} {} {} {} {}}

# join identical projections
array set proj_defs {}
set new_proj_specs {}
foreach prj $proj_specs {
  if {[info exists proj_defs([lindex $prj 2])]} {
    set px $proj_defs([lindex $prj 2])
    set orig_prj [lindex $new_proj_specs $px]
    # add identifiers
    foreach id [lindex $prj 0] {
      if {[lsearch -exact [lindex $orig_prj 0] $id] == -1} {
	lset orig_prj 0 "[lindex $orig_prj 0] $id"
      }
    }
    # replace PRJ spec
    if {[string length [lindex $orig_prj 3]] == 0} {
      lset orig_prj 3 [lindex $prj 3]
    }
    if {[string length [lindex $orig_prj 4]] == 0} {
      lset orig_prj 4 [lindex $prj 4]
    }
    lset new_proj_specs $px $orig_prj
  } else {
    set proj_defs([lindex $prj 2]) [llength $new_proj_specs]
    lappend new_proj_specs $prj
  }
}

set proj_specs $new_proj_specs

# create parsing table
set proj_parse {{local TTCS_LOCAL}}
set proj_enum {}
foreach prj $proj_specs {
  set id0 [lindex [lindex $prj 0] 0]
  set enm [format "TTCS_%s" [string toupper $id0]]
  regsub -all {[\:\-]} $enm "_" enm
  lappend proj_enum $enm
  foreach id [lindex $prj 0] {
    lappend proj_parse [list [string toupper $id] $enm]
  }
}

	
proc escape {str} {
  return [regsub -all {"} $str {\"}]
}

# write header file and parsing table
set fid [open "thcsdata.h" w]
puts $fid {/**
 * @file thcsdata.h
 * Coordinate systems data.
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */
       
#ifndef thcsdata_h
#define thcsdata_h
 
#include "thstok.h"
 
#include <map>

/**
* Add default CS transformations.
*/
void thcs_add_default_transformations();
	
/**
 * CS tokens.
 */
}

puts $fid "enum \{\n  TTCS_UNKNOWN = -2,\n  TTCS_LOCAL = -1,"




 
foreach e $proj_enum {
  puts $fid "  $e,"
} 
 
puts $fid "\};"

 
puts $fid {

/**
 * CS data structure.
 */
}
puts $fid "typedef struct \{\n  bool dms, output, swap;\n  const char * params;  const char * prjspec;  const char * prjname;\n\} thcsdata;\n"

puts $fid "extern const thstok thtt_cs\[[expr [llength $proj_parse] + 1]\];\n";

puts $fid "extern const thcsdata thcsdata_table\[[llength $proj_specs]\];\n"

puts $fid {
#endif
} 

close $fid

set fid [open "thcsdata.cxx" w]

puts $fid {/**
 * @file thcsdata.cxx
 * Coordinate systems data.
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */
       
#include "thcsdata.h"
#include "thcs.h"

}


puts $fid {

/**
 * CS parsing table.
 */
}   
puts $fid "const thstok thtt_cs\[[expr [llength $proj_parse] + 1]\] = \{";
foreach e [lsort -ascii -nocase $proj_parse] {
  puts $fid "  \{\"[lindex $e 0]\", [lindex $e 1]\},"
} 
puts $fid "  \{NULL, TTCS_UNKNOWN\}\n\};"


puts $fid {

/**
 * CS data table.
 */
}   
puts $fid "const thcsdata thcsdata_table\[[llength $proj_specs]\] = \{";
foreach p $proj_specs {
  if {[lsearch -exact [lindex $p 1] output] > -1} {set o_output true} else {set o_output false}
  if {[lsearch -exact [lindex $p 1] dms] > -1} {set o_dms true; set o_output false} else {set o_dms false}
  if {[lsearch -exact [lindex $p 1] swap] > -1} {set o_swap true; set o_output false} else {set o_swap false}
  set prjspec [lindex $p 3]
  regsub -all {\"} $prjspec {\"} prjspec
  set prjname [lindex $p 4]
  regsub -all {\"} $prjname {\"} prjname
  puts $fid "  \{$o_dms, $o_output, $o_swap, \"[lindex $p 2]\", \"$prjspec\"\, \"$prjname\"\},"
} 
puts $fid "\};"

puts $fid "\nvoid thcs_add_default_transformations() {"
foreach p $proj_transformations {
  puts $fid "thcs_add_cs_trans(\"[escape [lindex $p 0]]\", \"[escape [lindex $p 1]]\", \"[escape [lindex $p 2]]\");"
}
puts $fid "}\n\n\n"
		

close $fid

exit
