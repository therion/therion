##
## scroll.tcl --
##
##     Special scrollbar commands.  
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


proc xth_scroll_showcmd {sbar cmd} {
  global xth
  set xth(scroll,$sbar,show) $cmd
  set xth(scroll,$sbar,open) 0
}

proc xth_scroll_hidecmd {sbar cmd} {
  global xth
  set xth(scroll,$sbar,hide) $cmd
  set xth(scroll,$sbar,open) 0
}

proc xth_scroll {sbar first last} {
  global xth
  if {[expr $first == 0.0] && [expr $last == 1.0]} {
    if {$xth(scroll,$sbar,open) == 1} {
      set xth(scroll,$sbar,open) 0
      eval $xth(scroll,$sbar,hide)
      update idletasks
    }
  } else {
    if {$xth(scroll,$sbar,open) == 0} {
      set xth(scroll,$sbar,open) 1
      eval $xth(scroll,$sbar,show)
      update idletasks
    }
    $sbar set $first $last
  }
}
