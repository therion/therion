##
## syntax.tcl --
##
##     Syntax module.  
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

# prepare the syntax commands
foreach cmd $xth(icmds) {
  set xth(cmd,$cmd) 2
  set xth(endcmd,$cmd) end$cmd
  set xth(endcmd,end$cmd) ""
  set xth(cmd,end$cmd) -2
}

foreach cmd $xth(cmds) {
  set xth(cmd,$cmd) 1
  set xth(endcmd,$cmd) end$cmd
  set xth(endcmd,end$cmd) ""
  set xth(cmd,end$cmd) -1
}

foreach datafmt $xth(datafmts) {
  set qt [lindex $datafmt 0]
  set xth(datafmt,$qt,format) [lindex $datafmt 1]
  set xth(datafmt,$qt,special) [lindex $datafmt 2]
}
