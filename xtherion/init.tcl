##
## init.tcl --
##
##     Initialization script.   
##
## Copyright (C) 2002 Stacho Mudrak
## 
## $Date: $
## $RCSfile: $
## $Revision: $
##
## -------------------------------------------------------------------- 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
## --------------------------------------------------------------------

package require BWidget

if {[catch {set imgver [package require Img]}]} {
  set xth(gui,imgfiletypes) {
	   { {Pictures} {.gif .pnm .ppm .xvi} }
	   { {All Files}               * }
	 } 
} else {
  set xth(gui,imgfiletypes) {
	   { {Pictures} {.png .jpeg .jpg .gif .pnm .ppm .xvi} }
	   { {All Files}                                               * }
	 } 
}

# read xtherion.ini file from THERION directory
set idir {}
if {[catch {set idir $env(THERION)}]} {
  if {![catch {set idir $env(HOME)}]} {
    append idir "/.therion"
    if {[string equal $xth(gui,platform) windows]} {
      append idir ";"
    } else {
      append idir ":"
    }
  }
  if {[string equal $xth(gui,platform) windows]} {
    if {[catch {
	append idir [registry get {HKEY_LOCAL_MACHINE\SOFTWARE\Therion} InstallDir]
	}]} {
      append idir "C:/WINDOWS;C:/WINNT;C:/Program files/Therion"
    }
  } else {
    append idir "/etc:/usr/etc:/usr/local/etc"
  }
}
if {[string equal $xth(gui,platform) windows]} {
  set idirs [split $idir ";"]
} else {
  set idirs [split $idir ":"]
}

set xth(idirs) $idirs
foreach idir $xth(idirs) {
  catch {source [file join $idir xtherion.ini]}
}
catch {source xtherion.ini}

# create xth window
wm withdraw .
xth_about_show 0
toplevel $xth(gui,main)
wm withdraw $xth(gui,main)
wm protocol $xth(gui,main) WM_DELETE_WINDOW "xth_exit"
wm title $xth(gui,main) $xth(prj,name)
wm geometry $xth(gui,main) [format "%dx%d+0+0" [lindex $xth(gui,minsize) 0] \
  [lindex $xth(gui,minsize) 1]]
wm minsize $xth(gui,main) [lindex $xth(gui,minsize) 0] \
  [lindex $xth(gui,minsize) 1]
update idletasks
bind $xth(gui,main) <Configure> { 
  catch {xth_app_place $xth(app,active)}
}

set xth(gui,clock) "00:00"

# redefine some public key bindigs
if {$xth(gui,bindinsdel)} {
  bind Text <Delete> { }
}
bind Text <$xth(kb_control)-Key-o> "#"
bind Text <$xth(kb_control)-Key-a> "#"
bind Text <$xth(kb_control)-Key-i> "#"
bind Text <$xth(kb_control)-Key-s> "#"
bind Text <$xth(kb_control)-Key-w> "#"
bind Text <$xth(kb_control)-Key-q> "#"
bind Text <$xth(kb_control)-Key-x> "#"
bind Text <$xth(kb_control)-Key-n> "#"
bind Text <$xth(kb_control)-Key-p> "#"
bind Text <$xth(kb_control)-Key-c> "#"
bind Text <$xth(kb_control)-Key-v> "#"
bind Text <$xth(kb_control)-Key-f> "#"
bind Text <$xth(kb_control)-Key-h> "#"
bind Text <$xth(kb_control)-Key-z> "#"
bind Text <$xth(kb_control)-Key-y> "#"
bind Text <$xth(kb_control)-Key-d> "#"
bind Text <$xth(kb_control)-Key-k> "#"
bind Text <$xth(kb_control)-Key-r> "#"

bind Entry <$xth(kb_control)-Key-d> "#"
bind Entry <$xth(kb_control)-Key-k> "#"

set xth(gui,bind,text_tab) [bind Text <Tab>]
set xth(gui,bind,text_return) [bind Text <Return>]
bind Text <Tab> "#"
bind Text <Return> "#"

if {[string length $xth(gui,initdir)] == 0} {
  if {[catch {
    set xth(gui,initdir) [registry get {HKEY_LOCAL_MACHINE\SOFTWARE\Therion} XTherionDir]
  }]} {
    set xth(gui,initdir) [pwd]
  }
}

proc thememc {tt} {
  set trans [mc $tt]
  if {[string equal $tt $trans]} {
    set trans [lindex $tt 1]
    regsub -all {[\-\:]} $trans { } trans
  }
  return $trans
}

set alllist {}
foreach type $xth(point_types) {
  lappend alllist [list point $type]
}
foreach type $xth(line_types) {
  lappend alllist [list line $type]
}
foreach type $xth(area_types) {
  lappend alllist [list area $type]
}
set tmp [mc "theme all-symbols"]
set alllist [linsert $alllist 0 {theme all-symbols}]
set xth(me,themes) "$alllist $xth(me,themes)"
set xth(me,themes,list) {}
set xth(me,themes,showlist) {}

# process themes
set acttheme all-symbols
foreach itm $xth(me,themes) {
  set key [lindex $itm 0]
  set value [lindex $itm 1]
  switch $key {
    theme {
      lappend xth(me,themes,list) $value
      lappend xth(me,themes,showlist) [thememc $itm]
      set xth(me,themes,$value,point,list) {}
      set xth(me,themes,$value,point,xlist) {}
      set xth(me,themes,$value,line,list) {}
      set xth(me,themes,$value,line,xlist) {}
      set xth(me,themes,$value,area,list) {}
      set xth(me,themes,$value,area,xlist) {}
      set acttheme $value
    }
    point {
      lappend xth(me,themes,$acttheme,point,list) $value
      lappend xth(me,themes,$acttheme,point,xlist) [list [thememc $itm] $value]
    }
    line {
      lappend xth(me,themes,$acttheme,line,list) $value
      lappend xth(me,themes,$acttheme,line,xlist) [list [thememc $itm] $value]
    }
    area {
      lappend xth(me,themes,$acttheme,area,list) $value
      lappend xth(me,themes,$acttheme,area,xlist) [list [thememc $itm] $value]
    }
  }
}

proc xth_me_sortxlist {cl} {
  set cl [lsort -index 0 $cl]
  set hl {}
  set sl {}
  foreach xl $cl {
    lappend sl [lindex $xl 0]
    lappend hl [lindex $xl 1]
  }
  return [list $sl $hl]
}

foreach tm $xth(me,themes,list) {
  foreach itm {point line area} {
    set lists [xth_me_sortxlist $xth(me,themes,$tm,$itm,xlist)]
    set xth(me,themes,$tm,$itm,showlist) [lindex $lists 0]
    set xth(me,themes,$tm,$itm,hidelist) [lindex $lists 1]
    unset xth(me,themes,$tm,$itm,xlist)
  }
}

