# - dzach add toolbar
set xth(tb,me,action) 0
set xth(gui,tb) $xth(gui,main).tb
set allapps {te me cp}

option add *tb*relief flat
option add *tb*overRelief groove
option add *tb*padx 0
option add *tb*indicatorOn off
option add *tb*offRelief flat
option add *tb*Button.width 20
option add *tb*Button.height 20
option add *tb*Radiobutton.width 18
option add *tb*Radiobutton.height 18

option add *edif*relief flat
option add *edif*overRelief groove
option add *edif*padx 0
option add *edif*indicatorOn off
option add *edif*offRelief flat
option add *edif*Button.width 20
option add *edif*Button.height 20
option add *edif*Radiobutton.width 18
option add *edif*Radiobutton.height 18

option add *movf*relief flat
option add *movf*overRelief groove
option add *movf*padx 0
option add *movf*indicatorOn off
option add *movf*offRelief flat
option add *movf*Button.width 20
option add *movf*Button.height 20

frame $xth(gui,tb) -relief raised -bd 1 -padx 5
pack $xth(gui,tb) -side top -anchor nw -fill x
frame $xth(gui,tb).filf

button $xth(gui,tb).newb -image {new_img} \
		-command {
			switch -- $xth(app,active) {
				te {
					xth_te_new_file
				}
				me {
					xth_me_create_file
				}
				cp {
					xth_cp_new_file
				}
			}
		}
xth_status_bar {te me cp} $xth(gui,tb).newb [mc "New file"]



button $xth(gui,tb).openb -image {open_img} \
		-command {
			switch -- $xth(app,active) {
				te {
					xth_te_open_file 1 {} 0
				}
				me {
					xth_me_open_file 1 {} 1
				}
				cp {
					xth_cp_open_file {}
				}
			}
		}
xth_status_bar $allapps $xth(gui,tb).openb [mc "Open file"]

button $xth(gui,tb).saveb -image {save_img} \
		-command {
			switch -- $xth(app,active) {
				te {
					if {$xth(te,fcurr) >= 0} {
					  xth_te_save_file 0 [lindex $xth(te,flist) $xth(te,fcurr)]
					}
				}
				me {
					xth_me_save_file 0
				}
			}
		}
xth_status_bar {te me cp} $xth(gui,tb).saveb [mc "Save file"]
    
button $xth(gui,tb).savasb -image {saveas_img} \
		-command {
			switch -- $xth(app,active) {
				te {
					if {$xth(te,fcurr) >= 0} {
					  xth_te_save_file 1 [lindex $xth(te,flist) $xth(te,fcurr)]
					}
				}
				me {
					xth_me_save_file 1
				}
				cp {
					xth_cp_save_as
				}
			}
		}
xth_status_bar {te me cp} $xth(gui,tb).savasb [mc "Save file as"]

button $xth(gui,tb).closeb -image {close_img} \
 	-command {
			switch -- $xth(app,active) {
				te {
					xth_te_close_file
				}
				me {
					xth_me_close_file
				}
				cp {
					xth_cp_close_file
				}
			}
		}
xth_status_bar {te me cp} $xth(gui,tb).closeb [mc "Close file"]

label $xth(gui,tb).sp01 -relief groove -width 0 -padx 0 -pady 0 -bd 1




frame $xth(gui,tb).unredof

button $xth(gui,tb).undob -image {undo_img} -state disabled \
    -command {
      switch -- $xth(app,active) {
        me {
          xth_me_unredo_undo
        }
        default {
          xth_app_clipboard redo                    
        }
      }
    }
xth_status_bar $allapps $xth(gui,tb).undob [mc "Undo"]



button $xth(gui,tb).redob -image {redo_img} -state disabled \
    -command {
      switch -- $xth(app,active) {
        me {
          xth_me_unredo_redo
        }
        default {
          xth_app_clipboard redo                    
        }
      }
    }
xth_status_bar $allapps $xth(gui,tb).redob [mc "Redo"]

label $xth(gui,tb).sp01x -relief groove -width 0 -padx 0 -pady 0 -bd 1






frame $xth(gui,tb).winf
button $xth(gui,tb).edb -image {edit_img} \
		-command {
			xth_app_show te
		}
xth_status_bar $allapps $xth(gui,tb).edb [mc "Switch to text editor"]

button $xth(gui,tb).e2db -image {2d_img} \
		-command {
			xth_app_show me
		}
xth_status_bar $allapps $xth(gui,tb).e2db [mc "Switch to map editor"]

button $xth(gui,tb).cpb -image {compile_img} \
		-command {
			xth_app_show cp
		}
xth_status_bar $allapps $xth(gui,tb).cpb [mc "Switch to compiler"]

label $xth(gui,tb).sp02 -relief groove -width 0 -padx 0 -pady 0 -bd 1

frame $xth(gui,tb).makf
button $xth(gui,tb).makeb -text "Make" -image {make_img} \
	-command {
		xth_app_make
	}
xth_status_bar $allapps $xth(gui,tb).makeb [mc "Compile"]

label $xth(gui,tb).sp03 -relief groove -width 0 -padx 0 -pady 0 -bd 1

frame $xth(gui,tb).self
radiobutton $xth(gui,tb).selb -text "Select" -image {select_img} \
		-variable xth(tb,me,action) -value 0 \
		-command {
			xth_me_cmds_set_mode 0
		} -state disabled
xth_status_bar me $xth(gui,tb).selb [mc "Switch to select mode"]

button $xth(gui,tb).selscrb -text "Select scrap" -image {selscr_img} \
		-command {
			xth_me_cmds_set_mode 0
      xth_me_ss_next_cmd 4
		} -state disabled
xth_status_bar me $xth(gui,tb).selscrb [mc "Select next scrap"]

button $xth(gui,tb).selarrb -image {selare_img} \
		-command {
			xth_me_cmds_set_mode 0
      xth_me_ss_next_cmd 6
		} -state disabled
xth_status_bar me $xth(gui,tb).selarrb [mc "Select next area"]
    

label $xth(gui,tb).sp1 -relief groove -width 0 -padx 0 -pady 0 -bd 1
  
frame $xth(gui,tb).edif

button $xth(gui,tb).edif.zoo+b -image {zoom+_img} \
		-command {
			if {$::xth(me,zoom)*2 > 400} return
			xth_me_area_zoom_to [expr {$::xth(me,zoom) * 2}]
		} -state disabled
  xth_status_bar me $xth(gui,tb).edif.zoo+b [mc "Zoom in"]
button $xth(gui,tb).edif.zoo-b -image {zoom-_img} \
		-command {
			if {$::xth(me,zoom)/2 < 25} return
			xth_me_area_zoom_to [expr {$::xth(me,zoom) / 2}]
		} -state disabled
xth_status_bar me $xth(gui,tb).edif.zoo-b [mc "Zoom out"]
label $xth(gui,tb).edif.spZoom -relief groove -width 0 -padx 0 -pady 0 -bd 1

button $xth(gui,tb).edif.scb -image {scrap_img} \
  -command {
    xth_me_cmds_create_scrap {} 1 "" ""
    xth_ctrl_scroll_to me scrap
    xth_ctrl_maximize me scrap
  } -state disabled
xth_status_bar me $xth(gui,tb).edif.scb [mc "Insert new scrap"]

radiobutton $xth(gui,tb).edif.ptrb \
		-image {point_img} -indicatoron off \
		-variable xth(tb,me,action) -value 1 \
		-command {xth_me_cmds_set_mode 1} -state disabled
xth_status_bar me $xth(gui,tb).edif.ptrb [mc "Insert new point"]

radiobutton $xth(gui,tb).edif.lnrb \
		-image {line_img} -indicatoron off \
		-variable xth(tb,me,action) -value 2 \
		-command {
      xth_me_cmds_create_line {} 1 "" "" ""
      xth_ctrl_scroll_to me line
      xth_ctrl_maximize me line
      xth_ctrl_maximize me linept
    } -state disabled

xth_status_bar me $xth(gui,tb).edif.lnrb [mc "Insert new line"]

radiobutton $xth(gui,tb).edif.arrb \
		-image {area_img} -indicatoron off \
		-variable xth(tb,me,action) -value 3 \
		-command {
      xth_me_cmds_create_area {} 1 "" "" ""
      xth_ctrl_scroll_to me ac
      xth_ctrl_maximize me ac
    } -state disabled
xth_status_bar me $xth(gui,tb).edif.arrb [mc "Insert new area"]

button $xth(gui,tb).edif.txrb \
		-image {text_img} \
    -command {
      xth_me_cmds_create_text {} 1 "\n" "1.0"
      xth_ctrl_scroll_to me text
      xth_ctrl_maximize me text
      focus $xth(ctrl,me,text).txt
    } -state disabled

label $xth(gui,tb).sp2 -relief groove -width 0 -padx 0 -pady 0 -bd 1
frame $xth(gui,tb).movf
	button $xth(gui,tb).movf.topb -text "top" -image {top_img} \
		-command {xth_me_cmds_move_up {}}
	button $xth(gui,tb).movf.upb -text "Up" -image {up_img} \
		-command {xth_me_cmds_move_up {}}
	button $xth(gui,tb).movf.dob -text "Down" -image {down_img} \
		-command {xth_me_cmds_move_down {}}
	button $xth(gui,tb).movf.botb -text "Bottom" -image {bottom_img} \
		-command {xth_me_cmds_move_down {}}
	button $xth(gui,tb).movf.tob -text "To" -image {to_img} \
		-command {xth_me_cmds_move_to {} {}}

button $xth(gui,tb).deb -image {delete_img} \
	-command {xth_me_cmds_delete {}} -state disabled
xth_status_bar me $xth(gui,tb).deb [mc "Delete selected object"]

label $xth(gui,tb).sp3 -relief groove -width 0 -padx 0 -pady 0 -bd 1
# - file icons
pack $xth(gui,tb).filf -side left
pack $xth(gui,tb).newb $xth(gui,tb).openb $xth(gui,tb).saveb $xth(gui,tb).savasb \
	 $xth(gui,tb).closeb \
	-side left -anchor center -in $xth(gui,tb).filf
pack $xth(gui,tb).sp01 -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).filf

# - undo/redo icons
pack $xth(gui,tb).unredof -side left
pack $xth(gui,tb).undob $xth(gui,tb).redob -side left -anchor center -in $xth(gui,tb).unredof
pack $xth(gui,tb).sp01x -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).unredof


# - window icons
pack $xth(gui,tb).winf -side left

pack $xth(gui,tb).edb $xth(gui,tb).e2db $xth(gui,tb).cpb \
	-side left -anchor center -in $xth(gui,tb).winf
pack $xth(gui,tb).sp02 -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).winf

# - make icons
pack $xth(gui,tb).makf -side left
pack $xth(gui,tb).makeb \
	-side left -anchor center -in $xth(gui,tb).makf
pack $xth(gui,tb).sp03 -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).makf

# - move icons
# pack $xth(gui,tb).movf -side right -anchor nw
pack $xth(gui,tb).movf.topb $xth(gui,tb).movf.botb $xth(gui,tb).movf.tob \
	$xth(gui,tb).movf.upb $xth(gui,tb).movf.dob \
	-side left -anchor center

# - select icons
pack $xth(gui,tb).self -side right -anchor nw
pack $xth(gui,tb).selb \
	-side left -anchor center -in $xth(gui,tb).self
pack $xth(gui,tb).selscrb $xth(gui,tb).selarrb \
	-side left -anchor center -in $xth(gui,tb).self
pack $xth(gui,tb).sp3 -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).self
pack $xth(gui,tb).deb \
	-side left -anchor center -in $xth(gui,tb).self
#pack $xth(gui,tb).sp1 -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).self

# - edit icons
pack $xth(gui,tb).edif -side right 
pack $xth(gui,tb).edif.zoo+b $xth(gui,tb).edif.zoo-b -side left -anchor center
pack $xth(gui,tb).edif.spZoom -padx 7 -pady 5 -fill y -side left -anchor center
pack $xth(gui,tb).edif.scb $xth(gui,tb).edif.ptrb $xth(gui,tb).edif.lnrb $xth(gui,tb).edif.arrb \
	-side left -anchor center
pack $xth(gui,tb).sp2 -padx 7 -pady 5 -fill y -side left -anchor center -in $xth(gui,tb).edif


proc xth_tools_me_enable {} {
  global xth
  set tools [list \
    $xth(gui,tb).deb $xth(gui,tb).selb $xth(gui,tb).selscrb $xth(gui,tb).selarrb \
    $xth(gui,tb).edif.zoo+b $xth(gui,tb).edif.zoo-b \
    $xth(gui,tb).edif.scb $xth(gui,tb).edif.ptrb $xth(gui,tb).edif.lnrb $xth(gui,tb).edif.arrb \
  ]
  if {$xth(me,fopen)} {
     foreach tl $tools {
       $tl configure -state normal
     }
  } else {
     foreach tl $tools {
       $tl configure -state disabled
     }
  }
}

proc xth_tools_toggle {} {
  global xth
  set xth(gui,toolbar) [expr !$xth(gui,toolbar)]
  if $xth(gui,toolbar) {
    catch {
      pack $xth(gui,tb) -side top -anchor nw -fill x -before $xth(gui,main).$xth(app,active)
    }
  } else {
    catch {
      pack forget $xth(gui,tb)
    }
  }
}