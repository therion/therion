#! /usr/bin/wish

proc xth_dbg_dbg {} {
  global xth
  set f $xth(gui,dbg)
  $f.cmd configure -values [linsert [$f.cmd cget -values] 0 $xth(ctrl,dbg,cmd)]
  set tmp {}
  if {[regexp {^\s*\$} $xth(ctrl,dbg,cmd)]} {
    set tmpcmd "set tmp $xth(ctrl,dbg,cmd)"
  } else {
    set tmpcmd "set tmp \[$xth(ctrl,dbg,cmd)\]"
  }
  $f.txt delete 1.0 end
  set xth(ctrl,dbg,cmd) {}
  eval $tmpcmd
  $f.txt insert 1.0 $tmp  
}

proc xth_dbg_init {} {

  global xth
  
  set f $xth(gui,dbg)
  toplevel $f
  wm title $f "Debug window"

  ComboBox $f.cmd -font $xth(gui,lfont) -height 8 \
    -textvariable xth(ctrl,dbg,cmd) -command xth_dbg_dbg

  # initialize text editor
  set txb $f
  text $txb.txt -height 12 -wrap none -font $xth(gui,efont) \
  -bg $xth(gui,ecolorbg) \
  -fg $xth(gui,ecolorfg) -insertbackground $xth(gui,ecolorfg) \
  -relief sunken -selectbackground $xth(gui,ecolorselbg) \
  -selectforeground $xth(gui,ecolorselfg) \
  -selectborderwidth 0 \
  -yscrollcommand "$txb.sv set" \
  -xscrollcommand "$txb.sh set" 
  scrollbar $txb.sv -orient vert  -command "$txb.txt yview" \
    -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)
  scrollbar $txb.sh -orient horiz  -command "$txb.txt xview" \
    -takefocus 0 -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)

  grid columnconf $f 0 -weight 1
  grid columnconf $f 1 -weight 0
  grid rowconf $f 1 -weight 1
  grid $f.cmd -column 0 -row 0 -columnspan 2 -sticky news
  grid $f.txt -column 0 -row 1 -sticky news
  grid $f.sv -column 1 -row 1 -sticky news
  grid $f.sh -column 0 -row 2 -sticky news
      
  wm geometry $f 480x320+100+100
  update idletasks
  
}


if 0 {
  package require BWidget
  source ver.tcl
  source global.tcl
  wm withdraw .
  xth_dbg_init
} else {
  xth_dbg_init
}

