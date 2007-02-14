#! /usr/bin/tclsh
# Test all therion features and generate features HTML pages.
#
# Usage: tests.tcl
#        tests.tcl clean

set flog [open tests.log w]

proc log_msg {msg} {
  global flog
  puts -nonewline $msg
  puts -nonewline $flog $msg
}

set dirnum 0
set dirlist {}
set filelist {}

proc scan_files {dir} {
  global filelist dirnum farray cleanlist dirlist
  set subdirs {}
  catch {
    set subdirs [glob -directory $dir -types d *]
  }
  foreach sd $subdirs {
    scan_files $sd
    lappend dirlist $sd
  }
  incr dirnum
  log_msg "Scanning directory: $dir\n"
  set flist {}
  catch {
    set flist [glob -directory $dir *.th *.th2 *.tc thconfig*]
  }
  foreach fn $flist {
    set fn [file tail $fn]
    lappend filelist [list $fn $dir [llength $filelist] $dirnum]
    log_msg "  $fn\n"
  }
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    set farray($fnum,FILE) {}
    set farray($fnum,CLEAN) {}
    set farray($fnum,PROCESS) {}
    set fid [open [file join [lindex $fr 1] [lindex $fr 0]] r]
    while {![eof $fid]} {
      gets $fid ln
      if {[regexp -nocase {^\s*\#\!(FILE|PROCESS|CLEAN)(.*)} $ln dum cmnd rest]} {
	regsub {^\s+} $rest {} rest
	regsub {\s+$} $rest {} rest
	switch [string toupper $cmnd] {
	  FILE {
	    if {[string length $rest] > 0} {
	      set farray($fnum,FILE) $rest
	    }
	  }
	  PROCESS {
	      lappend farray($fnum,PROCESS) $rest
	  }
	  CLEAN {
	    if {[llength $rest] > 0} {
	      foreach fn $rest {
		lappend farray($fnum,CLEAN) $fn
	      }
	    }
	  }
	}
      }
    }
    close $fid
  }
}

set thcmd [file normalize [file join [pwd] "../therion"]]

set cleanlist {.docs}
set processlist {}
proc scan_lists {} {
  global filelist farray cleanlist processlist dirlist thcmd
  foreach dn $dirlist {
    set clist {}
    catch {
      set clist [glob -directory $dn *.log *~ .xth_*]
    }
    foreach cf $clist {
      lappend cleanlist $cf
    }
  }
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    set fsort([format "%06d%20s" [lindex $fr 3] $farray($fnum,FILE)]) $fnum
  }
  set newfilelist {}
  set xlist [lsort [array names fsort]]
  foreach x $xlist {
    lappend newfilelist [lindex $filelist $fsort($x)]
  }
  set filelist $newfilelist
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    foreach item $farray($fnum,CLEAN) {
      lappend cleanlist [file join [lindex $fr 1] $item]
    }
    foreach item $farray($fnum,PROCESS) {
      set nitem $item
      if {[string length $nitem] == 0} {
	set nitem "$thcmd [lindex $fr 0]"
      }
      lappend processlist [list $nitem [lindex $fr 1]]
    }
  }
}


proc clean_files {} {
  global cleanlist
  log_msg "\nDeleting files:\n"
  foreach fn $cleanlist {
    log_msg "$fn "
    if {[catch {
      file delete -force -- $fn
    }]} {
      log_msg "\[ERROR\]\n"   
    } else {
      log_msg "\[OK\]\n"   
    }
  }
}


proc process_files {} {
  global processlist
  set cdir [pwd]
  log_msg "\nProcessing files:\n"
  foreach ps $processlist {
    set p [lindex $ps 0]
    set d [lindex $ps 1]
    cd $d
    log_msg "$d: $p\n"
    catch {
      eval "exec $p >&@ stdout"
#      eval exec "cmd /c $p"
    }
    log_msg "\n\n"
    cd $cdir
  }
}


proc create_docs {} {
  global filelist tcl_platform
  set cdir {}
  set chid 0
  set imid 0
  set chapters {}
  set imagelist {}
  array set data {}
  # scan chapters and subchapters
  log_msg "\nScanning texts:\n"
  set inparagraph 0
  set incode 0
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    if {[string compare [lindex $fr 1] $cdir] != 0} {
      if {$inparagraph} {
	append data($chid,TEXT) "</p>\n"
	set inparagraph 0
      }
      incr chid
      lappend chapters [list [format ":%06d" $chid] $chid]
      set data($chid,TITLE) [file tail [lindex $fr 1]]
      set data($chid,TEXT) {}
      set csub 0
      set data($chid,SUBS) {}
      set cdir [lindex $fr 1]
    }
    log_msg "[file join [lindex $fr 1] [lindex $fr 0]]\n"
    set fid [open [file join [lindex $fr 1] [lindex $fr 0]] r]
    while {![eof $fid]} {
      gets $fid ln
      if {[regexp -nocase {^\s*\#\!title\s+(\S+)\s+(\S.*)} $ln dum cn ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  set inparagraph 0
	}
	incr chid
	lappend chapters [list [format "$cn:%06d" $chid] $chid $cn]
	set data($chid,TITLE) $ct
	set data($chid,TEXT) {}
	set csub 0
	set data($chid,SUBS) {}
      } elseif {[regexp -nocase {^\s*\#\!subtitle\s+(\S.*)$} $ln dum ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  set inparagraph 0
	}
	lappend data($chid,SUBS) $ct
	append data($chid,TEXT) "<a name=\"$csub\"><h2>$ct</h2></a>\n"
	incr csub
      } elseif {[regexp {^\s*\#\!\s*$} $ln dum ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  set inparagraph 0
	}
      } elseif {[regexp -nocase {^\s*\#\!image\s+(\S.*)$} $ln dum ii]} {
	set iisrc [file join [lindex $fr 1] $ii]
	set iifnm [format "%06d-%s" $imid [file tail $iisrc]]
	set iiimg [format "%06d.png" $imid [file tail $iisrc]]
	lappend imagelist [list $imid $iisrc $iifnm $iiimg]
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  set inparagraph 0
	}
	append data($chid,TEXT) "<p>\n<a href=\"$iifnm\"><img border=\"1\" src=\"$iiimg\"/></a>\n</p>\n"
	incr imid
      } elseif {[regexp -nocase {^\s*\#\!code\s*$} $ln dum ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  set inparagraph 0
	}
	append data($chid,TEXT) "<pre class=\"code\">\n"
	set incode 1
      } elseif {[regexp -nocase {^\s*\#\!endcode\s*$} $ln dum ct]} {
	append data($chid,TEXT) "</pre>\n"
	set incode 0
      } elseif {[regexp -nocase {^\s*\#\!(process|clean|file)} $ln dum cmd]} {
      } elseif {$incode} {
	append data($chid,TEXT) "$ln\n"
      } elseif {[regexp {^\s*\#\!(.*\S.*)$} $ln dum ct]} {
	if {!$inparagraph} {
	  append data($chid,TEXT) "<p>\n"
	  set inparagraph 1
	}
	append data($chid,TEXT) "$ct\n"
      }
    }
    close $fid 
  }

  if {[string compare $tcl_platform(platform) windows] == 0} {
    set convpath [file normalize "C:/Program files/Therion/bin/convert.exe"]
  } else {
    set convpath convert
  }

  # create output directory
  file mkdir ".docs"
  log_msg "\nConverting images:\n"
  foreach img $imagelist {
    set iiimg [lindex $img 3]
    set iifnm [lindex $img 2]
    set iisrc [lindex $img 1]
    log_msg "$iisrc\n"
    eval "exec \"$convpath\" -density 300 $iisrc .docs/tmp.png"
    eval "exec \"$convpath\" -resize 419x419 .docs/tmp.png .docs/$iiimg"
    file delete -force .docs/tmp.png
    file copy -force -- $iisrc ".docs/$iifnm"
  }

  set fid [open ".docs/index.css" w]
  puts $fid {
body,td,.headerlinks {font-family: Verdana, Helvetica, Arial, sans-serif; line-height: 1.5;}
body { background-color:white;}
.headerlinks {font-weight: 500;}

p,h1,h2,h3 {margin-left: 16px; margin-right: 16px;}
h1 {font-size: 150%}
h2 {font-size: 125%}
.footer {margin-left: 16px;}

a {text-decoration: none}
a:active {color: blue; text-decoration: none;}
a:visited {color: blue; text-decoration: none;}
a:hover {color: red; text-decoration: none; }

ol { list-style-type: lower-greek }
ul { list-style-type: circle }

code {font-family: Courier; font-weight: bold; line-height: 1.5;}
pre.code {padding:0.5em; border:1px solid #000; font-weight:bold; color:#000; background-color:#f7f9fa; overflow:auto; line-height:140%; margin-left: 16px; margin-right: 16px;}

}
  close $fid

  # export html pages
  set fidx [open ".docs/index.html" w]
  set nlist [lsort $chapters]
  set chapters {}
  foreach ch $nlist {
    if {[string length $data([lindex $ch 1],TEXT)] > 0} {
      lappend chapters $ch
    }
  }
  set nchapters [llength $chapters]
  puts $fidx "<html>\n<head>\n<title>Therion features.</title><link href=\"index.css\" rel=\"styleSheet\" type=\"text/css\">\n</head>\n<body>"
  puts $fidx "<center>\n<table width=\"640\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n<td>\n"
  puts $fidx "<h1>Therion features:</h1>\n<p>\n<ul>"
  foreach ch $chapters {
    set chid [lindex $ch 1]
    set chfn "[lindex $ch 2].html"
    puts $fidx "<li><a href=\"$chfn\">$data($chid,TITLE)</a></li>"
    set fout [open ".docs/$chfn" w]
    puts $fout "<html>\n<head>\n<title>$data($chid,TITLE)</title><link href=\"index.css\" rel=\"styleSheet\" type=\"text/css\">\n</head>\n<body>"
    puts $fout "<center>\n<table width=\"640\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n<td>\n"
    puts $fout "<h1>$data($chid,TITLE)</h1>"
    puts -nonewline $fout $data($chid,TEXT)
    puts $fout "</td>\n</tr>\n</table>\n</center>\n</body></html>"
    close $fout
  }
  puts $fidx "</ul>\n</p>"
  puts $fidx "</td>\n</tr>\n</table>\n</center>\n</body></html>"
  puts $fidx "</body></html>"
}


if {([llength $argv] > 0) && [regexp -nocase {^clean$} [lindex $argv 0]]} {
  scan_files ""
  scan_lists
  clean_files
} else {
  if {[llength $argv] == 0} {
    scan_files ""
  } else {
    foreach d $argv {
      scan_files $d
    }
  }
  scan_lists
  process_files
  create_docs
}
  

close $flog
exit