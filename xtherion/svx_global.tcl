##
## svx_global.tcl --
##
##     Global variables for svxedit.   
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


# file extensions
set xth(app,te,filetypes) {    
  {{Survex files}       {.svx}}    
  {{Text files}       {.txt}}    
  {{All files}       {*}}    
}
set xth(app,te,fileext) {.svx}

# command indenting
set xth(icmds) {}
set xth(cmds) {}
set xth(cmd,*begin) 2
set xth(endcmd,*begin) "*end"
set xth(cmd,*end) -2
set xth(endcmd,*end) ""

# application titles
set xth(prj,name) "svxedit"
set xth(prj,title) "survex source editor"
set xth(about,info) "svxedit v1.0 beta\n \u00A9 2002 Stacho Mudrak"

# fonts :-)
case $tcl_platform(platform) {
  unix {
    set xth(gui,lfont) "Helvetica 10"
    set xth(gui,efont) {fixed 10 roman}
  }
  windows {
    set xth(gui,efont) "Courier 12 roman"
  }
  macintosh {
  }
}
