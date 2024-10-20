##
## help.tcl --
##
##     Help menu and its functions.  
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


set hm "$xth(gui,main).hmenu"
set xth(gui,menu,help) $hm

menu $hm -tearoff 0
$hm add command -label [mc "Control..."] -underline 0 -font $xth(gui,lfont) \
  -command xth_help_control_show
$hm add command -label [mc "BAC calculator..."] -underline 0 -font $xth(gui,lfont) \
  -command xth_bac_init
$hm add command -label [mc "Calibrate bitmap (v5.3–6.2 files)..."] -underline 0 -font $xth(gui,lfont) \
  -command xth_calibrate_bitmap
$hm add command -label [mc "About..."] -underline 0 -font $xth(gui,lfont) \
  -command {
    xth_about_show 1
    xth_about_status $xth(prj,title)
  }

proc xth_help_control_show {} {
  global xth
  if {[winfo exists $xth(gui,main).help_control]} {
    focus $xth(gui,main).help_control
  } else {
    xth_help_control_init
  }
}

proc xth_help_control_search {} {
}

proc xth_help_control_init {} {

  global xth
  
  set f $xth(gui,main).help_control
  toplevel $f
  wm transient $f $xth(gui,main)
  wm title $f "Xtherion control"

  set ff $f.lf
  frame $ff
  Entry $ff.se -font $xth(gui,lfont)
  Button $ff.sb -text [mc "Search"] -anchor center -font $xth(gui,lfont) \
  -command xth_help_control_search -width 8
  Button $ff.cb -text [mc "Close"] -anchor center -font $xth(gui,lfont) \
  -command "destroy $f" -width 8
  grid columnconf $ff 2 -weight 1
#  grid columnconf $ff 0 -weight 1
#  grid columnconf $ff 1 -weight 0
#  grid columnconf $ff 1 -weight 0
#  grid $ff.se -column 0 -row 0 -sticky news
#  grid $ff.sb -column 1 -row 0 -sticky news
  grid $ff.cb -column 2 -row 0 -sticky news


  set txb $f.tf
  frame $txb
  text $txb.txt -wrap none -font $xth(gui,lfont) \
    -relief sunken -state disabled \
    -selectbackground $xth(gui,ecolorselbg) \
    -selectforeground $xth(gui,ecolorselfg) \
    -selectborderwidth 0 \
    -yscrollcommand "$txb.sv set" \
    -xscrollcommand "$txb.sh set" 
  scrollbar $txb.sv -orient vert  -command "$txb.txt yview" \
    -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
  scrollbar $txb.sh -orient horiz  -command "$txb.txt xview" \
    -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
  grid columnconf $txb 0 -weight 1
  grid rowconf $txb 0 -weight 1
  grid $txb.txt -column 0 -row 0 -sticky news
  grid $txb.sv -column 1 -row 0 -sticky news
  grid $txb.sh -column 0 -row 1 -sticky news

  grid rowconf $f 0 -weight 1
  grid columnconf $f 0 -weight 1
  grid rowconf $f 1 -weight 0
  grid $txb -column 0 -row 0 -sticky news
  grid $ff -column 0 -row 1 -sticky news
  
 
  set sw [winfo screenwidth .]
  set sh [winfo screenheight .]
  update idletasks
  wm geometry $f +0+0
  wm geometry $f 640x480
  wm minsize $f 320 240
  set x [expr {($sw - [winfo width $f])/2}]
  set y [expr {($sh - [winfo height $f])/2}]
  wm geometry $f +$x+$y
  update idletasks
  
  $txb.txt configure -state normal
  set hlptxt [ mc "xtherion_help_control" ]  
  if {[string length $hlptxt] < 100} {
    set tmploc ::msgcat::mclocale
    ::msgcat::mclocale en
    set hlptxt [ mc "xtherion_help_control" ]  
    ::msgcat::mclocale $tmploc
  }
  $txb.txt insert end $hlptxt
    
     
  $txb.txt configure -state disabled
 
}



