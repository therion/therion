##
## global.tcl --
##
##     Global variables definition.   
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



set xth(destroyed) 0
set xth(prj,name) "therion"
set xth(prj,title) "therion user interface"
set xth(gui,main) ".xth"
set xth(gui,about) ".xth_about"
set xth(gui,message) ".xthmsg"
set xth(gui,minsize) {480 300}

set xth(gui,compshow) 0
set xth(gui,compcmd) "therion"

set xth(encodings) { iso8859-1 iso8859-2 iso8859-5 iso8859-7 utf-8 }
set xth(length_units) {m cm in ft yd}
set xth(angle_units) {deg min grad}
set xth(point_types) {}
set xth(line_types) {}
set xth(scrap_projections) {plan elevation extended none}
set xth(app,te,filetypes) {    
  {{Therion files}       {.th}}    
  {{2D therion files}       {.th2}}    
  {{All files}       {*}}    
}
set xth(app,te,fileext) {}

set xth(app,me,filetypes) {    
  {{Therion 2D files}       {.th2}}    
  {{Therion files}       {.th}}    
  {{All files}       {*}}    
}

set xth(app,cp,filetypes) {    
  {{Therion config files}       {thconfig*}}    
  {{All files}       {*}}    
}

set xth(icmds) {survey}
set xth(cmds) {scrap centerline grade line area map}
set dfs {8s}                    
set dfss {8s}                    
set dfuf {8.2fx {-}}             
set dfdf {+8.2fx {-}}            
set dfcf {8.2fx {-}}             
set dfccf {8.2f {-}}            
set dfgf {{8.2fx} {up down -}} 
set xth(datafmts) [list \
  "unknown      $dfs" \
  "station      $dfss" \
  "from         $dfss" \
  "to           $dfss" \
  "compass      $dfuf" \
  "backcompass  $dfuf" \
  "bearing      $dfuf" \
  "backbearing  $dfuf" \
  "tape         $dfcf" \
  "length       $dfcf" \
  "count        $dfccf" \
  "counter      $dfccf" \
  "fromcount    $dfccf" \
  "tocount      $dfccf" \
  "fromcounter  $dfccf" \
  "tocounter    $dfccf" \
  "gradient     $dfgf" \
  "clino        $dfgf" \
  "backgradient $dfgf" \
  "backclino    $dfgf" \
  "depth        $dfdf" \
  "fromdepth    $dfdf" \
  "todepth      $dfdf" \
  "depthchange  $dfdf" \
  "dx           $dfcf" \
  "dy           $dfcf" \
  "dz           $dfcf" \
  "northing     $dfcf" \
  "easting      $dfcf" \
  "altitude     $dfcf" \
]

set xth(gui,initdir) [pwd]
set xth(app,active) ""
set xth(app,list) {}
set xth(app,all,relw) -1
set xth(app,all,wmwd) 180
set xth(app,all,wpsw) 1

set xth(app,fencoding) iso8859-2
set xth(app,sencoding) iso8859-2

# autodetect some options
frame .def
scrollbar .def.scrollbar
text .def.text
if {[catch {.def.text configure -undo 1}]} {
  set xth(gui,text_undo) 0;
} else {
  set xth(gui,text_undo) 1;
}
label .def.label

set xth(gui,sbwidth) [.def.scrollbar cget -width]
set xth(gui,sbwidthb) [.def.scrollbar cget -borderwidth]
set xth(gui,lfont) [.def.label cget -font]
set xth(gui,efont) [.def.text cget -font]
set xth(gui,ecolorbg) black
set xth(gui,ecolorfg) green
set xth(gui,escolorbg) black
set xth(gui,escolorfg) red
set xth(gui,ecolorselbg) green
set xth(gui,ecolorselfg) black
set xth(gui,selfg) white
set xth(gui,selbg) darkBlue
set xth(gui,etabsize) 2
set xth(gui,controlk) Ctrl

destroy .def
# end of options autodetection

# map editor settings
set xth(gui,me,scrap,psize) 4
set xth(gui,me,point,psize) 4
set xth(gui,me,point,cpsize) 5
set xth(gui,me,line,psize) 4
set xth(gui,me,line,cpsize) 4
set xth(gui,me,line,spsize) 5

set xth(gui,bindinsdel) 1

# platform dependend settings
case $tcl_platform(platform) {
  unix {
    set xth(gui,sbwidth) 9
    set xth(gui,sbwidthb) 1
    set xth(gui,efont) {fixed -20}
    set xth(gui,platform) unix
    set xth(gui,cursor) top_left_arrow
    set xth(gui,compshow) 1
  }
  windows {
    package require registry
    catch {
      set xth(gui,compcmd) "\"[file join [registry get {HKEY_LOCAL_MACHINE\SOFTWARE\Therion} InstallDir] therion.exe]\""
    }
    set xth(gui,efont) "Courier 16 roman bold"
    set xth(gui,platform) windows
    set xth(gui,cursor) arrow
    set xth(app,sencoding) cp1250
    set xth(gui,bindinsdel) 0
    if {[catch {
      set fid [open "|cmd.exe /c" r]
      read $fid;
      close $fid
    }]} {
      set xth(gui,compcmd) "command.com /c $xth(gui,compcmd)"
    } else {
      set xth(gui,compcmd) "cmd.exe /c $xth(gui,compcmd)"
    }
  }
  macintosh {
    set xth(gui,controlk) Cmd
    set xth(gui,platform) macintosh
    set xth(gui,cursor) arrow
    set xth(gui,bindinsdel) 0
  }
}
# end of platform dependend settings

set xth(about,info) "xtherion $xth(about,ver)\n \u00A9 2002-2003 Stacho Mudrak"
set xth(about,image_data) {
R0lGODlhwACQAOcAAAAAAAAAVQAAqgAA/wAkAAAkVQAkqgAk/wBJAABJVQBJ
qgBJ/wBtAABtVQBtqgBt/wCSAACSVQCSqgCS/wC2AAC2VQC2qgC2/wDbAADb
VQDbqgDb/wD/AAD/VQD/qgD//yQAACQAVSQAqiQA/yQkACQkVSQkqiQk/yRJ
ACRJVSRJqiRJ/yRtACRtVSRtqiRt/ySSACSSVSSSqiSS/yS2ACS2VSS2qiS2
/yTbACTbVSTbqiTb/yT/ACT/VST/qiT//0kAAEkAVUkAqkkA/0kkAEkkVUkk
qkkk/0lJAElJVUlJqklJ/0ltAEltVUltqklt/0mSAEmSVUmSqkmS/0m2AEm2
VUm2qkm2/0nbAEnbVUnbqknb/0n/AEn/VUn/qkn//20AAG0AVW0Aqm0A/20k
AG0kVW0kqm0k/21JAG1JVW1Jqm1J/21tAG1tVW1tqm1t/22SAG2SVW2Sqm2S
/222AG22VW22qm22/23bAG3bVW3bqm3b/23/AG3/VW3/qm3//5IAAJIAVZIA
qpIA/5IkAJIkVZIkqpIk/5JJAJJJVZJJqpJJ/5JtAJJtVZJtqpJt/5KSAJKS
VZKSqpKS/5K2AJK2VZK2qpK2/5LbAJLbVZLbqpLb/5L/AJL/VZL/qpL//7YA
ALYAVbYAqrYA/7YkALYkVbYkqrYk/7ZJALZJVbZJqrZJ/7ZtALZtVbZtqrZt
/7aSALaSVbaSqraS/7a2ALa2Vba2qra2/7bbALbbVbbbqrbb/7b/ALb/Vbb/
qrb//9sAANsAVdsAqtsA/9skANskVdskqtsk/9tJANtJVdtJqttJ/9ttANtt
Vdttqttt/9uSANuSVduSqtuS/9u2ANu2Vdu2qtu2/9vbANvbVdvbqtvb/9v/
ANv/Vdv/qtv///8AAP8AVf8Aqv8A//8kAP8kVf8kqv8k//9JAP9JVf9Jqv9J
//9tAP9tVf9tqv9t//+SAP+SVf+Sqv+S//+2AP+2Vf+2qv+2///bAP/bVf/b
qv/b////AP//Vf//qv///yH+CHh0aGVyaW9uACwAAAAAwACQAAAI/gABCBxI
sKDBgwgTKlzIsKHDhxAjSpxIsaLFixgzatzIsaPHjyBDihxJsqTJkyhTqlzJ
sqXLjyRiloj5sqbNlDJJlJg586bPnxx1liiSpKjRJD2BKl0KUWfRNm0kQW1T
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
GRAAOw==
====
}