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
    if {[string length $xth(me,imgs,$imgx,image)] > 0} {
      set imgw [image width $xth(me,imgs,$imgx,image)]
      set imgh [image height $xth(me,imgs,$imgx,image)]
    } else {
      set imgw "---"
      set imgh "---"
    }
    lappend xth(me,imgs,list) "$xth(me,imgs,$imgx,name) ($imgw x $imgh)"
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
  xth_me_unredo_action "moving image" "xth_me_image_move $imgx $xth(me,imgs,$imgx,position); xth_me_image_update_position" \
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

  xth_me_unredo_action "toggle image visibility" "xth_me_image_toggle_vsb $iidx" "xth_me_image_toggle_vsb $iidx"

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


proc xth_me_image_rescan {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
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


proc xth_me_image_set_gamma {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
    return
  }
  set ng $xth(me,imgs,$imgx,gamma)
  xth_status_bar_push me
  set totalsi [llength $xth(me,imgs,$imgx,subimgs)]
  set csi 0
  xth_status_bar_status me [format "Correcting image gamma (%s) ..." $xth(me,imgs,$imgx,name)]
  xth_me_progbar_show $totalsi
  foreach imgl $xth(me,imgs,$imgx,subimgs) {
    set dsti [lindex $imgl 0]
    incr csi
    xth_me_progbar_prog $csi
    $dsti configure -gamma $ng
  }
  xth_me_progbar_hide
  xth_status_bar_pop me
}


proc xth_me_image_redraw {imgx} {
  global xth
  if {$xth(me,imgs,$imgx,vsb) <= 0} {
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
  

  if {[catch {expr $xx}]} {
    set xx $xth(me,area,xmin)
  }
  if {[catch {expr $yy}]} {
    set yy $xth(me,area,ymax)
  }
  
  set dial_id 0
  if {[string length $fname] < 1} {
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
	if {!$xth(gui,openxp)} {
	  catch {
	      set imgid [image create photo -file $ffname]
	  } errorinf
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

  xth_me_unredo_action "inserting image" "xth_me_image_remove $iidx" "xth_me_image_insert {$xx $vsb} $yy [list $fname] $iidx $imgx"

  incr xth(me,nimgs)
  set xth(me,imgs,xlist) [linsert $xth(me,imgs,xlist) $iidx $imgx]
  set xth(me,imgs,$imgx,name) $fname
  set xth(me,imgs,$imgx,image) $imgid
  set xth(me,imgs,$imgx,position) [list $xx $yy]  
  set xth(me,imgs,$imgx,subimgs) {}
  set xth(me,imgs,$imgx,vsb) $vsb
  set xth(me,imgs,$imgx,gamma) $igamma
  
  
  # let's create image subimages
  if {$vsb >= 0} {
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
    if {$iidx > 0} {
      $xth(me,can) lower $xth(me,imgs,$imgx,image) $xth(me,imgs,[lindex $xth(me,imgs,xlist) [expr $iidx - 1]],image)
    }
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
      image delete $xth(me,imgs,$imgx,image)
    }
    unset xth(me,imgs,$imgx,image)
    unset xth(me,imgs,$imgx,vsb)
    unset xth(me,imgs,$imgx,gamma)
    foreach silist $xth(me,imgs,$imgx,subimgs) {
      image delete [lindex $silist 0]
      $xth(me,can) delete [lindex $silist 1]
    }
    unset xth(me,imgs,$imgx,subimgs)
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
  xth_me_unredo_action "removing image" "xth_me_image_insert {[lindex $xth(me,imgs,$imgx,position) 0] $xth(me,imgs,$imgx,vsb) $xth(me,imgs,$imgx,gamma)} [lindex $xth(me,imgs,$imgx,position) 1] [list $xth(me,imgs,$imgx,name)] $iidx $imgx" "xth_me_image_remove $iidx"
  unset xth(me,imgs,$imgx,name)
  unset xth(me,imgs,$imgx,position)
  if {[string length $xth(me,imgs,$imgx,image)] > 0} {
   image delete $xth(me,imgs,$imgx,image)
  }
  unset xth(me,imgs,$imgx,image)
  unset xth(me,imgs,$imgx,vsb)
  unset xth(me,imgs,$imgx,gamma)
  foreach silist $xth(me,imgs,$imgx,subimgs) {
    image delete [lindex $silist 0]
    $xth(me,can) delete [lindex $silist 1]
  }
  unset xth(me,imgs,$imgx,subimgs)
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

  xth_me_unredo_action "moving image" "xth_me_image_move_special $newiidx $iidx" "xth_me_image_move_special $iidx $newiidx"
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
  xth_me_unredo_action "gamma correction" "xth_me_image_gamma $imgx $og" \
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

