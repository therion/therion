set xth(calib_dir) ""
set xth(calib_res) 96
set xth(calib_fmt) png
set xth(calib_msg) "Wait please"

proc xth_calibrate_bitmap {} {

  global xth tcl_platform
  if {[string length $xth(calib_dir)] == 0} {
    set xth(calib_dir) $xth(gui,initdir)
  }

  # identify image
  set ipath identify
  set cpath convert
  if {[string equal $tcl_platform(platform) windows]} {
    package require registry
    set win32registry {HKEY_LOCAL_MACHINE\SOFTWARE\Therion}
    if {[catch {
      set ipath "[file join [registry get $win32registry InstallDir] bin identify.exe]"
      set cpath "[file join [registry get $win32registry InstallDir] bin convert.exe]"
    }]} {
      set win32registry {HKEY_CURRENT_USER\SOFTWARE\Therion}
      catch {
	set ipath "[file join [registry get $win32registry InstallDir] bin identify.exe]"
	set cpath "[file join [registry get $win32registry InstallDir] bin convert.exe]"
      }
    }
  }


  # load bitmap and its size using identify
  set fname [tk_getOpenFile -filetypes {{{Images} {.png .jpg .jpeg .gif .pdf .PNG .JPG .JPEG .GIF .PDF}} {{All files} {*}}} \
      -parent $xth(gui,main) \
      -initialdir $xth(calib_dir)]
  if {[string length $fname] == 0} return
  set xth(calib_dir) [file dirname $fname]

  if {[string equal -nocase .pdf [file extension $fname]]} {
     # create bitmap
     set pdfname $fname
     set dp $xth(gui,main).resoldlg
     Dialog $dp -title "Image resolution" -parent $xth(gui,main) -default 0
     set df [$dp getframe]
     Label $df.l -text "Resolution (dpi)"
     Entry $df.e -textvariable xth(calib_res)
     radiobutton $df.p -text "PNG" -variable xth(calib_fmt) -value png
     radiobutton $df.j -text "JPEG" -variable xth(calib_fmt) -value jpg
     pack $df.l $df.e $df.p $df.j -side left -fill both -expand yes -padx 4 -pady 4
     $dp add -name ok 
     $dp draw
     destroy $dp
     if {[catch {set xth(calib_res) [expr int($xth(calib_res))]}]} {
       set xth(calib_res) 96
     }
     xth_calibrate_prog_show "Rastering PDF image..."
     set fname "[file rootname $fname].$xth(calib_fmt)"
     catch {exec $cpath -density $xth(calib_res)x$xth(calib_res) $pdfname -quality 95 $fname}
     xth_calibrate_prog_hide
  } else {
    set pdfname "[file rootname $fname].pdf"
    if {![file exists $pdfname]} {
      set pdfname [tk_getOpenFile -filetypes {{{PDF files} {.pdf}} {{All files} {*}}} \
	  -parent $xth(gui,main) \
	  -initialdir $xth(calib_dir)]
    }
    if {[string length $pdfname] == 0} return
  }


  xth_calibrate_prog_show "Reading bitmap..."
  catch {exec $ipath -format {%w\n%h\n} $fname > $fname.tmp}
  xth_calibrate_prog_hide
  set fid [open $fname.tmp r]
  gets $fid imgw
  gets $fid imgh
  close $fid
  file delete -force -- $fname.tmp

  # load corresponding pdf file
  xth_calibrate_prog_show "Reading calibration data..."
  catch {
  set fid [open $pdfname r]
  fconfigure $fid -eofchar {}
  set goon 1
  while {(![eof $fid])} {
    gets $fid cln
    if {[regexp {\/thCalibrate0 \(} $cln]} {
      break
    }
  }
  close $fid

  set fid [open $pdfname r]
  set cln [read $fid]
  close $fid

  xth_calibrate_prog_hide

  # find calibration points
  set HD -1.0
  xth_calibrate_prog_show "Writing map file..."
  for {set i 0} {$i < 9} {incr i} {
    set rx "\\/thCalibrate$i"
    set X($i) 0.0
    set Y($i) 0.0
    set L($i) 0.0
    set F($i) 0.0
    append rx {\s+\([^)]*X=([\+\-\.\d]+)[^)]*Y=([\+\-\.\d]+)[^)]*L=([\+\-\.\d]+)[^)]*F=([\+\-\.\d]+)}
    regexp $rx $cln dum X($i) Y($i) L($i) F($i)
  }
  regexp {\/thCalibrate\s+\([^)]*HS=([\+\-\.\d]+)[^)]*VS=([\+\-\.\d]+)[^)]*HD=([\+\-\.\d]+)} $cln dum HS VS HD

  set geo_err 1
  set fid [open err.log w]
  for {set i 0} {$i < 9} {incr i} {
    set geo_err [expr $geo_err && ($L($i) == 0.0) && ($F($i) == 0.0)]
  }
  set geo_err [expr $geo_err || ($HD <= 0.0)]
  if {$geo_err} {
    xth_calibrate_show_err "Georeferencing of map is not correct!"
    return
  }

  
  # write map file
  set fid [open "[file rootname $fname].map" w]
  puts $fid {OziExplorer Map Data File Version 2.2}
  puts $fid [file tail $fname]
  puts $fid $fname
  puts $fid {1 ,Map Code,}
  puts $fid {WGS 84,,   0.0000,   0.0000,WGS 84}
  puts $fid {Reserved 1}
  puts $fid {Reserved 2}
  puts $fid {Magnetic Variation,,,E}
  puts $fid {Map Projection,Mercator,PolyCal,No,AutoCalOnly,No,BSBUseWPX,No}
  for {set i 0} {$i < 9} {incr i} {
    set imgx [expr $imgw - int(double($imgw) * ($HS - $X($i)) / $HS)]
    set imgy [expr int(double($imgh) * ($VS - $Y($i)) / $VS)]
    puts $fid "Point0[expr $i + 1],xy,$imgx,$imgy,in, deg,[ll2ozi $F($i) $L($i)], grid,   ,           ,           ,N"
    set mmx($i) $imgx
    set mmy($i) $imgy
  }
  # Point01,xy,    0, 0,in, deg, 48, 12.39601,N, 17, 1.25473,E, grid,   ,           ,           ,N
  for {set i 10} {$i <= 30} {incr i} {
    puts $fid "Point$i,xy,     ,     ,in, deg,    ,        ,N,    ,        ,W, grid,   ,           ,           ,N"
  }
  puts $fid {Projection Setup,,,,,,,,,,}
  puts $fid {Map Feature = MF ; Map Comment = MC     These follow if they exist}
  puts $fid {Track File = TF      These follow if they exist}
  puts $fid {Moving Map Parameters = MM?    These follow if they exist}
  puts $fid {MM0,Yes}
  puts $fid {MMPNUM,4}
  puts $fid "MMPXY,1,$mmx(5),$mmy(5)"
  puts $fid "MMPXY,2,$mmx(7),$mmy(7)"
  puts $fid "MMPXY,3,$mmx(2),$mmy(2)"
  puts $fid "MMPXY,4,$mmx(0),$mmy(0)"
  puts $fid "MMPLL,1, $L(5), $F(5)"
  puts $fid "MMPLL,2, $L(7), $F(7)"
  puts $fid "MMPLL,3, $L(2), $F(2)"
  puts $fid "MMPLL,4, $L(0), $F(0)"
  puts $fid "MM1B,[expr $HD / abs(double($mmx(7)) - double($mmx(5)))]"
  puts $fid {MOP,Map Open Position,0,0}
  puts $fid "IWH,Map Image Width/Height,$imgw,$imgh"
  close $fid
  }
  xth_calibrate_prog_hide
  
}


proc xth_calibrate_show_err {msg} {
    global xth
    set dp $xth(gui,main).messagedlg
    MessageDlg $dp -parent $xth(gui,main) -message $msg -type ok -icon error
}


proc xth_calibrate_prog_show {msg} {
    global xth
    set dp $xth(gui,main).progdlg
    set xth(calib_msg) $msg
    ProgressDlg $dp -parent $xth(gui,main) -title "Wait..." \
	-type         infinite \
	-width        30 \
	-textvariable xth(calib_msg)
    update
    after 200
}

proc xth_calibrate_prog_hide {} {
    global xth
    set dp $xth(gui,main).progdlg
    destroy $dp
}

proc ll2ozi {lat lon} {
  set latD [expr int($lat)]
  set latM [expr 60.0 * ($lat - double($latD))]
  if {$lat >= 0.0} {set latH N} else {set latH S}
  set lonD [expr int($lon)]
  set lonM [expr 60.0 * ($lon - double($lonD))]
  if {$lon >= 0.0} {set lonH E} else {set lonE W}
  return "$latD,$latM,$latH,$lonD,$lonM,$lonH"
}


if 0 {
  package require BWidget
  set xth(gui,main) .x
  toplevel .x
  set xth(gui,initdir) .
  xth_calibrate_bitmap
  exit
}
