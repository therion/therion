#TODO: + add more objects in DXF export
#      + add SVG import using tcldom library

set utldbg 0

proc xth_me_import_check_outline {coutline pt} {
  set px [expr double([lindex $pt 0])]
  set py [expr double([lindex $pt 1])]
  if {[llength $coutline] == 4} {
    if {[lindex $coutline 0] > $px} {lset coutline 0 $px}
    if {[lindex $coutline 1] > $py} {lset coutline 1 $py}
    if {[lindex $coutline 2] < $px} {lset coutline 2 $px}
    if {[lindex $coutline 3] < $py} {lset coutline 3 $py}
  } else {
    set coutline [list $px $py $px $py]
  }
  return $coutline
}

set xthmeimpcdlp {}

proc xth_me_import_check_distance {cdist p2} {
  global xthmeimpcdlp
  if {[llength $xthmeimpcdlp] == 0} {
    set xthmeimpcdlp $p2
    return $cdist
  }
  set p1 $xthmeimpcdlp
  set ndist [expr hypot(double([lindex $p1 0]) - double([lindex $p2 0]),double([lindex $p1 1]) - double([lindex $p2 1]))]
  if {($ndist > 0.0) && (($ndist < $cdist) || ($cdist < 0.0))} {
    return $ndist
  } else {
    return $cdist
  }
}


proc xth_me_import_dxf {fnm} {
  global xth utldbg
  set rv(olist) {}
  set fid [open $fnm r]
  set cmd 0
  set val 0
  set curcmd {}
  set cursec {}
  set x0 0.0
  set x1 0.0
  set lastcmdn 0
  set lastcmd {}
  set curcmdn 0
  set outcmdn -1
  set outline {}
  set distance -1.0
  set curdat {}
  set curseq {}
  set polylinevx 0
  set nofitpoint 1
  
  set hasx0 0
  set hasx1 0

  incr outcmdn
  lappend rv(olist) $outcmdn
  set rv(odata,$outcmdn,type) scrap
  set rv(odata,$outcmdn,opts) {}
  set rv(odata,$outcmdn,data) {}

if $utldbg {
  puts -nonewline "reading $fnm ..."
} else {
  xth_status_bar_push me
  xth_status_bar_status me "reading $fnm ..."
}

  set line_number 0
  while {![eof $fid]} {
    set cmd $val
    gets $fid val
    incr line_number
    if {($line_number % 1000) == 0} {
if $utldbg {
      puts -nonewline "."
} else {
      xth_status_bar_status me "reading $fnm ... (line $line_number)"
}
    }
    regsub {^\s+} $val {} val
    regsub {\s+$} $val {} val
    if {([string length $cursec] == 0) && [string equal $cmd 0] && [string equal -nocase $val SECTION]} {
      set cursec UNKNOWN
      set curcmd {}
    } elseif {[string equal $cmd 2] && [string equal $cursec UNKNOWN] && [regexp {^([A-Z]+)$} $val dum newsec]} {
      set cursec [string toupper $newsec]
    } elseif {([string length $cursec] > 0) && [string equal $cmd 0] && [regexp {^((3D)?[A-Z]+)$} $val dum newcmd]} {
      set lastcmd $curcmd
      incr curcmdn
      set curcmd {}
      set curdat {}
      set newcmd [string toupper $newcmd]
      if {[string equal $newcmd ENDSEC]} {
	set curcmd {}
	set cursec {}
      } else {
	switch -- $newcmd {
	  POLYLINE {
	    set curseq $newcmd
	    set polylinevx 0
	  }
	  SEQEND {
	    set curseq {}
	  }
	}
	set curcmd $newcmd
      }
    }
    
    # parse entities
    if {[string equal $cursec ENTITIES]} {
      set postprocess 0
      switch -- $cmd {
	10 {set x0 [expr double($val)]; set hasx0 1}
	11 {set x1 [expr double($val)]; set hasx1 1}
	20 {set y0 [expr double($val)]; if {$hasx0} {set postprocess 1}; set hasx0 0}
	21 {set y1 [expr double($val)]; if {$hasx1} {set postprocess 1}; set hasx1 0}
	40 {set n40 [expr double($val)]}
	50 {set n50 [expr double($val)]}
	51 {set n51 [expr double($val)]; set postprocess 1}
      }
      set cmdinserted 0
      if {$postprocess} {
	switch -- $curcmd {
	  CIRCLE {
	    if {[string equal $cmd 20]} {
	      set cpoint [list $x0 $y0]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]              
	      incr outcmdn
	      lappend rv(olist) $outcmdn
	      lappend curdat $cpoint
	      set rv(odata,$outcmdn,type) point
	      set rv(odata,$outcmdn,opts) "#CIRCLE"
	      set rv(odata,$outcmdn,data) $curdat
	    }
	  }
	  POINT {
	    if {[string equal $cmd 20]} {
	      set cpoint [list $x0 $y0]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]              
	      incr outcmdn
	      lappend rv(olist) $outcmdn
	      lappend curdat $cpoint
	      set rv(odata,$outcmdn,type) point
	      set rv(odata,$outcmdn,opts) "#POINT"
	      set rv(odata,$outcmdn,data) $curdat
	    }
	  }
	  ARC {
	    #TODO: Fit arc with bezier curve, not line
	    if {[string equal $cmd 51]} {
	      if {$n50 > $n51} {set n51 [expr $n51 + 360.0]}
	      set rstep [expr ($n51 - $n50) / (round(($n51 - $n50) / 10.0) + 1)]
	      set curdat {}
	      for {set cr $n50} {$cr < $n51} {set cr [expr $cr + $rstep]} {
		set crr [expr $cr / 180.0 * 3.14159265359]
		set cpoint [list [expr $x0 + $n40 * cos($crr)] [expr $y0 + $n40 * sin($crr)]]
		set distance [xth_me_import_check_distance $distance $cpoint]
		set outline [xth_me_import_check_outline $outline $cpoint]              
		lappend curdat $cpoint
	      }
	      set cr $n51
	      set crr [expr $cr / 180.0 * 3.14159265359]
	      set cpoint [list [expr $x0 + $n40 * cos($crr)] [expr $y0 + $n40 * sin($crr)]]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]              
	      lappend curdat $cpoint

	      incr outcmdn
	      lappend rv(olist) $outcmdn
	      set rv(odata,$outcmdn,type) line
	      set rv(odata,$outcmdn,opts) "#ARC"
	      set rv(odata,$outcmdn,data) $curdat
	    }
	  }
	  VERTEX {
	    if {[string equal $cmd 20]} {
	      set cpoint [list $x0 $y0]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]              
	      if {[string equal $curseq POLYLINE]} {
		if {$polylinevx == 0} {
		  incr outcmdn
		  lappend rv(olist) $outcmdn
		  lappend curdat $cpoint
		  set rv(odata,$outcmdn,type) line
		  set rv(odata,$outcmdn,opts) "#POLYLINE"
		  set rv(odata,$outcmdn,data) $curdat
		}
		if {![string equal [lindex $rv(odata,$outcmdn,data) end] $cpoint]} {
		  lappend rv(odata,$outcmdn,data) $cpoint
		}
		incr polylinevx
	      } else {
		incr outcmdn
		lappend rv(olist) $outcmdn
		lappend curdat $cpoint
		set rv(odata,$outcmdn,type) point
		set rv(odata,$outcmdn,opts) "#VERTEX"
		set rv(odata,$outcmdn,data) $curdat
	      }
	    }
	  }
	  LWPOLYLINE {
	    if {[string equal $cmd 20]} {
	      set cpoint [list $x0 $y0]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]
	      lappend curdat $cpoint
	    }
	    if {[llength $curdat] == 2} {
	      incr outcmdn
	      lappend rv(olist) $outcmdn
	      lappend curdat $cpoint
	      set rv(odata,$outcmdn,type) line
	      set rv(odata,$outcmdn,opts) "#LWPOLYLINE"
	      set rv(odata,$outcmdn,data) $curdat
	    } elseif {[llength $curdat] > 2} {
	      set rv(odata,$outcmdn,data) $curdat
	    }
	  }
	  SPLINE {
	    if {[string equal $cmd 20] || [string equal $cmd 21]} {
	      if {[llength $curdat] == 0} {
		set nofitpoint 1
	      }
	      set cpoint {}
	      if {[string equal $cmd 20]} {
		if {$nofitpoint} {
		  set cpoint [list $x0 $y0]
		}
	      } else {
		if {$nofitpoint} {
		  if {[llength $curdat] >= 2} {
		    incr outcmdn -1
		    set rv(olist) [lrange $rv(olist) 0 end-1]
		  }
		  set curdat {}
		  set nofitpoint 0
		}
		set cpoint [list $x1 $y1]
	      }
	      if {([llength $cpoint] > 0) && (![string equal [lindex $curdat end] $cpoint])} {
		set distance [xth_me_import_check_distance $distance $cpoint]
		set outline [xth_me_import_check_outline $outline $cpoint]
		lappend curdat $cpoint
		if {[llength $curdat] == 2} {
		  incr outcmdn
		  lappend rv(olist) $outcmdn
		  set rv(odata,$outcmdn,type) line
		  set rv(odata,$outcmdn,opts) "#SPLINE"
		  set rv(odata,$outcmdn,data) $curdat
		} elseif {[llength $curdat] > 2} {
		  if {$nofitpoint && ([llength $curdat] > 3)} {
		    if {([llength [lindex $curdat end]] == 2) && ([llength [lindex $curdat end-1]] == 2) && ([llength [lindex $curdat end-2]] == 2)} {
		      set curdat [lreplace $curdat end-2 end "[lindex $curdat end-2] [lindex $curdat end-1] [lindex $curdat end]"]
		    }
		  }
		  if {(!$nofitpoint) && ([llength $curdat] > 2)} {
		    set pp0 [lindex $curdat end-2]
		    set pp1 [lindex $curdat end-1]
		    set pp2 [lindex $curdat end]
		    set p0x [lindex $pp0 end-1]
		    set p0y [lindex $pp0 end]
		    set p1x [lindex $pp1 end-1]
		    set p1y [lindex $pp1 end]
		    set p2x [lindex $pp2 end-1]
		    set p2y [lindex $pp2 end]
		    set l1 [expr hypot($p1x - $p0x, $p1y - $p0y)]
		    set l2 [expr hypot($p2x - $p1x, $p2y - $p1y)]
		    if {($l1 > 0.0) && ($l2 > 0.0)} {
		      set n1x [expr ($p1x - $p0x) / $l1]
		      set n1y [expr ($p1y - $p0y) / $l1]
		      set n2x [expr ($p2x - $p1x) / $l2]
		      set n2y [expr ($p2y - $p1y) / $l2]
		      set nx [expr $l2 * $n1x + $l1 * $n2x]
		      set ny [expr $l2 * $n1y + $l1 * $n2y]
		      set nl [expr hypot($nx, $ny)]
		      if {$nl > 0.0} {
		        set nx [expr $nx / $nl]
		        set ny [expr $ny / $nl]
		        set c1x [expr $p1x - 0.33 * $l1 * $nx]
		        set c1y [expr $p1y - 0.33 * $l1 * $ny]
		        set c2x [expr $p1x + 0.33 * $l2 * $nx]
		        set c2y [expr $p1y + 0.33 * $l2 * $ny]
		        set p1path [lindex $curdat end-1]
		        if {[llength $p1path] == 6} {
		          set p1path [list [lindex $p1path 0] [lindex $p1path 1] $c1x $c1y $p1x $p1y]
		        } else {
		          set p1path [list [expr 0.5 * ($c1x + $p0x)] [expr 0.5 * ($c1y + $p0y)] $c1x $c1y $p1x $p1y]
		        }
		        set p2path [list $c2x $c2y [expr 0.5 * ($c2x + $p2x)] [expr 0.5 * ($c2y + $p2y)] $p2x $p2y]
		        set curdat [lreplace $curdat end-1 end $p1path $p2path]
		      }
		    }
		  }
		  set rv(odata,$outcmdn,data) $curdat
		}
	      }
	    }
	  }
	  LINE {
	    if {[string equal $cmd 20]} {
	      set cpoint [list $x0 $y0]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]
	      lappend curdat $cpoint
	    } elseif {[string equal $cmd 21]}  {
	      set cpoint [list $x1 $y1]
	      set distance [xth_me_import_check_distance $distance $cpoint]
	      set outline [xth_me_import_check_outline $outline $cpoint]
	      if {($outcmdn >= 0) && [string equal $lastcmd LINE] && [string equal [lindex $rv(odata,$outcmdn,data) end] [lindex $curdat 0]]} {
		if {![string equal [lindex $rv(odata,$outcmdn,data) end] $cpoint]} {
		  lappend rv(odata,$outcmdn,data) $cpoint
		}
	      } else {
		incr outcmdn
		lappend rv(olist) $outcmdn
		lappend curdat $cpoint
		set rv(odata,$outcmdn,type) line
		set rv(odata,$outcmdn,opts) "#LINE"
		set rv(odata,$outcmdn,data) $curdat
	      }
	    }
	  }
	}
      }
    }
  }
  close $fid

if $utldbg {
  puts "done."
} else {
  xth_status_bar_pop me
}


  incr outcmdn
  lappend rv(olist) $outcmdn
  set rv(odata,$outcmdn,type) endscrap
  set rv(odata,$outcmdn,opts) {}
  set rv(odata,$outcmdn,data) {}
  
  set rv(dist) $distance
  set rv(bbox) $outline
  return [array get rv]
}


proc xth_xml_get_flag {flags flag} {
  if {[regexp "(^|\\s)\\s*$flag\\s*\\=\\s*\\\"(\[^\\\"\]*)\\\"" $flags dum dum1 data]} {
    return $data
  } elseif {[regexp "(^|\\s)\\s*$flag\\s*\\=\\s*\\\'(\[^\\\'\]*)\\\'" $flags dum dum1 data]} {
    return $data
  } else {
    return {}
  }
}

proc xth_xml_clear_flag {flagsVar flag} {
  upvar 1 $flagsVar flags
  regsub "(^|\\s)\\s*$flag\\s*\\=\\s*\\\"(\[^\\\"\]*)\\\"" $flags {} flags
  regsub "(^|\\s)\\s*$flag\\s*\\=\\s*\\\'(\[^\\\'\]*)\\\'" $flags {} flags
}



proc xth_me_impsvg_trans {xVar yVar distVar outVar matrix} {
  upvar 1 $xVar x $yVar y $distVar distance $outVar outline
  set nx [expr double($x) * [lindex $matrix 0] + double($y) * [lindex $matrix 2] + [lindex $matrix 4]]
  set ny [expr double($x) * [lindex $matrix 1] + double($y) * [lindex $matrix 3] + [lindex $matrix 5]]
  set x $nx
  set y [expr -1.0 * $ny]
  set distance [xth_me_import_check_distance $distance [list $x $y]]
  set outline [xth_me_import_check_outline $outline [list $x $y]]
}


proc xth_me_impsvg_mm {A B} {
  set m0 [expr [lindex $A 0] * [lindex $B 0] + [lindex $A 2] * [lindex $B 1]]
  set m1 [expr [lindex $A 1] * [lindex $B 0] + [lindex $A 3] * [lindex $B 1]]
  set m2 [expr [lindex $A 0] * [lindex $B 2] + [lindex $A 2] * [lindex $B 3]]
  set m3 [expr [lindex $A 1] * [lindex $B 2] + [lindex $A 3] * [lindex $B 3]]
  set m4 [expr [lindex $A 0] * [lindex $B 4] + [lindex $A 2] * [lindex $B 5] + [lindex $A 4]]
  set m5 [expr [lindex $A 1] * [lindex $B 4] + [lindex $A 3] * [lindex $B 5] + [lindex $A 5]]
  return [list $m0 $m1 $m2 $m3 $m4 $m5]
}

proc xth_me_impsvg_push_transform {cmatrixVar lmatrixVar transformStr} {
  upvar 1 $cmatrixVar cmatrix $lmatrixVar lmatrix
  set nmatrix $cmatrix
  regsub {\)} $transformStr ")\n" transformStr
  set tl [split $transformStr "\n"]
  foreach t $tl {
    if {[regexp -nocase {(\w+)\(([E\+\-\d\.\,]+)\)} $t dum type param]} {
      set paraml [split $param ","]
      set xmatrix {1.0 0.0 0.0 1.0 0.0 0.0}
      switch -- $type {
	translate {
	  if {[llength $paraml] == 2} {
	    set xmatrix [list 1.0 0.0 0.0 1.0 [expr double([lindex $paraml 0])] [expr double([lindex $paraml 1])]]
	  }
	}
	scale {
	  if {[llength $paraml] == 2} {
	    set xmatrix [list [expr double([lindex $paraml 0])] 0.0 0.0 [expr double([lindex $paraml 1])] 0.0 0.0]
	  }
	}
	rotate {
	}
	skewX {
	}
	skewY {
	}
	matrix {
	  if {[llength $paraml] == 6} {
	    set xmatrix [list [expr double([lindex $paraml 0])] [expr double([lindex $paraml 1])] [expr double([lindex $paraml 2])] [expr double([lindex $paraml 3])] [expr double([lindex $paraml 4])] [expr double([lindex $paraml 5])]]
	  }
	}
      }
      set nmatrix [xth_me_impsvg_mm $nmatrix $xmatrix]
    }
  }
  lappend lmatrix $cmatrix
  set cmatrix $nmatrix
}


proc xth_me_impsvg_pop_transform {cmatrixVar lmatrixVar} {
  upvar 1 $cmatrixVar cmatrix $lmatrixVar lmatrix
  if {[llength $lmatrix] > 0} {
    set cmatrix [lindex $lmatrix end]
    set lmatrix [lrange $lmatrix 0 end-1]
  }
}

proc xth_me_import_svg {fnm} {

  global xth utldbg
  set rv(olist) {}
  set outcmdn -1
  set outline {}
  set distance -1.0
  set cmatrix {1.0 0.0 0.0 1.0 0.0 0.0}
  set lmatrix {}

  incr outcmdn
  lappend rv(olist) $outcmdn
  set rv(odata,$outcmdn,type) scrap
  set rv(odata,$outcmdn,opts) {}
  set rv(odata,$outcmdn,data) {}

if $utldbg {
  puts -nonewline "reading $fnm ..."
} else {
  xth_status_bar_push me
  xth_status_bar_status me "reading $fnm ..."
}

  set fid [open $fnm r]
  set dat [read $fid]
  close $fid

if $utldbg {
  puts -nonewline " done.\nparsing $fnm ..."
} else {
  xth_status_bar_status me "parsing $fnm ..."
}

  regsub -all {\n} $dat {} dat
  regsub -all {\s*(\<[^\>]*\>)([^\<]*)} $dat "\\1\n\\2\n" dat
  set data [split $dat "\n"]
  
if !$utldbg {
  xth_me_progbar_show [llength $data]
}
  set cdn 0
  
  set insvg 0
  set indef 0
  foreach itm $data {
    incr cdn
    if {$cdn % 10 == 0} {
      if $utldbg {
	puts -nonewline "."
      } else {
	xth_me_progbar_prog $cdn
      }
    }
    
    # set tag type
    set tagtype data
    if {[regexp {^\s*\<\s*(\/?)(\w+)([^\>]*)\>\s*$} $itm dum slash1 tagid flags]} {
      if {[string length $slash1] > 0} {
	set tagtype end
      } elseif {[regexp {\/\>\s*$} $itm]} {
	set tagtype single
      } else {
	set tagtype start
      }
    }
    
    switch $tagtype {
      start {
	# parse start tag
	switch -- $tagid {
	  svg {
	    if {!$insvg} {set insvg 1}
	  }
	  defs {
	    if {!$indef} {set indef 1}
	  }
	  g {
	    xth_me_impsvg_push_transform cmatrix lmatrix [xth_xml_get_flag $flags transform]
	  }
	}
      }
      end {
	# parse end tag
	switch -- $tagid {
	  svg {
	    if {$insvg} {set insvg 0}
	  }
	  defs {
	    if {$indef} {set indef 0}
	  }
	  g {
	    xth_me_impsvg_pop_transform cmatrix lmatrix
	  }
	}
      }
      single {
	# parse single tag
	switch -- $tagid {
	  path {
	    if {$insvg && (!$indef)} {
	      set pdata [xth_xml_get_flag $flags d]
	      xth_xml_clear_flag flags d
	      regsub -nocase -all {([mlcz])} $pdata "\n\\1" pdata
	      set pdatal [split $pdata "\n"]
	      set curdat {}
	      foreach lni $pdatal {
		if {[regexp -nocase {^\s*(M|L)\s+([e\+\-\d\.]+),([e\+\-\d\.]+)\s*$} $lni dum dum1 x y]} {
		  xth_me_impsvg_trans x y distance outline $cmatrix
		  lappend curdat [list $x $y]
		} elseif {[regexp -nocase {^\s*C\s+([e\+\-\d\.]+),([e\+\-\d\.]+)\s+([e\+\-\d\.]+),([e\+\-\d\.]+)\s+([e\+\-\d\.]+),([e\+\-\d\.]+)\s*$} $lni dum c1x c1y c2x c2y x y]} {
		  xth_me_impsvg_trans c1x c1y distance outline $cmatrix
		  xth_me_impsvg_trans c2x c2y distance outline $cmatrix
		  xth_me_impsvg_trans x y distance outline $cmatrix
		  lappend curdat [list $c1x $c1y $c2x $c2y $x $y]
		}
	      }
	      if {[llength $curdat] > 1} {
		incr outcmdn
		lappend rv(olist) $outcmdn
		set rv(odata,$outcmdn,type) line
		regsub -all {\s+} $flags { } flags
		set rv(odata,$outcmdn,opts) "# $flags #"
		set rv(odata,$outcmdn,data) $curdat
	      }
	    }
	  }
	}
      }
      data {
      }
    }
    
    
  }

if $utldbg {
  puts " done."
} else {
  xth_status_bar_pop me
}


  incr outcmdn
  lappend rv(olist) $outcmdn
  set rv(odata,$outcmdn,type) endscrap
  set rv(odata,$outcmdn,opts) {}
  set rv(odata,$outcmdn,data) {}
  
  set rv(dist) $distance
  set rv(bbox) $outline
  return [array get rv]
}


proc xth_me_import_file_trans {data sc dx dy} {
  set next 0
  set res {}
  foreach pt $data {
    if $next {append res "\n"} 
    switch [llength $pt] {
      2 {
	append res "[expr $sc * ([lindex $pt 0] - $dx)]\t[expr $sc * ([lindex $pt 1] - $dy)]"
      }
      6 {
	append res "[expr $sc * ([lindex $pt 0] - $dx)]\t[expr $sc * ([lindex $pt 1] - $dy)]"
	append res "\t[expr $sc * ([lindex $pt 2] - $dx)]\t[expr $sc * ([lindex $pt 3] - $dy)]"
	append res "\t[expr $sc * ([lindex $pt 4] - $dx)]\t[expr $sc * ([lindex $pt 5] - $dy)]"
      }
    }
    set next 1
  }
  return $res
}


proc xth_me_import_file {fnm fmt} {

  global xth xthmeimpcdlp
  set xthmeimpcdlp {}

  # if file name not specified, load it from dialog  
  if {[string length $fnm] == 0} {
    set fnm [tk_getOpenFile -filetypes {{{SVG files} {*.svg}} {{DXF files} {*.dxf}} {{All files} *}} \
      -parent $xth(gui,main) \
      -initialdir $xth(gui,initdir) -defaultextension ".svg"]  
  }
  if {[string length $fnm] == 0} {
    return
  } else {
    set xth(gui,initdir) [file dirname $fnm]
  }

  # detect file format from file name if format not specified
  if {[string length $fmt] == 0} {
    if {[regexp -nocase {\.dxf} $fnm]} {
      set fmt dxf
    }
    if {[regexp -nocase {\.svg} $fnm]} {
      set fmt svg
    }
  }
  
  # import objects
  switch $fmt {
    dxf {set objects [xth_me_import_dxf $fnm]}
    svg {set objects [xth_me_import_svg $fnm]}
    default {
      set objects [xth_me_import_svg $fnm]
    }
  }
  array set objs $objects
  
  # calculate scrap transformation matrix
  set dx [expr [lindex $objs(bbox) 2] - [lindex $objs(bbox) 0]]
  set dy [expr [lindex $objs(bbox) 3] - [lindex $objs(bbox) 1]]
  set dd $dx
  set md $objs(dist)
  if {$dy > $dd} {set dd $dy}

  set sc [expr $xth(import,size) / $dd]
  if {($md * $sc) < $xth(import,mind)} {set sc [expr $xth(import,mind) / $md]}
  
  if {[info exists xth(import,sscl)]} {
    set sc $xth(import,sscl)]
  }

  set dx [lindex $objs(bbox) 0]
  set dy [lindex $objs(bbox) 1]


  # calculate scrap options (-scale and -cs if applicable)
  set sc0 [expr $sc * ([lindex $objs(bbox) 0] - $dx)]
  set sc1 [expr $sc * ([lindex $objs(bbox) 1] - $dy)]
  set sc2 [expr $sc * ([lindex $objs(bbox) 2] - $dx)]
  set sc3 [expr $sc * ([lindex $objs(bbox) 3] - $dy)]
  set scrapopts {}
  switch $fmt {
    dxf {
      set sc4 [expr [lindex $objs(bbox) 0] * $xth(import,dxf,scale)]
      set sc5 [expr [lindex $objs(bbox) 1] * $xth(import,dxf,scale)]
      set sc6 [expr [lindex $objs(bbox) 2] * $xth(import,dxf,scale)]
      set sc7 [expr [lindex $objs(bbox) 3] * $xth(import,dxf,scale)]
      if {[string length $xth(import,dxf,cs)] > 0} {
	append scrapopts "-cs $xth(import,dxf,cs) "
      }
    }
    svg {
      set sc4 [expr [lindex $objs(bbox) 0] * $xth(import,svg,scale)]
      set sc5 [expr [lindex $objs(bbox) 1] * $xth(import,svg,scale)]
      set sc6 [expr [lindex $objs(bbox) 2] * $xth(import,svg,scale)]
      set sc7 [expr [lindex $objs(bbox) 3] * $xth(import,svg,scale)]
      if {[string length $xth(import,svg,cs)] > 0} {
	append scrapopts "-cs $xth(import,svg,cs) "
      }
    }
    default {
      set sc4 [expr [lindex $objs(bbox) 0] * $xth(import,default,scale)]
      set sc5 [expr [lindex $objs(bbox) 1] * $xth(import,default,scale)]
      set sc6 [expr [lindex $objs(bbox) 2] * $xth(import,default,scale)]
      set sc7 [expr [lindex $objs(bbox) 3] * $xth(import,default,scale)]
      if {[string length $xth(import,default,cs)] > 0} {
	append scrapopts "-cs $xth(import,default,cs) "
      }
    }
  }
  append scrapopts "-scale \[$sc0 $sc1 $sc2 $sc3 $sc4 $sc5 $sc6 $sc7\] "
  
  
  # create objects
  global utldbg
if $utldbg {

  foreach obj $objs(olist) {
    switch $objs(odata,$obj,type) {
      scrap {
	puts "\nscrap import $scrapopts $objs(odata,$obj,opts)"
      }
      point {
	puts "\npoint [xth_me_import_file_trans $objs(odata,$obj,data) $sc $dx $dy] u:point $objs(odata,$obj,opts)"
      }
      line {
	puts "\nline u:line $objs(odata,$obj,opts)\n[xth_me_import_file_trans $objs(odata,$obj,data) $sc $dx $dy]\nendline"
      }
      endscrap {
	puts "\nendscrap"
      }
      default {
	puts ""
      }
    }
  }


} else {

  xth_status_bar_push me
  xth_status_bar_status me [mc "Importing objects ..."]
  xth_me_progbar_show [llength $objs(olist)]

  set scount 0
  set ocount 0
  set xth(me,unredook) 0
  foreach obj $objs(olist) {
    incr ocount
    xth_me_progbar_prog $ocount
    
    set pos [expr [llength $xth(me,cmds,xlist)] - 1]
    switch $objs(odata,$obj,type) {
      scrap {
	xth_me_cmds_create_scrap $pos 0 import$scount "$scrapopts $objs(odata,$obj,opts)"
	incr scount
      }
      point {
	set coords [xth_me_import_file_trans $objs(odata,$obj,data) $sc $dx $dy]
	xth_me_cmds_create_point $pos 0 [lindex $coords 0] [lindex $coords 1] "u:point" $objs(odata,$obj,opts)
      }
      line {
	set coords [xth_me_import_file_trans $objs(odata,$obj,data) $sc $dx $dy]
	xth_me_cmds_create_line $pos 0 "u:line" $objs(odata,$obj,opts) [split $coords "\n"]
      }
      endscrap {
	xth_me_cmds_create_endscrap $pos 0 {}
      }
    }
  }

  xth_me_cmds_select [lindex $xth(me,cmds,xlist) $pos]
  xth_me_area_auto_adjust
  xth_me_progbar_hide
  xth_status_bar_pop me
  set xth(me,unredook) 1

}  
}


if $utldbg {
  set xth(gui,main) .
  #xth_me_import_svg test.svg
  #xth_me_import_dxf map1.dxf
  xth_me_import_file map1.dxf {}
  exit
}
