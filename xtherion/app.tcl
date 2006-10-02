##
## app.tcl --
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


proc xth_app_move_panel {aname xx} {
  global xth
  if {$xth(app,$aname,wpsw) == 1} {
    set xth(app,$aname,relw) [expr [winfo width $xth(gui,main)] - $xx + $xth(app,$aname,wrtx)]; 
  } else {
    set xth(app,$aname,relw) [expr $xx - $xth(app,$aname,wrtx)]; 
  }
  xth_app_place $aname
}


proc xth_app_create {aname title} {
  
  global xth
  
  set aw "$xth(gui,main).$aname"
  set xth(gui,$aname) $aw
  set xth(app,list) [concat $xth(app,list) $aname]
  if {![info exists xth(app,$aname,relw)]} {
    set xth(app,$aname,relw) $xth(app,all,relw)
  }
  if {![info exists xth(app,$aname,wpsw)]} {
    set xth(app,$aname,wpsw) $xth(app,all,wpsw)
  }
  if {![info exists xth(app,$aname,wmwd)]} {
    set xth(app,$aname,wmwd) $xth(app,all,wmwd)
  }
  
  # create and configure application frames
  frame $aw
  frame $aw.af
  frame $aw.af.apps
  frame $aw.af.ctrl
  frame $aw.af.lrhn -borderwidth 2 -relief raised -cursor sb_h_double_arrow
  xth_status_bar $aname $aw.af.lrhn [mc "Drag to resize control panel."]

  frame $aw.sf
  set sbar $aw.sf.sbar
  label $sbar -text "" -anchor w -relief sunken -font $xth(gui,lfont)
  pack $sbar -side left -fill both -expand 1
  
  bind $aw.af.lrhn <Configure> "set xth(app,$aname,wwid) \[winfo width $xth(gui,main)\]; set xth(app,$aname,wrtx) \[winfo rootx $xth(gui,main)]; xth_ctrl_reshape $aname"
  bind $aw.af.lrhn <B1-Motion> "xth_app_move_panel $aname %X"
  
  set amn $aw.menu
  menu $amn -tearoff 0
  set xth($aname,menu) $amn
  
  set fmn $amn.file
  menu $fmn -tearoff 0 
  $amn add cascade -label [mc "File"] -menu $fmn -font $xth(gui,lfont) -underline 0
  set xth($aname,menu,file) $fmn
  
  set xth($aname,title) $title
  set xth($aname,wtitle) [string tolower $title]

  pack $aw.af -expand yes -fill both
  pack $aw.sf -side bottom -fill x

  set fr $xth(app,$aname,relw)
  set minfr $xth(app,$aname,wmwd)
  set lrhny [expr [winfo height $xth(gui,main)] - 64]
  if {$fr < $minfr} {
    set fr $minfr
  } elseif {$fr > ([winfo width $xth(gui,main)] - $xth(app,$aname,wmwd))} {
    set fr [expr {([winfo width $xth(gui,main)] - $xth(app,$aname,wmwd))}]
  }
  set xth(app,$aname,relw) $fr
  set fr [expr 1.0 - $fr / double([winfo width $xth(gui,main)])]
  
  if {$xth(app,$aname,wpsw) == 1} {
    place $aw.af.apps -relx 0 -rely 0 -relheight 1 -relwidth $fr
    place $aw.af.ctrl -relx $fr -rely 0 -relheight 1 -relwidth [expr 1.0 - $fr]
    place $aw.af.lrhn -relx $fr -y $lrhny -width 8 -height 8 -anchor center
  } else {
    place $aw.af.ctrl -relx 0 -rely 0 -relheight 1 -relwidth $fr
    place $aw.af.apps -relx $fr -rely 0 -relheight 1 -relwidth [expr 1.0 - $fr]
    place $aw.af.lrhn -relx $fr -y $lrhny -width 8 -height 8 -anchor center
  }
  
  xth_ctrl_create $aname
  
}


proc xth_app_clock {} {
  global xth
  set xth(gui,clock) [clock format [clock seconds] -format "%H:%M"]
  after 15000 xth_app_clock
}


proc xth_app_place {aname} {
  
  global xth
  set aw "$xth(gui,main).$aname"

  set fr $xth(app,$aname,relw)
  set minfr $xth(app,$aname,wmwd)
  if {$fr < $minfr} {
    set fr $minfr
  } elseif {$fr > ([winfo width $xth(gui,main)] - $xth(app,$aname,wmwd))} {
    set fr [expr {([winfo width $xth(gui,main)] - $xth(app,$aname,wmwd))}]
  }
  set xth(app,$aname,relw) $fr
  set fr [expr 1.0 - $fr / double([winfo width $xth(gui,main)])]

  set lrhny [expr [winfo height $xth(gui,main)] - 64]
  
  if {$xth(app,$aname,wpsw) == 1} {
    place configure $aw.af.apps -relx 0 -relwidth $fr
    place configure $aw.af.ctrl -relx $fr -relwidth [expr 1.0 - $fr]
    place configure $aw.af.lrhn -relx $fr -y $lrhny
  } else {
    place configure $aw.af.apps -relx [expr 1.0 - $fr] -relwidth $fr
    place configure $aw.af.ctrl -relx 0 -relwidth [expr 1.0 - $fr]
    place configure $aw.af.lrhn -relx [expr 1.0 - $fr] -y $lrhny
  }

  xth_ctrl_reshape $aname   
}

proc xth_app_switch {} {

  global xth
  
  set aname $xth(app,active)
  
  if {$xth(app,$aname,wpsw) == 1} {
    set xth(app,$aname,wpsw) 0
  } else {
    set xth(app,$aname,wpsw) 1
  }
  
  xth_app_place $aname
}


proc xth_app_finish {} {

  global xth
  
  # add Window menu to each menu
  set m "$xth(gui,main).wmenu"
  menu $m -tearoff 0
  set i 0
  
  set xth(gui,menu,window) $m

  foreach aname $xth(app,list) {

    if {[llength $xth(app,list)] > 1} {
      set i [expr $i + 1]
      $m add command -label $xth($aname,title) -accelerator "F$i" \
        -command "xth_app_show $aname" -font $xth(gui,lfont)
      bind $xth(gui,main) <F$i> "xth_app_show $aname"
    }
    
    # add clock to aname
    set clockbar $xth(gui,$aname).sf.clockbar
    label $clockbar -textvariable xth(gui,clock) -anchor center \
      -relief sunken -font $xth(gui,lfont) -width 5
    pack $clockbar -side left

  }
  if {[llength $xth(app,list)] > 1} {
    $m add separator
  }
  $m add command -label [mc "Maximize"] -underline 1 \
      -command "xth_app_maximize" -font $xth(gui,lfont)
  $m add command -label [mc "Normalize"] -underline 1 \
      -command "xth_app_normalize" -font $xth(gui,lfont)
  $m add command -label [mc "Switch panels"] -underline 1 \
      -command "xth_app_switch" -font $xth(gui,lfont)
  $m add separator

  $m add command -label [mc "Toggle toolbar"] -underline 1 \
      -command "xth_tools_toggle" -font $xth(gui,lfont)
  
  menu $m.kbes -tearoff 0
  set encnames [encoding names]
  set xth(encoding_system) [encoding system]
  foreach ecd [lsort $xth(kbencodings)] {
    if {[lsearch $encnames $ecd] >= 0} {
      $m.kbes add radiobutton -label $ecd \
        -command "encoding system $ecd\nset xth(encoding_system) \[encoding system\]" -font $xth(gui,lfont) \
        -variable xth(encoding_system) -value $ecd
    }
  }
  $m add cascade -label [mc "KBD encoding"] -menu $m.kbes
  

  if {$xth(debug)} {
    set dm "$xth(gui,main).dmenu"
    menu $dm -tearoff 0
  
    $dm add command -label "Refresh procs" -underline 0 -command {
      source global.tcl
      source sbar.tcl
      source cp_procs.tcl
      source te_sdata.tcl
      source me_imgs.tcl
      source me_cmds.tcl
      source me_cmds2.tcl
      source me_ss.tcl
      source bac.tcl
      source mv_procs.tcl
    } -font $xth(gui,lfont)
    $dm add command -label "Screen dump" -underline 0 -command {
      after 5000 {xwd -out screendump -frame}
    } -font $xth(gui,lfont)
    $dm add separator
    $dm add command -label "Show command console" -underline 1 \
      -command "wm deiconify $xth(gui,dbg); wm transient $xth(gui,dbg) $xth(gui,main)" -font $xth(gui,lfont)
    $dm add command -label "Hide command console" -underline 1 \
      -command "wm withdraw $xth(gui,dbg)" -font $xth(gui,lfont)
  }

  bind $xth(gui,main) <$xth(kb_control)-Key-q> "xth_exit"
  bind $xth(gui,main) <$xth(kb_control)-Key-o> xth_app_control_o 
  bind $xth(gui,main) <$xth(kb_control)-Key-r> xth_app_control_r 
  bind $xth(gui,main) <$xth(kb_control)-Key-w> xth_app_control_w
  bind $xth(gui,main) <$xth(kb_control)-Key-s> xth_app_control_s 
  bind $xth(gui,main) <$xth(kb_control)-Key-z> xth_app_control_z
  bind $xth(gui,main) <$xth(kb_control)-Key-y> xth_app_control_y 
  bind $xth(gui,main) <$xth(kb_control)-Key-p> xth_app_control_p 
  bind $xth(gui,main) <$xth(kb_control)-Key-l> xth_app_control_l 
  bind $xth(gui,main) <$xth(kb_control)-Key-d> xth_app_control_d
  bind $xth(gui,main) <$xth(kb_control)-Key-a> xth_app_control_a
  bind $xth(gui,main) <Prior> xth_app_pgup
  bind $xth(gui,main) <Next> xth_app_pgdn
  bind $xth(gui,main) <Shift-Prior> xth_app_shift_pgup
  bind $xth(gui,main) <Shift-Next> xth_app_shift_pgdn
  bind $xth(gui,main) <Key-Escape> xth_app_escape 
  bind $xth(gui,main) <F9> xth_app_make
  foreach aname $xth(app,list) {
    $xth($aname,menu) add cascade -label [mc "Window"] -menu $m -underline 0 \
      -font $xth(gui,lfont)
    if $xth(debug) {
      $xth($aname,menu) add cascade -label "Debug" -menu $dm -underline 0 \
        -font $xth(gui,lfont)
    }
    $xth($aname,menu,file) add separator
    $xth($aname,menu,file) add command -label [mc "Compile"] -underline 0 \
      -command "xth_app_make" -font $xth(gui,lfont) \
      -accelerator "F9"
    $xth($aname,menu,file) add command -label [mc "Quit"] -underline 0 \
      -command "xth_exit" -font $xth(gui,lfont) \
      -accelerator "$xth(gui,controlk)-q"
    $xth($aname,menu) add cascade -label [mc "Help"] -menu $xth(gui,menu,help) \
      -underline 0  -font $xth(gui,lfont)
  }  

}

proc xth_app_title {aname} {
  
  global xth
  
  # set the application menu
  set ofn ""
  if {[info exists xth($aname,open_file)]} {
    set ofn $xth($aname,open_file)
  }
  if {[string length $xth($aname,wtitle)] > 0} {
    set atit " $xth($aname,wtitle)"
  } else {
    set atit ""
  }
  if {[string equal $aname me] && ([string length $xth(me,curscrap)] > 0)} {
    set sname " - $xth(me,curscrap)"
  } else {
    set sname ""
  }
  if {[string length $ofn] > 0} {
    wm title $xth(gui,main) "$xth(prj,name)$atit - $xth($aname,open_file)$sname"
  } else {
    wm title $xth(gui,main) "$xth(prj,name)$atit$sname"
  }
}

proc xth_app_control_o {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    te  {xth_te_open_file 1 {} 1}
    me  {xth_me_open_file 1 {} 1}
    cp  {
      set xth(cp,updcf) 0
      xth_cp_open_file {}
    }
    mv {xth_mv_open_file {}}
  }
}  


proc xth_app_pgup {} {
  global xth
  switch $xth(app,active) {
    te  {}
    me  {
      $xth(gui,main).me.af.ctrl.c yview scroll -1 pages
    }
    cp  {}
    mv {}
  }
}

proc xth_app_pgdn {} {
  global xth
  switch $xth(app,active) {
    te  {}
    me  {
      $xth(gui,main).me.af.ctrl.c yview scroll 1 pages
    }
    cp  {}
    mv {}
  }
}

proc xth_app_shift_pgup {} {
  global xth
  switch $xth(app,active) {
    te  {}
    me  {
      $xth(ctrl,me,cmds).cl.l yview scroll -1 pages
    }
    cp  {}
    mv {}
  }
}


proc xth_app_shift_pgdn {} {
  global xth
  switch $xth(app,active) {
    te  {}
    me  {
      $xth(ctrl,me,cmds).cl.l yview scroll 1 pages
    }
    cp  {}
    mv {}
  }
}


proc xth_app_control_a {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    te  {}
    me  {
      xth_me_cmds_create_area {} 1 "" "" ""
      xth_ctrl_scroll_to me ac
      xth_ctrl_maximize me ac
    }
    cp  {}
    mv {}
  }
}  


proc xth_app_control_r {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    te  {}
    me  {
      xth_me_cmds_create_scrap {} 1 "" ""
      xth_ctrl_scroll_to me scrap
      xth_ctrl_maximize me scrap
    }
    
    cp  {}
    mv {xth_mv_reload_file}
  }
}  


proc xth_app_control_w {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    me  {xth_me_close_file}
    cp  {xth_cp_close_file}
  }
}  

proc xth_app_control_s {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    me  {xth_me_save_file 0}
  }
}  


proc xth_app_control_z {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    me  {xth_me_unredo_undo}
  }
  
}  

proc xth_app_control_y {} {

  global xth

  # puts $xth(app,active)  
  switch $xth(app,active) {
    me  {xth_me_unredo_redo}
  }
}  

proc xth_app_control_p {} {
  global xth
  switch $xth(app,active) {
    me  {xth_me_cmds_set_mode 1}
  }
}  


proc xth_app_control_d {} {
  global xth
  switch $xth(app,active) {
    me  {xth_me_cmds_delete {}}
  }
}  


proc xth_app_control_l {} {
  global xth
  switch $xth(app,active) {
    me  {
      xth_me_cmds_create_line {} 1 "" "" ""
      xth_ctrl_scroll_to me line
      xth_ctrl_maximize me line
      xth_ctrl_maximize me linept
    }
  }
}  


proc xth_app_escape {} {
  global xth
  switch $xth(app,active) {
    me  {xth_me_cmds_set_mode 0}
  }
}


proc xth_app_show {aname} {

  global xth
  
  if {[string equal $xth(app,active) $aname]} {
    return;
  }
  
  if {![string equal $xth(app,active) ""]} {
    pack forget $xth(gui,$xth(app,active))
  }

  set xth(app,active) $aname
  pack $xth(gui,$aname) -expand yes -fill both

  xth_app_title $aname

  $xth(gui,main) configure -menu $xth($aname,menu)
  
  regexp {([0-9]+)x([0-9]+)} [winfo geometry $xth(gui,main)] geom xsize ysize

  if {($xsize < [lindex $xth(gui,minsize) 0]) || \
      ($ysize < [lindex $xth(gui,minsize) 1])} {
    if {($xsize < [lindex $xth(gui,minsize) 0])} {
      set xsize [lindex $xth(gui,minsize) 0]
    }
    if {($ysize < [lindex $xth(gui,minsize) 1])} {
      set ysize [lindex $xth(gui,minsize) 1]
    }
    set ogeom [winfo geometry $xth(gui,main)]
    regsub $geom $ogeom [format "%sx%s" $xsize $ysize] ngeom
    wm geometry $xth(gui,main) $ngeom
  }

  update idletasks  
  xth_ctrl_reshape $aname
  
  # toolbar changes
  # disable map editor tools
  catch {
    pack forget $xth(gui,tb).edif
    pack forget $xth(gui,tb).self
  }
  switch $aname {
    cp {
    }
    me {
      # enable map editor tools
      pack $xth(gui,tb).self -side right -anchor nw
      pack $xth(gui,tb).edif -side right
    }
    mv {
    }    
  }
}


proc xth_exit {} {

  global xth

  catch {
    registry set {HKEY_LOCAL_MACHINE\SOFTWARE\Therion} XTherionDir $xth(gui,initdir)
  }
  
  # save all open text editor files
  if {![info exists xth(te,flist)]} {
    set xth(te,flist) {}
  }
  
  foreach cfid $xth(te,flist) {
    if {[xth_te_before_close_file $cfid yesnocancel] == 0} {
      return
    }
  }
  
  if {[info exists xth(me,fopen)]} {
    if {$xth(me,fopen) == 1} {
      if {[xth_me_before_close_file yesnocancel] == 0} {
        return
      }
    }
  }

  if {[info exists xth(cp,fopen)]} {
    xth_cp_close_file
  }
  
  destroy $xth(gui,main)
  update
  destroy .    
  
}


proc xth_app_maximize {} {
  global xth
  set swd [winfo screenwidth $xth(gui,main)]
  set shg [winfo screenheight $xth(gui,main)]
  wm geometry $xth(gui,main) [format "%dx%d+0+0" $swd $shg]
  update idletasks
  regexp {([0-9]+)x([0-9]+)\+([0-9]+)\+([0-9]+)} [winfo geometry $xth(gui,main)] geom xsize ysize xshft yshft
  wm geometry $xth(gui,main) [format "%dx%d+0+0" [expr $swd - $xshft] [expr $shg - $yshft]]
  update idletasks
}

proc xth_app_normalize {} {
  global xth
  set twd [expr int(0.8 * [winfo screenwidth $xth(gui,main)])]
  if {$twd < [lindex $xth(gui,minsize) 0]} {
    set twd [lindex $xth(gui,minsize) 0]
  }
  set thg [expr int(0.8 * [winfo screenheight $xth(gui,main)])]
  if {$thg < [lindex $xth(gui,minsize) 1]} {
    set thg [lindex $xth(gui,minsize) 1]
  }
  set tpx [expr int(0.5 * ([winfo screenwidth $xth(gui,main)] - $twd))]
  set tpy [expr int(0.5 * ([winfo screenheight $xth(gui,main)] - $thg))]
  wm geometry $xth(gui,main) [format "%dx%d+%d+%d" $twd $thg $tpx $tpy]
  update idletasks
  regexp {([0-9]+)x([0-9]+)\+([0-9]+)\+([0-9]+)} [winfo geometry $xth(gui,main)] geom xsize ysize xshft yshft
  wm geometry $xth(gui,main) [format "%dx%d+%d+%d" [expr $twd - $xshft + $tpx] \
    [expr $thg - $yshft + $tpy] $tpx $tpy]
  update idletasks
}

proc xth_app_clipboard {ev} {
  global xth
  set w [focus -lastfor $xth(gui,main)]
  if {[winfo ismapped $w]} {
    switch $ev {
      cut {
         event generate $w <<Cut>>
      }
      copy {
         event generate $w <<Copy>>
      }
      paste {
         event generate $w <<Paste>>
      }
      undo {
         event generate $w <<Undo>>
      }
      redo {
         event generate $w <<Redo>>
      }
    }
  }
}


proc xth_app_check_text_undo_redo {} {
  global xth
  catch {
    set w [focus -lastfor $xth(gui,main)]
    if {[winfo ismapped $w]} {
      catch {
        $w edit separator
      }
    }
  }
  after idle {after 1000 xth_app_check_text_undo_redo}
}

if {$xth(gui,text_undo)} {
  after idle {after 1000 xth_app_check_text_undo_redo}
}


proc xth_app_make {} {
  global xth
  set oactive $xth(app,active)
  if {[string length $oactive] == 0} {
    set oactive cp
  }
  xth_te_save_all
  xth_me_save_file 0
  switch $oactive {
    cp {}
    default {
      set xth(cp,updcf) 0
      xth_app_show cp
    }
  }
  if {!$xth(cp,fopen)} {
    xth_cp_open_file {}
  }
  update idletasks
  if {[xth_cp_compile]} {
    if {[string equal $xth(gui,platform) windows]} {
      xth_app_show te
      xth_app_show me
      xth_app_show cp
    }
    if {![string equal $oactive cp]} {
      xth_app_show $oactive
    }
  }
}


proc xth_app_autosave_schedule {} {
  global xth
  if $xth(gui,auto_save) {
    set xth(gui,auto_save,id) [after 60000 xth_app_autosave]
  } else {
    catch {
      after cancel $xth(gui,auto_save,id)
    }
  }
}

proc xth_app_autosave {} {
  global xth
  switch $xth(app,active) {
    me {xth_me_save_file 0}
    te {xth_te_save_all}
  }
  xth_app_autosave_schedule
}


