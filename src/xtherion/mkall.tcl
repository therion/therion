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
    set oid [open "../../bin/xtherion" w]
    puts $oid "#!/usr/bin/wish"
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
    catch {exec chmod 775 ../../bin/xtherion}
    catch {exec chmod 775 svxedit}
  }
  windows {
  }
}

exit 0