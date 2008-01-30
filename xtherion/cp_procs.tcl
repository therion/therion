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


xth_about_status [mc "loading compiler..."]


proc xth_xcfg_fname {fpath} {
  return ".xtherion.dat"
}


proc xth_cp_new_file {} {
  global xth
  xth_status_bar_status cp ""
  if {$xth(cp,fopen)} {
    return
  }
  set xth(cp,fopen) 1
  set xth(cp,special) {}
  set xth(cp,ffull) [file join $xth(gui,initdir) "thconfig"]
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
  xth_status_bar_status cp ""
  
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

  set is_config_file [xth_cp_is_config_file $fpath];
  if {[string length $is_config_file] > 0} {
    MessageDlg $xth(gui,message) -parent $xth(gui,main) \
      -icon info -type ok \
      -message  "$fpath\n-----\n$is_config_file" \
      -font $xth(gui,lfont)
    return 0;
  }
  

  # read the file
  xth_status_bar_push cp
  xth_status_bar_status cp [format [mc "Opening %s ..."] $fpath]

  set fdata [xth_me_read_file $fpath 0]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
	-icon error -type ok \
	-message [lindex $fdata 1] \
	-font $xth(gui,lfont)
      xth_status_bar_pop cp
      return 0
  }
  
  # now let's show the file
  catch {
    set fid [open [file join [file dirname $fpath] [xth_xcfg_fname $fpath]] r]
    fconfigure $fid -encoding utf-8
    while {![eof $fid]} {
      catch {
	eval [gets $fid]
      }
    }
    close $fid
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
  catch {
    $xth(cp,editor).txt edit reset
  }
  $xth(cp,editor).txt mark set insert $xth(cp,cursor)
  $xth(cp,editor).txt see $xth(cp,cursor)
  
  $xth(ctrl,cp,stp).wl configure -state normal
  $xth(ctrl,cp,stp).we configure -state normal
  $xth(ctrl,cp,stp).fl configure -state normal
  $xth(ctrl,cp,stp).fe configure -state normal
  $xth(ctrl,cp,stp).optl configure -state normal
  $xth(ctrl,cp,stp).opte configure -state normal
  $xth(ctrl,cp,stp).go configure -state normal
  $xth(ctrl,cp,stp).gores configure -state normal

  $xth(ctrl,cp,info).txt configure -state normal
  
  $xth(cp,menu,file) entryconfigure [mc "New"] -state disabled
  $xth(cp,menu,file) entryconfigure [mc "Open"] -state disabled
  $xth(cp,menu,file) entryconfigure [mc "Save as"] -state normal
  $xth(cp,menu,file) entryconfigure [mc "Close"] -state normal
  $xth(cp,menu) entryconfigure [mc "Edit"] -state normal
  
  xth_app_title cp
  xth_status_bar_pop cp
  update idletasks
  return 1
}


set xth(cps,n) 0
set xth(cps,mxs) 0
set xth(cps,mxl) 0
set xth(cps,ts) 0
set xth(cps,tl) 0

proc xth_cp_comp_stat {tlen tscrap} {
  global xth
  incr xth(cps,n)
  if {$tscrap > $xth(cps,mxs)} {
    set xth(cps,mxs) $tscrap
  }
  if {$tlen > $xth(cps,mxl)} {
    set xth(cps,mxl) $tlen
  }
  set xth(cps,ts) [expr $xth(cps,ts) + $tscrap]
  set xth(cps,tl) [expr $xth(cps,tl) + $tscrap]
  set xth(cps,as) [expr double($xth(cps,ts)) / double($xth(cps,n))]
  set xth(cps,al) [expr double($xth(cps,tl)) / double($xth(cps,n))]  
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

  $xth(cp,menu,file) entryconfigure [mc "New"] -state normal
  $xth(cp,menu,file) entryconfigure [mc "Open"] -state normal
  $xth(cp,menu,file) entryconfigure [mc "Save as"] -state disabled
  $xth(cp,menu,file) entryconfigure [mc "Close"] -state disabled
  $xth(cp,menu) entryconfigure [mc "Edit"] -state disabled
  
  $xth(ctrl,cp,stp).wl configure -state disabled
  $xth(ctrl,cp,stp).we configure -state disabled
  $xth(ctrl,cp,stp).fl configure -state disabled
  $xth(ctrl,cp,stp).fe configure -state disabled
  $xth(ctrl,cp,stp).optl configure -state disabled
  $xth(ctrl,cp,stp).opte configure -state disabled
  $xth(ctrl,cp,stp).go configure -state disabled
  $xth(ctrl,cp,stp).gores configure -text "" -fg $xth(cp,resfg) -bg $xth(cp,resbg) \
    -state disabled

  $xth(ctrl,cp,info).txt configure -state disabled
  
  # set variables
  set xth(cp,fopen) 0
  set xth(cp,cursor) 1.0
  set xth(cp,fname) ""
  set xth(cp,open_file) ""
  set xth(cp,fpath) ""
  set xth(cp,opts) ""
  xth_app_title cp
  focus $xth(ctrl,cp,dat).t 
  
  set xth(ctrl,cp,datrestore) {}
  set xth(ctrl,cp,msrestore) {}

}


# xth_cp_write_file --
#
# return list containing
# {success name}

proc xth_cp_write_file {pth} {

  global errorInfo xth

  xth_status_bar_push cp
  xth_status_bar_status cp [format [mc "Saving %s ..."] $pth]

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
  regsub {\s*$} $data {} data
  puts $fid $data
    
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
    -initialdir [xth_getdir $fname]]

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


proc xth_cp_is_config_file {fname} {
  set str {}
  return $str
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
  $xth(cp,log).txt configure -state normal
  $xth(cp,log).txt delete 1.0 end
  $xth(cp,log).txt configure -wrap word
  $xth(cp,editor).txt configure -state disabled
  xth_status_bar_push cp
  
  catch {
    set lid [open "therion.log" w]
    puts $lid "ERROR: Can not execute \"$xth(gui,compcmd) -x $xth(cp,opts) $xth(cp,fname)\"."
    close $lid
  }  

  set acroreadopen 1
  if {[llength $xth(cp,preview,acroreadpath)] > 0 } {
    catch {
      set acroreadopen [catch {
	exec $xth(gui,appctrlcmd) ARExists [file join $xth(cp,fpath) $xth(cp,preview,acroreadpath)]
      }]
    }
    if {!$acroreadopen} {
      catch {
	exec $xth(gui,appctrlcmd) AROpen [file join $xth(cp,fpath) $xth(cp,preview,acroreadpath)]
	exec $xth(gui,appctrlcmd) ARClose [file join $xth(cp,fpath) $xth(cp,preview,acroreadpath)]
      }
    }
  }

  
  xth_status_bar_status cp [mc "Running therion ..."]
  $xth(ctrl,cp,stp).gores configure -text [mc "RUNNING"] -fg black -bg yellow
  update idletasks
  set err [catch {
    set thid [open "|$xth(gui,compcmd) -x $xth(cp,opts) $xth(cp,fname)" r]
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
  
  set see_end 0
  #puts $err
  #global errorInfo
  #puts $errorInfo
  
  if {$err} {
    bell
    $xth(ctrl,cp,stp).gores configure -text [mc "ERROR"] -fg white -bg red
    set ret 0
    set see_end 1
  } else {
    set xth(cp,compres) 1
    if {$xth(gui,auto_backup)} {
      xth_cp_write_file "$xth(cp,ffull)$xth(gui,auto_backup_ext)"
    }

    if { [llength $xth(cp,preview,xpdfpath)] > 0 } {
      catch {
	exec xpdf -remote thpdf [file join $xth(cp,fpath) $xth(cp,preview,xpdfpath)] &
      }
    }
    if { [llength $xth(cp,preview,acroreadpath)] > 0 } {
      catch {
	if {$acroreadopen} {
	  exec $xth(gui,appctrlcmd) AROpen [file join $xth(cp,fpath) $xth(cp,preview,acroreadpath)]
	} else {
	  exec $xth(gui,appctrlcmd) ARBack [file join $xth(cp,fpath) $xth(cp,preview,acroreadpath)]
	}
      }
    }
    if {$err} {
      bell
      $xth(ctrl,cp,stp).gores configure -text [mc "WARNING"] -fg black -bg orange
    } else {
      $xth(ctrl,cp,stp).gores configure -text [mc "OK"] -fg black -bg green
    }
  }
  
  xth_status_bar_status cp [mc "Reading therion log file ..."]
  if {[catch {
    set lid [open "therion.log" r]
    $xth(cp,log).txt delete 1.0 end
    $xth(cp,log).txt configure -wrap none
    $xth(cp,log).txt insert end "[read $lid]\n"
    close $lid
    }]} {
      $xth(cp,log).txt insert end [mc "\nerror opening therion.log file\n"]
  }
  if ($see_end) {
    $xth(cp,log).txt see end
  } else {
    $xth(cp,log).txt see 1.0
  }
  xth_status_bar_pop cp
  update idletasks

  $xth(cp,log).txt configure -state normal
  $xth(cp,editor).txt configure -state normal
  xth_cp_show_errors
  
  # update the tree
  xth_cp_data_tree_clear      
  catch {
    set fid [open [file join [file dirname $xth(cp,ffull)] [xth_xcfg_fname $xth(cp,ffull)]] r]
    fconfigure $fid -encoding utf-8
    while {![eof $fid]} {
      catch {eval [gets $fid]}
    }
    close $fid
  }

  # update configuration file if required
  set xth(cp,cursor) [$xth(cp,editor).txt index insert]
  if {0} {
    set fdata [xth_me_read_file $xth(cp,ffull) 0]
    if {[lindex $fdata 0] == 0} {
	MessageDlg $xth(gui,message) -parent $xth(gui,main) \
	  -icon error -type ok \
	  -message [lindex $fdata 1] \
	  -font $xth(gui,lfont)
    } else {
      ###xth_cp_data_tree_clear      


      set xth(cp,special) [lindex $fdata 2]
     
      $xth(cp,editor).txt configure -undo 0 
      
      $xth(cp,editor).txt delete 1.0 end
      foreach ln [lindex $fdata 3] {
	$xth(cp,editor).txt insert end "$ln\n"
      }
      
      $xth(cp,editor).txt mark set insert $xth(cp,cursor)
      $xth(cp,editor).txt see $xth(cp,cursor)
      
      $xth(cp,editor).txt configure -undo 1
      
    }
  }
  cd $cdir
  xth_me_xvi_refresh
  xth_me_save_file 0
  return $ret
}


proc xth_cp_data_tree_clear {} {
  global xth
  set tp $xth(ctrl,cp,dat).t 
  set xth(ctrl,cp,datrestore) {}
  foreach di $xth(ctrl,cp,datlist) {
    catch {
      append xth(ctrl,cp,datrestore) "catch \{$tp itemconfigure [lindex $di 1] -open [$tp itemcget [lindex $di 1] -open]\}\n";
    }
  }
  append xth(ctrl,cp,datrestore) "update idletasks\n"
  append xth(ctrl,cp,datrestore) "catch \{$tp xview moveto [lindex [$tp xview] 0]\}\n";
  append xth(ctrl,cp,datrestore) "catch \{$tp yview moveto [lindex [$tp yview] 0]\}\n";
  append xth(ctrl,cp,datrestore) "catch \{$tp selection set [$tp selection get]\}\n";
  append xth(ctrl,cp,datrestore) "update idletasks\n"
  catch {
    $tp delete [$tp nodes root]
  }
  set tp $xth(ctrl,cp,ms).t 
  set xth(ctrl,cp,msrestore) {}
  foreach di $xth(ctrl,cp,maplist) {
    catch {
      append xth(ctrl,cp,msrestore) "catch \{$tp itemconfigure [lindex $di 1] -open [$tp itemcget [lindex $di 1] -open]\}\n";
    }
  }
  append xth(ctrl,cp,msrestore) "update idletasks\n"
  append xth(ctrl,cp,msrestore) "catch \{$tp xview moveto [lindex [$tp xview] 0]\}\n";
  append xth(ctrl,cp,msrestore) "catch \{$tp yview moveto [lindex [$tp yview] 0]\}\n";
  append xth(ctrl,cp,msrestore) "catch \{$tp selection set [$tp selection get]\}\n";
  append xth(ctrl,cp,msrestore) "update idletasks\n"
  catch {
    $tp delete [$tp nodes root]
  }
  $xth(ctrl,cp,info).txt delete 1.0 end
  # prejde oba stromy a priradi rozvinutie/zvinutie do prikazov
  # plus ulozi poziciu
}

proc xth_cp_data_tree_insert {id parent level name fullname title stitle stat} {
  global xth
  if {[string length $title] < 1} {
    set title $name
    set stitle $name
  }
  lappend xth(ctrl,cp,datlist) [list $stitle $id $parent $level $name $fullname $title $stat]
}

proc xth_cp_data_tree_create {} {
  global xth
  if {[llength $xth(ctrl,cp,datlist)] == 0} {
    return
  }
  set nlist [lsort -dictionary -index 0 $xth(ctrl,cp,datlist)]
  set level 1
  set tocnt 1
  set copen 1
  set tp $xth(ctrl,cp,dat).t 
  while {$tocnt} {
    set tocnt 0
    foreach di $nlist {
      if {[lindex $di 3] == $level} {
	if {$level == 1} {
	  set parent root
	} else {
	  set parent [lindex $di 2]
	}
	catch {
	  $tp insert end $parent [lindex $di 1] -data [list survey [lindex $di 5] [lindex $di 6] [lindex $di 7]] \
	    -text [lindex $di 6] -image [Bitmap::get folder] -open $copen -font $xth(gui,lfont)
	}
      }
      if {[lindex $di 3] > $level} {
	set tocnt 1
      }
    }
    set copen 0
    incr level
  }
  catch {
    eval $xth(ctrl,cp,datrestore)
  }
}

proc xth_cp_map_tree_insert {type subtype id parent level name fullname title stitle} {
  global xth
  if {[string length $title] < 1} {
    set title $name
    set stitle $name
  }
  lappend xth(ctrl,cp,maplist) [list $stitle $id $parent $level $name $fullname $title $type $subtype]
}

proc xth_cp_map_tree_create {} {
  global xth
  if {[llength $xth(ctrl,cp,maplist)] == 0} {
    return
  }
  set nlist [lsort -dictionary -index 0 $xth(ctrl,cp,maplist)]
  set level 0
  set tocnt 1
  set copen 1
  set tp $xth(ctrl,cp,ms).t 
  while {$tocnt} {
    set tocnt 0
    foreach di $nlist {
      if {[lindex $di 3] == $level} {
	if {$level == 0} {
	  set parent root
	} else {
	  set parent [lindex $di 2]
	}
	set ccopen $copen
	switch [lindex $di 7] {
	  map {
	    if {[lindex $di 8]} {
	      set ii [Bitmap::get file]
	      set ccopen 0
	    } else {
	      set ii [Bitmap::get copy]
	    }
	  }
	  scrap {
	    set ii [Bitmap::get new]
	  }
	  default {
	    set ii [Bitmap::get folder]
	  } 
	}
	catch {
	  $tp insert end $parent [lindex $di 1] -data [list [lindex $di 7] [lindex $di 5] [lindex $di 6]] \
	    -text [lindex $di 6] -image $ii -open $ccopen -font $xth(gui,lfont)
	}
      }
      if {[lindex $di 3] > $level} {
	set tocnt 1
      }
    }
    set copen 0
    incr level
  }
  
  # odstrani projekcie bez map
  set prjs [$tp nodes root]
  foreach prj $prjs {
    set nds [$tp nodes $prj]
    switch [llength $nds] {
      0 {
	$tp delete $prj
      }
      1 {
	$tp itemconfigure $nds -open 1
      }
    }
  }

  catch {
    eval $xth(ctrl,cp,msrestore)
  }
}



proc xth_cp_data_tree_enter {node} {
  global xth
  set tp $xth(ctrl,cp,dat).t 
  xth_status_bar_push cp
  set d [$xth(ctrl,cp,dat).t itemcget $node -data]
  xth_status_bar_status cp [format "%s - %s (%s)" [lindex $d 0] [lindex $d 1] [lindex $d 2]]
}


proc xth_cp_data_tree_select {tpath node} {
  global xth
  set tp $tpath
  xth_status_bar_push cp
  set d [$tp itemcget $node -data]
  $xth(ctrl,cp,info).txt delete 1.0 end
  $xth(ctrl,cp,info).txt see 1.0
  $xth(ctrl,cp,info).txt insert 1.0 "name: [lindex $d 1]\ntitle: [lindex $d 2]\n[lindex $d 3]"
}



proc xth_cp_data_tree_leave {node} {
  xth_status_bar_pop cp
}

proc xth_cp_data_tree_double_click {node} {
  global xth
  set tp $xth(ctrl,cp,dat).t 
  set d [$tp itemcget $node -data]
  set i [$xth(cp,editor).txt index insert]  
  regexp {(\d+)\.} $i dum cln
  $xth(cp,editor).txt insert $cln.0 [format "select %s\n" [lindex $d 1]]
}

proc xth_cp_map_tree_enter {node} {
  global xth
  set tp $xth(ctrl,cp,ms).t 
  xth_status_bar_push cp
  set d [$xth(ctrl,cp,ms).t itemcget $node -data]
  xth_status_bar_status cp [format "%s - %s (%s)" [lindex $d 0] [lindex $d 1] [lindex $d 2]]
}

proc xth_cp_map_tree_leave {node} {
  xth_status_bar_pop cp
}

proc xth_cp_map_tree_double_click {node} {
  global xth
  set tp $xth(ctrl,cp,ms).t 
  set d [$tp itemcget $node -data]
  set i [$xth(cp,editor).txt index insert]  
  regexp {(\d+)\.} $i dum cln
  if {[string length [lindex $d 1]] > 0} {
    $xth(cp,editor).txt insert $cln.0 [format "select %s\n" [lindex $d 1]]
  }
}


proc xth_cp_show_errors {} {
  
  global xth
  
  set w $xth(cp,log).txt
  $w tag remove xtherr 1.0 end

  set rx {\S*[^\]\s]\s+\[\d+\]}
  set fnd [$w search -regexp -count cnt $rx 1.0 end]
  
  set i 0
  while {([string length $fnd] > 0) && ($i < 10000)} {
    
    set enx [$w index "$fnd + $cnt chars"]
    set ctext [$w get $fnd "$fnd lineend"]
    set cfnm {}
    regexp {\S+} $ctext cfnm
  
    if {![regexp {(\.mp|\.tex)\)?$} $cfnm]} {
      $w tag add xtherr $fnd $enx
    }

    set fnd [$w search -regexp -count cnt $rx $enx end]
    incr i    
    
  }
  
  $w tag configure xtherr -foreground red
  set prevcur [$xth(cp,log).txt cget -cursor]
  $w tag bind xtherr <Enter> "$w configure -cursor hand2"
  $w tag bind xtherr <Leave> "$w configure -cursor $prevcur"
  $w tag bind xtherr <1> "xth_cp_goto_error %x %y"
 
}



proc xth_cp_goto_error {x y} {
  global xth

  set epos [$xth(cp,log).txt get "@$x,$y wordstart" "@$x,$y lineend"]
  
  # skusime najst source error
  set fnm {}
  set fln {}
  if {(![regexp {\s*(\S+)\s+\[(\d+)\]} $epos dum fnm fln]) || [string equal $fnm "th"] || [string equal $fnm "th2"] || [string equal $fnm ".th"] || [string equal $fnm ".th2"]} {
    set epos [$xth(cp,log).txt get "@$x,$y linestart" "@$x,$y lineend"]
    regexp {\s*(\S+)\s+\[(\d+)\]} $epos dum fnm fln
  }
  
  if {([string length $fnm] == 0) || ([string length $fln] == 0)} {
    return
  }  
  
  if {[catch {set fln [expr $fln - 1]}]} {
    return
  }
  
  # potom sa pozrieme ci subor nemame otvoreny v kompilatore,
  # mapovom alebo textovom editore a skocime tam
  
  if $xth(cp,fopen) {
  
    set fullfnm [file join $xth(cp,fpath) $fnm]

    if {[string equal $fullfnm $xth(cp,ffull)]} {
      after idle "focus $xth(cp,editor).txt; $xth(cp,editor).txt see $fln.0; $xth(cp,editor).txt mark set insert $fln.0; $xth(cp,editor).txt tag remove sel 1.0 end; $xth(cp,editor).txt tag add sel $fln.0 \"$fln.0 lineend\""
      return
    }
    
    if {$xth(me,fopen) && [string equal $fullfnm $xth(me,ffull)]} {
      after idle "xth_app_show me; xth_me_goto_line [expr $fln + 1]"
      return
    }
  
    # skusime textovy editor, ci to mame otvorene
    foreach fx $xth(te,flist) {
      if {[string equal $fullfnm $xth(te,$fx,path)]} {
	after idle "xth_app_show te; xth_te_show_file $fx; $xth(te,$fx,frame).txt see $fln.0; $xth(te,$fx,frame).txt mark set insert $fln.0; $xth(te,$fx,frame).txt tag remove sel 1.0 end; $xth(te,$fx,frame).txt tag add sel $fln.0 \"$fln.0 lineend\""
	return
      }
    }
    
    after idle "xth_app_show te; xth_te_open_file 0 [list $fullfnm] $fln"
  
  }
  
  
}



