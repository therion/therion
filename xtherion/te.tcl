##
## te.tcl --
##
##     Text editor.   
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


xth_about_status "loading text editor ..."

if {[string equal -nocase $xth(prj,name) svxedit]} {
  xth_app_create te {}
} else {
  xth_app_create te "Text Editor"
}

xth_ctrl_add te files "Files"
xth_ctrl_add te sdata "Data table"
xth_ctrl_add te sr "Search & Replace"
xth_ctrl_finish te

set xth(te,open_file_encoding) $xth(app,fencoding)

set xth(te,bind,text_tab) {
  if { [string equal [%W cget -state] "normal"] } {
    xth_te_insert_tab %W
    break
  }
}

set xth(te,bind,text_return) {
  regexp {(\d+)\.} [%W index insert] dum cln
  set spcs ""
  regexp {^\s+} [%W get $cln.0 $cln.end] spcs
  set spcsc [string length $spcs]
  set indct [string length [xth_te_get_indent %W $cln.0 1]]
  if {$spcsc == $indct} {
  } elseif {$spcsc > $indct} {
    %W delete $cln.0 $cln.[expr $spcsc - $indct]
  } elseif {$spcsc < $indct} {
    %W insert $cln.0 [format \x25[expr $indct - $spcsc]s " "]
  }
  xth_te_insert_text %W "\n[xth_te_get_indent %W [expr $cln + 1].0 0]"
}


proc xth_te_insert_text {w s} {
    if {[string equal $s ""] || [string equal [$w cget -state] "disabled"]} {
	return
    }
    set compound 0
    catch {
	if {[$w compare sel.first <= insert] \
		&& [$w compare sel.last >= insert]} {
            set oldSeparator [$w cget -autoseparators]
            if { $oldSeparator } {
                $w configure -autoseparators 0
                $w edit separator
                set compound 1
            }
	    $w delete sel.first sel.last
	}
    }
    $w insert insert $s
    $w see insert
    if { $compound && $oldSeparator } {
        $w edit separator
        $w configure -autoseparators 1
    }
}


proc xth_te_insert_tab W {
  global xth
  regexp {\.(\d+)} [$W index insert] dum col
  set nsp [expr $xth(gui,etabsize) - ($col % $xth(gui,etabsize))]
  xth_te_insert_text $W  [format \x25$nsp\s " "]
  focus $W
}


proc xth_te_sdata_enable {w} {
  global xth
  if {[string length $w] < 1} {
    set w $xth(ctrl,te,sdata)
  }
  set chlist [winfo children $w]
  if {[llength $chlist] > 0} {
    foreach sdw $chlist {
      catch {$sdw configure -state normal}
      catch {xth_te_sdata_enable $sdw}
    }
  }
}

proc xth_te_sdata_disable {w} {
  global xth
  if {[string length $w] < 1} {
    set w $xth(ctrl,te,sdata)
  }
  set chlist [winfo children $w]
  if {[llength $chlist] > 0} {
    foreach sdw $chlist {
      catch {$sdw configure -state disabled}
      catch {xth_te_sdata_disable $sdw}
    }
  }
}


set xth(te,flist) {}
set xth(te,fcurr) -1
set xth(te,fltid) 0

# create position bar
set pbar $xth(gui,te).sf.pbar
label $pbar -text "2.0" -width 8 -relief sunken -font $xth(gui,lfont)
pack $pbar -side left


# file control
frame $xth(ctrl,te,files).fl
set flbox $xth(ctrl,te,files).fl.flbox 
listbox $flbox -height 6 -selectmode single -takefocus 1 \
  -yscrollcommand "xth_scroll $xth(ctrl,te,files).fl.sv" \
  -xscrollcommand "xth_scroll $xth(ctrl,te,files).fl.sh" \
  -font $xth(gui,lfont) -exportselection no \
  -selectborderwidth 1

scrollbar $xth(ctrl,te,files).fl.sv -orient vert  -command "$flbox yview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
scrollbar $xth(ctrl,te,files).fl.sh -orient horiz  -command "$flbox xview" \
  -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
frame $xth(ctrl,te,files).ef

  
bind $flbox <<ListboxSelect>> "xth_te_show_file \[lindex \[%W curselection\] 0\]"

grid columnconf $xth(ctrl,te,files).fl 0 -weight 1
grid rowconf $xth(ctrl,te,files).fl 0 -weight 1
grid $flbox -column 0 -row 0 -sticky news
xth_scroll_showcmd $xth(ctrl,te,files).fl.sv "grid $xth(ctrl,te,files).fl.sv -column 1 -row 0 -sticky news"
xth_scroll_hidecmd $xth(ctrl,te,files).fl.sv "grid forget $xth(ctrl,te,files).fl.sv"
xth_scroll_showcmd $xth(ctrl,te,files).fl.sh "grid $xth(ctrl,te,files).fl.sh -column 0 -row 1 -sticky news"
xth_scroll_hidecmd $xth(ctrl,te,files).fl.sh "grid forget $xth(ctrl,te,files).fl.sh"
xth_status_bar te $flbox "Switch open files."
grid columnconf $xth(ctrl,te,files) 0 -weight 1
grid $xth(ctrl,te,files).fl -column 0 -row 0 -sticky news
if {![string equal -nocase $xth(prj,name) svxedit]} {
grid $xth(ctrl,te,files).ef -column 0 -row 1 -sticky news
}
Label $xth(ctrl,te,files).ef.ecl -text Encoding -anchor e -font $xth(gui,lfont) -state disabled
ComboBox $xth(ctrl,te,files).ef.ecb -values $xth(encodings) \
  -textvariable xth(te,open_file_encoding) \
  -font $xth(gui,lfont) -height 4 -command xth_te_set_encoding \
  -state disabled
Button $xth(ctrl,te,files).ef.chb -text "Change to" -anchor e -font $xth(gui,lfont) -padx 1 -state disabled -command xth_te_set_encoding
Label $xth(ctrl,te,files).ef.cel -text "" -anchor w -padx 2 -font $xth(gui,lfont) -state disabled
#grid columnconf $xth(ctrl,te,files).ef 0 -weight 0
grid columnconf $xth(ctrl,te,files).ef 1 -weight 1
grid $xth(ctrl,te,files).ef.ecl -column 0 -row 0 -sticky news
grid $xth(ctrl,te,files).ef.cel -column 1 -row 0 -sticky news
grid $xth(ctrl,te,files).ef.chb -column 0 -row 1 -sticky news
grid $xth(ctrl,te,files).ef.ecb -column 1 -row 1 -sticky ew
xth_status_bar te $xth(ctrl,te,files).ef "To set file encoding, type encoding name and press <Change> button."

frame $xth(gui,te).af.apps.ff -bg $xth(gui,ecolorbg)
pack $xth(gui,te).af.apps.ff -fill both -expand yes



# table control
Button $xth(ctrl,te,sdata).sfb -text "Scan data format" \
  -font $xth(gui,lfont) -state disabled
xth_status_bar te $xth(ctrl,te,sdata).sfb \
  "Scan data format and rebuild centerline data insertion tool."

checkbutton $xth(ctrl,te,sdata).sfs -text "Enter station names" -anchor w \
  -font $xth(gui,lfont) -variable xth(te,sdata,es) -state disabled
xth_status_bar te $xth(ctrl,te,sdata).sfs \
  "Check if you want to insert station names for each shot."

frame $xth(ctrl,te,sdata).sdf

button $xth(ctrl,te,sdata).taf -text "Auto format selection" \
  -font $xth(gui,lfont) -state disabled
xth_status_bar te $xth(ctrl,te,sdata).taf "Format selection to given table."

grid columnconf $xth(ctrl,te,sdata) 0 -weight 1
grid $xth(ctrl,te,sdata).sfb -column 0 -row 0 -sticky nsew
grid $xth(ctrl,te,sdata).sdf -column 0 -row 1 -sticky nsew
grid $xth(ctrl,te,sdata).sfs -column 0 -row 2 -sticky nsew
grid $xth(ctrl,te,sdata).taf -column 0 -row 3 -sticky nsew




set sfm $xth(ctrl,te,sr)

set xth(ctrl,te,sr,selection_io) 0
set xth(ctrl,te,sr,search) ""
set xth(ctrl,te,sr,replace_io) 0
set xth(ctrl,te,sr,replace) ""
set xth(ctrl,te,sr,case_io) 0
set xth(ctrl,te,sr,regular_io) 0
set xth(ctrl,te,sr,selection_io) 0

set xth(ctrl,te,sr,selection_start) {}
set xth(ctrl,te,sr,selection_end) {}
set xth(ctrl,te,sr,search_end) end

Label $sfm.seal -text "search" -anchor w -font $xth(gui,lfont) -state disabled \
  -width 4
xth_status_bar te $sfm.seal "Search expression."
Entry $sfm.seae -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,te,sr,search)
xth_status_bar te $sfm.seae "Search expression."

checkbutton $sfm.replc -text replace -anchor w -font $xth(gui,lfont) \
  -state disabled -width 4 \
  -variable xth(ctrl,te,sr,replace_io) \
  -command {}
xth_status_bar te $sfm.replc "Check whether to replace found expression."
Entry $sfm.reple -font $xth(gui,lfont) -state disabled -width 4 \
  -textvariable xth(ctrl,te,sr,replace)
xth_status_bar te $sfm.reple "Replace expression."

checkbutton $sfm.ccase -text "case sensitive search" -anchor w -font $xth(gui,lfont) \
  -state disabled -width 0 \
  -variable xth(ctrl,te,sr,case_io) \
  -command {}
xth_status_bar te $sfm.ccase "Check if search should be case sensitive."

checkbutton $sfm.creg -text "regular expressions" -anchor w -font $xth(gui,lfont) \
  -state disabled -width 0 \
  -variable xth(ctrl,te,sr,regular_io) \
  -command {}
xth_status_bar te $sfm.creg "Check whether to evaluate search and replace as regular expressions."

checkbutton $sfm.csel -text "search selection only" -anchor w -font $xth(gui,lfont) \
  -state disabled -width 0 \
  -variable xth(ctrl,te,sr,selection_io) \
  -command {}
xth_status_bar te $sfm.csel "Check whether to do search only in selected text."

Button $sfm.bfirst -text "First" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 4 -command te_sr_first
xth_status_bar te $sfm.bfirst "Search or replace first expression in the file."
Button $sfm.bnext -text "Next" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 4 -command te_sr_next
xth_status_bar te $sfm.bnext "Search or replace next expression after the cursor in the file."
Button $sfm.ball -text "All" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 4 -command te_sr_all
xth_status_bar te $sfm.ball "Search or replace all expressions in the file."
Button $sfm.bclear -text "Clear" -anchor center -font $xth(gui,lfont) \
  -state disabled -width 4 -command {
    te_sr_clear
    set xth(ctrl,te,sr,replace_io) 0
    update idletasks
  }
xth_status_bar te $sfm.bclear "Clear all highlights in the file."


grid columnconf $sfm 0 -weight 1
grid columnconf $sfm 1 -weight 1
grid $sfm.seal -row 0 -column 0 -sticky news
grid $sfm.seae -row 1 -column 0 -sticky news
grid $sfm.replc -row 0 -column 1 -sticky news
grid $sfm.reple -row 1 -column 1 -sticky news
grid $sfm.ccase -row 2 -column 0 -columnspan 2 -sticky news
grid $sfm.creg -row 3 -column 0 -columnspan 2 -sticky news
grid $sfm.csel -row 4 -column 0 -columnspan 2 -sticky news
grid $sfm.bfirst -row 5 -column 0 -sticky news
grid $sfm.bnext -row 5 -column 1 -sticky news
grid $sfm.ball -row 6 -column 0 -sticky news
grid $sfm.bclear -row 6 -column 1 -sticky news





proc xth_te_show_file {fidx} {

  global xth
  
  if {$xth(te,fcurr) >= 0} {
    pack forget $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame)
  }
  
  if {$fidx < 0} {
    set fidx 0
  }
  if {$fidx >= [llength $xth(te,flist)]} {
    set fidx [expr [llength $xth(te,flist)] - 1]
  }
  
  set xth(te,fcurr) $fidx
  if {$xth(te,fcurr) >= 0} {
    set cfid [lindex $xth(te,flist) $xth(te,fcurr)]
    pack $xth(te,$cfid,frame) -expand yes -fill both
    $xth(ctrl,te,files).fl.flbox delete $xth(te,fcurr)
    $xth(ctrl,te,files).fl.flbox insert $xth(te,fcurr) "[expr $xth(te,fcurr) + 1]. $xth(te,$cfid,name) ($xth(te,$cfid,path))"
    $xth(ctrl,te,files).fl.flbox see $fidx
    $xth(ctrl,te,files).fl.flbox selection clear 0 end
    $xth(ctrl,te,files).fl.flbox selection set $fidx $fidx
    focus $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
    set xth(te,open_file) $xth(te,$cfid,name)
    # set xth(te,open_file_encoding) $xth(te,$cfid,encoding)
    $xth(ctrl,te,files).ef.cel configure -text $xth(te,$cfid,encoding)
    $xth(ctrl,te,files).ef.ecl configure -state normal
    $xth(ctrl,te,files).ef.ecb configure -state normal
    $xth(ctrl,te,files).ef.chb configure -state normal
    $xth(ctrl,te,files).ef.cel configure -state normal
    $xth(te,menu) entryconfigure Edit -state normal
    $xth(te,menu,file) entryconfigure "Save" -state normal
    $xth(te,menu,file) entryconfigure "Save as" -state normal
    $xth(te,menu,file) entryconfigure "Save all" -state normal
    $xth(te,menu,file) entryconfigure "Close" -state normal
    $xth(ctrl,te,sr).seal configure -state normal
    $xth(ctrl,te,sr).seae configure -state normal
    $xth(ctrl,te,sr).replc configure -state normal
    $xth(ctrl,te,sr).reple configure -state normal
    $xth(ctrl,te,sr).ccase configure -state normal
    $xth(ctrl,te,sr).creg configure -state normal
    $xth(ctrl,te,sr).csel configure -state normal
    $xth(ctrl,te,sr).bfirst configure -state normal
    $xth(ctrl,te,sr).bnext configure -state normal
    $xth(ctrl,te,sr).ball configure -state normal 
    $xth(ctrl,te,sr).bclear configure -state normal
    if {[llength $xth(te,flist)] > 1} {
      $xth(te,menu,file) entryconfigure "Next" -state normal
      $xth(te,menu,file) entryconfigure "Previous" -state normal
    } else {
      $xth(te,menu,file) entryconfigure "Next" -state disabled
      $xth(te,menu,file) entryconfigure "Previous" -state disabled
    }
    xth_te_sdata_enable ""
  } else {
    set xth(te,open_file) ""
    set xth(te,open_file_encoding) $xth(app,fencoding)
    $xth(te,menu,file) entryconfigure "Save" -state disabled
    $xth(te,menu,file) entryconfigure "Save as" -state disabled
    $xth(te,menu,file) entryconfigure "Save all" -state disabled
    $xth(te,menu,file) entryconfigure "Close" -state disabled
    $xth(te,menu,file) entryconfigure "Next" -state disabled
    $xth(te,menu,file) entryconfigure "Previous" -state disabled
    $xth(ctrl,te,files).ef.ecl configure -state disabled
    $xth(ctrl,te,files).ef.ecb configure -state disabled
    $xth(ctrl,te,files).ef.chb configure -state disabled
    $xth(ctrl,te,files).ef.cel configure -state disabled -text ""
    xth_te_sdata_disable ""
    $xth(te,menu) entryconfigure Edit -state disabled
    $xth(ctrl,te,sr).seal configure -state disabled
    $xth(ctrl,te,sr).seae configure -state disabled
    $xth(ctrl,te,sr).replc configure -state disabled
    $xth(ctrl,te,sr).reple configure -state disabled
    $xth(ctrl,te,sr).ccase configure -state disabled
    $xth(ctrl,te,sr).creg configure -state disabled
    $xth(ctrl,te,sr).csel configure -state disabled
    $xth(ctrl,te,sr).bfirst configure -state disabled
    $xth(ctrl,te,sr).bnext configure -state disabled
    $xth(ctrl,te,sr).ball configure -state disabled 
    $xth(ctrl,te,sr).bclear configure -state disabled
    xth_ctrl_minimize te sr
  }
  xth_app_title te
  
}


proc xth_te_set_encoding {} {

  global xth
 
  if {$xth(te,fcurr) >= 0} {
    # convert encoding into system's one
    set rxp "\\s+($xth(te,open_file_encoding))\\s+"
    if {[regexp -nocase $rxp $xth(encodings) dum temp]} {
      set xth(te,open_file_encoding) $temp
      set xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],encoding) $temp
      $xth(ctrl,te,files).ef.cel configure -text $temp
    } else {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message "uknown encoding -- $xth(te,open_file_encoding)" \
        -font $xth(gui,lfont)
    }
  }
}

proc xth_te_switch_file {fdr} {
  global xth
  set cf $xth(te,fcurr)
  if {$cf != -1} {
    incr cf $fdr
    if {$cf < 0} {
      set cf [expr [llength $xth(te,flist)] - 1]
    }
    if {$cf >= [llength $xth(te,flist)]} {
      set cf 0
    }
    xth_te_show_file $cf
  }
}


proc xth_te_create_file {} {

  global xth
  
  # create file variables
  incr xth(te,fltid)
  set cfid $xth(te,fltid)
  set xth(te,$cfid,name) [format "noname%02d$xth(app,te,fileext)" $cfid]
  set xth(te,$cfid,path) [file join $xth(gui,initdir) $xth(te,$cfid,name)]
  set xth(te,$cfid,newf) 1
  set xth(te,$cfid,encoding) $xth(app,fencoding)
  set xth(te,$cfid,frame) $xth(gui,te).af.apps.ff.file$cfid
  set cfr $xth(te,$cfid,frame)

  # create the frame and bind the events
  set iac {if {[string equal $xth(app,active) te]} }
  frame $cfr
  text $cfr.txt  -font $xth(gui,efont) -bg $xth(gui,ecolorbg) \
    -fg $xth(gui,ecolorfg) -insertbackground $xth(gui,ecolorfg) \
    -yscrollcommand "$cfr.sv set" \
    -xscrollcommand "$cfr.sh set" \
    -relief sunken \
    -selectbackground $xth(gui,ecolorselbg) \
    -selectforeground $xth(gui,ecolorselfg) \
    -selectborderwidth 0 \
    -wrap none
  if {$xth(gui,text_undo)} {
    $cfr.txt configure -undo 1 -maxundo -1
  }
  set xth(te,$cfid,otext) [$cfr.txt get 1.0 end]
  scrollbar $cfr.sv -orient vert  -command "$cfr.txt yview" \
    -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
  scrollbar $cfr.sh -orient horiz  -command "$cfr.txt xview" \
    -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
  bind $cfr.txt <Tab> "$iac {$xth(te,bind,text_tab)}"
  bind $cfr.txt <Return> "$iac {$xth(te,bind,text_return)}"
  bind $cfr.txt <<xthPositionChange>> "xth_te_update_position $cfr.txt"
  bind $cfr.txt <Key> "+ $iac {event generate $cfr.txt <<xthPositionChange>> -when tail}"
  bind $cfr.txt <Button-1> "+ $iac {event generate $cfr.txt <<xthPositionChange>> -when tail}"
  bind $cfr.txt <Control-Key-1> "$iac {xth_te_show_file 0}"
  bind $cfr.txt <Control-Key-2> "$iac {xth_te_show_file 1}"
  bind $cfr.txt <Control-Key-3> "$iac {xth_te_show_file 2}"
  bind $cfr.txt <Control-Key-4> "$iac {xth_te_show_file 3}"
  bind $cfr.txt <Control-Key-5> "$iac {xth_te_show_file 4}"
  bind $cfr.txt <Control-Key-6> "$iac {xth_te_show_file 5}"
  bind $cfr.txt <Control-Key-7> "$iac {xth_te_show_file 6}"
  bind $cfr.txt <Control-Key-8> "$iac {xth_te_show_file 7}"
  bind $cfr.txt <Control-Key-9> "$iac {xth_te_show_file 8}"
  bind $cfr.txt <Control-Key-0> "$iac {xth_te_show_file 9}"
  bind $cfr.txt <Control-Key-n> "$iac {xth_te_switch_file 1}"
  bind $cfr.txt <Control-Key-p> "$iac {xth_te_switch_file -1}"
  bind $cfr.txt <Control-Key-w> "$iac {xth_te_close_file}"
  bind $cfr.txt <Control-Key-a> "$iac {xth_te_select_all}"
  bind $cfr.txt <Control-Key-i> "$iac {xth_te_auto_indent}"
  bind $cfr.txt <Control-Key-s> "$iac {xth_te_save_file 0 $cfid}"
  bind $cfr.txt <Destroy> "xth_te_before_close_file $cfid yesno"  
#  if {$xth(gui,bindclip) == 1} {
    bind $cfr.txt <Control-Key-x> "$iac {tk_textCut $cfr.txt}"
    bind $cfr.txt <Control-Key-c> "$iac {tk_textCopy $cfr.txt}"
    bind $cfr.txt <Control-Key-v> "$iac {tk_textPaste $cfr.txt}"
    bind $cfr.txt <Control-Key-z> "$iac {catch {$cfr.txt edit undo}}"
    bind $cfr.txt <Control-Key-y> "$iac {catch {$cfr.txt edit redo}}"
  if {$xth(gui,bindinsdel)} {
    bind $cfr.txt <Shift-Key-Delete> "$iac {tk_textCut $cfr.txt}"
    bind $cfr.txt <Control-Key-Insert> "$iac {tk_textCopy $cfr.txt}"
    bind $cfr.txt <Shift-Key-Insert> "$iac {tk_textPaste $cfr.txt}"
#    catch {
#      bind $cfr.txt <Shift-Key-KP_Decimal> "$iac {tk_textCut $cfr.txt}"
#      bind $cfr.txt <Control-Key-KP_Insert> "$iac {tk_textCopy $cfr.txt}"
#      bind $cfr.txt <Shift-Key-KP_0> "$iac {tk_textPaste $cfr.txt}"
#    }
  }
#  }
    
  grid columnconf $cfr 0 -weight 1
  grid rowconf $cfr 0 -weight 1
  grid $cfr.txt -column 0 -row 0 -sticky news
  grid $cfr.sv -column 1 -row 0 -sticky news
  grid $cfr.sh -column 0 -row 1 -sticky news
  
  
  # add file to list and listbox
  set xth(te,flist) [linsert $xth(te,flist) end $cfid]
  $xth(ctrl,te,files).fl.flbox insert end "[llength $xth(te,flist)]. $xth(te,$cfid,name) ($xth(te,$cfid,path))"
  
  xth_te_show_file [expr [llength $xth(te,flist)] - 1]
}


# xth_te_read_file --
#
# return list containing
# {success name encoding text}

proc xth_te_read_file {pth} {

  global errorInfo xth

  set curenc utf-8
  set nm [file tail $pth]
  set encspc 0
  set flnn 0
  set success 1
  set txt ""
  if {[catch {set fid [open $pth r]}]} {
    set success 0
    set nm $errorInfo
    return [list $success $nm $curenc $txt]
  }
  fconfigure $fid -encoding $curenc
  while {[eof $fid] != 1} {
    gets $fid fln
    # replace tabs
    regsub -all {\t} $fln "  " fln
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
    } else {
#      if {$encspc == 0} {
#        if {[regexp {^\s*[^\#]+} $fln]} {
#          set success 0
#          set nm "$pth \[$flnn\] -- encoding command expected"
#          break
#        }
#      }
      append txt "$fln\n"
    }
  }
  close $fid
  return [list $success $nm $curenc $txt]
  
}  


# xth_te_write_file --
#
# return list containing
# {success name}

proc xth_te_write_file {pth enc txt} {

  global errorInfo xth

  set curenc utf-8
  set nm [file tail $pth]
  set success 1
  if {[catch {set fid [open $pth w]}]} {
    set success 0
    set nm $errorInfo
    return [list $success $nm]
  }
  
  fconfigure $fid -encoding $curenc -translation {auto lf}
  if {![string equal $xth(prj,name) svxedit]} {
    puts $fid "encoding  $enc"
  }
  fconfigure $fid -encoding $enc -translation {auto lf}
  puts -nonewline $fid $txt
  close $fid
  return [list $success $nm]
  
}


proc xth_te_destroy_file {} {

  global xth

  if {$xth(te,fcurr) >= 0} {
  

    # delete file from list and listbox and destroy windows
    set tempcurr $xth(te,fcurr)
    set cfid [lindex $xth(te,flist) $tempcurr]
    pack forget $xth(te,$cfid,frame)
    set xth(te,flist) [lreplace $xth(te,flist) $xth(te,fcurr) $xth(te,fcurr)]
    $xth(ctrl,te,files).fl.flbox delete $tempcurr    
    set xth(te,fcurr) -1

    # set other window to be active
    xth_te_show_file $tempcurr
    
    # destroy variable
    unset xth(te,$cfid,name)
    unset xth(te,$cfid,path)
    unset xth(te,$cfid,newf)
    unset xth(te,$cfid,encoding)
    unset xth(te,$cfid,frame)
    unset xth(te,$cfid,otext)
  }  
  
}


proc xth_te_open_file {dialogid fname fline} {

  global xth
  
  if {$dialogid} {
    set fname [tk_getOpenFile -filetypes $xth(app,te,filetypes) \
      -parent $xth(gui,main) \
      -initialdir $xth(gui,initdir) -defaultextension $xth(app,te,fileext)]
  }
  
  if {[string length $fname] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fname]
  }
  
  # now let's open file fname
  
  # check if not open exists
  for {set fid 0} {$fid < [llength $xth(te,flist)]} {incr fid} {
    if {[string equal $fname $xth(te,[lindex $xth(te,flist) $fid],path)]} {
      xth_te_show_file $fid
      return 1
    }
  }
  
  # read the file
  xth_status_bar_push te
  xth_status_bar_status te "Opening $fname ..."
  
  set fdata [xth_te_read_file $fname]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message [lindex $fdata 1] \
        -font $xth(gui,lfont)
      xth_status_bar_pop te
      return 0
  }
  
  # show the file
  xth_te_create_file
  set cfid [lindex $xth(te,flist) $xth(te,fcurr)]
  set xth(te,$cfid,name) [lindex $fdata 1]
  set xth(te,$cfid,path) $fname
  set xth(te,$cfid,newf) 0
  set xth(te,$cfid,encoding) [lindex $fdata 2]
  $xth(ctrl,te,files).ef.cel configure -text [lindex $fdata 2]
  regsub -all {\s*$} [lindex $fdata 3] "" ftext
  xth_te_insert_text $xth(te,$cfid,frame).txt "$ftext\n"
  catch {
    $xth(te,$cfid,frame).txt edit reset
  }
  set xth(te,$cfid,otext) [$xth(te,$cfid,frame).txt get 1.0 end]
  xth_te_show_file $xth(te,fcurr) 
  $xth(te,$cfid,frame).txt mark set insert "$fline.0"
  $xth(te,$cfid,frame).txt see insert
  if {$fline > 0} {
    $xth(te,$cfid,frame).txt tag remove sel 1.0 end
    $xth(te,$cfid,frame).txt tag add sel "$fline.0" "$fline.0 lineend"
  }
  
  xth_status_bar_pop te
  return 1
}

proc xth_te_before_close_file {cfid btns} {
  global xth
  if {
      [catch {winfo exists $xth(te,$cfid,frame).txt}] 
      || (![winfo exists $xth(te,$cfid,frame).txt])} {
    return 1
  }
  set ftext [$xth(te,$cfid,frame).txt get 1.0 end]
  if {[string compare $xth(te,$cfid,otext) $ftext] != 0} {    
    set wtd [MessageDlg $xth(gui,message) -parent $xth(gui,main) \
      -icon question -type $btns\
      -message "File $xth(te,$cfid,path) is not saved. Save it now?" \
      -font $xth(gui,lfont)]
    switch $wtd {
      0 {
        if {[xth_te_save_file 0 $cfid] == 0} {
          return 0
        }
      }
      1 {}
      default {return 0}
    }
  }
  return 1
}

proc xth_te_close_file {} {

  global xth
  
  if {$xth(te,fcurr) < 0} {
    return
  }
  set cfid [lindex $xth(te,flist) $xth(te,fcurr)]
  if {[xth_te_before_close_file $cfid yesnocancel]} {
    xth_te_destroy_file
    return 1
  } else {
    return 0
  }
  
}

proc xth_te_save_file {dialogid cfid} {

  global xth
  
  if {[llength $xth(te,flist)] == 0} {
    return 0
  }

  set fid [lsearch -exact $xth(te,flist) $cfid]
  if {$fid == -1} {
    return 0
  }
  
  set cfid [lindex $xth(te,flist) $fid]
  
  # let's check if we need to save
  set ftext [$xth(te,$cfid,frame).txt get 1.0 end]
  if {! $dialogid} {
    if {[string compare $xth(te,$cfid,otext) $ftext] == 0} {
        return 1
    }
  }
  
  xth_status_bar_push te
  
  if {$xth(te,$cfid,newf)} {
    set dialogid 1
  }

  set fname $xth(te,$cfid,path)
  set ofname $fname
  if {$dialogid} {
    set fname [tk_getSaveFile -filetypes $xth(app,te,filetypes) \
      -parent $xth(gui,main) \
      -initialfile [file tail $fname] -initialdir [file dirname $fname] \
      -defaultextension $xth(app,te,fileext)]
  }
  
  if {[string length $fname] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fname]
  }
  
  # save the file
  xth_status_bar_status te "Saving $fname ..."
  set fdata [xth_te_write_file $fname $xth(te,$cfid,encoding) $ftext]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message [lindex $fdata 1] \
        -font $xth(gui,lfont)
      xth_status_bar_pop te
      return
  }
  
  set xth(te,$cfid,otext) $ftext
  set xth(te,$cfid,newf) 0
  
  # if SaveAs, then redisplay the file
  if {$dialogid} {
    if {[string compare $ofname $fname] != 0} {
      set xth(te,$cfid,name) [lindex $fdata 1]
      set xth(te,$cfid,path) $fname
      xth_te_show_file $fid
    }
  }  

  after 250 {xth_status_bar_pop te}
  return 1
    
}

proc xth_te_save_all {} {

  global xth
  set ocur $xth(te,fcurr)
  foreach cfid $xth(te,flist) {
    xth_te_save_file 0 $cfid
  }
  xth_te_show_file $ocur
  
}


proc xth_te_update_position {W} {
  global xth
  regexp {(\d+)\.(\d+)} [$W index insert] dum lns pos
  $xth(gui,te).sf.pbar configure -text [format "%d.%d" [expr $lns + 1] $pos]
}


proc xth_te_text_select_all {txt} {
    $txt tag add sel 1.0 end
}


proc xth_te_select_all {} {
  global xth
  if {$xth(te,fcurr) > -1} {
    set cfid [lindex $xth(te,flist) $xth(te,fcurr)]
    $xth(te,$cfid,frame).txt tag add sel 1.0 end
  }
}


$xth(te,menu,file) add command -label "New" -command xth_te_create_file \
  -font $xth(gui,lfont) -underline 0
$xth(te,menu,file) add command -label "Open" -underline 0 \
  -accelerator "$xth(gui,controlk)-o" \
  -font $xth(gui,lfont) -command {xth_te_open_file 1 {} 1}
$xth(te,menu,file) add command -label "Save" -underline 0 \
  -accelerator "$xth(gui,controlk)-s" -state disabled \
  -font $xth(gui,lfont) -command {
    if {$xth(te,fcurr) >= 0} {
      xth_te_save_file 0 [lindex $xth(te,flist) $xth(te,fcurr)]
    }
  }
$xth(te,menu,file) add command -label "Save as" -underline 5 \
  -font $xth(gui,lfont) -state disabled -command {
    if {$xth(te,fcurr) >= 0} {
      xth_te_save_file 1 [lindex $xth(te,flist) $xth(te,fcurr)]
    }
  }
$xth(te,menu,file) add command -label "Save all" -underline 6 \
  -font $xth(gui,lfont) -state disabled -command xth_te_save_all
$xth(te,menu,file) add command -state disabled -label "Close" -underline 0 \
  -accelerator "$xth(gui,controlk)-w" \
  -font $xth(gui,lfont) \
  -command "xth_te_close_file"

$xth(te,menu,file) add separator
$xth(te,menu,file) add command -state disabled -label "Next" \
  -accelerator "$xth(gui,controlk)-n" \
  -font $xth(gui,lfont) -command "xth_te_switch_file 1" -underline 2
$xth(te,menu,file) add command -state disabled -label "Previous" \
  -accelerator "$xth(gui,controlk)-p" \
  -font $xth(gui,lfont) -command "xth_te_switch_file -1" -underline 0
  
set xth(te,menu,edit) $xth(te,menu).edit
menu $xth(te,menu,edit) -tearoff 0
$xth(te,menu) add cascade -label "Edit" -state disabled \
  -font $xth(gui,lfont) -menu $xth(te,menu,edit) -underline 0
if {$xth(gui,text_undo)} {
  $xth(te,menu,edit) add command -label "Undo" -font $xth(gui,lfont) \
    -accelerator "$xth(gui,controlk)-z" -command "xth_app_clipboard undo"
  $xth(te,menu,edit) add command -label "Redo" -font $xth(gui,lfont) \
    -accelerator "$xth(gui,controlk)-y" -command "xth_app_clipboard redo"
  $xth(te,menu,edit) add separator
}
$xth(te,menu,edit) add command -label "Cut" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-x" -command "xth_app_clipboard cut"
$xth(te,menu,edit) add command -label "Copy" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-c" -command "xth_app_clipboard copy"
$xth(te,menu,edit) add command -label "Paste" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-v" -command "xth_app_clipboard paste"
$xth(te,menu,edit) add separator
$xth(te,menu,edit) add command -label "Select all" -font $xth(gui,lfont) \
  -accelerator "$xth(gui,controlk)-a" -command "xth_te_select_all"
$xth(te,menu,edit) add command -label "Auto indent" -font $xth(gui,lfont) \
  -command "xth_te_auto_indent" -accelerator "$xth(gui,controlk)-i"

proc xth_te_get_indent {w i cilc} {

  global xth
  set indls ""
  set cmdls ""
  set cmd0s ""
  set cmdl 0
  regexp {(\d+)\.} $i dum cln
  set line0 [$w get $cln.0 $cln.end]
  regexp {\S+} $line0 cmd0s
  if {[info exists xth(cmd,$cmd0s)]} {
    set cmd0 $xth(cmd,$cmd0s)
  } else {
    set cmd0 0
  }
  set sln [expr $cln - 1]
  set line1 [$w get $sln.0 $sln.end]
  set linel $line1
  set hasl 0
  set escan 0
  while {($sln > 1) && (! $hasl)} {
    incr sln -1
    if {[regexp {\S} $linel]} {
      set cline [$w get $sln.0 $sln.end]
      if {[regexp {\\\s*$} $cline]} {
        set linel $cline
      } else {
        set hasl 1
      }
    } else {
      set linel [$w get $sln.0 $sln.end]
    }
  }
  regexp {\S+} $linel cmdls
  if {[info exists xth(cmd,$cmdls)]} {
    set cmdl $xth(cmd,$cmdls)
    set endcmdls $xth(endcmd,$cmdls)
  } else {
    set cmdl 0
  }
  regexp {^\s+} $linel indls
  set indl [string length $indls]
  set bsl1 [regexp {\\\s*$} $line1]
  
  # preskenuje prikazy nad
  #puts "cilc |$cilc|\nindl |$indl|\ncmd0 |$cmd0|\ncmd0s |$cmd0s|\ncmdl |$cmdl|\ncmdls |$cmdls|\n"
  if {$cmdl == 1} {
#    puts "$cln. cilc |$cilc|"
    set cmdcomct 0
    set cmdcomctfi 1
    set enddetect 0
    set set_cmd_counts {
      regexp {\S+} $slns cmdcomx
#      puts "$cmdls ?? $cmdcomx"
      if {[string compare $endcmdls $cmdcomx] == 0} {
        set endscan 1
        set enddetect 1
      } elseif {[string compare $cmdls $cmdcomx] == 0} {
        if {! $cmdcomctfi} {
          if {$cmdcomct} {
            set endscan 1
          }
          incr cmdcomct
        } else {
          set cmdcomctfi 0
        }
      }
    }
    set sln [expr $cln - 1]
    set slns $line1
    set endscan 0
    while {($sln > 1) && (!$endscan)} {
      incr sln -1
      if {[regexp {\S} $slns]} {
        set clns [$w get $sln.0 $sln.end]
        if {[regexp {(.*)\\\s*$} $clns dum vlns]} {
          set slns "$vlns$slns"
        } else {
          eval $set_cmd_counts
          set slns $clns
        }
      } else {
        set slns [$w get $sln.0 $sln.end]
      }
    }
    if {!$enddetect} {
      eval $set_cmd_counts
    }
 #   puts $cmdcomct
    if {$cmdcomct > 0} {
      set cmdl 0
    }
  }
  # koniec scanovania
  
  if {$bsl1} {
    set ind [expr $indl + 2 * $xth(gui,etabsize)]
  } else {
    set ind $indl
    if {$cmdl > 0} {
      incr ind $xth(gui,etabsize)
    } 
    if {$cilc && ($cmd0 < 0)} {
      incr ind -$xth(gui,etabsize)
    } 
  }
  
  if {$ind > 0} {
    return [format %$ind\s " "]
  } else {
    return ""
  }
}

proc xth_te_auto_indent {} {

  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  set cfid [lindex $xth(te,flist) $xth(te,fcurr)]
  set W $xth(te,$cfid,frame).txt
  set rngs [$W tag ranges sel]
  set fln 1
  set tln -1
  regexp {(\d+)\.} [lindex $rngs 0] dum fln
  regexp {(\d+)\.} [lindex $rngs 1] dum tln
  xth_status_bar_push te
  for {set cln $fln} {$cln < $tln} {incr cln} {
    xth_status_bar_status te "Processing line $cln ..."
    $W see $cln.0
    set spcs ""
    regexp {^\s+} [$W get $cln.0 $cln.end] spcs
    set spcsc [string length $spcs]
    set indct [string length [xth_te_get_indent $W $cln.0 1]]
    if {$spcsc == $indct} {
    } elseif {$spcsc > $indct} {
      $W delete $cln.0 $cln.[expr $spcsc - $indct]
    } elseif {$spcsc < $indct} {
      $W insert $cln.0 [format \x25[expr $indct - $spcsc]s " "]
    }
  }
  $W see insert
  # $W tag remove sel 1.0 end
  xth_status_bar_pop te
}


proc xth_te_text_auto_indent {W} {

  set rngs [$W tag ranges sel]
  set fln 1
  set tln -1
  regexp {(\d+)\.} [lindex $rngs 0] dum fln
  regexp {(\d+)\.} [lindex $rngs 1] dum tln
  for {set cln $fln} {$cln < $tln} {incr cln} {
    $W see $cln.0
    set spcs ""
    regexp {^\s+} [$W get $cln.0 $cln.end] spcs
    set spcsc [string length $spcs]
    set indct [string length [xth_te_get_indent $W $cln.0 1]]
    if {$spcsc == $indct} {
    } elseif {$spcsc > $indct} {
      $W delete $cln.0 $cln.[expr $spcsc - $indct]
    } elseif {$spcsc < $indct} {
      $W insert $cln.0 [format \x25[expr $indct - $spcsc]s " "]
    }
  }
  $W see insert
  
}

xth_ctrl_minimize te sr

