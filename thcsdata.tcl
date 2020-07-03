#! /usr/bin/tclsh
set proj4v6file "proj.db-no-sqlite3"
catch {
  package require sqlite3
  set proj4v6file "proj.db"
}

set pkg_cnf "pkg-config"
if {$argc == 1} {
  set pkg_cnf [lindex $argv 0]$pkg_cnf
}

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



proc load_proj_init_file_for_labels {fn shortcut} {
  global projlabels
  set projlabels($shortcut) "map<int, const char *> $shortcut\_labels = {\n"
  set active_comment {}
  set active_name {}
  set active_spec { }
  set fid [open $fn r]
  while {![eof $fid]} {
    gets $fid line
    if {[regexp {^\<metadata\> } $line]} {continue}
    regexp {^\s*\#\s*(.*)$} $line dum active_comment
    regsub -all {"} $active_comment active_comment
    regsub {\s*\#.*$} $line {} line
    if {[string length $active_name] == 0} {
      if {[regexp {^\s*\<(\S+)\>} $line dum id]} {
	set active_name [expr int($id)]
	regsub {^\s*\<\S+\>\s*} $line {} line
      }
    }
    if {[string length $active_name] > 0} {
      append active_spec $line
      if {[regexp {\s+\<\>} $line]} {
	append projlabels($shortcut) "  {$active_name, \"$active_comment\"},\n"
	set active_name {}
	set active_spec { }
      }
    }
  }
  close $fid
  append projlabels($shortcut) "};\n\n\n"
}


# <identifiers> <options> <libPROJ definition> <.PRJ definition>
set proj_specs {
  {{long-lat} {dms} "+init=epsg:4326" {GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]]} {}}
  {{lat-long} {dms swap} "+init=epsg:4326 " {} {}}
  {{ijtsk}   {output} "+proj=krovak +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {PROJCS["S-JTSK_Krovak_East_North",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",-1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",90.0],UNIT["Meter",1.0]]} {}}
  {{ijtsk03} {output} "+proj=krovak +ellps=bessel +lat_0=49.5 +lon_0=24.833333333333333333 +k=0.9999 +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {PROJCS["S-JTSK_Krovak",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",0.0],UNIT["Meter",1.0]]} {}}
  {{s-merc} {output} "+init=epsg:3857" {} {}}
  {{eur79z30} {output} "+proj=utm +zone=30 +ellps=intl +towgs84=-86,-98,-119,0,0,0,0 +no_defs" {} {}}
}

if {[string equal [exec $pkg_cnf proj --modversion] "7.1.0"]} {  # TODO: proj 7.1.0 workaround
  lappend proj_specs {{jtsk}   {} "+proj=krovak +axis=wsu +ellps=bessel +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {PROJCS["S-JTSK_Krovak",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",0.0],UNIT["Meter",1.0]]} {}}
  lappend proj_specs {{jtsk03} {} "+proj=krovak +axis=wsu +ellps=bessel +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {} {}}
} else {
  lappend proj_specs {{jtsk}   {} "+proj=krovak +czech +ellps=bessel +towgs84=570.8285,85.6769,462.842,4.9984,1.5867,5.2611,3.5623" {PROJCS["S-JTSK_Krovak",GEOGCS["GCS_S_JTSK",DATUM["D_S_JTSK",SPHEROID["Bessel_1841",6377397.155,299.1528128]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Krovak"],PARAMETER["False_Easting",0.0],PARAMETER["False_Northing",0.0],PARAMETER["Pseudo_Standard_Parallel_1",78.5],PARAMETER["Scale_Factor",0.9999],PARAMETER["Azimuth",30.28813975277778],PARAMETER["Longitude_Of_Center",24.83333333333333],PARAMETER["Latitude_Of_Center",49.5],PARAMETER["X_Scale",1.0],PARAMETER["Y_Scale",1.0],PARAMETER["XY_Plane_Rotation",0.0],UNIT["Meter",1.0]]} {}}
  lappend proj_specs {{jtsk03} {} "+proj=krovak +czech +ellps=bessel +towgs84=485.021,169.465,483.839,7.786342,4.397554,4.102655,0" {} {}}
}

#  {{OSGB:ST} {output} {+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=100000 +y_0=-200000 +ellps=airy +datum=OSGB36 +units=m +no_defs} {} {}}
#  {{OSGB:SN} {output} {+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=200000 +y_0=-300000 +ellps=airy +datum=OSGB36 +units=m +no_defs} {} {}}

# add UTM projections
for {set zone 1} {$zone <= 60} {incr zone} {
  lappend proj_specs [list [format "utm%dn utm%d" $zone $zone $zone $zone $zone] {output} [format "+init=epsg:%d" [expr $zone+32600]] [format {PROJCS["WGS_1984_UTM_Zone_%dN",GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["False_Easting",500000.0],PARAMETER["False_Northing",0.0],PARAMETER["Central_Meridian",%.1f],PARAMETER["Scale_Factor",0.9996],PARAMETER["Latitude_Of_Origin",0.0],UNIT["Meter",1.0]]} $zone [expr double($zone * 6 - 183)]] [format "WGS84 / UTM zone %dN" $zone]]
  lappend proj_specs [list [format "utm%ds" $zone $zone $zone] {output} [format "+init=epsg:%d" [expr $zone+32700]] [format {PROJCS["WGS_1984_UTM_Zone_%dS",GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]],PROJECTION["Transverse_Mercator"],PARAMETER["False_Easting",500000.0],PARAMETER["False_Northing",10000000.0],PARAMETER["Central_Meridian",%.1f],PARAMETER["Scale_Factor",0.9996],PARAMETER["Latitude_Of_Origin",0.0],UNIT["Meter",1.0]]} $zone [expr double($zone * 6 - 183)]] [format "WGS84 / UTM zone %dS" $zone]]
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

#load_proj_init_file extern/proj4/nad/epsg epsg
#load_proj_init_file extern/proj4/nad/esri esri
set prefix [exec $pkg_cnf proj --variable=prefix]
set projlabels(esri) "\nmap<int, const char *> esri_labels = {};\n\n"
set projlabels(epsg) "\nmap<int, const char *> epsg_labels = {};\n\n"
if {[file exists $prefix/share/proj/$proj4v6file]} {
  sqlite3 projdb $prefix/share/proj/$proj4v6file
  set projlabels(esri) "\n\nmap<int, const char *> esri_labels = {\n"
  set projlabels(epsg) "\n\nmap<int, const char *> epsg_labels = {\n"
  projdb eval {select auth_name, code, name from projected_crs where auth_name in ('EPSG', 'ESRI') order by cast(code as integer);} pdef {
    set name $pdef(name)
    if {![regexp {\s+} $name]} {
      set name [regsub -all "\\_" $name " "]
    }
    set name [regsub -all "\\\"" $name "'"]
    append projlabels([string tolower $pdef(auth_name)]) "\t{$pdef(code), \"$name\"},\n"
  }
  append projlabels(esri) "};\n\n"
  append projlabels(epsg) "};\n\n"
} elseif {[file exists $prefix/share/proj/epsg]} {
  load_proj_init_file_for_labels $prefix/share/proj/epsg epsg
  load_proj_init_file_for_labels $prefix/share/proj/esri esri
} else {
  puts "No PROJ system definitions found."
}

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
 
#include <map>
using namespace std;

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
extern map<int, const char *> esri_labels;
extern map<int, const char *> epsg_labels;
}


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

puts $fid $projlabels(esri)
puts $fid $projlabels(epsg)


close $fid

exit
