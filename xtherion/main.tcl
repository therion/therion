##
## main.tcl --
##
##     Main script.   
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

xth_app_finish
xth_app_clock

encoding system $xth(app,sencoding)
set xth(encoding_system) [encoding system]
if {[string length $xth(about,nvr)] > 0} {
  xth_about_status $xth(prj,title)
  after $xth(about,infotime) xth_about_hide
} else {
  xth_about_hide
}

wm deiconify $xth(gui,main)
xth_app_normalize

foreach xapp $xth(app,list) {
  catch {
    set xth(app,$xapp,relw) $xth(app,$xapp,tbwidth)
    xth_app_show $xapp
  }
}

if {[llength $xth(app,list)] > 2} {
  xth_app_show [lindex $xth(app,list) 2]
} else {
  xth_app_show [lindex $xth(app,list) 0]
}


set th2open 1
set cfgopen 1

foreach fname $argv {
  if {$cfgopen && [regexp -nocase {thconfig|thcfg} $fname]} {
    set cfgopen 0
    xth_app_show cp
    update idletasks
    xth_cp_open_file $fname
  } elseif {$th2open && [regexp -nocase {\.th2$} $fname]} {
    set th2open 0
    xth_app_show me
    update idletasks
    xth_me_open_file 0 $fname 1
  } elseif {[regexp -nocase {\.thm$} $fname]} {
    if {[string length $xthmvw] > 0} {
      xth_app_show mv
      update idletasks
      xth_mv_open_file $fname
    }
  } else {
    xth_app_show te
    update idletasks
    xth_te_open_file 0 $fname 1
  }
}

