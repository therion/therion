#! /usr/bin/wish
## source.tcl --
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

set xth(debug) 1

#@ALL
source ver.tcl
source ivc.tcl
source global.tcl
#@XTHERION
source therion.tcl
#@SURVEX
if 0 {
source svx_global.tcl
}
#@ALL
source about.tcl
source syntax.tcl
source init.tcl
source sbar.tcl
source scroll.tcl
source help.tcl
source ctrl.tcl
source app.tcl
source te.tcl
source te_sdata.tcl
source te_sdata2.tcl
#@XTHERION
source me_imgs.tcl
source me_cmds.tcl
source me_cmds2.tcl
source me_ss.tcl
source me.tcl
source cp_procs.tcl
source bac.tcl
source cp.tcl
source mv.tcl
source mv_procs.tcl
#@DEBUG
if {![string equal $xth(gui,platform) windows]} {
  source console.tcl
} else {
  source dbg.tcl
}
#@ALL
source main.tcl


