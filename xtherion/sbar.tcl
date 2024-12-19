##
## sbar.tcl --
##
##     Status bar module.  
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
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
## --------------------------------------------------------------------


proc xth_status_bar {aname widg stext} {
  global xth  
  if {$xth(gui,balloons)} {
    DynamicHelp::add $widg -type balloon -text $stext
  }
  set anml $aname
  foreach aname $anml {
    set sbar $xth(gui,$aname).sf.sbar
    set widgx "$aname$widg"
    set xth(gui,sbar,$widgx,exp) 0  
    bind $widg <FocusIn> "+ if {\$xth(gui,sbar,$widgx,exp) == 0} {catch {set xth(gui,sbar,$widgx,exp) 1; set xth(gui,sbar,$widgx,otext) \[$sbar cget -text\]; $sbar configure -text [list $stext]}}"
    bind $widg <Enter> "+ if {\$xth(gui,sbar,$widgx,exp) == 0} {catch {set xth(gui,sbar,$widgx,exp) 1; set xth(gui,sbar,$widgx,otext) \[$sbar cget -text\]; $sbar configure -text [list $stext]}}"
    bind $widg <FocusOut> "+ if {\$xth(gui,sbar,$widgx,exp) == 1} {catch {$sbar configure -text \$xth(gui,sbar,$widgx,otext); set xth(gui,sbar,$widgx,exp) 0}}"
    bind $widg <Leave> "+ if {\$xth(gui,sbar,$widgx,exp) == 1} {catch {$sbar configure -text \$xth(gui,sbar,$widgx,otext); set xth(gui,sbar,$widgx,exp) 0}}"
  }
}

proc xth_status_bar_push aname {
  global xth
  catch {
  set sbar $xth(gui,$aname).sf.sbar
  if {![info exists xth(gui,sbar,$aname)]} {
    set xth(gui,sbar,$aname) [$sbar cget -text]
  } else {
    set xth(gui,sbar,$aname) [lappend $xth(gui,sbar,$aname) [$sbar cget -text]]
  }
  }
}


proc xth_status_bar_pop aname {
  global xth
  catch {
  set sbar $xth(gui,$aname).sf.sbar
  if {! [info exists xth(gui,sbar,$aname)]} {
    set xth(gui,sbar,$aname) ""
  } else {
    $sbar configure -text [lindex $xth(gui,sbar,$aname) 0]
    set xth(gui,sbar,$aname) [lreplace $xth(gui,sbar,$aname) 0 0]
  }
  }
}


proc xth_status_bar_status {aname txt} {
  global xth
  catch {
  set sbar $xth(gui,$aname).sf.sbar
  $sbar configure -text $txt
  update idletasks
  }
}

