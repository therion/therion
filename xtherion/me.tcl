##
## me.tcl --
##
##     The therion map editor.
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


xth_about_status "loading map editor ..."

set xth(me,dflt,scrap,scale) {}

proc xth_me_reset_defaults {} {
  global xth

  set xth(me,dflt,scrap,projection) {}
  set xth(me,dflt,scrap,options) {}

  set xth(me,dflt,point,type) {station}
  set xth(me,dflt,point,options) {}
  set xth(me,dflt,point,rotation) {}
  set xth(me,dflt,point,xsize) {}
  set xth(me,dflt,point,ysize) {}

  set xth(me,dflt,line,type) {wall}
  set xth(me,dflt,line,options) {}
    
}

xth_me_reset_defaults

proc xth_me_bind_entry_focusin {wlist} {
  foreach w $wlist {
    bind $w <FocusIn> "$w selection range 0 end"
  }
}

proc xth_me_bind_entry_return {wlist retcmd} {
  foreach w $wlist {
    bind $w <Return> $retcmd
  }
}

proc xth_me_bind_entry_focus_return {wlist retcmd} {
  set llen [expr [llength $wlist] - 1]
  for {set i 0} {$i < $llen} {incr i} {
    bind [lindex $wlist $i] <Return> "focus [lindex $wlist [expr $i + 1]]"
  }
  bind [lindex $wlist end] <Return> "$retcmd\nfocus [lindex $wlist 0]"
}


proc xth_me_unredo_reset {} {
  global xth
  set xth(me,undolist) {}
  set xth(me,redolist) {}
  set xth(me,unredook) 1
  xth_me_unredo_update
}


proc xth_me_real2canx {x} {
  global xth
  return [expr 0.01 * $xth(me,zoom) * $x]
}

proc xth_me_real2cany {y} {
  global xth
  return [expr -0.01 * $xth(me,zoom) * $y]
}

proc xth_me_can2realx {x} {
  global xth
  return [expr 100.0 / $xth(me,zoom) * $x]
}

proc xth_me_can2realy {y} {
  global xth
  return [expr -100.0 / $xth(me,zoom) * $y]
}

proc xth_me_unredo_update {} {
  
  global xth
  
  if {[llength $xth(me,undolist)] > 0} {
    $xth(me,menu,edit) entryconfigure $xth(me,menu,edit,undo) \
      -label [format "Undo %s" [lindex [lindex $xth(me,undolist) 0] 0]] -state normal
  } else {
    $xth(me,menu,edit) entryconfigure $xth(me,menu,edit,undo) \
      -label "Undo" -state disabled
  }

  if {[llength $xth(me,redolist)] > 0} {
    $xth(me,menu,edit) entryconfigure $xth(me,menu,edit,redo) \
      -label [format "Redo %s" [lindex [lindex $xth(me,redolist) 0] 0]] -state normal
  } else {
    $xth(me,menu,edit) entryconfigure $xth(me,menu,edit,redo) \
      -label "Redo" -state disabled
  }
    
}


proc xth_me_unredo_undo {} {

  global xth
  xth_me_cmds_update {}
  if {[llength $xth(me,undolist)] > 0} {
    set acmd [lindex $xth(me,undolist) 0]
    set xth(me,undolist) [lreplace $xth(me,undolist) 0 0]
    set xth(me,redolist) [linsert $xth(me,redolist) 0 $acmd]
    set xth(me,unredook) 0
    set xth(me,unredoshift) 1
    eval [lindex $acmd 3]
    set xth(me,unredook) 1
    if {$xth(me,unredoshift)} {
      $xth(me,can) xview moveto [lindex $acmd 1]
      $xth(me,can) yview moveto [lindex $acmd 2]
    }
    xth_me_unredo_update
  }  
}


proc xth_me_get_center {} {
  global xth
  set x [winfo x $xth(me,can)]
  set y [winfo y $xth(me,can)]
  set w [winfo width $xth(me,can)]
  set h [winfo height $xth(me,can)]
  return [list [xth_me_can2realx [$xth(me,can) canvasx [expr $x + $w / 2]]] \
    [xth_me_can2realy [$xth(me,can) canvasy [expr $y + $h / 2]]]]
}


proc xth_me_center_to {crds} {
  global xth
  set x [xth_me_real2canx [lindex $crds 0]]
  set y [xth_me_real2cany [lindex $crds 1]]
  set sr [$xth(me,can) cget -scrollregion]
  set xw [$xth(me,can) xview]
  set yw [$xth(me,can) yview]
  # adjust x
  set wf [expr [lindex $xw 1] - [lindex $xw 0]]
  set hf [expr [lindex $yw 1] - [lindex $yw 0]]
  set tw [expr [lindex $sr 2] - [lindex $sr 0]]
  set th [expr [lindex $sr 3] - [lindex $sr 1]]
  if {double($wf) < 1.0} {
    set pp [expr double($x) - [lindex $sr 0] - \
      0.5 * $wf * $tw]
    if {$pp < 0} {
      set pf 0.0
    } else {
      set pf [expr double($pp) / $tw]
    }
    $xth(me,can) xview moveto $pf
  }
  # adjust y
  if {double($hf) < 1.0} {
    set pp [expr double($y) - [lindex $sr 1] - \
      0.5 * $hf * $th]
    if {$pp < 0} {
      set pf 0.0
    } else {
      set pf [expr double($pp) / $th]
    }
    $xth(me,can) yview moveto $pf
  }
}



proc xth_me_unredo_redo {} {

  global xth

  if {[llength $xth(me,redolist)] > 0} {
    set acmd [lindex $xth(me,redolist) 0]
    set xth(me,redolist) [lreplace $xth(me,redolist) 0 0]
    set xth(me,undolist) [linsert $xth(me,undolist) 0 $acmd]
    $xth(me,can) xview moveto [lindex $acmd 1]
    $xth(me,can) yview moveto [lindex $acmd 2]
    set xth(me,unredook) 0
    eval [lindex $acmd 4]
    set xth(me,unredook) 1
    xth_me_unredo_update
  }  
  
}


proc xth_me_unredo_action {txt undocmd redocmd} {
  
  global xth
  if {$xth(me,unredook)} {
    set xth(me,fsave) 1
    set xth(me,redolist) {}
    if {[string length $xth(me,unredola)] > 0} {
      set txt $xth(me,unredola)
      set xth(me,unredola) {}
    }
    set xth(me,undolist) [linsert $xth(me,undolist) 0 [list \
      $txt [lindex [$xth(me,can) xview] 0] [lindex [$xth(me,can) yview] 0] $undocmd $redocmd]]
    xth_me_unredo_update
  }
  
}


# create new file
proc xth_me_create_file {} {

  global xth
  
  # create file variables
  set xth(me,unredook) 0
  incr xth(me,fltid)
  set cfid $xth(me,fltid)
  set xth(me,fname) [format "noname%02d.th2" $cfid]
  set xth(me,open_file) $xth(me,fname)
  set xth(me,fpath) $xth(gui,initdir)
  set xth(me,ffull) [file join $xth(gui,initdir) $xth(me,fname)]

  set xth(me,fnewf) 1
  set xth(me,fopen) 1
  set xth(me,fsave) 0

  set xth(me,nimgs) 0
  set xth(me,imgln) 0
  set xth(me,imgs,list) {}
  set xth(me,imgs,xlist) {}

  set xth(me,cmds,cmdln) 1
  set xth(me,cmds,list) {}
  set xth(me,cmds,xlist) {}
  set xth(me,cmds,action) {}
  set xth(me,cmds,selid) 0
  set xth(me,cmds,selpid) 0
  set xth(me,cmds,selx) 0
  set xth(me,cmds,mode) 0
  xth_me_cmds_create 0 0 0
  xth_me_cmds_update_buttons
  xth_me_cmds_set_mode 0
  xth_me_prev_cmd {}
  $xth(ctrl,me,cmds).cl.l selection set 0 0

  # enable all controls
  $xth(me,pbar) configure -state normal -text "0.0 : 0.0"
  
  $xth(ctrl,me,images).ic.insp configure -state normal
  $xth(ctrl,me,area).l configure -state normal
  $xth(ctrl,me,area).zl configure -state normal
  $xth(ctrl,me,area).zb configure -state normal
    
  $xth(me,menu,file) entryconfigure "New" -state disabled
  $xth(me,menu,file) entryconfigure "Open" -state disabled
  $xth(me,menu,file) entryconfigure "Open XP" -state disabled
  $xth(me,menu,file) entryconfigure "Save" -state normal
  $xth(me,menu,file) entryconfigure "Save as" -state normal
  $xth(me,menu,file) entryconfigure "Close" -state normal
  
  $xth(me,menu) entryconfigure "Edit" -state normal

  $xth(ctrl,me,area).xmin configure -state normal
  $xth(ctrl,me,area).ymin configure -state normal
  $xth(ctrl,me,area).xmax configure -state normal
  $xth(ctrl,me,area).ymax configure -state normal
  $xth(ctrl,me,area).mab configure -state normal
  $xth(ctrl,me,area).aab configure -state normal

  xth_me_image_select 0
  xth_me_cmds_set_action 2

  # create working area
  grid $xth(me,canf) -column 0 -row 0 -sticky news
  xth_me_area_adjust 0 0 1600 1200

  xth_app_title me  

  xth_ctrl_maximize me area
  xth_ctrl_maximize me images
  set xth(me,fsave) 0
  xth_me_unredo_reset
  xth_me_reset_defaults  

}


proc xth_me_prev_cmd {cmd} {
  global xth
  $xth(ctrl,me,prev).txt configure -state normal
  $xth(ctrl,me,prev).txt delete 1.0 end
  $xth(ctrl,me,prev).txt insert 1.0 $cmd
  $xth(ctrl,me,prev).txt configure -state disabled
  $xth(ctrl,me,prev).txt see 1.0
  update idletasks
}


proc xth_me_destroy_file {} {

  global xth
  if {$xth(me,fopen)} {
  
    xth_me_cmds_unselect {}

    set xth(me,unredook) 0
    set xth(me,open_file) ""
    set xth(me,fnewf) 0
    set xth(me,fopen) 0
    xth_me_image_destroy_all
    set xth(me,fsave) 0

    set xth(me,zoom) 100
    set xth(me,zoomv) 100
    $xth(ctrl,me,area).zb configure -text [format "%d %%" 100]
    $xth(me,menu,edit) entryconfigure $xth(me,menu,edit,zoom) -label [format "Zoom %d %%" 100]
  
    # disable all controls
    $xth(me,pbar) configure -text "" -state disabled

    set xth(ctrl,me,images,posx) ""
    set xth(ctrl,me,images,posy) ""
    set xth(ctrl,me,images,vis) 0
    $xth(ctrl,me,images).ic.insp configure -state disabled
    $xth(ctrl,me,area).l configure -state disabled -text ""
    set xth(ctrl,me,area,xmin) ""
    set xth(ctrl,me,area,ymin) ""
    set xth(ctrl,me,area,xmax) ""
    set xth(ctrl,me,area,ymax) ""
    $xth(ctrl,me,area).zl configure -state disabled
    $xth(ctrl,me,area).zb configure -state disabled
    $xth(me,menu,file) entryconfigure "New" -state normal
    $xth(me,menu,file) entryconfigure "Open" -state normal
    $xth(me,menu,file) entryconfigure "Open XP" -state normal
    $xth(me,menu,file) entryconfigure "Save" -state disabled
    $xth(me,menu,file) entryconfigure "Save as" -state disabled
    $xth(me,menu,file) entryconfigure "Close" -state disabled

    $xth(me,menu) entryconfigure "Edit" -state disabled

    $xth(ctrl,me,area).xmin configure -state disabled
    $xth(ctrl,me,area).ymin configure -state disabled
    $xth(ctrl,me,area).xmax configure -state disabled
    $xth(ctrl,me,area).ymax configure -state disabled
    $xth(ctrl,me,area).mab configure -state disabled
    $xth(ctrl,me,area).aab configure -state disabled
    xth_me_image_select 0
    focus $xth(gui,main)

    $xth(me,can) delete command
    xth_me_cmds_update_buttons
    xth_me_prev_cmd {}
    
    set xth(me,cmds,list) {}
    set xth(me,cmds,xlist) {}
    grid forget $xth(me,canf) 
    
    xth_ctrl_minimize me cmds
    xth_ctrl_minimize me prev
    xth_ctrl_minimize me ss
    xth_ctrl_minimize me point
    xth_ctrl_minimize me line
    xth_ctrl_minimize me linept
    xth_ctrl_minimize me scrap
    xth_ctrl_minimize me text
    xth_ctrl_minimize me area
    xth_ctrl_minimize me images
    
    xth_me_unredo_reset
    xth_app_title me
    
  }
}



proc xth_me_before_close_file {btns} {

  global xth
  xth_me_cmds_update {}
  if {$xth(me,fsave)} {    
    set wtd [MessageDlg $xth(gui,message) -parent $xth(gui,main) \
      -icon question -type $btns\
      -message "File $xth(me,ffull) is not saved. Save it now?" \
      -font $xth(gui,lfont)]
    switch $wtd {
      0 {
        if {[xth_me_save_file 0] == 0} {
          return 0
        }
      }
      1 {}
      default {return 0}
    }
  }
  return 1
}



# xth_me_read_file --
#
# return success
# {success name cmds lns}

proc xth_me_read_file {pth changebs} {

  global errorInfo xth
  
  set curenc utf-8
  set nm [file tail $pth]
  set encspc 0
  set flnn 0
  set success 1
  set lastln ""
  set lns {}
  set cmds {}
  if {[catch {set fid [open $pth r]}]} {
    set success 0
    set nm $errorInfo
    return [list $success $nm {} {}]
  }
  fconfigure $fid -encoding $curenc
  while {![eof $fid]} {
    gets $fid fln
    incr flnn
    if {[regexp {^\s*encoding\s+(\S+)\s*$} $fln encln enc]} {
      if {$encspc} {
        set success 0
        set nm "$pth \[$flnn\] -- multiple encoding commands in file"
        break
      }
      set encspc 1
      set rxp "\\s+($enc)\\s+"
      set validenc [regexp -nocase $rxp $xth(encodings) dum curenc]
      if {$validenc == 0} {
        set success 0
        set nm "$pth \[$flnn\] -- unknown encoding -- $enc"
        break
      }
      fconfigure $fid -encoding $curenc
      set lastln ""
    } elseif {[regexp {^\s*\#\#XTHERION\#\#\s+(\S.*)\s*$} $fln cmmdln cmmd]} {
      lappend cmds $cmmd
      set lastln ""
      if {[regexp {^\s*\#\#BEGIN\#\#\s*$} $cmmd]} {
        fconfigure $fid -encoding utf-8
      }
      if {[regexp {^\s*\#\#END\#\#\s*$} $cmmd]} {
        fconfigure $fid -encoding $curenc
      }
    } else {
      if {$changebs && [regexp {(.*)\\\s*$} $lastln dumln prevln]} {
        set fln "$prevln$fln"
        if {[llength $lns] > 1} {
          set lns [lrange $lns 0 [expr [llength $lns] - 2]]
        } else {
          set lns {}
        }
      }
      lappend lns $fln
      set lastln $fln
    }
  }
  close $fid
  return [list $success $nm $cmds $lns]
  
}  


# xth_me_write_file --
#
# return list containing
# {success name}

proc xth_me_write_file {pth} {

  global errorInfo xth

  set curenc utf-8
  set nm [file tail $pth]
  set success 1
  if {[catch {set fid [open $pth w]}]} {
    set success 0
    set nm $errorInfo
    return [list $success $nm]
  }
  fconfigure $fid -encoding utf-8 -translation {auto lf}
  puts $fid "encoding  utf-8"
  # now let's put special commands
  puts $fid "##XTHERION## xth_me_area_adjust $xth(me,area,xmin) $xth(me,area,ymin) $xth(me,area,xmax) $xth(me,area,ymax)"
  puts $fid "##XTHERION## xth_me_area_zoom_to $xth(me,zoom)"
  # images
  set xxlist {}
  foreach imgx $xth(me,imgs,xlist) {
    set xxlist [linsert $xxlist 0 $imgx]
  }
  foreach imgx $xxlist {
    set vsb $xth(me,imgs,$imgx,vsb)
    set gamma $xth(me,imgs,$imgx,gamma)
    if {$vsb < 0} {
      set vsb [expr $xth(me,imgs,$imgx,vsb) + 2]
    }
    puts $fid [format "##XTHERION## xth_me_image_insert %s %s %s 0 {}" \
      "{[expr [lindex $xth(me,imgs,$imgx,position) 0]] $vsb $gamma}" \
      [expr [lindex $xth(me,imgs,$imgx,position) 1]] \
      [list $xth(me,imgs,$imgx,name)]]
  }
  foreach id $xth(me,cmds,xlist) {
    if {$xth(me,cmds,$id,ct) == 4} {
      puts $fid "\n"
    }
    puts $fid "\n$xth(me,cmds,$id,data)"
  }
  close $fid
  return [list $success $nm]
}


proc xth_me_open_file {dialogid fname fline} {

  global xth

  if {$xth(me,fopen) != 0} {
    return 0
  }
  
  if {$dialogid} {
    set fname [tk_getOpenFile -filetypes $xth(app,me,filetypes) \
      -parent $xth(gui,main) \
      -initialdir $xth(gui,initdir) -defaultextension {.th2}]
  }
  
  if {[string length $fname] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fname]
  }
  
  # now let's open file fname
  
  # read the file
  xth_status_bar_push me
  xth_status_bar_status me "Opening $fname ..."
  
  set fdata [xth_me_read_file $fname 1]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message [lindex $fdata 1] \
        -font $xth(gui,lfont)
      xth_status_bar_pop me
      return 0
  }
  
  # show the file
  xth_me_create_file
  set xth(me,unredook) 0

  set xth(me,fname) [file tail $fname]
  set xth(me,open_file) [lindex $fdata 1]
  set xth(me,fpath) [file dirname $fname]
  set xth(me,ffull) $fname
  
  foreach cmd [lindex $fdata 2] {
    catch {eval $cmd}
  }
  xth_me_cmds_create_all [lindex $fdata 3]
  
  xth_ctrl_maximize me cmds
  xth_ctrl_maximize me point
  xth_ctrl_maximize me line
  xth_ctrl_maximize me linept
  xth_ctrl_maximize me scrap
  xth_ctrl_maximize me text
  xth_ctrl_maximize me area
  xth_ctrl_maximize me images
  
  set xth(me,fnewf) 0
  set xth(me,fopen) 1
  set xth(me,fsave) 0
  xth_app_title me
  xth_status_bar_pop me
  xth_me_cmds_select 0
  xth_me_unredo_reset
  xth_me_reset_defaults
  return 1
}


proc xth_me_save_file {dialogid} {

  global xth
  if {$xth(me,fopen) == 0} {
    return 0
  }

  xth_me_cmds_update {}
  
  # let's check if we need to save
  if {!($xth(me,fnewf) || $xth(me,fsave) || $dialogid)} {
    return 1
  }
  
  xth_status_bar_push me
  
  if {$xth(me,fnewf)} {
    set dialogid 1
  }

  set fname $xth(me,ffull)
  set ofname $fname
  if {$dialogid} {
    set fname [tk_getSaveFile -filetypes $xth(app,me,filetypes) \
      -parent $xth(gui,main) \
      -initialfile [file tail $fname] -defaultextension {.th2} \
      -initialdir [file dirname $fname]]
  }
  
  if {[string length $fname] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fname]
  }
  
  # save the file
  xth_status_bar_status me "Saving $fname ..."
  set fdata [xth_me_write_file $fname]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message [lindex $fdata 1] \
        -font $xth(gui,lfont)
      xth_status_bar_pop me
      return 0
  }
  
  set xth(me,fnewf) 0
  set xth(me,fsave) 0
  
  # if SaveAs, then redisplay the file
  if {$dialogid} {
    if {[string compare $ofname $fname] != 0} {
      set xth(me,fname) [file tail $fname]
      set xth(me,ffull) $fname
      set xth(me,fpath) [file dirname $fname]
      set xth(me,open_file) $xth(me,fname)
      xth_app_title me
    }
  }  

  after 250 {xth_status_bar_pop me}
  return 1
    
}



proc xth_me_close_file {} {

  global xth

  if {$xth(me,fopen) == 0} {
    return
  }

  xth_me_cmds_update {}
  
  if {[xth_me_before_close_file yesnocancel]} {
    xth_me_destroy_file
    return 1
  } else {
    return 0
  }
  
}


proc xth_me_area_redraw {} {
  global xth
  set x1 [xth_me_real2canx $xth(me,area,xmin)]
  set x2 [xth_me_real2canx $xth(me,area,xmax)]
  set y1 [xth_me_real2cany $xth(me,area,ymin)]
  set y2 [xth_me_real2cany $xth(me,area,ymax)]
  $xth(me,can) coords $xth(me,canid,area) $x1 $y1 $x1 $y2 $x2 $y2 $x2 $y1  
}


proc xth_me_limitize {limits x y} {
  set xmin $x
  set xmax $x
  set ymin $y
  set ymax $y
  if {[llength $limits] == 4} {
    if {[lindex $limits 0] < $x} {
      set xmin [lindex $limits 0]
    }
    if {[lindex $limits 1] < $y} {
      set ymin [lindex $limits 1]
    }
    if {[lindex $limits 2] > $x} {
      set xmax [lindex $limits 2]
    }
    if {[lindex $limits 3] > $y} {
      set ymax [lindex $limits 3]
    }
  }
  return [list $xmin $ymin $xmax $ymax]
}


proc xth_me_area_auto_adjust {} {
  
  global xth 
  set limits {}
  
  # scan limits of pictures
  foreach imgx $xth(me,imgs,xlist) {
    set px [lindex $xth(me,imgs,$imgx,position) 0]
    set py [lindex $xth(me,imgs,$imgx,position) 1]
    set sx [image width $xth(me,imgs,$imgx,image)]
    set sy [image height $xth(me,imgs,$imgx,image)]
    set limits [xth_me_limitize $limits $px $py]
    set limits [xth_me_limitize $limits [expr $px + $sx] [expr $py - $sy]]
  }
  
  # scan limits of commands
  set cmdlim [$xth(me,can) bbox command]
  if {[llength $cmdlim] == 4} {
    set limits [xth_me_limitize $limits [xth_me_can2realx [lindex $cmdlim 0]] [xth_me_can2realy [lindex $cmdlim 1]]]
    set limits [xth_me_limitize $limits [xth_me_can2realx [lindex $cmdlim 2]] [xth_me_can2realy [lindex $cmdlim 3]]]
  }
  
  # adjust area limits
  if {[llength $limits] < 4} {
    set limits {128 128 128 128}
  }  
  xth_me_area_adjust [expr [lindex $limits 0] - 128] [expr [lindex $limits 1] - 128] \
    [expr [lindex $limits 2] + 128] [expr [lindex $limits 3] + 128]

}

proc xth_me_area_adjust {x1 y1 x2 y2} {
  
  global xth
  
  xth_me_unredo_action "adjusting area" \
    "xth_me_area_adjust $xth(me,area,xmin) $xth(me,area,ymin) $xth(me,area,xmax) $xth(me,area,ymax)" \
    "xth_me_area_adjust $x1 $y1 $x2 $y2"

  # let's assign zeros to non numbers
  if {[catch {expr $x1}]} {
    set x1 0.0
  }
  if {[catch {expr $x2}]} {
    set x2 0.0
  }
  if {[catch {expr $y1}]} {
    set y1 0.0
  }
  if {[catch {expr $y2}]} {
    set y2 0.0
  }
  if {($x2 - $x1) < 256} {
    set x2 [expr $x1 + 256]
  }
  if {($y2 - $y1) < 256} {
    set y2 [expr $y1 + 256]
  }
  
  set xth(me,area,xmin) $x1
  set xth(me,area,xmax) $x2
  set xth(me,area,ymin) $y1
  set xth(me,area,ymax) $y2

  set xth(ctrl,me,area,xmin) $x1
  set xth(ctrl,me,area,xmax) $x2
  set xth(ctrl,me,area,ymin) $y1
  set xth(ctrl,me,area,ymax) $y2

  xth_me_area_redraw
  xth_me_area_scroll_adjust
  
  catch {$xth(ctrl,me,area).l configure -text [format "%.0f:%.0f - %.0f:%.0f" $x1 $y1 $x2 $y2]}
  
}


proc xth_me_area_scroll {wdg fir las} {
  xth_scroll $wdg $fir $las
  xth_me_area_scroll_adjust
}


proc xth_me_area_scroll_adjust {} {
  
  global xth
  set scw [winfo width $xth(me,canf)]
  set sch [winfo height $xth(me,canf)]
  set axmax [xth_me_real2canx $xth(me,area,xmax)]
  set axmin [xth_me_real2canx $xth(me,area,xmin)]
  set aymax [xth_me_real2cany $xth(me,area,ymin)]
  set aymin [xth_me_real2cany $xth(me,area,ymax)]
  set aw [expr $axmax - $axmin]
  set ah [expr $aymax - $aymin]
  if {$aw < ($scw - 3 * $xth(gui,sbwidth))} {
    set sx1 [expr $axmin - ($scw - $aw)/2.0 + $xth(gui,sbwidth)]
    set sx2 [expr $axmax + ($scw - $aw)/2.0 - $xth(gui,sbwidth)]
    # eval $xth(scroll,$xth(me,canf).sh,hide)
  } else {
    set sx1 [expr $axmin - $xth(gui,sbwidth)/2.0]
    set sx2 [expr $axmax + $xth(gui,sbwidth)/2.0]
  }
  if {$ah < ($sch - 3 * $xth(gui,sbwidth))} {
    set sy1 [expr $aymin - ($sch - $ah)/2.0 + $xth(gui,sbwidth)]
    set sy2 [expr $aymax + ($sch - $ah)/2.0 - $xth(gui,sbwidth)]
    # eval $xth(scroll,$xth(me,canf).sv,hide)
  } else {
    set sy1 [expr $aymin - $xth(gui,sbwidth)/2.0]
    set sy2 [expr $aymax + $xth(gui,sbwidth)/2.0]
  }
  
  set csr [$xth(me,can) cget -scrollregion]
  set nsr "$sx1 $sy1 $sx2 $sy2"
  if {[string compare $csr $nsr] != 0} {
    $xth(me,can) configure -scrollregion $nsr
  }
}


proc xth_me_area_zoom_to {zv} {
  global xth
  
  xth_me_cmds_update {}
  
  if {($xth(me,cmds,$xth(me,cmds,selid),ct) == 3) && 
      ($xth(me,cmds,selpid) > 0)} {
    set ccrds [list $xth(me,cmds,$xth(me,cmds,selid),$xth(me,cmds,selpid),x) \
      $xth(me,cmds,$xth(me,cmds,selid),$xth(me,cmds,selpid),y)]
  } elseif {$xth(me,cmds,$xth(me,cmds,selid),ct) == 2} {
    set ccrds [list $xth(me,cmds,$xth(me,cmds,selid),x) \
      $xth(me,cmds,$xth(me,cmds,selid),y)]
  } else {
    set ccrds [xth_me_get_center]
  }
  
  xth_me_unredo_action zooming "xth_me_area_zoom_to $xth(me,zoom)" \
    "xth_me_area_zoom_to $zv"
    
  set xth(me,zoom) $zv
  set xth(me,zoomv) $zv
  $xth(ctrl,me,area).zb configure -text [format "%d %%" $zv]
  $xth(me,menu,edit) entryconfigure $xth(me,menu,edit,zoom) -label [format "Zoom %d %%" $zv]
  
  xth_me_area_redraw   
  xth_me_area_scroll_adjust
  
  foreach imgx $xth(me,imgs,xlist) {
    xth_me_image_rescan $imgx
    xth_me_image_redraw $imgx
  }
  
  xth_status_bar_push me
  xth_status_bar_status me "Zooming objects ..."
  set ncmds [llength $xth(me,cmds,xlist)]
  xth_me_progbar_show $ncmds
  set ccmd 0
  foreach id $xth(me,cmds,xlist) {
    incr ccmd
    xth_me_progbar_prog $ccmd
    switch $xth(me,cmds,$id,ct) {
      4 {
        if {$id == $xth(me,cmds,selid)} {
          xth_me_cmds_move_scrap_xctrl 1 [lindex $xth(me,cmds,$id,scale) 0] \
            [lindex $xth(me,cmds,$id,scale) 1] 
          xth_me_cmds_move_scrap_xctrl 2 [lindex $xth(me,cmds,$id,scale) 2] \
            [lindex $xth(me,cmds,$id,scale) 3]
        }
      }
      3 {
        xth_me_cmds_move_line $id
        if {$id == $xth(me,cmds,selid)} {
          set pid $xth(me,cmds,selpid)
          if {$pid != 0} {
            xth_me_cmds_show_linept_xctrl $id $pid
          }
        }
      }
      2 {
        $xth(me,can) coords pt$id [xth_me_cmds_calc_point_coords $id]
        if {$id == $xth(me,cmds,selid)} {
          xth_me_cmds_move_point_xctrl $id
          xth_me_cmds_move_point_fill_xctrl $id $xth(me,cmds,$id,rotation) $xth(me,cmds,$id,xsize) $xth(me,cmds,$id,ysize)
        }
      }
    }
  }
  
  xth_me_center_to $ccrds
  set xth(me,unredoshift) 0
  xth_me_progbar_hide
  xth_status_bar_pop me
  update idletasks
  
}

proc xth_me_image_choose {imgx} {
  global xth
  xth_me_image_select [lsearch $xth(me,imgs,xlist) $imgx]
}

proc xth_me_area_start_drag {tagOrId imgx x y} {
  global xth
  set hss [$xth(me,canf).sh get]
  set vss [$xth(me,canf).sv get]
  set srg [$xth(me,can) cget -scrollregion]
  set w [expr [lindex $srg 2] - [lindex $srg 0]]
  set h [expr [lindex $srg 3] - [lindex $srg 1]]
  set fx [expr (1 - [lindex $hss 1] + [lindex $hss 0]) * $w]
  set fy [expr (1 - [lindex $vss 1] + [lindex $vss 0]) * $h]     
  if {$fx > 0} {set fx [expr (1 - [lindex $hss 1] + [lindex $hss 0]) / $fx]}
  if {$fy > 0} {set fy [expr (1 - [lindex $vss 1] + [lindex $vss 0]) / $fy]}
  set xth(me,area,drag_fx) $fx
  set xth(me,area,drag_fy) $fy
  set xth(me,area,drag_ox) [lindex $hss 0]
  set xth(me,area,drag_oy) [lindex $vss 0]
  set xth(me,area,drag_mx) $x
  set xth(me,area,drag_my) $y
  $xth(me,can) configure -cursor plus
  $xth(me,can) bind $tagOrId <Shift-B1-Motion> "xth_me_area_drag %x %y"
  $xth(me,can) bind $tagOrId <Shift-B1-ButtonRelease> "xth_me_area_end_drag $tagOrId \"$imgx\" %x %y"
  $xth(me,can) bind $tagOrId <B3-Motion> "xth_me_area_drag %x %y"
  $xth(me,can) bind $tagOrId <B3-ButtonRelease> "xth_me_area_end_drag $tagOrId \"$imgx\" %x %y"
  update idletasks
}

proc xth_me_area_drag {x y} {
  global xth
  set xdelta [expr ($xth(me,area,drag_mx) - $x) * $xth(me,area,drag_fx)]
  set ydelta [expr ($xth(me,area,drag_my) - $y) * $xth(me,area,drag_fy)]
  $xth(me,can) xview moveto [expr $xth(me,area,drag_ox) + $xdelta]
  $xth(me,can) yview moveto [expr $xth(me,area,drag_oy) + $ydelta]
  update idletasks     
}


proc xth_me_area_end_drag {tagOrId imgx x y} {
  global xth
  xth_me_area_drag $x $y
  $xth(me,can) configure -cursor crosshair
  $xth(me,can) bind $tagOrId <Shift-B1-Motion> ""
  $xth(me,can) bind $tagOrId <Shift-B1-ButtonRelease> ""
  $xth(me,can) bind $tagOrId <B3-Motion> ""
  $xth(me,can) bind $tagOrId <B3-ButtonRelease> ""
  if {[string length $imgx] > 0} {
    xth_me_image_choose $imgx
  }
  update idletasks 
}


proc xth_me_area_motion {x y} {
  global xth
  $xth(me,pbar) configure -text [format "%.1f : %.1f" [xth_me_can2realx [$xth(me,can) canvasx $x]] [xth_me_can2realy [$xth(me,can) canvasy $y]]]
}


proc xth_me_bind_area_drag {tagOrId imgx} {
  global xth
  $xth(me,can) bind $tagOrId <1> "xth_me_cmds_click_area $tagOrId %x %y"
  $xth(me,can) bind $tagOrId <Motion> "xth_me_area_motion %x %y"
  $xth(me,can) bind $tagOrId <Shift-1> "xth_me_area_start_drag $tagOrId \"$imgx\" %x %y"
  $xth(me,can) bind $tagOrId <3> "xth_me_area_start_drag $tagOrId \"$imgx\" %x %y"
}

proc xth_me_bind_area_only_drag {tagOrId} {
  global xth
  $xth(me,can) bind $tagOrId <Shift-1> "xth_me_area_start_drag $tagOrId {} %x %y"
  $xth(me,can) bind $tagOrId <3> "xth_me_area_start_drag $tagOrId {} %x %y"
}




proc xth_me_image_start_drag {tagOrId imgx x y} {
  global xth
  set xth(me,imgs,drag_mx) $x
  set xth(me,imgs,drag_my) $y
  set xth(me,imgs,drag_px) [lindex $xth(me,imgs,$imgx,position) 0]
  set xth(me,imgs,drag_py) [lindex $xth(me,imgs,$imgx,position) 1]
  $xth(me,can) configure -cursor fleur
  $xth(me,can) bind $tagOrId <Shift-B1-Motion> "xth_me_image_drag $imgx %x %y"
  $xth(me,can) bind $tagOrId <Shift-B1-ButtonRelease> "xth_me_image_end_drag $tagOrId $imgx %x %y"
  $xth(me,can) bind $tagOrId <B3-Motion> "xth_me_image_drag $imgx %x %y"
  $xth(me,can) bind $tagOrId <B3-ButtonRelease> "xth_me_image_end_drag $tagOrId $imgx %x %y"
  update idletasks
}

proc xth_me_image_drag {imgx x y} {
  global xth
  set xth(me,imgs,$imgx,position) [list \
    [expr $xth(me,imgs,drag_px) - [expr double($xth(me,imgs,drag_mx) - $x) * 100.0 / $xth(me,zoom)]] \
    [expr $xth(me,imgs,drag_py) + [expr double($xth(me,imgs,drag_my) - $y) * 100.0 / $xth(me,zoom)]] \
  ]
  xth_me_image_redraw $imgx
}

proc xth_me_image_end_drag {tagOrId imgx x y} {
  global xth
  xth_me_image_drag $imgx $x $y
  $xth(me,can) configure -cursor crosshair
  $xth(me,can) bind $tagOrId <Shift-B1-Motion> ""
  $xth(me,can) bind $tagOrId <Shift-B1-ButtonRelease> ""
  $xth(me,can) bind $tagOrId <B3-Motion> ""
  $xth(me,can) bind $tagOrId <B3-ButtonRelease> ""
  xth_me_image_choose $imgx
  xth_me_unredo_action "dragging image" \
    "xth_me_image_move $imgx $xth(me,imgs,drag_px) $xth(me,imgs,drag_py)" \
    "xth_me_image_move $imgx $xth(me,imgs,$imgx,position)"
  update idletasks 
}


proc xth_me_bind_image_drag {tagOrId imgx} {
  global xth
  $xth(me,can) bind $tagOrId <Double-Shift-1> "xth_me_image_start_drag $tagOrId $imgx %x %y"
  $xth(me,can) bind $tagOrId <Double-3> "xth_me_image_start_drag $tagOrId $imgx %x %y"
}


xth_app_create me "Map Editor" 

xth_ctrl_add me cmds "File commands"
xth_ctrl_add me prev "Command preview"
xth_ctrl_add me ss "Search & Select"
xth_ctrl_add me point "Point control"
xth_ctrl_add me line "Line control"
xth_ctrl_add me linept "Line point control"
xth_ctrl_add me scrap "Scrap control"
xth_ctrl_add me text "Text editor"
xth_ctrl_add me area "Drawing area"
xth_ctrl_add me images "Background images"
xth_ctrl_finish me

# global variables initialization
set xth(me,fltid) 0
set xth(me,fnewf) 0
set xth(me,fopen) 0
set xth(me,fsave) 0
set xth(me,zoom) 100
set xth(me,zoomv) 100
set xth(me,area,xmin) 0
set xth(me,area,xmax) 1600
set xth(me,area,ymin) 0
set xth(me,area,ymax) 1200
set xth(me,undolist) {}
set xth(me,redolist) {}
set xth(me,unredook) 0
set xth(me,unredola) {}

set xth(ctrl,me,images,posx) ""
set xth(ctrl,me,images,posy) ""
set xth(ctrl,me,images,vis) 0
set xth(ctrl,me,images,gamma) 0.0
set xth(ctrl,me,area,xmin) ""
set xth(ctrl,me,area,xmax) ""
set xth(ctrl,me,area,ymin) ""
set xth(ctrl,me,area,ymax) ""

set xth(ctrl,me,scrap,name) ""
set xth(ctrl,me,scrap,projection) ""
set xth(ctrl,me,scrap,options) ""
set xth(ctrl,me,scrap,px1) ""
set xth(ctrl,me,scrap,py1) ""
set xth(ctrl,me,scrap,px2) ""
set xth(ctrl,me,scrap,py2) ""
set xth(ctrl,me,scrap,rx1) ""
set xth(ctrl,me,scrap,ry1) ""
set xth(ctrl,me,scrap,rx2) ""
set xth(ctrl,me,scrap,ry2) ""
set xth(ctrl,me,scrap,units) ""
set xth(ctrl,me,scrap,pp1) {}
set xth(ctrl,me,scrap,pp2) {}

set xth(ctrl,me,point,x) {}
set xth(ctrl,me,point,y) {}
set xth(ctrl,me,point,type) {}
set xth(ctrl,me,point,name) {}
set xth(ctrl,me,point,opts) {}
set xth(ctrl,me,point,rot) {}
set xth(ctrl,me,point,xs) {}
set xth(ctrl,me,point,ys) {}
set xth(ctrl,me,point,rotid) 0
set xth(ctrl,me,point,xsid) 0
set xth(ctrl,me,point,ysid) 0

set xth(ctrl,me,line,type) {}
set xth(ctrl,me,line,name) {}
set xth(ctrl,me,line,opts) {}
set xth(ctrl,me,line,reverse) 0
set xth(ctrl,me,line,close) 0
set xth(ctrl,me,line,empty) {}
set xth(ctrl,me,linept,x) {}
set xth(ctrl,me,linept,y) {}
set xth(ctrl,me,linept,xp) {}
set xth(ctrl,me,linept,yp) {}
set xth(ctrl,me,linept,xn) {}
set xth(ctrl,me,linept,yn) {}
set xth(ctrl,me,linept,idp) 0
set xth(ctrl,me,linept,idn) 0
set xth(ctrl,me,linept,smooth) 0
set xth(ctrl,me,linept,rot) {}
set xth(ctrl,me,linept,rs) {}
set xth(ctrl,me,linept,ls) {}
set xth(ctrl,me,linept,rotid) 0
set xth(ctrl,me,linept,rsid) 0
set xth(ctrl,me,linept,lsid) 0

set xth(ctrl,me,ss,expr) "station"
set xth(ctrl,me,ss,regexp) 0
set xth(ctrl,me,ss,cases) 0

# initialize drawing area

xth_about_status "loading area module ..."

$xth(gui,me).af.apps configure -bg black
set canfm $xth(gui,me).af.apps.cf 
set xth(me,canf) $canfm
grid columnconf $xth(gui,me).af.apps 0 -weight 1
grid rowconf $xth(gui,me).af.apps 0 -weight 1
frame $canfm
set xth(me,can) $canfm.c
scrollbar $canfm.sv -orient vertical -command "$xth(me,can) yview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
scrollbar $canfm.sh -orient horizontal -command "$xth(me,can) xview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
canvas $xth(me,can) -relief flat -borderwidth 0 -bg black \
	-xscrollcommand "xth_me_area_scroll $canfm.sh" \
	-yscrollcommand "xth_me_area_scroll $canfm.sv" \
	-cursor crosshair
set xth(me,canid,area) [$xth(me,can) create polygon 0 0 0 256 256 256 256 0 -fill LightYellow]
set xth(me,canid,scrap,scp1) [$xth(me,can) create rectangle 0 0 3 3 \
  -fill red -outline red -width 1 -state hidden -tags {cmd_ctrl}]
set xth(me,canid,scrap,scp2) [$xth(me,can) create rectangle 0 0 3 3 \
  -fill red -outline red -width 1 -state hidden -tags {cmd_ctrl}]

xth_me_bind_area_drag $xth(me,canid,area) {}
xth_me_area_adjust 0 0 1600 1200

set xth(me,canid,point,selector) [$xth(me,can) create oval 0 0 10 10 \
  -fill {} -outline red -width 1 -state hidden -tags {cmd_ctrl}]
set xth(me,canid,point,fill) [$xth(me,can) create polygon 0 0 10 0 10 10 0 10 \
  -fill red -stipple gray12 -state hidden -smooth on -tags {ptfill cmd_ctrl} -width 1]
set xth(me,canid,point,fx) [$xth(me,can) create line 0 0 10 10 \
  -width 5 -fill red -arrow last -arrowshape {9 12 4} \
  -state hidden -tags {ptfill cmd_ctrl}]
set xth(me,canid,point,fy) [$xth(me,can) create line 0 0 10 10 \
  -width 5 -fill red -arrow last -arrowshape {9 12 4} \
  -state hidden -tags {ptfill cmd_ctrl}]
#set xth(me,canid,point,fxc) [$xth(me,can) create oval 0 0 10 10 \
#  -outline red -fill red  -width 1 -state hidden -tags {ptfill cmd_ctrl}]
#set xth(me,canid,point,fyc) [$xth(me,can) create oval 0 0 10 10 \
#  -outline red -fill red  -width 1 -state hidden -tags {ptfill cmd_ctrl}]

xth_me_bind_area_drag $xth(me,canid,point,fill) {}
xth_me_bind_area_drag $xth(me,canid,point,selector) {}
xth_me_bind_area_only_drag $xth(me,canid,point,fx)
xth_me_bind_area_only_drag $xth(me,canid,point,fy)

set xth(me,canid,linept,selector) [$xth(me,can) create oval 0 0 10 10 \
  -fill {} -outline red -width 1 -state hidden -tags {linectrl cmd_ctrl}]
set xth(me,canid,linept,fr) [$xth(me,can) create line 0 0 10 10 \
  -width 5 -fill red -arrow last -arrowshape {9 12 4} \
  -state hidden -tags {linectrl cmd_ctrl}]
set xth(me,canid,linept,fl) [$xth(me,can) create line 0 0 10 10 \
  -width 5 -fill red -arrow last -arrowshape {9 12 4} \
  -state hidden -tags {linectrl cmd_ctrl}]

set xth(me,canid,linept,ppcpl) [$xth(me,can) create line 0 0 10 10 \
  -width 2 -fill magenta -state hidden -tags "linectrl lineptppcp cmd_ctrl"]
set xth(me,canid,linept,nncpl) [$xth(me,can) create line 0 0 10 10 \
  -width 2 -fill magenta -state hidden -tags "linectrl lineptnncp cmd_ctrl"]
set xth(me,canid,linept,ppcp) [$xth(me,can) create rectangle 0 0 10 10 \
  -width 1 -fill magenta -outline magenta -state hidden -tags "linectrl lineptppcp cmd_ctrl"]
set xth(me,canid,linept,nncp) [$xth(me,can) create rectangle 0 0 10 10 \
  -width 1 -fill magenta -outline magenta -state hidden -tags "linectrl lineptnncp cmd_ctrl"]

set xth(me,canid,linept,pcpl) [$xth(me,can) create line 0 0 10 10 \
  -width 2 -fill red -state hidden -tags "linectrl lineptpcp cmd_ctrl"]
set xth(me,canid,linept,ncpl) [$xth(me,can) create line 0 0 10 10 \
  -width 2 -fill red -state hidden -tags "linectrl lineptncp cmd_ctrl"]
set xth(me,canid,linept,pcp) [$xth(me,can) create rectangle 0 0 10 10 \
  -width 1 -fill red -outline red -state hidden -tags "linectrl lineptpcp cmd_ctrl"]
set xth(me,canid,linept,ncp) [$xth(me,can) create rectangle 0 0 10 10 \
  -width 1 -fill red -outline red -state hidden -tags "linectrl lineptncp cmd_ctrl"]
  
xth_me_bind_area_only_drag $xth(me,canid,linept,fr)
xth_me_bind_area_only_drag $xth(me,canid,linept,fl)
xth_me_bind_area_drag $xth(me,canid,linept,pcpl) {}
xth_me_bind_area_drag $xth(me,canid,linept,ncpl) {}
xth_me_bind_area_drag $xth(me,canid,linept,ppcpl) {}
xth_me_bind_area_drag $xth(me,canid,linept,nncpl) {}
xth_me_bind_area_drag $xth(me,canid,linept,selector) {}
  
grid columnconf $canfm 0 -weight 1
grid rowconf $canfm 0 -weight 1
grid $xth(me,can) -column 0 -row 0 -sticky news
xth_scroll_showcmd $canfm.sv "grid $canfm.sv -column 1 -row 0 -sticky news"
xth_scroll_hidecmd $canfm.sv "grid forget $canfm.sv"
xth_scroll_showcmd $canfm.sh "grid $canfm.sh -column 0 -row 1 -sticky news"
xth_scroll_hidecmd $canfm.sh "grid forget $canfm.sh"


Label $xth(ctrl,me,area).l -text "" -anchor center -font $xth(gui,lfont) -state disabled
xth_status_bar me $xth(ctrl,me,area).l "Current drawing area."
Entry $xth(ctrl,me,area).xmin -font $xth(gui,lfont) -state disabled -width 4 -textvariable xth(ctrl,me,area,xmin)
xth_status_bar me $xth(ctrl,me,area).xmin "X min."
Entry $xth(ctrl,me,area).ymin -font $xth(gui,lfont) -state disabled -width 4 -textvariable xth(ctrl,me,area,ymin)
xth_status_bar me $xth(ctrl,me,area).ymin "Y min."
Entry $xth(ctrl,me,area).xmax -font $xth(gui,lfont) -state disabled -width 4 -textvariable xth(ctrl,me,area,xmax)
xth_status_bar me $xth(ctrl,me,area).xmax "X max."
Entry $xth(ctrl,me,area).ymax -font $xth(gui,lfont) -state disabled -width 4 -textvariable xth(ctrl,me,area,ymax)
xth_status_bar me $xth(ctrl,me,area).ymax "Y max."
Button $xth(ctrl,me,area).mab -text "Adjust" -anchor center -font $xth(gui,lfont) -state disabled -width 12 \
  -command {
    xth_me_area_adjust $xth(ctrl,me,area,xmin) $xth(ctrl,me,area,ymin) \
      $xth(ctrl,me,area,xmax) $xth(ctrl,me,area,ymax) 
  }
xth_status_bar me $xth(ctrl,me,area).mab "Adjust drawing area to given limits."
Button $xth(ctrl,me,area).aab -text "Auto adjust" -anchor center -font $xth(gui,lfont) -state disabled -width 12 \
  -command xth_me_area_auto_adjust
xth_status_bar me $xth(ctrl,me,area).aab "Adjust drawing area to automatically calculated limits."
Label $xth(ctrl,me,area).zl -text "zoom" -anchor e -font $xth(gui,lfont) -state disabled
menubutton $xth(ctrl,me,area).zb -text "100 %" -anchor center -font $xth(gui,lfont) \
  -indicatoron true -menu $xth(ctrl,me,area).zb.m -state disabled
menu $xth(ctrl,me,area).zb.m -tearoff 0 -font $xth(gui,lfont)
$xth(ctrl,me,area).zb.m add radiobutton -label "25 %" -variable xth(me,zoomv) -value 25 -command "xth_me_area_zoom_to 25"
$xth(ctrl,me,area).zb.m add radiobutton -label "50 %" -variable xth(me,zoomv) -value 50 -command "xth_me_area_zoom_to 50"
$xth(ctrl,me,area).zb.m add radiobutton -label "100 %" -variable xth(me,zoomv) -value 100 -command "xth_me_area_zoom_to 100"
$xth(ctrl,me,area).zb.m add radiobutton -label "200 %" -variable xth(me,zoomv) -value 200 -command "xth_me_area_zoom_to 200"
$xth(ctrl,me,area).zb.m add radiobutton -label "400 %" -variable xth(me,zoomv) -value 400 -command "xth_me_area_zoom_to 400"
xth_status_bar me $xth(ctrl,me,area).zb "Zoom drawing area."
xth_status_bar me $xth(ctrl,me,area).zl "Zoom drawing area."

xth_me_bind_entry_focus_return "$xth(ctrl,me,area).xmin $xth(ctrl,me,area).ymin $xth(ctrl,me,area).xmax $xth(ctrl,me,area).ymax" {
    xth_me_area_adjust $xth(ctrl,me,area,xmin) $xth(ctrl,me,area,ymin) \
      $xth(ctrl,me,area,xmax) $xth(ctrl,me,area,ymax) 
}
xth_me_bind_entry_focusin "$xth(ctrl,me,area).xmin $xth(ctrl,me,area).ymin $xth(ctrl,me,area).xmax $xth(ctrl,me,area).ymax"


grid columnconf $xth(ctrl,me,area) 0 -weight 1
grid columnconf $xth(ctrl,me,area) 1 -weight 1
grid columnconf $xth(ctrl,me,area) 2 -weight 1
grid columnconf $xth(ctrl,me,area) 3 -weight 1
grid $xth(ctrl,me,area).l -column 0 -row 0 -columnspan 4 -sticky news
grid $xth(ctrl,me,area).xmin -column 0 -row 1 -sticky news -padx 1
grid $xth(ctrl,me,area).ymin -column 1 -row 1 -sticky news -padx 1
grid $xth(ctrl,me,area).xmax -column 2 -row 1 -sticky news -padx 1
grid $xth(ctrl,me,area).ymax -column 3 -row 1 -sticky news -padx 1
grid $xth(ctrl,me,area).mab -column 0 -row 2 -columnspan 2 -sticky news
grid $xth(ctrl,me,area).aab -column 2 -row 2 -columnspan 2 -sticky news
grid $xth(ctrl,me,area).zl -column 0 -row 3 -columnspan 2 -sticky news
grid $xth(ctrl,me,area).zb -column 2 -row 3 -columnspan 2 -sticky news


xth_about_status "loading commands module ..."

# initialize file commands
set clbox $xth(ctrl,me,cmds).cl
set ccbox $xth(ctrl,me,cmds).cc
frame $clbox
frame $ccbox
listbox $clbox.l -height 8 -selectmode single -takefocus 0 \
  -yscrollcommand "xth_scroll $clbox.sv" \
  -xscrollcommand "xth_scroll $clbox.sh" \
  -font $xth(gui,lfont) -exportselection no \
  -listvariable xth(me,cmds,list) -selectborderwidth 0
scrollbar $clbox.sv -orient vert  -command "$clbox.l yview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
scrollbar $clbox.sh -orient horiz  -command "$clbox.l xview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
bind $clbox.l <<ListboxSelect>> {xth_me_cmds_select {}}
bind $clbox.l <B1-ButtonRelease> "focus $clbox.l"

grid columnconf $clbox 0 -weight 1
grid rowconf $clbox 0 -weight 1
grid $clbox.l -column 0 -row 0 -sticky news
xth_scroll_showcmd $clbox.sv "grid $clbox.sv -column 1 -row 0 -sticky news"
xth_scroll_hidecmd $clbox.sv "grid forget $clbox.sv"
xth_scroll_showcmd $clbox.sh "grid $clbox.sh -column 0 -row 1 -sticky news"
xth_scroll_hidecmd $clbox.sh "grid forget $clbox.sh"
xth_status_bar me $clbox "Select command."

grid columnconf $xth(ctrl,me,cmds) 0 -weight 1
grid $clbox -column 0 -row 0 -sticky news
grid $ccbox -column 0 -row 1 -sticky news

Button $ccbox.go -text "Insert scrap" -anchor center -font $xth(gui,lfont) \
  -state disabled -command xth_me_cmds_action
xth_status_bar me $ccbox.go "Action button."
Button $ccbox.sel -text "Select" -anchor center -font $xth(gui,lfont) \
  -state disabled -command {xth_me_cmds_set_mode 0}
xth_status_bar me $ccbox.sel "Switch mouse mode to select objects."
menubutton $ccbox.cfg -text "Action" -anchor center -font $xth(gui,lfont) \
  -indicatoron true -menu $ccbox.cfg.m -state disabled
xth_status_bar me $ccbox.cfg "Configure action assigned to action button."
menu $ccbox.cfg.m -tearoff 0 -font $xth(gui,lfont)
$ccbox.cfg.m add command -label "Insert line" -command {xth_me_cmds_set_action 0}
$ccbox.cfg.m add command -label "Insert point" -command {xth_me_cmds_set_action 1}
$ccbox.cfg.m add command -label "Insert scrap" -command {xth_me_cmds_set_action 2}
$ccbox.cfg.m add command -label "Insert text" -command {xth_me_cmds_set_action 3}
$ccbox.cfg.m add separator
$ccbox.cfg.m add command -label "Delete" -command {xth_me_cmds_set_action 4}
Button $ccbox.mu -text "Move up" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 8 -command "xth_me_cmds_move_up {}"
Button $ccbox.md -text "Move down" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 8 -command "xth_me_cmds_move_down {}"
grid columnconf $ccbox 0 -weight 1
grid columnconf $ccbox 1 -weight 1
grid $ccbox.go -column 0 -row 0 -columnspan 2 -sticky news
grid $ccbox.cfg $ccbox.sel -row 1 -sticky news
grid $ccbox.mu $ccbox.md -row 2 -sticky news

# initialize text editor
set txb $xth(ctrl,me,text)
text $txb.txt -height 6 -wrap none -font $xth(gui,efont) \
  -bg $xth(gui,ecolorbg) \
  -fg $xth(gui,ecolorfg) -insertbackground $xth(gui,ecolorfg) \
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
Button $txb.upd -text "Update text" -anchor center -font $xth(gui,lfont) \
  -state disabled \
  -command {xth_me_cmds_update {}}
grid columnconf $txb 0 -weight 1
grid rowconf $txb 0 -weight 1
grid $txb.txt -column 0 -row 0 -sticky news
grid $txb.sv -column 1 -row 0 -sticky news
grid $txb.sh -column 0 -row 1 -sticky news
grid $txb.upd -column 0 -row 2 -columnspan 2 -sticky news
xth_status_bar me $txb.txt "Editor for free text in therion 2D file."
bind $txb.txt <Control-Key-x> "tk_textCut $txb.txt"
bind $txb.txt <Control-Key-c> "tk_textCopy $txb.txt"
bind $txb.txt <Control-Key-v> "tk_textPaste $txb.txt"

if {$xth(gui,bindinsdel)} {
  bind $txb.txt <Shift-Key-Delete> "tk_textCut $txb.txt"
  bind $txb.txt <Control-Key-Insert> "tk_textCopy $txb.txt"
  bind $txb.txt <Shift-Key-Insert> "tk_textPaste $txb.txt"
#  catch {
#    bind $txb.txt <Shift-Key-KP_Decimal> "tk_textCut $txb.txt"
#    bind $txb.txt <Control-Key-KP_Insert> "tk_textCopy $txb.txt"
#    bind $txb.txt <Shift-Key-KP_0> "tk_textPaste $txb.txt"
#  }
}

if {[info exists xth(gui,te)]} {
  bind $txb.txt <Control-Key-a> "xth_te_text_select_all %W"
  bind $txb.txt <Control-Key-i> "xth_te_text_auto_indent %W"
  bind $txb.txt <Tab> $xth(te,bind,text_tab)
  bind $txb.txt <Return> $xth(te,bind,text_return)
} else {
  bind $txb.txt <Tab> $xth(gui,bind,text_tab)
  bind $txb.txt <Return> $xth(gui,bind,text_return)
}



# initialize search & select tool
set ssbx $xth(ctrl,me,ss)

Label $ssbx.xl -text expression -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $ssbx.xl "Enter search expression."
Entry $ssbx.xe -font $xth(gui,lfont) -state disabled \
  -textvariable xth(ctrl,me,ss,expr) -width 3
bind $ssbx.xe <Return> xth_me_ss_show
xth_status_bar me $ssbx.xe "Enter search expression."
checkbutton $ssbx.rx -text "regular expression" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,ss,regexp) -command {}
xth_status_bar me $ssbx.rx "Search for regular expression."
checkbutton $ssbx.cs -text "case sensitive" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,ss,cases) -command {}
xth_status_bar me $ssbx.cs "Case sensitive search."
Button $ssbx.sn -text "Find next" -anchor center -font $xth(gui,lfont) \
  -state disabled -command xth_me_ss_next -width 10
xth_status_bar me $ssbx.sn "Select next object matching expression."
Button $ssbx.sf -text "Find first" -anchor center -font $xth(gui,lfont) \
  -state disabled -command xth_me_ss_first -width 10
xth_status_bar me $ssbx.sf "Select first object matching expression."
Button $ssbx.sa -text "Show all" -anchor center -font $xth(gui,lfont) \
  -state disabled -command xth_me_ss_show -width 9
xth_status_bar me $ssbx.sa "Highlight all objects matching expression."
Button $ssbx.ca -text "Clear all" -anchor center -font $xth(gui,lfont) \
  -state disabled -command xth_me_cmds_set_colors -width 9
xth_status_bar me $ssbx.ca "Clear highlighted objects."

grid columnconf $ssbx 0 -weight 1
grid columnconf $ssbx 1 -weight 1
grid $ssbx.xl -column 0 -row 0 -sticky news
grid $ssbx.xe -column 1 -row 0 -sticky news
grid $ssbx.cs -column 0 -row 1 -sticky news -columnspan 2
grid $ssbx.rx -column 0 -row 2 -sticky news -columnspan 2
grid $ssbx.sf -column 0 -row 3 -sticky news
grid $ssbx.sn -column 1 -row 3 -sticky news
grid $ssbx.sa -column 0 -row 4 -sticky news
grid $ssbx.ca -column 1 -row 4 -sticky news


xth_about_status "loading images module ..."

# initialize images
frame $xth(ctrl,me,images).il
set ilbox $xth(ctrl,me,images).il.ilbox 
set xth(me,imgs,list) {}
listbox $ilbox -height 4 -selectmode single -takefocus 0 \
  -yscrollcommand "xth_scroll $xth(ctrl,me,images).il.sv" \
  -xscrollcommand "xth_scroll $xth(ctrl,me,images).il.sh" \
  -font $xth(gui,lfont) -exportselection no \
  -listvariable xth(me,imgs,list) -selectborderwidth 0
scrollbar $xth(ctrl,me,images).il.sv -orient vert  -command "$ilbox yview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
scrollbar $xth(ctrl,me,images).il.sh -orient horiz  -command "$ilbox xview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
frame $xth(ctrl,me,images).ic
bind $ilbox <<ListboxSelect>> "xth_me_image_select \[lindex \[%W curselection\] 0\]"

grid columnconf $xth(ctrl,me,images).il 0 -weight 1
grid rowconf $xth(ctrl,me,images).il 0 -weight 1
grid $ilbox -column 0 -row 0 -sticky news
xth_scroll_showcmd $xth(ctrl,me,images).il.sv "grid $xth(ctrl,me,images).il.sv -column 1 -row 0 -sticky news"
xth_scroll_hidecmd $xth(ctrl,me,images).il.sv "grid forget $xth(ctrl,me,images).il.sv"
xth_scroll_showcmd $xth(ctrl,me,images).il.sh "grid $xth(ctrl,me,images).il.sh -column 0 -row 1 -sticky news"
xth_scroll_hidecmd $xth(ctrl,me,images).il.sh "grid forget $xth(ctrl,me,images).il.sh"
xth_status_bar me $ilbox "Select background image."
grid columnconf $xth(ctrl,me,images) 0 -weight 1
grid $xth(ctrl,me,images).il -column 0 -row 0 -sticky news
grid $xth(ctrl,me,images).ic -column 0 -row 1 -sticky news

Button $xth(ctrl,me,images).ic.insp -text "Insert" -anchor center -font $xth(gui,lfont) \
  -state disabled -command {xth_me_image_insert $xth(ctrl,me,images,posx) $xth(ctrl,me,images,posy) {} 0 {}}
xth_status_bar me $xth(ctrl,me,images).ic.insp "Insert new background image."
Button $xth(ctrl,me,images).ic.remp -text "Remove" -anchor center -font $xth(gui,lfont) -state disabled \
  -command {xth_me_image_remove ""}
xth_status_bar me $xth(ctrl,me,images).ic.remp "Remove selected image."
Separator $xth(ctrl,me,images).ic.s1 -orient horizontal
checkbutton $xth(ctrl,me,images).ic.viscb -text "visibility" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,images,vis) -command {xth_me_image_toggle_vsb ""}
xth_status_bar me $xth(ctrl,me,images).ic.viscb "Switch image visibility."

Label $xth(ctrl,me,images).ic.gl -text "gamma 1.00" -anchor w -font $xth(gui,lfont) -state disabled
xth_status_bar me $xth(ctrl,me,images).ic.gl "Control gamma value."
Button $xth(ctrl,me,images).ic.gr -text "Reset" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 8 -command "set xth(ctrl,me,images,gamma) 0.0; xth_me_image_update_gamma"
xth_status_bar me $xth(ctrl,me,images).ic.gr "Reset image gamma value."
scale $xth(ctrl,me,images).ic.gs -from -1.0 -to 1.0 \
  -font $xth(gui,lfont) -state disabled -showvalue 0 -resolution 0.01 \
  -variable xth(ctrl,me,images,gamma) -orient horiz
bind $xth(ctrl,me,images).ic.gs <B1-ButtonRelease> xth_me_image_update_gamma
xth_status_bar me $xth(ctrl,me,images).ic.gs "Set image gamma value."


Label $xth(ctrl,me,images).ic.posl -text position -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $xth(ctrl,me,images).ic.posl "Current position of selected image."
Label $xth(ctrl,me,images).ic.posln -text "" -anchor center -font $xth(gui,lfont) -state disabled
xth_status_bar me $xth(ctrl,me,images).ic.posln "Current position of selected image."
Button $xth(ctrl,me,images).ic.posch -text "Move to" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 8 -command "xth_me_image_move_to"
xth_status_bar me $xth(ctrl,me,images).ic.posch "Move image to given position."
Entry $xth(ctrl,me,images).ic.posx -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,images,posx)
xth_status_bar me $xth(ctrl,me,images).ic.posx "New X coordinate of image."
Entry $xth(ctrl,me,images).ic.posy -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,images,posy)
xth_status_bar me $xth(ctrl,me,images).ic.posy "New Y coordinate of image."
Button $xth(ctrl,me,images).ic.mvf -text "Move front" -anchor center -font $xth(gui,lfont) -state disabled -width 10 \
  -command xth_me_image_move_front
xth_status_bar me $xth(ctrl,me,images).ic.mvf "Move image in front of all images."
Button $xth(ctrl,me,images).ic.mvb -text "Move back" -anchor center -font $xth(gui,lfont) -state disabled -width 10 \
  -command xth_me_image_move_back
xth_status_bar me $xth(ctrl,me,images).ic.mvb "Move image behind all images."

xth_me_bind_entry_focus_return "$xth(ctrl,me,images).ic.posx $xth(ctrl,me,images).ic.posy" "xth_me_image_move_to"
xth_me_bind_entry_focusin "$xth(ctrl,me,images).ic.posx $xth(ctrl,me,images).ic.posy"

grid columnconf $xth(ctrl,me,images).ic 0 -weight 1
grid columnconf $xth(ctrl,me,images).ic 1 -weight 1
grid columnconf $xth(ctrl,me,images).ic 2 -weight 1
grid columnconf $xth(ctrl,me,images).ic 3 -weight 1
grid $xth(ctrl,me,images).ic.insp -column 0 -row 0 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.remp -column 2 -row 0 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.s1 -column 0 -row 1 -sticky news -columnspan 4 -pady 3
grid $xth(ctrl,me,images).ic.posl -column 0 -row 2 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.posln -column 2 -row 2 -sticky news -columnspan 2 -padx 1
grid $xth(ctrl,me,images).ic.posch -column 0 -row 3 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.posx -column 2 -row 3 -sticky ew -padx 1
grid $xth(ctrl,me,images).ic.posy -column 3 -row 3 -sticky ew -padx 1
grid $xth(ctrl,me,images).ic.mvf -column 0 -row 4 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.mvb -column 2 -row 4 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.gl -column 0 -row 5 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.gr -column 2 -row 5 -sticky news -columnspan 2
grid $xth(ctrl,me,images).ic.gs -column 0 -row 6 -sticky news -columnspan 4
grid $xth(ctrl,me,images).ic.viscb -column 0 -row 7 -sticky news -columnspan 4
# xth_status_bar me $xth(ctrl,me,images). "To set file encoding, type encoding name and press <Change> button."


xth_about_status "loading preview module ..."

# init command preview
set txb $xth(ctrl,me,prev)
text $txb.txt -height 4 -wrap none -font $xth(gui,efont) \
  -bg $xth(gui,ecolorbg) \
  -fg $xth(gui,ecolorfg) -insertbackground $xth(gui,ecolorfg) \
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
Button $txb.upd -text "Update command" -anchor center -font $xth(gui,lfont) \
  -state disabled -command {xth_me_cmds_update {}}
grid columnconf $txb 0 -weight 1
grid rowconf $txb 0 -weight 1
grid $txb.txt -column 0 -row 0 -sticky news
grid $txb.sv -column 1 -row 0 -sticky news
grid $txb.sh -column 0 -row 1 -sticky news
grid $txb.upd -column 0 -row 2 -columnspan 2 -sticky news
xth_status_bar me $txb.txt "Command preview."

# init scrap control

xth_about_status "loading scrap module ..."

set sfm $xth(ctrl,me,scrap)

Label $sfm.namel -text id -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $sfm.namel "Scrap name."
Entry $sfm.name -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,name)
xth_status_bar me $sfm.name "Scrap name."

Label $sfm.projl -text projection -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $sfm.projl "Scrap projection."
ComboBox $sfm.proj -values $xth(scrap_projections) \
  -font $xth(gui,lfont) -height 4 -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,projection) -command {xth_me_cmds_update {}}

xth_status_bar me $sfm.proj "Scrap projection."

Label $sfm.optl -text options -anchor e -font $xth(gui,lfont) -state disabled -width 8
xth_status_bar me $sfm.optl "Other scrap options."
Entry $sfm.opt -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,options)

xth_status_bar me $sfm.opt "Other scrap options." 

#Separator $sfm.s1 -orient horizontal
Label $sfm.scl -text scale -anchor sw -font $xth(gui,lfont) -state disabled
xth_status_bar me $sfm.scl "Scrap scale definition."
Button $sfm.scpb -text "Update scrap" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 4 -command {xth_me_cmds_update {}}
xth_status_bar me $sfm.scpb "Click twice on the drawing area to set picture scale points."
Label $sfm.scpp -text "picture scale points" -anchor w -font $xth(gui,lfont) -state disabled
xth_status_bar me $sfm.scpp "Calibration points on the picture (X1:Y1 - X2:Y2)."
Entry $sfm.scx1p -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,px1)
xth_status_bar me $sfm.scx1p "X1 picture." 
Entry $sfm.scy1p -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,py1)
xth_status_bar me $sfm.scy1p "Y1 picture." 
Entry $sfm.scx2p -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,px2)
xth_status_bar me $sfm.scx2p "X2 picture." 
Entry $sfm.scy2p -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,py2)
xth_status_bar me $sfm.scy2p "Y2 picture." 
Label $sfm.scrp -text "real scale points" -anchor w -font $xth(gui,lfont) -state disabled
xth_status_bar me $sfm.scrp "Real coordinates of calibration points (X1:Y1 - X2:Y2)."
Entry $sfm.scx1r -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,rx1)
xth_status_bar me $sfm.scx1r "X1 real." 
Entry $sfm.scy1r -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,ry1)
xth_status_bar me $sfm.scy1r "Y1 real." 
Entry $sfm.scx2r -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,rx2)
xth_status_bar me $sfm.scx2r "X2 real." 
Entry $sfm.scy2r -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,ry2)
xth_status_bar me $sfm.scy2r "Y2 real." 
Label $sfm.scul -text "units" -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $sfm.scul "Units of real coordinates."
ComboBox $sfm.scu -values $xth(length_units) \
  -font $xth(gui,lfont) -height 5 -state disabled -width 4 \
  -textvariable xth(ctrl,me,scrap,units) -command {xth_me_cmds_update {}}
xth_status_bar me $sfm.scu "Units of real coordinates."

xth_me_bind_entry_focus_return "$sfm.scx1p $sfm.scy1p $sfm.scx2p $sfm.scy2p" {xth_me_cmds_update {}}
xth_me_bind_entry_focus_return "$sfm.scx1r $sfm.scy1r $sfm.scx2r $sfm.scy2r" {xth_me_cmds_update {}}
xth_me_bind_entry_return "$sfm.name $sfm.opt" {xth_me_cmds_update {}}
xth_me_bind_entry_focusin "$sfm.name $sfm.opt $sfm.scx1p $sfm.scy1p $sfm.scx2p $sfm.scy2p $sfm.scx1r $sfm.scy1r $sfm.scx2r $sfm.scy2r"

grid columnconf $sfm 0 -weight 1
grid columnconf $sfm 1 -weight 1
grid columnconf $sfm 2 -weight 1
grid columnconf $sfm 3 -weight 1
grid $sfm.namel -row 0 -column 0 -columnspan 2 -sticky news
grid $sfm.name  -row 0 -column 2 -columnspan 2 -sticky news -padx 1
grid $sfm.projl -row 1 -column 0 -columnspan 2 -sticky news
grid $sfm.proj  -row 1 -column 2 -columnspan 2 -sticky news -padx 2
grid $sfm.optl -row 2 -column 0 -columnspan 2 -sticky news
grid $sfm.opt  -row 2 -column 2 -columnspan 2 -sticky news -padx 1
#grid $sfm.s1 -column 0 -row 3 -sticky news -columnspan 4 -pady 3
grid $sfm.scl -row 3 -column 0 -columnspan 2 -sticky news
grid $sfm.scpb -row 3 -column 2 -columnspan 2 -sticky news
grid $sfm.scpp -column 0 -row 4 -sticky news -columnspan 4
grid $sfm.scx1p $sfm.scy1p $sfm.scx2p $sfm.scy2p -row 5 -sticky news -padx 1
grid $sfm.scrp -column 0 -row 6 -sticky news -columnspan 4
grid $sfm.scx1r $sfm.scy1r $sfm.scx2r $sfm.scy2r -row 7 -sticky news -padx 1
grid $sfm.scul -row 8 -column 0 -columnspan 2 -sticky news
grid $sfm.scu  -row 8 -column 2 -columnspan 2 -sticky news -padx 2


# point control
xth_about_status "loading point module ..."

set ptc $xth(ctrl,me,point)
Label $ptc.posl -text "position" -anchor e -font $xth(gui,lfont) -state disabled -width 8
xth_status_bar me $ptc.posl "Point position."
Entry $ptc.posx -font $xth(gui,lfont) -state disabled -width 4 -textvariable xth(ctrl,me,point,x)
xth_status_bar me $ptc.posx "Point X coordinate." 
Entry $ptc.posy -font $xth(gui,lfont) -state disabled -width 4 -textvariable xth(ctrl,me,point,y)
xth_status_bar me $ptc.posy "Point Y coordinate." 

Button $ptc.upd -text "Update point" -anchor center -font $xth(gui,lfont) \
  -state disabled -command {xth_me_cmds_update {}}
xth_status_bar me $ptc.upd "Update point data." 

Label $ptc.typl -text "type" -anchor e -font $xth(gui,lfont) -state disabled -width 8
xth_status_bar me $ptc.typl "Point type."
ComboBox $ptc.typ -values $xth(point_types) \
  -font $xth(gui,lfont) -height 8 -state disabled -width 4 \
  -textvariable xth(ctrl,me,point,type) -command {xth_me_cmds_update {}}
xth_status_bar me $ptc.typ "Point type." 

Label $ptc.namel -text "id" -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $ptc.namel "Point name."
Entry $ptc.name -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,point,name)
xth_status_bar me $ptc.name "Point name." 

Label $ptc.optl -text "options" -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $ptc.optl "Other point options."
Entry $ptc.opt -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,point,opts)
xth_status_bar me $ptc.opt "Other point options." 
Separator $ptc.s1 -orient horizontal

checkbutton $ptc.rotc -text "orientation" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,point,rotid) -command xth_me_cmds_point_change_state
xth_status_bar me $ptc.rotc "Set point rotation."
Entry $ptc.rot -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,point,rot)
xth_status_bar me $ptc.rot "Enter point rotation."

checkbutton $ptc.xszc -text "x-size" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,point,xsid) -command xth_me_cmds_point_change_state
xth_status_bar me $ptc.xszc "Set point size in main direction."
Entry $ptc.xsz -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,point,xs)
xth_status_bar me $ptc.xsz "Enter point size in main direction."

checkbutton $ptc.yszc -text "y-size" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,point,ysid) -command xth_me_cmds_point_change_state
xth_status_bar me $ptc.yszc "Set point size in side direction."
Entry $ptc.ysz -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,point,ys)
xth_status_bar me $ptc.ysz "Enter point size in side direction."

xth_me_bind_entry_focus_return "$ptc.posx $ptc.posy" {xth_me_cmds_update {}}
xth_me_bind_entry_return "$ptc.name $ptc.opt $ptc.rot $ptc.xsz $ptc.ysz" {xth_me_cmds_update {}}
xth_me_bind_entry_focusin "$ptc.posx $ptc.posy $ptc.name $ptc.opt $ptc.rot $ptc.xsz $ptc.ysz"


grid columnconf $ptc 0 -weight 1
grid columnconf $ptc 1 -weight 1
grid columnconf $ptc 2 -weight 1
grid columnconf $ptc 3 -weight 1
grid $ptc.posl -row 0 -column 0 -columnspan 2 -sticky news
grid $ptc.posx -row 0 -column 2 -sticky news -padx 1
grid $ptc.posy -row 0 -column 3 -sticky news -padx 1
grid $ptc.typl -row 1 -column 0 -columnspan 2 -sticky news
grid $ptc.typ -row 1 -column 2 -columnspan 2 -sticky news -padx 2
grid $ptc.namel -row 2 -column 0 -columnspan 2 -sticky news
grid $ptc.name -row 2 -column 2 -columnspan 2 -sticky news -padx 1
grid $ptc.optl -row 3 -column 0 -columnspan 2 -sticky news
grid $ptc.opt -row 3 -column 2 -columnspan 2 -sticky news -padx 1
grid $ptc.s1 -row 4 -column 0 -columnspan 4 -sticky news -pady 3
grid $ptc.rotc -row 5 -column 0 -columnspan 2 -sticky news
grid $ptc.rot -row 5 -column 2 -columnspan 2 -sticky news -padx 1
grid $ptc.xszc -row 6 -column 0 -columnspan 2 -sticky news
grid $ptc.xsz -row 6 -column 2 -columnspan 2 -sticky news -padx 1
grid $ptc.yszc -row 7 -column 0 -columnspan 2 -sticky news
grid $ptc.ysz -row 7 -column 2 -columnspan 2 -sticky news -padx 1
grid $ptc.upd -row 8 -column 0 -columnspan 4 -sticky news


# line control
xth_about_status "loading line module ..."

set lnc $xth(ctrl,me,line)

Label $lnc.typl -text "type" -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $lnc.typl "Line type."
ComboBox $lnc.typ -values $xth(line_types) \
  -font $xth(gui,lfont) -height 8 -state disabled -width 4 \
  -textvariable xth(ctrl,me,line,type) \
  -command {xth_me_cmds_update {}}
xth_status_bar me $lnc.typ "Line type." 

Label $lnc.namel -text "id" -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $lnc.namel "Line name."
Entry $lnc.name -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,line,name)
xth_status_bar me $lnc.name "Line name." 

Label $lnc.optl -text "options" -anchor e -font $xth(gui,lfont) -state disabled
xth_status_bar me $lnc.optl "Other line options."
Entry $lnc.opt -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,line,opts)
xth_status_bar me $lnc.opt "Other line options." 

checkbutton $lnc.rev -text "reverse" -anchor w -font $xth(gui,lfont) \
  -state disabled \
  -variable xth(ctrl,me,line,reverse) \
  -command xth_me_cmds_toggle_line_reverse
xth_status_bar me $lnc.rev "Reverse line."
checkbutton $lnc.cls -text "close" -anchor w -font $xth(gui,lfont) \
  -state disabled \
  -variable xth(ctrl,me,line,close) \
  -command xth_me_cmds_toggle_line_close
xth_status_bar me $lnc.cls "Close line."

set plf $lnc.pl
frame $plf
listbox $plf.l -height 4 -selectmode single -takefocus 0 \
  -yscrollcommand "xth_scroll $plf.sv" \
  -xscrollcommand "xth_scroll $plf.sh" \
  -font $xth(gui,lfont) -exportselection no \
  -selectborderwidth 0
scrollbar $plf.sv -orient vert  -command "$plf.l yview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
scrollbar $plf.sh -orient horiz  -command "$plf.l xview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
bind $plf.l <<ListboxSelect>> {
  if {$xth(me,fopen)} {
    xth_me_cmds_select_linept $xth(me,cmds,selid) \
    [lindex $xth(me,cmds,$xth(me,cmds,selid),xplist) \
    [lindex [%W curselection] 0]]
    if {$xth(me,cmds,selpid) > 0} {
      xth_me_center_to [list \
        $xth(me,cmds,$xth(me,cmds,selid),$xth(me,cmds,selpid),x) \
        $xth(me,cmds,$xth(me,cmds,selid),$xth(me,cmds,selpid),y)]
    }
  }
}
bind $plf.l <B1-ButtonRelease> "focus $plf.l"

menubutton $lnc.lpa -text "Edit line" -anchor w -font $xth(gui,lfont) \
  -indicatoron true -menu $lnc.lpa.m -state disabled -width 10
xth_status_bar me $lnc.lpa "Insert/delete line point. Split line."
Button $lnc.upd -text "Update" -anchor center -font $xth(gui,lfont) \
  -state disabled -command {xth_me_cmds_update {}} -width 10
xth_status_bar me $lnc.upd "Update line."

menu $lnc.lpa.m -tearoff 0 -font $xth(gui,lfont)
$lnc.lpa.m add command -label "Insert point" -command {xth_me_cmds_start_linept_insert} -state disabled
$lnc.lpa.m add command -label "Delete point" -command {xth_me_cmds_delete_linept {} {}} -state disabled
$lnc.lpa.m add command -label "Split line" -command {xth_me_cmds_line_split} -state disabled

#Button $lnc.insp -text "Insert" -anchor center -font $xth(gui,lfont) \
#  -state disabled -width 10 -command {xth_me_cmds_start_linept_insert}
#Button $lnc.delp -text "Delete" -anchor center -font $xth(gui,lfont) \
#  -state disabled -width 10 -command {xth_me_cmds_delete_linept {} {}}

grid columnconf $plf 0 -weight 1
grid rowconf $plf 0 -weight 1
grid $plf.l -column 0 -row 0 -sticky news
xth_scroll_showcmd $plf.sv "grid $plf.sv -column 1 -row 0 -sticky news"
xth_scroll_hidecmd $plf.sv "grid forget $plf.sv"
xth_scroll_showcmd $plf.sh "grid $plf.sh -column 0 -row 1 -sticky news"
xth_scroll_hidecmd $plf.sh "grid forget $plf.sh"
xth_status_bar me $plf "Select line point."


grid columnconf $lnc 0 -weight 1
grid columnconf $lnc 1 -weight 1
grid $lnc.typl -row 0 -column 0 -sticky news
grid $lnc.typ -row 0 -column 1 -sticky news -padx 2
grid $lnc.namel -row 1 -column 0 -sticky news
grid $lnc.name -row 1 -column 1 -sticky news -padx 1
grid $lnc.optl -row 2 -column 0 -sticky news
grid $lnc.opt -row 2 -column 1 -sticky news -padx 1
grid $lnc.rev -row 3 -column 0 -sticky news
grid $lnc.cls -row 3 -column 1 -sticky news
grid $plf -row 4 -column 0 -columnspan 2 -sticky news
grid $lnc.lpa -row 5 -column 0 -sticky news
grid $lnc.upd -row 5 -column 1 -sticky news
#grid $lnc.insp -row 5 -column 0 -sticky news
#grid $lnc.delp -row 5 -column 1 -sticky news

xth_me_bind_entry_return "$lnc.name $lnc.opt" {xth_me_cmds_update {}}
xth_me_bind_entry_focusin "$lnc.name $lnc.opt"

# line point control
xth_about_status "loading line point module ..."

set lpc $xth(ctrl,me,linept)

Label $lpc.posl -text "position" -anchor e -font $xth(gui,lfont) -state disabled -width 0
xth_status_bar me $lpc.posl "Point position."
Entry $lpc.posx -font $xth(gui,lfont) -state disabled \
  -textvariable xth(ctrl,me,linept,x) -width 0
xth_status_bar me $lpc.posx "Point X coordinate."
Entry $lpc.posy -font $xth(gui,lfont) -state disabled \
  -textvariable xth(ctrl,me,linept,y) -width 0
xth_status_bar me $lpc.posy "Point Y coordinate." 

Entry $lpc.xp -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,linept,xp)
xth_status_bar me $lpc.xp "Previous control point X coordinate." 
Entry $lpc.yp -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,linept,yp)
xth_status_bar me $lpc.yp "Previous control point Y coordinate." 
Entry $lpc.xn -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,linept,xn)
xth_status_bar me $lpc.xn "Next control point X coordinate." 
Entry $lpc.yn -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,me,linept,yn)
xth_status_bar me $lpc.yn "Next control point Y coordinate." 

checkbutton $lpc.cbp -text "<<" -anchor w -font $xth(gui,lfont) \
  -state disabled -width 0 \
  -variable xth(ctrl,me,linept,idp) \
  -command xth_me_cmds_toggle_linept
xth_status_bar me $lpc.cbp "Checkbox whether to use previous control point."
checkbutton $lpc.cbs -text "smooth" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,linept,smooth) -width 0\
  -command xth_me_cmds_toggle_linept
xth_status_bar me $lpc.cbs "Set line to be smooth in given point."
checkbutton $lpc.cbn -text ">>" -anchor w -font $xth(gui,lfont) \
  -state disabled -width 0 \
  -variable xth(ctrl,me,linept,idn) \
  -command xth_me_cmds_toggle_linept
xth_status_bar me $lpc.cbn "Checkbox whether to use next control point."

checkbutton $lpc.rotc -text "orientation" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,linept,rotid) -width 0 \
  -command xth_me_cmds_toggle_linept
xth_status_bar me $lpc.rotc "Set point rotation."
Entry $lpc.rot -font $xth(gui,lfont) -state disabled -width 0 \
  -textvariable xth(ctrl,me,linept,rot)
xth_status_bar me $lpc.rot "Enter point rotation."

checkbutton $lpc.rszc -text "r-size" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,linept,rsid) -width 0 \
  -command xth_me_cmds_toggle_linept
xth_status_bar me $lpc.rszc "Set line size in right direction."
Entry $lpc.rsz -font $xth(gui,lfont) -state disabled -width 0 \
  -textvariable xth(ctrl,me,linept,rs)
xth_status_bar me $lpc.rsz "Enter line size in right direction."

checkbutton $lpc.lszc -text "l-size" -anchor w -font $xth(gui,lfont) -state disabled \
  -variable xth(ctrl,me,linept,lsid) -width 0  \
  -command xth_me_cmds_toggle_linept
xth_status_bar me $lpc.lszc "Set line size in left direction."
Entry $lpc.lsz -font $xth(gui,lfont) -state disabled -width 0 \
  -textvariable xth(ctrl,me,linept,ls)
xth_status_bar me $lpc.lsz "Enter line size in left direction."

Label $lpc.optl -text "options" -anchor sw -font $xth(gui,lfont) -state disabled \
 -width 0
xth_status_bar me $lpc.optl "Line point options editor."
Button $lpc.upd -text "Update" -anchor center -font $xth(gui,lfont) \
  -state disabled -command {xth_me_cmds_update {}} -width 0
xth_status_bar me $lpc.upd "Update line point."

set txb $lpc.oe
frame $txb
text $txb.txt -height 2 -wrap none -font $xth(gui,efont) \
  -bg $xth(gui,ecolorbg) \
  -fg $xth(gui,ecolorfg) -insertbackground $xth(gui,ecolorfg) \
  -relief sunken -state disabled \
  -selectbackground $xth(gui,ecolorselbg) \
  -selectforeground $xth(gui,ecolorselfg) \
  -yscrollcommand "$txb.sv set" \
  -selectborderwidth 0 \
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
xth_status_bar me $txb "Editor for line point options."
bind $txb.txt <Control-Key-x> "tk_textCut $txb.txt"
bind $txb.txt <Control-Key-c> "tk_textCopy $txb.txt"
bind $txb.txt <Control-Key-v> "tk_textPaste $txb.txt"

if {$xth(gui,bindinsdel)} {
  bind $txb.txt <Shift-Key-Delete> "tk_textCut $txb.txt"
  bind $txb.txt <Control-Key-Insert> "tk_textCopy $txb.txt"
  bind $txb.txt <Shift-Key-Insert> "tk_textPaste $txb.txt"
#  catch {
#    bind $txb.txt <Shift-Key-KP_Decimal> "tk_textCut $txb.txt"
#    bind $txb.txt <Control-Key-KP_Insert> "tk_textCopy $txb.txt"
#    bind $txb.txt <Shift-Key-KP_0> "tk_textPaste $txb.txt"
#  }
}

bind $txb.txt <Tab> $xth(gui,bind,text_tab)
bind $txb.txt <Return> $xth(gui,bind,text_return)

grid columnconf $lpc 0 -weight 1
grid columnconf $lpc 1 -weight 1
grid columnconf $lpc 2 -weight 1
grid columnconf $lpc 3 -weight 1

grid $lpc.posl -row 0 -column 0 -columnspan 2 -sticky news
grid $lpc.posx -row 0 -column 2 -sticky news
grid $lpc.posy -row 0 -column 3 -sticky news

xth_me_bind_entry_focus_return "$lpc.posx $lpc.posy" {xth_me_cmds_update {}}

grid $lpc.xp -row 1 -column 0 -sticky news
grid $lpc.yp -row 1 -column 1 -sticky news
grid $lpc.xn -row 1 -column 2 -sticky news
grid $lpc.yn -row 1 -column 3 -sticky news

xth_me_bind_entry_focus_return "$lpc.xp $lpc.yp" {xth_me_cmds_update {}}
xth_me_bind_entry_focus_return "$lpc.xn $lpc.yn" {xth_me_cmds_update {}}
xth_me_bind_entry_focusin "$lpc.posx $lpc.posy $lpc.xp $lpc.yp $lpc.xn $lpc.yn"

grid $lpc.cbp -row 2 -column 0 -sticky news
grid $lpc.cbs -row 2 -column 1 -columnspan 2 -sticky news
grid $lpc.cbn -row 2 -column 3 -sticky news

grid $lpc.rotc -row 3 -column 0 -columnspan 2 -sticky news
grid $lpc.rot -row 3 -column 2 -columnspan 2 -sticky news

grid $lpc.lszc -row 4 -column 0 -columnspan 2 -sticky news
grid $lpc.lsz -row 4 -column 2 -columnspan 2 -sticky news

grid $lpc.rszc -row 5 -column 0 -columnspan 2 -sticky news
grid $lpc.rsz -row 5 -column 2 -columnspan 2 -sticky news

grid $lpc.optl -row 6 -column 0 -columnspan 2 -sticky news
grid $lpc.upd -row 6 -column 2 -columnspan 2 -sticky news

grid $txb -row 7 -column 0 -columnspan 4 -sticky news

xth_me_bind_entry_focusin "$lpc.rot $lpc.lsz $lpc.rsz"
xth_me_bind_entry_return "$lpc.rot $lpc.lsz $lpc.rsz" {xth_me_cmds_update {}}

# main menu
xth_about_status "loading main menu ..."


$xth(me,menu,file) add command -label "New" -command xth_me_create_file \
  -font $xth(gui,lfont) -underline 0 -state normal
$xth(me,menu,file) add command -label "Open" -underline 0 \
  -accelerator "$xth(gui,controlk)-o" -state normal \
  -font $xth(gui,lfont) -command {
			set xth(gui,openxp) 0
			xth_me_open_file 1 {} 1
	}
$xth(me,menu,file) add command -label "Open XP" -underline 5 \
  -state normal -font $xth(gui,lfont) -command {
	    set xth(gui,openxp) 1
			xth_me_open_file 1 {} 1
			set xth(gui,openxp) 0
	}
$xth(me,menu,file) add command -label "Save" -underline 0 \
  -accelerator "$xth(gui,controlk)-s" -state disabled \
  -font $xth(gui,lfont) -command {xth_me_save_file 0}
$xth(me,menu,file) add command -label "Save as" -underline 5 \
  -font $xth(gui,lfont) -command {xth_me_save_file 1} -state disabled 
$xth(me,menu,file) add command -label "Close" -underline 0 \
  -accelerator "$xth(gui,controlk)-w"  -state disabled \
  -font $xth(gui,lfont) \
  -command xth_me_close_file

set xth(me,menu,edit) $xth(me,menu).edit
menu $xth(me,menu,edit) -tearoff 0
bind $xth(me,menu,edit) <FocusIn> {xth_me_cmds_update {}}
$xth(me,menu) add cascade -label "Edit" -state disabled \
  -font $xth(gui,lfont) -menu $xth(me,menu,edit) -underline 0
$xth(me,menu,edit) add command -label "Undo" -font $xth(gui,lfont) \
  -underline 0 -accelerator "$xth(gui,controlk)-z" -state disabled \
  -command xth_me_unredo_undo
$xth(me,menu,edit) add command -label "Redo" -font $xth(gui,lfont) \
  -underline 0 -accelerator "$xth(gui,controlk)-y" -state disabled \
  -command xth_me_unredo_redo
$xth(me,menu,edit) add separator
$xth(me,menu,edit) add command -label "Cut" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-x" -command "xth_app_clipboard cut"
$xth(me,menu,edit) add command -label "Copy" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-c" -command "xth_app_clipboard copy"
$xth(me,menu,edit) add command -label "Paste" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-v" -command "xth_app_clipboard paste"
$xth(me,menu,edit) add separator
$xth(me,menu,edit) add command -label "Select" -accelerator "Esc" -underline 0 -font $xth(gui,lfont) -command {xth_me_cmds_set_mode 0}
$xth(me,menu,edit) add command -label "Insert point" -accelerator "$xth(gui,controlk)-p" -underline 7 -font $xth(gui,lfont) -command {xth_me_cmds_set_mode 1}
$xth(me,menu,edit) add command -label "Insert line" -accelerator "$xth(gui,controlk)-l" -underline 7 -font $xth(gui,lfont) -command {
  xth_me_cmds_create_line {} 1 "" "" ""
  xth_ctrl_scroll_to me line
}
$xth(me,menu,edit) add command -label "Delete" -accelerator "$xth(gui,controlk)-d" -underline 0 -font $xth(gui,lfont) -command {xth_me_cmds_delete {}}
$xth(me,menu,edit) add separator
$xth(me,menu,edit) add cascade -label "Zoom 100 %" -font $xth(gui,lfont) \
  -underline 0 -menu $xth(ctrl,me,area).zb.m
$xth(me,menu,edit) add command -label "Auto adjust area" \
  -font $xth(gui,lfont) -command xth_me_area_auto_adjust
$xth(me,menu,edit) add command -label "Insert image" \
  -font $xth(gui,lfont) \
  -command {xth_me_image_insert $xth(ctrl,me,images,posx) $xth(ctrl,me,images,posy) {} 0 {}}
set xth(me,menu,edit,undo) [$xth(me,menu,edit) index "Undo"]
set xth(me,menu,edit,redo) [$xth(me,menu,edit) index "Redo"]
set xth(me,menu,edit,zoom) [$xth(me,menu,edit) index "Zoom 100 %"]


# create mouse mode bar and progess bar
set barfm $xth(gui,me).sf.barfm
frame $barfm
pack $barfm -side left
grid columnconf $barfm 0 -weight 1
grid rowconf $barfm 0 -weight 1

set xth(me,mbar) $barfm.mbar
Label $xth(me,mbar) -text "" -width 17 -relief sunken -font $xth(gui,lfont) \
  -anchor center -state disabled
grid $xth(me,mbar) -column 0 -row 0 -sticky news
set xth(me,mbar,bg) [$xth(me,mbar) cget -bg]
set xth(me,mbar,fg) [$xth(me,mbar) cget -fg]
xth_status_bar me $xth(me,mbar) "Mouse mode."

set xth(me,progbar) $barfm.pbar
set xth(me,progbar,value) 0
ProgressBar $xth(me,progbar) -type normal -width 100 -variable xth(me,progbar,value) -fg darkBlue

proc xth_me_progbar_show {max} {
  global xth
  set pbw [winfo width $xth(me,mbar)]
  set pbh [winfo height $xth(me,mbar)]
  grid forget $xth(me,mbar)
  grid $xth(me,progbar) -column 0 -row 0 -sticky news
  $xth(me,progbar) configure -maximum $max -width $pbw -height $pbh
  update idletasks
}

proc xth_me_progbar_hide {} {
  global xth
  grid forget $xth(me,progbar)
  grid $xth(me,mbar) -column 0 -row 0 -sticky news
  update idletasks
}

proc xth_me_progbar_prog {val} {
  global xth
  set xth(me,progbar,value) $val
  update idletasks
}

# create position bar
set xth(me,pbar) $xth(gui,me).sf.pbar
Label $xth(me,pbar) -text "" -width 15 -relief sunken -font $xth(gui,lfont) \
  -anchor center -state disabled
pack $xth(me,pbar) -side left
xth_status_bar me $xth(me,pbar) "Current mouse position."

xth_ctrl_minimize me cmds
xth_ctrl_minimize me prev
xth_ctrl_minimize me ss
xth_ctrl_minimize me point
xth_ctrl_minimize me line
xth_ctrl_minimize me linept
xth_ctrl_minimize me scrap
xth_ctrl_minimize me text
xth_ctrl_minimize me area
xth_ctrl_minimize me images
    
set xth(ctrl,me,area,xmin) ""
set xth(ctrl,me,area,ymin) ""
set xth(ctrl,me,area,xmax) ""
set xth(ctrl,me,area,ymax) ""

xth_about_status "loading line procs ..."

