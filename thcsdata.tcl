#! /usr/bin/tclsh

proc load_proj_init_file_add {name spec desc} {
  global proj_specs
  regsub -all {\+?no\_defs} $spec {} spec
  regsub -all {\s+\<\>} $spec  { } spec
  regsub -all {\s+\+?} $spec { } spec
  regsub {\s+$} $spec {} spec
  regsub -all {(\s+)} $spec { +} spec
  regsub {^\s+} $spec {} spec
  regsub {longlat} $spec {latlong} spec
  if {[regexp {proj\=latlong} $spec]} {
    set options {dms}
  } else {
    set options {output}
  }
  lappend proj_specs [list $name $options $spec {} $desc]
}

proc load_proj_init_file {fn shortcut} {
  set active_comment {}
  set active_name {}
  set active_spec { }
  set fid [open $fn r]
  while {![eof $fid]} {
    gets $fid line
    regexp {^\s*\#\s*(.*)$} $line dum active_comment
    regsub {\s*\#.*$} $line {} line
    if {[string length $active_name] == 0} {
      if {[regexp {^\s*\<(\S+)\>} $line dum id]} {
	set active_name [string tolower "$shortcut:$id"]
	regsub {^\s*\<\S+\>\s*} $line {} line
      }
    }
    if {[string length $active_name] > 0} {
      append active_spec $line
      if {[regexp {\s+\<\>} $line]} {
	load_proj_init_file_add $active_name $active_spec $active_comment
	set active_name {}
	set active_spec { }
      }
    }
  }
  close $fid
  load_proj_init_file_add $active_name $active_spec $active_comment
}

# <identifiers> <options> <libPROJ definition> <.PRJ definition>
set proj_specs {
  {{long-lat} {dms} "+proj=latlong  +datum=WGS84" {GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]]} {}}
  {{lat-long} {dms swap} "+proj=latlong +datum=WGS84" {} {}}
  {{jtsk}   {} "+proj=krovak +czech +ellps=bessel +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {PROJCS["S-JTSK_Krovak",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",0.0],UNIT["Meter",1.0]]} {}}
  {{jtsk03} {} "+proj=krovak +czech +ellps=bessel +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {} {}}
  {{ijtsk}   {output} "+proj=krovak +ellps=bessel +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {PROJCS["S-JTSK_Krovak_East_North",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",-1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",90.0],UNIT["Meter",1.0]]} {}}
  {{ijtsk03} {output} "+proj=krovak +ellps=bessel +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {PROJCS["S-JTSK_Krovak",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",0.0],UNIT["Meter",1.0]]} {}}
  {{s-merc} {output} {+proj=merc +latts=0 +lon0=0 +k=1 +x0=0 +y0=0 +a=6378137 +b=6378137 +units=m +nadgrids=\\@null +no_defs} {} {}}
  {{eur79z30} {output} "+proj=utm +zone=30 +ellps=intl +towgs84=-86,-98,-119,0,0,0,0 +no_defs" {} {}}
}

#  {{OSGB:ST} {output} {+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=100000 +y_0=-200000 +ellps=airy +datum=OSGB36 +units=m +no_defs} {} {}}
#  {{OSGB:SN} {output} {+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=200000 +y_0=-300000 +ellps=airy +datum=OSGB36 +units=m +no_defs} {} {}}

# add UTM projections
for {set zone 1} {$zone <= 60} {incr zone} {
  lappend proj_specs [list [format "utm%dn utm%d" $zone $zone $zone $zone $zone] {output} "+proj=utm +zone=$zone +ellps=WGS84 +datum=WGS84 +units=m" [format {PROJCS["WGS_1984_UTM_Zone_%dN",GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["False_Easting",500000.0],PARAMETER["False_Northing",0.0],PARAMETER["Central_Meridian",%.1f],PARAMETER["Scale_Factor",0.9996],PARAMETER["Latitude_Of_Origin",0.0],UNIT["Meter",1.0]]} $zone [expr double($zone * 6 - 183)]] [format "WGS84 / UTM zone %dN" $zone]]
  lappend proj_specs [list [format "utm%ds" $zone $zone $zone] {output} "+proj=utm +zone=$zone +south +ellps=WGS84 +datum=WGS84 +units=m" [format {PROJCS["WGS_1984_UTM_Zone_%dS",GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["False_Easting",500000.0],PARAMETER["False_Northing",10000000.0],PARAMETER["Central_Meridian",%.1f],PARAMETER["Scale_Factor",0.9996],PARAMETER["Latitude_Of_Origin",0.0],UNIT["Meter",1.0]]} $zone [expr double($zone * 6 - 183)]] [format "WGS84 / UTM zone %dS" $zone]]
}

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

load_proj_init_file extern/proj4/nad/epsg epsg
load_proj_init_file extern/proj4/nad/esri esri

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
 
#include "thparse.h"
 
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
}

puts $fid {

/**
 * CS parsing table.
 */
}   
puts $fid "const thstok thtt_cs\[[expr [llength $proj_parse] + 1]\] = \{";
foreach e [lsort -ascii $proj_parse] {
  puts $fid "  \{\"[lindex $e 0]\", [lindex $e 1]\},"
} 
puts $fid "  \{NULL, TTCS_UNKNOWN\}\n\};"

puts $fid {

/**
 * CS names.
 */
}   
puts $fid "const thstok thtt_csnames\[[expr [llength $proj_parse] + 1]\] = \{";
foreach e [lsort -ascii $proj_parse] {
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


close $fid

exit
