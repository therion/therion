proc xth_te_import_file {} {

  global xth
  
  set fname [tk_getOpenFile -filetypes {{{PocketTopo therion export} {.txt}} {{All files} {*}}} \
    -parent $xth(gui,main) \
    -initialdir $xth(gui,initdir) -defaultextension .txt]
  
  if {[string length $fname] == 0} {
    return 0
  } else {
    set xth(gui,initdir) [file dirname $fname]
  }
  
  # read the file
  xth_status_bar_push te
  xth_status_bar_status te [format [mc "Importing %s ..."] $fname]
  
  set fdata [xth_te_read_file $fname]
  if {[lindex $fdata 0] == 0} {
      MessageDlg $xth(gui,message) -parent $xth(gui,main) \
        -icon error -type ok \
        -message [lindex $fdata 1] \
        -font $xth(gui,lfont)
      xth_status_bar_pop te
      return 0
  }
  
  # show the file
  set cfid [lindex $xth(te,flist) $xth(te,fcurr)]
  catch {
    $xth(te,$cfid,frame).txt edit separator
  }
  xth_te_insert_text $xth(te,$cfid,frame).txt "[xth_te_read_ptopo $fname]\n"
  $xth(te,$cfid,frame).txt see insert

  xth_status_bar_pop te
  return 1
}






proc xth_te_read_ptopo {fname} {
  set fid [open $fname r]
  set ccline {}
  set cclines {}
  set csection {NULL}
  set indata 0
  set cdirec 0
  while {![eof $fid]} {
    gets $fid ln
    ##append ccline "### $csection: $ln\n"
    if {[string equal $ln FIX] || [string equal $ln TRIP] || [string equal $ln PLAN] || [string equal $ln ELEVATION]} {
      if {[string length $ccline] > 0} {
        lappend cclines $ccline
        set ccline {}
        set indata 0
        set cdirec 0
      }
      set csection $ln
    }
    if {[llength $ln] > 0} {
      switch $csection {
        FIX {
          if {[llength $ln] == 4} {
            append ccline "  fix [lindex $ln 0] [lindex $ln 1] [lindex $ln 2] [lindex $ln 3]\n"
          }
        }
        TRIP {
          if {$indata} {
            # check direction
            if {[string equal [lindex $ln end] "<"]} {
              if {$cdirec >= 0} {
                append ccline "  extend left\n"
                set cdirec -1
              }
            } elseif {[string equal [lindex $ln end] ">"]} {
              if {$cdirec <= 0} {
                append ccline "  extend right\n"
                set cdirec 1
              }
            }
            if {[llength $ln] == 5} {
              append ccline "  [lindex $ln 0] - [lindex $ln 1] [lindex $ln 2] [lindex $ln 3]\n"
            } elseif {[llength $ln] == 6} {
              append ccline "  [lindex $ln 0] [lindex $ln 1] [lindex $ln 2] [lindex $ln 3] [lindex $ln 4]\n"
            }
          } else {
            append $ccline "# --[lindex $ln 0]--"
            switch [lindex $ln 0] {
              DATE {
                set xd [lindex $ln 1]
                regsub -all {\-} $xd {.} xd
                append ccline "  date $xd\n"
              }
              DECLINATION {
              }
              DATA {
                append ccline "  data normal from to compass clino tape\n"
                set indata 1
              }
            }
          }
        }
      }
    }
  }
  if {[string length $ccline] > 0} {
    lappend cclines $ccline
  }
  close $fid
  set txt {}
  foreach cln $cclines {
    append txt "centreline\n$cln"
    append txt "endcentreline\n\n"
  }
  return $txt
}


