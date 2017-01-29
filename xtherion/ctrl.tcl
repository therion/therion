##
## ctrl.tcl --
##
##     Controller module.
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

set xth(ctrl,all,number) 0

proc xth_ctrl_create {aname} {

  global xth
  
  set cf $xth(gui,$aname).af.ctrl  
    
  canvas $cf.c -yscrollcommand "xth_scroll $cf.sv" \
    -highlightthickness 0
  scrollbar $cf.sv -orient vert  -command "$cf.c yview" -takefocus 0 \
    -width $xth(gui,sbwidth) -borderwidth $xth(gui,sbwidthb)

  grid columnconf $cf 0 -weight 1
  grid rowconf $cf 0 -weight 1
  xth_scroll_showcmd $cf.sv "grid $cf.sv -row 0 -column 1 -sticky nsew; update idletasks; xth_ctrl_reshape te"
  xth_scroll_hidecmd $cf.sv "grid forget $cf.sv; update idletasks; xth_ctrl_reshape te"
  grid $cf.c -row 0 -column 0 -sticky nsew
  
  set xth(ctrl,$aname,number) 0
  set xth(ctrl,$aname,list) {}
}

proc xth_ctrl_add {aname cname title} {
  
  global xth
  
  incr xth(ctrl,$aname,number)
  incr xth(ctrl,all,number)
  lappend xth(ctrl,$aname,list) $cname
  set cn $xth(ctrl,$aname,number)
  set cf $xth(gui,$aname).af.ctrl  
  
  set ccf $cf.cf$cn
  frame $ccf
  frame $ccf.f 
  set cid [$cf.c create window 0 0 -window $ccf -anchor nw]
  set xth(ctrl,$aname,$cname) $ccf.f
  set xth(ctrl,$aname,$cname,frm) $ccf
  set xth(ctrl,$aname,$cname,pos) $cn
  set xth(ctrl,$aname,$cname,max) 1
  set xth(ctrl,$aname,$cname,menu) .xth_popup$xth(ctrl,all,number)
  set xth(ctrl,$aname,$cn) $cid 
  
  menu .xth_popup$xth(ctrl,all,number) -tearoff 0
  button $ccf.rb -text "$title" -command "xth_ctrl_minmax $aname $cname" \
    -font $xth(gui,lfont) -bg #aaaaaa \
    -fg white -bg darkBlue -activebackground lightBlue \
    -anchor w -relief flat \
    -takefocus 0
  bind $ccf.rb <Button-$xth(gui,rmb)> "tk_popup .xth_popup$xth(ctrl,all,number) %X %Y"
  xth_status_bar $aname $ccf.rb [mc "Show or hide this control panel"]
  
  pack $ccf.rb -side top -fill x -expand 1
  pack $ccf.f -expand yes -fill both

  xth_ctrl_reshape $aname
}


proc xth_ctrl_finish {aname} {

  global xth  

  foreach ct $xth(ctrl,$aname,list) {
    set cmn $xth(ctrl,$aname,$ct,menu)
    foreach oct $xth(ctrl,$aname,list) {
      if {[string compare $ct $oct] != 0} {
        $cmn add command -label [$xth(ctrl,$aname,$oct,frm).rb cget -text] \
          -command "xth_ctrl_replace $aname $ct $oct" -font $xth(gui,lfont)
      }
    }
  }

}


proc xth_ctrl_reshape {aname} {
  
  global xth
  set cn $xth(ctrl,$aname,number)
  set cnv $xth(gui,$aname).af.ctrl.c  
  
  # position the windows
  set height 0
  set width [winfo width $cnv]
  for {set i 1} {$i <= $cn} {incr i} {
    set cid $xth(ctrl,$aname,$i)
    set cw [$cnv itemcget $cid -window]
    $cnv coord $cid 0 $height
    $cnv itemconfigure $cid -width $width
    incr height [winfo height $cw]
  }

  $cnv configure -scrollregion "0 0 $width $height"
}

proc xth_ctrl_minmax {aname cname} {

  global xth

  set cmm $xth(ctrl,$aname,$cname,max)
  if {$cmm == 1} {
    pack forget $xth(ctrl,$aname,$cname)
    $xth(ctrl,$aname,$cname,frm).rb configure -relief raised
    set cmm 0
  } else {
    pack $xth(ctrl,$aname,$cname) -expand yes -fill both
    $xth(ctrl,$aname,$cname,frm).rb configure -relief flat
    set cmm 1
  }
  set xth(ctrl,$aname,$cname,max) $cmm
  
  update idletasks
  xth_ctrl_reshape $aname
}

proc xth_ctrl_replace {aname ccname dcname} {
  
  global xth

  set cnv $xth(gui,$aname).af.ctrl.c
  set p1 $xth(ctrl,$aname,$ccname,pos)
  set p2 $xth(ctrl,$aname,$dcname,pos)
  
  set xth(ctrl,$aname,$ccname,pos) $p2
  set xth(ctrl,$aname,$dcname,pos) $p1
  $cnv itemconfigure $xth(ctrl,$aname,$p1) \
    -window $xth(ctrl,$aname,$dcname,frm)
  $cnv itemconfigure $xth(ctrl,$aname,$p2) \
    -window $xth(ctrl,$aname,$ccname,frm)
    
  update idletasks
  xth_ctrl_reshape $aname
  
}

proc xth_ctrl_minimize {aname cname} {
  global xth
  set xth(ctrl,$aname,$cname,max) 1
  xth_ctrl_minmax $aname $cname
}

proc xth_ctrl_maximize {aname cname} {
  global xth
  set xth(ctrl,$aname,$cname,max) 0
  xth_ctrl_minmax $aname $cname
}

proc xth_ctrl_scroll_to {aname cname} {
  global xth
  set cf $xth(gui,$aname).af.ctrl
  set sr [$cf.c cget -scrollregion]
  set wp [$cf.c coord $xth(ctrl,$aname,[expr [lsearch $xth(ctrl,$aname,list) [lindex $xth(ctrl,$aname,list) [expr $xth(ctrl,$aname,$cname,pos) - 1]]] + 1])]
#  set wp [$cf.c coord $xth(ctrl,$aname,[expr [lsearch $xth(ctrl,$aname,list) $cname] + 1])]
  $cf.c yview moveto [expr [lindex $wp 1] / ([lindex $sr 3] - [lindex $sr 1])]   
}

