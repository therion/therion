proc xth_te_sdata_scan {} {

  global xth
  if {$xth(te,fcurr) < 0} {
    return [list [expr 2 * $xth(gui,etabsize)] {from to compass clino tape}]
  }
  
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt  

  # let's find the index
  set seli [$w tag ranges sel]
  if {[llength $seli] > 0} {
    set i [lindex $seli 0]
  } else {
    set i [$w index insert]
  }
  
  regexp {(\d+)\.} $i dum cln
  incr cln
  set i [$w index $cln.0]
  regexp {(\d+)\.} $i dum cln
    
  set dind [format \x25[expr 2 * $xth(gui,etabsize)]s " "]
  set dqts {from to compass clino tape}
  set scan_data {
    if {[regexp {(\s*)data\s+\w+\s+(.*)} $slns dum dind dqts]} {
      set endscan 1
    }
  }
  set sln $cln
  set slns ""
  set endscan 0
  while {($sln > 1) && (!$endscan)} {
    incr sln -1
    if {[regexp {\S} $slns]} {
      set clns [$w get $sln.0 $sln.end]
      if {[regexp {(.*)\\\s*$} $clns dum vlns]} {
        set slns "$vlns$slns"
      } else {
        eval $scan_data
        set slns $clns
      }
    } else {
      set slns [$w get $sln.0 $sln.end]
    }
  }
  if {!$endscan} {
    eval $scan_data
  }
  
  return [list [string length $dind] $dqts]     
}


proc xth_te_sdata_buid {qts indc} {

  global xth
  set cf $xth(ctrl,te,sdata).sdf
  catch {destroy $cf}
  frame $cf 
  grid $cf -in $xth(ctrl,te,sdata) -column 0 -row 1 -sticky nsew
  grid columnconf $cf 0 -weight 0
  grid columnconf $cf 1 -weight 1
  grid columnconf $cf 2 -weight 0

  set nent 0
  set invd 0
  set sent 0
  set grow 0
  foreach qtt $qts {
    switch $qtt {
      newline {
        if {(!$invd) && ($nent > 0)} {
          frame $cf.nlf
          grid columnconf $cf.nlf 0 -weight 1
          grid columnconf $cf.nlf 1 -weight 1
          Button $cf.nlf.s$grow -text "Start series" -font $xth(gui,lfont)
          Button $cf.nlf.b$grow -text "Break series" -font $xth(gui,lfont)
          grid $cf.nlf.s$grow -column 0 -row 0 -sticky news
          grid $cf.nlf.b$grow -column 1 -row 0 -sticky news
          grid $cf.nlf -in $cf -column 0 -columnspan 3 -row $grow \
            -sticky news
          set xth(te,sdata,invd,ssb) $cf.nlf.s$grow
          set xth(te,sdata,invd,bsb) $cf.nlf.b$grow
          set xth(te,sdata,invd,ent) $nent
          set invd 1
          incr grow
        }
      }
      default {
        # zistime typ
        if {[info exists xth(datafmt,$qtt,format)]} {
          set tqtt $qtt
        } else {
          set tqtt "unknown"
        }
        set xth(te,sdata,$nent,qtt) $tqtt
        set xth(te,sdata,$nent,format) $xth(datafmt,$tqtt,format)
        set xth(te,sdata,$nent,cvalue) ""
        set xth(te,sdata,$nent,special) $xth(datafmt,$tqtt,special)
        set xth(te,sdata,$nent,de) $cf.de$grow
        set xth(te,sdata,$nent,fe) $cf.fe$grow
        set xth(te,sdata,$nent,nextde) $cf.de$grow
        set xth(te,sdata,$nent,nextvde) $cf.de$grow
        Label $cf.l$grow -text $qtt -anchor e -font $xth(gui,lfont)
        Entry $cf.de$grow -font $xth(gui,lfont) \
          -textvariable xth(te,sdata,$nent,cvalue)
        Entry $cf.fe$grow -font $xth(gui,lfont) -width 6 \
          -textvariable xth(te,sdata,$nent,format)
        bind $cf.de$grow <<xthFocusTo>> "focus $cf.de$grow; $cf.de$grow icursor 0; $cf.de$grow selection range 0 end"
        bind $cf.de$grow <Key-Tab> "event generate \$xth(te,sdata,$nent,nextde) <<xthFocusTo>> -when tail"
        bind $cf.de$grow <Key-Return> "event generate \$xth(te,sdata,$nent,nextvde) <<xthFocusTo>> -when tail"
        bind $cf.de$grow <Key-KP_Enter> "event generate \$xth(te,sdata,$nent,nextvde) <<xthFocusTo>> -when tail"
        grid $cf.l$grow -in $cf -column 0 -row $grow -sticky news
        grid $cf.de$grow -in $cf  -column 1 -row $grow -sticky news
        grid $cf.fe$grow -in $cf  -column 2 -row $grow -sticky news
        incr grow
        incr nent
      }
    }
  }
  
  set xth(te,sdata,nent) $nent
  set xth(te,sdata,indc) $indc
  set xth(te,sdata,invd) $invd
  
  xth_te_sdata_bind
}


proc xth_te_sdata_incr_station {ss} {
  return [xth_incr_station_name $ss 1]
}

proc xth_te_sdata_incr {} {

  global xth
  if {$xth(te,sdata,incr,station) != -1} {
    set xth(te,sdata,$xth(te,sdata,incr,station),cvalue) \
      [xth_te_sdata_incr_station \
      $xth(te,sdata,$xth(te,sdata,incr,station),cvalue)]
  } else {
    if {$xth(te,sdata,incr,from) != -1} {
      if {$xth(te,sdata,incr,to) != -1} {
        set xth(te,sdata,$xth(te,sdata,incr,from),cvalue) \
            $xth(te,sdata,$xth(te,sdata,incr,to),cvalue)
      }
    }
    if {$xth(te,sdata,incr,to) != -1} {
      set xth(te,sdata,$xth(te,sdata,incr,to),cvalue) \
        [xth_te_sdata_incr_station \
        $xth(te,sdata,$xth(te,sdata,incr,to),cvalue)]
    }    
  }
  
}


proc xth_te_sdata_bind {} {

  global xth

  if {! [info exists xth(te,sdata,nent)]} {
    return
  } elseif {$xth(te,sdata,nent) < 1} {
    return
  }

  set xth(te,sdata,incr,station) -1
  set xth(te,sdata,incr,from) -1
  set xth(te,sdata,incr,to) -1
  for {set i 0} {$i < $xth(te,sdata,nent)} {incr i} {
    if {$i != [expr $xth(te,sdata,nent) - 1]} {
      set xth(te,sdata,$i,nextde) $xth(te,sdata,[expr $i + 1],de)
      set xth(te,sdata,$i,nextvde) $xth(te,sdata,[expr $i + 1],de)
    } else {
      set xth(te,sdata,$i,nextde) $xth(te,sdata,0,de)
      set xth(te,sdata,$i,nextvde) $xth(te,sdata,0,de)
    }
    set sx [lsearch {from to station} $xth(te,sdata,$i,qtt)]
    if {$sx != -1} {
      set xth(te,sdata,incr,[lindex {from to station} $sx]) $i
    }
  }
  
  if {! $xth(te,sdata,es)} {
    for {set i 0} {$i < $xth(te,sdata,nent)} {incr i} {
      if {[lsearch {station from to} $xth(te,sdata,$i,qtt)] == -1} {
        for {set j 1} {$j < $xth(te,sdata,nent)} {incr j} {
          set jj [expr ($i + $j) % $xth(te,sdata,nent)]
          if {[lsearch {station from to} $xth(te,sdata,$jj,qtt)] == -1} {
            set xth(te,sdata,$i,nextvde) $xth(te,sdata,$jj,de)
            set j $xth(te,sdata,nent)
          }
        }
      }
    }  
  }
  
  # now let's bind enter keys
  if {$xth(te,sdata,invd)} {
    set wtw1 ""
    set wtw2 ""
    set clw ""
    for {set iet 0} {$iet < $xth(te,sdata,nent)} {incr iet} {
      if {$iet < $xth(te,sdata,invd,ent)} {
        append wtw1 " \$xth(te,sdata,$iet,cvalue)"
      } else {
        append wtw2 " \$xth(te,sdata,$iet,cvalue)"
      }
      if {[lsearch {from to station} $xth(te,sdata,$iet,qtt)] == -1} {
        append clw "set xth(te,sdata,$iet,cvalue) \"\"\n"
      }
    }
    set enter_cmd "xth_te_sdata_insert \"$wtw2\" 2 insert\nxth_te_sdata_insert \"$wtw1\" 1 insert\nxth_te_sdata_incr\n$clw"
    append enter_cmd "event generate \$xth(te,sdata,[expr $xth(te,sdata,nent) - 1],nextvde) <<xthFocusTo>> -when tail"
    bind $xth(te,sdata,[expr $xth(te,sdata,nent) - 1],de) <Return> $enter_cmd
    bind $xth(te,sdata,[expr $xth(te,sdata,nent) - 1],de) <KP_Enter> $enter_cmd

    set enter_cmd "xth_te_sdata_insert \"$wtw1\" 1 insert\nxth_te_sdata_incr\n$clw"
    append enter_cmd "event generate \$xth(te,sdata,[expr $xth(te,sdata,nent) - 1],nextvde) <<xthFocusTo>> -when tail"
    $xth(te,sdata,invd,ssb) configure -command $enter_cmd

    set enter_cmd "xth_te_sdata_insert \"break\" 3 insert\n$clw"
    append enter_cmd "event generate \$xth(te,sdata,0,de) <<xthFocusTo>> -when tail"
    $xth(te,sdata,invd,bsb) configure -command $enter_cmd
    
  } else {
    set wtw ""
    set clw ""
    for {set iet 0} {$iet < $xth(te,sdata,nent)} {incr iet} {
      append wtw " \$xth(te,sdata,$iet,cvalue)"
      if {[lsearch {from to station} $xth(te,sdata,$iet,qtt)] == -1} {
        append clw "set xth(te,sdata,$iet,cvalue) \"\"\n"
      }
    }
    set enter_cmd "xth_te_sdata_insert \"$wtw\" 0 insert\nxth_te_sdata_incr\n$clw"
    append enter_cmd "event generate \$xth(te,sdata,[expr $xth(te,sdata,nent) - 1],nextvde) <<xthFocusTo>> -when tail"
    bind $xth(te,sdata,[expr $xth(te,sdata,nent) - 1],de) <Return> $enter_cmd
    bind $xth(te,sdata,[expr $xth(te,sdata,nent) - 1],de) <KP_Enter> $enter_cmd
  }  
}


$xth(ctrl,te,sdata).sfb configure -command {
  set dil [xth_te_sdata_scan]
  xth_te_sdata_buid [lindex $dil 1] [lindex $dil 0] 
}


$xth(ctrl,te,sdata).sfs configure -command xth_te_sdata_bind


proc xth_te_sdata_insert {data invd iidx} {

  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }

  if {! [info exists xth(te,sdata,nent)]} {
    return
  } elseif {$xth(te,sdata,nent) < 1} {
    return
  }

  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
  set xth(me,sdata,err_notenought) 0

  set err 0
  if {$xth(gui,etabsize) > 0} {
    set tabspc [format \x25$xth(gui,etabsize)s " "]
  } else {
    set tabspc "  "
  }
  set sent 0
  set eent $xth(te,sdata,nent)
  switch $invd {
    1 {
      set eent $xth(te,sdata,invd,ent)
    }
    2 {
      set sent $xth(te,sdata,invd,ent)
    }
  }
  
  set txt ""
  set fst ""
  set iet $sent
  set tmp $data
  set ldata {}
  while {[string length $tmp] > 0} {
    if {[regexp {\S+} $tmp itm]} {
      lappend ldata $itm
    }
    regsub {\s*\S*\s*} $tmp {} tmp
  }
  if {([llength $ldata] < $eent) && (!$xth(te,sdata,invd))} {
    set xth(me,sdata,err_notenought) 1
  }
  foreach itm $ldata {
    set postwrt 0
    set tobreak 0
    if {$iet < $eent} {
      if {[lsearch -exact $xth(te,sdata,$iet,special) $itm] != -1} {
        set postwrt 1
      } else {
        set curfmt $xth(te,sdata,$iet,format)
        set extfmt 0
        if {[regsub {fx(\s*)$} $curfmt {f\1} curfmt]} {
          set extfmt 1
        }
#        puts "$itm -> $fst\x25$curfmt"
        if {[catch {append txt [format "$fst\x25$curfmt" $itm]}]} {
          set postwrt 1
#          puts "error"
          set err 1
        } elseif {$extfmt == 1} {
          if {[regexp {\.?0+\s*$} $txt txtextend]} {
            set teel [string length $txtextend]
            regsub {\.?0+\s*$} $txt [format \x25[expr $teel]s " "] txt
          }
        }
      }
    } else {
      # ak je dlhsie, uz neformatuj
#      puts $data
      set unfdata $data
      for {set ufi 0} {$ufi < $eent} {incr ufi} {
        regsub {^\s*\S+\s*} $unfdata "" unfdata
      }
      append txt $fst $unfdata
      # append txt [format $fst\x25$xth(datafmt,unknown,format) $itm]
      # set err 1
      set tobreak 1
    }
    
      
    if {$postwrt == 1} {
      if {[regexp {(\d+)\.?(\d*)} $xth(te,sdata,$iet,format) dum nfln nzadc]} {
        set nitm $itm
        if {[string length nzadc] > 0} {
          append nitm [format \x25[expr $nzadc + 1]s " "]
        }
        append txt [format $fst\x25[expr $nfln]s $nitm]
      } else {
        append txt [format $fst\x25$xth(datafmt,unknown,format) $itm]
      }
    }
    
    set fst $tabspc  
    incr iet
    if {$tobreak} {
      break
    }
  }
  
  set cind $xth(te,sdata,indc)
  if {($invd == 2) && (!$err)} {
    if {[regexp {\d+} $xth(te,sdata,0,format) plusindc]} {
      incr cind [expr $plusindc + 1]
    } else {
      incr cind [expr 2 * $xth(gui,etabsize)]
    }
  }
  
  if {($invd == 3) || $err} {
    set txt $data
    regsub {^\s+} $txt "" txt
    regsub {\s+$} $txt "" txt
  }
  
  if {$cind > 0} {
    set txt [format \x25[expr $cind + [string length $txt]]s $txt]  
  }
  
  if {[string compare $iidx insert] == 0} {
    xth_te_insert_text $w "\n$txt"
  } else {
    $w insert $iidx $txt
  }
  catch {
    $w edit separator
  }
  return $err
  
}


proc xth_te_sdata_auto_format {} {

  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }

  if {! [info exists xth(te,sdata,nent)]} {
    return
  } elseif {$xth(te,sdata,nent) < 1} {
    return
  }
  
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
  set s [$w tag ranges sel]
  if {[llength $s] < 2} {
    return
  }
  
  set eline -1
  set sline 0
  regexp {(\d+)\.} [lindex $s 0] dum sline 
  regexp {(\d+)\.} [lindex $s 1] dum eline 
  # $w tag remove sel 1.0 end
  set ict 1
  for {set cline $sline} {$cline <= $eline} {incr cline} {
    set txt [$w get $cline.0 $cline.end]
    if {[regexp {\S+} $txt]} {
      set orig [$w get $cline.0 $cline.end]
      $w delete $cline.0 $cline.end
      if {$xth(te,sdata,invd)} {
        set formatres [xth_te_sdata_insert $txt $ict $cline.0]
        if {$formatres == 0} {
          if {$ict == 1} {
            set ict 2
          } else {
            set ict 1
          }
        } else {
          if {[regexp {^\s*break\s*$} $txt]} {
            set xth(me,sdata,err_notenought) 0
            set ict 1
          }
        }
      } else {
        set formatres [xth_te_sdata_insert $txt 0 $cline.0]
      }
      # an error occured
      if {$formatres || $xth(me,sdata,err_notenought)} {
#        puts "inserting >>$orig<<"
        $w delete $cline.0 $cline.end
        $w insert $cline.0 $orig
      }
    }
  }
  $w see insert
}


proc te_sr_reset {} {

  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt

  set has_sel 0  
  if {$xth(ctrl,te,sr,selection_io)} {
    set seli [$w tag ranges sel]
    if {[llength $seli] > 0} {
      set xth(ctrl,te,sr,selection_start) [lindex $seli 0]
      set xth(ctrl,te,sr,selection_end) [lindex $seli 1]
      set xth(ctrl,te,sr,search_end) [lindex $seli 1]
      $w mark set insert [lindex $seli 0]
      set has_sel 1
    } else {
      set xth(ctrl,te,sr,selection_io) 0
      update idletasks
    }
  }
  
  if {!$has_sel} {
    set xth(ctrl,te,sr,selection_start) {}
    set xth(ctrl,te,sr,selection_end) {}
    set xth(ctrl,te,sr,search_end) end
    $w mark set insert 1.0
  }
  
}


proc te_sr_first {} {
  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  if {[string length $xth(ctrl,te,sr,search)] == 0} {
    return
  }
  catch {
    $w edit separator
  }
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
  te_sr_clear  
  te_sr_reset
  if {![te_sr_next_next]} {
    bell
  }
  catch {
    $w edit separator
  }
  focus $w
}

proc te_sr_next_next {} {
  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  if {[string length $xth(ctrl,te,sr,search)] == 0} {
    return
  }
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt

  # prehlada text od pozicie kurzora po koniec (vyberu)
  # najde text - (zameni ho) - vyznaci ho a kurzor nastavi za neho
  
  set cnt 0
  set fndcmd "set fnd \[$w search -count cnt"
  if {!$xth(ctrl,te,sr,case_io)} {
    append fndcmd " -nocase"
  }
  if {$xth(ctrl,te,sr,regular_io)} {
    append fndcmd " -regexp"
  }
  append fndcmd { $xth(ctrl,te,sr,search) insert $xth(ctrl,te,sr,search_end)]}
  eval $fndcmd
  
  if {[string length $fnd] > 0} {
    $w mark set insert "$fnd + $cnt chars"
    $w mark set xthsrend "$fnd + $cnt chars"
    # do replace if necessary
    if {$xth(ctrl,te,sr,replace_io)} {
      set ostr [$w get $fnd xthsrend]
      $w delete $fnd xthsrend
      #puts "<<$ostr"
      set nstr $xth(ctrl,te,sr,replace)
      if {$xth(ctrl,te,sr,regular_io)} {
        set repcmd {regsub}
        if {!$xth(ctrl,te,sr,case_io)} {
          append repcmd " -nocase"
        }
        append repcmd { $xth(ctrl,te,sr,search) $ostr $xth(ctrl,te,sr,replace) nstr}
        eval $repcmd
      }
      #puts ">>$nstr"
      $w insert $fnd $nstr
    }
    $w tag add xthsr $fnd xthsrend
    $w tag configure xthsr -background $xth(gui,escolorbg) -foreground $xth(gui,escolorfg)
    $w see insert
    return 1
  } else {
    # uz sme nic nenasli
    return 0
  }  

}

proc te_sr_next {} {
  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  if {[string length $xth(ctrl,te,sr,search)] == 0} {
    return
  }
  catch {
    $w edit separator
  }
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
  if {![te_sr_next_next]} {
    bell
  }
  catch {
    $w edit separator
  }
  focus $w
}

proc te_sr_all {} {
  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  if {[string length $xth(ctrl,te,sr,search)] == 0} {
    return
  }
  catch {
    $w edit separator
  }
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
  te_sr_clear  
  te_sr_reset
  if {![te_sr_next_next]} {
    bell
  } else {
    while {[te_sr_next_next]} {}
  }
  catch {
    $w edit separator
  }
  focus $w
}

proc te_sr_clear {} {
  global xth
  if {$xth(te,fcurr) < 0} {
    return
  }
  set w $xth(te,[lindex $xth(te,flist) $xth(te,fcurr)],frame).txt
  $w tag remove xthsr 1.0 end
  focus $w
}

