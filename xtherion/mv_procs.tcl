##
## mv_procs.tcl --
##
##     Model viewer procedures.
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

if {[string length $xthmvw] > 0} {

proc xth_mv_configure_camera {w h} {
  global xthmvw xthmvv
  if {[string length $w] > 0} {
    glViewport 0 0 $w $h 
  } else {
    set w [winfo width $xthmvw]
    set h [winfo height $xthmvw]
  }

  set diam $xthmvv(model,diam)
  set dist $xthmvv(cam,dist)

  set prof [expr double($xthmvv(cam,profile)) / 180.0 * 3.14159265359]
  set face [expr double($xthmvv(cam,facing)) / 180.0 * 3.14159265359]

  set hdist [expr cos($prof)]
  set fwdx [expr $hdist * sin($face)]
  set fwdy [expr $hdist * cos($face)]
  set fwdz [expr - sin($prof)]
  
  set cx $xthmvv(cam,cx)
  set cy $xthmvv(cam,cy)
  set cz $xthmvv(cam,cz)

  set fromx [expr $cx - $fwdx * $dist]
  set fromy [expr $cy - $fwdy * $dist]
  set fromz [expr $cz - $fwdz * $dist]
  
#  set hdist [expr cos($prof) * $dist]
#  set fromx [expr $cx - $hdist * sin($face)]
#  set fromy [expr $cy - $hdist * cos($face)]
#  set fromz [expr $cz + sin($prof) * $dist]

  set hdist [expr sin($prof)]
  set upx [expr $hdist * sin($face)]
  set upy [expr $hdist * cos($face)]
  set upz [expr cos($prof)]
  
  set rightx [expr cos($face)]
  set righty [expr - sin($face)]
  set rightz 0.0

  set xthmvv(cam,fwdx) $fwdx
  set xthmvv(cam,fwdy) $fwdy
  set xthmvv(cam,fwdz) $fwdz
  
  set xthmvv(cam,upx) $upx
  set xthmvv(cam,upy) $upy
  set xthmvv(cam,upz) $upz

  set xthmvv(cam,rightx) $rightx
  set xthmvv(cam,righty) $righty
  set xthmvv(cam,rightz) $rightz

  set pnear [expr sqrt($fromx * $fromx + $fromy * $fromy + $fromz * $fromz) - 1.1 * $diam]
  if {$pnear < 1.0} {
    set pnear 1.0
  }
  set pfar [expr $pnear + 2.2 * $diam]

  glMatrixMode $GL::GL_PROJECTION
  glLoadIdentity
  gluPerspective 40.0 [expr double($w) / double($h)] $pnear $pfar

  glMatrixMode $GL::GL_MODELVIEW
  glLoadIdentity
  gluLookAt $fromx $fromy $fromz $cx $cy $cz $upx $upy $upz
  glLightfv $GL::GL_LIGHT0 $GL::GL_POSITION [list $fromx $fromy $fromz 1.0]
  
}

proc xth_mv_update {} {
  global xthmvv
  xth_mv_configure_camera {} {}
  xth_mv_draw
  set xthmvv(cam,profile) [expr double($xthmvv(cam,profile))]
  set xthmvv(cam,facing) [expr double($xthmvv(cam,facing))]
}

proc xth_mv_draw {} {
  global xthmvw xthmvv
  # nakreslime si model
  glClear [expr $GL::GL_COLOR_BUFFER_BIT | $GL::GL_DEPTH_BUFFER_BIT]
  glCallList $xthmvv(list,model)
  glCallList $xthmvv(list,bbox)
  glFlush
  $xthmvw swap
}

proc xth_mv_gl_wireframe {} {
  glShadeModel $GL::GL_FLAT
  glPolygonMode $GL::GL_FRONT_AND_BACK $GL::GL_LINE
  glDisable $GL::GL_LIGHTING
}

proc xth_mv_gl_surface {} {
  glShadeModel $GL::GL_SMOOTH
  glPolygonMode $GL::GL_FRONT_AND_BACK $GL::GL_FILL
  glEnable $GL::GL_LIGHTING
}

proc xth_mv_init {} {

  global xthmvw xthmvv xth
  glClearColor 0.0 0.0 0.0 0.0
  glEnable $GL::GL_DEPTH_TEST


  glEnable $GL::GL_LIGHT0
  glLightfv $GL::GL_LIGHT0 $GL::GL_AMBIENT {0.4 0.4 0.4 1.0}
  glLightfv $GL::GL_LIGHT0 $GL::GL_DIFFUSE {1.0 1.0 1.0 1.0}

  # vytvorime default model
  set xthmvv(list,model) 1
  set xthmvv(list,bbox) 2
  glDeleteLists $xthmvv(list,model) 1
  glNewList $xthmvv(list,model) $GL::GL_COMPILE
  xth_mv_gl_surface  
  glColor3f 1.0 1.0 1.0
  set xthmvv(model,maxx) 1.0
  set xthmvv(model,maxy) 1.0
  set xthmvv(model,maxz) 1.0
  set xthmvv(model,minx) -1.0
  set xthmvv(model,miny) -1.0
  set xthmvv(model,minz) -1.0
#  if {[string equal $xth(gui,platform) windows]} {
#    set q [gluNewQuadric]
#    gluSphere $q 1.0 20 16
#    gluDeleteQuadric $q
#  }
  glEndList
}


proc xth_mv_reload_file {} {
  global xth xthmvv
  if {[catch {source $xth(mv,ffull)} opnerr]} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message $opnerr\
        -font $xth(gui,lfont)
      return 0
  }
  xth_mv_init_model 0
}


proc xth_mv_open_file {fpath} {

  global xthmvv xth

  if {[string length $fpath] == 0} {
    set fpath [tk_getOpenFile -filetypes $xth(app,mv,filetypes) \
      -parent $xth(gui,main) -initialdir $xth(gui,initdir)]
  }
  
  if {[string length $fpath] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fpath]
  }

  xth_status_bar_push mv
  xth_status_bar_status mv "Opening $fpath ..."
  
  if {[catch {source $fpath} opnerr]} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message $opnerr\
        -font $xth(gui,lfont)
      xth_status_bar_pop mv
      return 0
  }

  set xth(mv,fopen) 1
  set xth(mv,fname) [file tail $fpath]
  set xth(mv,fpath) [file dirname $fpath]
  set xth(mv,ffull) $fpath
  
  xth_mv_init_model 1
  xth_app_title mv
  xth_status_bar_pop mv
  
  update idletasks
  return 1

}


proc xth_mv_init_camera {} {
  global xthmvv

  set xthmvv(cam,profile) 20.0
  set xthmvv(cam,facing) 160.0
  set mx $xthmvv(model,maxx)
  set my $xthmvv(model,maxy)
  set mz $xthmvv(model,maxz)
  set nx $xthmvv(model,minx)  
  set ny $xthmvv(model,miny)  
  set nz $xthmvv(model,minz)
  set xthmvv(cam,cx) [expr ($mx + $nx) / 2.0]
  set xthmvv(cam,cy) [expr ($my + $ny) / 2.0]
  set xthmvv(cam,cz) [expr ($mz + $nz) / 2.0]

  set xthmvv(cam,autorotate) 0
  set xthmvv(autorotate,dir) 1
  set xthmvv(autorotate,counter) 0
  if {$xthmvv(model,diam) > 0} {
    set xthmvv(cam,dist) [expr 4.0 * $xthmvv(model,diam)]
  } else {
    set xthmvv(cam,dist) 1.0
  }

}


proc xth_mv_init_model {initcam} {
  global xthmvv
  set mx $xthmvv(model,maxx)
  set my $xthmvv(model,maxy)
  set mz $xthmvv(model,maxz)
  set nx $xthmvv(model,minx)  
  set ny $xthmvv(model,miny)  
  set nz $xthmvv(model,minz)
  set dx [expr ($mx - $nx) / 2.0]
  set dy [expr ($my - $ny) / 2.0]
  set dz [expr ($mz - $nz) / 2.0]
  set diam [expr sqrt($dx * $dx + $dy * $dy + $dz * $dz)]
  set xthmvv(model,diam) $diam
  glDeleteLists $xthmvv(list,bbox) 1
  glNewList $xthmvv(list,bbox) $GL::GL_COMPILE
  xth_mv_gl_wireframe
  glBegin $GL::GL_LINE_STRIP
  glColor3f 1.0 0.0 0.0
  glVertex3f [expr $mx] [expr $my] [expr $mz]
  glVertex3f [expr $nx] [expr $my] [expr $mz]
  glVertex3f [expr $nx] [expr $ny] [expr $mz]
  glVertex3f [expr $mx] [expr $ny] [expr $mz]
  glVertex3f [expr $mx] [expr $my] [expr $mz]
  glVertex3f [expr $mx] [expr $my] [expr $nz]
  glVertex3f [expr $nx] [expr $my] [expr $nz]
  glVertex3f [expr $nx] [expr $my] [expr $mz]
  glVertex3f [expr $nx] [expr $my] [expr $nz]
  glVertex3f [expr $nx] [expr $ny] [expr $nz]
  glVertex3f [expr $nx] [expr $ny] [expr $mz]
  glVertex3f [expr $nx] [expr $ny] [expr $nz]
  glVertex3f [expr $mx] [expr $ny] [expr $nz]
  glVertex3f [expr $mx] [expr $ny] [expr $mz]
  glVertex3f [expr $mx] [expr $ny] [expr $nz]
  glVertex3f [expr $mx] [expr $my] [expr $nz]
  glVertex3f [expr $mx] [expr $my] [expr $mz]
  glEnd
  glEndList
  if {$initcam} {
    xth_mv_init_camera
  }
  xth_mv_update
}

proc xth_mv_change_profile {v} {
  xth_mv_update
}


proc xth_mv_start_drag {x y} {
  global xthmvv
  set xthmvv(drag,x) $x
  set xthmvv(drag,y) $y
  set xthmvv(drag,facing) $xthmvv(cam,facing)
  set xthmvv(drag,dist) $xthmvv(cam,dist)
}


proc xth_mv_start_shift {x y} {
  global xthmvv
  set xthmvv(shift,x) $x
  set xthmvv(shift,y) $y
  set xthmvv(shift,cx) $xthmvv(cam,cx)
  set xthmvv(shift,cy) $xthmvv(cam,cy)
  set xthmvv(shift,cz) $xthmvv(cam,cz)
}

proc xth_mv_start_walk {x y} {
  global xthmvv
  set xthmvv(walk,x) $x
  set xthmvv(walk,y) $y
  set xthmvv(walk,cx) $xthmvv(cam,cx)
  set xthmvv(walk,cy) $xthmvv(cam,cy)
  set xthmvv(walk,cz) $xthmvv(cam,cz)
}

proc xth_mv_continue_drag {x y} {
  global xthmvv
  set dx [expr $x - $xthmvv(drag,x)]
  set dy [expr $y - $xthmvv(drag,y)]
  if {!$xthmvv(cam,autorotate)} {
    set xthmvv(cam,facing) [expr double(int($dx + $xthmvv(drag,facing)) % 360)]
  }  
  set xthmvv(cam,dist) [expr $xthmvv(drag,dist) * pow(2.0, $dy/100.0)]
  xth_mv_update
}


proc xth_mv_check_center {} {
  global xthmvv
  set cx $xthmvv(cam,cx)
  set cy $xthmvv(cam,cy)
  set cz $xthmvv(cam,cz)
  set mx $xthmvv(model,maxx)
  set my $xthmvv(model,maxy)
  set mz $xthmvv(model,maxz)
  set nx $xthmvv(model,minx)  
  set ny $xthmvv(model,miny)  
  set nz $xthmvv(model,minz)
  if {$cx < $nx} {
    set xthmvv(cam,cx) $nx
  } elseif {$cx > $mx} {
    set xthmvv(cam,cx) $mx
  } 
  if {$cy < $ny} {
    set xthmvv(cam,cy) $ny
  } elseif {$cy > $my} {
    set xthmvv(cam,cy) $my
  } 
  if {$cz < $nz} {
    set xthmvv(cam,cz) $nz
  } elseif {$cz > $mz} {
    set xthmvv(cam,cz) $mz
  } 
}


proc xth_mv_continue_shift {x y} {
  global xthmvv
  set dx [expr double($x - $xthmvv(shift,x)) / 500.0 * $xthmvv(cam,dist)]
  set dy [expr double($y - $xthmvv(shift,y)) / 500.0 * $xthmvv(cam,dist)]
  if {!$xthmvv(cam,autorotate)} {
    set xthmvv(cam,cx) [expr $xthmvv(shift,cx) - $dx * $xthmvv(cam,rightx) + $dy * $xthmvv(cam,upx)]
    set xthmvv(cam,cy) [expr $xthmvv(shift,cy) - $dx * $xthmvv(cam,righty) + $dy * $xthmvv(cam,upy)]
    set xthmvv(cam,cz) [expr $xthmvv(shift,cz) - $dx * $xthmvv(cam,rightz) + $dy * $xthmvv(cam,upz)]
    xth_mv_check_center    
    xth_mv_update
  }  
}

proc xth_mv_continue_walk {x y} {
  global xthmvv
  set dy [expr double($y - $xthmvv(walk,y)) / 200.0 * $xthmvv(cam,dist)]
  if {!$xthmvv(cam,autorotate)} {
    set xthmvv(cam,cx) [expr $xthmvv(walk,cx) - $dy * $xthmvv(cam,fwdx)]
    set xthmvv(cam,cy) [expr $xthmvv(walk,cy) - $dy * $xthmvv(cam,fwdy)]
    set xthmvv(cam,cz) [expr $xthmvv(walk,cz) - $dy * $xthmvv(cam,fwdz)]
    xth_mv_check_center    
    xth_mv_update
  }  
}

proc xth_mv_camera_autorotate {} {
  global xthmvv
  if {$xthmvv(cam,autorotate)} {
    set xthmvv(cam,facing) [expr double(int($xthmvv(cam,facing) + $xthmvv(autorotate,dir)) % 360)]
    xth_mv_update
    if {$xthmvv(autorotate,counter) == 0} {
      set xthmvv(autorotate,watch) [clock clicks -milliseconds]
    }
    incr xthmvv(autorotate,counter)
    if {$xthmvv(autorotate,counter) == 50} {
      catch {
        $xthmvv(ctrlframe).fa configure -text "auto rotate ([format %.1f [expr 50000.0 / double([clock clicks -milliseconds] - $xthmvv(autorotate,watch))]] fps)"                 
      }
      set xthmvv(autorotate,counter) 0
      after 10 xth_mv_camera_autorotate
    } else {
      after idle xth_mv_camera_autorotate
    }
  } else {
    $xthmvv(ctrlframe).fa configure -text "auto rotate" 
    set xthmvv(autorotate,fps) {}
    set xthmvv(autorotate,counter) 0
    set xthmvv(cam,autorotate) 0
  }
}

bind $xthmvw <Configure> {
  xth_mv_configure_camera %w %h
  xth_mv_draw
}
bind $xthmvw <Expose> xth_mv_draw
bind $xthmvw <1> {xth_mv_start_drag %x %y}
bind $xthmvw <B1-Motion> {xth_mv_continue_drag %x %y}
bind $xthmvw <B1-ButtonRelease> {xth_mv_continue_drag %x %y}

bind $xthmvw <3> {xth_mv_start_shift %x %y}
bind $xthmvw <B3-Motion> {xth_mv_continue_shift %x %y}
bind $xthmvw <B3-ButtonRelease> {xth_mv_continue_shift %x %y}

bind $xthmvw <2> {xth_mv_start_walk %x %y}
bind $xthmvw <B2-Motion> {xth_mv_continue_walk %x %y}
bind $xthmvw <B2-ButtonRelease> {xth_mv_continue_walk %x %y}

$f.pscale configure -command xth_mv_change_profile

xth_mv_init
xth_mv_init_model 1

# IF, CI SA MV KONA
} 
