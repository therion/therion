##
## about.tcl --
##
##     Application module.   
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


set xth(about,image_id) [image create photo -data $xth(about,image_data)]
set xth(about,infotime) 2500

proc xth_about_status {str} {
    global xth
    set xth(about,status) "$str"
    update idletasks
}

proc xth_about_show {btnid} {
    global xth
    if {[winfo exists $xth(gui,about)]} xth_about_hide
    xth_about_status ""
    set w $xth(gui,about)
    toplevel $w -relief raised -bg black -bd 3 -cursor $xth(gui,cursor)
    wm transient $w
    wm withdraw $w
    set sw [winfo screenwidth .]
    set sh [winfo screenheight .]
    wm overrideredirect $w 1
    label $w.image -bd 0 -relief sunken -background black -fg white -image $xth(about,image_id)
    pack $w.image -side top -expand 1 -fill both
    label $w.status -relief flat -background black -foreground white \
    	-textvariable xth(about,status) -font $xth(gui,lfont) -anchor center
    pack $w.status -side top -expand 1 -fill both
    label $w.i1 -bd 0 -relief sunken -background black -fg white -text "xtherion\n$xth(about,ver)" \
      -font $xth(gui,lfont) -anchor center
    pack $w.i1 -side top -expand 1 -fill both -pady 5
    if {$btnid} {
      button $w.close -text "Close" -font $xth(gui,lfont) -anchor center \
        -command xth_about_hide -width 5
      pack $w.close -side top -fill none -anchor center -pady 5
      focus $w.close
    }
    wm geometry $xth(gui,about) -$sw-$sh
    wm deiconify $xth(gui,about)
    update idletasks
    set x [expr {($sw - [winfo width $xth(gui,about)])/2}]
    set y [expr {($sh - [winfo height $xth(gui,about)])/2}]
    wm geometry $xth(gui,about) +$x+$y
    $w configure -bg black
    $w.image configure -image $xth(about,image_id)
    $w.i1 configure -text "xtherion $xth(about,ver)\n\u00A9 2002-2017 Stacho Mudrak"
    update idletasks
}


proc xth_about_hide {} {
  global xth
  destroy $xth(gui,about)
  focus $xth(gui,main)
}

