##
## me_imgs.tcl --
##
##     Map editor images module.   
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

set xth(gui,openxp) 0
set xth(me,imgs,xviid) 0

proc xth_me_image_update_position {} {
  global xth 
  set iidx [lindex [$xth(ctrl,me,images).il.ilbox curselection] 0]
  set imgx [lindex $xth(me,imgs,xlist) $iidx]
  $xth(ctrl,me,images).ic.posln configure -text [format "%.0f:%.0f" \
    [lindex $xth(me,imgs,$imgx,position) 0] \
    [lindex $xth(me,imgs,$imgx,position) 1]]
  set xth(ctrl,me,images,posx) [format "%.0f" [lindex $xth(me,imgs,$imgx,position) 0]]
  set xth(ctrl,me,images,posy) [format "%.0f" [lindex $xth(me,imgs,$imgx,position) 1]]
  update idletasks
}

proc xth_me_image_update_list {} {
  global xth
  set xth(me,imgs,list) {}
  foreach imgx $xth(me,imgs,xlist) {
    if {$xth(me,imgs,$imgx,XVI)} {
      set imgsize "(XVI)"
    } elseif {[string length $xth(me,imgs,$imgx,image)] > 0} {
      set imgw [image width $xth(me,imgs,$imgx,image)]
      set imgh [image height $xth(me,imgs,$imgx,image)]
      set imgsize "($imgw x $imgh)"
    } else {
      set imgsize "(unknown)"
    }
    lappend xth(me,imgs,list) "$xth(me,imgs,$imgx,name) $imgsize"
  }
}


proc xth_me_image_move_to {} {
  global xth
  xth_me_cmds_update {}
  if {$xth(me,nimgs) < 1} {
    return;
  }
  set iidx [lindex [$xth(ctrl,me,images).il.ilbox curselection] 0]
  set imgx [lindex $xth(me,imgs,xlist) $iidx]
  xth_me_unredo_action [mc "moving image"] \
    "xth_me_image_move $imgx $xth(me,imgs,$imgx,position); xth_me_image_update_position" \
    "xth_me_image_move $imgx $xth(ctrl,me,images,posx) $xth(ctrl,me,images,posy); xth_me_image_update_position"
  xth_me_image_move $imgx $xth(ctrl,me,images,posx) $xth(ctrl,me,images,posy)
  xth_me_image_update_position
}



proc xth_me_image_move {imgx xx yy} {
  # zisti si aktualne suradnice
  global xth
  if {[catch {expr $xx}]} {
    set xx [lindex $xth(me,imgs,$imgx,position) 0]
  }
  if {[catch {expr $yy}]} {
    set yy [lindex $xth(me,imgs,$imgx,position) 1]
  }
  set xth(me,imgs,$imgx,position) [list $xx $yy]
  xth_me_image_redraw $imgx
}


proc xth_me_image_toggle_vsb {iidx} {
  global xth
  if {$xth(me,nimgs) < 1} {
    return
  }
  set isel [$xth(ctrl,me,images).il.ilbox curselection]
  if {[llength $isel] < 1} {
    return
  }
  if {[string length $iidx] < 1} {
    set iidx [lindex $isel 0]
  }
  set imgx [lindex $xth(me,imgs,xlist) $iidx]

  xth_me_unredo_action [mc "toggle image visibility"] \
    "xth_me_image_toggle_vsb $iidx" "xth_me_image_toggle_vsb $iidx"

  switch $xth(me,imgs,$imgx,vsb) {
    0 {
      set xth(me,imgs,$imgx,vsb) 1
      $xth(me,can) itemconfigure $xth(me,imgs,$imgx,image) -state normal
      xth_me_image_rescan $imgx
      xth_me_image_redraw $imgx
      set xth(ctrl,me,images,vis) 1
    }
    1 {
      set xth(me,imgs,$imgx,vsb) 0
      $xth(me,can) itemconfigure $xth(me,imgs,$imgx,image) -state hidden
      set xth(ctrl,me,images,vis) 0
    }
    default {
      set xth(ctrl,me,images,vis) 0
    }
  }
}


proc xth_me_image_set_gamma {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
    return
  }
  if {$xth(me,imgs,$imgx,XVI)} {
    return
  }
  set ng $xth(me,imgs,$imgx,gamma)
  xth_status_bar_push me
  set totalsi [llength $xth(me,imgs,$imgx,subimgs)]
  set csi 0
  xth_status_bar_status me [format "Correcting image gamma (%s) ..." $xth(me,imgs,$imgx,name)]
  xth_me_progbar_show $totalsi
  $xth(me,imgs,$imgx,image) configure -gamma $ng
  foreach imgl $xth(me,imgs,$imgx,subimgs) {
    set dsti [lindex $imgl 0]
    incr csi
    xth_me_progbar_prog $csi
    $dsti configure -gamma $ng
  }
  xth_me_progbar_hide
  xth_status_bar_pop me
}

if {$xth(gui,me,nozoom)} {

proc xth_me_images_rescandraw {} {
  global xth
  if {($xth(me,zoom) > 100) && $xth(gui,me,nozoom)} {
    foreach imgx $xth(me,imgs,xlist) {
      xth_me_image_redraw $imgx
    }
  }
}

proc xth_me_image_redraw {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
    return
  }
  if {$xth(me,imgs,$imgx,XVI)} {
    xth_me_imgs_xvi_redraw $imgx
    return
  }
  set totalsi [llength $xth(me,imgs,$imgx,subimgs)]
  set csi 0
  set x [lindex $xth(me,imgs,$imgx,position) 0]
  set y [lindex $xth(me,imgs,$imgx,position) 1]
  set w [image width $xth(me,imgs,$imgx,image)]
  set h [image height $xth(me,imgs,$imgx,image)]
  # ak je zoom 100 - nastavi image na source image
  # a kasle na ostatne
  if {$xth(me,zoom) <= 100} {
    foreach imgl $xth(me,imgs,$imgx,subimgs) {
      incr csi
      $xth(me,can) coords [lindex $imgl 1] \
        [xth_me_real2canx [expr $x + [lindex $imgl 2]]] \
        [xth_me_real2cany [expr $y - [lindex $imgl 3]]]
    }
  } else {
    # najde si suradnice z obrazka, ktore su viditelne
    set imgl [lindex $xth(me,imgs,$imgx,subimgs) 0]
    set cminx [winfo x $xth(me,can)]
    set cminy [winfo y $xth(me,can)]
    set cmaxx [expr $cminx + [winfo width $xth(me,can)]]
    set cmaxy [expr $cminy + [winfo height $xth(me,can)]]
    set sx [xth_me_can2realx [$xth(me,can) canvasx $cminx]]
    set sw [expr [xth_me_can2realx [$xth(me,can) canvasx $cmaxx]] - $sx]
    set sy [xth_me_can2realy [$xth(me,can) canvasy $cminy]]
    set sh [expr $sy - [xth_me_can2realy [$xth(me,can) canvasy $cmaxy]]]

    # ak je nieco viditelne - tak to zobrazi
    set vfx [expr round($sx - $x)]
    set vfy [expr round($y - $sy)] 
    set vtx [expr round($vfx + $sw)] 
    set vty [expr round($vfy + $sh)]
    if {$vfx < 0} {set vfx 0}
    if {$vfy < 0} {set vfy 0}
    if {$vtx > $w} {set vtx $w}
    if {$vty > $h} {set vty $h}
    
    #puts "$vfx $vfy $vtx $vty"
    if {($vtx <= 0) || ($vty <= 0) || 
        ($vfx >= $w) || ($vfy >= $h) ||
        ($vtx <= $vfx) || ($vty <= $vfy)} {
      # nezobrazime nic
      $xth(me,can) itemconfigure [lindex $imgl 1] -image {}
    } else {
      # zobrazime vyrez
      set dsti [lindex $imgl 0]
      $dsti copy $xth(me,imgs,$imgx,image) -zoom [expr $xth(me,zoom) / 100] -shrink \
        -from $vfx $vfy $vtx $vty
      $xth(me,can) itemconfigure [lindex $imgl 1] -image $dsti
      $xth(me,can) coords [lindex $imgl 1] \
        [xth_me_real2canx [expr $x + $vfx]] \
        [xth_me_real2cany [expr $y - $vfy]]
    }
  }
  update idletasks
}

proc xth_me_image_rescan {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
    return
  }
  if {$xth(me,imgs,$imgx,XVI)} {
    return
  }
  set srci $xth(me,imgs,$imgx,image)
  xth_status_bar_push me
  set origgamma [$srci cget -gamma]
  $srci configure -gamma 1.0
  set totalsi [llength $xth(me,imgs,$imgx,subimgs)]
  set csi 0
  xth_status_bar_status me [format "Zooming image %s ..." $xth(me,imgs,$imgx,name)]
  xth_me_progbar_show $totalsi
  foreach imgl $xth(me,imgs,$imgx,subimgs) {
    set dsti [lindex $imgl 0]
    incr csi
    xth_me_progbar_prog $csi
    switch $xth(me,zoom) {
      100 {
        $xth(me,can) itemconfigure [lindex $imgl 1] -image $srci
      }
      default {
        $xth(me,can) itemconfigure [lindex $imgl 1] -image $dsti
      }
    }
    switch $xth(me,zoom) {
      25 {$dsti copy $srci -subsample 4 -shrink}
      50 {$dsti copy $srci -subsample 2 -shrink}
      200 {}
      400 {}
      default {}
    }
  }
  xth_me_progbar_hide
  $srci configure -gamma $origgamma
  xth_status_bar_pop me
}

# NOZOOMING
} else {

proc xth_me_images_rescandraw {} {
}

proc xth_me_image_redraw {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
    return
  }
  if {$xth(me,imgs,$imgx,XVI)} {
    xth_me_imgs_xvi_redraw $imgx
    return
  }
  set totalsi [llength $xth(me,imgs,$imgx,subimgs)]
  set csi 0
  set x [lindex $xth(me,imgs,$imgx,position) 0]
  set y [lindex $xth(me,imgs,$imgx,position) 1]
  foreach imgl $xth(me,imgs,$imgx,subimgs) {
    incr csi
    $xth(me,can) coords [lindex $imgl 1] \
      [xth_me_real2canx [expr $x + [lindex $imgl 2]]] \
      [xth_me_real2cany [expr $y - [lindex $imgl 3]]]
  }
  update idletasks
}

proc xth_me_image_rescan {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
    return
  }
  if {$xth(me,imgs,$imgx,XVI)} {
    return
  }
  set srci $xth(me,imgs,$imgx,image)
  xth_status_bar_push me
  set totalsi [llength $xth(me,imgs,$imgx,subimgs)]
  set csi 0
  xth_status_bar_status me [format "Zooming image %s ..." $xth(me,imgs,$imgx,name)]
  xth_me_progbar_show $totalsi
  foreach imgl $xth(me,imgs,$imgx,subimgs) {
    set dsti [lindex $imgl 0]
    incr csi
    xth_me_progbar_prog $csi
    switch $xth(me,zoom) {
      25 {$dsti copy $srci -subsample 4 -shrink -from \
        [lindex $imgl 2] [lindex $imgl 3] [lindex $imgl 4] [lindex $imgl 5]}
      50 {$dsti copy $srci -subsample 2 -shrink -from \
        [lindex $imgl 2] [lindex $imgl 3] [lindex $imgl 4] [lindex $imgl 5]}
      200 {$dsti copy $srci -zoom 2 -shrink -from \
        [lindex $imgl 2] [lindex $imgl 3] [lindex $imgl 4] [lindex $imgl 5]}
      400 {$dsti copy $srci -zoom 4 -shrink -from \
        [lindex $imgl 2] [lindex $imgl 3] [lindex $imgl 4] [lindex $imgl 5]}
      default {$dsti copy $srci -shrink -from \
        [lindex $imgl 2] [lindex $imgl 3] [lindex $imgl 4] [lindex $imgl 5]}
    }
  }
  xth_me_progbar_hide
  xth_status_bar_pop me
}

}
# END NO NOZOOM

proc xth_me_imgs_xvi_redraw {imgx} {
  global xth
  set id $xth(me,imgs,$imgx,image)  
  
  # calculate all coordinates
  # grid
  set px [lindex $xth(me,imgs,$imgx,position) 0]
  set py [lindex $xth(me,imgs,$imgx,position) 1]
  set gx [lindex $xth(me,imgs,$imgx,XVIgrid) 0]
  set gy [lindex $xth(me,imgs,$imgx,XVIgrid) 1]
  set gxx [lindex $xth(me,imgs,$imgx,XVIgrid) 2]
  set gxy [lindex $xth(me,imgs,$imgx,XVIgrid) 3]
  set gyx [lindex $xth(me,imgs,$imgx,XVIgrid) 4]
  set gyy [lindex $xth(me,imgs,$imgx,XVIgrid) 5]
  set ngx [lindex $xth(me,imgs,$imgx,XVIgrid) 6]
  set ngy [lindex $xth(me,imgs,$imgx,XVIgrid) 7]
  
  set shx [expr double($px) - double($gx)]
  set shy [expr double($py) - double($gy)]
  
  set odd 0
  set crd {}
  for {set i 0} {$i <= $ngy} {incr i} {
    set lvx [xth_me_real2canx [expr $shx + $gx + $i * $gyx]]
    set lvy [xth_me_real2cany [expr $shy + $gy + $i * $gyy]]
    set rvx [xth_me_real2canx [expr $shx + $gx + $i * $gyx + $ngx * $gxx]]
    set rvy [xth_me_real2cany [expr $shy + $gy + $i * $gyy + $ngx * $gxy]]
    if {$odd} {
      lappend crd $lvx $lvy $rvx $rvy
      set odd 0
    } else {
      lappend crd $rvx $rvy $lvx $lvy
      set odd 1
    }
  }
  $xth(me,can) coords [format "%sGH" $id] $crd
  set crd {}
  for {set i 0} {$i <= $ngx} {incr i} {
    set lvx [xth_me_real2canx [expr $shx + $gx + $i * $gxx]]
    set lvy [xth_me_real2cany [expr $shy + $gy + $i * $gxy]]
    set rvx [xth_me_real2canx [expr $shx + $gx + $i * $gxx + $ngy * $gyx]]
    set rvy [xth_me_real2cany [expr $shy + $gy + $i * $gxy + $ngy * $gyy]]
    if {$odd} {
      lappend crd $lvx $lvy $rvx $rvy
      set odd 0
    } else {
      lappend crd $rvx $rvy $lvx $lvy
      set odd 1
    }
  }
  $xth(me,can) coords [format "%sGV" $id] $crd

  # walls
  set shts $xth(me,imgs,$imgx,XVIshots)
  for {set i 0} {$i < [llength $shts]} {incr i} {
    if {[llength [lindex $shts $i]] >= 12} {
      set x0 [xth_me_real2canx [expr $shx + [lindex [lindex $shts $i] 4]]]
      set y0 [xth_me_real2cany [expr $shy + [lindex [lindex $shts $i] 5]]]
      set x1 [xth_me_real2canx [expr $shx + [lindex [lindex $shts $i] 6]]]
      set y1 [xth_me_real2cany [expr $shy + [lindex [lindex $shts $i] 7]]]
      set x2 [xth_me_real2canx [expr $shx + [lindex [lindex $shts $i] 8]]]
      set y2 [xth_me_real2cany [expr $shy + [lindex [lindex $shts $i] 9]]]
      set x3 [xth_me_real2canx [expr $shx + [lindex [lindex $shts $i] 10]]]
      set y3 [xth_me_real2cany [expr $shy + [lindex [lindex $shts $i] 11]]]
      $xth(me,can) coords [format "%sWLF%d" $id $i] $x0 $y0 $x1 $y1 $x2 $y2 $x3 $y3
      $xth(me,can) coords [format "%sWLO%d" $id $i] $x0 $y0 $x1 $y1 $x2 $y2 $x3 $y3 $x0 $y0
    }
  }

  # shots
  for {set i 0} {$i < [llength $shts]} {incr i} {
    set fx [xth_me_real2canx [expr $shx + [lindex [lindex $shts $i] 0]]]
    set fy [xth_me_real2cany [expr $shy + [lindex [lindex $shts $i] 1]]]
    set tx [xth_me_real2canx [expr $shx + [lindex [lindex $shts $i] 2]]]
    set ty [xth_me_real2cany [expr $shy + [lindex [lindex $shts $i] 3]]]
    $xth(me,can) coords [format "%sSH%d" $id $i] $fx $fy $tx $ty
  }

  # stations
  set stns $xth(me,imgs,$imgx,XVIstations)
  set xth(me,imgs,$imgx,XVIstationsX) {}
  for {set i 0} {$i < [llength $stns]} {incr i} {
    set cx [expr $shx + [lindex [lindex $stns $i] 0]]
    set cy [expr $shy + [lindex [lindex $stns $i] 1]]
    lappend xth(me,imgs,$imgx,XVIstationsX) [list $cx $cy]
    set cx [xth_me_real2canx $cx]
    set cy [xth_me_real2cany $cy]
    $xth(me,can) coords [format "%sST%d" $id $i] [expr $cx - 3.0] [expr $cy - 3.0] [expr $cx + 3.0] [expr $cy + 3.0]
  }
  
  if {$xth(me,imgs,$imgx,vsb) == 1} {
    $xth(me,can) itemconfigure $xth(me,imgs,$imgx,image) -state normal
  }
}

proc xth_me_imgs_xvi_create {imgx} {
  global xth

  # firstly delete all imgx items
  foreach silist $xth(me,imgs,$imgx,subimgs) {
    $xth(me,can) delete [lindex $silist 1]
  }
  set xth(me,imgs,$imgx,subimgs) {}
  set id $xth(me,imgs,$imgx,image)
  set stpref [format "XVIstI%sS" $imgx] 
  # now create them

  # walls fill
  set shts $xth(me,imgs,$imgx,XVIshots)
  for {set i 0} {$i < [llength $shts]} {incr i} {
    if {[llength [lindex $shts $i]] >= 12} {
      set cid [$xth(me,can) create polygon 0 0 1 0 1 1 -stipple $xth(gui,xvi_walls_fptn) -fill $xth(gui,xvi_walls_fclr) -width 1 -state hidden -tags [list $id [format "%sWLF%d" $id $i]]]
      xth_me_bind_area_drag $cid $imgx
      xth_me_bind_image_drag $cid $imgx
      lappend xth(me,imgs,$imgx,subimgs) [list {} $cid]
    }
  }

  # grid
  set grid_info_str [format [mc "grid - spacing %s"] $xth(me,imgs,$imgx,XVIgrids)]
  set cid [$xth(me,can) create line 0 0 1 1 -fill $xth(gui,xvi_grid_clr) -width 1 -state hidden -tags [list $id [format "%sGH" $id]]]
  xth_me_bind_area_drag $cid $imgx
  xth_me_bind_image_drag $cid $imgx
  $xth(me,can) bind $cid <Enter> "xth_status_bar_push me; xth_status_bar_status me [list $grid_info_str]"
  $xth(me,can) bind $cid <Leave> "xth_status_bar_pop me"
  lappend xth(me,imgs,$imgx,subimgs) [list {} $cid]
  set cid [$xth(me,can) create line 0 0 1 1 -fill $xth(gui,xvi_grid_clr) -width 1 -state hidden -tags [list $id [format "%sGV" $id]]]
  xth_me_bind_area_drag $cid $imgx
  xth_me_bind_image_drag $cid $imgx
  $xth(me,can) bind $cid <Enter> "xth_status_bar_push me; xth_status_bar_status me [list $grid_info_str]"
  $xth(me,can) bind $cid <Leave> "xth_status_bar_pop me"
  lappend xth(me,imgs,$imgx,subimgs) [list {} $cid]

  # walls outline
  set shts $xth(me,imgs,$imgx,XVIshots)
  for {set i 0} {$i < [llength $shts]} {incr i} {
    if {[llength [lindex $shts $i]] >= 12} {
      set cid [$xth(me,can) create line 0 0 1 1 -fill $xth(gui,xvi_walls_oclr) -width 1 -state hidden -tags [list $id [format "%sWLO%d" $id $i]]]
      xth_me_bind_area_drag $cid $imgx
      xth_me_bind_image_drag $cid $imgx
      lappend xth(me,imgs,$imgx,subimgs) [list {} $cid]
    }
  }

  # shots
  for {set i 0} {$i < [llength $shts]} {incr i} {
    set cid [$xth(me,can) create line 0 0 1 1 -fill $xth(gui,xvi_shot_clr) -width 3 -state hidden -tags [list $id [format "%sSH%d" $id $i]]]
    xth_me_bind_area_drag $cid $imgx
    xth_me_bind_image_drag $cid $imgx
    lappend xth(me,imgs,$imgx,subimgs) [list {} $cid]
  }

  # stations
  set stns $xth(me,imgs,$imgx,XVIstations)
  for {set i 0} {$i < [llength $stns]} {incr i} {
    set cid [$xth(me,can) create oval 0 0 1 1 -fill $xth(gui,xvi_station_fclr) -outline $xth(gui,xvi_station_oclr) -width 1 -state hidden -tags [list $id [format "%sST%d" $id $i] "$stpref$i"]]
    xth_me_bind_area_drag $cid $imgx
    xth_me_bind_image_drag $cid $imgx
    set stinfo [format [mc "station - %s"] [lindex [lindex $stns $i] 2]]
    $xth(me,can) bind $cid <Enter> "xth_status_bar_push me; xth_status_bar_status me [list $stinfo]"
    $xth(me,can) bind $cid <Leave> "xth_status_bar_pop me"
    lappend xth(me,imgs,$imgx,subimgs) [list {} $cid]
  }

  # and change coordinates
  $xth(me,can) lower $xth(me,imgs,$imgx,image) command
  set iidx [lsearch -exact $xth(me,imgs,xlist) $imgx]
  if {$iidx > 0} {
    $xth(me,can) lower $xth(me,imgs,$imgx,image) $xth(me,imgs,[lindex $xth(me,imgs,xlist) [expr $iidx - 1]],image)
  }
  xth_me_imgs_xvi_redraw $imgx
}

proc xth_me_imgs_set_root {imgx} {
  global xth
  if {!$xth(me,imgs,$imgx,XVI)} {
    return
  }
  set rs $xth(me,imgs,$imgx,XVIroot)
  set xss $xth(me,imgs,$imgx,XVIstations)
  if {[string length $rs] < 1} {
    return
  }
  
  # prejde vsetky stations, a skusi najst tu s danym menom
  # ked ju najde, prerata position na grid origin a vynuluje
  for {set i 0} {$i < [llength $xss]} {incr i} {
    if {[string equal [lindex [lindex $xss $i] 2] $rs]} {
      set sx [lindex [lindex $xss $i] 0]
      set sy [lindex [lindex $xss $i] 1]
      set px [lindex $xth(me,imgs,$imgx,position) 0]
      set py [lindex $xth(me,imgs,$imgx,position) 1]
      set gx [lindex $xth(me,imgs,$imgx,XVIgrid) 0]
      set gy [lindex $xth(me,imgs,$imgx,XVIgrid) 1]
      set xth(me,imgs,$imgx,position) [list [expr double($gx) + double($px) - double($sx)] [expr double($gy) + double($py) - double($sy)]]
      set xth(me,imgs,$imgx,XVIroot) {}  
      break
    }
  }
}

proc xth_me_imgs_get_root {imgx} {
  global xth
  set xss $xth(me,imgs,$imgx,XVIstations)
  if {[llength $xss] > 0} {
    set i 0
    set sx [lindex [lindex $xss $i] 0]
    set sy [lindex [lindex $xss $i] 1]
    set px [lindex $xth(me,imgs,$imgx,position) 0]
    set py [lindex $xth(me,imgs,$imgx,position) 1]
    set gx [lindex $xth(me,imgs,$imgx,XVIgrid) 0]
    set gy [lindex $xth(me,imgs,$imgx,XVIgrid) 1]
    return [list [lindex $xss $i 2] [expr double($sx) + double($px) - double($gx)] [expr double($sy) + double($py) - double($gy)]]
  } else {
    return [list $xth(me,imgs,$imgx,XVIroot)]
  }
}


proc xth_me_image_insert {xx yy fname iidx imgx} {

  global xth
  
  if {! $xth(me,fopen)} {
    return
  }

  set vsb 1
  set igamma 1.0
  if {[llength $xx] > 1} {
    if {[llength $xx] > 2} {
       set igamma [lindex $xx 2]
    }
    set vsb [lindex $xx 1]
    set xx [lindex $xx 0]
  } 
  
  set XVIroot {}
  set isXVI 0
  if {[llength $yy] > 1} {
    set XVIroot [lindex $yy 1]
    set yy [lindex $yy 0]
  }

  if {[catch {expr $xx}]} {
    set xx $xth(me,area,xmin)
  }
  if {[catch {expr $yy}]} {
    if $isXVI {
      set yy $xth(me,area,ymax)
    } else {
      set yy $xth(me,area,ymin)
    }
  }
  
  set dial_id 0

  if {[string length $fname] < 1} {
  
    if {$xth(me,fnewf)} {
      set wtd [MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon question -type yesno \
        -message [mc "New *.th2 file needs to be saved before inserting background image. Save it now?"] \
        -font $xth(gui,lfont)]
      if {$wtd == 1} {
        return
      }
      xth_me_save_file 1
    }
    if {$xth(me,fnewf)} {
      return
    }
    
    set fname [tk_getOpenFile -parent $xth(gui,main) \
       -filetypes $xth(gui,imgfiletypes) \
       -initialdir $xth(me,fpath) -defaultextension ".gif"]  
    if {[string length $fname] < 1} {
      return
    } else {
      # overi ci cesta sedi
      if {![string equal -length [string length $xth(me,fpath)] \
        $xth(me,fpath) $fname]} {
        MessageDlg $xth(gui,message) -parent $xth(gui,main) \
          -icon error -type ok \
          -message "Picture $fname not in file path $xth(me,fpath)." \
          -font $xth(gui,lfont)
        return
      } else {
        set fname [string range $fname [expr [string length $xth(me,fpath)] + 1] end]
        set dial_id 1
      }
    }
  }

    
  xth_status_bar_push me
  xth_status_bar_status me "Loading image file $fname ..."
  set ffname [file join $xth(me,fpath) $fname]
  if {[string length $imgx] < 1} {
    set imgx $xth(me,imgln)
  }
  set imgid {}
  set XVIgrids {}
  set XVIgrid {}
  set XVIstations {}
  set XVIshots {}
	if {!$xth(gui,openxp)} {
    catch {
	      set imgid [image create photo -file $ffname]
	  } errorinf
    if {[string length $imgid] < 1} {
      catch {
        source $ffname
      } errorinf
      if {[string length $XVIgrids] > 0} {
        set isXVI 1
        set imgid "XVI$xth(me,imgs,xviid)"
        incr xth(me,imgs,xviid)
      }
    }
  } else {
		set errorinf "excluded picture"
	}
  
  if {[string length $imgid] < 1} {
    if {$xth(me,unredook)} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message "$errorinf" \
        -font $xth(gui,lfont)
    }
    xth_status_bar_pop me
    if {$dial_id} {
      return
    } else {
      set vsb [expr $vsb - 2]
    }
  }

  set undocmd "xth_me_image_remove %d"
  set redocmd "xth_me_image_insert {$xx $vsb} {$yy $XVIroot} [list $fname] %d $imgx"

  xth_me_unredo_action [mc "inserting image"] [format $undocmd $iidx] [format $redocmd $iidx]
     

  incr xth(me,nimgs)
  set xth(me,imgs,xlist) [linsert $xth(me,imgs,xlist) $iidx $imgx]
  set xth(me,imgs,$imgx,name) $fname
  set xth(me,imgs,$imgx,image) $imgid
  set xth(me,imgs,$imgx,position) [list $xx $yy]  
  set xth(me,imgs,$imgx,subimgs) {}
  set xth(me,imgs,$imgx,vsb) $vsb
  set xth(me,imgs,$imgx,gamma) $igamma
  set xth(me,imgs,$imgx,reload) [list $undocmd $redocmd]
  set xth(me,imgs,$imgx,ffname) $ffname
  set xth(me,imgs,$imgx,fmtime) 0
  catch {set xth(me,imgs,$imgx,fmtime) [file mtime $ffname]}
  set xth(me,imgs,$imgx,XVI) $isXVI
  set xth(me,imgs,$imgx,XVIroot) $XVIroot
  set xth(me,imgs,$imgx,XVIgrids) $XVIgrids
  set xth(me,imgs,$imgx,XVIgrid) $XVIgrid
  set xth(me,imgs,$imgx,XVIshots) $XVIshots
  set xth(me,imgs,$imgx,XVIstations) $XVIstations
  set xth(me,imgs,$imgx,XVIstationsX) {}
  
  xth_me_imgs_set_root $imgx
  
  
  # let's create image subimages
  if {($vsb >= 0) && (!$xth(me,imgs,$imgx,XVI))} {
    if {$xth(gui,me,nozoom)} {
      set subimg [image create photo]
      set subcimg [$xth(me,can) create image 0 0 -image $subimg -anchor nw \
        -tags "$imgid bgimg"]
      xth_me_bind_area_drag $subcimg $imgx
      xth_me_bind_image_drag $subcimg $imgx
      set iw [image width $imgid]
      set ih [image height $imgid]
      lappend xth(me,imgs,$imgx,subimgs) [list $subimg $subcimg 0 0 $iw $ih]
      $xth(me,can) lower $xth(me,imgs,$imgx,image) command
      if {$iidx > 0} {
        $xth(me,can) lower $xth(me,imgs,$imgx,image) $xth(me,imgs,[lindex $xth(me,imgs,xlist) [expr $iidx - 1]],image)
      }
    } else {
      set iw [image width $imgid]
      set ih [image height $imgid]
      set subisize 128
      for {set subx 0} {$subx < $iw} {incr subx $subisize} {
        for {set suby 0} {$suby < $ih} {incr suby $subisize} {
          set subxx [expr $subx + $subisize]
          set subyy [expr $suby + $subisize]
          if {$subxx > $iw} {
            set subxx $iw
          }
          if {$subyy > $ih} {
            set subyy $ih
          }
          set subimg [image create photo]
          set subcimg [$xth(me,can) create image 0 0 -image $subimg -anchor nw \
            -tags "$imgid bgimg"]
          xth_me_bind_area_drag $subcimg $imgx
          xth_me_bind_image_drag $subcimg $imgx
          lappend xth(me,imgs,$imgx,subimgs) [list $subimg $subcimg $subx $suby $subxx $subyy]
        }
      }
      $xth(me,can) lower $xth(me,imgs,$imgx,image) command
      set iidx [lsearch -exact $xth(me,imgs,xlist) $imgx]
      if {$iidx > 0} {
        $xth(me,can) lower $xth(me,imgs,$imgx,image) $xth(me,imgs,[lindex $xth(me,imgs,xlist) [expr $iidx - 1]],image)
      }
    }  
  }
  if {($vsb >= 0) && ($xth(me,imgs,$imgx,XVI))} {
    xth_me_imgs_xvi_create $imgx
  }
  xth_me_image_rescan $imgx
  xth_me_image_redraw $imgx
  xth_me_image_set_gamma $imgx
  xth_me_image_update_list
  xth_me_image_select 0
  incr xth(me,imgln)
  catch {$xth(me,can) raise cmd_ctrl bgimg}
  xth_status_bar_pop me
}


proc xth_me_image_destroy_all {} {
  global xth
  foreach imgx $xth(me,imgs,xlist) {
    unset xth(me,imgs,$imgx,name)
    unset xth(me,imgs,$imgx,position)
    if {[string length $xth(me,imgs,$imgx,image)] > 0} {
      catch {image delete $xth(me,imgs,$imgx,image)}
    }
    unset xth(me,imgs,$imgx,image)
    unset xth(me,imgs,$imgx,vsb)
    unset xth(me,imgs,$imgx,gamma)
    foreach silist $xth(me,imgs,$imgx,subimgs) {
      catch {image delete [lindex $silist 0]}
      $xth(me,can) delete [lindex $silist 1]
    }
    unset xth(me,imgs,$imgx,reload)
    unset xth(me,imgs,$imgx,ffname)
    unset xth(me,imgs,$imgx,fmtime)
    unset xth(me,imgs,$imgx,subimgs)
    unset xth(me,imgs,$imgx,XVI)
    unset xth(me,imgs,$imgx,XVIroot)
    unset xth(me,imgs,$imgx,XVIgrids)
    unset xth(me,imgs,$imgx,XVIgrid)
    unset xth(me,imgs,$imgx,XVIshots)
    unset xth(me,imgs,$imgx,XVIstations)
    unset xth(me,imgs,$imgx,XVIstationsX)
  }
  set xth(me,nimgs) 0
  set xth(me,imgln) 0
  set xth(me,imgs,list) {}
  set xth(me,imgs,xlist) {}
}

proc xth_me_image_remove {iidx} {
  global xth
  if {$xth(me,nimgs) < 1} {
    return
  }
  set isel [$xth(ctrl,me,images).il.ilbox curselection]
  if {[llength $isel] < 1} {
    return;
  }
  if {[string length $iidx] < 1} {
    set iidx [lindex $isel 0]
  }
  set imgx [lindex $xth(me,imgs,xlist) $iidx]
  xth_me_unredo_action [mc "removing image"] \
    "xth_me_image_insert {[lindex $xth(me,imgs,$imgx,position) 0] $xth(me,imgs,$imgx,vsb) $xth(me,imgs,$imgx,gamma)} [lindex $xth(me,imgs,$imgx,position) 1] [list $xth(me,imgs,$imgx,name)] $iidx $imgx" "xth_me_image_remove $iidx"
  unset xth(me,imgs,$imgx,name)
  unset xth(me,imgs,$imgx,position)
  if {[string length $xth(me,imgs,$imgx,image)] > 0} {
    catch {image delete $xth(me,imgs,$imgx,image)}
  }
  unset xth(me,imgs,$imgx,image)
  unset xth(me,imgs,$imgx,vsb)
  unset xth(me,imgs,$imgx,gamma)
  foreach silist $xth(me,imgs,$imgx,subimgs) {
    catch {image delete [lindex $silist 0]}
    $xth(me,can) delete [lindex $silist 1]
  }
  unset xth(me,imgs,$imgx,subimgs)
  unset xth(me,imgs,$imgx,reload)
  unset xth(me,imgs,$imgx,ffname)
  unset xth(me,imgs,$imgx,fmtime)
  unset xth(me,imgs,$imgx,XVI)
  unset xth(me,imgs,$imgx,XVIroot)
  unset xth(me,imgs,$imgx,XVIgrids)
  unset xth(me,imgs,$imgx,XVIgrid)
  unset xth(me,imgs,$imgx,XVIshots)
  unset xth(me,imgs,$imgx,XVIstations)
  unset xth(me,imgs,$imgx,XVIstationsX)
  set xth(me,nimgs) [expr $xth(me,nimgs) - 1]
  set xth(me,imgs,xlist) [lreplace $xth(me,imgs,xlist) $iidx $iidx]
  xth_me_image_update_list
  if {$iidx >= [llength $xth(me,imgs,xlist)]} {
    set iidx end
  }
  xth_me_image_select $iidx
}


proc xth_me_image_move_special {iidx newiidx} {

  global xth  

  if {$iidx == [expr $xth(me,nimgs) - 1]} {
    set iidx $xth(me,nimgs)
  }
  if {$iidx < $xth(me,nimgs)} {
    set iiidx $iidx
    set xiidx $iidx
  } else {
    set iiidx end
    set xiidx [expr $xth(me,nimgs) - 1]
  }
  set imgx [lindex $xth(me,imgs,xlist) $iiidx]

  if {$newiidx < $xth(me,nimgs)} {
    $xth(me,can) raise $xth(me,imgs,$imgx,image) $xth(me,imgs,[lindex $xth(me,imgs,xlist) $newiidx],image)
    set xth(me,imgs,xlist) [linsert [lreplace $xth(me,imgs,xlist) $iiidx $iiidx] $newiidx $imgx]
  } else {
    $xth(me,can) lower $xth(me,imgs,$imgx,image) $xth(me,imgs,[lindex $xth(me,imgs,xlist) end],image)
    set xth(me,imgs,xlist) [linsert [lreplace $xth(me,imgs,xlist) $iiidx $iiidx] end $imgx]
  }  

  xth_me_unredo_action [mc "moving image"] \
    "xth_me_image_move_special $newiidx $iidx" "xth_me_image_move_special $iidx $newiidx"
  xth_me_image_update_list
  xth_me_image_select $xiidx
  
}


proc xth_me_image_move_front {} {
  global xth
  xth_me_cmds_update {}
  if {$xth(me,nimgs) < 1} {
    return
  }
  set isel [$xth(ctrl,me,images).il.ilbox curselection]
  if {[llength $isel] < 1} {
    return;
  }
  set iidx [lindex $isel 0]
  if {$iidx == 0} {
    return
  }
  
  xth_me_image_move_special $iidx 0

}


proc xth_me_image_move_back {} {
  global xth
  xth_me_cmds_update {}
  if {$xth(me,nimgs) < 1} {
    return
  }
  set isel [$xth(ctrl,me,images).il.ilbox curselection]
  if {[llength $isel] < 1} {
    return;
  }
  set iidx [lindex $isel 0]
  if {$iidx == ($xth(me,nimgs) - 1)} {
    return
  }

  xth_me_image_move_special $iidx $xth(me,nimgs)
}


proc xth_me_image_select {iidx} {
  
  global xth

  if {!$xth(me,fopen)} {
    return
  }
  
  if {[string length $iidx] == 0} {
    set iidx 0
  }
  
  if {$xth(me,nimgs) > 0} {
    $xth(ctrl,me,images).ic.remp configure -state normal
    $xth(ctrl,me,images).ic.posl configure -state normal
    $xth(ctrl,me,images).ic.posln configure -state normal
    $xth(ctrl,me,images).ic.posch configure -state normal
    $xth(ctrl,me,images).ic.posx configure -state normal
    $xth(ctrl,me,images).ic.posy configure -state normal
    $xth(ctrl,me,images).ic.mvf configure -state normal
    $xth(ctrl,me,images).ic.mvb configure -state normal
#    $xth(ctrl,me,images).il.ilbox configure -state normal
    $xth(ctrl,me,images).il.ilbox selection clear 0 end
    $xth(ctrl,me,images).il.ilbox selection set $iidx
    set iidx [lindex [$xth(ctrl,me,images).il.ilbox curselection] 0]
    set imgx [lindex $xth(me,imgs,xlist) $iidx]
    if {$xth(me,imgs,$imgx,vsb) < 0} {
      $xth(ctrl,me,images).ic.viscb configure -state disabled
      $xth(ctrl,me,images).ic.gs configure -state disabled
      $xth(ctrl,me,images).ic.gr configure -state disabled
      $xth(ctrl,me,images).ic.gl configure -state disabled
      xth_me_image_update_gamma_scale
      set xth(ctrl,me,images,vis) 0
    } else {
      $xth(ctrl,me,images).ic.viscb configure -state normal
      $xth(ctrl,me,images).ic.gs configure -state normal
      $xth(ctrl,me,images).ic.gr configure -state normal
      $xth(ctrl,me,images).ic.gl configure -state normal
      xth_me_image_update_gamma_scale
      set xth(ctrl,me,images,vis) $xth(me,imgs,$imgx,vsb)
    }
    xth_me_image_update_position
    update idletasks
  } else {
    $xth(ctrl,me,images).ic.viscb configure -state disabled
    $xth(ctrl,me,images).ic.remp configure -state disabled
    $xth(ctrl,me,images).ic.posl configure -state disabled
    $xth(ctrl,me,images).ic.posln configure -state disabled -text ""
    set xth(ctrl,me,images,posx) ""
    set xth(ctrl,me,images,posy) ""
    $xth(ctrl,me,images).ic.posch configure -state disabled
    $xth(ctrl,me,images).ic.posx configure -state disabled
    $xth(ctrl,me,images).ic.posy configure -state disabled
    $xth(ctrl,me,images).ic.mvf configure -state disabled
    $xth(ctrl,me,images).ic.mvb configure -state disabled
    $xth(ctrl,me,images).ic.gs configure -state disabled
    $xth(ctrl,me,images).ic.gr configure -state disabled
    $xth(ctrl,me,images).ic.gl configure -state disabled -text "gamma 1.00"
#    $xth(ctrl,me,images).il.ilbox configure -state disabled
    focus $xth(gui,main)
    update idletasks
  }
  
}

proc xth_me_image_gamma {imgx gv} {
  global xth
  set og $xth(me,imgs,$imgx,gamma)
  set xth(me,imgs,$imgx,gamma) $gv
  xth_me_image_set_gamma $imgx
  xth_me_unredo_action [mc "gamma correction"] \
    "xth_me_image_gamma $imgx $og" \
    "xth_me_image_gamma $imgx $gv"
  xth_me_image_update_gamma_scale
}

proc xth_me_image_update_gamma {} {
  global xth
  catch { 
    set iidx [lindex [$xth(ctrl,me,images).il.ilbox curselection] 0]
    set imgx [lindex $xth(me,imgs,xlist) $iidx]
    if {[string length $imgx] > 0} {
      xth_me_image_gamma $imgx [expr pow(10.0,$xth(ctrl,me,images,gamma))]
    }
  }
}

proc xth_me_image_update_gamma_scale {} {
  global xth
  set iidx [lindex [$xth(ctrl,me,images).il.ilbox curselection] 0]
  set imgx [lindex $xth(me,imgs,xlist) $iidx]
  if {[string length $imgx] > 0} {
    set xth(ctrl,me,images,gamma) [expr log10($xth(me,imgs,$imgx,gamma))]
    $xth(ctrl,me,images).ic.gl configure -text [format "gamma %.2f" $xth(me,imgs,$imgx,gamma)]
  } else {
    $xth(ctrl,me,images).ic.gl configure -text "gamma 1.00"
    set xth(ctrl,me,images,gamma) 0
  }
}


proc xth_me_xvi_refresh {} {
  global xth
  if {[catch {
    set todolist $xth(me,imgs,xlist)
  }]} {
    return
  }
  foreach imgx $todolist {
    if $xth(me,imgs,$imgx,XVI) {
      set fmtime 0
      if {![catch {set fmtime [file mtime $xth(me,imgs,$imgx,ffname)]}]} {
        if {$fmtime > $xth(me,imgs,$imgx,fmtime)} {
          set cpos [lsearch -exact $xth(me,imgs,xlist) $imgx]
          if {$cpos > -1} {
            set undocmd [format [lindex $xth(me,imgs,$imgx,reload) 0] $cpos]
            set redocmd [format [lindex $xth(me,imgs,$imgx,reload) 1] $cpos]
            set xth(me,unredook) 0
            eval $undocmd
            eval $redocmd
            set xth(me,unredook) 1
          }
        }
      }
    }
  }
  xth_me_area_auto_adjust
}


