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
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
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

set xth(datafmts) [list \
  "unknown      $xth(datafmt,unknown)" \
  "station      $xth(datafmt,station)" \
  "from         $xth(datafmt,station)" \
  "to           $xth(datafmt,station)" \
  "compass      $xth(datafmt,bearing)" \
  "backcompass  $xth(datafmt,bearing)" \
  "bearing      $xth(datafmt,bearing)" \
  "backbearing  $xth(datafmt,bearing)" \
  "tape         $xth(datafmt,length)" \
  "length       $xth(datafmt,length)" \
  "count        $xth(datafmt,counter)" \
  "counter      $xth(datafmt,counter)" \
  "fromcount    $xth(datafmt,counter)" \
  "tocount      $xth(datafmt,counter)" \
  "fromcounter  $xth(datafmt,counter)" \
  "tocounter    $xth(datafmt,counter)" \
  "gradient     $xth(datafmt,gradient)" \
  "clino        $xth(datafmt,gradient)" \
  "backgradient $xth(datafmt,gradient)" \
  "backclino    $xth(datafmt,gradient)" \
  "depth        $xth(datafmt,depth)" \
  "fromdepth    $xth(datafmt,depth)" \
  "todepth      $xth(datafmt,depth)" \
  "depthchange  $xth(datafmt,depth)" \
  "dx           $xth(datafmt,length)" \
  "dy           $xth(datafmt,length)" \
  "dz           $xth(datafmt,length)" \
  "northing     $xth(datafmt,length)" \
  "easting      $xth(datafmt,length)" \
  "altitude     $xth(datafmt,length)" \
  "up           $xth(datafmt,dimensions)" \
  "down         $xth(datafmt,dimensions)" \
  "left         $xth(datafmt,dimensions)" \
  "right        $xth(datafmt,dimensions)" \
  "ceiling      $xth(datafmt,dimensions)" \
  "floor        $xth(datafmt,dimensions)" \
]

foreach datafmt $xth(datafmts) {
  set qt [lindex $datafmt 0]
  set xth(datafmt,$qt,format) [lindex $datafmt 1]
  set xth(datafmt,$qt,special) [lindex $datafmt 2]
}
