##
## help.tcl --
##
##     Help menu and its functions.  
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


set hm "$xth(gui,main).hmenu"
set xth(gui,menu,help) $hm

menu $hm -tearoff 0
$hm add command -label "BAC calculator..." -underline 0 -font $xth(gui,lfont) \
  -command xth_bac_init
$hm add command -label "About..." -underline 0 -font $xth(gui,lfont) \
  -command {
    xth_about_show 1
    xth_about_status $xth(prj,title)
  }
