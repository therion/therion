##
## mv.tcl --
##
##     Model viewer module.
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

set xthmvw {}

if {![catch {set tomver [package require Tom]}]} {

xth_about_status [mc "loading model viewer..."]

namespace import ::GL::*

xth_app_create mv [mc "Model viewer"] 
xth_ctrl_add mv cam [mc "Camera"]
xth_ctrl_add mv mod [mc "Model"]
xth_ctrl_finish mv

# create config editor
set xthmvw $xth(gui,mv).af.apps.tom
set xthmvv(model,surftrans) 1
set xthmvv(model,headlight) 1
set xthmvv(model,lightinpos) 1

tom $xthmvw -doublebuffer 1 -depth 1

# pack editor and log widow
grid columnconf $xth(gui,mv).af.apps 0 -weight 1
grid rowconf $xth(gui,mv).af.apps 0 -weight 1
grid $xthmvw -column 0 -row 0 -sticky news

$xth(mv,menu,file) add command -label [mc "Open"] -underline 0 \
  -accelerator "$xth(gui,controlk)-o" -state normal \
  -font $xth(gui,lfont) -command {
    xth_mv_open_file {}
  }

$xth(mv,menu,file) add command -label [mc "Reload"] -underline 0 \
  -accelerator "$xth(gui,controlk)-r" -state normal \
  -font $xth(gui,lfont) -command {
    xth_mv_reload_file
  }

set f $xth(ctrl,mv,cam)
set xthmvv(ctrlframe) $f

menubutton $f.fb -text [mc "facing"] -anchor e -font $xth(gui,lfont) \
  -indicatoron true -menu $f.fb.m -width 4
Label $f.fl -textvariable xthmvv(cam,facing) -anchor e -font $xth(gui,lfont) -width 4 -padx 30
menu $f.fb.m -tearoff 0 -font $xth(gui,lfont)
$f.fb.m add command -label [mc "north"] -command {set xthmvv(cam,facing) 0.0; xth_mv_update}
$f.fb.m add command -label [mc "east"] -command {set xthmvv(cam,facing) 90.0; xth_mv_update}
$f.fb.m add command -label [mc "west"] -command {set xthmvv(cam,facing) 180.0; xth_mv_update}
$f.fb.m add command -label [mc "south"] -command {set xthmvv(cam,facing) 270.0; xth_mv_update}
$f.fb.m add separator
$f.fb.m add command -label [mc "reset"] -command {xth_mv_init_camera; xth_mv_update}
xth_status_bar mv $f.fb [mc "Set facing angle."]
xth_status_bar mv $f.fl [mc "Facing angle."]

checkbutton $f.fa -text [mc "auto rotate"] -anchor w -font $xth(gui,lfont) \
  -variable xthmvv(cam,autorotate) -command {
    if {$xthmvv(cam,autorotate)} {
      set xthmvv(autorotate,dir) [expr -1 * $xthmvv(autorotate,dir)]
    }
    xth_mv_camera_autorotate
  } -width 4
xth_status_bar mv $f.fa [mc "Turn on/off camera auto rotation"]

menubutton $f.pb -text [mc "profile"] -anchor e -font $xth(gui,lfont) \
  -indicatoron true -menu $f.pb.m -width 4
Label $f.pl -textvariable xthmvv(cam,profile) -anchor e -font $xth(gui,lfont) -width 4 -padx 30
menu $f.pb.m -tearoff 0 -font $xth(gui,lfont)
$f.pb.m add command -label [mc "top"] -command {set xthmvv(cam,profile) 90.0; xth_mv_update}
$f.pb.m add command -label [mc "side"] -command {set xthmvv(cam,profile) 0.0; xth_mv_update}
$f.pb.m add command -label [mc "bottom"] -command {set xthmvv(cam,profile) -90.0; xth_mv_update}
$f.pb.m add separator
$f.pb.m add command -label [mc "reset"] -command {xth_mv_init_camera; xth_mv_update}
xth_status_bar mv $f.pb [mc "Set profile angle."]
xth_status_bar mv $f.pl [mc "Profile angle."]
scale $f.pscale -orient horiz -showvalue 0 -from -90.0 -to 90.0 \
  -resolution 1.0 -variable xthmvv(cam,profile)
  
grid columnconf $f 0 -weight 1
grid columnconf $f 1 -weight 1
grid $f.fb -row 0 -column 0 -sticky news
grid $f.fl -row 0 -column 1 -sticky news
grid $f.fa -row 1 -column 0 -columnspan 2 -sticky news
grid $f.pb -row 2 -column 0 -sticky news
grid $f.pl -row 2 -column 1 -sticky news
grid $f.pscale -row 3 -column 0 -columnspan 2 -sticky news


set f $xth(ctrl,mv,mod)
set xthmvv(modelframe) $f

checkbutton $f.st -text [mc "surface transparency"] -anchor w -font $xth(gui,lfont) \
  -variable xthmvv(model,surftrans) -command xth_mv_reload_file -width 4
xth_status_bar mv $f.st [mc "Turn on/off surface transparency."]

checkbutton $f.hl -text [mc "light follows camera"] -anchor w -font $xth(gui,lfont) \
  -variable xthmvv(model,headlight) -command xth_mv_update -width 4
xth_status_bar mv $f.hl [mc "Fix light source to camera position."]

grid columnconf $f 0 -weight 1
grid columnconf $f 1 -weight 1
grid $f.st -row 0 -column 0 -columnspan 2 -sticky news
grid $f.hl -row 1 -column 0 -columnspan 2 -sticky news

xth_ctrl_maximize mv cam
xth_ctrl_maximize mv mod

xth_status_bar_status mv [mc "Model viewer is not active. To activate it, open some model file."]

# IF, CI SA MV NEKONA
} 


