##
## me_cmds2.tcl --
##
##     Map editor command processing 2.
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

proc xth_me_cmds_get_option {ln opt} {
  set rxl [list [list "^\\s*$opt\\s+\\\[(\[^\\\]\]*)\\\]" "\["]\
    [list "^\\s*$opt\\s+\\\"((\\\"\\\"|\[^\\\"])+)\\\"" "\""]\
    [list "^\\s*$opt\\s+(\\S+)" {}]]
  set rln $ln
  set val {}
  set sep {}
  set res 0
  foreach rx $rxl {
    if {[regexp [lindex $rx 0] $ln dump val]} {
      regsub [lindex $rx 0] $ln {} rln
      regsub {^\s*} $rln {} rln
      set sep [lindex $rx 1]
      set res 1
      break
    }
  }
  return [list $val $rln $res]
}


proc xth_me_cmds_get_onoffauto {opt} {
  switch $opt {
    on {return 1}
    off {return 0}
    default {return -1}
  }
}


proc xth_me_cmds_get_bool {opt} {
  if {[lsearch {on 1 true yes} $opt] > -1} {
    return 1
  } else {
    return 0
  }
}


proc xth_me_cmds_update_line_ctrl {id} {

  global xth
  
  if {[string length $id] > 0} {

    $xth(ctrl,me,line).typl configure -state normal    
    $xth(ctrl,me,line).typ configure -state normal    
    $xth(ctrl,me,line).namel configure -state normal    
    $xth(ctrl,me,line).name configure -state normal    
    $xth(ctrl,me,line).optl configure -state normal    
    $xth(ctrl,me,line).opt configure -state normal    
    $xth(ctrl,me,line).rev configure -state normal    
    $xth(ctrl,me,line).cls configure -state normal    
#    $xth(ctrl,me,line).insp configure -state normal    
#    $xth(ctrl,me,line).delp configure -state normal    
    $xth(ctrl,me,line).theme configure -state normal    
    $xth(ctrl,me,line).themetype configure -state normal    
    $xth(ctrl,me,line).lpa configure -state normal    
    $xth(ctrl,me,line).upd configure -state normal    
    $xth(ctrl,me,line).trace configure -state normal    
    $xth(ctrl,me,line).vector configure -state normal    
    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Insert point"] -state normal    
    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Delete point"] -state normal    
    $xth(ctrl,me,line).pl.l configure -takefocus 1 \
      -listvariable xth(me,cmds,$id,plist)

    set xth(ctrl,me,line,type) $xth(me,cmds,$id,type)
    set xth(ctrl,me,line,name) $xth(me,cmds,$id,name)
    set xth(ctrl,me,line,opts) $xth(me,cmds,$id,options)
    set xth(ctrl,me,line,reverse) $xth(me,cmds,$id,reverse)
    set xth(ctrl,me,line,close) $xth(me,cmds,$id,close)
    set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
    set tx [lsearch -exact $xth(me,themes,$at,line,hidelist) $xth(me,cmds,$id,type)]
    if {$tx < 0} {
      set xth(ctrl,me,line,themetype) {}
    } else {
      set xth(ctrl,me,line,themetype) [lindex $xth(me,themes,$at,line,showlist) $tx]
    }

    xth_me_prev_cmd $xth(me,cmds,$id,data)

    catch {$xth(me,can) raise lnln$id line}
    catch {$xth(me,can) raise lnpt$id point}
    
  } else {
  
    set xth(ctrl,me,line,name) {}
    set xth(ctrl,me,line,reverse) 0
    set xth(ctrl,me,line,close) 0
    set xth(ctrl,me,line,type) $xth(me,dflt,line,type)
    set xth(ctrl,me,line,opts) $xth(me,dflt,line,options)

    $xth(ctrl,me,line).typl configure -state disabled    
    $xth(ctrl,me,line).typ configure -state disabled    
    $xth(ctrl,me,line).namel configure -state disabled    
    $xth(ctrl,me,line).name configure -state disabled    
    $xth(ctrl,me,line).optl configure -state disabled    
    $xth(ctrl,me,line).opt configure -state disabled    
    $xth(ctrl,me,line).rev configure -state disabled    
    $xth(ctrl,me,line).cls configure -state disabled    
#    $xth(ctrl,me,line).insp configure -state disabled    
#    $xth(ctrl,me,line).delp configure -state disabled    
    $xth(ctrl,me,line).theme configure -state disabled     
    $xth(ctrl,me,line).themetype configure -state disabled     
    $xth(ctrl,me,line).lpa configure -state disabled    
    $xth(ctrl,me,line).upd configure -state disabled
    $xth(ctrl,me,line).trace configure -state disabled
    $xth(ctrl,me,line).vector configure -state disabled
    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Insert point"] -state disabled
    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Delete point"] -state disabled   
    $xth(ctrl,me,line).pl.l configure -takefocus 0 \
      -listvariable xth(ctrl,me,line,empty)
    $xth(ctrl,me,line).pl.l selection clear 0 end
      
  }
  
}



proc xth_me_cmds_update_area_ctrl {id} {

  global xth
  
  if {[string length $id] > 0} {

    $xth(ctrl,me,ac).typl configure -state normal    
    $xth(ctrl,me,ac).typ configure -state normal    
    $xth(ctrl,me,ac).theme configure -state normal    
    $xth(ctrl,me,ac).themetype configure -state normal
    $xth(ctrl,me,ac).optl configure -state normal    
    $xth(ctrl,me,ac).opt configure -state normal    
    $xth(ctrl,me,ac).ins configure -state normal    
    $xth(ctrl,me,ac).del configure -state normal    
    $xth(ctrl,me,ac).insid configure -state normal    
    $xth(ctrl,me,ac).inside configure -state normal    
    $xth(ctrl,me,ac).upd configure -state normal    
    $xth(ctrl,me,ac).shw configure -state normal    
    $xth(ctrl,me,ac).ll.l configure -takefocus 1 \
      -listvariable xth(me,cmds,$id,llist)
    $xth(ctrl,me,ac).ll.l selection clear 0 end
    $xth(ctrl,me,ac).ll.l selection set end end
    $xth(ctrl,me,ac).ll.l see end
    
    set xth(ctrl,me,ac,type) $xth(me,cmds,$id,type)
    set xth(ctrl,me,ac,name) $xth(me,cmds,$id,name)
    set xth(ctrl,me,ac,opts) $xth(me,cmds,$id,options)

    set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
    set tx [lsearch -exact $xth(me,themes,$at,area,hidelist) $xth(me,cmds,$id,type)]
    if {$tx < 0} {
      set xth(ctrl,me,ac,themetype) {}
    } else {
      set xth(ctrl,me,ac,themetype) [lindex $xth(me,themes,$at,area,showlist) $tx]
    }

    set xth(ctrl,me,ac,insid) {}
    xth_me_prev_cmd $xth(me,cmds,$id,data)

  } else {
  
    set xth(ctrl,me,ac,name) {}
    set xth(ctrl,me,ac,insid) {}
    set xth(ctrl,me,ac,type) $xth(me,dflt,area,type)
    set xth(ctrl,me,ac,opts) $xth(me,dflt,area,options)

    $xth(ctrl,me,ac).typl configure -state disabled    
    $xth(ctrl,me,ac).typ configure -state disabled    
    $xth(ctrl,me,ac).theme configure -state disabled    
    $xth(ctrl,me,ac).themetype configure -state disabled
    $xth(ctrl,me,ac).optl configure -state disabled    
    $xth(ctrl,me,ac).opt configure -state disabled    
    $xth(ctrl,me,ac).ins configure -state disabled    
    $xth(ctrl,me,ac).del configure -state disabled    
    $xth(ctrl,me,ac).insid configure -state disabled    
    $xth(ctrl,me,ac).inside configure -state disabled    
    $xth(ctrl,me,ac).upd configure -state disabled    
    $xth(ctrl,me,ac).shw configure -state disabled    
    $xth(ctrl,me,ac).ll.l configure -takefocus 0 \
      -listvariable xth(ctrl,me,ac,empty)
    $xth(ctrl,me,ac).ll.l selection clear 0 end
      
  }
  
}



proc xth_me_cmds_move_lineptcp_xctrl {id ppid pid npid} {
  global xth
  
  set px [xth_me_real2canx $xth(me,cmds,$id,$pid,x)]
  set py [xth_me_real2cany $xth(me,cmds,$id,$pid,y)]
  
  if {$xth(me,cmds,$id,$pid,idp)} {
    set x [xth_me_real2canx $xth(me,cmds,$id,$pid,xp)]
    set y [xth_me_real2cany $xth(me,cmds,$id,$pid,yp)]
    $xth(me,can) coords $xth(me,canid,linept,pcp) [list \
      [expr $x - $xth(gui,me,line,cpsize)] \
      [expr $y - $xth(gui,me,line,cpsize)] \
      [expr $x + $xth(gui,me,line,cpsize)] \
      [expr $y + $xth(gui,me,line,cpsize)]]
    $xth(me,can) coords $xth(me,canid,linept,pcpl) $px $py $x $y
  }
  
  if {$xth(me,cmds,$id,$pid,idn)} {
    set x [xth_me_real2canx $xth(me,cmds,$id,$pid,xn)]
    set y [xth_me_real2cany $xth(me,cmds,$id,$pid,yn)]
    $xth(me,can) coords $xth(me,canid,linept,ncp) [list \
      [expr $x - $xth(gui,me,line,cpsize)] \
      [expr $y - $xth(gui,me,line,cpsize)] \
      [expr $x + $xth(gui,me,line,cpsize)] \
      [expr $y + $xth(gui,me,line,cpsize)]]
    $xth(me,can) coords $xth(me,canid,linept,ncpl) $px $py $x $y
  }

  if {($npid > 0) && $xth(me,cmds,$id,$npid,idp)} {
    set px [xth_me_real2canx $xth(me,cmds,$id,$npid,x)]
    set py [xth_me_real2cany $xth(me,cmds,$id,$npid,y)]
    set x [xth_me_real2canx $xth(me,cmds,$id,$npid,xp)]
    set y [xth_me_real2cany $xth(me,cmds,$id,$npid,yp)]
    $xth(me,can) coords $xth(me,canid,linept,nncp) [list \
      [expr $x - $xth(gui,me,line,cpsize)] \
      [expr $y - $xth(gui,me,line,cpsize)] \
      [expr $x + $xth(gui,me,line,cpsize)] \
      [expr $y + $xth(gui,me,line,cpsize)]]
    $xth(me,can) coords $xth(me,canid,linept,nncpl) $px $py $x $y
  }

  if {($ppid > 0) && $xth(me,cmds,$id,$ppid,idn)} {
    set px [xth_me_real2canx $xth(me,cmds,$id,$ppid,x)]
    set py [xth_me_real2cany $xth(me,cmds,$id,$ppid,y)]
    set x [xth_me_real2canx $xth(me,cmds,$id,$ppid,xn)]
    set y [xth_me_real2cany $xth(me,cmds,$id,$ppid,yn)]
    $xth(me,can) coords $xth(me,canid,linept,ppcp) [list \
      [expr $x - $xth(gui,me,line,cpsize)] \
      [expr $y - $xth(gui,me,line,cpsize)] \
      [expr $x + $xth(gui,me,line,cpsize)] \
      [expr $y + $xth(gui,me,line,cpsize)]]
    $xth(me,can) coords $xth(me,canid,linept,ppcpl) $px $py $x $y
  }

  xth_me_cmds_move_linept_size_xctrl $id $pid $xth(me,cmds,$id,$pid,rotation) \
    $xth(me,cmds,$id,$pid,rs) $xth(me,cmds,$id,$pid,ls)
  xth_me_cmds_move_line_xctrl $id
}


proc xth_me_cmds_move_linept_xctrl {id pid} {
  global xth
  set x [xth_me_real2canx $xth(me,cmds,$id,$pid,x)]
  set y [xth_me_real2cany $xth(me,cmds,$id,$pid,y)]
  $xth(me,can) coords $xth(me,canid,linept,selector) [list \
    [expr $x - 3 * $xth(gui,me,line,psize)] \
    [expr $y - 3 * $xth(gui,me,line,psize)] \
    [expr $x + 3 * $xth(gui,me,line,psize)] \
    [expr $y + 3 * $xth(gui,me,line,psize)]]
}


proc xth_me_cmds_move_line_xctrl {id} {
  global xth
  set pid [lindex $xth(me,cmds,$id,xplist) 0]
  set rot [xth_me_cmds_get_default_rotation $id $pid]
  if {$xth(me,cmds,$id,reverse)} {
    set rot [expr $rot + 180.0]
  }
  set x [xth_me_real2canx $xth(me,cmds,$id,$pid,x)]
  set y [xth_me_real2cany $xth(me,cmds,$id,$pid,y)]
  $xth(me,can) coords $xth(me,canid,line,tick) [list $x $y \
      [expr $x + sin($rot/180.0*3.14159265259) * $xth(gui,me,line,ticksize)] \
      [expr $y - cos($rot/180.0*3.14159265259) * $xth(gui,me,line,ticksize)]]
}


proc xth_me_cmds_show_line_xctrl {id} {
  global xth
  if {[llength $xth(me,cmds,$id,xplist)] < 3} {
    xth_me_cmds_hide_line_xctrl    
    return
  }
  xth_me_cmds_move_line_xctrl $id
  $xth(me,can) itemconfigure entirelinectrl -state normal
  $xth(me,can) raise entirelinectrl
  $xth(me,can) lower entirelinectrl point
}

proc xth_me_cmds_hide_line_xctrl {} {
  global xth
  $xth(me,can) itemconfigure entirelinectrl -state hidden
}

proc xth_me_cmds_show_linept_xctrl {id pid} {
  global xth
  set x [xth_me_real2canx $xth(me,cmds,$id,$pid,x)]
  set y [xth_me_real2cany $xth(me,cmds,$id,$pid,y)]
  set xl $xth(me,cmds,$id,xplist)
  $xth(me,can) raise linectrl
  $xth(me,can) lower linectrl point

  $xth(me,can) itemconfigure $xth(me,canid,linept,selector) -state normal
  xth_me_cmds_move_linept_xctrl $id $pid

  if {$xth(me,cmds,$id,$pid,idp)} {
    $xth(me,can) itemconfigure $xth(me,canid,linept,pcpl) -state normal
    $xth(me,can) itemconfigure $xth(me,canid,linept,pcp) -state normal
  } else {
    $xth(me,can) itemconfigure $xth(me,canid,linept,pcpl) -state hidden
    $xth(me,can) itemconfigure $xth(me,canid,linept,pcp) -state hidden
  }
  if {$xth(me,cmds,$id,$pid,idn)} {
    $xth(me,can) itemconfigure $xth(me,canid,linept,ncpl) -state normal
    $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -state normal
  } else {
    $xth(me,can) itemconfigure $xth(me,canid,linept,ncpl) -state hidden
    $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -state hidden
  }
  set ix [lsearch $xl $pid]
  set lix [expr [llength $xl] - 2]
  if {$ix > 0} {
    set ppid [lindex $xl [expr $ix - 1]]
  } elseif {$xth(me,cmds,$id,close) && ($lix > 0)} {
    set ppid [lindex $xl [expr $lix - 1]]
  } else {
    set ppid 0
  }
  if {$ix < $lix} {
    set npid [lindex $xl [expr $ix + 1]]
  } elseif {$xth(me,cmds,$id,close) && ($lix > 0)} {
    set npid [lindex $xl 1]
  } else {
    set npid 0
  }
  if {($npid > 0) && $xth(me,cmds,$id,$npid,idp)} {
    $xth(me,can) itemconfigure $xth(me,canid,linept,nncpl) -state normal
    $xth(me,can) itemconfigure $xth(me,canid,linept,nncp) -state normal
  } else {
    $xth(me,can) itemconfigure $xth(me,canid,linept,nncpl) -state hidden
    $xth(me,can) itemconfigure $xth(me,canid,linept,nncp) -state hidden
  }
  if {($ppid > 0) && $xth(me,cmds,$id,$ppid,idn)} {
    $xth(me,can) itemconfigure $xth(me,canid,linept,ppcpl) -state normal
    $xth(me,can) itemconfigure $xth(me,canid,linept,ppcp) -state normal
  } else {
    $xth(me,can) itemconfigure $xth(me,canid,linept,ppcpl) -state hidden
    $xth(me,can) itemconfigure $xth(me,canid,linept,ppcp) -state hidden
  }
  xth_me_cmds_move_lineptcp_xctrl $id $ppid $pid $npid
  $xth(me,can) raise pt$id.$pid
  $xth(me,can) bind $xth(me,canid,linept,pcp) <Enter> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,pcp) -fill yellow"
  $xth(me,can) bind $xth(me,canid,linept,pcp) <Leave> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,pcp) -fill red"
  $xth(me,can) bind $xth(me,canid,linept,pcp) <1> \
    "xth_me_cmds_start_linecp_drag $xth(me,canid,linept,pcp) $id $ppid $pid $npid p %x %y"
  $xth(me,can) bind $xth(me,canid,linept,ncp) <Enter> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill yellow"
  $xth(me,can) bind $xth(me,canid,linept,ncp) <Leave> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill red"
  $xth(me,can) bind $xth(me,canid,linept,ncp) <1> \
    "xth_me_cmds_start_linecp_drag $xth(me,canid,linept,ncp) $id $ppid $pid $npid n %x %y"
  $xth(me,can) bind $xth(me,canid,linept,ppcp) <Enter> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,ppcp) -fill yellow"
  $xth(me,can) bind $xth(me,canid,linept,ppcp) <Leave> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,ppcp) -fill magenta"
  $xth(me,can) bind $xth(me,canid,linept,ppcp) <1> \
    "xth_me_cmds_start_linecp_drag $xth(me,canid,linept,ppcp) $id $ppid $pid $npid pp %x %y"
  $xth(me,can) bind $xth(me,canid,linept,nncp) <Enter> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,nncp) -fill yellow"
  $xth(me,can) bind $xth(me,canid,linept,nncp) <Leave> \
    "$xth(me,can) itemconfigure $xth(me,canid,linept,nncp) -fill magenta"
  $xth(me,can) bind $xth(me,canid,linept,nncp) <1> \
    "xth_me_cmds_start_linecp_drag $xth(me,canid,linept,nncp) $id $ppid $pid $npid nn %x %y"
    
  xth_me_cmds_configure_linept_size_xctrl $id $pid
  xth_me_cmds_move_linept_size_xctrl $id $pid $xth(me,cmds,$id,$pid,rotation) \
    $xth(me,cmds,$id,$pid,rs) $xth(me,cmds,$id,$pid,ls)
  xth_me_cmds_move_line_xctrl $id
    
}


proc xth_me_cmds_hide_linept_xctrl {} {
  global xth
  $xth(me,can) itemconfigure linectrl -state hidden
  $xth(me,can) bind $xth(me,canid,linept,pcp) <Enter> ""
  $xth(me,can) bind $xth(me,canid,linept,pcp) <Leave> ""
  $xth(me,can) bind $xth(me,canid,linept,ncp) <Enter> ""
  $xth(me,can) bind $xth(me,canid,linept,ncp) <Leave> ""
  $xth(me,can) bind $xth(me,canid,linept,ppcp) <Enter> ""
  $xth(me,can) bind $xth(me,canid,linept,ppcp) <Leave> ""
  $xth(me,can) bind $xth(me,canid,linept,nncp) <Enter> ""
  $xth(me,can) bind $xth(me,canid,linept,nncp) <Leave> ""
  $xth(me,can) bind $xth(me,canid,linept,pcp) <1> ""
  $xth(me,can) bind $xth(me,canid,linept,ncp) <1> ""
  $xth(me,can) bind $xth(me,canid,linept,ppcp) <1> ""
  $xth(me,can) bind $xth(me,canid,linept,nncp) <1> ""
  xth_me_cmds_configure_linept_size_xctrl {} 0
}

proc xth_me_cmds_update_linept_ctrl {id pid} {

  global xth
  
  if {[string length $id] > 0} {
    xth_me_cmds_show_line_xctrl $id
  } else {
    xth_me_cmds_hide_line_xctrl
  }

  if {([string length $id] > 0) && ($pid > 0)} {

    $xth(ctrl,me,linept).posl configure -state normal
    $xth(ctrl,me,linept).posx configure -state normal
    $xth(ctrl,me,linept).posy configure -state normal
    $xth(ctrl,me,linept).xp configure -state normal
    $xth(ctrl,me,linept).yp configure -state normal
    $xth(ctrl,me,linept).xn configure -state normal
    $xth(ctrl,me,linept).yn configure -state normal
    $xth(ctrl,me,linept).cbp configure -state normal
    $xth(ctrl,me,linept).cbn configure -state normal
    $xth(ctrl,me,linept).cbs configure -state normal
    $xth(ctrl,me,linept).rotc configure -state normal
    $xth(ctrl,me,linept).rot configure -state normal
    $xth(ctrl,me,linept).rszc configure -state normal
    $xth(ctrl,me,linept).rsz configure -state normal
    $xth(ctrl,me,linept).lszc configure -state normal
    $xth(ctrl,me,linept).lsz configure -state normal
    $xth(ctrl,me,linept).optl configure -state normal
    $xth(ctrl,me,linept).upd configure -state normal

    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Delete point"] -state normal    
    set xpid [lsearch -exact $xth(me,cmds,$id,xplist) $pid]
    if {($xpid > 0) && ($xpid < ([llength $xth(me,cmds,$id,xplist)] - 2))} {
      $xth(ctrl,me,line).lpa.m entryconfigure [mc "Split line"] -state normal
    } else {
      $xth(ctrl,me,line).lpa.m entryconfigure [mc "Split line"] -state disabled
    }

    $xth(ctrl,me,linept).oe.txt configure -state normal
    $xth(ctrl,me,linept).oe.txt delete 1.0 end
    $xth(ctrl,me,linept).oe.txt insert 1.0 $xth(me,cmds,$id,$pid,options)
    $xth(ctrl,me,linept).oe.txt mark set insert $xth(me,cmds,$id,$pid,optpos)
    $xth(ctrl,me,linept).oe.txt see $xth(me,cmds,$id,$pid,optpos)
    
    set xth(ctrl,me,linept,x) $xth(me,cmds,$id,$pid,x)
    set xth(ctrl,me,linept,y) $xth(me,cmds,$id,$pid,y)
    set xth(ctrl,me,linept,idn) $xth(me,cmds,$id,$pid,idn)
    set xth(ctrl,me,linept,idp) $xth(me,cmds,$id,$pid,idp)
    
    if $xth(me,cmds,$id,$pid,idp) {
      set xth(ctrl,me,linept,xp) $xth(me,cmds,$id,$pid,xp)
      set xth(ctrl,me,linept,yp) $xth(me,cmds,$id,$pid,yp)
    } else {
      set xth(ctrl,me,linept,xp) {}
      set xth(ctrl,me,linept,yp) {}
    }
    if $xth(me,cmds,$id,$pid,idn) {
      set xth(ctrl,me,linept,xn) $xth(me,cmds,$id,$pid,xn)
      set xth(ctrl,me,linept,yn) $xth(me,cmds,$id,$pid,yn)
    } else {
      set xth(ctrl,me,linept,xn) {}
      set xth(ctrl,me,linept,yn) {}
    }    
    set xth(ctrl,me,linept,smooth) $xth(me,cmds,$id,$pid,smooth)
    set xth(ctrl,me,linept,rot) $xth(me,cmds,$id,$pid,rotation)
    if {[string length $xth(me,cmds,$id,$pid,rotation)] > 0} {
      set xth(ctrl,me,linept,rotid) 1
    } else {
      set xth(ctrl,me,linept,rotid) 0
    }

    set xth(ctrl,me,linept,rs) $xth(me,cmds,$id,$pid,rs)
    if {[string length $xth(me,cmds,$id,$pid,rs)] > 0} {
      set xth(ctrl,me,linept,rsid) 1
    } else {
      set xth(ctrl,me,linept,rsid) 0
    }

    set xth(ctrl,me,linept,ls) $xth(me,cmds,$id,$pid,ls)
    if {[string length $xth(me,cmds,$id,$pid,ls)] > 0} {
      set xth(ctrl,me,linept,lsid) 1
    } else {
      set xth(ctrl,me,linept,lsid) 0
    }
    
    xth_me_cmds_show_linept_xctrl $id $pid
    
  } else {

    set xth(ctrl,me,linept,x) {}
    set xth(ctrl,me,linept,y) {}
    set xth(ctrl,me,linept,xp) {}
    set xth(ctrl,me,linept,yp) {}
    set xth(ctrl,me,linept,xn) {}
    set xth(ctrl,me,linept,yn) {}
    set xth(ctrl,me,linept,idn) 0
    set xth(ctrl,me,linept,idp) 0
    set xth(ctrl,me,linept,smooth) 0
    set xth(ctrl,me,linept,rot) {}
    set xth(ctrl,me,linept,rotid) 0
    set xth(ctrl,me,linept,rs) {}
    set xth(ctrl,me,linept,rsid) 0
    set xth(ctrl,me,linept,ls) {}
    set xth(ctrl,me,linept,lsid) 0

    $xth(ctrl,me,linept).posl configure -state disabled
    $xth(ctrl,me,linept).posx configure -state disabled
    $xth(ctrl,me,linept).posy configure -state disabled
    $xth(ctrl,me,linept).xp configure -state disabled
    $xth(ctrl,me,linept).upd configure -state disabled
    $xth(ctrl,me,linept).yp configure -state disabled
    $xth(ctrl,me,linept).xn configure -state disabled
    $xth(ctrl,me,linept).yn configure -state disabled
    $xth(ctrl,me,linept).cbp configure -state disabled
    $xth(ctrl,me,linept).cbn configure -state disabled
    $xth(ctrl,me,linept).cbs configure -state disabled
    $xth(ctrl,me,linept).rotc configure -state disabled
    $xth(ctrl,me,linept).rot configure -state disabled
    $xth(ctrl,me,linept).rszc configure -state disabled
    $xth(ctrl,me,linept).rsz configure -state disabled
    $xth(ctrl,me,linept).lszc configure -state disabled
    $xth(ctrl,me,linept).lsz configure -state disabled
    $xth(ctrl,me,linept).optl configure -state disabled
    $xth(ctrl,me,linept).oe.txt configure -state normal
    $xth(ctrl,me,linept).oe.txt delete 1.0 end
    $xth(ctrl,me,linept).oe.txt see 1.0
    $xth(ctrl,me,linept).oe.txt configure -state disabled

    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Delete point"] -state disabled
    $xth(ctrl,me,line).lpa.m entryconfigure [mc "Split line"] -state disabled
    
    xth_me_cmds_hide_linept_xctrl  
    
  }
  
}


proc xth_me_cmds_delete_linept {id pid} {

  global xth
  
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {[string length $pid] < 1} {
    set pid $xth(me,cmds,selpid)
  }
  if {$pid == 0} {
    return
  }

  set oldselpid $xth(me,cmds,selpid)
  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  
  # ak mazeme prvy alebo posledny, tak zrusime close ak je nastaveny
  set closerem 0
  if {$xth(me,cmds,$id,close) && ([llength $xth(me,cmds,$id,xplist)] > 2)} {
    set fpid [lindex $xth(me,cmds,$id,xplist) 0]
    set lpid [lindex $xth(me,cmds,$id,xplist) [expr [llength $xth(me,cmds,$id,xplist)] - 2]]
    if {$pid == $fpid} {
      set closerem 1
    } elseif {$pid == $lpid} {
      set closerem 1
    }
  }
  
  if {$closerem} {
    set closeremstr "set xth(me,cmds,$id,close) 0\nxth_me_cmds_update_line_vars $id $pid"
    set closeaddstr "set xth(me,cmds,$id,close) 1\nxth_me_cmds_update_line_vars $id $pid"
  } else {
    set closeremstr {}
    set closeaddstr {}
  }
  
  eval $closeremstr
  
  # odstrani ho zo zoznamu
  set ix [lsearch $xth(me,cmds,$id,xplist) $pid]
  set xth(me,cmds,$id,xplist) [lreplace $xth(me,cmds,$id,xplist) $ix $ix]
  set xth(me,cmds,$id,plist) [lreplace $xth(me,cmds,$id,plist) $ix $ix]
  $xth(me,can) delete pt$id.$pid
  $xth(me,can) delete ln$id.$pid
  xth_me_cmds_move_line $id
  if {$oldselpid == $pid} {
    set nwpid [lindex $xth(me,cmds,$id,xplist) $ix]
  } else {
    set nwpid $oldselpid
  }
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  
  # BUG FIX when deleting last point
  if {$nwpid == 0} {
    set xth(me,cmds,selpid) $nwpid
  }
  xth_me_cmds_select_linept $id $nwpid
    
  xth_me_unredo_action [mc "deleting line point"] \
    "xth_me_cmds_undelete_linept $id $pid $ix\n$closeaddstr" \
    "$closeremstr\nxth_me_cmds_delete_linept $id $pid"
  
}


proc xth_me_cmds_undelete_linept {id pid ix} {
  global xth
  set oldpid [lindex $xth(me,cmds,$id,xplist) $ix]
  set xth(me,cmds,$id,xplist) [linsert $xth(me,cmds,$id,xplist) $ix $pid]
  set xth(me,cmds,$id,plist) [linsert $xth(me,cmds,$id,plist) $ix {}]
  xth_me_cmds_update_linept_list $id $pid
  if {$ix > 0} {
    set ppid [lindex $xth(me,cmds,$id,xplist) [expr $ix - 1]]
  } else {
    set ppid 0
  }
  xth_me_cmds_draw_linept $id $pid
  xth_me_cmds_draw_lineln $id $ppid $pid 
  xth_me_cmds_move_line $id
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  xth_me_cmds_select_linept $id $pid
  
}


proc xth_me_cmds_create_line_point {id ix mode x y xp yp xn yn smooth rot rs ls opts optpos} {

  global xth
  
  incr xth(me,cmds,$id,lpid)
  set pid $xth(me,cmds,$id,lpid)
  set xth(me,cmds,$id,$pid,x) [expr double($x)]
  set xth(me,cmds,$id,$pid,y) [expr double($y)]

  if {([string length $xp] > 0) && ((![string equal $xp $x]) || (![string equal $yp $y]))} {
    set xth(me,cmds,$id,$pid,xp) [expr double($xp)]
    set xth(me,cmds,$id,$pid,yp) [expr double($yp)]
    set xth(me,cmds,$id,$pid,idp) 1
  } else {
    set xth(me,cmds,$id,$pid,xp) {}
    set xth(me,cmds,$id,$pid,yp) {}
    set xth(me,cmds,$id,$pid,idp) 0
  }
  
  if {([string length $xn] > 0) && ((![string equal $xn $x]) || (![string equal $yn $y]))} {
    set xth(me,cmds,$id,$pid,xn) [expr double($xn)]
    set xth(me,cmds,$id,$pid,yn) [expr double($yn)]
    set xth(me,cmds,$id,$pid,idn) 1
  } else {
    set xth(me,cmds,$id,$pid,xn) {}
    set xth(me,cmds,$id,$pid,yn) {}
    set xth(me,cmds,$id,$pid,idn) 0
  }
  
  if {[string length $smooth] < 1} {
    set xth(me,cmds,$id,$pid,smooth) -1
  } else {
    set xth(me,cmds,$id,$pid,smooth) $smooth
  }
  
  if {[string length $rot] > 0} {
    set rot [expr double($rot)]
  }
  set xth(me,cmds,$id,$pid,rotation) $rot

  if {[string length $ls] > 0} {
    set ls [expr double($ls)]
  }
  set xth(me,cmds,$id,$pid,ls) $ls

  if {[string length $rs] > 0} {
    set rs [expr double($rs)]
  }
  set xth(me,cmds,$id,$pid,rs) $rs
  
  regsub {\s*$} $opts "" opts
  if {[string length $opts] > 0} {
    regsub {$} $opts "\n" opts
  }
  set xth(me,cmds,$id,$pid,options) $opts
  set xth(me,cmds,$id,$pid,optpos) $optpos
  
  # vlozi ho do zoznamu
  set xth(me,cmds,$id,plist) [linsert $xth(me,cmds,$id,plist) $ix {}]
  set xth(me,cmds,$id,xplist) [linsert $xth(me,cmds,$id,xplist) $ix $pid]
  xth_me_cmds_update_linept_list $id $pid
  
  # vytvori ho
  if {$ix > 0} {
    set ppid [lindex $xth(me,cmds,$id,xplist) [expr $ix - 1]]
  } else {
    set ppid 0
  }
  xth_me_cmds_draw_lineln $id $ppid $pid
  xth_me_cmds_draw_linept $id $pid
  
}



proc xth_me_cmds_create_area_line {id ix mode txt} {

  global xth
  incr xth(me,cmds,$id,llid)
  set lid $xth(me,cmds,$id,llid)
  regsub {\s*$} $txt "" txt
  regsub {^\s*} $txt "" txt
  set xth(me,cmds,$id,$lid,txt) $txt
  # vlozi ho do zoznamu
  set xth(me,cmds,$id,llist) [linsert $xth(me,cmds,$id,llist) $ix $txt]
  set xth(me,cmds,$id,xllist) [linsert $xth(me,cmds,$id,xllist) $ix $lid]
  
}


proc xth_me_cmds_insert_area_lineid {id mx my} {
  global xth
  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  if {[string length $xth(me,cmds,$id,name)] == 0} {
    set nn [format "l%d-%.0f-%.0f" $id [xth_me_can2realx [$xth(me,can) canvasx $mx]] [xth_me_can2realy [$xth(me,can) canvasy $my]]]
    set unspec "set xth(me,cmds,$id,name) {}\nxth_me_cmds_update_line_data $id\nxth_me_cmds_update_list $id"
    set respec "set xth(me,cmds,$id,name) [list $nn]\nxth_me_cmds_update_line_data $id\nxth_me_cmds_update_list $id"
    eval $respec
  } else {
    set nn $xth(me,cmds,$id,name)
    set unspec {}
    set respec {}
  }    
  xth_me_cmds_insert_area_line $nn $unspec $respec
}


proc xth_me_cmds_insert_area_line {txt unspec respec} {

  global xth
  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  regsub {\s*$} $txt "" txt
  regsub {^\s*} $txt "" txt  
  if {[string length $txt] == 0} {
    return;
  }
  set id $xth(me,cmds,selid)
  set lid [lindex $xth(me,cmds,$id,xllist) [$xth(ctrl,me,ac).ll.l curselection]]
  set ix [lsearch $xth(me,cmds,$id,xllist) $lid]
  xth_me_cmds_create_area_line $id $ix 1 $txt
  set lid $xth(me,cmds,$id,llid)
  xth_me_cmds_update_area_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  $xth(ctrl,me,ac).ll.l selection clear 0 end
  $xth(ctrl,me,ac).ll.l selection set [expr $ix + 1] [expr $ix + 1]
  $xth(ctrl,me,ac).ll.l see [expr $ix + 1]

  xth_me_unredo_action [mc "inserting area border"] \
    "xth_me_cmds_select $id\nxth_me_cmds_delete_area_line $id $lid\n$unspec" \
    "xth_me_cmds_select $id\n$respec\nxth_me_cmds_undelete_area_line 1 $id $lid $ix"
      
}


proc xth_me_cmds_delete_area_line {id lid} {

  global xth
  
  if {[string length $id] < 1} {
    set id $xth(me,cmds,selid)
  }
  if {[string length $lid] < 1} {
    set lid [lindex $xth(me,cmds,$id,xllist) [$xth(ctrl,me,ac).ll.l curselection]]
  }
  if {$lid == 0} {
    return
  }

  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  
  # odstrani ho zo zoznamu
  set ix [lsearch $xth(me,cmds,$id,xllist) $lid]
  set xth(me,cmds,$id,xllist) [lreplace $xth(me,cmds,$id,xllist) $ix $ix]
  set xth(me,cmds,$id,llist) [lreplace $xth(me,cmds,$id,llist) $ix $ix]
  xth_me_cmds_update_area_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  $xth(ctrl,me,ac).ll.l selection clear 0 end
  $xth(ctrl,me,ac).ll.l selection set $ix $ix
  $xth(ctrl,me,ac).ll.l see $ix
    
  xth_me_unredo_action [mc "deleting area border"] \
    "xth_me_cmds_undelete_area_line 0 $id $lid $ix" \
    "xth_me_cmds_delete_area_line $id $lid"
  
}


proc xth_me_cmds_undelete_area_line {cr id lid ix} {

  global xth
  set xth(me,cmds,$id,xllist) [linsert $xth(me,cmds,$id,xllist) $ix $lid]
  set xth(me,cmds,$id,llist) [linsert $xth(me,cmds,$id,llist) $ix $xth(me,cmds,$id,$lid,txt)]
  xth_me_cmds_update_area_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  $xth(ctrl,me,ac).ll.l selection clear 0 end
  $xth(ctrl,me,ac).ll.l selection set [expr $ix + $cr] [expr $ix + $cr]
  $xth(ctrl,me,ac).ll.l see [expr $ix + $cr]
  
}



proc xth_me_cmds_create_area {ix mode type opts lines} {

  global xth
  if {$mode} {
    xth_me_cmds_update {}
  }
  set id [xth_me_cmds_create 6 {} $ix]
  set xth(me,cmds,$id,llid) 0
  set xth(me,cmds,$id,llist) {"end of area"}
  set xth(me,cmds,$id,xllist) {0}

  if {$mode && ([string length $opts] < 1)} {
    set opts $xth(me,dflt,area,options)
  }

  if {[string length $type] > 0} {
    set xth(me,cmds,$id,type) $type
  } else {
    set xth(me,cmds,$id,type) $xth(me,dflt,area,type)
  }  

  foreach ln $lines {
    xth_me_cmds_create_area_line $id \
	  [expr [llength $xth(me,cmds,$id,xllist)] - 1] $mode $ln
  }
  
  # nastavit options
  regsub {^\s*} $opts "" opts
  regsub {\s*$} $opts "" opts
  set xth(me,cmds,$id,options) $opts

  xth_me_cmds_update_list $id
  xth_me_cmds_update_area_data $id
  if {$mode} {
    xth_me_unredo_action [mc "creating area"] \
      "xth_me_cmds_delete $id\nxth_me_cmds_set_mode 0" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]\nxth_me_cmds_set_mode 3"  
    xth_me_cmds_select $id
    xth_me_cmds_start_area_insert 0
  }


}




proc xth_me_cmds_create_line {ix mode type opts lines} {

  global xth
  if {$mode} {
    xth_me_cmds_update {}
  }
  set id [xth_me_cmds_create 3 {} $ix]
  set xth(me,cmds,$id,lpid) 0
  set xth(me,cmds,$id,plist) {"end of line"}
  set xth(me,cmds,$id,xplist) {0}

  if {$mode && ([string length $opts] < 1)} {
    set opts $xth(me,dflt,line,options)
  }

  if {[string length $type] > 0} {
    set xth(me,cmds,$id,type) $type
  } else {
    set xth(me,cmds,$id,type) $xth(me,dflt,line,type)
  }
  
  # nastavit meno
  set optl [xth_me_cmds_get_line_option $opts id]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,name) [lindex $optl 0]
    set opts [lindex $optl 1]
  }
  # prejde vsetky riadky a pohlada meno tam
  set newlines {}
  foreach ln $lines {
    set optl [xth_me_cmds_get_option $ln id]
    if {[lindex $optl 2]} {
      set xth(me,cmds,$id,name) [lindex $optl 0]
      set ln [lindex $optl 1]
      if {[string length $ln] > 0} {
	lappend newlines $ln
      }
    } else {
      lappend newlines $ln
    }
  }
  set lines $newlines
  
  # nastavit reversion
  set xth(me,cmds,$id,reverse) 0
  set optl [xth_me_cmds_get_line_option $opts reverse]
  if {[lindex $optl 2]} {
    #puts ">>$opts<< = >>$optl<<"
    set xth(me,cmds,$id,reverse) [xth_me_cmds_get_bool [lindex $optl 0]]
    set opts [lindex $optl 1]
    #puts "$xth(me,cmds,$id,reverse) >>$opts<<"
  }
  # prejde vsetky riadky a pohlada reverse tam
  set newlines {}
  foreach ln $lines {
    set optl [xth_me_cmds_get_option $ln reverse]
    if {[lindex $optl 2]} {
      set xth(me,cmds,$id,reverse) [xth_me_cmds_get_bool [lindex $optl 0]]
      set ln [lindex $optl 1]
      if {[string length $ln] > 0} {
	lappend newlines $ln
      }
    } else {
      lappend newlines $ln
    }
  }
  set lines $newlines
  
  # nastavit close
  set xth(me,cmds,$id,close) -1
  set optl [xth_me_cmds_get_line_option $opts close]
  if {[lindex $optl 2]} {
    set xth(me,cmds,$id,close) [xth_me_cmds_get_onoffauto [lindex $optl 0]]
    set opts [lindex $optl 1]
  }
  # prejde vsetky riadky a pohlada reverse tam
  set newlines {}
  foreach ln $lines {
    set optl [xth_me_cmds_get_option $ln close]
    if {[lindex $optl 2]} {
      set xth(me,cmds,$id,close) [xth_me_cmds_get_onoffauto [lindex $optl 0]]
      set ln [lindex $optl 1]
      if {[string length $ln] > 0} {
	lappend newlines $ln
      }
    } else {
      lappend newlines $ln
    }
  }
  set lines $newlines
  
  # nastavit options
  regsub {^\s*} $opts "" opts
  regsub {\s*$} $opts "" opts
  set xth(me,cmds,$id,options) $opts

  # vlozi body
  set has_some 0
  set opts {}
  set rsz {}
  set lsz {}
  set smth {}
  set rot {}
  foreach ln $lines {
    set what 0
    set cmt 0
    if {[regexp {^\s*\!?\s*(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s*(\#.*)?$} \
	  $ln dum x1 y1 x2 y2 x y cmt] && (![catch {expr $x * $y * $x1 * $y1 * $x2 * $y2}])} {
      set what 2
    } elseif {[regexp {^\s*\!?\s*(\S+)\s+(\S+)\s*(\#.*)?$} $ln dum x y cmt] && (![catch {expr $x * $y}])} {
      set x1 {}
      set y1 {}
      set x2 {}
      set y2 {}
      set what 1
    } elseif {$has_some} {
      # skusi jednotlive options rotation size r-size l-size smooth
      set cmt $ln
      set optl [xth_me_cmds_get_option $ln orientation]
      if {[lindex $optl 2]} {
	set rot [lindex $optl 0]
	set cmt [lindex $optl 1]
      } else {      
	set optl [xth_me_cmds_get_option $ln orient]
	if {[lindex $optl 2]} {
	  set rot [lindex $optl 0]
	  set cmt [lindex $optl 1]
	} else {
	  set optl [xth_me_cmds_get_option $ln smooth]
	  if {[lindex $optl 2]} {
	    set smth [xth_me_cmds_get_onoffauto [lindex $optl 0]]
	    set cmt [lindex $optl 1]
	  } else {
	    set optl [xth_me_cmds_get_option $ln size]
	    if {[lindex $optl 2]} {
	      switch $xth(me,cmds,$id,type) {
		slope {
		  set lsz [expr [lindex $optl 0]]
		}
		default {
		  set rsz [expr [lindex $optl 0] / 2.0]
		  set lsz [expr [lindex $optl 0] / 2.0]
		}
	      }
	      set cmt [lindex $optl 1]
	    } else {
	      set optl [xth_me_cmds_get_option $ln r-size]
	      if {[lindex $optl 2]} {
		set rsz [lindex $optl 0]
		set cmt [lindex $optl 1]
	      } else {
		set optl [xth_me_cmds_get_option $ln l-size]
		if {[lindex $optl 2]} {
		  set lsz [lindex $optl 0]
		  set cmt [lindex $optl 1]
		}
	      }
	    }
	  }
	}
      }
      regsub {^\s*} $cmt {} cmt
    } else {
      regsub {^\s*} $ln {} ln
      set cmt $ln
    }
    if {$what == 0} {
      if {[string length $cmt] > 0} {
	if {[string length $opts] > 0} {
	  set opts "$opts\n$cmt"
	} else {
	  set opts "$cmt"
	}
      }
    } else {
      if {$has_some} {
	xth_me_cmds_create_line_point $id \
	  [expr [llength $xth(me,cmds,$id,xplist)] - 1] $mode \
	  $px $py $px2 $py2 $x1 $y1 $smth $rot $rsz $lsz $opts 1.0
	set opts {}
	set rsz {}
	set lsz {}
	set smth {}
	set rot {}
      }
      set has_some 1
      set px $x
      set py $y
      set px1 $x1
      set py1 $y1
      set px2 $x2
      set py2 $y2
    }
  }
  if {$has_some} {
    xth_me_cmds_create_line_point $id \
      [expr [llength $xth(me,cmds,$id,xplist)] - 1] $mode \
      $px $py $px2 $py2 {} {} $smth $rot $rsz $lsz $opts 1.0
  }

  if {$mode == 1} {
    set xth(me,cmds,$id,close) 0
  } else {
    xth_me_cmds_postprocess_line $id
  }
  xth_me_cmds_update_list $id
  xth_me_cmds_update_line_data $id
  if {$mode} {
    xth_me_unredo_action [mc "creating line"] \
      "xth_me_cmds_delete $id\nxth_me_cmds_set_mode 0" \
      "xth_me_cmds_undelete $id 0 [lsearch $xth(me,cmds,xlist) $id]\nxth_me_cmds_set_mode 2"  
    xth_me_cmds_select $id
    xth_me_cmds_start_linept_insert
  }
}


proc xth_me_cmds_postprocess_line {id} {
  global xth
  set xl $xth(me,cmds,$id,xplist)
  set lix [expr [llength $xl] - 2]
  if {$lix < 0} {
    return
  }

  # overi uzavretie
  set postclose 0
  if {$xth(me,cmds,$id,close) != 0} {
      if {($xth(me,cmds,$id,[lindex $xl 0],x) == $xth(me,cmds,$id,[lindex $xl $lix],x)) && \
	($xth(me,cmds,$id,[lindex $xl 0],y) == $xth(me,cmds,$id,[lindex $xl $lix],y)) && \
	($lix > 0)} {
	set xth(me,cmds,$id,close) 1
      } else {
        if {$xth(me,cmds,$id,close) == 1} {
          set postclose 1
        }
        set xth(me,cmds,$id,close) 0
      }
  }
  
  if {$xth(me,cmds,$id,close)} {
    set fid [lindex $xl 0]
    set lid [lindex $xl $lix]
    if {$xth(me,cmds,$id,$fid,idn)} {
      set xth(me,cmds,$id,$lid,idn) $xth(me,cmds,$id,$fid,idn)
      set xth(me,cmds,$id,$lid,xn) $xth(me,cmds,$id,$fid,xn)
      set xth(me,cmds,$id,$lid,yn) $xth(me,cmds,$id,$fid,yn)
    }
    if {$xth(me,cmds,$id,$lid,idp)} {
      set xth(me,cmds,$id,$fid,idp) $xth(me,cmds,$id,$lid,idp)
      set xth(me,cmds,$id,$fid,xp) $xth(me,cmds,$id,$lid,xp)
      set xth(me,cmds,$id,$fid,yp) $xth(me,cmds,$id,$lid,yp)
    }
  }

  # overi smoothness
  if {$lix == 0} {
    set pid [lindex $xth(me,cmds,$id,xplist) 0]
    if {($xth(me,cmds,$id,$pid,smooth) == -1) && \
	$xth(me,cmds,$id,$pid,idp) && \
	$xth(me,cmds,$id,$pid,idn)} {
      set xth(me,cmds,$id,$pid,smooth) [xth_me_cmds_are_smooth \
	$xth(me,cmds,$id,$pid,xp) $xth(me,cmds,$id,$pid,yp) \
	$xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y) \
	$xth(me,cmds,$id,$pid,xn) $xth(me,cmds,$id,$pid,yn)]
    } elseif {$xth(me,cmds,$id,$pid,smooth) == -1} {
      set xth(me,cmds,$id,$pid,smooth) 0
    }
    return
  }
  
  # prejde vsetky body
  set lid [lindex $xl $lix]
  set fid [lindex $xl 0]
  set cls $xth(me,cmds,$id,close)
  for {set ix 0} {$ix <= $lix} {incr ix} {
    set pid [lindex $xl $ix]
    if {($ix == 0) && $cls} {
      set idp $xth(me,cmds,$id,$lid,idp)
    } else {
      set idp $xth(me,cmds,$id,$pid,idp)
    }
    if {($ix == $lix) && $cls} {
      set idn $xth(me,cmds,$id,$fid,idn)
    } else {
      set idn $xth(me,cmds,$id,$pid,idn)
    }
    if {$idn && $idp && ($xth(me,cmds,$id,$pid,smooth) == -1)} {
      set x $xth(me,cmds,$id,$pid,x)
      set y $xth(me,cmds,$id,$pid,y)
      if {($ix == 0) && $cls} {
	set xp $xth(me,cmds,$id,$lid,xp)
	set yp $xth(me,cmds,$id,$lid,yp)
      } else {
	set xp $xth(me,cmds,$id,$pid,xp)
	set yp $xth(me,cmds,$id,$pid,yp)
      }
      if {($ix == $lix) && $cls} {
	set xn $xth(me,cmds,$id,$fid,xn)
	set yn $xth(me,cmds,$id,$fid,yn)
      } else {
	set xn $xth(me,cmds,$id,$pid,xn)
	set yn $xth(me,cmds,$id,$pid,yn)
      }
      set xth(me,cmds,$id,$pid,smooth) [xth_me_cmds_are_smooth $xp $yp $x $y $xn $yn]
    } elseif {$xth(me,cmds,$id,$pid,smooth) == -1} {
      set xth(me,cmds,$id,$pid,smooth) 0
    }
  }
  
  if {$postclose} {
    xth_me_cmds_close_line $id 0
  }
  
}


proc xth_me_cmds_update_area_data {id} {

  global xth
  set xl $xth(me,cmds,$id,xllist)
  set lix [expr [llength $xl] - 2]

  set d "area $xth(me,cmds,$id,type)"
  set xth(me,dflt,area,type) $xth(me,cmds,$id,type)

  # options
  if {[string length $xth(me,cmds,$id,options)] > 0} {
    set d "$d $xth(me,cmds,$id,options)"
  }
  set xth(me,dflt,area,options) $xth(me,cmds,$id,options)

  for {set ix 0} {$ix <= $lix} {incr ix} {
    set lid [lindex $xl $ix]
    set d "$d\n  $xth(me,cmds,$id,$lid,txt)"
  }

  set xth(me,cmds,$id,data) "$d\nendarea"
  
}



proc xth_me_cmds_update_line_data {id} {
  global xth
  set xl $xth(me,cmds,$id,xplist)
  set lix [expr [llength $xl] - 2]

  set d "line $xth(me,cmds,$id,type)"
  set xth(me,dflt,line,type) $xth(me,cmds,$id,type)

  # id
  if {[string length $xth(me,cmds,$id,name)] > 0} {
    set d "$d -id $xth(me,cmds,$id,name)"
  }
  
  if {$lix > -1} {
  
    # close
    if {$xth(me,cmds,$id,close)} {
      set d "$d -close on"
    } elseif {($xth(me,cmds,$id,[lindex $xl 0],x) == $xth(me,cmds,$id,[lindex $xl $lix],x)) && \
	($xth(me,cmds,$id,[lindex $xl 0],y) == $xth(me,cmds,$id,[lindex $xl $lix],y))} {
      set d "$d -close off"
    }

    # reverse
    if {$xth(me,cmds,$id,reverse)} {
      set d "$d -reverse on"
    }
    
  }
  
  if {[string length $xth(me,cmds,$id,options)] > 0} {
    set d "$d $xth(me,cmds,$id,options)"
  }
  set xth(me,dflt,line,options) $xth(me,cmds,$id,options)
  
  set csbar $d
  
  set xth(me,cmds,$id,data_ln) "$d\nendline"
  set xth(me,cmds,$id,data_pt) {}
  
  for {set ix 0} {$ix <= $lix} {incr ix} {
    set cto 0
    set pid [lindex $xl $ix]
    if {$ix == 0} {
      set crd [format "  %s %s" $xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y)]
    } else {
      if {$xth(me,cmds,$id,$pid,idp) || $xth(me,cmds,$id,$ppid,idn)} {
	if {$xth(me,cmds,$id,$ppid,idn)} {
	  set x1 $xth(me,cmds,$id,$ppid,xn)
	  set y1 $xth(me,cmds,$id,$ppid,yn)
	} else {
	  set x1 $xth(me,cmds,$id,$ppid,x)
	  set y1 $xth(me,cmds,$id,$ppid,y)
	}
	if {$xth(me,cmds,$id,$pid,idp)} {
	  set x2 $xth(me,cmds,$id,$pid,xp)
	  set y2 $xth(me,cmds,$id,$pid,yp)
	} else {
	  set x2 $xth(me,cmds,$id,$pid,x)
	  set y2 $xth(me,cmds,$id,$pid,y)
	}
	set crd [format "  %s %s %s %s" $x1 $y1 $x2 $y2]
	set cto 1
      } else {
	set crd " "
      }
      set crd "$crd [format "%s %s" $xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y)]"
    }
    set ptd "$crd"
    # rotation lsize rsize smooth options
    if {[string length $xth(me,cmds,$id,$pid,rotation)] > 0} {
      set ptd "$ptd\n  orientation $xth(me,cmds,$id,$pid,rotation)"
    }

    if {$cto && (! $xth(me,cmds,$id,$pid,smooth))} {
      set ptd "$ptd\n  smooth off"
    }
    
    if {[string length $xth(me,cmds,$id,$pid,ls)] > 0} {
      set ptd "$ptd\n  l-size $xth(me,cmds,$id,$pid,ls)"
    }

    if {[string length $xth(me,cmds,$id,$pid,rs)] > 0} {
      set ptd "$ptd\n  r-size $xth(me,cmds,$id,$pid,rs)"
    }
    
    if {[string length $xth(me,cmds,$id,$pid,options)] > 0} {
      set popts $xth(me,cmds,$id,$pid,options)
      regsub {\s*$} $popts {} popts
      if {[string length $popts] > 0} {
	regsub -all -line {^\s*} $popts {  } popts
      }
      set ptd "$ptd\n$popts"
    }
    
    lappend xth(me,cmds,$id,data_pt) $ptd
    
    set d "$d\n$ptd"
    set ppid $pid
  }

  set xth(me,cmds,$id,data) "$d\nendline"
  set xth(me,cmds,$id,sbar) $csbar
  catch {
  set ttype [lindex $csbar 1]
  if {([string length $ttype] > 0)} {
    set ttyp [ mc "line $ttype"]
    if {![string equal $ttype $ttyp]} {
      set nd [lreplace $csbar 1 1 "$ttype:$ttyp"]
      set xth(me,cmds,$id,sbar) "$nd"
    }
  }
  } 
  
}

proc xth_me_cmds_are_smooth {xp yp x y xn yn} {
  set rv 1
  set d1 [expr hypot($x - $xp,$y - $yp)]
  set d2 [expr hypot($xn - $x,$yn - $y)]
  if {($d2 > 0) && ($d1 > 0)} {
    set a1 [expr atan2($y - $yp, $x - $xp) / 3.14159265359 * 180.0];                        
    set a2 [expr atan2($yn - $y, $xn - $x) / 3.14159265359 * 180.0];
    if {($a2 - $a1) > 180.0} {
      set a2 [expr $a2 - 360.0]
    }
    if {($a2 - $a1) < -180.0} {
      set a2 [expr $a2 + 360.0]
    }
    if {($a2 > ($a1 + 5.0)) || ($a2 < ($a1 - 5.0))} {
      set rv 0
    }
  }        
  return $rv
}


proc xth_me_cmds_select_linept {id pid} {
  global xth
  if {$xth(me,unredook)} {
    xth_me_cmds_update {}
  }
  xth_me_cmds_update_linept_ctrl $id $pid
  set ix [lsearch $xth(me,cmds,$id,xplist) $pid]
  $xth(ctrl,me,line).pl.l selection clear 0 end
  $xth(ctrl,me,line).pl.l selection set $ix $ix
  $xth(ctrl,me,line).pl.l see $ix
  set xth(me,cmds,selpid) $pid
}


proc xth_me_cmds_toggle_line_close {} {
  global xth
  set id $xth(me,cmds,selid)
  set ncls $xth(ctrl,me,line,close)
  set xth(ctrl,me,line,close_tmp) $xth(ctrl,me,line,close)
  set xth(ctrl,me,line,close) $xth(me,cmds,$id,close)
  $xth(ctrl,me,line).cls configure -variable xth(ctrl,me,line,close_tmp)
  xth_me_cmds_update {}
  set xth(ctrl,me,line,close) $ncls
  if {$ncls && (!$xth(me,cmds,$id,close))} {
    xth_me_cmds_close_line $id
  } elseif {(!$ncls) && $xth(me,cmds,$id,close)} {
    xth_me_cmds_open_line $id
  }
  $xth(ctrl,me,line).cls configure -variable xth(ctrl,me,line,close)

}


proc xth_me_cmds_toggle_line_reverse {} {
  global xth
  set id $xth(me,cmds,selid)
  set nrev $xth(ctrl,me,line,reverse)
  set xth(ctrl,me,line,reverse_tmp) $xth(ctrl,me,line,reverse)
  set xth(ctrl,me,line,reverse) $xth(me,cmds,$id,reverse)
  $xth(ctrl,me,line).rev configure -variable xth(ctrl,me,line,reverse_tmp)
  xth_me_cmds_update {}
  set xth(ctrl,me,line,reverse) $nrev
  set xth(me,unredola) "line reversion"
  xth_me_cmds_update {}
  if {[llength $xth(me,cmds,$id,xplist)] > 1} {
    xth_me_cmds_move_lineln $id [lindex $xth(me,cmds,$id,xplist) 0] [lindex $xth(me,cmds,$id,xplist) 1]
  }
  $xth(ctrl,me,line).rev configure -variable xth(ctrl,me,line,reverse)
}


proc xth_me_cmds_toggle_linept {} {

  global xth

  set nidp $xth(ctrl,me,linept,idp)
  set xth(ctrl,me,linept,idp_tmp) $nidp
  set nidn $xth(ctrl,me,linept,idn)
  set xth(ctrl,me,linept,idn_tmp) $nidn
  set nsmooth $xth(ctrl,me,linept,smooth)
  set xth(ctrl,me,linept,smooth_tmp) $nsmooth
  set nrotid $xth(ctrl,me,linept,rotid)
  set xth(ctrl,me,linept,rotid_tmp) $nrotid
  set nrsid $xth(ctrl,me,linept,rsid)
  set xth(ctrl,me,linept,rsid_tmp) $nrsid
  set nlsid $xth(ctrl,me,linept,lsid)
  set xth(ctrl,me,linept,lsid_tmp) $nlsid

  set lpc $xth(ctrl,me,linept)
  $lpc.cbp configure -variable xth(ctrl,me,linept,idp_tmp)
  $lpc.cbn configure -variable xth(ctrl,me,linept,idn_tmp)
  $lpc.cbs configure -variable xth(ctrl,me,linept,smooth_tmp)
  $lpc.rotc configure -variable xth(ctrl,me,linept,rotid_tmp)
  $lpc.rszc configure -variable xth(ctrl,me,linept,rsid_tmp)
  $lpc.lszc configure -variable xth(ctrl,me,linept,lsid_tmp)

  update idletasks
  xth_me_cmds_update {}
  
  set id $xth(me,cmds,selid)
  set pid $xth(me,cmds,selpid)

  if {(!$nidp) && $xth(me,cmds,$id,$pid,idp)} {
    set xth(ctrl,me,linept,xp) {}
    set xth(ctrl,me,linept,yp) {}
    set xth(ctrl,me,linept,smooth) 0
  } elseif {$nidp && (!$xth(me,cmds,$id,$pid,idp))} {
    set crds [xth_me_cmds_get_default_linept_cp 1 $id $pid]
    set xth(ctrl,me,linept,xp) [lindex $crds 0]
    set xth(ctrl,me,linept,yp) [lindex $crds 1]
  }

  if {(!$nidn) && $xth(me,cmds,$id,$pid,idn)} {
    set xth(ctrl,me,linept,xn) {}
    set xth(ctrl,me,linept,yn) {}
    set xth(ctrl,me,linept,smooth) 0
  } elseif {$nidn && (!$xth(me,cmds,$id,$pid,idn))} {
    set crds [xth_me_cmds_get_default_linept_cp 0 $id $pid]
    set xth(ctrl,me,linept,xn) [lindex $crds 0]
    set xth(ctrl,me,linept,yn) [lindex $crds 1]
  }

  if {$nsmooth != $xth(me,cmds,$id,$pid,smooth)} {
    if {$nsmooth} {
      if {!$xth(me,cmds,$id,$pid,idp)} {
	set crds [xth_me_cmds_get_default_linept_cp 1 $id $pid]
	set xth(ctrl,me,linept,xp) [lindex $crds 0]
	set xth(ctrl,me,linept,yp) [lindex $crds 1]
      }
      if {!$xth(me,cmds,$id,$pid,idn)} {
	set crds [xth_me_cmds_get_default_linept_cp 0 $id $pid]
	set xth(ctrl,me,linept,xn) [lindex $crds 0]
	set xth(ctrl,me,linept,yn) [lindex $crds 1]
      }
    }
    set xth(ctrl,me,linept,smooth) $nsmooth
  }
  
  if {(!$nrotid) && ([string length $xth(me,cmds,$id,$pid,rotation)] > 0)} {
    set xth(ctrl,me,linept,rot) {}
  } elseif {$nrotid && ([string length $xth(me,cmds,$id,$pid,rotation)] < 1)} {
    set xth(ctrl,me,linept,rot) [xth_me_cmds_get_default_rotation $id $pid]
  }
  
  if {(!$nrsid) && ([string length $xth(me,cmds,$id,$pid,rs)] > 0)} {
    set xth(ctrl,me,linept,rs) {}
  } elseif {$nrsid && ([string length $xth(me,cmds,$id,$pid,rs)] < 1)} {
    set xth(ctrl,me,linept,rs) 40.0
  }
  
  if {(!$nlsid) && ([string length $xth(me,cmds,$id,$pid,ls)] > 0)} {
    set xth(ctrl,me,linept,ls) {}
  } elseif {$nlsid && ([string length $xth(me,cmds,$id,$pid,ls)] < 1)} {
    set xth(ctrl,me,linept,ls) 40.0
  }
  
  xth_me_cmds_update {}

  $lpc.cbp configure -variable xth(ctrl,me,linept,idp)
  $lpc.cbn configure -variable xth(ctrl,me,linept,idn)
  $lpc.cbs configure -variable xth(ctrl,me,linept,smooth)
  $lpc.rotc configure -variable xth(ctrl,me,linept,rotid)
  $lpc.rszc configure -variable xth(ctrl,me,linept,rsid)
  $lpc.lszc configure -variable xth(ctrl,me,linept,lsid)
  update idletasks
}

proc xth_me_cmds_update_line {id pid ntype nname nopts nrev nx ny nxp nyp \
  nxn nyn nsmth nrot nrs nls nptopts nptoptpos} {

  global xth
  
  set otype $xth(me,cmds,$id,type)
  set oname $xth(me,cmds,$id,name)
  set oopts $xth(me,cmds,$id,options)
  set orev $xth(me,cmds,$id,reverse)
  set oline "$otype $oname $oopts $orev"
  
  if {$pid > 0} {
    set ox $xth(me,cmds,$id,$pid,x)
    set oy $xth(me,cmds,$id,$pid,y)
    set oxp $xth(me,cmds,$id,$pid,xp)
    set oyp $xth(me,cmds,$id,$pid,yp)
    set oxn $xth(me,cmds,$id,$pid,xn)
    set oyn $xth(me,cmds,$id,$pid,yn)
    set osmth $xth(me,cmds,$id,$pid,smooth)
    set orot $xth(me,cmds,$id,$pid,rotation)
    set ors $xth(me,cmds,$id,$pid,rs)
    set ols $xth(me,cmds,$id,$pid,ls)
    set optopts $xth(me,cmds,$id,$pid,options)
    set optoptpos $xth(me,cmds,$id,$pid,optpos)
    set olinept "$ox $oy $oxp $oyp $oxn $oyn $osmth $orot $ors $ols $optopts"
  } else {
    set ox {}
    set oy {}
    set oxp {}
    set oyp {}
    set oxn {}
    set oyn {}
    set osmth {}
    set orot {}
    set ors {}
    set ols {}
    set optopts {}
    set optoptpos {}
    set olinept ""
  }


  if {[string length $ntype] < 1} {
    set ntype $otype
  }

  set optsredo {}
  set optsundo {}

  if {(![string equal $ntype $otype]) && [string equal $nopts $oopts]} {
    set nopts {}
    set nrs {}
    set nls {}
    set nrot {} 
    foreach xpid $xth(me,cmds,$id,xplist) {
      if {$xpid > 0} {
	foreach item {rs ls rotation} {
	  if {[string length $xth(me,cmds,$id,$xpid,$item)] > 0} {
	    set optsredo "$optsredo set xth(me,cmds,$id,$xpid,$item) {}; "
	    set optsundo "$optsundo set xth(me,cmds,$id,$xpid,$item) [list $xth(me,cmds,$id,$xpid,$item)]; "
	  }
	}
      }
    }
  }

  if {[regexp {^([^ :]+)\:([^ :]+)$} $ntype dum xntype xstype]} {
    if {![string equal $xntype u]} {
      set ntype $xntype
      set nopts [regsub -all {(^|\s+)\-subtype\s+\S+} $nopts {}]
      set nopts [regsub -all {^\s+|\s+$} $nopts {}]
      set nopts "-subtype $xstype $nopts"
    }
  }     
    
  # uprav options
  regsub {^\s*} $nopts "" nopts
  regsub {\s*$} $nopts "" nopts
  
  set nline "$ntype $nname $nopts $nrev"
  
  if {$pid > 0} {
    
    if {[catch {expr $nx}]} {
      set nx $ox
    }
    set nx [expr double($nx)]
    if {[catch {expr $ny}]} {
      set ny $oy
    }
    set ny [expr double($ny)]

    if {[string length $nxp] > 0} {
      if {[catch {expr $nxp}]} {
	set nxp $oxp
      }
      set nxp [expr double($nxp)]
    }
    if {[string length $nyp] > 0} {
      if {[catch {expr $nyp}]} {
	set nyp $oyp
      }
      set nyp [expr double($nyp)]
    }    
    if {([string length $nxp] < 1) || ([string length $nyp] < 1)} {
      set nxp {}
      set nyp {}
      set nsmth 0
    }
    
    if {[string length $nxn] > 0} {
      if {[catch {expr $nxn}]} {
	set nxn $oxn
      }
      set nxn [expr double($nxn)]
    }
    if {[string length $nyn] > 0} {
      if {[catch {expr $nyn}]} {
	set nyn $oyn
      }
      set nyn [expr double($nyn)]
    }    
    if {([string length $nxn] < 1) || ([string length $nyn] < 1)} {
      set nxn {}
      set nyn {}
      set nsmth 0
    }
    
    if {$nsmth} {
      set crds [xth_me_cmds_get_smoothed_cp 0 $nxp $nyp $nx $ny $nxn $nyn]
      set nxp [lindex $crds 0]
      set nyp [lindex $crds 1]
      set nxn [lindex $crds 2]
      set nyn [lindex $crds 3]
    }
    
    if {[string length $nrot] > 0} {
      if {[catch {expr $nrot}]} {
	set nrot $orot
      } elseif {($nrot < 0.0) || ($nrot >= 360.0)} {
	set nrot $orot
      } else {
	set nrot [expr double($nrot)]
      }
    }

    if {[string length $nrs] > 0} {
      if {[catch {expr $nrs}]} {
	set nrs $ors
      } elseif {$nrs <= 0.0} {
	set nrs $ors
      } else {
	set nrs [expr double($nrs)]
      }
    }

    if {[string length $nls] > 0} {
      if {[catch {expr $nls}]} {
	set nls $ols
      } elseif {$nls <= 0.0} {
	set nls $ols
      } else {
	set nls [expr double($nls)]
      }
    }
    
    # uprav options
    regsub {\s*$} $nptopts "" nptopts
    if {[string length $nptopts] > 0} {
      regsub {$} $nptopts "\n" nptopts
    }

    set nlinept "$nx $ny $nxp $nyp $nxn $nyn $nsmth $nrot $nrs $nls $nptopts"
  } else {
    set nlinept ""
  }

  # ak je v niecom rozdiel, tak to updatni
  if {![string equal "$nline $nlinept" "$oline $olinept"]} {
    #puts "new\n{$nline}\n===\n{$nlinept}\n===\nold\n{$oline}\n===\n{$olinept}\n===\n"
    xth_me_unredo_action [mc "line changes"] \
      "xth_me_cmds_update_line $id $pid $otype [list $oname] [list $oopts] $orev {$ox} {$oy} {$oxp} {$oyp} {$oxn} {$oyn} {$osmth} {$orot} {$ors} {$ols} [list $optopts] {$optoptpos}; $optsundo xth_me_cmds_select {$id $pid}" \
      "xth_me_cmds_update_line $id $pid $ntype [list $nname] [list $nopts] $nrev {$nx} {$ny} {$nxp} {$nyp} {$nxn} {$nyn} {$nsmth} {$nrot} {$nrs} {$nls} [list $nptopts] {$nptoptpos}; $optsredo xth_me_cmds_select {$id $pid}"    

    set xth(me,cmds,$id,type) $ntype
    set xth(me,cmds,$id,name) $nname 
    set xth(me,cmds,$id,options) $nopts 
    set xth(me,cmds,$id,reverse) $nrev 
  
    if {[string length $optsredo] > 0} {
      eval $optsredo
    }
  
    if {$pid > 0} {
      set xth(me,cmds,$id,$pid,x) $nx
      set xth(me,cmds,$id,$pid,y) $ny
      set xth(me,cmds,$id,$pid,xp) $nxp
      set xth(me,cmds,$id,$pid,yp) $nyp
      if {[string length "$nxp$nyp"] > 0} {
	set xth(me,cmds,$id,$pid,idp) 1
      } else {
	set xth(me,cmds,$id,$pid,idp) 0
      }
      set xth(me,cmds,$id,$pid,xn) $nxn
      set xth(me,cmds,$id,$pid,yn) $nyn
      if {[string length "$nxn$nyn"] > 0} {
	set xth(me,cmds,$id,$pid,idn) 1
      } else {
	set xth(me,cmds,$id,$pid,idn) 0
      }
      set xth(me,cmds,$id,$pid,smooth) $nsmth 
      set xth(me,cmds,$id,$pid,rotation) $nrot
      set xth(me,cmds,$id,$pid,rs) $nrs
      set xth(me,cmds,$id,$pid,ls) $nls
      set xth(me,cmds,$id,$pid,options) $nptopts 
      set xth(me,cmds,$id,$pid,optpos) $nptoptpos 
      xth_me_cmds_update_linept_list $id $pid
      set cpid 0
      if {$xth(me,cmds,$id,close)} {
	set fpid [lindex $xth(me,cmds,$id,xplist) 0]
	set lpid [lindex $xth(me,cmds,$id,xplist) [expr [llength $xth(me,cmds,$id,xplist)] - 2]]
	if {$pid == $fpid} {
	  set cpid $lpid
	} elseif {$pid == $lpid} {
	  set cpid $fpid
	}
      }
      if {$cpid > 0} {
	set xth(me,cmds,$id,$cpid,x) $nx
	set xth(me,cmds,$id,$cpid,y) $ny
	xth_me_cmds_move_linept $id $cpid
	set xth(me,cmds,$id,$cpid,xp) $nxp
	set xth(me,cmds,$id,$cpid,yp) $nyp
	set xth(me,cmds,$id,$cpid,idp) $xth(me,cmds,$id,$pid,idp)
	set xth(me,cmds,$id,$cpid,xn) $nxn
	set xth(me,cmds,$id,$cpid,yn) $nyn
	set xth(me,cmds,$id,$cpid,idn) $xth(me,cmds,$id,$pid,idn)
	set xth(me,cmds,$id,$cpid,smooth) $nsmth 
	set ix [lsearch $xth(me,cmds,$id,xplist) $cpid]
	xth_me_cmds_update_linept_list $id $cpid
      }
      xth_me_cmds_move_linelnpt $id $pid
    }  
    xth_me_cmds_update_line_data $id
    xth_me_cmds_update_list $id    
  }
  
}


proc xth_me_cmds_update_line_vars {id pid} {

  global xth
  set xth(ctrl,me,line,type) $xth(me,cmds,$id,type)
  set xth(ctrl,me,line,name) $xth(me,cmds,$id,name)
  set xth(ctrl,me,line,opts) $xth(me,cmds,$id,options)
  set xth(ctrl,me,line,reverse) $xth(me,cmds,$id,reverse)
  set xth(ctrl,me,line,close) $xth(me,cmds,$id,close)

  set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
  set tx [lsearch -exact $xth(me,themes,$at,line,hidelist) $xth(me,cmds,$id,type)]
  if {$tx < 0} {
    set xth(ctrl,me,line,themetype) {}
  } else {
    set xth(ctrl,me,line,themetype) [lindex $xth(me,themes,$at,line,showlist) $tx]
  }
  

  xth_me_cmds_update_linept_ctrl $id $pid
    
}

proc xth_me_cmds_update_area_vars {id} {

  global xth
  set xth(ctrl,me,ac,type) $xth(me,cmds,$id,type)
  set xth(ctrl,me,ac,opts) $xth(me,cmds,$id,options)

  set at [lindex $xth(me,themes,list) $xth(me,acttheme)]
  set tx [lsearch -exact $xth(me,themes,$at,area,hidelist) $xth(me,cmds,$id,type)]
  if {$tx < 0} {
    set xth(ctrl,me,ac,themetype) {}
  } else {
    set xth(ctrl,me,ac,themetype) [lindex $xth(me,themes,$at,area,showlist) $tx]
  }
  
}





proc xth_me_cmds_get_default_linept_cp {prv id pid} {
  global xth

  set x $xth(me,cmds,$id,$pid,x)
  set y $xth(me,cmds,$id,$pid,y)
  
  if {$prv} {
    if {$xth(me,cmds,$id,$pid,idn)} {
      set dx [expr $x - $xth(me,cmds,$id,$pid,xn)]
      set dy [expr $y - $xth(me,cmds,$id,$pid,yn)]
    } else {
      set dx 0.0
      set dy 0.0
    }
  } else {
    if {$xth(me,cmds,$id,$pid,idp)} {
      set dx [expr $x - $xth(me,cmds,$id,$pid,xp)]
      set dy [expr $y - $xth(me,cmds,$id,$pid,yp)]
    } else {
      set dx 0.0
      set dy 0.0
    }
  }
  set dd [expr hypot($dy, $dx)]
  set rd $dd

  if {$dd == 0.0} {

    set ppix [expr [lsearch $xth(me,cmds,$id,xplist) $pid] - 1]
    if {$ppix >= 0} {  
      set ppid [lindex $xth(me,cmds,$id,xplist) $ppix]
      set xp $xth(me,cmds,$id,$ppid,x)
      set yp $xth(me,cmds,$id,$ppid,y)
    } else {
      set xp $x
      set yp $y
    }
  
    set npix [expr [lsearch $xth(me,cmds,$id,xplist) $pid] + 1]
    set npid [lindex $xth(me,cmds,$id,xplist) $npix]
    if {$npid > 0} {
      set xn $xth(me,cmds,$id,$npid,x)
      set yn $xth(me,cmds,$id,$npid,y)
    } else {
      set xn $x
      set yn $y
    }

    if {$prv} {
      set dx [expr $xp - $xn]
      set dy [expr $yp - $yn]
    } else {
      set dx [expr $xn - $xp]
      set dy [expr $yn - $yp]
    }
    set dd [expr hypot($dy, $dx)]
 
  }
  
  if {$dd == 0.0} {
    set dx 0.0
    set dy 1.0
    set dd 1.0
    set rd 40.0
  }
  
  if {$rd == 0.0} {
    if {$prv} {
      set ppix [expr [lsearch $xth(me,cmds,$id,xplist) $pid] - 1]
      if {$ppix >= 0} {
	set ppid [lindex $xth(me,cmds,$id,xplist) $ppix]
	if {$xth(me,cmds,$id,$ppid,idn)} {
	  set rdx [expr $xth(me,cmds,$id,$ppid,xn) - $xth(me,cmds,$id,$ppid,x)]
	  set rdy [expr $xth(me,cmds,$id,$ppid,yn) - $xth(me,cmds,$id,$ppid,y)]
	  set rd [expr hypot($rdy,$rdx)]
	}
      }        
    } else {
      set npix [expr [lsearch $xth(me,cmds,$id,xplist) $pid] + 1]
      set npid [lindex $xth(me,cmds,$id,xplist) $npix]
      if {$npid > 0} {
	if {$xth(me,cmds,$id,$npid,idp)} {
	  set rdx [expr $xth(me,cmds,$id,$npid,xp) - $xth(me,cmds,$id,$npid,x)]
	  set rdy [expr $xth(me,cmds,$id,$npid,yp) - $xth(me,cmds,$id,$npid,y)]
	  set rd [expr hypot($rdy,$rdx)]
	}
      }        
    }
  }
  
  if {$rd == 0.0} {
    set rd 40.0
  }
  
  return [list [expr double([format "%.2f" [expr $x + $dx / $dd * $rd]])] \
    [expr double([format "%.2f" [expr $y + $dy / $dd * $rd]])]]
  
}


proc xth_me_cmds_get_smoothed_cp {which xp yp x y xn yn} {

  set dxp [expr $x - $xp]
  set dyp [expr $y - $yp]
  set dp [expr hypot($dxp,$dyp)]

  set dxn [expr $xn - $x]
  set dyn [expr $yn - $y]
  set dn [expr hypot($dxn,$dyn)]
  
  if {($dp == 0.0) || ($dn == 0.0)} {
    return [list $xp $yp $xn $yn]
  }
  
  if {$which < 0} {
    set xp [expr $x - $dxn / $dn * $dp]
    set yp [expr $y - $dyn / $dn * $dp]
  } elseif {$which > 0} {
    set xn [expr $x + $dxp / $dp * $dn]
    set yn [expr $y + $dyp / $dp * $dn]
  } else {
    set dx [expr $xn - $xp]
    set dy [expr $yn - $yp]
    set d [expr hypot($dx,$dy)]
    if {$d == 0.0} {
       set dx [expr -1.0 * $dyp]
       set dy $dxp
       set d $dp
    }
    set xp [expr $x - $dx / $d * $dp]
    set yp [expr $y - $dy / $d * $dp]      
    set xn [expr $x + $dx / $d * $dn]
    set yn [expr $y + $dy / $d * $dn]
  }

  return [list [expr double([format "%.2f" $xp])] \
     [expr double([format "%.2f" $yp])] [expr double([format "%.2f" $xn])] \
     [expr double([format "%.2f" $yn])]]
}


proc xth_me_cmds_get_default_rotation {id pid} {

  global xth

  set x $xth(me,cmds,$id,$pid,x)
  set y $xth(me,cmds,$id,$pid,y)
  set dp 0.0
  set dn 0.0

  if {$xth(me,cmds,$id,$pid,idp) || $xth(me,cmds,$id,$pid,idn)} {

    set xp $xth(me,cmds,$id,$pid,xp)
    set yp $xth(me,cmds,$id,$pid,yp)
    set dp [expr hypot($xp - $x, $yp - $y)]
    set xn $xth(me,cmds,$id,$pid,xn)
    set yn $xth(me,cmds,$id,$pid,yn)
    set dn [expr hypot($xn - $x, $yn - $y)]
    
  } else {

    set ppix [expr [lsearch $xth(me,cmds,$id,xplist) $pid] - 1]
    if {$ppix >= 0} {
      set ppid [lindex $xth(me,cmds,$id,xplist) $ppix]
      set xp $xth(me,cmds,$id,$ppid,x)
      set yp $xth(me,cmds,$id,$ppid,y)
      set dp [expr hypot($xp - $x, $yp - $y)]
    } else {
      set xp {}
      set yp {}
    }
    
    set npix [expr [lsearch $xth(me,cmds,$id,xplist) $pid] + 1]
    set npid [lindex $xth(me,cmds,$id,xplist) $npix]
    if {$npid > 0} {
      set xn $xth(me,cmds,$id,$npid,x)
      set yn $xth(me,cmds,$id,$npid,y)
      set dn [expr hypot($xn - $x, $yn - $y)]
    } else {
      set xn {}
      set yn {}
    }
  }
  
  if {($dp > 0.0) && ($dn > 0.0)} {
    if {$dp > $dn} {
      set xn [expr $x + ($xn - $x) / $dn * $dp]
      set yn [expr $y + ($yn - $y) / $dn * $dp]
    } else {
      set xp [expr $x + ($xp - $x) / $dp * $dn]
      set yp [expr $y + ($yp - $y) / $dp * $dn]
    }
  }

  if {[string length $xn] < 1} {
    set xn $x
    set yn $y
  } 

  if {[string length $xp] < 1} {
    set xp $x
    set yp $y
  } 

  set rr [expr 360.0 - atan2($yn - $yp,$xn - $xp) / 3.14159265359 * 180.0];
  while {$rr >= 360.0} {set rr [expr $rr - 360.0]};
  while {$rr < 0.0} {set rr [expr $rr + 360.0]};
  return [format "%.1f" $rr]
  
}


proc xth_me_cmds_update_linept_list {id pid} {
  global xth
  set ix [lsearch $xth(me,cmds,$id,xplist) $pid]
  set xth(me,cmds,$id,plist) \
    [lreplace $xth(me,cmds,$id,plist) $ix $ix [format "%7.2f %7.2f" $xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y)]]
}


proc xth_me_cmds_close_line {id {unredo 1}} {
  global xth
  # prida bod ak treba, ak je novy, oznaci ho
  set xl $xth(me,cmds,$id,xplist)
  set lix [expr [llength $xl] - 2]
  set fpid [lindex $xl 0]
  set nwpid $xth(me,cmds,selpid)
  set olpid $xth(me,cmds,selpid)
  if {($lix < 2) || \
    ($xth(me,cmds,$id,$fpid,x) != $xth(me,cmds,$id,[lindex $xl $lix],x)) || \
    ($xth(me,cmds,$id,$fpid,y) != $xth(me,cmds,$id,[lindex $xl $lix],y))} {
    set oldurok $xth(me,unredook)
    set xth(me,unredook) 0
    set iix [expr $lix + 1]
    xth_me_cmds_create_line_point $id $iix 0 \
      $xth(me,cmds,$id,$fpid,x) $xth(me,cmds,$id,$fpid,y) $xth(me,cmds,$id,$fpid,xp) $xth(me,cmds,$id,$fpid,yp) \
      $xth(me,cmds,$id,$fpid,xn) $xth(me,cmds,$id,$fpid,yn) $xth(me,cmds,$id,$fpid,smooth) {} {} {} {} 1.0
    set xth(me,unredook) $oldurok
    set nwpid $xth(me,cmds,$id,lpid)
    xth_me_cmds_select_linept $id $nwpid
    set unpoint "xth_me_cmds_delete_linept $id $nwpid"
    set repoint "xth_me_cmds_undelete_linept $id $nwpid $iix"
  } else {
    set lpid [lindex $xl $lix]
    set unpoint "xth_me_cmds_select $id\nset xth(me,cmds,$id,$lpid,xp) {$xth(me,cmds,$id,$lpid,xp)}\nset xth(me,cmds,$id,$lpid,yp) {$xth(me,cmds,$id,$lpid,yp)}\nset xth(me,cmds,$id,$lpid,idp) {$xth(me,cmds,$id,$lpid,idp)}\nset xth(me,cmds,$id,$lpid,xn) {$xth(me,cmds,$id,$lpid,xn)}\nset xth(me,cmds,$id,$lpid,yn) {$xth(me,cmds,$id,$lpid,yn)}\nset xth(me,cmds,$id,$lpid,idn) {$xth(me,cmds,$id,$lpid,idn)}\nset xth(me,cmds,$id,$lpid,smooth) {$xth(me,cmds,$id,$lpid,smooth)}\nxth_me_cmds_move_linelnpt $id $lpid"
    set repoint "xth_me_cmds_select $id\nset xth(me,cmds,$id,$lpid,xp) {$xth(me,cmds,$id,$fpid,xp)}\nset xth(me,cmds,$id,$lpid,yp) {$xth(me,cmds,$id,$fpid,yp)}\nset xth(me,cmds,$id,$lpid,idp) {$xth(me,cmds,$id,$fpid,idp)}\nset xth(me,cmds,$id,$lpid,xn) {$xth(me,cmds,$id,$fpid,xn)}\nset xth(me,cmds,$id,$lpid,yn) {$xth(me,cmds,$id,$fpid,yn)}\nset xth(me,cmds,$id,$lpid,idn) {$xth(me,cmds,$id,$fpid,idn)}\nset xth(me,cmds,$id,$lpid,smooth) {$xth(me,cmds,$id,$fpid,smooth)}\nxth_me_cmds_move_linelnpt $id $lpid"
    eval $repoint
  }
  # nastavi close
  set xth(me,cmds,$id,close) 1
  # update vars
  xth_me_cmds_update_line_vars $id $nwpid
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  if {$xth(me,cmds,mode) == 2} {
    set remode "xth_me_cmds_set_mode 0"
    set unmode "xth_me_cmds_set_mode 2"
    eval $remode
  } else {
    set remode ""
    set unmode ""
  }
  if {$unredo} {
    xth_me_unredo_action [mc "line closing"] \
    "$unpoint\nset xth(me,cmds,$id,close) 0\nxth_me_cmds_update_line_vars $id $olpid\nxth_me_cmds_update_line_data $id\nxth_me_prev_cmd \$xth(me,cmds,$id,data)\n$unmode" \
    "$repoint\nset xth(me,cmds,$id,close) 1\nxth_me_cmds_update_line_vars $id $nwpid\nxth_me_cmds_update_line_data $id\nxth_me_prev_cmd \$xth(me,cmds,$id,data)\n$remode"
  }
}


proc xth_me_cmds_open_line {id} {
  global xth
  set xth(me,cmds,$id,close) 0
  xth_me_cmds_update_line_vars $id $xth(me,cmds,selpid)
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  xth_me_unredo_action [mc "line opening"] \
    "xth_me_cmds_reclose_line $id" \
    "xth_me_cmds_open_line $id"
}

proc xth_me_cmds_reclose_line {id} {
  global xth
  set xth(me,cmds,$id,close) 1
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  xth_me_cmds_update_line_vars $id $xth(me,cmds,selpid)
}


proc xth_me_cmds_get_bezier_coords {x1 y1 c1x c1y c2x c2y x2 y2} {
#  if {[llength $x1] > 1} {
#    set tlen [lindex $x1 2]
#    set rotation [lindex $x1 1]
#    set x1 [lindex $x1 0]
#  } else {
#    set rotation {}
#  }
  if {[string length $c1x] < 1} {
    set c1x $x1
    set c1y $y1
  }
  if {[string length $c2x] < 1} {
    set c2x $x2
    set c2y $y2
  }
  set q 20
#  if {[string length $rotation] > 0} {    
#    set crds [list [expr $x1 + $tlen * sin(double($rotation)/180.0*3.14159265359)] \
#      [expr $y1 + $tlen * cos(double($rotation)/180.0*3.14159265359)]]
#  } else {
    set crds {}
#  }
  for {set i 0} {$i <= $q} {incr i} {
    set t [expr $i.0 / $q.0]
    set t2 [expr pow($t,2.0)]
    set t3 [expr pow($t,3.0)]
    set t_ [expr 1.0 - $t]
    set t_2 [expr pow($t_,2.0)]
    set t_3 [expr pow($t_,3.0)]
    lappend crds [expr $t_3 * $x1 + 3.0 * $t * $t_2 * $c1x + 3.0 * $t2 * $t_ * $c2x + $t3 * $x2] \
      [expr $t_3 * $y1 + 3.0 * $t * $t_2 * $c1y + 3.0 * $t2 * $t_ * $c2y + $t3 * $y2]
  }
  return $crds
}


proc xth_me_cmds_real2can_coords {crds} {
  set x 1
  set r {}
  foreach c $crds {
    if $x {
      lappend r [xth_me_real2canx $c]
      set x 0
    } else {
      lappend r [xth_me_real2cany $c]
      set x 1
    }
  }
  return $r
}


proc xth_me_cmds_get_crds2state {id ppid pid} {
  global xth
#  set tlen [expr 0.01 * $xth(me,zoom) * $xth(gui,me,line,ticksize)]

  if {$ppid > 0} {

#    if {[lsearch $xth(me,cmds,$id,xplist) $ppid] == 0} {
#      set rot [xth_me_cmds_get_default_rotation $id $ppid]
#      set x1 [list $xth(me,cmds,$id,$ppid,x) $rot $tlen]
#    } else {
#      set rot {}
      set x1 $xth(me,cmds,$id,$ppid,x)
#    }

    set st normal
    if {$xth(me,cmds,$id,$ppid,idn) || $xth(me,cmds,$id,$pid,idp)} {
      set crds [xth_me_cmds_get_bezier_coords $x1 \
	$xth(me,cmds,$id,$ppid,y) $xth(me,cmds,$id,$ppid,xn) \
	$xth(me,cmds,$id,$ppid,yn) $xth(me,cmds,$id,$pid,xp) \
	$xth(me,cmds,$id,$pid,yp) $xth(me,cmds,$id,$pid,x) \
	$xth(me,cmds,$id,$pid,y)]
    } else {
#      if {[string length $rot] > 0} {
#        set crds [list [expr $xth(me,cmds,$id,$ppid,x) + $tlen * sin(double($rot)/180.0*3.14159265359)] \
#          [expr $xth(me,cmds,$id,$ppid,y) + $tlen * cos(double($rot)/180.0*3.14159265359)]]
#      } else {
	set crds {}
#      }
      lappend crds $xth(me,cmds,$id,$ppid,x) $xth(me,cmds,$id,$ppid,y) \
	$xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y)
    }
  } else {
    set crds {0 0 10 10}
    set st hidden
  }
  return [list $st $crds]
}



proc xth_me_cmds_draw_lineln {id ppid pid} {
  global xth
  set st2crds [xth_me_cmds_get_crds2state $id $ppid $pid]
  set st [lindex $st2crds 0]
  set crds [lindex $st2crds 1]
  $xth(me,can) create line [xth_me_cmds_real2can_coords $crds] -width $xth(gui,me,line,width) -fill $xth(gui,me,activefill) \
    -tags "line ln$id lnln$id ln$id.$pid command" -state $st
  xth_me_bind_area_drag ln$id.$pid {}
  $xth(me,can) bind ln$id.$pid <1> "xth_me_cmds_click_lineln {$id $pid} pt$id.$pid %x %y"
  set highlight_on "if {\$xth(me,cmds,selid) != $id} {\$xth(me,can) itemconfigure lnln$id -fill \$xth(gui,me,highlightfill)}"
  set highlight_off "if {\$xth(me,cmds,selid) != $id} {\$xth(me,can) itemconfigure lnln$id -fill \[$xth(me,can) itemcget pt$id.$pid -outline\]}"
  $xth(me,can) bind ln$id.$pid <Enter> "$highlight_on\nxth_status_bar_push me; xth_status_bar_status me \"\$xth(me,cmds,$id,listix): \$xth(me,cmds,$id,sbar)\""
  $xth(me,can) bind ln$id.$pid <Leave> "$highlight_off\nxth_status_bar_pop me"
  $xth(me,can) bind ln$id.$pid <$xth(gui,rmb)> "xth_me_show_context_menu {$id $pid} %x %y"  
  catch {$xth(me,can) lower ln$id.$pid point}
}


proc xth_me_cmds_move_lineln {id ppid pid} {
  global xth
  set st2crds [xth_me_cmds_get_crds2state $id $ppid $pid]
  set st [lindex $st2crds 0]
  set crds [lindex $st2crds 1]
  $xth(me,can) coords ln$id.$pid [xth_me_cmds_real2can_coords $crds]
  #$xth(me,can) itemconfigure ln$id.$pid -state $st
}


proc xth_me_cmds_draw_linept {id pid} {
  global xth
  $xth(me,can) create oval [expr [xth_me_real2canx $xth(me,cmds,$id,$pid,x)] - $xth(gui,me,line,psize)] \
  [expr [xth_me_real2cany $xth(me,cmds,$id,$pid,y)] - $xth(gui,me,line,psize)] [expr [xth_me_real2canx $xth(me,cmds,$id,$pid,x)] + $xth(gui,me,line,psize)] \
  [expr [xth_me_real2cany $xth(me,cmds,$id,$pid,y)] + $xth(gui,me,line,psize)] -width 1 -outline blue -fill $xth(gui,me,activefill) \
  -tags "point ln$id lnpt$id pt$id.$pid command"
  set highlight_on "if {\$xth(me,cmds,selid) != $id} {\$xth(me,can) itemconfigure lnln$id -fill \$xth(gui,me,highlightfill)}"
  set highlight_off "if {\$xth(me,cmds,selid) != $id} {\$xth(me,can) itemconfigure lnln$id -fill \[$xth(me,can) itemcget pt$id.$pid -outline\]}"
  $xth(me,can) bind pt$id.$pid <Enter> "$highlight_on\n$xth(me,can) itemconfigure pt$id.$pid -fill cyan; xth_status_bar_push me; xth_status_bar_status me \"\$xth(me,cmds,$id,listix): \[lindex \[regexp -inline -- {^\[^\\n\]*} \$xth(me,cmds,$id,data)\] 0\]\""
  $xth(me,can) bind pt$id.$pid <Leave> "$highlight_off\n$xth(me,can) itemconfigure pt$id.$pid -fill \[$xth(me,can) itemcget ln$id.$pid -fill\]; xth_status_bar_pop me"
  $xth(me,can) bind pt$id.$pid <1> "xth_me_cmds_click {$id $pid} pt$id.$pid \$xth(me,cmds,$id,$pid,x) \$xth(me,cmds,$id,$pid,y) %x %y"
  $xth(me,can) bind pt$id.$pid <$xth(gui,rmb)> "xth_me_show_context_menu {$id $pid} %x %y"  
  $xth(me,can) bind pt$id.$pid <Shift-1> "xth_me_cmds_special_select {$id $pid} %x %y"  
  $xth(me,can) bind pt$id.$pid <$xth(kb_control)-1> "xth_me_cmds_click_area pt$id.$pid %x %y"
}


proc xth_me_cmds_move_linept {id pid} {
  global xth
  $xth(me,can) coords pt$id.$pid [expr [xth_me_real2canx $xth(me,cmds,$id,$pid,x)] - $xth(gui,me,line,psize)] \
  [expr [xth_me_real2cany $xth(me,cmds,$id,$pid,y)] - $xth(gui,me,line,psize)] [expr [xth_me_real2canx $xth(me,cmds,$id,$pid,x)] + $xth(gui,me,line,psize)] \
  [expr [xth_me_real2cany $xth(me,cmds,$id,$pid,y)] + $xth(gui,me,line,psize)]
}


proc xth_me_cmds_draw_line {id} {
  global xth
  set ppid 0
  foreach pid $xth(me,cmds,$id,xplist) {
    if {($pid > 0)} {
      xth_me_cmds_draw_lineln $id $ppid $pid
      xth_me_cmds_draw_linept $id $pid
    }
    set ppid $pid
  }
  catch {$xth(me,can) raise lnpt$id point}
}


proc xth_me_cmds_move_line {id} {
  global xth
  set ppid 0
  foreach pid $xth(me,cmds,$id,xplist) {
    if {($pid > 0)} {
      xth_me_cmds_move_lineln $id $ppid $pid
      xth_me_cmds_move_linept $id $pid
    }
    set ppid $pid
  }
}


proc xth_me_cmds_move_linelnpt {id pid} {
  global xth
  set xl $xth(me,cmds,$id,xplist)
  set ix [lsearch $xl $pid]
  if {($ix < 0) || ($pid == 0)} {
    return
  }
  
  set pix [expr $ix - 1]
  set nix [expr $ix + 1]
    
  set fix 0
  set lix [expr [llength $xl] - 2]
  
  set cpid 0
  set mpix 0
  set mnix 0
  
  if {$xth(me,cmds,$id,close)} {
    if {$ix == $fix} {
      set cpid [lindex $xl $lix]
      set pix [expr $lix - 1]
      set mpix 1
    } elseif {$ix == $lix} {
      set cpid [lindex $xl $fix]
      set nix [expr $fix + 1]
      set mnix 1
    }
  }
  xth_me_cmds_move_linept $id $pid
  if {$cpid > 0} {
    xth_me_cmds_move_linept $id $cpid
  }
  
  if {($pix >= $fix) && ($pix <= $lix)} {
    set ppid [lindex $xl $pix]
  } else {
    set ppid 0
  }
  if {$mpix} {
    xth_me_cmds_move_lineln $id $ppid $cpid
  } else {
    xth_me_cmds_move_lineln $id $ppid $pid
  }


  if {($nix >= $fix) && ($nix <= $lix)} {
    set npid [lindex $xl $nix]
    if {$mnix} {
      xth_me_cmds_move_lineln $id $cpid $npid
    } else {
      xth_me_cmds_move_lineln $id $pid $npid
    }
  }
  
}

proc xth_me_cmds_start_create_linept {tagOrId x y mx my} {

  global xth

  set xth(me,lptc,id) $xth(me,cmds,selid)
  set id $xth(me,lptc,id)
  if {$xth(me,cmds,$id,ct) != 3} {
    return
  }
  set xl $xth(me,cmds,$id,xplist)
  set inspid $xth(me,cmds,inspid)
  set oldpid $xth(me,cmds,selpid)
  set ix [lsearch $xl $inspid]
  set xth(me,lptc,mx) $mx
  set xth(me,lptc,my) $my
  set xth(me,lptc,tagOrId) $tagOrId
  if {($ix == 0) && $xth(me,cmds,$id,close)} {
    set unclosecmd "xth_me_cmds_reclose_line $id"
    set reclosecmd "xth_me_cmds_open_line $id"
    set ook $xth(me,unredook)
    set xth(me,unredook) 0
    eval $reclosecmd
    set xth(me,unredook) $ook
  } else {
    set unclosecmd ""
    set reclosecmd ""
  }

  xth_me_cmds_create_line_point $id $ix 0 $x $y {} {} {} {} 0 {} {} {} {} 1.0
  xth_me_cmds_hide_linept_xctrl  
  set pid $xth(me,cmds,$id,lpid)
  set pnpid [xth_me_cmds_get_line_pnpid $id $pid]
  set xth(me,lptc,ppid) [lindex $pnpid 0]
  set ppid $xth(me,lptc,ppid)
  set xth(me,lptc,npid) [lindex $pnpid 1]
  set npid $xth(me,lptc,npid)
  set xth(me,lptc,pid) $pid
  set xth(me,lptc,oldm) [$xth(me,can) bind $tagOrId <B1-Motion>]
  set xth(me,lptc,oldr) [$xth(me,can) bind $tagOrId <B1-ButtonRelease>]
  $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill yellow
  $xth(me,can) bind $tagOrId <B1-Motion> "xth_me_cmds_continue_linept_creation %x %y 1"
  $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-Motion> "xth_me_cmds_continue_linept_creation %x %y 0"
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> "xth_me_cmds_end_create_linept %x %y 1"
  $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-ButtonRelease> "xth_me_cmds_end_create_linept %x %y 0"
  xth_me_cmds_continue_linept_creation $mx $my 1
  xth_me_unredo_action [mc "inserting line point"] \
    "xth_me_cmds_delete_linept $id $pid\n$unclosecmd\nxth_me_cmds_select_linept $id $oldpid" \
    "$reclosecmd\nxth_me_cmds_undelete_linept $id $pid $ix"
  if {($ppid > 0) && $xth(me,cmds,$id,$ppid,idn)} {
    $xth(me,can) itemconfigure lineptppcp -state normal
  }
  if {($npid > 0) && $xth(me,cmds,$id,$npid,idp)} {
    $xth(me,can) itemconfigure lineptnncp -state normal
  }
  xth_me_cmds_continue_linept_creation $mx $my 1
}


proc xth_me_cmds_continue_linept_creation {x y motionID} {
  global xth
  set id $xth(me,lptc,id)
  set pid $xth(me,lptc,pid)
  set ppid $xth(me,lptc,ppid)
  set npid $xth(me,lptc,npid)
  set dx [expr $x - $xth(me,lptc,mx)]
  set dy [expr $y - $xth(me,lptc,my)]
  set dst [expr hypot($dy,$dx)]
  if {$dst > $xth(gui,me,line,psize)} {
    set xn [xth_me_can2realx [$xth(me,can) canvasx $x]]
    set yn [xth_me_can2realy [$xth(me,can) canvasy $y]]
    set xth(me,cmds,$id,$pid,idn) 1
    set xth(me,cmds,$id,$pid,xn) [expr double([format %.2f $xn])]
    set xth(me,cmds,$id,$pid,yn) [expr double([format %.2f $yn])]
    set xth(me,cmds,$id,$pid,idp) 1
    set x $xth(me,cmds,$id,$pid,x)
    set y $xth(me,cmds,$id,$pid,y)
    set dx [expr $xn - $x]
    set dy [expr $yn - $y]
    $xth(me,can) itemconfigure lineptpcp -state normal
    $xth(me,can) itemconfigure lineptncp -state normal
    if {($ppid > 0) && $xth(me,cmds,$id,$ppid,idn)} {
      $xth(me,can) itemconfigure lineptppcp -state normal
    }
    set d [expr hypot($dy,$dx)]
    if {$motionID} {
      set xd $d
      set xth(me,lptc,xd) $d
    } else {
      if {[info exist xth(me,lptc,xd)]} {
	set xd $xth(me,lptc,xd)
      } else {
	set xd $d
      }
    }
    if {(!$motionID)} {
      set dx [expr $dx / $d * $xd]
      set dy [expr $dy / $d * $xd]
    }
    set xth(me,cmds,$id,$pid,xp) [expr double([format %.2f [expr $x - $dx]])]
    set xth(me,cmds,$id,$pid,yp) [expr double([format %.2f [expr $y - $dy]])]
    set xth(me,cmds,$id,$pid,smooth) 1
  } else {
    set xth(me,cmds,$id,$pid,idn) 0
    set xth(me,cmds,$id,$pid,xn) {}
    set xth(me,cmds,$id,$pid,yn) {}
    set xth(me,cmds,$id,$pid,idp) 0
    set xth(me,cmds,$id,$pid,xp) {}
    set xth(me,cmds,$id,$pid,yp) {}
    $xth(me,can) itemconfigure lineptpcp -state hidden
    $xth(me,can) itemconfigure lineptncp -state hidden
  }
  xth_me_cmds_move_lineptcp_xctrl $id $ppid $pid $npid
  xth_me_cmds_move_line_xctrl $id
  xth_me_cmds_move_linelnpt $id $pid
  update idletasks
}


proc xth_me_cmds_end_create_linept {x y motionID} {
  global xth
  xth_me_cmds_continue_linept_creation $x $y $motionID
  set tagOrId $xth(me,lptc,tagOrId)
  set id $xth(me,lptc,id)
  set pid $xth(me,lptc,pid)
  $xth(me,can) bind $tagOrId <B1-Motion> $xth(me,lptc,oldm)
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> $xth(me,lptc,oldr)
  $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-Motion> ""
  $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-ButtonRelease> ""
  xth_me_cmds_hide_linept_xctrl  
  set ook $xth(me,unredook)
  set xth(me,unredook) 0
  if {$xth(me,cmds,$id,$pid,idn)} {
    $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill yellow
  } else {
    $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill red
  }
  xth_me_cmds_select_linept $id $pid
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)
  set xth(me,unredook) $ook
}


proc xth_me_cmds_end_line {} {
  set recmds "xth_me_cmds_set_mode 0"
  set uncmds "xth_me_cmds_set_mode 2"
  eval $recmds
  xth_me_unredo_action [mc "line ending"] $uncmds $recmds
}


proc xth_me_cmds_start_linept_insert {} {
  global xth
  set xth(me,cmds,inspid) $xth(me,cmds,selpid)
  xth_me_cmds_set_mode 2
}


proc xth_me_cmds_start_area_insert {btn} {
  global xth
  if {$btn && ($xth(me,cmds,mode) == 3)} {
    xth_me_cmds_set_mode 0
  } else {
    xth_me_cmds_set_mode 3
  }
}


proc xth_me_cmds_get_line_pnpid {id pid} {
  global xth
  set xl $xth(me,cmds,$id,xplist)
  set ix [lsearch $xl $pid]
  set lix [expr [llength $xl] - 2]
  if {$ix > 0} {
    set ppid [lindex $xl [expr $ix - 1]]
  } elseif {$xth(me,cmds,$id,close) && ($lix > 0)} {
    set ppid [lindex $xl [expr $lix - 1]]
  } else {
    set ppid 0
  }
  if {$ix < $lix} {
    set npid [lindex $xl [expr $ix + 1]]
  } elseif {$xth(me,cmds,$id,close) && ($lix > 0)} {
    set npid [lindex $xl 1]
  } else {
    set npid 0
  }
  return [list $ppid $npid]
}


proc xth_me_cmds_start_linecp_drag {tagOrId id ppid pid npid which x y} {

  global xth
  
  xth_me_cmds_update {}

  set xth(me,lcpd,tagOrId) $tagOrId
  set xth(me,lcpd,id) $id
  set xth(me,lcpd,pid) $pid
  set xth(me,lcpd,ppid) $ppid
  set xth(me,lcpd,npid) $npid
  set xth(me,lcpd,which) $which
  set xth(me,lcpd,mx) $x
  set xth(me,lcpd,my) $y
  
  set lix [expr [llength $xth(me,cmds,$id,xplist)] - 2]
  if {$lix > 0} {
    set fpid [lindex $xth(me,cmds,$id,xplist) 0]
    set lpid [lindex $xth(me,cmds,$id,xplist) $lix] 
  }
  set altpid 0
  set altppid 0
  set altnpid 0
  if {($lix > 0) && $xth(me,cmds,$id,close)} {
    if {$pid == $fpid} {
      set altpid $lpid
    } elseif {$pid == $lpid} {
      set altpid $fpid
    }
    if {$ppid == $fpid} {
      set altppid $lpid
    } elseif {$ppid == $lpid} {
      set altppid $fpid
    }
    if {$npid == $fpid} {
      set altnpid $lpid
    } elseif {$npid == $lpid} {
      set altnpid $fpid
    }
  }
  
  set xth(me,lcpd,altpid) $altpid
  set xth(me,lcpd,altppid) $altppid
  set xth(me,lcpd,altnpid) $altnpid

  set xth(me,lcpd,oldenter) [$xth(me,can) bind $tagOrId <Enter>]
  $xth(me,can) bind $tagOrId <Enter> ""
  set xth(me,lcpd,oldleave) [$xth(me,can) bind $tagOrId <Leave>]
  $xth(me,can) bind $tagOrId <Leave> ""
  set xth(me,lcpd,oldfill) [$xth(me,can) itemcget $tagOrId -fill]
  $xth(me,can) itemconfigure $tagOrId -fill {}
  $xth(me,can) bind $tagOrId <B1-Motion> "xth_me_cmds_continue_linecp_drag %x %y 0"
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> "xth_me_cmds_end_linecp_drag %x %y 0"

  set dragto 0

  switch $which {
    x {
      set dragto 1
      $xth(me,can) bind $tagOrId <B1-Motion> "xth_me_cmds_continue_linecp_drag %x %y 1"
      $xth(me,can) bind $tagOrId <B1-ButtonRelease> "xth_me_cmds_end_linecp_drag %x %y 1"
      $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-Motion> "xth_me_cmds_continue_linecp_drag %x %y 0"
      $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-ButtonRelease> "xth_me_cmds_end_linecp_drag %x %y 0"
      set xth(me,lcpd,oldmove,pcplstate) [$xth(me,can) itemcget $xth(me,canid,linept,pcpl) -state]
      $xth(me,can) itemconfigure $xth(me,canid,linept,pcpl) -state hidden
      set xth(me,lcpd,oldmove,ncplstate) [$xth(me,can) itemcget $xth(me,canid,linept,ncpl) -state]
      $xth(me,can) itemconfigure $xth(me,canid,linept,ncpl) -state hidden
      set xth(me,lcpd,oldmove,selstate) [$xth(me,can) itemcget $xth(me,canid,linept,selector) -state]
      $xth(me,can) itemconfigure $xth(me,canid,linept,selector) -state hidden
      set xth(me,lcpd,oldmove,pcpfill) [$xth(me,can) itemcget $xth(me,canid,linept,pcp) -fill]
      $xth(me,can) itemconfigure $xth(me,canid,linept,pcp) -fill {}
      set xth(me,lcpd,oldmove,ncpfill) [$xth(me,can) itemcget $xth(me,canid,linept,ncp) -fill]
      $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill {}
      $xth(me,can) itemconfigure $xth(me,canid,linept,fr) -width 1 -arrow none
      $xth(me,can) itemconfigure $xth(me,canid,linept,fl) -width 1 -arrow none
      set xth(me,lcpd,oldx) $xth(me,cmds,$id,$pid,x)
      set xth(me,lcpd,oldy) $xth(me,cmds,$id,$pid,y)
      set xth(me,lcpd,oldxp) $xth(me,cmds,$id,$pid,xp)
      set xth(me,lcpd,oldyp) $xth(me,cmds,$id,$pid,yp)
      set xth(me,lcpd,oldxn) $xth(me,cmds,$id,$pid,xn)
      set xth(me,lcpd,oldyn) $xth(me,cmds,$id,$pid,yn)
    }
    p {
      set xth(me,lcpd,oldxp) $xth(me,cmds,$id,$pid,xp)
      set xth(me,lcpd,oldyp) $xth(me,cmds,$id,$pid,yp)
      set xth(me,lcpd,oldxn) $xth(me,cmds,$id,$pid,xn)
      set xth(me,lcpd,oldyn) $xth(me,cmds,$id,$pid,yn)
    }
    n {
      set xth(me,lcpd,oldxp) $xth(me,cmds,$id,$pid,xp)
      set xth(me,lcpd,oldyp) $xth(me,cmds,$id,$pid,yp)
      set xth(me,lcpd,oldxn) $xth(me,cmds,$id,$pid,xn)
      set xth(me,lcpd,oldyn) $xth(me,cmds,$id,$pid,yn)
    }
    pp {
      set xth(me,lcpd,oldxpp) $xth(me,cmds,$id,$ppid,xn)
      set xth(me,lcpd,oldypp) $xth(me,cmds,$id,$ppid,yn)
    }
    nn {
      set xth(me,lcpd,oldxnn) $xth(me,cmds,$id,$npid,xp)
      set xth(me,lcpd,oldynn) $xth(me,cmds,$id,$npid,yp)
    }
  }
  
  xth_me_cmds_continue_linecp_drag $x $y $dragto
  $xth(me,can) configure -cursor {}
  
}


proc xth_me_cmds_continue_linecp_drag {x y dragto} {
  global xth
  set id $xth(me,lcpd,id)
  set pid $xth(me,lcpd,pid)
  set ppid $xth(me,lcpd,ppid)
  set npid $xth(me,lcpd,npid)
  set tagOrId $xth(me,lcpd,tagOrId)
  set altpid $xth(me,lcpd,altpid)
  set altppid $xth(me,lcpd,altppid)
  set altnpid $xth(me,lcpd,altnpid)
  set nx [expr double([format %.2f [xth_me_can2realx [$xth(me,can) canvasx $x]]])]
  set ny [expr double([format %.2f [xth_me_can2realy [$xth(me,can) canvasy $y]]])]
  set dts 0
  if $dragto {
    set dtl [xth_me_cmds_drag_to $id $pid $x $y]
    if {[lindex $dtl 0]} {
      set nx [lindex $dtl 1]
      set ny [lindex $dtl 2]
      set dts 1
    }
  }
  if $dts {
    $xth(me,can) itemconfigure $tagOrId -fill cyan
  } else {
    $xth(me,can) itemconfigure $tagOrId -fill {}
  }
  switch $xth(me,lcpd,which) {
    x {
      set xth(ctrl,me,linept,x) $nx
      set xth(me,cmds,$id,$pid,x) $nx
      set xth(ctrl,me,linept,y) $ny
      set xth(me,cmds,$id,$pid,y) $ny
      if {$altpid > 0} {
	set xth(me,cmds,$id,$altpid,y) $ny
	set xth(me,cmds,$id,$altpid,x) $nx
      }
      
      if {$xth(me,cmds,$id,$pid,idp)} {
	set nxp [expr double([format %.2f [expr $nx + $xth(me,lcpd,oldxp) - $xth(me,lcpd,oldx)]])]
	set nyp [expr double([format %.2f [expr $ny + $xth(me,lcpd,oldyp) - $xth(me,lcpd,oldy)]])]
	set xth(ctrl,me,linept,xp) $nxp
	set xth(me,cmds,$id,$pid,xp) $nxp
	set xth(ctrl,me,linept,yp) $nyp
	set xth(me,cmds,$id,$pid,yp) $nyp
	if {$altpid > 0} {
	  set xth(me,cmds,$id,$altpid,xp) $nxp
	  set xth(me,cmds,$id,$altpid,yp) $nyp
	}
      }
      
      if {$xth(me,cmds,$id,$pid,idn)} {
	set nxn [expr double([format %.2f [expr $nx + $xth(me,lcpd,oldxn) - $xth(me,lcpd,oldx)]])]
	set nyn [expr double([format %.2f [expr $ny + $xth(me,lcpd,oldyn) - $xth(me,lcpd,oldy)]])]
	set xth(ctrl,me,linept,xn) $nxn
	set xth(me,cmds,$id,$pid,xn) $nxn
	set xth(ctrl,me,linept,yn) $nyn
	set xth(me,cmds,$id,$pid,yn) $nyn
	if {$altpid > 0} {
	  set xth(me,cmds,$id,$altpid,xn) $nxn
	  set xth(me,cmds,$id,$altpid,yn) $nyn
	}
      }
      xth_me_cmds_move_linept_xctrl $id $pid
      xth_me_cmds_move_line_xctrl $id
    }
    p {
      set xth(ctrl,me,linept,xp) $nx
      set xth(me,cmds,$id,$pid,xp) $nx
      set xth(ctrl,me,linept,yp) $ny
      set xth(me,cmds,$id,$pid,yp) $ny
      if {$altpid > 0} {
	set xth(me,cmds,$id,$altpid,xp) $nx
	set xth(me,cmds,$id,$altpid,yp) $ny
      }
      if {$xth(me,cmds,$id,$pid,idn) && $xth(me,cmds,$id,$pid,smooth)} {
	set ncn [xth_me_cmds_get_smoothed_cp 1 $nx $ny \
	  $xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y) \
	  $xth(me,cmds,$id,$pid,xn) $xth(me,cmds,$id,$pid,yn)]
	set nxn [lindex $ncn 2]
	set nyn [lindex $ncn 3]
	set xth(ctrl,me,linept,xn) $nxn
	set xth(me,cmds,$id,$pid,xn) $nxn
	set xth(ctrl,me,linept,yn) $nyn
	set xth(me,cmds,$id,$pid,yn) $nyn
	if {$altpid > 0} {
	  set xth(me,cmds,$id,$altpid,xn) $nxn
	  set xth(me,cmds,$id,$altpid,yn) $nyn
	}
      }
    }
    n {
      set xth(ctrl,me,linept,xn) $nx
      set xth(me,cmds,$id,$pid,xn) $nx
      set xth(ctrl,me,linept,yn) $ny
      set xth(me,cmds,$id,$pid,yn) $ny
      if {$altpid > 0} {
	set xth(me,cmds,$id,$altpid,xn) $nx
	set xth(me,cmds,$id,$altpid,yn) $ny
      }
      if {$xth(me,cmds,$id,$pid,idp) && $xth(me,cmds,$id,$pid,smooth)} {
	set ncp [xth_me_cmds_get_smoothed_cp -1 \
	  $xth(me,cmds,$id,$pid,xp) $xth(me,cmds,$id,$pid,yp) \
	  $xth(me,cmds,$id,$pid,x) $xth(me,cmds,$id,$pid,y) \
	  $nx $ny]
	set nxp [lindex $ncp 0]
	set nyp [lindex $ncp 1]
	set xth(ctrl,me,linept,xp) $nxp
	set xth(me,cmds,$id,$pid,xp) $nxp
	set xth(ctrl,me,linept,yp) $nyp
	set xth(me,cmds,$id,$pid,yp) $nyp
	if {$altpid > 0} {
	  set xth(me,cmds,$id,$altpid,xp) $nxp
	  set xth(me,cmds,$id,$altpid,yp) $nyp
	}
      }
    }
    pp {
      if {$xth(me,cmds,$id,$ppid,smooth)} {
	set ncp [xth_me_cmds_get_smoothed_cp 1 \
	  $xth(me,cmds,$id,$ppid,xp) $xth(me,cmds,$id,$ppid,yp) \
	  $xth(me,cmds,$id,$ppid,x) $xth(me,cmds,$id,$ppid,y) \
	  $nx $ny]
	set xth(me,cmds,$id,$ppid,xn) [lindex $ncp 2]
	set xth(me,cmds,$id,$ppid,yn) [lindex $ncp 3]
      } else {
	set xth(me,cmds,$id,$ppid,xn) $nx
	set xth(me,cmds,$id,$ppid,yn) $ny
      }
      if {$altppid > 0} {
	set xth(me,cmds,$id,$altppid,xn) $xth(me,cmds,$id,$ppid,xn)
	set xth(me,cmds,$id,$altppid,yn) $xth(me,cmds,$id,$ppid,yn)
      }
    }
    nn {
      if {$xth(me,cmds,$id,$npid,smooth)} {
	set ncp [xth_me_cmds_get_smoothed_cp -1 \
	  $nx $ny \
	  $xth(me,cmds,$id,$npid,x) $xth(me,cmds,$id,$npid,y) \
	  $xth(me,cmds,$id,$npid,xn) $xth(me,cmds,$id,$npid,yn)]
	set xth(me,cmds,$id,$npid,xp) [lindex $ncp 0]
	set xth(me,cmds,$id,$npid,yp) [lindex $ncp 1]
      } else {
	set xth(me,cmds,$id,$npid,xp) $nx
	set xth(me,cmds,$id,$npid,yp) $ny
      }
      if {$altnpid > 0} {
	set xth(me,cmds,$id,$altnpid,xp) $xth(me,cmds,$id,$npid,xp)
	set xth(me,cmds,$id,$altnpid,yp) $xth(me,cmds,$id,$npid,yp)
      }
    }
  }
  xth_me_cmds_move_lineptcp_xctrl $id $ppid $pid $npid
  xth_me_cmds_move_linelnpt $id $pid
  update idletasks
}


proc xth_me_cmds_end_linecp_drag {x y dragto} {
  global xth

  xth_me_cmds_continue_linecp_drag $x $y $dragto
  set id $xth(me,lcpd,id)
  set pid $xth(me,lcpd,pid)
  set ppid $xth(me,lcpd,ppid)
  set npid $xth(me,lcpd,npid)
  set altpid $xth(me,lcpd,altpid)
  set altppid $xth(me,lcpd,altppid)
  set altnpid $xth(me,lcpd,altnpid)
  set tagOrId $xth(me,lcpd,tagOrId)

  set movecmd "xth_me_cmds_move_lineptcp_xctrl $id $ppid $pid $npid\nxth_me_cmds_move_linept_xctrl $id $pid\nxth_me_cmds_move_linelnpt $id $pid"

  switch $xth(me,lcpd,which) {
    x {
      set xth(me,cmds,$id,$pid,x) $xth(me,lcpd,oldx)
      set xth(me,cmds,$id,$pid,y) $xth(me,lcpd,oldy)
      set xth(me,cmds,$id,$pid,xp) $xth(me,lcpd,oldxp)
      set xth(me,cmds,$id,$pid,yp) $xth(me,lcpd,oldyp)
      set xth(me,cmds,$id,$pid,xn) $xth(me,lcpd,oldxn)
      set xth(me,cmds,$id,$pid,yn) $xth(me,lcpd,oldyn)
      if {$altpid > 0} {
	set xth(me,cmds,$id,$altpid,x) $xth(me,lcpd,oldx)
	set xth(me,cmds,$id,$altpid,y) $xth(me,lcpd,oldy)
	set xth(me,cmds,$id,$altpid,xp) $xth(me,lcpd,oldxp)
	set xth(me,cmds,$id,$altpid,yp) $xth(me,lcpd,oldyp)
	set xth(me,cmds,$id,$altpid,xn) $xth(me,lcpd,oldxn)
	set xth(me,cmds,$id,$altpid,yn) $xth(me,lcpd,oldyn)
      }
      set xth(me,unredola) "moving line point"
      $xth(me,can) itemconfigure $xth(me,canid,linept,pcpl) -state $xth(me,lcpd,oldmove,pcplstate)
      $xth(me,can) itemconfigure $xth(me,canid,linept,ncpl) -state $xth(me,lcpd,oldmove,ncplstate)
      $xth(me,can) itemconfigure $xth(me,canid,linept,selector) -state $xth(me,lcpd,oldmove,selstate)
      $xth(me,can) itemconfigure $xth(me,canid,linept,pcp) -fill $xth(me,lcpd,oldmove,pcpfill)
      $xth(me,can) itemconfigure $xth(me,canid,linept,ncp) -fill $xth(me,lcpd,oldmove,ncpfill)
      $xth(me,can) itemconfigure $xth(me,canid,linept,fr) -width 5 -arrow last
      $xth(me,can) itemconfigure $xth(me,canid,linept,fl) -width 5 -arrow last
    }
    p {
      set xth(me,cmds,$id,$pid,xp) $xth(me,lcpd,oldxp)
      set xth(me,cmds,$id,$pid,yp) $xth(me,lcpd,oldyp)
      set xth(me,cmds,$id,$pid,xn) $xth(me,lcpd,oldxn)
      set xth(me,cmds,$id,$pid,yn) $xth(me,lcpd,oldyn)
      if {$altpid > 0} {
	set xth(me,cmds,$id,$altpid,xp) $xth(me,lcpd,oldxp)
	set xth(me,cmds,$id,$altpid,yp) $xth(me,lcpd,oldyp)
	set xth(me,cmds,$id,$altpid,xn) $xth(me,lcpd,oldxn)
	set xth(me,cmds,$id,$altpid,yn) $xth(me,lcpd,oldyn)
      }
      set xth(me,unredola) "moving control pint"
    }
    n {
      set xth(me,cmds,$id,$pid,xp) $xth(me,lcpd,oldxp)
      set xth(me,cmds,$id,$pid,yp) $xth(me,lcpd,oldyp)
      set xth(me,cmds,$id,$pid,xn) $xth(me,lcpd,oldxn)
      set xth(me,cmds,$id,$pid,yn) $xth(me,lcpd,oldyn)
      if {$altpid > 0} {
	set xth(me,cmds,$id,$altpid,xp) $xth(me,lcpd,oldxp)
	set xth(me,cmds,$id,$altpid,yp) $xth(me,lcpd,oldyp)
	set xth(me,cmds,$id,$altpid,xn) $xth(me,lcpd,oldxn)
	set xth(me,cmds,$id,$altpid,yn) $xth(me,lcpd,oldyn)
      }
      set xth(me,unredola) "moving control pint"
    }
    pp {
      if {$altppid > 0} {
	set unaltcmd "set xth(me,cmds,$id,$altppid,xn) $xth(me,lcpd,oldxpp)\nset xth(me,cmds,$id,$altppid,yn) $xth(me,lcpd,oldypp)"
	set realtcmd "set xth(me,cmds,$id,$altppid,xn) $xth(me,cmds,$id,$ppid,xn)\nset xth(me,cmds,$id,$altppid,yn) $xth(me,cmds,$id,$ppid,yn)"
      } else {
	set unaltcmd ""
	set realtcmd ""
      }
      xth_me_unredo_action [mc "moving control point"] \
      "xth_me_cmds_select {$id $pid}\nset xth(me,cmds,$id,$ppid,xn) $xth(me,lcpd,oldxpp)\nset xth(me,cmds,$id,$ppid,yn) $xth(me,lcpd,oldypp)\n$unaltcmd\n$movecmd\nxth_me_cmds_update_line_data $id\nxth_me_prev_cmd [list $xth(me,cmds,$id,data)]" \
      "xth_me_cmds_select {$id $pid}\nset xth(me,cmds,$id,$ppid,xn) $xth(me,cmds,$id,$ppid,xn)\nset xth(me,cmds,$id,$ppid,yn) $xth(me,cmds,$id,$ppid,yn)\n$realtcmd\n$movecmd\nxth_me_cmds_update_line_data $id\nxth_me_prev_cmd [list $xth(me,cmds,$id,data)]"
    }
    nn {
      if {$altnpid > 0} {
	set unaltcmd "set xth(me,cmds,$id,$altnpid,xp) $xth(me,lcpd,oldxnn)\nset xth(me,cmds,$id,$altnpid,yp) $xth(me,lcpd,oldynn)"
	set realtcmd "set xth(me,cmds,$id,$altnpid,xp) $xth(me,cmds,$id,$npid,xp)\nset xth(me,cmds,$id,$altnpid,yp) $xth(me,cmds,$id,$npid,yp)"
      } else {
	set unaltcmd ""
	set realtcmd ""
      }
      xth_me_unredo_action [mc "moving control point"] \
      "xth_me_cmds_select {$id $pid}\nset xth(me,cmds,$id,$npid,xp) $xth(me,lcpd,oldxnn)\nset xth(me,cmds,$id,$npid,yp) $xth(me,lcpd,oldynn)\n$unaltcmd\n$movecmd\nxth_me_cmds_update_line_data $id\nxth_me_prev_cmd [list $xth(me,cmds,$id,data)]" \
      "xth_me_cmds_select {$id $pid}\nset xth(me,cmds,$id,$npid,xp) $xth(me,cmds,$id,$npid,xp)\nset xth(me,cmds,$id,$npid,yp) $xth(me,cmds,$id,$npid,yp)\n$realtcmd\n$movecmd\nxth_me_cmds_update_line_data $id\nxth_me_prev_cmd [list $xth(me,cmds,$id,data)]"
    }
  }

  $xth(me,can) bind $tagOrId <Enter> $xth(me,lcpd,oldenter)
  $xth(me,can) bind $tagOrId <Leave> $xth(me,lcpd,oldleave)
  $xth(me,can) itemconfigure $tagOrId -fill $xth(me,lcpd,oldfill)
  $xth(me,can) bind $tagOrId <B1-Motion> ""
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> ""
  $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-Motion> ""
  $xth(me,can) bind $tagOrId <$xth(kb_control)-B1-ButtonRelease> ""
  $xth(me,can) configure -cursor crosshair
  xth_me_cmds_update_line_data $id
  xth_me_prev_cmd $xth(me,cmds,$id,data)  
  xth_me_cmds_move_line_xctrl $id
  xth_me_cmds_update {}
  
}


proc xth_me_cmds_configure_linept_size_xctrl {id pid} {

  global xth
  
  if {([string length $id] > 0) && ($pid > 0)} {
    if {([string length $xth(me,cmds,$id,$pid,rs)] > 0)} {
      $xth(me,can) itemconfigure $xth(me,canid,linept,fr) -state normal
      $xth(me,can) bind $xth(me,canid,linept,fr) <1> \
	"xth_me_cmds_start_linept_fdrag $xth(me,canid,linept,fr) $id $pid r %x %y"
      $xth(me,can) bind $xth(me,canid,linept,fr) <Enter> \
	"$xth(me,can) itemconfigure $xth(me,canid,linept,fr) -fill #ffda00"
      $xth(me,can) bind $xth(me,canid,linept,fr) <Leave> \
	"$xth(me,can) itemconfigure $xth(me,canid,linept,fr) -fill red"
    } else {
      $xth(me,can) bind $xth(me,canid,linept,fr) <1> ""
      $xth(me,can) bind $xth(me,canid,linept,fr) <Enter> ""
      $xth(me,can) bind $xth(me,canid,linept,fr) <Leave> ""
      $xth(me,can) itemconfigure $xth(me,canid,linept,fr) -state hidden
    }
    if {([string length $xth(me,cmds,$id,$pid,ls)] > 0) || \
      (([string length $xth(me,cmds,$id,$pid,rotation)] > 0) && \
       ([string length $xth(me,cmds,$id,$pid,rs)] < 1))} {
      $xth(me,can) itemconfigure $xth(me,canid,linept,fl) -state normal
      $xth(me,can) bind $xth(me,canid,linept,fl) <1> \
	"xth_me_cmds_start_linept_fdrag $xth(me,canid,linept,fl) $id $pid l %x %y"
      $xth(me,can) bind $xth(me,canid,linept,fl) <Enter> \
	"$xth(me,can) itemconfigure $xth(me,canid,linept,fl) -fill #ffda00"
      $xth(me,can) bind $xth(me,canid,linept,fl) <Leave> \
	"$xth(me,can) itemconfigure $xth(me,canid,linept,fl) -fill red"
    } else {
      $xth(me,can) bind $xth(me,canid,linept,fl) <1> ""
      $xth(me,can) bind $xth(me,canid,linept,fl) <Enter> ""
      $xth(me,can) bind $xth(me,canid,linept,fl) <Leave> ""
      $xth(me,can) itemconfigure $xth(me,canid,linept,fl) -state hidden
    }
  } else {
      $xth(me,can) bind $xth(me,canid,linept,fr) <1> ""
      $xth(me,can) bind $xth(me,canid,linept,fr) <Enter> ""
      $xth(me,can) bind $xth(me,canid,linept,fr) <Leave> ""
      $xth(me,can) itemconfigure $xth(me,canid,linept,fr) -state hidden
      $xth(me,can) bind $xth(me,canid,linept,fl) <1> ""
      $xth(me,can) bind $xth(me,canid,linept,fl) <Enter> ""
      $xth(me,can) bind $xth(me,canid,linept,fl) <Leave> ""
      $xth(me,can) itemconfigure $xth(me,canid,linept,fl) -state hidden
  }
}


proc xth_me_cmds_move_linept_size_xctrl {id pid rot rs ls} {

  global xth
  
  set rotng 1
  if {[string length $rot] > 0} {
    set rot [expr double($rot) / 180.0 * 3.14159265359]
    set rotng 0
  } else {
    set rot [expr double([xth_me_cmds_get_default_rotation $id $pid]) / 180 * 3.14159265359]
  }

  if {[string length $rs] > 0} {
    set rs [expr 0.01 * $rs * $xth(me,zoom)]
  } else {
    set rs 30.0
  }

  if {[string length $ls] > 0} {
    set ls [expr 0.01 * $ls * $xth(me,zoom)]
  } else {
    set ls 30.0
  }


  set x [xth_me_real2canx $xth(me,cmds,$id,$pid,x)]
  set y [xth_me_real2cany $xth(me,cmds,$id,$pid,y)]
  set ca [expr cos($rot)]
  set sa [expr sin($rot)]
  
  set yvx [expr $sa * $ls]
  set yvy [expr - $ca * $ls]
  set xvx [expr - $sa * $rs]
  set xvy [expr $ca * $rs]
  
  if {$xth(me,cmds,$id,reverse) && $rotng} {
    set xvx [expr -1.0 * $xvx]
    set xvy [expr -1.0 * $xvy]
    set yvx [expr -1.0 * $yvx]
    set yvy [expr -1.0 * $yvy]
  }
  
  $xth(me,can) coords $xth(me,canid,linept,fr) $x $y [expr $x + $xvx] [expr $y + $xvy]
  $xth(me,can) coords $xth(me,canid,linept,fl) $x $y [expr $x + $yvx] [expr $y + $yvy]
  update idletasks    
}


proc xth_me_cmds_start_linept_fdrag {tagOrId id pid side x y} {
  global xth
  xth_me_cmds_update {}
  set xth(me,lptfd,tagOrId) $tagOrId
  set xth(me,lptfd,id) $id
  set xth(me,lptfd,pid) $pid
  set xth(me,lptfd,side) $side
  set dx [expr [xth_me_can2realx [$xth(me,can) canvasx $x]] - $xth(me,cmds,$id,$pid,x)]
  set dy [expr [xth_me_can2realy [$xth(me,can) canvasy $y]] - $xth(me,cmds,$id,$pid,y)] 

  if {[string length $xth(me,cmds,$id,$pid,rotation)] == 0} {
    set xth(me,lptfd,rot) 0
  } else {
    set xth(me,lptfd,rot) 1
    set xth(me,lptfd,orot) [expr atan2($dy,$dx)]
  }

  if {[string length $xth(me,cmds,$id,$pid,[format "%ss" $side])] == 0} {
    set xth(me,lptfd,size) 0
  } else {
    set xth(me,lptfd,size) 1
    set xth(me,lptfd,osize) [expr hypot($dy,$dx)]
  }

  $xth(me,can) itemconfigure $tagOrId -fill #ffda00
  set xth(me,lptfd,benter) [$xth(me,can) bind $tagOrId <Enter>]
  set xth(me,lptfd,bleave) [$xth(me,can) bind $tagOrId <Leave>]
  $xth(me,can) bind $tagOrId <Enter> ""
  $xth(me,can) bind $tagOrId <Leave> ""
  $xth(me,can) bind $tagOrId <B1-Motion> "xth_me_cmds_linept_fdrag %x %y"
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> "xth_me_cmds_end_linept_fdrag %x %y"
  xth_me_cmds_linept_fdrag $x $y
  $xth(me,can) configure -cursor {}
}

proc xth_me_cmds_linept_fdrag {x y} {
  global xth
  set id $xth(me,lptfd,id)
  set pid $xth(me,lptfd,pid)
  set side $xth(me,lptfd,side)

  set dx [expr [xth_me_can2realx [$xth(me,can) canvasx $x]] - $xth(me,cmds,$id,$pid,x)]
  set dy [expr [xth_me_can2realy [$xth(me,can) canvasy $y]] - $xth(me,cmds,$id,$pid,y)]
  
  if $xth(me,lptfd,rot) {
    set rot [expr double($xth(me,cmds,$id,$pid,rotation)) - 180.0 / 3.14159265359 * (atan2($dy,$dx) - $xth(me,lptfd,orot))]
    if {$rot < 0.0} {
      set rot [expr 360.0 + $rot]
    } elseif {$rot >= 360.0} {
      set rot [expr $rot - 360.0]
    }
    set rot [format "%.1f" $rot]
    set xth(ctrl,me,linept,rot) $rot
  }
  
  if {$xth(me,lptfd,size)} {
    set cs [expr hypot($dy,$dx)]
    set ns [expr $xth(me,cmds,$id,$pid,[format "%ss" $side]) - $xth(me,lptfd,osize) + $cs]
    if {$ns <= 0.0} {set ns 0.1}
    set xth(ctrl,me,linept,[format "%ss" $side]) [format "%.1f" $ns]
  }

  xth_me_cmds_move_linept_size_xctrl $id $pid $xth(ctrl,me,linept,rot) $xth(ctrl,me,linept,rs) $xth(ctrl,me,linept,ls)
  xth_me_cmds_move_line_xctrl $id
}

proc xth_me_cmds_end_linept_fdrag {x y} {
  global xth

  xth_me_cmds_linept_fdrag $x $y

  set id $xth(me,lptfd,id)
  set pid $xth(me,lptfd,pid)
  set side $xth(me,lptfd,side)
  set tagOrId $xth(me,lptfd,tagOrId)

  $xth(me,can) configure -cursor crosshair
  if {$xth(me,lptfd,size)} {
    set xth(me,unredola) "line point resizing"
  } else {
    set xth(me,unredola) "line point rotation"
  }
  $xth(me,can) bind $tagOrId <B1-Motion> ""
  $xth(me,can) bind $tagOrId <B1-ButtonRelease> ""
  if {[lsearch [$xth(me,can) itemcget $tagOrId -tags] current] > -1} {
    $xth(me,can) itemconfigure $tagOrId -fill #ffda00
  }
  
  $xth(me,can) bind $tagOrId <Enter> $xth(me,lptfd,benter)
  $xth(me,can) bind $tagOrId <Leave> $xth(me,lptfd,bleave)
  xth_me_cmds_update {}
}


proc xth_me_cmds_set_colors {} {
  global xth
  # najde id zaciatku a konca sucasneho scrapu
  set xid [lsearch $xth(me,cmds,xlist) $xth(me,cmds,selid)]
  set llen [llength $xth(me,cmds,xlist)]
  set cid $xid

  set dcol #fff222
  set scol $xth(gui,me,pasivefill)
  if {$xth(me,cmds,$xth(me,cmds,selid),ct) == 4} {
    set col $dcol
    set ocol $scol
  } elseif {$xth(me,cmds,$xth(me,cmds,selid),ct) == 5} {
    set col $scol
    set ocol $dcol
  } else {
    set col $scol
    set ocol $scol
  }

  set xth(me,curscrap) {}
  set godown 1
  if {$cid < 0} {
    set cid [expr $xid + 1]
    set godown 0
  }
  while {(($cid >= 0) && ($cid < $llen)) || ($godown)} {
    set id [lindex $xth(me,cmds,xlist) $cid]
    switch $xth(me,cmds,$id,ct) {
      2 {        
        $xth(me,can) itemconfigure pt$id -outline $col -fill $col -state normal
        if {$xth(me,hinactives) && ($col == $dcol)} {
          $xth(me,can) itemconfigure pt$id -state hidden
        }
      }
      3 {
        $xth(me,can) itemconfigure lnpt$id -outline $col -fill $col -state normal
        $xth(me,can) itemconfigure lnln$id -fill $col -state normal
        if {$xth(me,hinactives) && ($col == $dcol)} {
          $xth(me,can) itemconfigure ln$id -state hidden  
        }
      }
      4 - 5 {
        if {(![string equal $col $dcol]) && ($xth(me,cmds,$id,ct) == 4)} {
          set xth(me,curscrap) $xth(me,cmds,$id,name)
          #          if {[string equal $xth(me,cmds,$id,projection) extended]} {
          #            set xth(me,snai) -1
          #          } else {
          #            set xth(me,snai) 1
          #          }
          set xth(me,snai) 1
        }
        if {$cid != $xid} {
          set col $dcol
        }
      }
    }

    if {$godown} {
      incr cid -1
      if {$cid < 0} {
        set cid [expr $xid + 1]
        set godown 0
        set col $ocol
      }
    } else {
      incr cid 1
    }
  }
  xth_app_title me
}


proc xth_me_cmds_show_current_area {} {

  global xth

  set id $xth(me,cmds,selid)
  if {$xth(me,cmds,$id,ct) != 6} {
    return
  }

  set xid [expr [lsearch $xth(me,cmds,xlist) $id] + 1]
  set llen [llength $xth(me,cmds,xlist)]
  set cid $xid

  
  set godown 1
  if {$cid < 0} {
    set cid [expr $xid - 1]
    set godown 0
  }

  set llist $xth(me,cmds,$id,llist)
  set llast [expr [llength $llist] - 1]
  set lcsel [$xth(ctrl,me,ac).ll.l curselection]
  set lsel {}
  if {([llength $lcsel] > 0) && ($lcsel < $llast)} {
    set lsel [lindex $llist $lcsel]
  }
  
  while {(($cid >= 0) && ($cid < $llen)) || ($godown)} {
    set oid [lindex $xth(me,cmds,xlist) $cid]
    switch $xth(me,cmds,$oid,ct) {
      3 {
	foreach lnid $llist {
	  if {[string equal $xth(me,cmds,$oid,name) $lnid]} {
	    if {([llength $lsel] == 0) || [string equal $lsel $lnid]} {
	      $xth(me,can) itemconfigure lnpt$oid -fill red
	      $xth(me,can) itemconfigure lnln$oid -fill red
	    } else {
	      $xth(me,can) itemconfigure lnpt$oid -fill $xth(gui,me,pasivefill)
	      $xth(me,can) itemconfigure lnln$oid -fill $xth(gui,me,pasivefill)
	    }
	  }
	}
      }
      4 {
	return
      }
      5 {
	if {$godown} {
	  if {$cid != $xid} {
	    set cid [expr $xid - 1]
	    set godown 0
	  }
	} else return
      }
    }
    
    if {$godown} {
      incr cid 1      
      if {$cid >= $llen} {
	set cid [expr $xid - 1]
	set godown 0
      }
    } else {
      incr cid -1
    }
    
  }
}




proc xth_me_cmds_line_split {} {

  global xth
  
  xth_me_cmds_update {}
  
  set id $xth(me,cmds,selid)
  set pid $xth(me,cmds,selpid)  
	
	set prev_center [xth_me_get_center]

  # najprv zisti ci mooze, ak nie tak exit
  if {$xth(me,cmds,$id,ct) != 3} {
    return
  }
  
  set px [lsearch -exact $xth(me,cmds,$id,xplist) $pid]
  set lpx [llength $xth(me,cmds,$id,xplist)]
  if {($px <= 0) || ($px >= ($lpx - 2))} {
    return
  }
  
  # vytvori dve nove ciary, close nastavene na false
  set xth(me,unredook) 0
  set ix [lsearch $xth(me,cmds,xlist) $id]
  
  set id1 [xth_me_cmds_create 3 {} {}]
  set xth(me,cmds,$id1,lpid) 0
  set xth(me,cmds,$id1,plist) {"end of line"}
  set xth(me,cmds,$id1,xplist) {0}
  set id2 [xth_me_cmds_create 3 {} {}]
  set xth(me,cmds,$id2,lpid) 0
  set xth(me,cmds,$id2,plist) {"end of line"}
  set xth(me,cmds,$id2,xplist) {0}
  
  set xth(me,cmds,$id1,type) $xth(me,cmds,$id,type)
  set xth(me,cmds,$id2,type) $xth(me,cmds,$id,type)
  set xth(me,cmds,$id1,name) {}
  set xth(me,cmds,$id2,name) {}
  set xth(me,cmds,$id1,reverse) $xth(me,cmds,$id,reverse)
  set xth(me,cmds,$id2,reverse) $xth(me,cmds,$id,reverse)
  set xth(me,cmds,$id1,close) 0
  set xth(me,cmds,$id2,close) 0
  set xth(me,cmds,$id1,options) $xth(me,cmds,$id,options)
  set xth(me,cmds,$id2,options) $xth(me,cmds,$id,options)

  xth_me_cmds_update_list $id1
  xth_me_cmds_update_list $id2
  
  # povklada body
  set ix1 0
  set ix2 0
  for {set cx 0} {$cx < ($lpx - 1)} {incr cx} {
    set opid [lindex $xth(me,cmds,$id,xplist) $cx]
    if {$cx <= $px} {
      #vlozi ho do prvej      
      xth_me_cmds_create_line_point $id1 $ix1 0 \
      $xth(me,cmds,$id,$opid,x) $xth(me,cmds,$id,$opid,y) \
      $xth(me,cmds,$id,$opid,xp) $xth(me,cmds,$id,$opid,yp) \
      $xth(me,cmds,$id,$opid,xn) $xth(me,cmds,$id,$opid,yn) \
      $xth(me,cmds,$id,$opid,smooth) $xth(me,cmds,$id,$opid,rotation) \
      $xth(me,cmds,$id,$opid,rs) $xth(me,cmds,$id,$opid,ls) \
      $xth(me,cmds,$id,$opid,options) 1.0
      incr ix1
    }
    if {$cx >= $px} {
      #vlozi ho do druhej
      xth_me_cmds_create_line_point $id2 $ix2 0 \
      $xth(me,cmds,$id,$opid,x) $xth(me,cmds,$id,$opid,y) \
      $xth(me,cmds,$id,$opid,xp) $xth(me,cmds,$id,$opid,yp) \
      $xth(me,cmds,$id,$opid,xn) $xth(me,cmds,$id,$opid,yn) \
      $xth(me,cmds,$id,$opid,smooth) $xth(me,cmds,$id,$opid,rotation) \
      $xth(me,cmds,$id,$opid,rs) $xth(me,cmds,$id,$opid,ls) \
      $xth(me,cmds,$id,$opid,options) 1.0
      incr ix2
    }
  }
  
  xth_me_cmds_update_line_data $id1
  xth_me_cmds_update_line_data $id2
  
  # zmaze originalnu
  set pid1 0
  set pid2 [lindex $xth(me,cmds,$id2,xplist) 0]
  xth_me_cmds_delete $id
  xth_me_cmds_select [list $id2 $pid2]
	
	# scrollne sa tam, odkial sme zacinali
	xth_me_center_to $prev_center

  set xth(me,unredook) 1
  
  # nastavi undo na zmazanie novych a undelete originalnej
  # a redo na undelete novych a zmazanie originalnej
  xth_me_unredo_action [mc "split line"] \
    "xth_me_cmds_delete $id1; xth_me_cmds_delete $id2; xth_me_cmds_undelete $id $pid $ix; xth_me_center_to {$prev_center}" \
    "xth_me_cmds_undelete $id1 $pid1 $ix; xth_me_cmds_undelete $id2 $pid2 $ix; xth_me_cmds_delete $id; xth_me_cmds_select {$id2 $pid2}; xth_me_center_to {$prev_center}"

}

# todo - zistovat, ci sme nad inymi, ako prave startovacimi pt a lnpt
# pridat button na tracovanie a stop
# pridat vytvorenie bezierovej krivky
proc xth_me_image_get_color {x y {include {}} {exclude {}}} {
  global xth
  # najde obrazok na ktory sme klikli - ak taky ma, spocita suradnice a vrati RGB
  set iid {}
  foreach id [$xth(me,can) find overlapping [xth_me_real2canx $x] [xth_me_real2cany $y] [xth_me_real2canx $x] [xth_me_real2cany $y]] {
    set tags [$xth(me,can) itemcget $id -tags]
    if {[regexp {bgimg.*imgx(\d+)} $tags dum tmpiid]} {
      set iid $tmpiid
    }
    if {[regexp {command} $tags]} {      
      if {([string length $include] > 0) && (!([regexp $include $tags])) && ([regexp $exclude $tags])} {
	return 1
      }
    }
  }
  if {[string length $iid] == 0} {
    return {}
  }
  # calculate color
  set img $xth(me,imgs,$iid,image)
  set iw [image width $img]
  set ih [image height $img]
  set ix [expr round($x - [lindex $xth(me,imgs,$iid,position) 0])]
  set iy [expr round([lindex $xth(me,imgs,$iid,position) 1] - $y)]
  if {$ix < 0} {set ix 0}
  if {$ix >= $iw} {set ix [expr $iw - 1]}
  if {$iy < 0} {set iy 0}
  if {$iy >= $ih} {set iy [expr $ih - 1]}
  return [$img get $ix $iy]
}

proc xth_me_cmds_line_trace_stop {} {
  global xth
  set xth(ctrl,me,line,tracecontinue) 0  
}

proc xth_me_cmds_line_trace_start {} {
  global xth
  set xth(ctrl,me,line,tracedist) 0.0
  xth_me_cmds_line_trace
}

proc xth_me_cmds_line_trace {} {
  global xth
  xth_me_cmds_update {}
  set xth(ctrl,me,line,tracecontinue) 1
  if {$xth(ctrl,me,line,tracerow) > 0} {
    grid $xth(ctrl,me,line).trace -row $xth(ctrl,me,line,tracerow) -column 0 -sticky news
    grid $xth(ctrl,me,line).vector -row $xth(ctrl,me,line,tracerow) -column 1 -sticky news
    set xth(ctrl,me,line,tracebtnfg) [$xth(ctrl,me,line).trace cget -fg]
    set xth(ctrl,me,line,tracebtnbg) [$xth(ctrl,me,line).trace cget -bg]
    set xth(ctrl,me,line,tracebtnafg) [$xth(ctrl,me,line).trace cget -activeforeground]
    set xth(ctrl,me,line,tracebtnabg) [$xth(ctrl,me,line).trace cget -activebackground]
    set xth(ctrl,me,line,tracerow) 0
  }
  set id $xth(me,cmds,selid)
  set npx [llength $xth(me,cmds,$id,xplist)]
  if {$npx < 3} {return}
  # zoberieme posledne 2 body na ciare
  set px0 [lindex $xth(me,cmds,$id,xplist) [expr $npx - 3]]
  set px1 [lindex $xth(me,cmds,$id,xplist) [expr $npx - 2]]
  set xth(me,cmds,selpid) 0
  # spocitame ich vzdialenost
  set x0 $xth(me,cmds,$id,$px0,x)
  set y0 $xth(me,cmds,$id,$px0,y)
  set x1 $xth(me,cmds,$id,$px1,x)
  set y1 $xth(me,cmds,$id,$px1,y)
  set dx [expr $x1 - $x0]
  set dy [expr $y1 - $y0]
  set l [expr hypot($dx, $dy)]
  if {($x0 == $x1) && ($y0 == $y1)} {
    return
  }
  if {$l < 1.0} {set l 1.0}
  set tcol [xth_me_image_get_color $x1 $y1]
  if {[llength $tcol] != 3} {return}
  if {$xth(ctrl,me,line,tracedist) == 0.0} {
    set xth(ctrl,me,line,tracedist) $l
    set xth(ctrl,me,line,tracecolor) $tcol
    set dist $l
  } else {
    set dist $xth(ctrl,me,line,tracedist)
    set tcol $xth(ctrl,me,line,tracecolor)
  }
  xth_me_cmds_line_trace_point_cycle $dist $tcol
  return
}

proc xth_me_cmds_line_trace_point_cycle {dist tcol} {
    global xth
    set next [xth_me_cmds_line_trace_point $dist $tcol]
    set cont [expr [llength $next] > 0]
    if {(!$cont) && ($dist > 2.0)} {
      set next [xth_me_cmds_line_trace_point [expr $dist / 2.0] $tcol]
      set cont [expr [llength $next] > 0]
    }
    if {(!$cont)} {
      set next [xth_me_cmds_line_trace_point [expr $dist * 2.0] $tcol]
      set cont [expr [llength $next] > 0]
    }
    if {$cont && $xth(ctrl,me,line,tracecontinue) && ([llength $next] == 2)} {
      $xth(ctrl,me,line).trace configure -command {xth_me_cmds_line_trace_stop} -text [mc "Stop tracing"] \
        -fg white -bg red -activeforeground white -activebackground red
      set lt 0
      set ld 100
      catch {
        set lt $xth(ctrl,me,line,traceltime)
        set ld $xth(ctrl,me,line,traceldelay)
      }
      set xth(ctrl,me,line,traceltime) [clock clicks -milliseconds]
      set cd [expr 100 - $xth(ctrl,me,line,traceltime) + $lt]
      if {$cd < 0} {
        set cd 0
      }
      if {$cd > 100} {
        set cd 100
      }
      set cd [expr int(0.95 * $ld + 0.05 * $cd)]
      set xth(ctrl,me,line,traceldelay) $cd
      after $cd "after idle \"xth_me_cmds_line_trace_point_cycle $dist [list $tcol]\""
    } else {
      $xth(ctrl,me,line).trace configure -command {xth_me_cmds_line_trace} -text [mc "Continue tracing"] \
        -fg $xth(ctrl,me,line,tracebtnfg) -bg $xth(ctrl,me,line,tracebtnbg) \
        -activeforeground $xth(ctrl,me,line,tracebtnafg) -activebackground $xth(ctrl,me,line,tracebtnabg)
    }
}


proc xth_me_cmds_line_trace_point {dist tcol} {

  global xth
  set id $xth(me,cmds,selid)
  if {$xth(me,cmds,$id,ct) != 3} {return}
  set npx [llength $xth(me,cmds,$id,xplist)]
  if {$npx < 3} {return}
  if {$xth(me,cmds,$id,close)} {return}

  # zoberieme posledne 2 body na ciare
  set pxs [lindex $xth(me,cmds,$id,xplist) 0]
  set px0 [lindex $xth(me,cmds,$id,xplist) [expr $npx - 3]]
  set px1 [lindex $xth(me,cmds,$id,xplist) [expr $npx - 2]]
  set xth(me,cmds,selpid) 0
  
  set itags "pt$id\\.$px1|ln$id\\.$px1"
  set etags "pt$id\\.\\d+|ln$id\\.\\d+"
  
  # spocitame ich vzdialenost
  set xs $xth(me,cmds,$id,$pxs,x)
  set ys $xth(me,cmds,$id,$pxs,y)
  set x0 $xth(me,cmds,$id,$px0,x)
  set y0 $xth(me,cmds,$id,$px0,y)
  set x1 $xth(me,cmds,$id,$px1,x)
  set y1 $xth(me,cmds,$id,$px1,y)
  set dx [expr $x1 - $x0]
  set dy [expr $y1 - $y0]
  set l [expr hypot($dx, $dy)]
  if {($x0 == $x1) && ($y0 == $y1)} {
    return
  }
  set dx [expr $dx / $l * $dist]
  set dy [expr $dy / $l * $dist]
  
  set resol 1.0
  xth_me_center_to [list $x1 $y1]
  set da [expr atan2($resol, $dist)]
  set maxta [expr $da * (2.0 * $dist / $resol)]
  set minta [expr -1.0 * $maxta]
  set tR [lindex $tcol 0]
  set tG [lindex $tcol 1]
  set tB [lindex $tcol 2]
  if {[llength $tcol] == 0} {
    return
  }
  
  # spocitame polkruznicu
  set mode 0
  set ta 0.0
  set sa {}
  set ea {}
  set cstep 0.0
  set goback 0
  while {$mode >= 0} {
    set tx [expr $x1 + $dx * cos($ta) + $dy * sin($ta)]
    set ty [expr $y1 - $dx * sin($ta) + $dy * cos($ta)]
    set ccol [xth_me_image_get_color $tx $ty $itags $etags]
    if {[llength $ccol] != 3} {
      set ceq 0
    } else {
      set dR [expr [lindex $ccol 0] - $tR]
      set dG [expr [lindex $ccol 1] - $tG]
      set dB [expr [lindex $ccol 2] - $tB]
      set ceq [expr [expr ($dR * $dR) + ($dG * $dG) + ($dB * $dB)] < 1600]
    }
    switch $mode {
      0 {
	if {$ceq} {
	  set sa $ta
	  set ea $ta
	  if {$ta == 0.0} {
	    set mode 1
	    set goback 1
	  } elseif {$ta < 0.0} {
	    set mode 1
	  } else {
	    set mode 2
	  }
	} else {
	  if {$ta == 0.0} {
	    set ta $da
	  } elseif {$ta < 0.0} {
	    set ta [expr -1.0 * $ta + $da]
	  } else {
	    set ta [expr -1.0 * $ta]
	  }
	}
      }
      1 {
	# odratavame
	if {$ceq} {
	  set sa $ta
	  set ta [expr $ta - $da]
	} else {
	  if $goback {
	    set ta [expr $ea + $da]
	    set mode 2
	    set goback 0
	  } else {
	    set mode -1
	  }
	}
      }
      2 {
	# priratavame
	if {$ceq} {
	  set ea $ta
	  set ta [expr $ta + $da]
	} else {
	  if $goback {
	    set ta [expr $sa - $da]
	    set mode 1
	    set goback 0
	  } else {
	    set mode -1
	  }
	}
      }
    }    
    if {($ta > $maxta) || ($ta < $minta)} {
      set mode -1
    }
  }
  if {[llength $sa] == 0} {
    return
  }
  
  # vypocitame bod
  set ta [expr 0.5 * ($sa + $ea)]
  set tx [expr $x1 + $dx * cos($ta) + $dy * sin($ta)]
  set ty [expr $y1 - $dx * sin($ta) + $dy * cos($ta)]
  
  set ok 0
  set tryclose [expr ($npx > 3) && (hypot($x1 - $xs, $y1 - $ys) < $dist)]
  if {$tryclose} {
    set ttx $tx
    set tty $ty
    set tx $xs
    set ty $ys
  }
  while {!$ok} {
    set nin 0
    set nto 0
    for {set t 0.0} {$t <= $dist} {set t [expr $t + 1.0]} {
      set lx [expr $x1 + ($tx - $x1) / $dist * $t]
      set ly [expr $y1 + ($ty - $y1) / $dist * $t]
      if {$tryclose} {
	set ccol [xth_me_image_get_color $lx $ly]
      } else {
	set ccol [xth_me_image_get_color $lx $ly $itags $etags]
      }
      if {[llength $ccol] == 1} {
	return
      } elseif {[llength $ccol] == 0} {
	set ceq 0
      } else {
	set dR [expr [lindex $ccol 0] - $tR]
	set dG [expr [lindex $ccol 1] - $tG]
	set dB [expr [lindex $ccol 2] - $tB]
	set ceq [expr [expr ($dR * $dR) + ($dG * $dG) + ($dB * $dB)] < 1600]
      }
      if {$ceq} {incr nin}
      incr nto
    }
    if {double($nin)/double($nto) < 0.5} {
      if {$tryclose} {
	set tryclose 0
	set tx $ttx
	set ty $tty
      } else {
	return
      }
    } else {
      set ok 1
    }
  }
  
  # vlozi bod na ciare
  if {$tryclose} {
    xth_me_cmds_close_line $id
    return 1
  } else {
    xth_me_cmds_start_create_linept {} [format "%.2f" $tx] [format "%.2f" $ty] 0 0
    xth_me_cmds_end_create_linept 0 0 0
    return [list $tx $ty]
  }

}

proc xth_me_cmds_line_poly2bezier {} {
  global xth
  set id $xth(me,cmds,selid)
  if {$xth(me,cmds,$id,ct) != 3} {return}
  set npx [llength $xth(me,cmds,$id,xplist)]
  if {$npx < 4} {return}
  foreach x $xth(me,cmds,$id,xplist) {
    if {($x != 0) && ($xth(me,cmds,$id,$x,idn) || $xth(me,cmds,$id,$x,idp))} return
  }
  set cdir [pwd]
  cd $xth(me,fpath)
  set f [open "therion.bci" w]
  foreach x $xth(me,cmds,$id,xplist) {
    if {$x != 0} {
      puts $f "$xth(me,cmds,$id,$x,x) $xth(me,cmds,$id,$x,y)"
    }
  }
  close $f
  set f [open "therion.bco" w]
  close $f
  update idletasks
  catch {
    set thid [open "|$xth(gui,compcmd) -b" r]
    read $thid
    close $thid
  } 
  set data {}
  catch {
    set f [open "therion.bco" r]
    while {![eof $f]} {
      set ln [gets $f]
      if {[regexp {\d+} $ln]} {
        lappend data $ln
      }
    }
    close $f
  }
  catch {
    file delete therion.bci
    file delete therion.bco
  }
  cd $cdir
  if {[llength $data] == 0} {return}
  set type $xth(me,cmds,$id,type)
  set opts " $xth(me,cmds,$id,options)"
  if {$xth(me,cmds,$id,reverse)} {
    set opts "$opts -reverse on"
  }
  if {$xth(me,cmds,$id,close)} {
    set opts "$opts -close on"
  }
  xth_me_cmds_delete $id
  xth_me_cmds_create_line 0 2 $type $opts $data
  xth_me_cmds_set_mode 0
  return
}
  