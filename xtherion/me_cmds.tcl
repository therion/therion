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

proc xth_me_set_theme {x} {
  global xth
  set xth(me,acttheme) $x
  set at [lindex $xth(me,themes,list) $x]
  $xth(ctrl,me,point).typ configure -values $xth(me,themes,$at,point,list)
  $xth(ctrl,me,line).typ configure -values $xth(me,themes,$at,line,list)
  $xth(ctrl,me,ac).typ configure -values $xth(me,themes,$at,area,list)
  $xth(ctrl,me,point).themetype configure -values $xth(me,themes,$at,point,showlist)
  $xth(ctrl,me,line).themetype configure -values $xth(me,themes,$at,line,showlist)
  $xth(ctrl,me,ac).themetype configure -values $xth(me,themes,$at,area,showlist)
  if {[info exists xth(me,cmds,selid)]} {
    xth_me_cmds_update {}
  }
}


proc xth_me_point_themetype_modified {} {
  global xth
  set tx [$xth(ctrl,me,point).themetype getvalue]
  set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
  if {$tx > -1} {
    set xth(ctrl,me,point,type) [lindex $xth(me,themes,$at,point,hidelist) $tx]
    xth_me_cmds_update {}
  }
}

proc xth_me_line_themetype_modified {} {
  global xth
  set tx [$xth(ctrl,me,line).themetype getvalue]
  set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
  if {$tx > -1} {
    set xth(ctrl,me,line,type) [lindex $xth(me,themes,$at,line,hidelist) $tx]
    xth_me_cmds_update {}
  }
}

proc xth_me_area_themetype_modified {} {
  global xth
  set tx [$xth(ctrl,me,ac).themetype getvalue]
  set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
  if {$tx > -1} {
    set xth(ctrl,me,ac,type) [lindex $xth(me,themes,$at,area,hidelist) $tx]
    xth_me_cmds_update {}
  }
}

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


proc xth_me_cmds_get_sketch_option {ln} {
  set rxl [list [list "\\s*\\-sketch\\s+\\\[((\[^\\\]\]*))\\\]\\s+(\\S+)\\s+(\\S+)" "\[" "\]"]\
    [list "\\s*\\-sketch\\s+\\\"((\\\"\\\"|\[^\\\"])+)\\\"\\s+(\\S+)\\s+(\\S+)" "\"" "\""]\
    [list "\\s*\\-sketch\\s+((\\S+))\\s+(\\S+)\\s+(\\S+)" {} {}]]
  set rln $ln
  set val {}
  set sep {}
  set res 0
  foreach rx $rxl {
    if {[regexp [lindex $rx 0] $ln dump v1 dum v2 v3]} {
      regsub [lindex $rx 0] $ln {} rln
      set val "[lindex $rx 1]$v1[lindex $rx 2] $v2 $v3"
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
      $xth(ctrl,me,cmds).cc.go configure -text [mc "Insert line"]
    }
    1 {
      $xth(ctrl,me,cmds).cc.go configure -text [mc "Insert point"]
    }
    2 {
      $xth(ctrl,me,cmds).cc.go configure -text [mc "Insert scrap"]
    }
    3 {
      $xth(ctrl,me,cmds).cc.go configure -text [mc "Insert text"]
    }
    4 {
      $xth(ctrl,me,cmds).cc.go configure -text [mc "Delete"]
    }
    5 {
      $xth(ctrl,me,cmds).cc.go configure -text [mc "Insert area"]
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
      $xth(me,menu,edit) entryconfigure [mc "Select"] -state normal
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
	$ccbox.shf configure -state normal
	$ccbox.sht configure -state normal
        $ccbox.shfx configure -state normal
        $ccbox.shfy configure -state normal
        $ccbox.shtx configure -state normal
        $ccbox.shty configure -state normal
        $ccbox.shswap configure -state normal
        $ccbox.shift configure -state normal
      } else {
	$ccbox.mu configure -state disabled
	$ccbox.md configure -state disabled
	$ccbox.mt configure -state disabled
	$ccbox.tt configure -state disabled
	$ccbox.shf configure -state disabled
	$ccbox.sht configure -state disabled
	$ccbox.shfx configure -state disabled
	$ccbox.shfy configure -state disabled
	$ccbox.shtx configure -state disabled
	$ccbox.shty configure -state disabled
	$ccbox.shswap configure -state disabled
	$ccbox.shift configure -state disabled
      }
      $ccbox.cfg.m entryconfigure [mc "Delete"] -state normal
      $xth(me,menu,edit) entryconfigure [mc "Delete"] -state normal
    } else {
      $xth(ctrl,me,prev).upd  configure -state disabled
      $ccbox.sel configure -state disabled
      $xth(me,menu,edit) entryconfigure [mc "Select"] -state disabled
      $ccbox.cfg.m entryconfigure [mc "Delete"] -state disabled
      $xth(me,menu,edit) entryconfigure [mc "Delete"] -state disabled
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
  set xth(me,cmds,$id,sbar) {}
  set ix [lsearch $xth(me,cmds,xlist) $xth(me,cmds,selid)]

	# ak sme v normalnom mode a na scrape a chceme vlozit
	# point, line, area alebo scrap
  if {$xth(me,unredook)} {
		set ccmdid $xth(me,cmds,selid)
		if {($xth(me,cmds,$ccmdid,ct) == 4) && (($typ == 2) || ($typ == 3) || ($typ == 6))} {
		    incr ix;
		}
	}
	
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
    xth_me_unredo_action [mc "deleting"] \
      "xth_me_cmds_undelete $id $oldselpid $ix" \
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
    set pid -1
  } elseif {[llength $id] < 2} {
    set id [lindex $id 0]
    set pid -1
  } else {
    set pid [lindex $id 1]
    set id [lindex $id 0]
  }  

  if {$pid == -1} {
    set pid 0
    if {($xth(me,cmds,$id,ct) == 3) && ([llength $xth(me,cmds,$id,xplist)] > 1)} {
      set center_to [lindex $xth(me,cmds,$id,xplist) [expr [llength $xth(me,cmds,$id,xplist)] - 2]]
    }
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
  
  if {$center_to > 0} {
    switch $xth(me,cmds,$id,ct) {
      2 { xth_me_center_to [list $xth(me,cmds,$id,x) $xth(me,cmds,$id,y)]
	}
      3 { xth_me_center_to [list \
	  $xth(me,cmds,$xth(me,cmds,selid),$center_to,x) \
	  $xth(me,cmds,$xth(me,cmds,selid),$center_to,y)]
	}
    }
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
  xth_me_unredo_action [mc "moving command"] \
    "xth_me_cmds_move_to $id $sx\nxth_me_cmds_select $id" \
    "xth_me_cmds_move_to $id $dx\nxth_me_cmds_select $nid"
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
  xth_me_unredo_action [mc "moving command"] \
    "xth_me_cmds_move_down $id$selcmd" \
    "xth_me_cmds_move_up $id$selcmd"
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
  xth_me_unredo_action [mc "moving command"] \
   "xth_me_cmds_move_up $id$selcmd" "xth_me_cmds_move_down $id$selcmd"
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
    xth_me_cmds_select [expr $id - 1]
    xth_me_unredo_action [mc "creating endscrap"] \
     "xth_me_cmds_delete $id" \
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

  $xth(me,can) itemconfigure $xth(me,canid,scrap,scpA) -state hidden
  
}


proc xth_me_cmds_move_scrap_xctrl {id x y} {
  global xth
  set xth(me,canid,scrap,scp$id,pos) [list $x $y]
  set x [xth_me_real2canx $x]
  set y [xth_me_real2cany $y]
  $xth(me,can) coords $xth(me,canid,scrap,scp$id) \
    [expr $x - $xth(gui,me,scrap,psize)] [expr $y - $xth(gui,me,scrap,psize)] \
    [expr $x + $xth(gui,me,scrap,psize)] [expr $y + $xth(gui,me,scrap,psize)]
  set x1 [xth_me_real2canx [lindex $xth(me,canid,scrap,scp1,pos) 0]]
  set y1 [xth_me_real2cany [lindex $xth(me,canid,scrap,scp1,pos) 1]]
  set x2 [xth_me_real2canx [lindex $xth(me,canid,scrap,scp2,pos) 0]]
  set y2 [xth_me_real2cany [lindex $xth(me,canid,scrap,scp2,pos) 1]]
  $xth(me,can) coords $xth(me,canid,scrap,scpA) $x1 $y1 $x2 $y2
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

  $xth(me,can) itemconfigure $xth(me,canid,scrap,scpA) -state normal
  
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
    $xth(ctrl,me,scrap).skl configure -state normal
    $xth(ctrl,me,scrap).skll.l configure -takefocus 1 \
      -listvariable xth(me,cmds,$id,sklist)
    $xth(ctrl,me,scrap).skins configure -state normal
    $xth(ctrl,me,scrap).skdel configure -state normal
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
    $xth(ctrl,me,scrap).skl configure -state disabled
    $xth(ctrl,me,scrap).skll.l configure -takefocus 0 \
      -listvariable xth(ctrl,me,scrap,sklempty)
    $xth(ctrl,me,scrap).skins configure -state disabled
    $xth(ctrl,me,scrap).skdel configure -state disabled
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
    xth_me_unredo_action [mc "scrap changes"] \
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
  
  foreach sk $xth(me,cmds,$id,sklist) {
    set d "$d -sketch $sk"
  }

  set xth(me,cmds,$id,data) "$d"
  
}


proc xth_me_cmds_create_scrap {ix mode name opts} {

  global xth
  xth_me_cmds_update {}

  set undoselect {}
  if {[string length $ix] == 0} {
    set undoselect "; xth_me_cmds_select $xth(me,cmds,selid)"

    #xth_me_cmds_select [lindex $xth(me,cmds,xlist) 0]
    set newselid 0
    set cx [lsearch -exact $xth(me,cmds,xlist) $xth(me,cmds,selid)]
    if {$cx > -1} {
      for {set cc [expr $cx + 1]} {$cc < [llength $xth(me,cmds,xlist)]} {incr cc} {
	set cselid [lindex $xth(me,cmds,xlist) $cc]
	if {$xth(me,cmds,$cselid,ct) == 4} {
	  set newselid $cselid
	  break
	}
      }
    }
    xth_me_cmds_select $newselid
    
  }
  
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

  # set the list of sketches    
  set xth(me,cmds,$id,sklist) {}
  set optl [xth_me_cmds_get_sketch_option $opts]
  while {[lindex $optl 2]} {
    lappend xth(me,cmds,$id,sklist) [lindex $optl 0]
    set opts [lindex $optl 1]
    set optl [xth_me_cmds_get_sketch_option $opts]
  }
  
  # nastavit options
  regsub {^\s*} $opts "" opts
  regsub {\s*$} $opts "" opts
  set xth(me,cmds,$id,options) $opts
  
  xth_me_cmds_update_list $id
  xth_me_cmds_update_scrap_data $id
  if {$mode} {
    xth_me_unredo_action [mc "creating scrap"] \
      "xth_me_cmds_delete $id$undoselect" \
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
    xth_me_unredo_action [mc "creating text"] \
      "xth_me_cmds_delete $id" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]"
  }
}


proc xth_me_cmds_update_text {id newdata newcpos} {
  global xth
  set olddata $xth(me,cmds,$id,data)
  set oldcpos $xth(me,cmds,$id,cpos)
  regsub {\s*$} $newdata {} newdata
  if {![string equal $xth(me,cmds,$id,data) $newdata]} {
    set newdata "$newdata\n"
    xth_me_unredo_action [mc "text changes"] \
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
      xth_ctrl_maximize me line
      xth_ctrl_maximize me linept
    }
    1 {
      xth_me_cmds_set_mode 1    
    }
    2 {
      xth_me_cmds_create_scrap {} 1 "" ""
      xth_ctrl_scroll_to me scrap
      xth_ctrl_maximize me scrap
    }
    3 {
      xth_me_cmds_create_text {} 1 "\n" "1.0"
      xth_ctrl_scroll_to me text
      xth_ctrl_maximize me text
      focus $xth(ctrl,me,text).txt
    }
    4 {
      xth_me_cmds_delete {}
    }
    5 {
      xth_me_cmds_create_area {} 1 "" "" ""
      xth_ctrl_scroll_to me ac
      xth_ctrl_maximize me ac
    }
  }
}


proc xth_me_cmds_create_all {lns} {
  global xth
  set ctext {}
  xth_status_bar_push me
  set ctext_push {
    regsub {^\s*} $ctext {} ctext
    regsub {\s*$} $ctext {} ctext
    if {[string length $ctext] > 0} {
      xth_me_cmds_create_text [expr [llength $xth(me,cmds,xlist)] - 1] 0 "$ctext\n" 1.0
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
	    xth_ctrl_maximize me point
	  } else {
	    xth_ctrl_scroll_to me line
	    xth_ctrl_maximize me line
	    xth_ctrl_maximize me linept
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
      xth_ctrl_maximize me point
      # check, whether we clicked on XVI station
      set cptype {}
      set cpopts {}
      if {[regexp {XVIstI(\d+)S(\d+)} [$xth(me,can) itemcget $tagOrId -tags] dum imgx sti]} {
	set csname [lindex [lindex $xth(me,imgs,$imgx,XVIstations) $sti] 2]
	set x [lindex [lindex $xth(me,imgs,$imgx,XVIstationsX) $sti] 0]
	set y [lindex [lindex $xth(me,imgs,$imgx,XVIstationsX) $sti] 1]
	set cptype station
	set cpopts $xth(me,dflt,point,options)
	if {[regexp {\-name\s+(\S+)} $cpopts]} {
	  regsub {\-name\s+(\S+)} $cpopts "-name $csname" cpopts
	} else {
	  set cpopts "-name $csname"
	}
      }
      if {$id == ($xth(me,cmds,cmdln) - 1)} {
	xth_me_cmds_end_point 
      } else {
	xth_me_cmds_create_point {} 1 $x $y $cptype $cpopts
      }
    }
    2 {
      xth_ctrl_scroll_to me line
      xth_ctrl_maximize me line
      xth_ctrl_maximize me linept
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
    # ak sme klikli na XVIbod, tak hodime presne tam
      if {[regexp {XVIstI(\d+)S(\d+)} [$xth(me,can) itemcget $tagOrId -tags] dum imgx sti]} {
        set x [lindex [lindex $xth(me,imgs,$imgx,XVIstationsX) $sti] 0]
        set y [lindex [lindex $xth(me,imgs,$imgx,XVIstationsX) $sti] 1]
      }    
	xth_me_cmds_start_create_linept $tagOrId $x $y $mx $my
      }      
    }
    3 {
      if {([string length $id] > 0) && ($xth(me,cmds,$id,ct) == 3)} {
	xth_me_cmds_insert_area_lineid $id $mx $my
      }
    }
    4 {
      xth_me_cmds_scrap_scale $x $y
    }
    5 {
      xth_me_cmds_scrap_sketch_insert $tagOrId
    }
  }
}


proc xth_me_cmds_end_point {} {
  set recmds "xth_me_cmds_set_mode 0"
  set uncmds "xth_me_cmds_set_mode 1"
  eval $recmds
  xth_me_unredo_action [mc "end point insertion"] $uncmds $recmds
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
    4 {
      xth_me_cmds_scrap_scale
    }
    0 {
      xth_me_cmds_select "$id $pid"
    }
    default {
      xth_me_cmds_click_area ln$id.$pid $mx $my
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
      $xth(me,mbar) configure -text [mc "select object"] -bg green -fg black
      $xth(ctrl,me,ac).ins configure -text [mc "Insert"]
    }
    1 {
      $xth(me,mbar) configure -text [mc "insert point"] -bg red -fg white
    }
    2 {
      $xth(me,mbar) configure -text [mc "insert line point"] -bg red -fg white
    }
    3 {
      $xth(me,mbar) configure -text [mc "insert area border"] -bg red -fg white
      $xth(ctrl,me,ac).ins configure -text [mc "Select"]
    }
    4 {
      $xth(me,mbar) configure -text [mc "scale scrap"] -bg red -fg white
    }
    5 {
      $xth(me,mbar) configure -text [mc "insert sketch"] -bg red -fg white
    }
  }
  
  set xth(tb,me,action) $nmode
  
}


proc xth_me_cmds_create_point {ix mode x y type opts} {

  global xth
  xth_me_cmds_update {}
  set id [xth_me_cmds_create 2 {} $ix]

  set xth(me,cmds,$id,x) $x
  set xth(me,cmds,$id,y) $y
  
  if {$mode && ([string length $opts] < 1)} {
    if {([string length $type] < 1) && \
	[string equal $xth(me,dflt,point,type) station] && \
	[regexp {\-name\s+(\S+)} $xth(me,dflt,point,options) dum stname]} {
      regsub {\-name\s+(\S+)} $xth(me,dflt,point,options) "-name [xth_incr_station_name $stname $xth(me,snai)]" xth(me,dflt,point,options)
    }
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

#  # nastavit xsize
#  if {$mode} {
#    set xth(me,cmds,$id,xsize) $xth(me,dflt,point,xsize)
#  } else {
    set xth(me,cmds,$id,xsize) {}
#  }
#  set optl [xth_me_cmds_get_line_option $opts size]
#  if {[lindex $optl 2]} {
#    set xth(me,cmds,$id,xsize) [lindex $optl 0]
#    set xth(me,cmds,$id,ysize) [lindex $optl 0]
#    set opts [lindex $optl 1]
#  }
#  set optl [xth_me_cmds_get_line_option $opts "x-size"]
#  if {[lindex $optl 2]} {
#    set xth(me,cmds,$id,xsize) [lindex $optl 0]
#    set opts [lindex $optl 1]
#  }
#
#  # nastavit ysize
#  if {$mode} {
#    set xth(me,cmds,$id,ysize) $xth(me,dflt,point,ysize)
#  } else {
    set xth(me,cmds,$id,ysize) {}
#  }
#  set optl [xth_me_cmds_get_line_option $opts "y-size"]
#  if {[lindex $optl 2]} {
#    set xth(me,cmds,$id,ysize) [lindex $optl 0]
#    set opts [lindex $optl 1]
#  }
#  
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
    xth_me_unredo_action [mc "creating point"] \
      "xth_me_cmds_delete $id" \
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

  catch {
  set ttype [lindex $d 3]
  set xth(me,cmds,$id,sbar) "$d"
  if {([string length $ttype] > 0)} {
    set ttyp [ mc "point $ttype"]
    if {![string equal $ttype $ttyp]} {
      set nd [lreplace $d 3 3 "$ttype:$ttyp"]
      set xth(me,cmds,$id,sbar) "$nd"
    }
  } 
  }

  set xth(me,cmds,$id,data) "$d"
  
}


proc xth_me_cmds_move_point_xctrl {id} {
  global xth
  set cx [xth_me_real2canx $xth(me,cmds,$id,x)]
  set cy [xth_me_real2cany $xth(me,cmds,$id,y)]
  set c1 [expr $cx - 3 * $xth(gui,me,point,psize)]
  set c2  [expr $cy - 3 * $xth(gui,me,point,psize)]
  set c3  [expr $cx + 3 * $xth(gui,me,point,psize)]
  set c4  [expr $cy + 3 * $xth(gui,me,point,psize)]
  $xth(me,can) coords $xth(me,canid,point,selector) [list $c1 $c2 $c3 $c4]
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
    $xth(ctrl,me,point).theme configure -state normal
    $xth(ctrl,me,point).themetype configure -state normal
    
    set xth(ctrl,me,point,x) $xth(me,cmds,$id,x)
    set xth(ctrl,me,point,y) $xth(me,cmds,$id,y)
    set xth(ctrl,me,point,type) $xth(me,cmds,$id,type)
    set xth(ctrl,me,point,name) $xth(me,cmds,$id,name)
    set xth(ctrl,me,point,opts) $xth(me,cmds,$id,options)
    set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
    set tx [lsearch -exact $xth(me,themes,$at,point,hidelist) $xth(me,cmds,$id,type)]
    if {$tx < 0} {
      set xth(ctrl,me,point,themetype) {}
    } else {
      set xth(ctrl,me,point,themetype) [lindex $xth(me,themes,$at,point,showlist) $tx]
    }
    
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
    $xth(ctrl,me,point).theme configure -state disabled
    $xth(ctrl,me,point).themetype configure -state disabled
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
  set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
  set tx [lsearch -exact $xth(me,themes,$at,point,hidelist) $xth(me,cmds,$id,type)]
  if {$tx < 0} {
    set xth(ctrl,me,point,themetype) {}
  } else {
    set xth(ctrl,me,point,themetype) [lindex $xth(me,themes,$at,point,showlist) $tx]
  }
    
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
  
  if {$xth(gui,me,type_reset_options) && (![string equal $ntype $otype]) && [string equal $nopt $oopt]} {
    set nopt {}
  }
  
  if {[regexp {^([^ :]+)\:([^ :]+)$} $ntype dum xntype xstype]} {
    if {![string equal $xntype u]} {
      set ntype $xntype
      set nopt [regsub -all {(^|\s+)\-subtype\s+\S+} $nopt {}]
      set nopt [regsub -all {^\s+|\s+$} $nopt {}]
      set nopt "-subtype $xstype $nopt"
      set nopt [regsub -all {^\s+|\s+$} $nopt {}]
    }
  }    
  
  if {![string equal "$ntype $nopt" "$otype $oopt"]} {
    xth_me_unredo_action [mc "area changes"] \
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
  
  #reset point options & rotation when type changes
  if {$xth(gui,me,type_reset_options) && (![string equal $ntype $otype]) && [string equal $nopt $oopt] && [string equal $nrot $orot] && [string equal $nxs $oxs] && [string equal $nys $oys]} {
    set nopt {}
    set nrot {}
    set nxs {}
    set nys {}
  }
  
  if {[regexp {^([^ :]+)\:([^ :]+)$} $ntype dum xntype xstype]} {
    if {![string equal $xntype u]} {
      set ntype $xntype
      set nopt [regsub -all {(^|\s+)\-subtype\s+\S+} $nopt {}]
      set nopt [regsub -all {^\s+|\s+$} $nopt {}]
      set nopt "-subtype $xstype $nopt"
      set nopt [regsub -all {^\s+|\s+$} $nopt {}]
    }
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
    xth_me_unredo_action [mc "point changes"] \
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
    
    # VG 230216: Erase and recreate the point when the type changes. The new triangular stations use 6 coordinates for a polygon, the circles use 4
    # so they can not be interchanged. Maybe there is an easier way to just update the coordinates list instead of recreating the point
    if {![string equal $ntype $otype]} {
      xth_me_cmds_erase $id
      xth_me_cmds_draw_point $id
      xth_me_cmds_set_colors
    }
    
    $xth(me,can) coords pt$id [xth_me_cmds_calc_point_coords $id]
    xth_me_cmds_update_point_data $id
    xth_me_cmds_update_list $id
  }
  
}


proc xth_me_cmds_calc_point_coords {id} {
  global xth
  switch $xth(me,cmds,$id,type) {
    station {
      # draw a triangle for the station points
      set x1 [expr [xth_me_real2canx $xth(me,cmds,$id,x)] - $xth(gui,me,point,psize)]
      set y1 [expr [xth_me_real2cany $xth(me,cmds,$id,y)] + $xth(gui,me,point,psize)]
      set x2 [expr [xth_me_real2canx $xth(me,cmds,$id,x)] + $xth(gui,me,point,psize)]
      set y2 [expr [xth_me_real2cany $xth(me,cmds,$id,y)] + $xth(gui,me,point,psize)]
      set x3 [expr [xth_me_real2canx $xth(me,cmds,$id,x)]]
      set y3 [expr [xth_me_real2cany $xth(me,cmds,$id,y)] - $xth(gui,me,point,psize)/2]
      return [list $x1 $y1 $x2 $y2 $x3 $y3]
      }
    default {
      set x1 [expr [xth_me_real2canx $xth(me,cmds,$id,x)] - $xth(gui,me,point,psize)]
      set y1 [expr [xth_me_real2cany $xth(me,cmds,$id,y)] - $xth(gui,me,point,psize)]
      set x2 [expr [xth_me_real2canx $xth(me,cmds,$id,x)] + $xth(gui,me,point,psize)]
      set y2 [expr [xth_me_real2cany $xth(me,cmds,$id,y)] + $xth(gui,me,point,psize)]
      return [list $x1 $y1 $x2 $y2]
    }
  }  
}


proc xth_me_cmds_draw_point {id} {
  global xth
  set coords [xth_me_cmds_calc_point_coords $id]
  switch $xth(me,cmds,$id,type) {
    station {
      # draw a triangle for the station points
      $xth(me,can) create polygon [xth_me_cmds_calc_point_coords $id] \
        -tags "command point pt$id" -width 1 -outline blue -fill blue
    }
    default {
      $xth(me,can) create oval $coords \
        -tags "command point pt$id" -width 1 -outline blue -fill blue
    }
  }  
  $xth(me,can) bind pt$id <Enter> "$xth(me,can) itemconfigure pt$id -fill cyan; xth_status_bar_push me; xth_status_bar_status me \"\$xth(me,cmds,$id,listix): \$xth(me,cmds,$id,sbar)\""
  $xth(me,can) bind pt$id <Leave> "$xth(me,can) itemconfigure pt$id -fill \[$xth(me,can) itemcget pt$id -outline\]; xth_status_bar_pop me"
  $xth(me,can) bind pt$id <1> "xth_me_cmds_click $id pt$id \$xth(me,cmds,$id,x) \$xth(me,cmds,$id,y) %x %y"
  $xth(me,can) bind pt$id <$xth(gui,rmb)> "xth_me_show_context_menu $id %x %y"  
  $xth(me,can) bind pt$id <Shift-1> "xth_me_cmds_special_select $id %x %y"  
  $xth(me,can) bind pt$id <$xth(kb_control)-1> "xth_me_cmds_click_area pt$id %x %y"
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
      xth_ctrl_maximize me line
      xth_ctrl_maximize me linept
    } else {
      xth_ctrl_scroll_to me point
      xth_ctrl_maximize me point

    }
  } elseif {($xth(me,cmds,$id,ct) == 3) && ($xth(me,cmds,selpid) != $pid)} {
    xth_me_cmds_select_linept $id $pid
    xth_ctrl_scroll_to me line
    xth_ctrl_maximize me line
    xth_ctrl_maximize me linept
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
      xth_ctrl_maximize me point
      catch {$xth(me,can) lower $utag point}
      catch {$xth(me,can) raise $utag line}
    } elseif {[regexp "(^|\\s)pt(\\d+)\.(\\d+)($|\\s)" $tgs d1 d2 nid npid]} {
      #puts "select $nid $npid"
      xth_me_cmds_select "$nid $npid"
      if {$npid != 0} {
	xth_ctrl_scroll_to me line
	xth_ctrl_maximize me line
	xth_ctrl_maximize me linept
      } else {
	xth_ctrl_scroll_to me point
	xth_ctrl_maximize me point
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
  $xth(me,can) bind pt$id <$xth(kb_control)-B1-Motion> "xth_me_cmds_point_drag $id %x %y 0"
  $xth(me,can) bind pt$id <$xth(kb_control)-B1-ButtonRelease> "xth_me_cmds_end_point_drag $id %x %y 0"
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
  } elseif {[regexp {(^|\s)XVIstI(\d+)S(\d+)} $tgs d1 d2 imgx sti]} {
    set nx [lindex [lindex $xth(me,imgs,$imgx,XVIstationsX) $sti] 0]
    set ny [lindex [lindex $xth(me,imgs,$imgx,XVIstationsX) $sti] 1]
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
  $xth(me,can) bind pt$id <$xth(kb_control)-B1-Motion> ""
  $xth(me,can) bind pt$id <$xth(kb_control)-B1-ButtonRelease> ""
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
    if $xth(gui,me,pointsizectrl) {
        $xth(me,can) itemconfigure $xth(me,canid,point,fx) -state normal
    }
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
  $xth(me,can) coords $xth(me,canid,point,fy) $x $y [expr $x + $yvx] [expr $y + $yvy]
  set xth(me,canid,point,fy_tox) [expr $x + $yvx]
  set xth(me,canid,point,fy_toy) [expr $y + $yvy]
  #$xth(me,can) coords $xth(me,canid,point,fxc) [expr $x + $xvx - $xth(gui,me,point,cpsize)] [expr $y + $xvy - $xth(gui,me,point,cpsize)] [expr $x + $xvx + $xth(gui,me,point,cpsize)] [expr $y + $xvy + $xth(gui,me,point,cpsize)]
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
  set xth(me,point,drag_stfy) [$xth(me,can) itemcget $xth(me,canid,point,fy) -state]
  $xth(me,can) itemconfigure $xth(me,canid,point,fy) -width 1 -arrow none
  #$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -state hidden
  #$xth(me,can) itemconfigure $xth(me,canid,point,fyc) -state hidden
  $xth(me,can) itemconfigure $xth(me,canid,point,fill) -fill {} -outline red
}


proc xth_me_cmds_end_drag_point_config_xctrl {id} {
  global xth
  $xth(me,can) itemconfigure $xth(me,canid,point,selector) -state $xth(me,point,drag_stsel)
  $xth(me,can) itemconfigure $xth(me,canid,point,fx) -width 5 -arrow last
  $xth(me,can) itemconfigure $xth(me,canid,point,fy) -width 5 -arrow last
  #$xth(me,can) itemconfigure $xth(me,canid,point,fxc) -state $xth(me,point,drag_stfx)
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


proc xth_me_get_center {} {
  global xth
  set sr [$xth(me,can) cget -scrollregion]
  set xw [$xth(me,can) xview]
  set yw [$xth(me,can) yview]
  set xx [expr double([lindex $xw 1] + [lindex $xw 0]) / 2.0]
  set yy [expr double([lindex $yw 1] + [lindex $yw 0]) / 2.0]
  set tw [expr [lindex $sr 2] - [lindex $sr 0]]
  set th [expr [lindex $sr 3] - [lindex $sr 1]]        
  set x [xth_me_can2realx [expr [lindex $sr 0] + $xx * $tw]]
  set y [xth_me_can2realy [expr [lindex $sr 1] + $yy * $th]]
	return "$x $y"
}

proc xth_me_cmds_scrap_scale_start {} {
  global xth
  xth_me_cmds_update {}
  set xth(me,cmds,scrap_scale) 1
  xth_me_cmds_set_mode 4
}


proc xth_me_cmds_scrap_sketch_start {} {
  global xth
  xth_me_cmds_update {}
  xth_me_cmds_set_mode 5
}


proc xth_me_cmds_scrap_scale {x y} {
  global xth
  xth_me_cmds_update {}
  set rx $x
  set ry $y
  switch $xth(me,cmds,scrap_scale) {
    1 {
      set xth(ctrl,me,scrap,px1) $rx
      set xth(ctrl,me,scrap,py1) $ry
      incr xth(me,cmds,scrap_scale)
      xth_me_cmds_update {}
      focus $xth(ctrl,me,scrap).scx1r
    }
    2 {
      set xth(ctrl,me,scrap,px2) $rx
      set xth(ctrl,me,scrap,py2) $ry
      set xth(me,cmds,scrap_scale) 0
      xth_me_cmds_update {}
      focus $xth(ctrl,me,scrap).scx2r
      xth_me_cmds_set_mode 0
    }
  }
}


proc xth_me_cmds_scrap_sketch_insert {tagOrId} {
  global xth
  xth_me_cmds_set_mode 0 
  if {[string length $tagOrId] == 0} return
  xth_me_cmds_update {}
  set err 1
  set tags [$xth(me,can) itemcget $tagOrId -tags]
  foreach ix $xth(me,imgs,xlist) {
    if {[lsearch -exact $tags $xth(me,imgs,$ix,image)] > -1} {
      if {($xth(me,imgs,$ix,XVI) == 0) && ([string length $xth(me,imgs,$ix,ffname)] > 0)} {
	  set sk $xth(me,imgs,$ix,name)
	  if {[string first " " $sk] > -1} {
	    set sk "\[$sk\]"
	  }
	  set skpos $xth(me,imgs,$ix,position)
	  set imgh [image height $xth(me,imgs,$ix,image)]
	  set sk "$sk [lindex $skpos 0] [expr [lindex $skpos 1] - $imgh]"
	  xth_me_cmds_insert_scrap_sketch $sk
	  set err 0
      }
    }
  }
  if {$err} {
    MessageDlg $xth(gui,message) -parent $xth(gui,main) \
      -icon error -type ok \
      -message [mc "Scrap sketch not inserted."] \
      -font $xth(gui,lfont)
  }
}


proc xth_me_cmds_insert_scrap_sketch {sk} {

  global xth
  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  if {[string length $sk] == 0} {
    return;
  }
  set id $xth(me,cmds,selid)
  set lid [lindex $xth(me,cmds,$id,sklist) [$xth(ctrl,me,scrap).skll.l curselection]]
  set ix [lsearch -exact $xth(me,cmds,$id,sklist) $lid]
  set xth(me,cmds,$id,sklist) [linsert $xth(me,cmds,$id,sklist) $ix $sk]
  xth_me_cmds_update_scrap_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  $xth(ctrl,me,scrap).skll.l selection clear 0 end
  $xth(ctrl,me,scrap).skll.l selection set [expr $ix + 1] [expr $ix + 1]
  $xth(ctrl,me,scrap).skll.l see [expr $ix + 1]

  xth_me_unredo_action [mc "inserting scrap sketch"] \
    "xth_me_cmds_select $id\nxth_me_cmds_delete_scrap_sketch $id [list $sk]" \
    "xth_me_cmds_select $id\nxth_me_cmds_undelete_scrap_sketch 1 $id [list $sk] $ix"
      
}


proc xth_me_cmds_delete_scrap_sketch {id lid} {

  global xth
  
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {[string length $lid] == 0} {
    set lid [lindex $xth(me,cmds,$id,sklist) [$xth(ctrl,me,scrap).skll.l curselection]]
  }
  if {[string length $lid] == 0} {
    return
  }

  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  
  # odstrani ho zo zoznamu
  set ix [lsearch -exact $xth(me,cmds,$id,sklist) $lid]
  set xth(me,cmds,$id,sklist) [lreplace $xth(me,cmds,$id,sklist) $ix $ix]
  xth_me_cmds_update_scrap_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  $xth(ctrl,me,ac).ll.l selection clear 0 end
  $xth(ctrl,me,ac).ll.l selection set $ix $ix
  $xth(ctrl,me,ac).ll.l see $ix
    
  xth_me_unredo_action [mc "deleting scrap sketch"] \
    "xth_me_cmds_select $id\nxth_me_cmds_undelete_scrap_sketch 0 $id [list $lid] $ix" \
    "xth_me_cmds_select $id\nxth_me_cmds_delete_scrap_sketch $id [list $lid]"
  
}




proc xth_me_cmds_undelete_scrap_sketch {cr id lid ix} {
  global xth
  set xth(me,cmds,$id,sklist) [linsert $xth(me,cmds,$id,sklist) $ix $lid]
  xth_me_cmds_update_scrap_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  $xth(ctrl,me,ac).ll.l selection clear 0 end
  $xth(ctrl,me,ac).ll.l selection set [expr $ix + $cr] [expr $ix + $cr]
  $xth(ctrl,me,ac).ll.l see [expr $ix + $cr]
  
}

proc xth_me_cmds_toggleishiding {} {
	global xth
  xth_me_cmds_set_colors
}

set xth(me,ctxmenu) $xth(gui,main).mectx
catch {menu $xth(me,ctxmenu) -tearoff 0}
catch {menu $xth(me,ctxmenu).ptypes -tearoff 0}
catch {menu $xth(me,ctxmenu).ltypes -tearoff 0}

$xth(me,ctxmenu).ptypes delete 0 end
$xth(me,ctxmenu).ltypes delete 0 end

for {set j 0} {$j < [llength $xth(me,themes,list)]} {incr j} {
  set cm [lindex $xth(me,themes,list) $j]
  set cmn "$xth(me,ctxmenu).ptypes.m$j"
  catch {destroy $cmn}
  # vytvorime podmenu
  menu $cmn -tearoff 0
  for {set i 0} {$i < [llength $xth(me,themes,$cm,point,showlist)]} {incr i} {
    if {$xth(gui,me,type_dropdown_trans)} {
      set l [lindex $xth(me,themes,$cm,point,showlist) $i]
      set v [lindex $xth(me,themes,$cm,point,hidelist) $i]
    } else {
      set l [lindex $xth(me,themes,$cm,point,list) $i]
      set v [lindex $xth(me,themes,$cm,point,list) $i]      
    }
    $cmn add radiobutton -label $l -variable xth(ctrl,me,point,type) -value $v -command {xth_me_cmds_update {}}
  }    
  # pripneme ho do menu
  $xth(me,ctxmenu).ptypes add cascade -label [lindex $xth(me,themes,showlist) $j] -menu $cmn
  
  set cmn "$xth(me,ctxmenu).ltypes.m$j"
  catch {destroy $cmn}
  # vytvorime podmenu
  menu $cmn -tearoff 0
  for {set i 0} {$i < [llength $xth(me,themes,$cm,line,showlist)]} {incr i} {
    if {$xth(gui,me,type_dropdown_trans)} {
      set l [lindex $xth(me,themes,$cm,line,showlist) $i]
      set v [lindex $xth(me,themes,$cm,line,hidelist) $i]
    } else {
      set l [lindex $xth(me,themes,$cm,line,list) $i]
      set v [lindex $xth(me,themes,$cm,line,list) $i]      
    }
    $cmn add radiobutton -label $l -variable xth(ctrl,me,line,type) -value $v -command {xth_me_cmds_update {}}
  }    
  # pripneme ho do menu
  $xth(me,ctxmenu).ltypes add cascade -label [lindex $xth(me,themes,showlist) $j] -menu $cmn  
}


catch {menu $xth(me,ctxmenu).clip -tearoff 0}
$xth(me,ctxmenu).clip delete 0 end
$xth(me,ctxmenu).clip add radiobutton -label [mc "on"] -variable xth(me,ctrl,ctx,clip) -value "on" -command {xth_me_set_option_value clip}
$xth(me,ctxmenu).clip add radiobutton -label [mc "off"] -variable xth(me,ctrl,ctx,clip) -value "off" -command {xth_me_set_option_value clip}

catch {menu $xth(me,ctxmenu).visibility -tearoff 0}
$xth(me,ctxmenu).visibility delete 0 end
$xth(me,ctxmenu).visibility add radiobutton -label [mc "on"] -variable xth(me,ctrl,ctx,visibility) -value "on" -command {xth_me_set_option_value visibility}
$xth(me,ctxmenu).visibility add radiobutton -label [mc "off"] -variable xth(me,ctrl,ctx,visibility) -value "off" -command {xth_me_set_option_value visibility}

catch {menu $xth(me,ctxmenu).place -tearoff 0}
$xth(me,ctxmenu).place delete 0 end
$xth(me,ctxmenu).place add radiobutton -label [mc "top"] -variable xth(me,ctrl,ctx,place) -value "top" -command {xth_me_set_option_value place}
$xth(me,ctxmenu).place add radiobutton -label [mc "bottom"] -variable xth(me,ctrl,ctx,place) -value "bottom" -command {xth_me_set_option_value place}

image create photo align-b.gif -data {R0lGODlhFQASAKECAP8AALi4uP///////yH5BAEKAAMALAAAAAAVABIAAAI/nI+pOwAMjYtwUuRe
szCE7IFBJ5SCYwojE6SouS6t+ZaxMqfpneS6TfrBgkIVUchD+H7Jw1LXNHim1OnlaigAADs=}
image create photo align-bl.gif -data {R0lGODlhFQASAKEBAP8AALi4uP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI+nI+pmwAMjYtwUuTe
yCn472XBZh2BgKZq2amuwCLnm8YmnY7MjOsLT/MpgC9hCycwypBJCNGlNIGmnosVUQAAOw==}
image create photo align-bls.gif -data {R0lGODlhFQASAKEDAAAAAP8AAP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI+nI+pmxEMjYtwUuTe
yAn472XAZh2AgKZq2amuwCLnm8YmnY7MjOsLT/MpgC9hCycwypBJCNGlNIGmnosVUQAAOw==}
image create photo align-br.gif -data {R0lGODlhFQASAKEBAP8AALi4uP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI8nI+pCLDPHIRtqiax
NTrgAIbg0wjmiQYkgLaCupTuCS/BTEM3/up8/9jhagrhjJgwupAIZYt5EElDm2oBADs=}
image create photo align-brs.gif -data {R0lGODlhFQASAKEDAAAAAP8AAP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI+nI+pGLHPHIRtqiax
NRpgAIbg0wjmiQJkgLaCupTuCS/ATEM3/up8/9jhagrhjJgwupAIZYt5AEinVOnmWgAAOw==}
image create photo align-bs.gif -data {R0lGODlhFQASAKECAAAAAP8AAP///////yH5BAEKAAMALAAAAAAVABIAAAJAnI+pOxEMjYtwUuRe
sxCA7IFAJ5SCYwojA6SouS6t+ZaxMqfpneS6TfrBgkIVUchD+H7Jw1LXNACm1Or0gjUUAAA7}
image create photo align-c.gif -data {R0lGODlhFQASAKECAP8AALi4uP///////yH5BAEKAAMALAAAAAAVABIAAAI5nI+py+0fopyxBYGz
DlYDsHXZFzLXCGacKXypK6zLBbekrJwYqYp7isEldLbSTIOMWSjMh/MJjSIKADs=}
image create photo align-cs.gif -data {R0lGODlhFQASAKECAAAAAP8AAP///////yH5BAEKAAMALAAAAAAVABIAAAI5nI+py+0PopyxAYGz
BlaHsHXZFzLXCGacKXypK6zLBbekrJwYqYp7isEldLbSTIOMWSjMh/MJjSIKADs=}
image create photo align-l.gif -data {R0lGODlhFQASAKECAP8AALi4uP///////yH5BAEKAAMALAAAAAAVABIAAAI2nI+py+0dopwRiotz
sLkDwASdBy7iiH0h+gFbm5ykO8CIzJYKPqommtLFgJdNiIJ8KJfMpqIAADs=}
image create photo align-ls.gif -data {R0lGODlhFQASAKECAAAAAP8AAP///////yH5BAEKAAMALAAAAAAVABIAAAI2nI+py+0NopwRiosz
sLmHwACdBy7iiH0h+gVbm5ykO8CIzJYKPqommtLFgJdNiIJ8KJfMpqIAADs=}
image create photo align-r.gif -data {R0lGODlhFQASAKECAP8AALi4uP///////yH5BAEKAAMALAAAAAAVABIAAAI5nI+py+1vgpxUtiCy
3oEBsIVCt3wi54FnRh4fMATv2RpvPIs1Yq5j6vuVVKsdAuMzHipMJeQJjSoKADs=}
image create photo align-rs.gif -data {R0lGODlhFQASAKECAAAAAP8AAP///////yH5BAEKAAMALAAAAAAVABIAAAI5nI+py+1vgJxUNiCy
3oCFsIVCt3wi54FnRh5fMADv2RpvPIs1Yq5j6vuVVKsdAuMzHipMJeQJjSoKADs=}
image create photo align-t.gif -data {R0lGODlhFQASAKECAP8AALi4uP///////yH5BAEKAAMALAAAAAAVABIAAAI+nI+pF+3flgmi2hvk
oLdnyXXWt4SiQCqmmCarp71YfAHADNb3SFs2XgrYAI1hRDMYGpTIhK0peUIV0qn1UAAAOw==}
image create photo align-tl.gif -data {R0lGODlhFQASAKEBAP8AALi4uP///7i4uCH5BAEKAAMALAAAAAAVABIAAAJAnI+pE+3flgmi2hsk
u1xkSXXWt4Sip5kiqagdm7gcjMgXAIBnBeTlLuiBIA2AMWP0aQxJpnGpeEIX0mmiap0WAAA7}
image create photo align-tls.gif -data {R0lGODlhFQASAKEDAAAAAP8AAP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI9nI+pA+3flgGi2gsk
u1xkSXXWt4Sip5kiqagdm7gcjMhXEIBnhet7X4I0cIEMUYM4DpTIZK65AEIT0im0AAA7}
image create photo align-tr.gif -data {R0lGODlhFQASAKEBAP8AALi4uP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI9nI+pGe3flgmi2hvk
oLdnyXXWt4SiQCqmmDLnqK2eBABv1R71jdL23aoBNkKIIyE0JDWKGlPjfC6i0qqmAAA7}
image create photo align-trs.gif -data {R0lGODlhFQASAKEDAAAAAP8AAP///7i4uCH5BAEKAAMALAAAAAAVABIAAAI9nI+pCe3flgGi2gvk
oLdnyXXWt4SiQCqmmDLnqK2eFARv1R71jdL23aoFNkKIIyE0JDWKGlPjfC6i0qqmAAA7}
image create photo align-ts.gif -data {R0lGODlhFQASAKECAAAAAP8AAP///////yH5BAEKAAMALAAAAAAVABIAAAI+nI+pB+3flgGi2gvk
oLdnyXXWt4SiQCqmmCarp71YfAXBDNb3SFs2XgLYAo1hRDMYGpTIhK0peUIV0qn1UAAAOw==}


catch {menu $xth(me,ctxmenu).align -tearoff 0}
$xth(me,ctxmenu).align delete 0 end
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-tl.gif -selectimage align-tls.gif -variable xth(me,ctrl,ctx,align) -value "top-left" -command {xth_me_set_option_value align} 
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-l.gif -selectimage align-ls.gif -variable xth(me,ctrl,ctx,align) -value "left" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-bl.gif -selectimage align-bls.gif -variable xth(me,ctrl,ctx,align) -value "bottom-left" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-t.gif -selectimage align-ts.gif -columnbreak 1 -variable xth(me,ctrl,ctx,align) -value "top" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-c.gif -selectimage align-cs.gif -variable xth(me,ctrl,ctx,align) -value "center" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-b.gif -selectimage align-bs.gif -variable xth(me,ctrl,ctx,align) -value "bottom" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-tr.gif -selectimage align-trs.gif -columnbreak 1 -variable xth(me,ctrl,ctx,align) -value "top-right" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-r.gif -selectimage align-rs.gif -variable xth(me,ctrl,ctx,align) -value "right" -command {xth_me_set_option_value align}
$xth(me,ctxmenu).align add radiobutton -hidemargin 1 -image align-br.gif -selectimage align-brs.gif -variable xth(me,ctrl,ctx,align) -value "bottom-right" -command {xth_me_set_option_value align}


catch {menu $xth(me,ctxmenu).scale -tearoff 0}
$xth(me,ctxmenu).scale delete 0 end
$xth(me,ctxmenu).scale add radiobutton -label [mc "tiny (xs)"] -variable xth(me,ctrl,ctx,scale) -value "xs" -command {xth_me_set_option_value scale}
$xth(me,ctxmenu).scale add radiobutton -label [mc "small (s)"] -variable xth(me,ctrl,ctx,scale) -value "s" -command {xth_me_set_option_value scale}
$xth(me,ctxmenu).scale add radiobutton -label [mc "normal (m)"] -variable xth(me,ctrl,ctx,scale) -value "m" -command {xth_me_set_option_value scale}
$xth(me,ctxmenu).scale add radiobutton -label [mc "large (l)"] -variable xth(me,ctrl,ctx,scale) -value "l" -command {xth_me_set_option_value scale}
$xth(me,ctxmenu).scale add radiobutton -label [mc "huge (xl)"] -variable xth(me,ctrl,ctx,scale) -value "xl" -command {xth_me_set_option_value scale}

catch {menu $xth(me,ctxmenu).outline -tearoff 0}
$xth(me,ctxmenu).outline delete 0 end
$xth(me,ctxmenu).outline add radiobutton -label [mc "out"] -variable xth(me,ctrl,ctx,outline) -value "out" -command {xth_me_set_option_value outline}
$xth(me,ctxmenu).outline add radiobutton -label [mc "in"] -variable xth(me,ctrl,ctx,outline) -value "in" -command {xth_me_set_option_value outline}
$xth(me,ctxmenu).outline add radiobutton -label [mc "none"] -variable xth(me,ctrl,ctx,outline) -value "none" -command {xth_me_set_option_value outline}

catch {menu $xth(me,ctxmenu).border -tearoff 0}
$xth(me,ctxmenu).border delete 0 end
$xth(me,ctxmenu).border add radiobutton -label [mc "on"] -variable xth(me,ctrl,ctx,border) -value "on" -command {xth_me_set_option_value border}
$xth(me,ctxmenu).border add radiobutton -label [mc "off"] -variable xth(me,ctrl,ctx,border) -value "off" -command {xth_me_set_option_value border}

catch {menu $xth(me,ctxmenu).direction -tearoff 0}
$xth(me,ctxmenu).direction delete 0 end
$xth(me,ctxmenu).direction add radiobutton -label [mc "begin"] -variable xth(me,ctrl,ctx,direction) -value "begin" -command {xth_me_set_option_value direction}
$xth(me,ctxmenu).direction add radiobutton -label [mc "both"] -variable xth(me,ctrl,ctx,direction) -value "both" -command {xth_me_set_option_value direction}
$xth(me,ctxmenu).direction add radiobutton -label [mc "end"] -variable xth(me,ctrl,ctx,direction) -value "end" -command {xth_me_set_option_value direction}



catch {menu $xth(me,ctxmenu).cps -tearoff 0}
$xth(me,ctxmenu).cps delete 0 end
$xth(me,ctxmenu).cps add checkbutton -label [mc "<<"] -variable xth(ctrl,me,linept,idp) -command xth_me_cmds_toggle_linept
$xth(me,ctxmenu).cps add checkbutton -label [mc "smooth"] -variable xth(ctrl,me,linept,smooth) -command xth_me_cmds_toggle_linept
$xth(me,ctxmenu).cps add checkbutton -label [mc ">>"] -variable xth(ctrl,me,linept,idn) -command xth_me_cmds_toggle_linept

proc xth_me_create_line_point_edit_menu { lpem_father {state normal} } {
    upvar $lpem_father f
    global xth
    
    catch {menu $f.m -tearoff 0 -font $xth(gui,lfont)}
    $f.m add command -label [mc "Insert point"] -command {xth_me_cmds_start_linept_insert} -state $state
    $f.m add command -label [mc "Delete point"] -command {xth_me_cmds_delete_linept {} {}} -state $state
    $f.m add command -label [mc "Split line"] -command {xth_me_cmds_line_split} -state $state
    $f.m add command -label [mc "Trace line"] -command {xth_me_cmds_line_trace_start}
    $f.m add command -label [mc "Convert to curve"] -command {xth_me_cmds_line_poly2bezier}
    $f.m add command -label [mc "Simplify line"] -command {xth_me_cmds_line_simplify}    
}

catch {menu $xth(me,ctxmenu).subtype -tearoff 0}
catch {menu $xth(me,ctxmenu).segsubtype -tearoff 0}

catch {menu $xth(me,ctxmenu).altitude -tearoff 0}
$xth(me,ctxmenu).altitude delete 0 end
$xth(me,ctxmenu).altitude add radiobutton -label [mc "auto"] -variable xth(me,ctrl,ctx,altitude) -value "." -command {xth_me_set_optionline_value altitude}
$xth(me,ctxmenu).altitude add command -label [mc "edit"] -command {xth_me_ctx_change_text altitude [mc "Altitude"]}

catch {menu $xth(me,ctxmenu).others -tearoff 0}
xth_me_create_line_point_edit_menu xth(me,ctxmenu)

# return point option
proc xth_me_get_option_value {key optkey} {
  global xth
  set opt $xth($optkey)
  if {[regexp "(^|.*\\s)-($key)\\s+\\\[(\[^\\\]\]*)\\\]\\s*(.*)\$" $opt dum obeg okey oval oend]} {
  } elseif {[regexp "(^|.*\\s)-($key)\\s+\\\"(.*)\$" $opt dum obeg okey oval]} {
    set nx 0
    set nl [string length $oval]
    set oend {}
    for {set xx 0} {$xx < $nl} {incr xx} {
      if {[string equal [string index $oval $xx] "\""]} {
        if {[string equal [string index $oval [expr $xx + 1]] "\""]} {
          incr xx
        } else {
          set oend [string range $oval [expr $xx + 1] end]
          set oval [string range $oval 0 [expr $xx - 1]]
        } 
      }
    }
  } elseif {[regexp "(^|.*\\s)-($key)\\s+(\\S+)\\s*(.*)\$" $opt dum obeg okey oval oend]} {    
  } else {return [list {} $opt]}
  set oend [regsub -all {^\s+|\s+$} $oend {}] 
  set obeg [regsub -all {^\s+|\s+$} $obeg {}]
  if {([string length $obeg] > 0) && ([string length $oend] > 0)} {
    set oend " $oend"
  }
  return [list $oval "$obeg$oend"]
}


proc xth_me_get_optionline_value {key} {
  global xth
  set opt [$xth(ctrl,me,linept).oe.txt get 1.0 end]
  set oval {}
  set orest {}
  foreach ln [split $opt "\n"] {
    set oend $ln
    if {[regexp "^\\s*($key)\\s+\\\[(\[^\\\]\]*)\\\]\\s*(.*)\$" $ln dum okey oval oend]} {
    } elseif {[regexp "^\\s*($key)\\s+\\\"(.*)\$" $ln dum okey oval]} {
      set nx 0
      set nl [string length $oval]
      set oend {}
      for {set xx 0} {$xx < $nl} {incr xx} {
        if {[string equal [string index $oval $xx] "\""]} {
          if {[string equal [string index $oval [expr $xx + 1]] "\""]} {
            incr xx
          } else {
            set oend [string range $oval [expr $xx + 1] end]
            set oval [string range $oval 0 [expr $xx - 1]]
          } 
        }
      }
    } elseif {[regexp "^\\s*($key)\\s+(\\S+)\\s*(.*)\$" $ln dum okey oval oend]} {    
    }
    if {([string length $oend] > 0) && [regexp {\S+} $oend]} {
      if {[string length $orest] > 0} {
        set orest "$orest\n"
      }
      set orest "$orest$oend"
    }
  }
  return [list $oval $orest]
}



proc xth_me_optlabel {opt args} {
  global xth
  set lab [mc $opt]
  if {[llength $args] > 0} {
    set lab [lindex $args 0]
  }
  set val $xth(me,ctrl,ctx,$opt)
  set lval {}
  switch $opt {
    name -
    text -
    altitude -
    value {
      if {[string length $val] > 22} {
        set lval " ([string range $val 0 18] ...)"
      } elseif {[string length $val] > 0} {
        set lval " ([string range $val 0 22])"
      }
      if {[string equal $opt altitude] && [string equal $val "."]} {
        set lval " (auto)"
      }
    }
    default {
      if {!([string equal $val "auto"] || [string equal $val ""])} {
        set lval " ([mc $val])"
      }
    }
  }
  return "$lab$lval"
}

proc xth_me_set_option_value {opt} {
  global xth
  set nval $xth(me,ctrl,ctx,$opt)
  set oval $xth(me,ctrl,ctxold,$opt)
  switch $opt {
    name -
    text -
    value {
    }
    default {
      if {[string equal $nval "auto"] || [string equal $nval $oval]} {
        set nval {}
      }
    }
  }
  set oopts $xth(me,ctrl,ctxopt,$opt)
  if {[string length $nval] > 0} {
    if {[regexp {^[\w\.\-][^\s\[\]\"]*$} $nval]} {
      set xth($xth(me,ctrl,ctx,optkey)) "$oopts -$opt $nval" 
    } elseif {![regexp {[\[\]]} $nval]} {
      set xth($xth(me,ctrl,ctx,optkey)) "$oopts -$opt \[$nval\]" 
    } else {
      set nval [regsub -all {"} $nval {""}]
      set xth($xth(me,ctrl,ctx,optkey)) "$oopts -$opt \"$nval\"" 
    }
  } else {
    set xth($xth(me,ctrl,ctx,optkey)) $oopts 
  }
  xth_me_cmds_update {}  
}

  
# option name, option name in therion command  
proc xth_me_set_optionline_value {opt args} {
  global xth
  set nval $xth(me,ctrl,ctx,$opt)
  set oval $xth(me,ctrl,ctxold,$opt)
  set oopts $xth(me,ctrl,ctxopt,$opt)
  set optstr $opt
  if {[llength $args] > 0} {
    set optstr [lindex $args 0]
  }
  switch $opt {
    altitude {  
      if {[string equal $nval "."] && [string equal $nval $oval]} {
        set nval {}
      }  
    }
    segsubtype {  
      if {[string equal $nval $oval]} {
        set nval {}
      }  
    }
    default {      
    }
  }
  if {[string length $nval] > 0} {
    if {[string length $oopts] > 0} {
      set oopts "$oopts\n"
    }
    if {[regexp {^[\w\.\-][^\s\[\]\"]*$} $nval]} {
      set nopts "$oopts$optstr $nval" 
    } elseif {![regexp {[\[\]]} $nval]} {
      set nopts "$oopts$optstr \[$nval\]" 
    } else {
      set nval [regsub -all {"} $nval {""}]
      set nopts "$oopts$optstr \"$nval\"" 
    }
  } else {
    set nopts $oopts 
  }
  $xth(ctrl,me,linept).oe.txt delete 1.0 end
  $xth(ctrl,me,linept).oe.txt insert 1.0 $nopts
  $xth(ctrl,me,linept).oe.txt mark set insert end
  $xth(ctrl,me,linept).oe.txt see end
  xth_me_cmds_update {}  
}
 
  

proc xth_me_ctx_change_text {id args} {
  global xth
  if {[llength $args] < 1} {
    set title $id
  } else {
    set title [lindex $args 0]
  }
  set d $xth(gui,main).ctxdlg
  catch {destroy $d}
  set dlg [Dialog $d -side bottom -title $title -transient no -cancel 1 -default 0 -geometry $xth(me,ctrl,ctx,dlggeom)]
  $d add -text [mc "OK"]
  $d add -text [mc "Cancel"]
  set f [$d getframe]
  Entry $f.stname -font $xth(gui,lfont) -width 0 -textvariable xth(me,ctrl,ctx,$id) -command "Dialog::enddialog $d 0"
  pack $f.stname -expand 1 -fill both
  set res [$d draw $f.stname]
  if {$res == 0} {
    if {[string equal $id altitude]} {
      xth_me_set_optionline_value $id
    } else {
      xth_me_set_option_value $id
    }
  }
}


# Show context menu at given coordinates 
proc xth_me_show_context_menu {id x y} {
  global xth
  if {[llength $id] == 2} {
    set pid [lindex $id 1]
    set id [lindex $id 0]
  } else {
    set pid 0
  }
  $xth(me,can) raise point
  set some_below {}
  set select {}
  # select point, if not selected
  if {$xth(me,cmds,selid) != $id} {
    #xth_me_cmds_select "$id $pid"
    set select [list "$id $pid"]
    if {$pid != 0} {
      xth_ctrl_scroll_to me line
      xth_ctrl_maximize me line
      xth_ctrl_maximize me linept
    } else {
      xth_ctrl_scroll_to me point
      xth_ctrl_maximize me point
    }
  # select line point, if not selected
  } elseif {($xth(me,cmds,$id,ct) == 3) && ($xth(me,cmds,selpid) != $pid)} {
    #xth_me_cmds_select_linept $id $pid
    set select [list $id $pid]
    xth_ctrl_scroll_to me line
    xth_ctrl_maximize me line
    xth_ctrl_maximize me linept
  } else {
    # check, whether there is some point below
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
      set some_below "xth_me_cmds_select $nid; xth_ctrl_scroll_to me point; xth_ctrl_maximize me point; catch {\$xth(me,can) lower $utag point}; catch {\$xth(me,can) raise $utag line}"
    } elseif {[regexp "(^|\\s)pt(\\d+)\.(\\d+)($|\\s)" $tgs d1 d2 nid npid]} {
      set some_below "xth_me_cmds_select \"$nid $npid\"; if {$npid != 0} {xth_ctrl_scroll_to me line; xth_ctrl_maximize me line; xth_ctrl_maximize me linept} else {xth_ctrl_scroll_to me point; xth_ctrl_maximize me point}; catch {\$xth(me,can) lower $utag point}; catch {\$xth(me,can) raise $utag line}"
    }
  }
    
  if {[llength $select] == 1} {
    xth_me_cmds_select [lindex $select 0]
  } elseif {[llength $select] == 2} {
    xth_me_cmds_select_linept [lindex $select 0] [lindex $select 1]
  }
  
  $xth(me,ctxmenu) delete 0 end
  $xth(me,ctxmenu).others delete 0 end
  
  set opts {}
  if {$xth(me,cmds,$id,ct) == 2} {
    # add point menu items
    set opts "ctrl,me,point,opts"
    $xth(me,ctxmenu) add cascade -label [format "%s (%s)" [mc "type"] [thememc [list point $xth(me,cmds,$id,type)]]] -menu $xth(me,ctxmenu).ptypes
    
    # subtype if applicable
    $xth(me,ctxmenu).subtype delete 0 end
    if {[lsearch -exact {station air-draught water-flow} $xth(me,cmds,$id,type)] > -1} {
      set optsubtype [xth_me_get_option_value "subtype" $opts]
      set xth(me,ctrl,ctx,subtype) [lindex $optsubtype 0]
      if {[string length $xth(me,ctrl,ctx,subtype)] == 0} {
        set xth(me,ctrl,ctx,subtype) auto
      }
      switch $xth(me,cmds,$id,type) {
        station {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "temporary"] -variable xth(me,ctrl,ctx,subtype) -value "temporary" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "painted"] -variable xth(me,ctrl,ctx,subtype) -value "painted" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "natural"] -variable xth(me,ctrl,ctx,subtype) -value "natural" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "fixed"] -variable xth(me,ctrl,ctx,subtype) -value "fixed" -command {xth_me_set_option_value subtype}
        }
        air-draught {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "winter"] -variable xth(me,ctrl,ctx,subtype) -value "winter" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "summer"] -variable xth(me,ctrl,ctx,subtype) -value "summer" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "undefined"] -variable xth(me,ctrl,ctx,subtype) -value "undefined" -command {xth_me_set_option_value subtype}
        }
        water-flow {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "permanent"] -variable xth(me,ctrl,ctx,subtype) -value "permanent" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "intermittent"] -variable xth(me,ctrl,ctx,subtype) -value "intermittent" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "paleo"] -variable xth(me,ctrl,ctx,subtype) -value "paleo" -command {xth_me_set_option_value subtype}
        }
      }
      set xth(me,ctrl,ctxopt,subtype) [lindex $optsubtype 1] 
      $xth(me,ctxmenu) add cascade -label [xth_me_optlabel subtype] -menu $xth(me,ctxmenu).subtype
    }    
    
    # change align
    if {[lsearch -exact {station} $xth(me,cmds,$id,type)] == -1} {
    	set optalign [xth_me_get_option_value "align" $opts]
    	# set variable
    	switch -nocase [lindex $optalign 0] {
    	  t - top {
          #DON'T REMOVE - for translation. [mc "top"]
    	    set xth(me,ctrl,ctx,align) "top"
    	  }
    	  tr - top-right {
          #DON'T REMOVE - for translation. [mc "top-right"]
    	    set xth(me,ctrl,ctx,align) "top-right"
    	  }
    	  tl - top-left {
          #DON'T REMOVE - for translation. [mc "top-left"]
    	    set xth(me,ctrl,ctx,align) "top-left"
    	  }
    	  b - bottom {
          #DON'T REMOVE - for translation. [mc "bottom"]
    	    set xth(me,ctrl,ctx,align) "bottom"
    	  }
    	  br - bottom-right {
          #DON'T REMOVE - for translation. [mc "bottom-right"]
    	    set xth(me,ctrl,ctx,align) "bottom-right"
    	  }
    	  bl - bottom-left {
          #DON'T REMOVE - for translation. [mc "bottom-left"]
    	    set xth(me,ctrl,ctx,align) "bottom-left"
    	  }
    	  c - centre - center {
          #DON'T REMOVE - for translation. [mc "center"]
    	    set xth(me,ctrl,ctx,align) "center"
    	  }
    	  r - right {
          #DON'T REMOVE - for translation. [mc "right"]
    	    set xth(me,ctrl,ctx,align) "right"
    	  }
    	  l - left {
          #DON'T REMOVE - for translation. [mc "left"]
    	    set xth(me,ctrl,ctx,align) "left"
    	  }
    	  default {
          #DON'T REMOVE - for translation. [mc "auto"]
    	    set xth(me,ctrl,ctx,align) "auto"
    	  }
    	}
    	# set options
      set xth(me,ctrl,ctxopt,align) [lindex $optalign 1]
      #DON'T REMOVE - for translation. [mc "align"]
      $xth(me,ctxmenu) add cascade -label [xth_me_optlabel align] -menu $xth(me,ctxmenu).align
    }
	  	  
    # name
    if {[lsearch -exact {station} $xth(me,cmds,$id,type)] > -1} {
      set optname [xth_me_get_option_value "name" $opts]
      set xth(me,ctrl,ctx,name) [lindex $optname 0]
      set xth(me,ctrl,ctxopt,name) [lindex $optname 1] 
      #DON'T REMOVE - for translation. [mc "name"]
      $xth(me,ctxmenu) add command -label [xth_me_optlabel name] -command {xth_me_ctx_change_text name [mc "Station name"]}
    }
    # scrap
    if {[lsearch -exact {section} $xth(me,cmds,$id,type)] > -1} {
      set optscrap [xth_me_get_option_value "scrap" $opts]
      set xth(me,ctrl,ctx,scrap) [lindex $optscrap 0]
      set xth(me,ctrl,ctxopt,scrap) [lindex $optscrap 1] 
      #DON'T REMOVE - for translation. [mc "scrap"]
      $xth(me,ctxmenu) add command -label [xth_me_optlabel scrap] -command {xth_me_ctx_change_text scrap [mc "Scrap reference"]}
    }
    # text
    if {[lsearch -exact {label remark continuation} $xth(me,cmds,$id,type)] > -1} {
      set opttext [xth_me_get_option_value "text" $opts]
      set xth(me,ctrl,ctx,text) [lindex $opttext 0]
      set xth(me,ctrl,ctxopt,text) [lindex $opttext 1]
      #DON'T REMOVE - for translation. [mc "text"]
      $xth(me,ctxmenu) add command -label [xth_me_optlabel text] -command {xth_me_ctx_change_text text}
    }
    # value
    if {[lsearch -exact {height passage-height dimensions} $xth(me,cmds,$id,type)] > -1} {
      set optvalue [xth_me_get_option_value "value" $opts]
      set xth(me,ctrl,ctx,value) [lindex $optvalue 0]
      set xth(me,ctrl,ctxopt,value) [lindex $optvalue 1] 
      #DON'T REMOVE - for translation. [mc "value"]
      $xth(me,ctxmenu) add command -label [xth_me_optlabel value] -command {xth_me_ctx_change_text value}
    }
    # toggle orientation
    if {[lsearch -exact {station} $xth(me,cmds,$id,type)] == -1} {
      $xth(me,ctxmenu) add checkbutton -label [mc "orientation"] -variable xth(ctrl,me,point,rotid) -command xth_me_cmds_point_change_state
    }
  } elseif {$xth(me,cmds,$id,ct) == 3} {
  
    # add line menu items
    set opts "ctrl,me,line,opts"
    $xth(me,ctxmenu) add cascade -label [format "%s (%s)" [mc "type"] [thememc [list line $xth(me,cmds,$id,type)]]] -menu $xth(me,ctxmenu).ltypes
    
    # subtype if applicable
    $xth(me,ctxmenu).subtype delete 0 end
    if {[lsearch -exact {wall border water-flow survey} $xth(me,cmds,$id,type)] > -1} {
      set optsubtype [xth_me_get_option_value "subtype" $opts]
      set xth(me,ctrl,ctx,subtype) [lindex $optsubtype 0]
      if {[string length $xth(me,ctrl,ctx,subtype)] == 0} {
        set xth(me,ctrl,ctx,subtype) auto
      }
      switch $xth(me,cmds,$id,type) {
        wall {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "bedrock"] -variable xth(me,ctrl,ctx,subtype) -value "bedrock" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "invisible"] -variable xth(me,ctrl,ctx,subtype) -value "invisible" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "underlying"] -variable xth(me,ctrl,ctx,subtype) -value "underlying" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "overlying"] -variable xth(me,ctrl,ctx,subtype) -value "overlying" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "unsurveyed"] -variable xth(me,ctrl,ctx,subtype) -value "unsurveyed" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "presumed"] -variable xth(me,ctrl,ctx,subtype) -value "presumed" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "pit"] -variable xth(me,ctrl,ctx,subtype) -value "pit" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "sand"] -variable xth(me,ctrl,ctx,subtype) -value "sand" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "clay"] -variable xth(me,ctrl,ctx,subtype) -value "clay" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "pebbles"] -variable xth(me,ctrl,ctx,subtype) -value "pebbles" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "debris"] -variable xth(me,ctrl,ctx,subtype) -value "debris" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "blocks"] -variable xth(me,ctrl,ctx,subtype) -value "blocks" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "ice"] -variable xth(me,ctrl,ctx,subtype) -value "ice" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "flowstone"] -variable xth(me,ctrl,ctx,subtype) -value "flowstone" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "moonmilk"] -variable xth(me,ctrl,ctx,subtype) -value "moonmilk" -command {xth_me_set_option_value subtype}
        }
        border {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "visible"] -variable xth(me,ctrl,ctx,subtype) -value "visible" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "invisible"] -variable xth(me,ctrl,ctx,subtype) -value "invisible" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "temporary"] -variable xth(me,ctrl,ctx,subtype) -value "temporary" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "presumed"] -variable xth(me,ctrl,ctx,subtype) -value "presumed" -command {xth_me_set_option_value subtype}
        }
        water-flow {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "permanent"] -variable xth(me,ctrl,ctx,subtype) -value "permanent" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "intermittent"] -variable xth(me,ctrl,ctx,subtype) -value "intermittent" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "conjectural"] -variable xth(me,ctrl,ctx,subtype) -value "conjectural" -command {xth_me_set_option_value subtype}
        }
        survey {
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "cave"] -variable xth(me,ctrl,ctx,subtype) -value "cave" -command {xth_me_set_option_value subtype}
          $xth(me,ctxmenu).subtype add radiobutton -label [mc "surface"] -variable xth(me,ctrl,ctx,subtype) -value "surface" -command {xth_me_set_option_value subtype}
        }
      }
      set xth(me,ctrl,ctxopt,subtype) [lindex $optsubtype 1] 
      #DON'T REMOVE - for translation. [mc "subtype"]
      $xth(me,ctxmenu) add cascade -label [xth_me_optlabel subtype] -menu $xth(me,ctxmenu).subtype
    }    

        
    $xth(me,ctxmenu) add cascade -label [mc "control points"] -menu $xth(me,ctxmenu).cps
    if {[lsearch -exact {slope} $xth(me,cmds,$id,type)] > -1} {
      $xth(me,ctxmenu) add checkbutton -label [mc "orientation"] -variable xth(ctrl,me,linept,rotid) -command xth_me_cmds_toggle_linept
      $xth(me,ctxmenu) add checkbutton -label [mc "l-size"] -variable xth(ctrl,me,linept,lsid) -command xth_me_cmds_toggle_linept
    }

    update
    set subtypelen [$xth(me,ctxmenu).subtype index end]
    if {($subtypelen > 0) && ($subtypelen < 22)} {
      $xth(me,ctxmenu).segsubtype delete 0 end
      set optsubtype [xth_me_get_optionline_value "subtype"]
      set xth(me,ctrl,ctx,segsubtype) [lindex $optsubtype 0]
      if {[string length $xth(me,ctrl,ctx,segsubtype)] == 0} {
        set xth(me,ctrl,ctx,segsubtype) auto
      }      
      set xth(me,ctrl,ctxopt,segsubtype) [lindex $optsubtype 1]
      for {set sti 0} {$sti < $subtypelen} {incr sti} {
        $xth(me,ctxmenu).segsubtype add radiobutton -label [$xth(me,ctxmenu).subtype entrycget $sti -label] -variable xth(me,ctrl,ctx,segsubtype) -value [$xth(me,ctxmenu).subtype entrycget $sti -value] -command {xth_me_set_optionline_value segsubtype subtype}
      }
      $xth(me,ctxmenu) add cascade -label [xth_me_optlabel segsubtype [mc "segment subtype"]] -menu $xth(me,ctxmenu).segsubtype
    }
    
    # wall altitude
    if {[lsearch -exact {wall} $xth(me,cmds,$id,type)] > -1} {
      #DON'T REMOVE - for translation. [mc "altitude"]
      set optalt [xth_me_get_optionline_value "altitude"]
      set xth(me,ctrl,ctx,altitude) [lindex $optalt 0]
      if {[lsearch -exact {- nan NaN NAN} xth(me,ctrl,ctx,altitude)] > -1} {
        set xth(me,ctrl,ctx,altitude) "."  
      }
      set xth(me,ctrl,ctxopt,altitude) [lindex $optalt 1]
      $xth(me,ctxmenu) add cascade -label [xth_me_optlabel altitude [mc "altitude label"]] -menu $xth(me,ctxmenu).altitude
    }
    
    
    # outline
    set optoutline [xth_me_get_option_value "outline" $opts]
    # set variable
    switch -nocase [lindex $optoutline 0] {
      in {
        set xth(me,ctrl,ctx,outline) "in"
      }
      out {
        set xth(me,ctrl,ctx,outline) "out"
      }
      none {
        set xth(me,ctrl,ctx,outline) "none"
      }
      default {
        set xth(me,ctrl,ctx,outline) "auto"
      }
    }
    # set options
    set xth(me,ctrl,ctxopt,outline) [lindex $optoutline 1] 
    #DON'T REMOVE - for translation. [mc "outline"]
    $xth(me,ctxmenu).others add cascade -label [xth_me_optlabel outline] -menu $xth(me,ctxmenu).outline
      
     # border
    if {[string equal $xth(me,cmds,$id,type) "slope"]} {
	    set optborder [xth_me_get_option_value "border" $opts]
	    # set variable
	    switch -nocase [lindex $optborder 0] {
		on {
		    set xth(me,ctrl,ctx,border) "on"
		}
		off {
		    set xth(me,ctrl,ctx,border) "off"
		}
		default {
		    set xth(me,ctrl,ctx,border) "auto"
		}
	    }
	    # set options
	    set xth(me,ctrl,ctxopt,border) [lindex $optborder 1] 
	    #DON'T REMOVE - for translation. [mc "border"]
	    $xth(me,ctxmenu) add cascade -label [xth_me_optlabel border] -menu $xth(me,ctxmenu).border
    }

  	
    # text for label
    if {[lsearch -exact {label} $xth(me,cmds,$id,type)] > -1} {
      set opttext [xth_me_get_option_value "text" $opts]
      set xth(me,ctrl,ctx,text) [lindex $opttext 0]
      set xth(me,ctrl,ctxopt,text) [lindex $opttext 1]
      #DON'T REMOVE - for translation. [mc "text"]
      $xth(me,ctxmenu) add command -label [xth_me_optlabel text] -command {xth_me_ctx_change_text text}
    }
	  
    # direction for section
    if {[lsearch -exact {section} $xth(me,cmds,$id,type)] > -1} {
      set optdir [xth_me_get_option_value "direction" $opts]
      switch -nocase [lindex $optdir 0] {
	begin {
	    set xth(me,ctrl,ctx,direction) "begin"
	}
	both {
	    set xth(me,ctrl,ctx,direction) "both"
	}
	end {
	    set xth(me,ctrl,ctx,direction) "end"
	}
	default {
	    set xth(me,ctrl,ctx,direction) ""
	}
      }	    
      set xth(me,ctrl,ctxopt,direction) [lindex $optdir 1]
      #DON'T REMOVE - for translation. [mc "direction"]
      $xth(me,ctxmenu) add cascade -label [xth_me_optlabel direction] -menu $xth(me,ctxmenu).direction
    }
      	  
  }
  
  # common options 
  set xth(me,ctrl,ctx,optkey) $opts
  set xth(me,ctrl,ctx,dlggeom) "+[expr $x + [winfo rootx $xth(me,can)]]+[expr $y + [winfo rooty $xth(me,can)]]"  

  # clip
  set optclip [xth_me_get_option_value "clip" $opts]
  # set variable
  switch -nocase [lindex $optclip 0] {
    0 - off - false {
      set xth(me,ctrl,ctx,clip) "off"
    }
    1 - on - true {
      set xth(me,ctrl,ctx,clip) "on"
    }
    default {
      set xth(me,ctrl,ctx,clip) "auto"
    }
  }
  # set options
  set xth(me,ctrl,ctxopt,clip) [lindex $optclip 1] 
  #DON'T REMOVE - for translation. [mc "clip"]
  $xth(me,ctxmenu).others add cascade -label [xth_me_optlabel clip] -menu $xth(me,ctxmenu).clip

  $xth(me,ctxmenu) add cascade -label [mc "other options"] -menu $xth(me,ctxmenu).others
  
    
  # place
  set optplace [xth_me_get_option_value "place" $opts]
  # set variable
  switch -nocase [lindex $optplace 0] {
    top {
      set xth(me,ctrl,ctx,place) "top"
    }
    bottom {
      set xth(me,ctrl,ctx,place) "bottom"
    }
    default {
      set xth(me,ctrl,ctx,place) "auto"
    }
  }
  # set options
  set xth(me,ctrl,ctxopt,place) [lindex $optplace 1] 
  #DON'T REMOVE - for translation. [mc "place"]
  $xth(me,ctxmenu).others add cascade -label [xth_me_optlabel place] -menu $xth(me,ctxmenu).place
  
  
  # scale
  set optscale [xth_me_get_option_value "scale" $opts]
  # set variable
  switch -nocase [lindex $optscale 0] {
    xs - tiny {
      set xth(me,ctrl,ctx,scale) "xs"
    }
    s - small {
      set xth(me,ctrl,ctx,scale) "s"
    }
    m - normal {
      set xth(me,ctrl,ctx,scale) "m"
    }
    l - large {
      set xth(me,ctrl,ctx,scale) "l"
    }
    xl - huge {
      set xth(me,ctrl,ctx,scale) "xl"
    }
    default {
      set xth(me,ctrl,ctx,scale) "auto"
    }
  }
  # set options
  set xth(me,ctrl,ctxopt,scale) [lindex $optscale 1] 
  #DON'T REMOVE - for translation. [mc "scale"]
  $xth(me,ctxmenu).others add cascade -label [xth_me_optlabel scale] -menu $xth(me,ctxmenu).scale
  
  
  # visibility
  set optvis [xth_me_get_option_value "visible|visibility" $opts]
  # set variable
  switch -nocase [lindex $optvis 0] {
    0 - off - false {
      set xth(me,ctrl,ctx,visibility) "off"
    }
    1 - on - true {
      set xth(me,ctrl,ctx,visibility) "on"
    }
    default {
      set xth(me,ctrl,ctx,visibility) "auto"
    }
  }
  # set options
  set xth(me,ctrl,ctxopt,visibility) [lindex $optvis 1]   
  $xth(me,ctxmenu).others add cascade -label [xth_me_optlabel visibility] -menu $xth(me,ctxmenu).visibility
      
  $xth(me,ctxmenu) add separator
  if {$xth(me,cmds,$id,ct) == 3} {
    $xth(me,ctxmenu) add cascade -label [mc "Edit line"] -menu $xth(me,ctxmenu).m
    $xth(me,ctxmenu).others add checkbutton -label [mc "close"] -variable xth(ctrl,me,line,close) -command xth_me_cmds_toggle_line_close
    $xth(me,ctxmenu).others add checkbutton -label [mc "reverse"] -variable xth(ctrl,me,line,reverse) -command xth_me_cmds_toggle_line_reverse
  }
  if {[string length $some_below] > 0} {
    $xth(me,ctxmenu) add command -label [mc "Select underlying point"] -command $some_below    
  }
  $xth(me,ctxmenu) add command -label [mc "Delete symbol"] -command {xth_me_cmds_delete {}}   
  	
  # set original values variables
  foreach item [array names xth {me,ctrl,ctx,*}] {
    if {[regexp {^me,ctrl,ctx,([^,]*)$} $item dum key]} {
      set xth(me,ctrl,ctxold,$key) $xth($item)
    }
  }
  tk_popup $xth(me,ctxmenu) [expr $x + [winfo rootx $xth(me,can)]] [expr $y + [winfo rooty $xth(me,can)]]
}


proc xth_me_mousewheel_up {} {
  global xth
  switch $xth(app,active) {
    me {
      if {[expr [clock seconds] - $xth(me,wheel,clk)] < 2.0} {
        incr xth(me,wheel,position) 1
      } else {
        set xth(me,wheel,position) 1
      }
      set xth(me,wheel,clk) [clock seconds]
      if {$xth(me,wheel,position) > $xth(me,wheel,sensitivity)} {
        set xth(me,wheel,position) 0
        if {$xth(me,zoom) > 200} return;
        xth_me_area_zoom_to [expr 2*$xth(me,zoom)]
      }
    }
  }
}

proc xth_me_mousewheel_down {} {
  global xth
  switch $xth(app,active) {
    me {
      if {[expr [clock seconds] - $xth(me,wheel,clk)] < 2.0} {
        incr xth(me,wheel,position) -1
      } else {
        set xth(me,wheel,position) -1
      }
      set xth(me,wheel,clk) [clock seconds]
      if {$xth(me,wheel,position) < -$xth(me,wheel,sensitivity)} {
        set xth(me,wheel,position) 0
        if {$xth(me,zoom) < 50} return;
        xth_me_area_zoom_to [expr $xth(me,zoom) / 2]
      }
    }
  }
}

