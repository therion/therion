##
## me_cmds.tcl --
##
##     Map editor command processing.
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


proc xth_me_cmds_get_line_option {ln opt} {
  set rxl [list [list "\\s*\\-$opt\\s+\\\[(\[^\\\]\]*)\\\]" "\["]\
    [list "\\s*\\-$opt\\s+\\\"((\\\"\\\"|\[^\\\"])+)\\\"" "\""]\
    [list "\\s*\\-$opt\\s+(\\S+)" {}]]
  set rln $ln
  set val {}
  set sep {}
  set res 0
  foreach rx $rxl {
    if {[regexp [lindex $rx 0] $ln dump val]} {
      regsub [lindex $rx 0] $ln {} rln
      set sep [lindex $rx 1]
      set res 1
      break
    }
  }
  return [list $val $rln $res]
}


proc xth_me_cmds_set_action {act} {

  global xth
  
  set xth(me,cmds,action) $act
  switch $act {
    0 {
      $xth(ctrl,me,cmds).cc.go configure -text "Insert line"
    }
    1 {
      $xth(ctrl,me,cmds).cc.go configure -text "Insert point"
    }
    2 {
      $xth(ctrl,me,cmds).cc.go configure -text "Insert scrap"
    }
    3 {
      $xth(ctrl,me,cmds).cc.go configure -text "Insert text"
    }
    4 {
      $xth(ctrl,me,cmds).cc.go configure -text "Delete"
    }
    5 {
      $xth(ctrl,me,cmds).cc.go configure -text "Insert area"
    }
  }

  update idletasks

}


proc xth_me_cmds_update_buttons {} {

  global xth
  set ccbox $xth(ctrl,me,cmds).cc
  set clbox $xth(ctrl,me,cmds).cl
  set ncmd [expr [llength $xth(me,cmds,xlist)] - 1]
  if {$xth(me,fopen)} {
    $ccbox.cfg configure -state normal
    $clbox.l configure -takefocus 1
    $ccbox.go configure -state normal
    $xth(me,mbar) configure -state normal
    if {$ncmd > 0} {
      $ccbox.sel configure -state normal
      $xth(me,menu,edit) entryconfigure "Select" -state normal
      $xth(ctrl,me,prev).upd  configure -state normal

      $xth(ctrl,me,ss).xl configure -state normal
      $xth(ctrl,me,ss).xe configure -state normal
      $xth(ctrl,me,ss).cs configure -state normal
      $xth(ctrl,me,ss).rx configure -state normal
      $xth(ctrl,me,ss).sn configure -state normal
      $xth(ctrl,me,ss).sa configure -state normal
      $xth(ctrl,me,ss).sf configure -state normal
      $xth(ctrl,me,ss).ca configure -state normal
      
      if {$ncmd > 1} {
        $ccbox.mu configure -state normal
        $ccbox.md configure -state normal
        $ccbox.mt configure -state normal
        $ccbox.tt configure -state normal
     } else {
        $ccbox.mu configure -state disabled
        $ccbox.md configure -state disabled
        $ccbox.mt configure -state disabled
        $ccbox.tt configure -state disabled
      }
      $ccbox.cfg.m entryconfigure "Delete" -state normal
      $xth(me,menu,edit) entryconfigure "Delete" -state normal
    } else {
      $xth(ctrl,me,prev).upd  configure -state disabled
      $ccbox.sel configure -state disabled
      $xth(me,menu,edit) entryconfigure "Select" -state disabled
      $ccbox.cfg.m entryconfigure "Delete" -state disabled
      $xth(me,menu,edit) entryconfigure "Delete" -state disabled
      $ccbox.mu configure -state disabled
      $ccbox.md configure -state disabled
      $ccbox.mt configure -state disabled
      $ccbox.tt configure -state disabled
      if {$xth(me,cmds,action) == 4} {
        xth_me_cmds_set_action 3
      }

      $xth(ctrl,me,ss).xl configure -state disabled
      $xth(ctrl,me,ss).xe configure -state disabled
      $xth(ctrl,me,ss).cs configure -state disabled
      $xth(ctrl,me,ss).rx configure -state disabled
      $xth(ctrl,me,ss).sn configure -state disabled
      $xth(ctrl,me,ss).sa configure -state disabled
      $xth(ctrl,me,ss).sf configure -state disabled
      $xth(ctrl,me,ss).ca configure -state disabled

    }
  } else {
    xth_me_cmds_set_action 3
    $ccbox.go configure -state disabled
    $ccbox.cfg configure -state disabled
    $clbox.l configure -takefocus 0
    $xth(me,mbar) configure -text "" -state disabled -bg $xth(me,mbar,bg) -fg $xth(me,mbar,fg)
    $ccbox.sel configure -state disabled
    $ccbox.mu configure -state disabled
    $ccbox.md configure -state disabled
    $ccbox.mt configure -state disabled
    $ccbox.tt configure -state disabled

    $xth(ctrl,me,ss).xl configure -state disabled
    $xth(ctrl,me,ss).xe configure -state disabled
    $xth(ctrl,me,ss).cs configure -state disabled
    $xth(ctrl,me,ss).rx configure -state disabled
    $xth(ctrl,me,ss).sn configure -state disabled
    $xth(ctrl,me,ss).sa configure -state disabled
    $xth(ctrl,me,ss).sf configure -state disabled
    $xth(ctrl,me,ss).ca configure -state disabled

  }
  
  update idletasks
  
}


# typy prikazov
# 0 eof
# 1 text
# 2 point
# 3 line
# 4 scrap
# 5 endscrap
# 6 area

proc xth_me_cmds_create {typ id ix} {
  global xth
  if {[string length $id] < 1} {
    set id $xth(me,cmds,cmdln)
    incr xth(me,cmds,cmdln)
  }
  set xth(me,cmds,$id,listix) -1
  set xth(me,cmds,$id,ct) $typ
  set xth(me,cmds,$id,type) {}
  set xth(me,cmds,$id,name) {}
  set xth(me,cmds,$id,data) {}
  set ix [lsearch $xth(me,cmds,xlist) $xth(me,cmds,selid)]
  set xth(me,cmds,list) [linsert $xth(me,cmds,list) $ix {}]
  set xth(me,cmds,xlist) [linsert $xth(me,cmds,xlist) $ix $id]
  if {$ix != -1} {
    xth_me_cmds_update_list_ft $ix {}
  }
  xth_me_cmds_update_buttons
  return $id
  
}


proc xth_me_cmds_update_list_ft {f t} {
  global xth
  if {[string length $f] == 0} {
    set f 0
  }
  if {[string length $t] == 0} {
    set t [llength $xth(me,cmds,xlist)]
  }
  for {set ii $f} {$ii < $t} {incr ii} {
    xth_me_cmds_update_list [lindex $xth(me,cmds,xlist) $ii]
  }
}

proc xth_me_cmds_update_list {id} {
  global xth
  set ix [lsearch $xth(me,cmds,xlist) $id]
  switch $xth(me,cmds,$id,ct) {
    0 {
      set cstr "end of file"
    }
    1 {
      set cstr "$ix: text"
    }
    2 {
      set cstr "$ix: point"
    }
    3 {
      set cstr "$ix: line"
    }
    4 {
      set cstr "$ix: scrap"
    }
    5 {
      set cstr "$ix: endscrap"
    }
    6 {
      set cstr "$ix: area"
    }
  }
  if {[string length $xth(me,cmds,$id,type)] > 0} {
    set cstr "$cstr $xth(me,cmds,$id,type)"
  }
  if {[string length $xth(me,cmds,$id,name)] > 0} {
    set cstr "$cstr - $xth(me,cmds,$id,name)"
  }
  set xth(me,cmds,list) [lreplace $xth(me,cmds,list) $ix $ix $cstr]
  set xth(me,cmds,$id,listix) $ix
  update idletasks
}


proc xth_me_cmds_draw {id} {
  global xth
  switch $xth(me,cmds,$id,ct) {
    2 {
      xth_me_cmds_draw_point $id
    }
    3 {
      xth_me_cmds_draw_line $id
    }
  }
}


proc xth_me_cmds_erase {id} {
  global xth
  switch $xth(me,cmds,$id,ct) {
    2 {
      $xth(me,can) delete pt$id
    }
    3 {
      $xth(me,can) delete ln$id
    }
  }
}


proc xth_me_cmds_undelete {id pid ix} {
  global xth
  xth_me_cmds_select 0
  set xth(me,cmds,list) [linsert $xth(me,cmds,list) $ix {}]
  set xth(me,cmds,xlist) [linsert $xth(me,cmds,xlist) $ix $id]
  xth_me_cmds_update_list_ft $ix {}
  #xth_me_cmds_update_list $id
  xth_me_cmds_draw $id
  xth_me_cmds_select "$id $pid"
  xth_me_cmds_update_buttons
}

proc xth_me_cmds_delete {id} {
  global xth
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {$xth(me,cmds,$id,ct) > 0} {
    set oldselid $xth(me,cmds,selid)
    set oldselpid $xth(me,cmds,selpid)
    xth_me_cmds_select 0
    set ix [lsearch $xth(me,cmds,xlist) $id]
    set xth(me,cmds,list) [lreplace $xth(me,cmds,list) $ix $ix]
    set xth(me,cmds,xlist) [lreplace $xth(me,cmds,xlist) $ix $ix]
    xth_me_cmds_update_list_ft $ix {}
    xth_me_cmds_erase $id
    if {$oldselid == $id} {
      set nwid [lindex $xth(me,cmds,xlist) $ix]
    } else {
      set nwid $oldselid
    }
    xth_me_cmds_select $nwid
    xth_me_unredo_action "deleting" "xth_me_cmds_undelete $id $oldselpid $ix" \
      "xth_me_cmds_delete $id"
    xth_me_cmds_update_buttons
  }
}


proc xth_me_cmds_update {id} {
  global xth

  if {[llength $id] < 1} {
    update idletasks
    set id $xth(me,cmds,selid)
    set pid $xth(me,cmds,selpid)
  } elseif {[llength $id] < 2} {
    set id [lindex $id 0]
    set pid $xth(me,cmds,selpid)
  } else {
    set id [lindex $id 0]
    set pid [lindex $id 1]
  }
  
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  switch $xth(me,cmds,$id,ct) {
    1 {
      xth_me_cmds_update_text $id [$xth(ctrl,me,text).txt get 1.0 end] \
        [$xth(ctrl,me,text).txt index insert]
    }
    2 {
      xth_me_cmds_update_point $id $xth(ctrl,me,point,x) \
        $xth(ctrl,me,point,y) $xth(ctrl,me,point,type) \
        $xth(ctrl,me,point,name) $xth(ctrl,me,point,opts) \
        $xth(ctrl,me,point,rot) $xth(ctrl,me,point,xs) \
        $xth(ctrl,me,point,ys)
      xth_me_cmds_update_point_vars $id
    }
    3 {
      xth_me_cmds_update_line $id $pid $xth(ctrl,me,line,type) \
        $xth(ctrl,me,line,name) $xth(ctrl,me,line,opts) \
        $xth(ctrl,me,line,reverse) $xth(ctrl,me,linept,x) \
        $xth(ctrl,me,linept,y) $xth(ctrl,me,linept,xp) \
        $xth(ctrl,me,linept,yp) $xth(ctrl,me,linept,xn) \
        $xth(ctrl,me,linept,yn) $xth(ctrl,me,linept,smooth) \
        $xth(ctrl,me,linept,rot) $xth(ctrl,me,linept,rs) \
        $xth(ctrl,me,linept,ls) \
        [$xth(ctrl,me,linept).oe.txt get 1.0 end] \
        [$xth(ctrl,me,linept).oe.txt index insert]
      xth_me_cmds_update_line_vars $id $pid
    }    
    4 {
      xth_me_cmds_update_scrap $id $xth(ctrl,me,scrap,name) \
        $xth(ctrl,me,scrap,projection) $xth(ctrl,me,scrap,options) \
         [list $xth(ctrl,me,scrap,px1) $xth(ctrl,me,scrap,py1) \
          $xth(ctrl,me,scrap,px2) $xth(ctrl,me,scrap,py2) \
          $xth(ctrl,me,scrap,rx1) $xth(ctrl,me,scrap,ry1) \
          $xth(ctrl,me,scrap,rx2) $xth(ctrl,me,scrap,ry2) \
          $xth(ctrl,me,scrap,units)]
      xth_me_cmds_update_scrap_vars $id
    }
    6 {
      xth_me_cmds_update_area $id $xth(ctrl,me,ac,type) \
        $xth(ctrl,me,ac,opts)
      xth_me_cmds_update_area_vars $id
    }
  }
  xth_me_cmds_update_list $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  update idletasks
}

proc xth_me_cmds_update_text_ctrl {id} {

  global xth
  if {[string length $id] > 0} {
    $xth(ctrl,me,text).txt configure -state normal
    $xth(ctrl,me,text).txt delete 1.0 end
    $xth(ctrl,me,text).txt insert 1.0 $xth(me,cmds,$id,data)
    $xth(ctrl,me,text).txt mark set insert $xth(me,cmds,$id,cpos)
    $xth(ctrl,me,text).txt see $xth(me,cmds,$id,cpos)
    $xth(ctrl,me,text).upd configure -state normal
    xth_me_prev_cmd $xth(me,cmds,$id,data)
  } else {
    $xth(ctrl,me,text).txt configure -state normal
    $xth(ctrl,me,text).txt delete 1.0 end
    $xth(ctrl,me,text).txt see 1.0
    $xth(ctrl,me,text).txt configure -state disabled
    $xth(ctrl,me,text).upd configure -state disabled
  }
  
}



proc xth_me_cmds_unselect {id} {
  
  global xth
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {$xth(me,unredook)} {
    xth_me_cmds_update $id
  }
  switch $xth(me,cmds,$id,ct) {
    1 {xth_me_cmds_update_text_ctrl {}}
    2 {xth_me_cmds_update_point_ctrl {}}
    3 {
      xth_me_cmds_update_line_ctrl {}
      xth_me_cmds_update_linept_ctrl {} 0
      set xth(me,cmds,selpid) 0
      if {$xth(me,cmds,mode) == 2} {
        xth_me_cmds_set_mode 0
      }
    }
    4 {xth_me_cmds_update_scrap_ctrl {}}
    6 {
      xth_me_cmds_update_area_ctrl {}
      if {$xth(me,cmds,mode) == 3} {
        xth_me_cmds_set_mode 0
      }
    }
  }
  update idletasks
  
}


proc xth_me_cmds_select {id} {

  global xth
  
  if {!$xth(me,fopen)} {
    return
  }
  set center_to 0
  update idletasks
  if {[llength $id] < 1} {
    set id [lindex $xth(me,cmds,xlist) [lindex [$xth(ctrl,me,cmds).cl.l curselection] 0]]
    if {$xth(me,cmds,$id,ct) == 2} {
      set center_to 1
    }
    set pid 0
  } elseif {[llength $id] < 2} {
    set id [lindex $id 0]
    set pid 0
  } else {
    set pid [lindex $id 1]
    set id [lindex $id 0]
  }
  
  set newx [lsearch $xth(me,cmds,xlist) $id]
  if {$xth(me,cmds,selid) == $id} {
    $xth(ctrl,me,cmds).cl.l selection clear 0 end  
    $xth(ctrl,me,cmds).cl.l selection set $newx $newx
    $xth(ctrl,me,cmds).cl.l see $newx
    if {($xth(me,cmds,$id,ct) == 3) && ($pid != $xth(me,cmds,selpid))} {
      xth_me_cmds_select_linept $id $pid
    }
    if {$xth(me,unredook)} {
      return
    }
  }
  if {$xth(me,cmds,$xth(me,cmds,selid),ct) != $xth(me,cmds,$id,ct)} {
    if {![string equal $xth(ctrl,me,cmds).cl.l [focus -lastfor $xth(gui,main)]]} {
      focus $xth(gui,main)
    }
  }
  
  if {$xth(me,cmds,selid) != $id} {
    xth_me_cmds_unselect $xth(me,cmds,selid)
  }

  set xth(me,cmds,selid) $id
  $xth(ctrl,me,cmds).cl.l selection clear 0 end  
  $xth(ctrl,me,cmds).cl.l selection set $newx $newx  
  $xth(ctrl,me,cmds).cl.l see $newx
  xth_me_cmds_set_colors  
  switch $xth(me,cmds,$id,ct) {
    1 {xth_me_cmds_update_text_ctrl $id}
    2 {xth_me_cmds_update_point_ctrl $id}
    3 {
      xth_me_cmds_update_line_ctrl $id
      xth_me_cmds_select_linept $id $pid
      $xth(me,can) itemconfigure lnln$id -fill $xth(gui,me,activefill)
      $xth(me,can) itemconfigure lnpt$id -fill $xth(gui,me,activefill)
    }
    4 {xth_me_cmds_update_scrap_ctrl $id}
    6 {
      xth_me_cmds_update_area_ctrl $id
      xth_me_cmds_show_current_area
    }
    default {xth_me_prev_cmd $xth(me,cmds,$id,data)}
  }
  
  if {$center_to} {
    xth_me_center_to [list $xth(me,cmds,$id,x) $xth(me,cmds,$id,y)]
  }
  update idletasks
}

proc xth_me_cmds_set_move_to_list {} {
  global xth
  # prejde vsetky prikazy a najde scrapy a endscrapy
  set xl [llength $xth(me,cmds,xlist)]
  set vls {}
  set lscrap {}
  for {set ii 0} {$ii < $xl} {incr ii} {
    set id [lindex $xth(me,cmds,xlist) $ii]
    switch $xth(me,cmds,$id,ct) {
      4 {
        set lscrap $xth(me,cmds,$id,name)
        lappend vls "$lscrap begin \[[expr $ii + 1]\]"
      }
      5 {
        lappend vls "$lscrap end \[$ii\]"
      }
    }
  }
  $xth(ctrl,me,cmds).cc.tt configure -values $vls
  update idletasks
}

proc xth_me_cmds_set_move_to {} {
  global xth
  set lnum {}
  regexp {\[(\d+)\]} $xth(ctrl,me,cmds,moveto) dum lnum
  set xth(ctrl,me,cmds,moveto) $lnum
  update idletasks
}


proc xth_me_cmds_move_to {id dx} {
  global xth
  xth_me_cmds_update {}
  if {[string length $dx] < 1} {
    set dx $xth(ctrl,me,cmds,moveto)
  }
  set dx [regexp -inline {\d*} $dx]
  if {[string length $dx] < 1} {
    return
  }
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  set sx [lsearch $xth(me,cmds,xlist) $id]
  set maxsdx [expr [llength $xth(me,cmds,xlist)] - 2]
  if {($dx == $sx) || ($sx > $maxsdx) || ($dx > $maxsdx)} {
    return;
  }

  # prehodi  
  set xth(me,cmds,list) [linsert $xth(me,cmds,list) $dx [lindex $xth(me,cmds,list) $sx]]
  set xth(me,cmds,xlist) [linsert $xth(me,cmds,xlist) $dx [lindex $xth(me,cmds,xlist) $sx]]
  if {$dx < $sx} {
    set xth(me,cmds,list) [lreplace $xth(me,cmds,list) [expr $sx + 1] [expr $sx + 1]]
    set xth(me,cmds,xlist) [lreplace $xth(me,cmds,xlist) [expr $sx + 1] [expr $sx + 1]]
  } else {
    set xth(me,cmds,list) [lreplace $xth(me,cmds,list) $sx $sx]
    set xth(me,cmds,xlist) [lreplace $xth(me,cmds,xlist) $sx $sx]
  }    

  if {$dx < $sx} {
    xth_me_cmds_update_list_ft $dx [expr $sx + 1]
  } else {
    xth_me_cmds_update_list_ft $sx [expr $dx + 1]
  }

  set nid [lindex $xth(me,cmds,xlist) $sx]
  if {$xth(me,unredook)} {
    xth_me_cmds_select $nid
  }
  

  # unredo
  xth_me_unredo_action "moving command" "xth_me_cmds_move_to $id $sx\nxth_me_cmds_select $id" "xth_me_cmds_move_to $id $dx\nxth_me_cmds_select $nid"
  update idletasks
  
}

proc xth_me_cmds_move_up {id} {
  global xth
  xth_me_cmds_update {}
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {$id < 1} {
    return
  }
  set ix [lsearch $xth(me,cmds,xlist) $id]
  if {$ix < 1} {
    return
  }
  set dix [expr $ix - 1]
  # vymenime v liste
  set xth(me,cmds,list) [lreplace $xth(me,cmds,list) $dix $ix \
    [lindex $xth(me,cmds,list) $ix] [lindex $xth(me,cmds,list) $dix]]
  # vymenime v xliste
  set xth(me,cmds,xlist) [lreplace $xth(me,cmds,xlist) $dix $ix \
    [lindex $xth(me,cmds,xlist) $ix] [lindex $xth(me,cmds,xlist) $dix]]
  # selection
  if {$id == $xth(me,cmds,selid)} {
    $xth(ctrl,me,cmds).cl.l selection clear 0 end  
    $xth(ctrl,me,cmds).cl.l selection set $dix $dix
    $xth(ctrl,me,cmds).cl.l see $dix
    set selcmd "\nxth_me_cmds_select $id"
  } else {
    set selcmd {}
  }
  xth_me_cmds_update_list [lindex $xth(me,cmds,xlist) $ix]
  xth_me_cmds_update_list [lindex $xth(me,cmds,xlist) $dix]
  # unredo
  xth_me_unredo_action "moving command" "xth_me_cmds_move_down $id$selcmd" "xth_me_cmds_move_up $id$selcmd"
  update idletasks
}


proc xth_me_cmds_move_down {id} {
  global xth
  xth_me_cmds_update {}
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {$id < 1} {
    return
  }
  set ix [lsearch $xth(me,cmds,xlist) $id]
  if {$ix > [expr [llength $xth(me,cmds,xlist)] - 3]} {
    return
  }
  set iix [expr $ix + 1]
  # vymenime v liste
  set xth(me,cmds,list) [lreplace $xth(me,cmds,list) $ix $iix \
    [lindex $xth(me,cmds,list) $iix] [lindex $xth(me,cmds,list) $ix]]
  # vymenime v xliste
  set xth(me,cmds,xlist) [lreplace $xth(me,cmds,xlist) $ix $iix \
    [lindex $xth(me,cmds,xlist) $iix] [lindex $xth(me,cmds,xlist) $ix]]
  # selection
  if {$id == $xth(me,cmds,selid)} {
    $xth(ctrl,me,cmds).cl.l selection clear 0 end  
    $xth(ctrl,me,cmds).cl.l selection set $iix $iix
    $xth(ctrl,me,cmds).cl.l see $iix
    set selcmd "\nxth_me_cmds_select $id"
  } else {
    set selcmd {}
  }
  # unredo
  xth_me_cmds_update_list [lindex $xth(me,cmds,xlist) $ix]
  xth_me_cmds_update_list [lindex $xth(me,cmds,xlist) $iix]
  xth_me_unredo_action "moving command" "xth_me_cmds_move_up $id$selcmd" "xth_me_cmds_move_down $id$selcmd"
  update idletasks
}


proc xth_me_cmds_create_endscrap {ix mode name} {
  global xth
  xth_me_cmds_update {}
  set id [xth_me_cmds_create 5 {} $ix]
  set xth(me,cmds,$id,name) {}
  set xth(me,cmds,$id,data) "endscrap"
  if {[string length $name] > 0} {
    set xth(me,cmds,$id,data) "$xth(me,cmds,$id,data)\n# $name"
  }
  xth_me_cmds_update_list $id
  if {$mode} {
    xth_me_cmds_select $id
    xth_me_unredo_action "creating endscrap" "xth_me_cmds_delete $id" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]"  
  }
}


proc xth_me_cmds_hide_scrap_xctrl {} {

  global xth

  $xth(me,can) itemconfigure $xth(me,canid,scrap,scp1) -state hidden
  $xth(me,can) bind $xth(me,canid,scrap,scp1) <Enter> ""
  $xth(me,can) bind $xth(me,canid,scrap,scp1) <Leave> ""

  $xth(me,can) itemconfigure $xth(me,canid,scrap,scp2) -state hidden
  $xth(me,can) bind $xth(me,canid,scrap,scp2) <Enter> ""
  $xth(me,can) bind $xth(me,canid,scrap,scp2) <Leave> ""
  
}


proc xth_me_cmds_move_scrap_xctrl {id x y} {
  global xth
  set x [xth_me_real2canx $x]
  set y [xth_me_real2cany $y]
  $xth(me,can) coords $xth(me,canid,scrap,scp$id) \
    [expr $x - $xth(gui,me,scrap,psize)] [expr $y - $xth(gui,me,scrap,psize)] \
    [expr $x + $xth(gui,me,scrap,psize)] [expr $y + $xth(gui,me,scrap,psize)]
}


proc xth_me_cmds_start_scrap_xctrl_drag {pid x y} {
  global xth
  xth_me_cmds_update {}
  set xth(me,scrap,xdrag_mx) $x
  set xth(me,scrap,xdrag_my) $y
  set xth(me,scrap,xdrag_px) $xth(ctrl,me,scrap,px$pid)
  set xth(me,scrap,xdrag_py) $xth(ctrl,me,scrap,py$pid)
  set xth(me,scrap,xdrag_benter) [$xth(me,can) bind $xth(me,canid,scrap,scp$pid) <Enter>]
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <Enter> {}
  set xth(me,scrap,xdrag_bleave) [$xth(me,can) bind $xth(me,canid,scrap,scp$pid) <Leave>]
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <Leave> {}
  $xth(me,can) itemconfigure $xth(me,canid,scrap,scp$pid) -fill {}
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <B1-Motion> "xth_me_cmds_scrap_xctrl_drag $pid %x %y"
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <B1-ButtonRelease> "xth_me_cmds_end_scrap_xctrl_drag $pid %x %y"
  $xth(me,can) configure -cursor {}
}


proc xth_me_cmds_scrap_xctrl_drag {pid x y} {
  global xth
  set nx [expr $xth(me,scrap,xdrag_px) - [expr double($xth(me,scrap,xdrag_mx) - $x) * 100.0 / $xth(me,zoom)]]
  set ny [expr $xth(me,scrap,xdrag_py) + [expr double($xth(me,scrap,xdrag_my) - $y) * 100.0 / $xth(me,zoom)]]
  xth_me_cmds_move_scrap_xctrl $pid $nx $ny
  set xth(ctrl,me,scrap,px$pid) $nx
  set xth(ctrl,me,scrap,py$pid) $ny
  update idletasks
}


proc xth_me_cmds_end_scrap_xctrl_drag {pid x y} {
  global xth
  xth_me_cmds_scrap_xctrl_drag $pid $x $y
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <B1-Motion> ""
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <B1-ButtonRelease> ""
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <Enter> $xth(me,scrap,xdrag_benter)
  $xth(me,can) bind $xth(me,canid,scrap,scp$pid) <Leave> $xth(me,scrap,xdrag_bleave)
  $xth(me,can) itemconfigure $xth(me,canid,scrap,scp$pid) -fill yellow
  $xth(me,can) configure -cursor crosshair
  xth_me_cmds_update {}
}


proc xth_me_cmds_show_scrap_xctrl {x1 y1 x2 y2} {

  global xth
  
  xth_me_cmds_move_scrap_xctrl 1 $x1 $y1
  $xth(me,can) itemconfigure $xth(me,canid,scrap,scp1) -state normal
  $xth(me,can) raise $xth(me,canid,scrap,scp1)
  $xth(me,can) bind $xth(me,canid,scrap,scp1) <Enter> "xth_status_bar_push me; xth_status_bar_status me \"Scrap picture scale point 1.\"; $xth(me,can) itemconfigure $xth(me,canid,scrap,scp1) -fill yellow"
  $xth(me,can) bind $xth(me,canid,scrap,scp1) <Leave> "xth_status_bar_pop me; $xth(me,can) itemconfigure $xth(me,canid,scrap,scp1) -fill red"
  $xth(me,can) bind $xth(me,canid,scrap,scp1) <1> "xth_me_cmds_start_scrap_xctrl_drag 1 %x %y"

  xth_me_cmds_move_scrap_xctrl 2 $x2 $y2
  $xth(me,can) itemconfigure $xth(me,canid,scrap,scp2) -state normal
  $xth(me,can) raise $xth(me,canid,scrap,scp2)
  $xth(me,can) bind $xth(me,canid,scrap,scp2) <Enter> "xth_status_bar_push me; xth_status_bar_status me \"Scrap picture scale point 2.\"; $xth(me,can) itemconfigure $xth(me,canid,scrap,scp2) -fill yellow"
  $xth(me,can) bind $xth(me,canid,scrap,scp2) <Leave> "xth_status_bar_pop me; $xth(me,can) itemconfigure $xth(me,canid,scrap,scp2) -fill red"
  $xth(me,can) bind $xth(me,canid,scrap,scp2) <1> "xth_me_cmds_start_scrap_xctrl_drag 2 %x %y"
  
}


proc xth_me_cmds_update_scrap_ctrl {id} {
  global xth
  if {[string length $id] > 0} {
    $xth(ctrl,me,scrap).namel configure -state normal
    $xth(ctrl,me,scrap).name configure -state normal
    $xth(ctrl,me,scrap).projl configure -state normal
    $xth(ctrl,me,scrap).proj configure -state normal
    $xth(ctrl,me,scrap).optl configure -state normal
    $xth(ctrl,me,scrap).opt configure -state normal
    $xth(ctrl,me,scrap).scl configure -state normal
    $xth(ctrl,me,scrap).scpb configure -state normal
    $xth(ctrl,me,scrap).scpp configure -state normal
    $xth(ctrl,me,scrap).scrp configure -state normal
    $xth(ctrl,me,scrap).scx1p configure -state normal
    $xth(ctrl,me,scrap).scy1p configure -state normal
    $xth(ctrl,me,scrap).scx2p configure -state normal
    $xth(ctrl,me,scrap).scy2p configure -state normal
    $xth(ctrl,me,scrap).scx1r configure -state normal
    $xth(ctrl,me,scrap).scy1r configure -state normal
    $xth(ctrl,me,scrap).scx2r configure -state normal
    $xth(ctrl,me,scrap).scy2r configure -state normal
    $xth(ctrl,me,scrap).scu configure -state normal
    $xth(ctrl,me,scrap).scul configure -state normal
    set xth(ctrl,me,scrap,name) $xth(me,cmds,$id,name)
    set xth(ctrl,me,scrap,projection) $xth(me,cmds,$id,projection)
    set xth(ctrl,me,scrap,options) $xth(me,cmds,$id,options)
    set xth(ctrl,me,scrap,px1) [lindex $xth(me,cmds,$id,scale) 0]
    set xth(ctrl,me,scrap,py1) [lindex $xth(me,cmds,$id,scale) 1]
    set xth(ctrl,me,scrap,px2) [lindex $xth(me,cmds,$id,scale) 2]
    set xth(ctrl,me,scrap,py2) [lindex $xth(me,cmds,$id,scale) 3]
    set xth(ctrl,me,scrap,rx1) [lindex $xth(me,cmds,$id,scale) 4]
    set xth(ctrl,me,scrap,ry1) [lindex $xth(me,cmds,$id,scale) 5]
    set xth(ctrl,me,scrap,rx2) [lindex $xth(me,cmds,$id,scale) 6]
    set xth(ctrl,me,scrap,ry2) [lindex $xth(me,cmds,$id,scale) 7]
    set xth(ctrl,me,scrap,units) [lindex $xth(me,cmds,$id,scale) 8]
    xth_me_cmds_show_scrap_xctrl [lindex $xth(me,cmds,$id,scale) 0] \
      [lindex $xth(me,cmds,$id,scale) 1] [lindex $xth(me,cmds,$id,scale) 2] \
      [lindex $xth(me,cmds,$id,scale) 3]
    xth_me_prev_cmd $xth(me,cmds,$id,data)
  } else {
    set xth(ctrl,me,scrap,name) ""
    set xth(ctrl,me,scrap,projection) $xth(me,dflt,scrap,projection)
    set xth(ctrl,me,scrap,options) $xth(me,dflt,scrap,options)
    set xth(ctrl,me,scrap,px1) [lindex $xth(me,dflt,scrap,scale) 0]
    set xth(ctrl,me,scrap,py1) [lindex $xth(me,dflt,scrap,scale) 1]
    set xth(ctrl,me,scrap,px2) [lindex $xth(me,dflt,scrap,scale) 2]
    set xth(ctrl,me,scrap,py2) [lindex $xth(me,dflt,scrap,scale) 3]
    set xth(ctrl,me,scrap,rx1) [lindex $xth(me,dflt,scrap,scale) 4]
    set xth(ctrl,me,scrap,ry1) [lindex $xth(me,dflt,scrap,scale) 5]
    set xth(ctrl,me,scrap,rx2) [lindex $xth(me,dflt,scrap,scale) 6]
    set xth(ctrl,me,scrap,ry2) [lindex $xth(me,dflt,scrap,scale) 7]
    set xth(ctrl,me,scrap,units) [lindex $xth(me,dflt,scrap,scale) 8]
    $xth(ctrl,me,scrap).namel configure -state disabled
    $xth(ctrl,me,scrap).name configure -state disabled
    $xth(ctrl,me,scrap).projl configure -state disabled
    $xth(ctrl,me,scrap).proj configure -state disabled
    $xth(ctrl,me,scrap).optl configure -state disabled
    $xth(ctrl,me,scrap).opt configure -state disabled
    $xth(ctrl,me,scrap).scl configure -state disabled
    $xth(ctrl,me,scrap).scpb configure -state disabled
    $xth(ctrl,me,scrap).scpp configure -state disabled
    $xth(ctrl,me,scrap).scrp configure -state disabled
    $xth(ctrl,me,scrap).scx1p configure -state disabled
    $xth(ctrl,me,scrap).scy1p configure -state disabled
    $xth(ctrl,me,scrap).scx2p configure -state disabled
    $xth(ctrl,me,scrap).scy2p configure -state disabled
    $xth(ctrl,me,scrap).scx1r configure -state disabled
    $xth(ctrl,me,scrap).scy1r configure -state disabled
    $xth(ctrl,me,scrap).scx2r configure -state disabled
    $xth(ctrl,me,scrap).scy2r configure -state disabled
    $xth(ctrl,me,scrap).scu configure -state disabled
    $xth(ctrl,me,scrap).scul configure -state disabled
    xth_me_cmds_hide_scrap_xctrl    
  }
}

proc xth_me_cmds_update_scrap_vars {id} {

  global xth
  set xth(ctrl,me,scrap,name) $xth(me,cmds,$id,name)
  set xth(ctrl,me,scrap,projection) $xth(me,cmds,$id,projection)
  set xth(ctrl,me,scrap,options) $xth(me,cmds,$id,options)
  set xth(ctrl,me,scrap,px1) [lindex $xth(me,cmds,$id,scale) 0]
  set xth(ctrl,me,scrap,py1) [lindex $xth(me,cmds,$id,scale) 1]
  set xth(ctrl,me,scrap,px2) [lindex $xth(me,cmds,$id,scale) 2]
  set xth(ctrl,me,scrap,py2) [lindex $xth(me,cmds,$id,scale) 3]
  set xth(ctrl,me,scrap,rx1) [lindex $xth(me,cmds,$id,scale) 4]
  set xth(ctrl,me,scrap,ry1) [lindex $xth(me,cmds,$id,scale) 5]
  set xth(ctrl,me,scrap,rx2) [lindex $xth(me,cmds,$id,scale) 6]
  set xth(ctrl,me,scrap,ry2) [lindex $xth(me,cmds,$id,scale) 7]
  set xth(ctrl,me,scrap,units) [lindex $xth(me,cmds,$id,scale) 8]
  xth_me_cmds_move_scrap_xctrl 1 [lindex $xth(me,cmds,$id,scale) 0] \
    [lindex $xth(me,cmds,$id,scale) 1] 
  xth_me_cmds_move_scrap_xctrl 2 [lindex $xth(me,cmds,$id,scale) 2] \
    [lindex $xth(me,cmds,$id,scale) 3]

}


proc xth_me_cmds_update_scrap {id nname nproj nopt nscale} {

  global xth
  
  set oname $xth(me,cmds,$id,name)
  set oproj $xth(me,cmds,$id,projection)
  set oopt $xth(me,cmds,$id,options)
  set oscale $xth(me,cmds,$id,scale)

  regsub {^\s*} $nopt "" nopt
  regsub {\s*$} $nopt "" nopt
  if {[string length $nname] < 1} {
    set nname $oname
  }  
  if {[llength $nscale] < 8} {
    set nscale oscale
  } else {
    for {set i 0} {$i < 8} {incr i} {
      if {[catch {expr [lindex $nscale $i]}]} {
        set nscale [lreplace $nscale $i $i [lindex $oscale $i]]
      }
    }
  }
  if {[llength $nscale] == 9} {
    if {[string length [lindex $nscale 8]] == 0} {
      set nscale [lreplace $nscale 8 8]
    }
  }
  
  if {![string equal "$oname $oproj $oopt $oscale" "$nname $nproj $nopt $nscale"]} {
    xth_me_unredo_action "scrap changes" \
      "xth_me_cmds_update_scrap $id $oname [list $oproj] [list $oopt] [list $oscale]; xth_me_cmds_select $id" \
      "xth_me_cmds_update_scrap $id $nname [list $nproj] [list $nopt] [list $nscale]; xth_me_cmds_select $id"
    set xth(me,cmds,$id,name) $nname
    set xth(me,cmds,$id,projection) $nproj
    set xth(me,cmds,$id,options) $nopt
    set xth(me,cmds,$id,scale) $nscale
    xth_me_cmds_update_scrap_data $id
    xth_me_cmds_update_list $id
  }
  
}


proc xth_me_cmds_update_scrap_data {id} {

  global xth

  set d "scrap $xth(me,cmds,$id,name)"

  if {[llength $xth(me,cmds,$id,projection)] > 1} {
    set d "$d -projection \[$xth(me,cmds,$id,projection)\]"
  } elseif {[llength $xth(me,cmds,$id,projection)] > 0} {
    set d "$d -projection $xth(me,cmds,$id,projection)"
  }
  set xth(me,dflt,scrap,projection) $xth(me,cmds,$id,projection)

  if {[string length $xth(me,cmds,$id,options)] > 0} {
    set d "$d $xth(me,cmds,$id,options)"
  }
  set xth(me,dflt,scrap,options) $xth(me,cmds,$id,options)

  set d "$d -scale \[$xth(me,cmds,$id,scale)\]"
  set xth(me,dflt,scrap,scale) $xth(me,cmds,$id,scale)

  set xth(me,cmds,$id,data) "$d"
  
}


proc xth_me_cmds_create_scrap {ix mode name opts} {

  global xth
  xth_me_cmds_update {}
  set id [xth_me_cmds_create 4 {} $ix]

  if {[string length $name] > 0} {
    set xth(me,cmds,$id,name) $name
  } else {
    set xth(me,cmds,$id,name) "scrap$id"
  }

  if {$mode && ([string length $opts] < 1)} {
    set opts $xth(me,dflt,scrap,options)
  }
  
  # nastavit projekciu
  if {$mode} {
    set xth(me,cmds,$id,projection) $xth(me,dflt,scrap,projection)
  } else {
    set xth(me,cmds,$id,projection) {}
  }
  set optl [xth_me_cmds_get_line_option $opts projection]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,projection) [lindex $optl 0]
    set opts [lindex $optl 1]
  }
  set optl [xth_me_cmds_get_line_option $opts proj]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,projection) [lindex $optl 0]
    set opts [lindex $optl 1]
  }
  
  # nastavit scale
  set xth(me,cmds,$id,scale) {}
  if {[llength $xth(me,dflt,scrap,scale)] < 1} {
    set xth(me,dflt,scrap,scale) [list $xth(me,area,xmin) $xth(me,area,ymin) \
      $xth(me,area,xmax) $xth(me,area,ymin) 0.0 0.0 [expr 0.0254 * ($xth(me,area,xmax) - $xth(me,area,xmin))] 0.0 m]
  }
  set optl [xth_me_cmds_get_line_option $opts scale]
  set optv [lindex $optl 0]
  set opts [lindex $optl 1]
  switch [llength $optv] {
    1 {
      set xth(me,cmds,$id,scale) [list $xth(me,area,xmin) $xth(me,area,ymin) \
        $xth(me,area,xmax) $xth(me,area,ymin) 0.0 0.0 [expr 1.0 * $optv * ($xth(me,area,xmax) - $xth(me,area,xmin))] 0.0 m]
    }
    2 {
      set xth(me,cmds,$id,scale) [list $xth(me,area,xmin) $xth(me,area,ymin) \
        $xth(me,area,xmax) $xth(me,area,ymin) 0.0 0.0 [expr 1.0 * [lindex $optv 0] * ($xth(me,area,xmax) - $xth(me,area,xmin))] 0.0 [lindex $optv 1]]
    }
    3 {
      set xth(me,cmds,$id,scale) [list $xth(me,area,xmin) $xth(me,area,ymin) \
        $xth(me,area,xmax) $xth(me,area,ymin) 0.0 0.0 [expr 1.0 * [lindex $optv 1] / [lindex $optv 0] * ($xth(me,area,xmax) - $xth(me,area,xmin))] 0.0 [lindex $optv 2]]
    }
    8 {
      set xth(me,cmds,$id,scale) [list [lindex $optv 0] [lindex $optv 1] [lindex $optv 2] \
      [lindex $optv 3] [lindex $optv 4] [lindex $optv 5] [lindex $optv 6] [lindex $optv 7]]
    }
    9 {
      set xth(me,cmds,$id,scale) [list [lindex $optv 0] [lindex $optv 1] [lindex $optv 2] \
      [lindex $optv 3] [lindex $optv 4] [lindex $optv 5] [lindex $optv 6] [lindex $optv 7] [lindex $optv 8]]
    }
    default {
      set xth(me,cmds,$id,scale) $xth(me,dflt,scrap,scale)
    }
  }

  # nastavit options
  regsub {^\s*} $opts "" opts
  regsub {\s*$} $opts "" opts
  set xth(me,cmds,$id,options) $opts
  
  xth_me_cmds_update_list $id
  xth_me_cmds_update_scrap_data $id
  if {$mode} {
    xth_me_unredo_action "creating scrap" "xth_me_cmds_delete $id" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]"  
    xth_me_cmds_create_endscrap $ix $mode {}
  }
}


proc xth_me_cmds_create_text {ix mode data cpos} {
  global xth
  xth_me_cmds_update {}
  set id [xth_me_cmds_create 1 {} $ix]
  set xth(me,cmds,$id,data) $data
  set xth(me,cmds,$id,cpos) $cpos
  if {$mode} {
    xth_me_cmds_select $id
    xth_me_unredo_action "creating text" "xth_me_cmds_delete $id" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]"
  }
}


proc xth_me_cmds_update_text {id newdata newcpos} {
  global xth
  set olddata $xth(me,cmds,$id,data)
  set oldcpos $xth(me,cmds,$id,cpos)
  regsub {\s*$} $newdata "\n" newdata
  if {![string equal $xth(me,cmds,$id,data) $newdata]} {
    xth_me_unredo_action "text changes" \
      "xth_me_cmds_update_text $id [list $olddata] $oldcpos; xth_me_cmds_select $id" \
      "xth_me_cmds_update_text $id [list $newdata] $newcpos; xth_me_cmds_select $id"
    set xth(me,cmds,$id,data) $newdata    
    set xth(me,cmds,$id,cpos) $newcpos    
  }
}


proc xth_me_cmds_action {} {
  global xth
  switch $xth(me,cmds,action) {
    0 {
      xth_me_cmds_create_line {} 1 "" "" ""
      xth_ctrl_scroll_to me line
    }
    1 {
      xth_me_cmds_set_mode 1    
    }
    2 {
      xth_me_cmds_create_scrap {} 1 "" ""
    }
    3 {
      xth_me_cmds_create_text {} 1 "\n" "1.0"
    }
    4 {
      xth_me_cmds_delete {}
    }
    5 {
      xth_me_cmds_create_area {} 1 "" "" ""
      xth_ctrl_scroll_to me ac
    }
  }
}


proc xth_me_cmds_create_all {lns} {
  global xth
  set ctext {}
  xth_status_bar_push me
  set ctext_push {
    regsub {^\s*} $ctext {} ctext
    regsub {\s*$} $ctext "\n" ctext
    if {[string length $ctext] > 0} {
      xth_me_cmds_create_text [expr [llength $xth(me,cmds,xlist)] - 1] 0 "$ctext" 1.0
      set ctext {}
    }
  }
  set line_lines {}
  set line_type {}
  set line_opts {}
  set inline 0
  set linenumber 0
  set totallns [llength $lns]
  xth_me_progbar_show $totallns
  xth_status_bar_status me "Processing commands ..."
  foreach ln $lns {
    incr linenumber
    xth_me_progbar_prog $linenumber
    # here take care of special commands
    if {[regexp {^\s*scrap\s+(\S+)\s*(.*)$} $ln dum name opts]} {
      eval $ctext_push
      xth_me_cmds_create_scrap [expr [llength $xth(me,cmds,xlist)] - 1] 0 $name $opts
    } elseif {[regexp {^\s*endscrap\s*(\S*)\s*$} $ln dum name]} {
      eval $ctext_push
      xth_me_cmds_create_endscrap [expr [llength $xth(me,cmds,xlist)] - 1] 0 $name
    } elseif {[regexp {^\s*point\s+(\S+)\s+(\S+)\s+(\S+)\s*(.*)$} $ln dum x y type opts]} {
      eval $ctext_push
      xth_me_cmds_create_point [expr [llength $xth(me,cmds,xlist)] - 1] 0 $x $y $type $opts
    } elseif {[regexp {^\s*line\s+(\S+)\s*(.*)$} $ln dum line_type line_opts]} {
      eval $ctext_push
      set line_lines {}
      set inline 1
    } elseif {($inline == 1) && [regexp {^\s*endline(\s|$)} $ln]} {
      xth_me_cmds_create_line [expr [llength $xth(me,cmds,xlist)] - 1] 0 $line_type $line_opts $line_lines
      set line_lines {}
      set line_type {}
      set line_opts {}
      set inline 0
    } elseif {[regexp {^\s*area\s+(\S+)\s*(.*)$} $ln dum line_type line_opts]} {
      eval $ctext_push
      set line_lines {}
      set inline 2
    } elseif {($inline == 2) && [regexp {^\s*endarea(\s|$)} $ln]} {
      xth_me_cmds_create_area [expr [llength $xth(me,cmds,xlist)] - 1] 0 $line_type $line_opts $line_lines
      set line_lines {}
      set line_type {}
      set line_opts {}
      set inarea 0
    } elseif {($inline > 0)} {
      lappend line_lines $ln
    } else {
      set ctext "$ctext\n$ln"
    }
  }
  if {$inline > 0} {
    foreach ln $line_lines {
      set ctext "$ctext\n$ln"
    }
  }
  eval $ctext_push
  xth_me_progbar_hide
  xth_status_bar_pop me
}


proc xth_me_cmds_click {id tagOrId x y mx my} {
  global xth
  xth_me_cmds_update {}
  if {[llength $id] == 2} {
    set pid [lindex $id 1]
    set id [lindex $id 0]
  } else {
    set pid 0
  }
  
  switch $xth(me,cmds,mode) {
    0 {
      if {[llength $id] > 0} {
        if {$id != $xth(me,cmds,selid)} {
          xth_me_cmds_select "$id $pid"
          if {$pid == 0} {
            xth_ctrl_scroll_to me point
          } else {
            xth_ctrl_scroll_to me line
          }
        } else {
          switch $xth(me,cmds,$id,ct) {
            2 {xth_me_cmds_start_point_drag $id $mx $my}
            3 {
              if {$xth(me,cmds,selpid) != $pid} {
                xth_me_cmds_select_linept $id $pid
              } else {
                xth_me_cmds_start_linecp_drag pt$id.$pid $id 0 $pid 0 x $mx $my
              }
            }
          }
        }
      }
    }
    1 {
      xth_ctrl_scroll_to me point
      xth_me_cmds_create_point {} 1 $x $y {} {}
    }
    2 {
      xth_ctrl_scroll_to me line
      set fpid -1
      set lpid -1
      if {($id == $xth(me,cmds,selid)) && ([string length $id] > 0) && ($pid > 0)} {
        set xl $xth(me,cmds,$id,xplist)
        set lix [expr [llength $xl] - 2]
        if {$lix >= 0} {
          set fpid [lindex $xl 0]
          set lpid [lindex $xl $lix]
        }
      }
      if {($id == $xth(me,cmds,selid)) && ($pid != 0) && ($pid == $xth(me,cmds,inspid))} {
        xth_me_cmds_end_line
      } elseif {($id == $xth(me,cmds,selid)) && ($pid == $fpid) && ($xth(me,cmds,inspid) == 0)} {
        xth_me_cmds_close_line $id
      } elseif {($id == $xth(me,cmds,selid)) && ($pid == $lpid) && ($xth(me,cmds,inspid) == 0)} {
        xth_me_cmds_end_line
      } else {
        # vytvori novy bod
        xth_me_cmds_start_create_linept $tagOrId $x $y $mx $my
      }      
    }
    3 {
      if {$xth(me,cmds,$id,ct) == 3} {
        xth_me_cmds_insert_area_lineid $id $mx $my
      }
    }
  }
}


proc xth_me_cmds_click_lineln {id tagOrId mx my} {
  global xth
  xth_me_cmds_update {}
  if {[llength $id] == 2} {
    set pid [lindex $id 1]
    set id [lindex $id 0]
  } else {
    set pid 0
  }

  switch $xth(me,cmds,mode) {
    3 {
      if {$xth(me,cmds,$id,ct) == 3} {
        xth_me_cmds_insert_area_lineid $id $mx $my
      }
    }
    0 {
      xth_me_cmds_select "$id $pid"
    }
    default {
      xth_me_cmds_click_area $tagOrId $mx $my
    }
  }
}



proc xth_me_cmds_click_area {tagOrId x y} {
  global xth
  xth_me_cmds_click {} $tagOrId [xth_me_can2realx [$xth(me,can) canvasx $x]] [xth_me_can2realy [$xth(me,can) canvasy $y]] $x $y
}


proc xth_me_cmds_set_mode {nmode} {
  
  global xth
  
  if {!$xth(me,fopen)} {
    return
  }

  if {($nmode == 0) && ($xth(me,cmds,mode) == 0)} {
    xth_me_cmds_select_nopoint
  }

  set xth(me,cmds,mode) $nmode
  switch $nmode {
    0 {
      $xth(me,mbar) configure -text "select object" -bg green -fg black
      $xth(ctrl,me,ac).ins configure -text "Insert"
    }
    1 {
      $xth(me,mbar) configure -text "insert point" -bg red -fg white
    }
    2 {
      $xth(me,mbar) configure -text "insert line point" -bg red -fg white
    }
    3 {
      $xth(me,mbar) configure -text "insert area border" -bg red -fg white
      $xth(ctrl,me,ac).ins configure -text "Select"
    }
  }
  
}


proc xth_me_cmds_create_point {ix mode x y type opts} {

  global xth
  xth_me_cmds_update {}
  set id [xth_me_cmds_create 2 {} $ix]

  set xth(me,cmds,$id,x) $x
  set xth(me,cmds,$id,y) $y
  
  if {$mode && ([string length $opts] < 1)} {
    set opts $xth(me,dflt,point,options)
  }

  if {[string length $type] > 0} {
    set xth(me,cmds,$id,type) $type
  } else {
    set xth(me,cmds,$id,type) $xth(me,dflt,point,type)
  }
  
  # nastavit meno
  set optl [xth_me_cmds_get_line_option $opts id]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,name) [lindex $optl 0]
    set opts [lindex $optl 1]
  }
  
  # nastavit rotation
  if {$mode} {
    set xth(me,cmds,$id,rotation) $xth(me,dflt,point,rotation)
  } else {
    set xth(me,cmds,$id,rotation) {}
  }
  set optl [xth_me_cmds_get_line_option $opts orientation]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,rotation) [lindex $optl 0]
    set opts [lindex $optl 1]
  } else {
    set optl [xth_me_cmds_get_line_option $opts orient]
    if {[lindex $optl 2]} {
      set xth(me,cmds,$id,rotation) [lindex $optl 0]
      set opts [lindex $optl 1]
    }
  }

  # nastavit xsize
  if {$mode} {
    set xth(me,cmds,$id,xsize) $xth(me,dflt,point,xsize)
  } else {
    set xth(me,cmds,$id,xsize) {}
  }
  set optl [xth_me_cmds_get_line_option $opts size]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,xsize) [lindex $optl 0]
    set xth(me,cmds,$id,ysize) [lindex $optl 0]
    set opts [lindex $optl 1]
  }
  set optl [xth_me_cmds_get_line_option $opts "x-size"]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,xsize) [lindex $optl 0]
    set opts [lindex $optl 1]
  }

  # nastavit ysize
  if {$mode} {
    set xth(me,cmds,$id,ysize) $xth(me,dflt,point,ysize)
  } else {
    set xth(me,cmds,$id,ysize) {}
  }
  set optl [xth_me_cmds_get_line_option $opts "y-size"]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,ysize) [lindex $optl 0]
    set opts [lindex $optl 1]
  }
  
#  if {([string length $xth(me,cmds,$id,ysize)] > 0) &&
#      ([string length $xth(me,cmds,$id,xsize)] == 0)} {
#    set xth(me,cmds,$id,xsize) $xth(me,cmds,$id,ysize)
#    set xth(me,cmds,$id,ysize) {}
#  }

  # nastavit options
  regsub {^\s*} $opts "" opts
  regsub {\s*$} $opts "" opts
  set xth(me,cmds,$id,options) $opts
  
  xth_me_cmds_draw_point $id
  if {$mode} {
    $xth(me,can) itemconfigure pt$id -fill lightBlue
  }
  xth_me_cmds_update_list $id
  xth_me_cmds_update_point_data $id
  if {$mode} {
    xth_me_unredo_action "creating point" "xth_me_cmds_delete $id" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]"  
    xth_me_cmds_select $id
  }

}


proc xth_me_cmds_update_point_data {id} {

  global xth

  set d "point $xth(me,cmds,$id,x) $xth(me,cmds,$id,y) $xth(me,cmds,$id,type)"
  set xth(me,dflt,point,type) $xth(me,cmds,$id,type)

  if {[string length $xth(me,cmds,$id,name)] > 0} {
    set d "$d -id $xth(me,cmds,$id,name)"
  }
  
  if {[string length $xth(me,cmds,$id,rotation)] > 0} {
    set d "$d -orientation $xth(me,cmds,$id,rotation)"
  }
  set xth(me,dflt,point,rotation) $xth(me,cmds,$id,rotation)

  if {[string length $xth(me,cmds,$id,xsize)] > 0} {
    set d "$d -x-size $xth(me,cmds,$id,xsize)"
  }
  set xth(me,dflt,point,xsize) $xth(me,cmds,$id,xsize)

  if {[string length $xth(me,cmds,$id,ysize)] > 0} {
    set d "$d -y-size $xth(me,cmds,$id,ysize)"
  }
  set xth(me,dflt,point,ysize) $xth(me,cmds,$id,ysize)

  if {[string length $xth(me,cmds,$id,options)] > 0} {
    set d "$d $xth(me,cmds,$id,options)"
  }
  set xth(me,dflt,point,options) $xth(me,cmds,$id,options)

  set xth(me,cmds,$id,data) "$d"
  
}


proc xth_me_cmds_move_point_xctrl {id} {
  global xth
  set cx [xth_me_real2canx $xth(me,cmds,$id,x)]
  set cy [xth_me_real2cany $xth(me,cmds,$id,y)]
  $xth(me,can) coords $xth(me,canid,point,selector) [list \
    [expr $cx - 3 * $xth(gui,me,point,psize)] \
    [expr $cy - 3 * $xth(gui,me,point,psize)] \
    [expr $cx + 3 * $xth(gui,me,point,psize)] \
    [expr $cy + 3 * $xth(gui,me,point,psize)]
  ]
}

proc xth_me_cmds_show_point_xctrl {id} {
  global xth
  $xth(me,can) itemconfigure $xth(me,canid,point,selector) -state normal
  $xth(me,can) raise $xth(me,canid,point,selector)
  $xth(me,can) raise ptfill
  $xth(me,can) lower ptfill point
  xth_me_cmds_move_point_xctrl $id
  xth_me_cmds_show_point_fill_xctrl $id
  $xth(me,can) raise pt$id
}


proc xth_me_cmds_show_point_fill_xctrl {id} {
  global xth
  if {$xth(ctrl,me,point,xsid) || $xth(ctrl,me,point,ysid)} {
    xth_me_cmds_configure_point_fill_xctrl $id 1
    xth_me_cmds_move_point_fill_xctrl $id $xth(ctrl,me,point,rot) $xth(ctrl,me,point,xs) $xth(ctrl,me,point,ys)
  } elseif {$xth(ctrl,me,point,rotid)} {
    xth_me_cmds_configure_point_fill_xctrl $id 0
    xth_me_cmds_move_point_fill_xctrl $id $xth(ctrl,me,point,rot) $xth(ctrl,me,point,xs) $xth(ctrl,me,point,ys)
  } else {
    xth_me_cmds_configure_point_fill_xctrl {} 0
  }
}


proc xth_me_cmds_hide_point_xctrl {} {
  global xth
  $xth(me,can) itemconfigure $xth(me,canid,point,selector) -state hidden
  xth_me_cmds_configure_point_fill_xctrl {} {}
}


proc xth_me_cmds_update_point_ctrl {id} {
  global xth
  if {[string length $id] > 0} {

    $xth(ctrl,me,point).posl configure -state normal
    $xth(ctrl,me,point).posx configure -state normal
    $xth(ctrl,me,point).posy configure -state normal
    $xth(ctrl,me,point).upd configure -state normal
    $xth(ctrl,me,point).typl configure -state normal
    $xth(ctrl,me,point).typ configure -state normal
    $xth(ctrl,me,point).namel configure -state normal
    $xth(ctrl,me,point).name configure -state normal
    $xth(ctrl,me,point).optl configure -state normal
    $xth(ctrl,me,point).opt configure -state normal
    $xth(ctrl,me,point).rotc configure -state normal
    $xth(ctrl,me,point).rot configure -state normal
    $xth(ctrl,me,point).xszc configure -state normal
    $xth(ctrl,me,point).xsz configure -state normal
    $xth(ctrl,me,point).yszc configure -state normal
    $xth(ctrl,me,point).ysz configure -state normal
    
    set xth(ctrl,me,point,x) $xth(me,cmds,$id,x)
    set xth(ctrl,me,point,y) $xth(me,cmds,$id,y)
    set xth(ctrl,me,point,type) $xth(me,cmds,$id,type)
    set xth(ctrl,me,point,name) $xth(me,cmds,$id,name)
    set xth(ctrl,me,point,opts) $xth(me,cmds,$id,options)

    set xth(ctrl,me,point,rot) $xth(me,cmds,$id,rotation)
    if {[string length $xth(me,cmds,$id,rotation)] > 0} {
      set xth(ctrl,me,point,rotid) 1
    } else {
      set xth(ctrl,me,point,rotid) 0
    }

    set xth(ctrl,me,point,xs) $xth(me,cmds,$id,xsize)
    if {[string length $xth(me,cmds,$id,xsize)] > 0} {
      set xth(ctrl,me,point,xsid) 1
    } else {
      set xth(ctrl,me,point,xsid) 0
    }

    set xth(ctrl,me,point,ys) $xth(me,cmds,$id,ysize)
    if {[string length $xth(me,cmds,$id,ysize)] > 0} {
      set xth(ctrl,me,point,ysid) 1
    } else {
      set xth(ctrl,me,point,ysid) 0
    }
    
    xth_me_cmds_show_point_xctrl $id
    xth_me_prev_cmd $xth(me,cmds,$id,data)
    
  } else {
  
    set xth(ctrl,me,point,x) {}
    set xth(ctrl,me,point,y) {}
    set xth(ctrl,me,point,type) $xth(me,dflt,point,type)
    set xth(ctrl,me,point,name) {}
    set xth(ctrl,me,point,opts) $xth(me,dflt,point,options)

    set xth(ctrl,me,point,rot) $xth(me,dflt,point,rotation)
    if {[string length $xth(me,dflt,point,rotation)] > 0} {
      set xth(ctrl,me,point,rotid) 1
    } else {
      set xth(ctrl,me,point,rotid) 0
    }

    set xth(ctrl,me,point,xs) $xth(me,dflt,point,xsize)
    if {[string length $xth(me,dflt,point,xsize)] > 0} {
      set xth(ctrl,me,point,xsid) 1
    } else {
      set xth(ctrl,me,point,xsid) 0
    }

    set xth(ctrl,me,point,ys) $xth(me,dflt,point,ysize)
    if {[string length $xth(me,dflt,point,ysize)] > 0} {
      set xth(ctrl,me,point,ysid) 1
    } else {
      set xth(ctrl,me,point,ysid) 0
    }

    $xth(ctrl,me,point).posl configure -state disabled
    $xth(ctrl,me,point).posx configure -state disabled
    $xth(ctrl,me,point).posy configure -state disabled
    $xth(ctrl,me,point).upd configure -state disabled
    $xth(ctrl,me,point).typl configure -state disabled
    $xth(ctrl,me,point).typ configure -state disabled
    $xth(ctrl,me,point).namel configure -state disabled
    $xth(ctrl,me,point).name configure -state disabled
    $xth(ctrl,me,point).optl configure -state disabled
    $xth(ctrl,me,point).opt configure -state disabled
    $xth(ctrl,me,point).rotc configure -state disabled
    $xth(ctrl,me,point).rot configure -state disabled
    $xth(ctrl,me,point).xszc configure -state disabled
    $xth(ctrl,me,point).xsz configure -state disabled
    $xth(ctrl,me,point).yszc configure -state disabled
    $xth(ctrl,me,point).ysz configure -state disabled
    
    xth_me_cmds_hide_point_xctrl  
  }
}


proc xth_me_cmds_update_point_vars {id} {

  global xth
  set xth(ctrl,me,point,x) $xth(me,cmds,$id,x)
  set xth(ctrl,me,point,y) $xth(me,cmds,$id,y)
  set xth(ctrl,me,point,type) $xth(me,cmds,$id,type)
  set xth(ctrl,me,point,name) $xth(me,cmds,$id,name)
  set xth(ctrl,me,point,opts) $xth(me,cmds,$id,options)
  set xth(ctrl,me,point,rot) $xth(me,cmds,$id,rotation)
  
  if {[string length $xth(me,cmds,$id,rotation)] > 0} {
    set xth(ctrl,me,point,rotid) 1
  } else {
    set xth(ctrl,me,point,rotid) 0
  }
  
  set xth(ctrl,me,point,xs) $xth(me,cmds,$id,xsize)
  if {[string length $xth(me,cmds,$id,xsize)] > 0} {
    set xth(ctrl,me,point,xsid) 1
  } else {
    set xth(ctrl,me,point,xsid) 0
  }
  
  set xth(ctrl,me,point,ys) $xth(me,cmds,$id,ysize)
  if {[string length $xth(me,cmds,$id,ysize)] > 0} {
    set xth(ctrl,me,point,ysid) 1
  } else {
    set xth(ctrl,me,point,ysid) 0
  }

  xth_me_cmds_move_point_xctrl $id
  xth_me_cmds_show_point_fill_xctrl $id
  
}

proc xth_me_cmds_update_area {id ntype nopt} {

  global xth
  
  set otype $xth(me,cmds,$id,type)
  set oopt $xth(me,cmds,$id,options)

  regsub {^\s*} $nopt "" nopt
  regsub {\s*$} $nopt "" nopt

  if {[string length $ntype] < 1} {
    set ntype $otype
  }
  if {(![string equal $ntype $otype]) && [string equal $nopt $oopt]} {
    set nopt {}
  }
  
  if {![string equal "$ntype $nopt" "$otype $oopt"]} {
    xth_me_unredo_action "area changes" \
      "xth_me_cmds_update_area $id $otype [list $oopt]; xth_me_cmds_select $id" \
      "xth_me_cmds_update_area $id $ntype [list $nopt]; xth_me_cmds_select $id"
    set xth(me,cmds,$id,type) $ntype
    set xth(me,cmds,$id,options) $nopt
    xth_me_cmds_update_area_data $id
    xth_me_cmds_update_list $id
  }

}




proc xth_me_cmds_update_point {id nx ny ntype nname nopt nrot nxs nys} {

  global xth
  
  set ox $xth(me,cmds,$id,x)
  set oy $xth(me,cmds,$id,y)
  set otype $xth(me,cmds,$id,type)
  set oname $xth(me,cmds,$id,name)
  set oopt $xth(me,cmds,$id,options)
  set orot $xth(me,cmds,$id,rotation)
  set oxs $xth(me,cmds,$id,xsize)
  set oys $xth(me,cmds,$id,ysize)

  regsub {^\s*} $nopt "" nopt
  regsub {\s*$} $nopt "" nopt

  if {[string length $ntype] < 1} {
    set ntype $otype
  }
  if {(![string equal $ntype $otype]) && [string equal $nopt $oopt]} {
    set nopt {}
    set nrot {}
    set nxs {}
    set nys {}
  }
  
  if {[string length $nrot] > 0} {
    if {[catch {expr $nrot}]} {
      set nrot $orot
    } elseif {($nrot < 0.0) || ($nrot >= 360.0)} {
      set nrot $orot
    }
  }
  
  if {[catch {expr $nx}]} {
    set nx $ox
  }
  if {[catch {expr $ny}]} {
    set ny $oy
  }
  
  if {[string length $nxs] > 0} {
    if {[catch {expr $nxs}]} {
      set nxs $oxs
    } elseif {$nxs < 0.0} {
      set nxs $oxs
    }
  }
  if {[string length $nys] > 0} {
    if {[catch {expr $nys}]} {
      set nys $oys
    } elseif {$nys < 0.0} {
      set nys $oys
    }
  }
#  if {([string length $nys] > 0) && ([string length $nxs] == 0)} {
#    set nxs $nys
#    set nys {}
#  }
  
  if {![string equal "$nx $ny $ntype $nname $nopt $nrot $nxs $nys" "$ox $oy $otype $oname $oopt $orot $oxs $oys"]} {
    xth_me_unredo_action "point changes" \
      "xth_me_cmds_update_point $id $ox $oy $otype [list $oname] [list $oopt] [list $orot] [list $oxs] [list $oys]; xth_me_cmds_select $id" \
      "xth_me_cmds_update_point $id $nx $ny $ntype [list $nname] [list $nopt] [list $nrot] [list $nxs] [list $nys]; xth_me_cmds_select $id"
    set xth(me,cmds,$id,x) $nx
    set xth(me,cmds,$id,y) $ny
    set xth(me,cmds,$id,type) $ntype
    set xth(me,cmds,$id,name) $nname
    set xth(me,cmds,$id,options) $nopt
    set xth(me,cmds,$id,rotation) $nrot
    set xth(me,cmds,$id,xsize) $nxs
    set xth(me,cmds,$id,ysize) $nys
    $xth(me,can) coords pt$id [xth_me_cmds_calc_point_coords $id]
    xth_me_cmds_update_point_data $id
    xth_me_cmds_update_list $id
  }
  
}


proc xth_me_cmds_calc_point_coords {id} {
  global xth
  return [list \
    [expr [xth_me_real2canx $xth(me,cmds,$id,x)] - $xth(gui,me,point,psize)] \
    [expr [xth_me_real2cany $xth(me,cmds,$id,y)] - $xth(gui,me,point,psize)] \
    [expr [xth_me_real2canx $xth(me,cmds,$id,x)] + $xth(gui,me,point,psize)] \
    [expr [xth_me_real2cany $xth(me,cmds,$id,y)] + $xth(gui,me,point,psize)]
  ]
}


proc xth_me_cmds_draw_point {id} {
  global xth
  $xth(me,can) create oval [xth_me_cmds_calc_point_coords $id] \
    -tags "command point pt$id" -width 1 -outline blue -fill blue
  $xth(me,can) bind pt$id <Enter> "$xth(me,can) itemconfigure pt$id -fill cyan; xth_status_bar_push me; xth_status_bar_status me \"\$xth(me,cmds,$id,listix): \$xth(me,cmds,$id,data)\""
  $xth(me,can) bind pt$id <Leave> "$xth(me,can) itemconfigure pt$id -fill \[$xth(me,can) itemcget pt$id -outline\]; xth_status_bar_pop me"
  $xth(me,can) bind pt$id <1> "xth_me_cmds_click $id pt$id \$xth(me,cmds,$id,x) \$xth(me,cmds,$id,y) %x %y"
  $xth(me,can) bind pt$id <3> "xth_me_cmds_special_select $id %x %y"  
  $xth(me,can) bind pt$id <Shift-1> "xth_me_cmds_special_select $id %x %y"  
  $xth(me,can) bind pt$id <Control-1> "xth_me_cmds_click_area pt$id %x %y"
}


proc xth_me_cmds_special_select {id x y} {
  global xth
  if {[llength $id] == 2} {
    set pid [lindex $id 1]
    set id [lindex $id 0]
  } else {
    set pid 0
  }
  $xth(me,can) raise point
  if {$xth(me,cmds,selid) != $id} {
    xth_me_cmds_select "$id $pid"
    if {$pid != 0} {
      xth_ctrl_scroll_to me line
    } else {
      xth_ctrl_scroll_to me point
    }
  } elseif {($xth(me,cmds,$id,ct) == 3) && ($xth(me,cmds,selpid) != $pid)} {
    xth_me_cmds_select_linept $id $pid
    xth_ctrl_scroll_to me line
  } else {
    $xth(me,can) dtag all nearest
    if {$pid != 0} {
      set utag pt$id.$pid
    } else {
      set utag pt$id
    }
    $xth(me,can) addtag nearest closest [$xth(me,can) canvasx $x] [$xth(me,can) canvasy $y] 0 $utag
    set tgs [$xth(me,can) itemcget nearest -tags]
    #puts $tgs
    if {[regexp "(^|\\s)pt(\\d+)($|\\s)" $tgs d1 d2 nid]} {
      #puts "select $nid"
      xth_me_cmds_select $nid
      xth_ctrl_scroll_to me point
      catch {$xth(me,can) lower $utag point}
      catch {$xth(me,can) raise $utag line}
    } elseif {[regexp "(^|\\s)pt(\\d+)\.(\\d+)($|\\s)" $tgs d1 d2 nid npid]} {
      #puts "select $nid $npid"
      xth_me_cmds_select "$nid $npid"
      if {$npid != 0} {
        xth_ctrl_scroll_to me line
      } else {
        xth_ctrl_scroll_to me point
      }
      catch {$xth(me,can) lower $utag point}
      catch {$xth(me,can) raise $utag line}
    }
  }
}


proc xth_me_cmds_start_point_drag {id x y} {
  global xth
  xth_me_cmds_update {}
  xth_me_cmds_drag_point_config_xctrl $id
  set xth(me,point,drag_mx) $x
  set xth(me,point,drag_my) $y
  set xth(me,point,drag_px) $xth(me,cmds,$id,x)
  set xth(me,point,drag_py) $xth(me,cmds,$id,y)
  set xth(me,point,drag_benter) [$xth(me,can) bind pt$id <Enter>]
  set xth(me,point,drag_bleave) [$xth(me,can) bind pt$id <Leave>]
  $xth(me,can) bind pt$id <Enter> ""
  $xth(me,can) bind pt$id <Leave> ""
  $xth(me,can) itemconfigure pt$id -fill {}
  $xth(me,can) bind pt$id <B1-Motion> "xth_me_cmds_point_drag $id %x %y 1"
  $xth(me,can) bind pt$id <B1-ButtonRelease> "xth_me_cmds_end_point_drag $id %x %y 1"
  $xth(me,can) bind pt$id <Control-B1-Motion> "xth_me_cmds_point_drag $id %x %y 0"
  $xth(me,can) bind pt$id <Control-B1-ButtonRelease> "xth_me_cmds_end_point_drag $id %x %y 0"
  $xth(me,can) configure -cursor {}
}


proc xth_me_cmds_drag_to {id pid x y} {
  global xth
  $xth(me,can) dtag all nearest
  if {[string length $pid] > 0} {
    set stt pt$id.$pid
  } else {
    set stt pt$id
  }
  $xth(me,can) addtag nearest closest [$xth(me,can) canvasx $x] \
    [$xth(me,can) canvasy $y] 0 $stt
  set tgs [$xth(me,can) itemcget nearest -tags]
  if {[regexp "(^|\\s)pt(\\d+)($|\\s)" $tgs d1 d2 nid]} {
    # je vybraty bod, nastavime suradnice podla neho
    set nx $xth(me,cmds,$nid,x)
    set ny $xth(me,cmds,$nid,y)
    return [list 1 $nx $ny]
  } elseif {[regexp "(^|\\s)pt(\\d+)\.(\\d+)($|\\s)" $tgs d1 d2 nid npid]} {
    # je vybraty bod na ciare, nastavime suradnice podla neho
    set nx $xth(me,cmds,$nid,$npid,x)
    set ny $xth(me,cmds,$nid,$npid,y)
    return [list 1 $nx $ny]
  }
  return 0
}


proc xth_me_cmds_point_drag {id x y dragto} {
  global xth
  set nx [expr $xth(me,point,drag_px) - [expr double($xth(me,point,drag_mx) - $x) * 100.0 / $xth(me,zoom)]]
  set ny [expr $xth(me,point,drag_py) + [expr double($xth(me,point,drag_my) - $y) * 100.0 / $xth(me,zoom)]]
  set dts 0
  if $dragto {
    set dtl [xth_me_cmds_drag_to $id {} $x $y]
    if {[lindex $dtl 0]} {
      set nx [lindex $dtl 1]
      set ny [lindex $dtl 2]
      set dts 1
    }
  }
  if $dts {
    $xth(me,can) itemconfigure pt$id -fill cyan
  } else {
    $xth(me,can) itemconfigure pt$id -fill {}
  }
  set xth(me,cmds,$id,x) $nx
  set xth(me,cmds,$id,y) $ny
  set xth(ctrl,me,point,x) $nx
  set xth(ctrl,me,point,y) $ny
  xth_me_cmds_move_point_xctrl $id
  xth_me_cmds_move_point_fill_xctrl $id $xth(me,cmds,$id,rotation) $xth(me,cmds,$id,xsize) $xth(me,cmds,$id,ysize) 
  $xth(me,can) coords pt$id [xth_me_cmds_calc_point_coords $id]
  update idletasks
}


proc xth_me_cmds_end_point_drag {id x y dragto} {
  global xth
  xth_me_cmds_point_drag $id $x $y $dragto
  set xth(me,cmds,$id,x) $xth(me,point,drag_px)
  set xth(me,cmds,$id,y) $xth(me,point,drag_py)
  $xth(me,can) bind pt$id <B1-Motion> ""
  $xth(me,can) bind pt$id <B1-ButtonRelease> ""
  $xth(me,can) bind pt$id <Control-B1-Motion> ""
  $xth(me,can) bind pt$id <Control-B1-ButtonRelease> ""
  $xth(me,can) bind pt$id <Enter> $xth(me,point,drag_benter)
  $xth(me,can) bind pt$id <Leave> $xth(me,point,drag_bleave)
  $xth(me,can) itemconfigure pt$id -fill cyan
  $xth(me,can) configure -cursor crosshair
  set xth(me,unredola) "point dragging"
  xth_me_cmds_update {}
  xth_me_cmds_end_drag_point_config_xctrl $id
}


proc xth_me_cmds_configure_point_fill_xctrl {id sid} {
  global xth
  if {[string length $id] > 0} {
    $xth(me,can) itemconfigure $xth(me,canid,point,fx) -state normal
    $xth(me,can) bind $xth(me,canid,point,fx) <1> \
      "xth_me_cmds_start_point_fdrag $xth(me,canid,point,fx) $id x %x %y"
    $xth(me,can) bind $xth(me,canid,point,fx) <Enter> \
      "$xth(me,can) itemconfigure $xth(me,canid,point,fx) -fill #ffda00"
    $xth(me,can) bind $xth(me,canid,point,fx) <Leave> \
      "$xth(me,can) itemconfigure $xth(me,canid,point,fx) -fill red"
    $xth(me,can) itemconfigure $xth(me,canid,point,fy) -state normal
    $xth(me,can) bind $xth(me,canid,point,fy) <1> \
      "xth_me_cmds_start_point_fdrag $xth(me,canid,point,fy) $id y %x %y"
    $xth(me,can) bind $xth(me,canid,point,fy) <Enter> \
      "$xth(me,can) itemconfigure $xth(me,canid,point,fy) -fill #ffda00"
    $xth(me,can) bind $xth(me,canid,point,fy) <Leave> \
      "$xth(me,can) itemconfigure $xth(me,canid,point,fy) -fill red"
    #$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -state normal
    #$xth(me,can) bind $xth(me,canid,point,fxc) <Enter> \
    #  "$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -fill yellow"
    #$xth(me,can) bind $xth(me,canid,point,fxc) <Leave> \
    #  "$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -fill red"
    #$xth(me,can) bind $xth(me,canid,point,fxc) <1> \
    #  "xth_me_cmds_start_point_fdrag $xth(me,canid,point,fxc) $id x %x %y"
    #$xth(me,can) itemconfigure $xth(me,canid,point,fyc) -state normal
    #$xth(me,can) bind $xth(me,canid,point,fyc) <Enter> \
    #  "$xth(me,can) itemconfigure $xth(me,canid,point,fyc) -fill yellow"
    #$xth(me,can) bind $xth(me,canid,point,fyc) <Leave> \
    #  "$xth(me,can) itemconfigure $xth(me,canid,point,fyc) -fill red"
    #$xth(me,can) bind $xth(me,canid,point,fyc) <1> \
    #  "xth_me_cmds_start_point_fdrag $xth(me,canid,point,fyc) $id y %x %y"
    if {$sid} {
      $xth(me,can) itemconfigure $xth(me,canid,point,fill) -state normal
      #$xth(me,can) bind $xth(me,canid,point,fill) <1> \
      #  "xth_me_cmds_start_point_fdrag $xth(me,canid,point,fill) $id z %x %y"
    } else {
      $xth(me,can) itemconfigure $xth(me,canid,point,fill) -state hidden
    }
  } else {
      # $xth(me,can) bind $xth(me,canid,point,fxc) <Enter> ""
      # $xth(me,can) bind $xth(me,canid,point,fxc) <Leave> ""
      # $xth(me,can) bind $xth(me,canid,point,fyc) <Enter> ""
      # $xth(me,can) bind $xth(me,canid,point,fyc) <Leave> ""
      $xth(me,can) bind $xth(me,canid,point,fx) <1> ""
      $xth(me,can) bind $xth(me,canid,point,fy) <1> ""
      $xth(me,can) bind $xth(me,canid,point,fx) <Enter> ""
      $xth(me,can) bind $xth(me,canid,point,fx) <Leave> ""
      $xth(me,can) bind $xth(me,canid,point,fy) <Enter> ""
      $xth(me,can) bind $xth(me,canid,point,fy) <Leave> ""
      #$xth(me,can) bind $xth(me,canid,point,fill) <1> ""
      $xth(me,can) itemconfigure ptfill -state hidden
  }
}


proc xth_me_cmds_move_point_fill_xctrl {id rot sx sy} {

  global xth
  
  if {[string length $rot] > 0} {
    set rot [expr double($rot) / 180 * 3.14159265359]
  } else {
    set rot 0.0
  }

  set setfsx 0
  if {[string length $sx] > 0} {
    set sx [expr $sx * 0.01 * $xth(me,zoom)]
    set fsx $sx
  } else {
    set sx 30.0
    set setfsx 1
  }

#  if {[string length $sy] > 0} {
#    set sy [expr $sy * 0.01 * $xth(me,zoom)]
#  } else {
#    if {[string length $sx] > 0} {
#      set sy $sx
#    } else {
#      set sy 30.0
#    }
#  }

  if {[string length $sy] > 0} {
    set sy [expr $sy * 0.01 * $xth(me,zoom)]
    set fsy $sy
  } else {
    set sy 30.0
    set fsy $sx
  }

  if {$setfsx} {
    set fsx $sy
  }

  set x [xth_me_real2canx $xth(me,cmds,$id,x)]
  set y [xth_me_real2cany $xth(me,cmds,$id,y)]
  set ca [expr cos($rot)]
  set sa [expr sin($rot)]
  
  set xvx [expr $ca * $sx]
  set xvy [expr $sa * $sx]
  set yvx [expr $sa * $sy]
  set yvy [expr - $ca * $sy]

  set fxvx [expr $ca * $fsx]
  set fxvy [expr $sa * $fsx]
  set fyvx [expr $sa * $fsy]
  set fyvy [expr - $ca * $fsy]
  
  
  $xth(me,can) coords $xth(me,canid,point,fx) $x $y [expr $x + $xvx] [expr $y + $xvy]
  set xth(me,canid,point,fx_tox) [expr $x + $xvx]
  set xth(me,canid,point,fx_toy) [expr $y + $xvy]
  #$xth(me,can) coords $xth(me,canid,point,fxc) [expr $x + $xvx - $xth(gui,me,point,cpsize)] [expr $y + $xvy - $xth(gui,me,point,cpsize)] [expr $x + $xvx + $xth(gui,me,point,cpsize)] [expr $y + $xvy + $xth(gui,me,point,cpsize)]
  $xth(me,can) coords $xth(me,canid,point,fy) $x $y [expr $x + $yvx] [expr $y + $yvy]
  set xth(me,canid,point,fy_tox) [expr $x + $yvx]
  set xth(me,canid,point,fy_toy) [expr $y + $yvy]
  #$xth(me,can) coords $xth(me,canid,point,fyc) [expr $x + $yvx - $xth(gui,me,point,cpsize)] [expr $y + $yvy - $xth(gui,me,point,cpsize)] [expr $x + $yvx + $xth(gui,me,point,cpsize)] [expr $y + $yvy + $xth(gui,me,point,cpsize)]
  $xth(me,can) coords $xth(me,canid,point,fill) [expr $x + $fxvx + $fyvx] [expr $y + $fxvy + $fyvy] \
    [expr $x + $fxvx - $fyvx] [expr $y + $fxvy - $fyvy] [expr $x - $fxvx - $fyvx] [expr $y - $fxvy - $fyvy] \
    [expr $x - $fxvx + $fyvx] [expr $y - $fxvy + $fyvy]

  update idletasks    
}


proc xth_me_cmds_point_change_state {} {

  global xth  
  set newrotid $xth(ctrl,me,point,rotid)
  set newxsid $xth(ctrl,me,point,xsid)
  set newysid $xth(ctrl,me,point,ysid)

  xth_me_cmds_update {}

  if {$newrotid && \
    ([string length $xth(ctrl,me,point,rot)] < 1)} {
    set xth(ctrl,me,point,rot) 0.0
  } elseif {(! $newrotid) && \
    ([string length $xth(ctrl,me,point,rot)] > 0)} {
    set xth(ctrl,me,point,rot) {}
  }

  if {$newxsid && \
    ([string length $xth(ctrl,me,point,xs)] < 1)} {
    set xth(ctrl,me,point,xs) 40.0
  } elseif {(! $newxsid) && \
    ([string length $xth(ctrl,me,point,xs)] > 0)} {
    set xth(ctrl,me,point,xs) {}
#    set xth(ctrl,me,point,ys) {}
#    set xth(ctrl,me,point,ysid) 0
  }

  if {$newysid && \
    ([string length $xth(ctrl,me,point,ys)] < 1)} {
    set xth(ctrl,me,point,ys) 40.0
  } elseif {(! $newysid) && \
    ([string length $xth(ctrl,me,point,ys)] > 0)} {
    set xth(ctrl,me,point,ys) {}
  }

#  if {$newysid && \
#      ([string length $xth(ctrl,me,point,ys)] < 1) && \
#      $newxsid} {
#    set xth(ctrl,me,point,ys) 40.0
#  } elseif {((! $newysid) && \
#      ([string length $xth(ctrl,me,point,ys)] > 0)) || \
#      (! $newxsid)} {
#    set xth(ctrl,me,point,ys) {}
#    set xth(ctrl,me,point,ysid) 0    
#  }
  
  xth_me_cmds_update {}
  
}


proc xth_me_cmds_start_point_fdrag {tagOrId id ax x y} {
  global xth
  xth_me_cmds_update {}
  #if {[string equal $ax z]} {
  #  set distx [expr hypot([$xth(me,can) canvasx $x] - $xth(me,canid,point,fx_tox), \
  #    [$xth(me,can) canvasy $y] - $xth(me,canid,point,fx_toy))]
  #  set disty [expr hypot([$xth(me,can) canvasx $x] - $xth(me,canid,point,fy_tox), \
  #    [$xth(me,can) canvasy $y] - $xth(me,canid,point,fy_toy))]
  #  if {$disty < $distx} {
  #    set ax y
  #  } else {
  #    set ax x
  #  }
  #}
  set dx [expr [xth_me_can2realx [$xth(me,can) canvasx $x]] - $xth(me,cmds,$id,x)]
  set dy [expr [xth_me_can2realy [$xth(me,can) canvasy $y]] - $xth(me,cmds,$id,y)] 
  if {[string length $xth(me,cmds,$id,rotation)] == 0} {
    set xth(me,point,fdrag_rot) 0
  } else {
    set xth(me,point,fdrag_rot) 1
    set xth(me,point,fdrag_orot) [expr atan2($dy,$dx)]
  }
  if {([string length $xth(me,cmds,$id,xsize)] == 0) && ([string length $xth(me,cmds,$id,ysize)] == 0)} {
    set xth(me,point,fdrag_size) 0
  } elseif {[string equal $ax y] && ([string length $xth(me,cmds,$id,ysize)] == 0)} {
    set xth(me,point,fdrag_size) 0
  } elseif {[string equal $ax x] && ([string length $xth(me,cmds,$id,xsize)] == 0)} {
    set xth(me,point,fdrag_size) 0
  } else {
    set xth(me,point,fdrag_ax) $ax
    set xth(me,point,fdrag_size) 1
    set xth(me,point,fdrag_osize) [expr hypot($dy,$dx)]
  }
  $xth(me,can) itemconfigure $tagOrId -fill #ffda00
  set xth(me,point,fdrag_benter) [$xth(me,can) bind $tagOrId <Enter>]
  set xth(me,point,fdrag_bleave) [$xth(me,can) bind $tagOrId <Leave>]
  $xth(me,can) bind $tagOrId <Enter> ""
  $xth(me,can) bind $tagOrId <Leave> ""
  $xth(me,can) bind $tagOrId <B1-Motion> "xth_me_cmds_point_fdrag $id %x %y"
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> "xth_me_cmds_end_point_fdrag $tagOrId $id %x %y"
  $xth(me,can) configure -cursor {}
}

proc xth_me_cmds_point_fdrag {id x y} {
  global xth
  set dx [expr [xth_me_can2realx [$xth(me,can) canvasx $x]] - $xth(me,cmds,$id,x)]
  set dy [expr [xth_me_can2realy [$xth(me,can) canvasy $y]] - $xth(me,cmds,$id,y)] 
  if $xth(me,point,fdrag_rot) {
    set rot [expr double($xth(me,cmds,$id,rotation)) - 180.0 / 3.14159265359 * (atan2($dy,$dx) - $xth(me,point,fdrag_orot))]
    if {$rot < 0.0} {
      set rot [expr 360.0 + $rot]
    } elseif {$rot >= 360.0} {
      set rot [expr $rot - 360.0]
    }
    set rot [format "%.1f" $rot]
  } else {
    set rot $xth(me,cmds,$id,rotation)
  }
  set xth(ctrl,me,point,rot) $rot
  if {$xth(me,point,fdrag_size)} {
    set cs [expr hypot($dy,$dx)]
    switch $xth(me,point,fdrag_ax) {
      x {
        set ns [expr $xth(me,cmds,$id,xsize) - $xth(me,point,fdrag_osize) + $cs]
        if {$ns <= 0.0} {set ns 0.1}
        set ns [format "%.1f" $ns]
        set xth(ctrl,me,point,xs) $ns
      }
      y {
        set ns [expr $xth(me,cmds,$id,ysize) - $xth(me,point,fdrag_osize) + $cs]
        if {$ns <= 0.0} {set ns 0.1}
        set ns [format "%.1f" $ns]
        set xth(ctrl,me,point,ys) $ns
      }
    }
  }
  xth_me_cmds_move_point_fill_xctrl $id $xth(ctrl,me,point,rot) $xth(ctrl,me,point,xs) $xth(ctrl,me,point,ys)
}

proc xth_me_cmds_end_point_fdrag {tagOrId id x y} {
  global xth
  xth_me_cmds_point_fdrag $id $x $y
  $xth(me,can) configure -cursor crosshair
  if {$xth(me,point,fdrag_size)} {
    set xth(me,unredola) "point resizing"
  } else {
    set xth(me,unredola) "point rotation"
  }
  $xth(me,can) bind $tagOrId <B1-Motion> ""
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> ""
  if {[lsearch [$xth(me,can) itemcget $tagOrId -tags] current] > -1} {
    $xth(me,can) itemconfigure $tagOrId -fill #ffda00
  }
  $xth(me,can) bind $tagOrId <Enter> $xth(me,point,fdrag_benter)
  $xth(me,can) bind $tagOrId <Leave> $xth(me,point,fdrag_bleave)
  xth_me_cmds_update {}
}

proc xth_me_cmds_drag_point_config_xctrl {id} {
  global xth
  set xth(me,point,drag_stsel) [$xth(me,can) itemcget $xth(me,canid,point,selector) -state]
  $xth(me,can) itemconfigure $xth(me,canid,point,selector) -state hidden
  set xth(me,point,drag_stfx) [$xth(me,can) itemcget $xth(me,canid,point,fx) -state]
  $xth(me,can) itemconfigure $xth(me,canid,point,fx) -width 1 -arrow none
  #$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -state hidden
  set xth(me,point,drag_stfy) [$xth(me,can) itemcget $xth(me,canid,point,fy) -state]
  $xth(me,can) itemconfigure $xth(me,canid,point,fy) -width 1 -arrow none
  #$xth(me,can) itemconfigure $xth(me,canid,point,fyc) -state hidden
  $xth(me,can) itemconfigure $xth(me,canid,point,fill) -fill {} -outline red
}


proc xth_me_cmds_end_drag_point_config_xctrl {id} {
  global xth
  $xth(me,can) itemconfigure $xth(me,canid,point,selector) -state $xth(me,point,drag_stsel)
  $xth(me,can) itemconfigure $xth(me,canid,point,fx) -width 5 -arrow last
  #$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -state $xth(me,point,drag_stfx)
  $xth(me,can) itemconfigure $xth(me,canid,point,fy) -width 5 -arrow last
  #$xth(me,can) itemconfigure $xth(me,canid,point,fyc) -state $xth(me,point,drag_stfy)
  $xth(me,can) itemconfigure $xth(me,canid,point,fill) -fill red -outline {}
}

proc xth_me_cmds_select_nopoint {} {
  global xth
  set id $xth(me,cmds,selid)
  set ix [lsearch $xth(me,cmds,xlist) $id]
  set xl [llength $xth(me,cmds,xlist)]
  for {set ii $ix} {$ii < $xl} {incr ii} {
    set nid [lindex $xth(me,cmds,xlist) $ii]
    switch $xth(me,cmds,$nid,ct) {
      3 {
        xth_me_cmds_select "$nid 0"
        break
      }
      2 {}
      default {
        xth_me_cmds_select $nid
        break
      }
    }
  }
}
