#! /usr/bin/tclsh
# Test all therion features and generate features HTML pages.
#
# Usage: samples.tcl
#        samples.tcl clean

set outdd ".."
if {[llength $argv] > 0} {
  set outdd [lindex $argv 0]
}

set outd "$outdd/samples.doc"
set flog [open samples.log w]

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
  }
  lappend dirlist $dir
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
}

set thcmd [file normalize [file join [pwd] "$outdd/therion"]]
set processlist {}

proc scan_lists {} {
  global dirnum farray dirlist
  global filelist farray cleanlist processlist thcmd
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    set farray($fnum,FILE) 0
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


  foreach dn $dirlist {
    set clist {}
    catch {
      set clist [glob -directory $dn *.log *~ .xth_* .xtherion.dat]
    }
    foreach cf $clist {
      lappend cleanlist $cf
    }
  }
  set sortl {}
  set fxmax {}
  set fxmin {}
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    set fdir [lindex $fr 1]
    set dnum [lindex $fr 3]
    set ftxt $farray($fnum,FILE)
    while {[regexp {^\s*\.\.\/} $ftxt]} {
      set fdir [file dirname $fdir]
      set dnum [expr [lsearch $dirlist $fdir] + 1]
      regsub {^\s*\.\.\/} $ftxt {} ftxt
    }
    set fxval [expr double($ftxt)]
    if {[llength $fxmax] == 0} {
      set fxmax $fxval
      set fxmin $fxmax
    } else {
      if {$fxmax < $fxval} {set fxmax $fxval}
      if {$fxmin > $fxval} {set fxmin $fxval}
    }
    lappend sortl [list $fnum $dnum $fxval]
  }
  set ddist [expr $fxmax - $fxmin + 1.0]
  foreach litem $sortl {
    set fnum [lindex $litem 0]
    set dnum [lindex $litem 1]
    set fxval [lindex $litem 2]
    set fsort([expr double($dnum) * $ddist + $fxval]) [list $fnum $dnum]
  }
  set newfilelist {}
  set xlist [lsort -real [array names fsort]]
  foreach x $xlist {
    set newitem [lindex $filelist [lindex $fsort($x) 0]]
    lset newitem 3 [lindex $fsort($x) 1]
    lappend newfilelist $newitem
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


proc get_html_body {fn} {
  set fid [open $fn "r"]
  set ftext [read $fid]
  close $fid
  regexp -nocase {\<\s*body[^\>]*\>\s*(.*)\<\/\s*body[^\>]*\>} $ftext dum ftext
  regsub -nocase -all {\<\s*table} $ftext {<table class="htmlinput"} ftext
  regsub -nocase -all {\<\s*tr} $ftext {<tr class="htmlinput"} ftext
  regsub -nocase -all {\<\s*td} $ftext {<td class="htmlinput"} ftext
  regsub -nocase -all {\<\s*th} $ftext {<th class="htmlinput"} ftext
  regsub {\s*$} $ftext {} ftext
  return $ftext
}

proc get_html_body_for_tex {fn} {
  set fid [open $fn "r"]
  set ftext [read $fid]
  close $fid
  regexp -nocase {\<\s*body[^\>]*\>\s*(.*)\<\/\s*body[^\>]*\>} $ftext dum ftext
  regsub -nocase -all {\<\s*table[^\>]*>} $ftext "\\bigskip\\begingroup\\input etc/TXSruled.tex \\LeftJustifyTables\\ruledtable" ftext
  regsub -nocase -all {</td>[\n\s]*</tr>[\n\s]*</table>} $ftext "\\endruledtable\\endgroup\\bigskip" ftext
  regsub -nocase -all {\<tr[^\>]*>} $ftext {} ftext
  regsub -nocase {</th>[\n\s]*</tr>} $ftext "\}\\cr" ftext
  regsub -nocase -all {</td>[\n\s]*\</tr>} $ftext "\\nr" ftext
  regsub -nocase -all {\<td[^\>]*>} $ftext "" ftext
  regsub -nocase -all {\</td>} $ftext {\&} ftext
  regsub -nocase -all {\<th[^\>]*>} $ftext "\{\\bf " ftext
  regsub -nocase -all {\</th>} $ftext "\}\\&" ftext
  regsub -all {\_} $ftext {\\_} ftext
  regsub -all {\&nbsp;} $ftext { } ftext
  regsub {\s*$} $ftext {} ftext
  return $ftext
}


proc create_docs {} {
  global filelist tcl_platform outd outdd
  set cdir {}
  set chid 0
  set imid 0
  set chapters {}
  set imagelist {}
  array set data {}
  array set texdata {}
  # scan chapters and subchapters
  log_msg "\nScanning texts:\n"
  set inparagraph 0
  set incode 0
  foreach fr $filelist {
    set fnum [lindex $fr 2]
    if {[lindex $fr 3] != $cdir} {
      if {$inparagraph} {
	append data($chid,TEXT) "</p>\n"
	append texdata($chid,TEXT) "\n\n"
	set inparagraph 0
      }
      incr chid
      lappend chapters [list $chid 0]
      set data($chid,TITLE) [file tail [lindex $fr 1]]
      set texdata($chid,TITLE) [file tail [lindex $fr 1]]
      set data($chid,TEXT) {}
      set texdata($chid,TEXT) {}
      set csub 0
      set data($chid,SUBS) {}
      set texdata($chid,SUBS) {}
      set cdir [lindex $fr 3]
    }
    log_msg "[file join [lindex $fr 1] [lindex $fr 0]]\n"
    set fid [open [file join [lindex $fr 1] [lindex $fr 0]] r]
    while {![eof $fid]} {
      gets $fid ln
      if {[regexp {^\s*encoding\s+(\S+)} $ln dum enc]} {
	switch $enc {
	  default {set fenc $enc}
	}
	fconfigure $fid -encoding $fenc
      }
      if {[regexp -nocase {^\s*\#\!title\s+(\S+)\s+(\S.*)} $ln dum cn ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 0
	}
	incr chid
	lappend chapters [list $chid $cn]
	set data($chid,TITLE) $ct
	set texdata($chid,TITLE) $ct
	set data($chid,TEXT) {}
	set texdata($chid,TEXT) {}
	set csub 0
	set data($chid,SUBS) {}
	set texdata($chid,SUBS) {}
      } elseif {[regexp -nocase {^\s*\#\!subtitle\s+(\S.*)$} $ln dum ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 0
	}
	lappend data($chid,SUBS) $ct
	lappend texdata($chid,SUBS) $ct
	append data($chid,TEXT) "<a name=\"$csub\"></a><h2>$ct</h2>\n"
	append texdata($chid,TEXT) "\\subsubchapter {$ct}.\n"
	incr csub
      } elseif {[regexp {^\s*\#\!\s*$} $ln dum ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 0
	}
      } elseif {[regexp -nocase {^\s*\#\!image\s+(\S.*)$} $ln dum ii]} {
	set iisrc [file join [lindex $fr 1] $ii]
	set iifnm [format "%06d-%s" $imid [file tail $iisrc]]
	set iiimg [format "%06d.png" $imid [file tail $iisrc]]
	lappend imagelist [list $imid $iisrc $iifnm $iiimg]
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 0
	}
	append data($chid,TEXT) "<p>\n<a href=\"$iifnm\"><img border=\"1\" src=\"$iiimg\"/></a>\n</p>\n"
	append texdata($chid,TEXT) "\n\n\\fitpic{$outdd/samples.doc/$iifnm}\n\n"
	incr imid
      } elseif {[regexp -nocase {^\s*\#\!HTML\s+(\S.*)$} $ln dum ii]} {
	set html_src [file join [lindex $fr 1] $ii]
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 0
	}
	append data($chid,TEXT) [get_html_body $html_src]
	append texdata($chid,TEXT) [get_html_body_for_tex $html_src]
      } elseif {[regexp -nocase {^\s*\#\!code\s*$} $ln dum ct]} {
	if {$inparagraph} {
	  append data($chid,TEXT) "</p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 0
	}
	append data($chid,TEXT) "<pre class=\"code\">\n"
	append texdata($chid,TEXT) "|"
	set incode 1
      } elseif {[regexp -nocase {^\s*\#\!endcode\s*$} $ln dum ct]} {
	append data($chid,TEXT) "</pre>\n"
	append texdata($chid,TEXT) "|\n"
	set incode 0
      } elseif {[regexp -nocase {^\s*\#\!(process|clean|file)} $ln dum cmd]} {
      } elseif {$incode} {
	append data($chid,TEXT) "$ln\n"
	append texdata($chid,TEXT) "$ln\n"
      } elseif {[regexp {^\s*\#\!(.*\S.*)$} $ln dum ct]} {
	if {!$inparagraph} {
	  append data($chid,TEXT) "<p>\n"
	  append texdata($chid,TEXT) "\n\n"
	  set inparagraph 1
	}
	append data($chid,TEXT) "$ct\n"
        regsub -nocase -all {\</?code\>} $ct {|} ct
        regsub -nocase -all {\<em\>} $ct "\{\\it " ct
        regsub -nocase -all {\</em\>} $ct "\}" ct
        regsub -nocase -all {MetaPost} $ct "\\MP{}" ct
	
	append texdata($chid,TEXT) "$ct\n"
      }
    }
    close $fid 
  }

  if {[string compare $tcl_platform(platform) windows] == 0} {
    package require registry
    set convpath [file normalize "[registry get HKEY_CURRENT_USER\\SOFTWARE\\Therion InstallDir]/bin/convert.exe"]
  } else {
    set convpath convert
  }

  # create output directory
  file mkdir $outd
  log_msg "\nConverting images:\n"
  foreach img $imagelist {
    set iiimg [lindex $img 3]
    set iifnm [lindex $img 2]
    set iisrc [lindex $img 1]
    log_msg "$iisrc\n"
    set dpi 300
    while {[catch {
      eval "exec \"$convpath\" -colorspace RGB -density $dpi $iisrc $outd/tmp.png"
    }] && ($dpi > 10)} {
      log_msg "error at $dpi dpi processing $iisrc\n"
      set dpi [expr int(double($dpi) * 0.9)]
    }
    eval "exec \"$convpath\" -colorspace RGB -resize 419x419 $outd/tmp.png $outd/$iiimg"
    file delete -force $outd/tmp.png
    file copy -force -- $iisrc "$outd/$iifnm"
  }

  set fid [open "$outd/index.css" w]
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

code {
  font-family: Courier; 
  font-weight: bold; 
  line-height: 1.5;
}

pre.code {
  padding:0.5em; 
  border:1px solid #000; 
  font-weight:bold; color:#000; 
  background-color:#f7f9fa; 
  overflow:auto; 
  line-height:140%; 
  margin-left: 16px; 
  margin-right: 16px;
}

font.nav {font-size: 75%;}

table.htmlinput {
    font-size:80%; 
    margin-left: 16px; 
    margin-right: 16px;
    border-spacing: 0px;
}
th.htmlinput {
    border-bottom:2px solid #a7a9aa;
    padding-left:8px; 
    padding-right:8px; 
    padding-top:0px; 
    padding-bottom:1px
}
td.htmlinput {
    border-bottom:1px solid #a7a9aa;
    padding-left:8px; 
    padding-right:8px;
    padding-top:0px; 
    padding-bottom:0px
}

}
# END OF CSS

  close $fid

  # export html pages
  set fidx [open "$outd/index.html" w]
  fconfigure $fidx -encoding utf-8
  set texfidx [open "$outd/index.tex" w]
  foreach ch $chapters {
    set csort([expr double([lindex $ch 1])]) $ch
  }
  set xlist [lsort -real [array names csort]]
  set chapters {}
  foreach xch $xlist {
    set ch $csort($xch)
    if {[string length $data([lindex $ch 0],TEXT)] > 0} {
      lappend chapters $ch
    }
  }
  set nchapters [llength $chapters]
  puts $fidx "<html>\n<head>\n<title>Therion features.</title><link href=\"index.css\" rel=\"styleSheet\" type=\"text/css\">\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n</head>\n<body>"
  puts $fidx "<center>\n<table width=\"640\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n<td>\n"
  puts $fidx "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>"
  puts $fidx "<td width=\"100%\" align=\"right\"><a href=\"[lindex [lindex $chapters 0] 1].html\"><font class=\"nav\">|Start|</font></a></td>"
  puts $fidx "</tr>\n</table>"
  puts $fidx "<hr noshade color=\"#000\" size=\"1\" width=\"100%\"/>"
  puts $fidx "<h1>Therion features:</h1>\n<p>\n<ul>"
  set chx 0
  foreach ch $chapters {
    set chid [lindex $ch 0]
    set chfn "[lindex $ch 1].html"
    puts $fidx "<li><a href=\"$chfn\">$data($chid,TITLE)</a></li>"
    puts $texfidx "\\subchapter {$texdata($chid,TITLE)}."
    puts $texfidx $texdata($chid,TEXT)
    set fout [open "$outd/$chfn" w]
    fconfigure $fout -encoding utf-8
    puts $fout "<html>\n<head>\n<title>$data($chid,TITLE)</title><link href=\"index.css\" rel=\"styleSheet\" type=\"text/css\">\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n</head>\n<body>"
    puts $fout "<center>\n<table width=\"640\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>\n<td>\n"
    puts $fout "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>"
    puts $fout "<td width=\"33%\" align=\"left\">"
    if {$chx > 0} {
      puts $fout "<a href=\"[lindex [lindex $chapters [expr $chx - 1]] 1].html\"><font class=\"nav\">|Previous|</font></a>"
    }
    puts $fout "</td>"
    puts $fout "<td width=\"34%\" align=\"center\"><a href=\"index.html\"><font class=\"nav\">|Contents|</font></a></td>"
    puts $fout "<td width=\"33%\" align=\"right\">"
    if {($chx + 1) < $nchapters} {
      puts $fout "<a href=\"[lindex [lindex $chapters [expr $chx + 1]] 1].html\"><font class=\"nav\">|Next|</font></a>"
    }
    puts $fout "</td>"
    puts $fout "</tr>\n</table>"
    puts $fout "<hr noshade color=\"#000\" size=\"1\" width=\"100%\"/>"
    puts $fout "<h1>$data($chid,TITLE)</h1>"
    puts -nonewline $fout $data($chid,TEXT)
    puts $fout "<p></p>\n";
    puts $fout "<hr noshade color=\"#000\" size=\"1\" width=\"100%\"/>"
    puts $fout "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>"
    puts $fout "<td width=\"33%\" align=\"left\">"
    if {$chx > 0} {
      puts $fout "<a href=\"[lindex [lindex $chapters [expr $chx - 1]] 1].html\"><font class=\"nav\">|Previous|</font></a>"
    }
    puts $fout "</td>"
    puts $fout "<td width=\"34%\" align=\"center\"><a href=\"index.html\"><font class=\"nav\">|Contents|</font></a></td>"
    puts $fout "<td width=\"33%\" align=\"right\">"
    if {($chx + 1) < $nchapters} {
      puts $fout "<a href=\"[lindex [lindex $chapters [expr $chx + 1]] 1].html\"><font class=\"nav\">|Next|</font></a>"
    }
    puts $fout "</td>"
    puts $fout "</tr>\n</table>"
    puts $fout "</td>\n</tr>\n</table>\n</center>\n</body></html>"
    close $fout
    incr chx
  }
  puts $fidx "</ul>\n</p>"
  puts $fidx "<hr noshade color=\"#000\" size=\"1\" width=\"100%\"/>"
  puts $fidx "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n<tr>"
  puts $fidx "<td width=\"100%\" align=\"right\"><a href=\"[lindex [lindex $chapters 0] 1].html\"><font class=\"nav\">|Start|</font></a></td>"
  puts $fidx "</tr>\n</table>"
  puts $fidx "</td>\n</tr>\n</table>\n</center>\n"
  puts $fidx "</body></html>"
}




if {([llength $argv] > 1) && [regexp -nocase {^clean$} [lindex $argv 1]]} {
  scan_files ""
  scan_lists
  clean_files
} else {
  if {[llength $argv] == 1} {
    scan_files ""
  } else {
    foreach d [lrange $argv 1 end] {
      scan_files $d
    }
  }
  scan_lists
  process_files
  create_docs
}
  

close $flog
exit
