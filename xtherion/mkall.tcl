proc append_file {fnm} {  
  global oid
  set fid [open $fnm r]
  while {![eof $fid]} {
    gets $fid ln
    if {![regexp {^\s*\#\#} $ln]} {
      puts $oid $ln
    }
  }
  puts $oid "\n\n\n\n"
  close $fid
}

proc append_all_file {fnm} {  
  global oid
  set fid [open $fnm r]
  fcopy $fid $oid
  puts $oid "\n\n\n\n"
  close $fid
}


set what [lindex $argv 0]
switch $what {
  survex {
    set oid [open "svxedit" w]
    puts $oid "#!/usr/bin/wish"
    puts $oid {##
## svxedit --
##
##     Survex svx files editor.
##
## Copyright (C) 2002 Stacho Mudrak
## 
##}
  }
  default {
    case $tcl_platform(platform) {
      windows {
        set oid [open "xtherion.tcl" w]
      }
      default {
        set oid [open "xtherion" w]
        puts $oid "#!/bin/sh"
        puts $oid "# the next line restarts with wish \\"
        puts $oid "exec wish \"\$0\" \"\$@\""
      }
    }
    puts $oid {##
## xtherion --
##
##     Therion user interface.
##
## Copyright (C) 2002 Stacho Mudrak
## 
##}
    set what xtherion
  }
}

append_all_file licence.tcl
puts $oid "\n\n\nset xth(debug) 0"

catch {
    set fid [open ../thversion.h r]
    gets $fid verl
    regexp {\d+\.\d+\.\d+} $verl vver
    close $fid
    set fid [open ver.tcl w]
    puts $fid "set xth(about,ver) $vver"
    close $fid
}

set fid [open source.tcl r]

set toappend 0
while {![eof $fid]} {
  gets $fid fl
  if {[regexp {^\s*\#\@(\S+)\s*$} $fl dum current]} {
    if {[string equal -nocase $what $current]} {
      set toappend 1
    } elseif {[string equal -nocase "ALL" $current]} {
      set toappend 1
    } else {
      set toappend 0
    }
  }
  if {![regexp {^\s*\#} $fl]} {
    if {[regexp {^\s*source\s+(\S+)} $fl dum afname] && $toappend} {
    	append_file $afname
    }
  }
}
close $fid
close $oid

case $tcl_platform(platform) {
  unix {
    catch {exec chmod 775 xtherion}
    catch {exec chmod 775 svxedit}
  }
  windows {
  }
}

exit 0