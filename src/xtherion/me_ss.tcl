##
## me_ss.tcl --
##
##     Map editor search & select tools.
##
## Copyright (C) 2003 Stacho Mudrak
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


proc xth_me_ss_next {} {
  global xth
  xth_me_cmds_update {}
  set cselid $xth(me,cmds,selid)
  set cselpid $xth(me,cmds,selpid)
  set eofvalid [expr $cselid == 0]
  xth_status_bar_push me
  xth_status_bar_status me "Searching ..."
  if {$cselid == 0} {
    set totalcnt [llength $xth(me,cmds,xlist)]
  } else {
    set totalcnt [expr [llength $xth(me,cmds,xlist)] - [lsearch -exact -integer $xth(me,cmds,xlist) $cselid] - 1]
  }
  set cnt 0
  xth_me_progbar_show $totalcnt
  while {($cselid != 0) || $eofvalid} {
    set eofvalid 0    
    # prejde na dalsi objekt
    if {$cselid == 0} {
      set cselid [lindex $xth(me,cmds,xlist) 0]
      if {$xth(me,cmds,$cselid,ct) == 3} {
        set cselpid [lindex $xth(me,cmds,$cselid,xplist) 0]
      } else {
        set cselpid 0
      }
      incr cnt
    } elseif {($xth(me,cmds,$cselid,ct) != 3) || ($cselpid == 0)} {
      set cselid [lindex $xth(me,cmds,xlist) \
          [expr [lsearch -exact -integer $xth(me,cmds,xlist) $cselid] + 1]]
      if {$xth(me,cmds,$cselid,ct) == 3} {
        set cselpid [lindex $xth(me,cmds,$cselid,xplist) 0]
      } else {
        set cselpid 0
      }
      incr cnt
    } else {
      set cselpid [lindex $xth(me,cmds,$cselid,xplist) \
          [expr [lsearch -exact -integer $xth(me,cmds,$cselid,xplist) $cselpid] + 1]]
    }
    xth_me_progbar_prog $cnt
    switch $xth(me,cmds,$cselid,ct) {
      0 {
        xth_me_cmds_select 0
        xth_me_progbar_hide
        xth_status_bar_pop me
        return
      }
      3 {
        if {$cselpid == 0} {
          if {[xth_me_ss_match $xth(me,cmds,$cselid,data_ln)]} {
            xth_me_cmds_select "$cselid 0"
            xth_me_progbar_hide
            xth_status_bar_pop me
            return
          }
        } else {
          if {[xth_me_ss_match [lindex $xth(me,cmds,$cselid,data_pt) \
              [lsearch -exact -integer $xth(me,cmds,$cselid,xplist) $cselpid]]]} {
            xth_me_cmds_select "$cselid $cselpid"
            xth_me_progbar_hide
            xth_status_bar_pop me
            return
          }
        }
      }
      default {
        if {[xth_me_ss_match $xth(me,cmds,$cselid,data)]} {
          xth_me_cmds_select $cselid
          xth_me_progbar_hide
          xth_status_bar_pop me
          return
        }
      }
    }
  }
  xth_me_progbar_hide
  xth_status_bar_pop me
}

proc xth_me_ss_first {} {
  global xth
  xth_me_cmds_update {}
  xth_me_cmds_select 0
  xth_me_ss_next
}


proc xth_me_ss_match {s} {
  global xth
  if $xth(ctrl,me,ss,regexp) {
    if $xth(ctrl,me,ss,cases) {
      return [regexp $xth(ctrl,me,ss,expr) $s]
    } else {
      return [regexp -nocase $xth(ctrl,me,ss,expr) $s]
    }
  } else {
    if $xth(ctrl,me,ss,cases) {
      if {[string first $xth(ctrl,me,ss,expr) $s] >= 0} {
        return 1
      } else {
        return 0
      }
    } else {
      if {[string first [string tolower $xth(ctrl,me,ss,expr)] [string tolower $s]] >= 0} {
        return 1
      } else {
        return 0
      }
    }
  }
}


proc xth_me_ss_show {} {
  global xth
  xth_me_cmds_update {}
  xth_me_cmds_set_colors
  set selcol red
  xth_status_bar_push me
  xth_status_bar_status me "Searching ..."
  xth_me_progbar_show [llength $xth(me,cmds,xlist)]
  set objcnt 0
  foreach id $xth(me,cmds,xlist) {
    incr objcnt
    xth_me_progbar_prog $objcnt
    switch $xth(me,cmds,$id,ct) {
      2 {
        if {[xth_me_ss_match $xth(me,cmds,$id,data)]} {
          # oznaci bod
          $xth(me,can) itemconfigure pt$id -fill $selcol
        }
      }
      3 {
        set pnm 0
        set trywhole 1
        foreach tx $xth(me,cmds,$id,data_pt) {
          if {[xth_me_ss_match $tx]} {
            set pid [lindex $xth(me,cmds,$id,xplist) $pnm]
            # oznaci bod na ciare
            set trywhole 0
            $xth(me,can) itemconfigure pt$id.$pid -fill $selcol
            set ppid [lindex $xth(me,cmds,$id,xplist) [expr $pnm + 1]]
            if {[string length $ppid] > 0} {
              $xth(me,can) itemconfigure ln$id.$ppid -fill $selcol
            }
          }
          incr pnm 1
        }
        if {$trywhole && [xth_me_ss_match $xth(me,cmds,$id,data_ln)]} {
          # oznaci ciaru
          $xth(me,can) itemconfigure lnln$id -fill $selcol
        }
      }
    }
  }
  xth_status_bar_pop me
  xth_me_progbar_hide
}
