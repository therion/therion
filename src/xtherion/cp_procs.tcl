##
## cp_procs.tcl --
##
##     Compiler procedures.   
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


xth_about_status "loading compiler..."


proc xth_cp_new_file {} {
  global xth
  if {$xth(cp,fopen)} {
    return
  }
  set xth(cp,fopen) 1
  set xth(cp,special) {}
  set xth(cp,ffull) "thconfig"
  if {[xth_cp_save_as]} {
    set ff $xth(cp,ffull)
    set xth(cp,fopen) 0
    set xth(cp,fname) ""
    set xth(cp,open_file) ""
    set xth(cp,fpath) ""
    set xth(cp,ffull) ""
    xth_cp_open_file $ff
  } else {
    set xth(cp,fopen) 0
    set xth(cp,fname) ""
    set xth(cp,open_file) ""
    set xth(cp,fpath) ""
    set xth(cp,ffull) ""
  }
}

proc xth_cp_open_file {fpath} {
  global xth
  
  if {$xth(cp,fopen)} {
    return
  }

  if {[string length $fpath] == 0} {
    set fpath [tk_getOpenFile -filetypes $xth(app,cp,filetypes) \
      -parent $xth(gui,main) -initialdir $xth(gui,initdir)]
  }

  if {[string length $fpath] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fpath]
  }

  # read the file
  xth_status_bar_push cp
  xth_status_bar_status cp "Opening $fpath ..."

  set fdata [xth_me_read_file $fpath]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message [lindex $fdata 1] \
        -font $xth(gui,lfont)
      xth_status_bar_pop cp
      return 0
  }
  
  # now let's show the file
  foreach cmd [lindex $fdata 2] {
    catch {eval $cmd}
  }
  
  set xth(cp,fopen) 1
  set xth(cp,special) [lindex $fdata 2]
  set xth(cp,fname) [file tail $fpath]
  set xth(cp,open_file) [file tail $fpath]
  set xth(cp,fpath) [file dirname $fpath]
  set xth(cp,ffull) $fpath
  
  # enable controls
  $xth(cp,editor).txt configure -state normal
#  $xth(cp,editor).txt delete 1.0 end
  foreach ln [lindex $fdata 3] {
    $xth(cp,editor).txt insert end "$ln\n"
  }
  $xth(cp,editor).txt mark set insert $xth(cp,cursor)
  $xth(cp,editor).txt see $xth(cp,cursor)
  
  $xth(ctrl,cp,stp).wl configure -state normal
  $xth(ctrl,cp,stp).we configure -state normal
  $xth(ctrl,cp,stp).fl configure -state normal
  $xth(ctrl,cp,stp).fe configure -state normal
  $xth(ctrl,cp,stp).upd configure -state normal
  $xth(ctrl,cp,stp).go configure -state normal
  $xth(ctrl,cp,stp).gores configure -state normal

  $xth(cp,menu,file) entryconfigure "New" -state disabled
  $xth(cp,menu,file) entryconfigure "Open" -state disabled
  $xth(cp,menu,file) entryconfigure "Save as" -state normal
  $xth(cp,menu,file) entryconfigure "Close" -state normal
  $xth(cp,menu) entryconfigure "Edit" -state normal
  
  xth_app_title cp
  xth_status_bar_pop cp
  update idletasks
  return 1
}


proc xth_cp_close_file {} {
  
  global xth

  xth_cp_data_tree_clear      

  if {!$xth(cp,fopen)} {
    return
  }
  
  xth_cp_write_file $xth(cp,ffull)

  # disable controls
  $xth(cp,editor).txt delete 1.0 end
  $xth(cp,editor).txt see 1.0
  $xth(cp,editor).txt configure -state disabled

  $xth(cp,log).txt configure -state normal
  $xth(cp,log).txt delete 1.0 end
  $xth(cp,log).txt see 1.0
  $xth(cp,log).txt configure -state disabled

  $xth(cp,menu,file) entryconfigure "New" -state normal
  $xth(cp,menu,file) entryconfigure "Open" -state normal
  $xth(cp,menu,file) entryconfigure "Save as" -state disabled
  $xth(cp,menu,file) entryconfigure "Close" -state disabled
  $xth(cp,menu) entryconfigure "Edit" -state disabled
  
  $xth(ctrl,cp,stp).wl configure -state disabled
  $xth(ctrl,cp,stp).we configure -state disabled
  $xth(ctrl,cp,stp).fl configure -state disabled
  $xth(ctrl,cp,stp).fe configure -state disabled
  $xth(ctrl,cp,stp).upd configure -state disabled
  $xth(ctrl,cp,stp).go configure -state disabled
  $xth(ctrl,cp,stp).gores configure -text "" -fg $xth(cp,resfg) -bg $xth(cp,resbg) \
    -state disabled
  
  # set variables
  set xth(cp,fopen) 0
  set xth(cp,cursor) 1.0
  set xth(cp,fname) ""
  set xth(cp,open_file) ""
  set xth(cp,fpath) ""
  xth_app_title cp
  
}


# xth_cp_write_file --
#
# return list containing
# {success name}

proc xth_cp_write_file {pth} {

  global errorInfo xth

  xth_status_bar_push cp
  xth_status_bar_status cp "Saving $pth ..."

  if {[catch {set fid [open $pth w]}]} {
    MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message $errorInfo \
        -font $xth(gui,lfont)    
    xth_status_bar_pop cp
    return 0
  }

  fconfigure $fid -encoding utf-8 -translation {auto lf}
  puts $fid "encoding  utf-8"

  # let's put data
  set data [$xth(cp,editor).txt get 1.0 end]
  regsub {\s*$} $data {} "data\n"
  puts -nonewline $fid $data
    
  # now let's put special commands
  foreach cmd $xth(cp,special) {
    puts $fid "##XTHERION## $cmd"
  }
  close $fid
  
  xth_status_bar_pop cp
  return 1
}

proc xth_cp_save_as {} {

  global xth

  if {!$xth(cp,fopen)} {
    return 0
  }

  set fname $xth(cp,ffull)
  set idir [file dirname $fname]
  if {[string length $idir] == 0} {
    set idir $xth(gui,initdir)
  }
  set fname [tk_getSaveFile -filetypes $xth(app,cp,filetypes) \
    -parent $xth(gui,main) \
    -initialfile [file tail $fname] \
    -initialdir [file dirname $fname]]

  if {[string length $fname] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fname]
  }

  if {![xth_cp_write_file $fname]} {
    return 0
  }

  set xth(cp,fname) [file tail $fname]
  set xth(cp,open_file) [file tail $fname]
  set xth(cp,fpath) [file dirname $fname]
  set xth(cp,ffull) $fname
  xth_app_title cp
  update idletasks  
  return 1
}


# xth_cp_read_file --
#
# return success
# {success name cmds lns}

proc xth_cp_read_file {pth} {

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
    } else {
      if {[regexp {(.*)\\\s*$} $lastln dumln prevln]} {
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


proc xth_cp_compile {} {
  global xth errorInfo
  set xth(cp,compres) 0
  if {!$xth(cp,fopen)} {
    return 0;
  }
  set ret 1
  xth_cp_write_file $xth(cp,ffull)
  set cdir [pwd]
  cd $xth(cp,fpath)
  if {$xth(cp,updcf)} {
    set uu "-u"
  } else {
    set uu ""
  }
  $xth(cp,log).txt configure -state normal
  $xth(cp,log).txt delete 1.0 end
  $xth(cp,log).txt configure -wrap word
  $xth(cp,editor).txt configure -state disabled
  xth_status_bar_push cp
  xth_status_bar_status cp "Running therion ..."
  set err [catch {
    set thid [open "|$xth(gui,compcmd) $uu $xth(cp,fname)" r]
    if $xth(gui,compshow) {
      while {![eof $thid]} {
        $xth(cp,log).txt insert end [read $thid 8]
        $xth(cp,log).txt see end
        update idletasks
      }
    } else {
      read $thid;
    }
    close $thid
  }]
  if {$err} {
    bell
    $xth(ctrl,cp,stp).gores configure -text "ERROR" -fg white -bg red
    set ret 0
  } else {
    set xth(cp,compres) 1
    $xth(ctrl,cp,stp).gores configure -text "OK" -fg black -bg green
  }
  
  xth_status_bar_status cp "Reading therion log file ..."
  if {[catch {
    set lid [open "therion.log" r]
    $xth(cp,log).txt delete 1.0 end
    $xth(cp,log).txt configure -wrap none
    $xth(cp,log).txt insert end "[read $lid]\n"
    close $lid
    }]} {
      $xth(cp,log).txt insert end "\nerror opening therion.log file\n"
  }
  $xth(cp,log).txt see end
  xth_status_bar_pop cp
  update idletasks

  $xth(cp,log).txt configure -state disabled
  $xth(cp,editor).txt configure -state normal

  # update configuration file if required
  set xth(cp,cursor) [$xth(cp,editor).txt index insert]
  if {$xth(cp,updcf)} {
    set fdata [xth_me_read_file $xth(cp,ffull)]
    if {[lindex $fdata 0] == 0} {
        MessageDlg $xth(gui,message) -parent $xth(gui,main) \
          -icon error -type ok \
          -message [lindex $fdata 1] \
          -font $xth(gui,lfont)
    } else {
      xth_cp_data_tree_clear      
      foreach cmd [lindex $fdata 2] {
        catch {eval $cmd}
      }
      set xth(cp,special) [lindex $fdata 2]
      
      $xth(cp,editor).txt delete 1.0 end
      foreach ln [lindex $fdata 3] {
        $xth(cp,editor).txt insert end "$ln\n"
      }
      
      $xth(cp,editor).txt mark set insert $xth(cp,cursor)
      $xth(cp,editor).txt see $xth(cp,cursor)
      
    }
  }
  set xth(cp,updcf) 0
  cd $cdir
  return $ret
}


proc xth_cp_data_tree_clear {} {
  global xth
  set tp $xth(ctrl,cp,dat).sw.t 
  $tp delete [$tp nodes root]
}

proc xth_cp_data_tree_insert {id parent type name fullname title} {
  global xth
  set tp $xth(ctrl,cp,dat).sw.t 
  set copen 0
  switch $type {
    survey {
      set nimg [Bitmap::get folder]
    }
    map {
      set nimg [Bitmap::get copy]
    }
    scrap {
      set nimg [Bitmap::get file]
    }
    default {
      set nimg [Bitmap::get new]
    }
  }
  if {[string length $parent] < 1} {
    set parent root
    set copen 1
  }
  if {[string length $title] < 1} {
    set title $name
  }
  $tp insert end $parent $id -data [list $type $fullname $title] -text $title -image $nimg -open $copen -font $xth(gui,lfont)
}

proc xth_cp_data_tree_enter {node} {
  global xth
  set tp $xth(ctrl,cp,dat).sw.t 
  xth_status_bar_push cp
  set d [$xth(ctrl,cp,dat).sw.t itemcget $node -data]
  xth_status_bar_status cp [format "%s - %s (%s)" [lindex $d 0] [lindex $d 1] [lindex $d 2]]
}

proc xth_cp_data_tree_leave {node} {
  xth_status_bar_pop cp
}

proc xth_cp_data_tree_double_click {node} {
  global xth
  set tp $xth(ctrl,cp,dat).sw.t 
  set d [$tp itemcget $node -data]
  set i [$xth(cp,editor).txt index insert]  
  regexp {(\d+)\.} $i dum cln
  $xth(cp,editor).txt insert $cln.0 [format "select %s\n" [lindex $d 1]]
}

