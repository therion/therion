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
$hm add command -label "Control..." -underline 0 -font $xth(gui,lfont) \
  -command xth_help_control_show
$hm add command -label "BAC calculator..." -underline 0 -font $xth(gui,lfont) \
  -command xth_bac_init
$hm add command -label "About..." -underline 0 -font $xth(gui,lfont) \
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
  Button $ff.sb -text "Search" -anchor center -font $xth(gui,lfont) \
  -command xth_help_control_search -width 8
  Button $ff.cb -text "Close" -anchor center -font $xth(gui,lfont) \
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
  $txb.txt insert end {MAP EDITOR SHORTCUTS
  
General shortcuts
 * Ctrl+Z = undo
 * Ctrl+Y = redo
 * F9 = compile current project
 * to select object in the listbox using keyboard:
    switch using "Tab" into desired listbox;
    move with underlined cursor to desired object;
    press "Space"

Drawing area and background images
 * RightClick = scroll drawing area
 * Double RightClick on the image = move the image

Inserting point
 * Ctrl+P = switch to `insert point' mode
 * LeftClick = insert point at given position
 * Ctrl+LeftClick = insert point very close to existing point (normally it
    will be inserted right above the closest point)
 * Esc = escape from the `inset point' mode

Editing point
 * LeftClick + drag = move point
 * Ctrl+LeftClick + drag = move point close to the existing
    point (normally it is moved right above closest existing point)
 * LeftClick + drag on point arrows = change point orientation or
    sizes (according to given switches in Point cotrol panel)

Inserting line
 * Crtl+L = insert new line and enter an `insert line point' mode
 * LeftClick = insert line point (without control points)
 * Ctrl+LeftClick = insert line point very close to existing point 
    (normally it's inserted right above closest existing point)
 * LeftClick + drag = insert line point (with control points)
 * hold Ctrl while dragging = fix the distance of previous control point
 * LeftClick + drag on the control point = move its position
 * RightClick on one of the previous points = selects the previous point while 
    in insert mode (useful if you want to change also the direction of
    previous control point)
 * Esc or LeftClick on the last point = end the line insertion
 * LeftClick on the first line point = close the line and end line insertion

Editing line
 * LeftClick + drag = move line point
 * Ctrl+LeftClick + drag = move line point close to the existing
    point (normally it is moved right above closest existing point)
 * LeftClick on control point + drag = move control point

Adding line point
 * select the point before which you want to insert points;
    insert required points;
    press Esc or left-click on the point you selected at the begining
  
Deleting line point
 * select the point you want to delete;
    press "Edit line" > "Delete point" in the Line control panel
    
Splitting line
 * select the point at which you want to split the line;
    press "Edit line" > "Split line" in the Line control panel
    
Inserting area
 * press "Edit" > "Insert area" to switch to the "insert area border" mode
 * RightClick on the lines, that suround desired area
 * Esc to finish area border lines insertion

Editing area
 * select area you want to edit
 * pres "Insert" in the area control to insert other border lines
    at current cursor position
 * pres "Insert ID" to insert border with given ID at current cursor position
 * pres "Delete" to remove selected area border line
    
Selecting an existing object
 * LeftClick = select object on the top
 * RightClick = select object right below the top object (useful when several
    points lie above each other)} 
    
     
  $txb.txt configure -state disabled
 
}



