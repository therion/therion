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
if {[llength $xth(app,list)] > 2} {
  xth_app_show [lindex $xth(app,list) 2]
} else {
  xth_app_show [lindex $xth(app,list) 0]
}

xth_app_clock

encoding system $xth(app,sencoding)
set xth(encoding_system) [encoding system]
xth_about_hide
wm deiconify $xth(gui,main)
xth_app_maximize
