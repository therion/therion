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
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
## --------------------------------------------------------------------

xth_app_finish
xth_app_clock

encoding system $xth(app,sencoding)
set xth(encoding_system) [encoding system]
xth_about_hide

wm deiconify $xth(gui,main)
if {$xth(gui,init_app_normalized)} {
  xth_app_normalize
}

foreach xapp $xth(app,list) {
  catch {
    set xth(app,$xapp,relw) $xth(app,all,tbwidth)
    set xth(app,$xapp,relw) $xth(app,$xapp,tbwidth)
  } 
  catch {
    xth_app_show $xapp
  }
}

if {[llength $xth(app,list)] > 2} {
  xth_app_show [lindex $xth(app,list) 2]
} else {
  xth_app_show [lindex $xth(app,list) 0]
}


foreach idir $xth(idirs) {
  catch {source [file join $idir xtherion.ini]}
}
catch {source xtherion.ini}

set xth(gui,initdir) [file normalize $xth(gui,initdir)]

set th2open 1
set cfgopen 1

foreach fnm $argv {
  set fname $fnm
  catch {
    set fname [file normalize $fnm]
  }
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
  } else {
    xth_app_show te
    update idletasks
    xth_te_open_file 0 $fname 0
  }
}


