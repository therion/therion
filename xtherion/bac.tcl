#! /usr/bin/wish

# TODO:
# vstup hodiny : minuty

proc xth_bac_check {number from to wgt} {

  set rw 0
  if [catch {expr double($number)}] {
    set rw 1
  } else {
    set num [expr double($number)]
    if {($num < $from) || ($num > $to)} {
      set rw 1
    }
  }
  
  if {$rw} {
    $wgt configure -fg red
    return 0
  } else {
    $wgt configure -fg black
    return 1
  }
  
}

proc xth_bac_hm2h {h} {
  if {[regexp {(.+)\:(.+)} $h dum hh mm]} {
    if {[catch {
      set h [expr double($hh) + double($mm) / 60.0]
      return $h
    }]} {
      return $h
    }
  } else {
    return $h
  }
}

proc xth_bac_h2hm {h} {
  catch {
    set hh [expr int(floor($h))]
    set mm [expr int($h*60.0) % 60]
    if {$mm != 0.0} {
      set h [format "%d:%02d" $hh $mm]  
    } else {
      set h $hh
    }
  }
  return $h
}

proc xth_bac_calculate {} {

  global xth

  set xth(bac,BAC) ""
  set xth(bac,ETA) ""
  set xth(bac,MRS) ""
  set f $xth(gui,bacw)
  $f.rdle configure -bg $xth(bac,bg) -fg black

  set gender xth(bac,gender)
  set nok 0
  if [xth_bac_check $xth(bac,age) 0 1e10 $f.bdal] {set age $xth(bac,age)} else {set nok 1}
  if [xth_bac_check $xth(bac,height) 0 1e10 $f.bdhl] {set height $xth(bac,height)} else {set nok 1}
  if [xth_bac_check $xth(bac,weight) 0 1e10 $f.bdwl] {set weight $xth(bac,weight)} else {set nok 1}
  if [xth_bac_check $xth(bac,volume) 0 1e10 $f.cdvl] {set volume $xth(bac,volume)} else {set nok 1}
  if [xth_bac_check $xth(bac,level) 0 100 $f.cdll] {set level $xth(bac,level)} else {set nok 1}
  set xth(bac,time) [xth_bac_hm2h $xth(bac,time)]
  if [xth_bac_check $xth(bac,time) 0 1e10 $f.cdtl] {set elapsedTime [expr double($xth(bac,time))]} else {set nok 1}
  set xth(bac,time) [xth_bac_h2hm $xth(bac,time)]
  if $nok return

  set ALCOHOL_DENSITY 0.79
  set ingested [expr double($volume) * double($level) * 0.01 * $ALCOHOL_DENSITY]

  # in g/hr
	set METABOLIC_REMOVAL_RATE 7.0
	set remaining [expr $ingested - ($METABOLIC_REMOVAL_RATE * $elapsedTime)]
  if {$remaining < 0.0} {set remaining 0}
  
  if $xth(bac,gender) {
	  set HEIGHT_FACTOR 0.1074
    set WEIGHT_FACTOR 0.3362
    set AGE_FACTOR 0.09516
    set BODY_WATER_CONST 2.447
  } else {
    set HEIGHT_FACTOR 0.1069
    set WEIGHT_FACTOR 0.2466
    set AGE_FACTOR 0
    set BODY_WATER_CONST 2.097
  }
	set h [expr $HEIGHT_FACTOR * $height]
	set w [expr $WEIGHT_FACTOR * $weight]
	set a [expr $AGE_FACTOR * $age]
	set bodyWater [expr ($h - $a + $w + $BODY_WATER_CONST) * 1000.0]
  
	set WATER_CONTENT_OF_BLOOD 0.8157
	set BAC [expr 100.0 * ($remaining / ($bodyWater / $WATER_CONTENT_OF_BLOOD))]
	set MRS [expr 100.0 * ($METABOLIC_REMOVAL_RATE / ($bodyWater / $WATER_CONTENT_OF_BLOOD))]
  set maxRemaining [expr ($bodyWater / $WATER_CONTENT_OF_BLOOD) * 0.0002]
  if {$maxRemaining < $remaining} {
    set ETA [expr ($remaining - $maxRemaining) / $METABOLIC_REMOVAL_RATE]
  } else {
    set ETA 0.0
  }
  
  
  set xth(bac,BAC) [format "%.3f" $BAC]
  set xth(bac,ETA) [format "%d:%02d" [expr int(floor($ETA))] [expr int($ETA*60.0) % 60]]
  set xth(bac,MRS) [format "%.3f" $MRS]
  
  if {$BAC < 0.02} {
    $f.rdle configure -bg green -fg black
  } else {
    $f.rdle configure -bg red -fg white
  }

}


proc xth_bac_init {} {

  global xth
  
  set f $xth(gui,bacw)
  toplevel $f
  wm transient $f $xth(gui,main)
  wm title $f "BAC calculator"
  
  Label $f.bdl -text "Biometric data" -anchor center -font $xth(gui,lfont)
  Label $f.bdal -text "age" -anchor e -font $xth(gui,lfont)
  Entry $f.bdae -font $xth(gui,lfont) -width 5 -textvariable xth(bac,age)
  Label $f.bdau -text "years" -anchor w -font $xth(gui,lfont)
  Label $f.bdgl -text "gender" -anchor e -font $xth(gui,lfont)
  radiobutton $f.bdgm -text "male" -anchor w \
  -font $xth(gui,lfont) -variable xth(bac,gender) -value 1
  radiobutton $f.bdgf -text "female" -anchor w \
  -font $xth(gui,lfont) -variable xth(bac,gender) -value 0
  Label $f.bdhl -text "height" -anchor e -font $xth(gui,lfont)
  Entry $f.bdhe -font $xth(gui,lfont) -width 5 -textvariable xth(bac,height)
  Label $f.bdhu -text "cm" -anchor w -font $xth(gui,lfont)
  Label $f.bdwl -text "weight" -anchor e -font $xth(gui,lfont)
  Entry $f.bdwe -font $xth(gui,lfont) -width 5 -textvariable xth(bac,weight)
  Label $f.bdwu -text "kg" -anchor w -font $xth(gui,lfont)
  Separator $f.s1 -orient horizontal

  Label $f.cdl -text "Consumption data" -anchor center -font $xth(gui,lfont)
  Label $f.cdvl -text "volume" -anchor e -font $xth(gui,lfont)
  Entry $f.cdve -font $xth(gui,lfont) -width 5 -textvariable xth(bac,volume)
  Label $f.cdvu -text "ml" -anchor w -font $xth(gui,lfont)

  Label $f.cdll -text "alcohol level" -anchor e -font $xth(gui,lfont)
  Entry $f.cdle -font $xth(gui,lfont) -width 5 -textvariable xth(bac,level)
  Label $f.cdlu -text "%" -anchor w -font $xth(gui,lfont)
  
  Label $f.cdtl -text "time elapsed" -anchor e -font $xth(gui,lfont)
  Entry $f.cdte -font $xth(gui,lfont) -width 5 -textvariable xth(bac,time)
  Label $f.cdtu -text "hours" -anchor w -font $xth(gui,lfont)

  Button $f.calc -text "Calculate" -anchor center -font $xth(gui,lfont) \
  -command xth_bac_calculate

  Label $f.rdll -text "BAC" -anchor e -font $xth(gui,lfont)
  Entry $f.rdle -font $xth(gui,lfont) -width 5 -textvariable xth(bac,BAC) \
    -editable 0
  Label $f.rdlu -text "%" -anchor w -font $xth(gui,lfont)
  
  Label $f.rdtl -text "ETA" -anchor e -font $xth(gui,lfont)
  Entry $f.rdte -font $xth(gui,lfont) -width 5 -textvariable xth(bac,ETA) \
    -editable 0
  Label $f.rdtu -text "hours" -anchor w -font $xth(gui,lfont)

  Label $f.rdml -text "MRR" -anchor e -font $xth(gui,lfont)
  Entry $f.rdme -font $xth(gui,lfont) -width 5 -textvariable xth(bac,MRS) \
    -editable 0
  Label $f.rdmu -text "%/h" -anchor w -font $xth(gui,lfont)

  Button $f.cls -text "Close" -anchor center -font $xth(gui,lfont) \
  -command "destroy $f"

  
  grid columnconf $f 0 -weight 0
  grid columnconf $f 1 -weight 1
  grid columnconf $f 2 -weight 0

  grid $f.bdl -column 0 -row 0 -columnspan 3 -sticky news 
  grid $f.cdl -column 0 -row 7 -columnspan 3 -sticky news 

  grid $f.bdal -column 0 -row 1 -columnspan 1 -sticky news 
  grid $f.bdgl -column 0 -row 2 -rowspan 2 -sticky news 
  grid $f.bdhl -column 0 -row 4 -columnspan 1 -sticky news 
  grid $f.bdwl -column 0 -row 5 -columnspan 1 -sticky news 
  grid $f.cdvl -column 0 -row 8 -columnspan 1 -sticky news 
  grid $f.cdll -column 0 -row 9 -columnspan 1 -sticky news 
  grid $f.cdtl -column 0 -row 10 -columnspan 1 -sticky news 
  grid $f.rdll -column 0 -row 12 -columnspan 1 -sticky news 
  grid $f.rdtl -column 0 -row 13 -columnspan 1 -sticky news 
  grid $f.rdml -column 0 -row 14 -columnspan 1 -sticky news 

  grid $f.bdae -column 1 -row 1 -columnspan 1 -sticky news 
  grid $f.bdgf -column 1 -row 3 -columnspan 2 -sticky news 
  grid $f.bdgm -column 1 -row 2 -columnspan 2 -sticky news 
  grid $f.bdhe -column 1 -row 4 -columnspan 1 -sticky news 
  grid $f.bdwe -column 1 -row 5 -columnspan 1 -sticky news 
  grid $f.cdve -column 1 -row 8 -columnspan 1 -sticky news 
  grid $f.cdle -column 1 -row 9 -columnspan 1 -sticky news 
  grid $f.cdte -column 1 -row 10 -columnspan 1 -sticky news 
  grid $f.rdle -column 1 -row 12 -columnspan 1 -sticky news 
  grid $f.rdte -column 1 -row 13 -columnspan 1 -sticky news 
  grid $f.rdme -column 1 -row 14 -columnspan 1 -sticky news 

  grid $f.bdau -column 2 -row 1 -columnspan 1 -sticky news 
  grid $f.bdhu -column 2 -row 4 -columnspan 1 -sticky news 
  grid $f.bdwu -column 2 -row 5 -columnspan 1 -sticky news 
  grid $f.cdvu -column 2 -row 8 -columnspan 1 -sticky news 
  grid $f.cdlu -column 2 -row 9 -columnspan 1 -sticky news 
  grid $f.cdtu -column 2 -row 10 -columnspan 1 -sticky news 
  grid $f.rdlu -column 2 -row 12 -columnspan 1 -sticky news 
  grid $f.rdtu -column 2 -row 13 -columnspan 1 -sticky news 
  grid $f.rdmu -column 2 -row 14 -columnspan 1 -sticky news 

  grid $f.s1 -row 6 -column 0 -columnspan 3 -sticky news -pady 3 -padx 3
  grid $f.calc -row 11 -column 0 -columnspan 3 -sticky news -pady 3 -padx 3
  grid $f.cls -row 15 -column 0 -columnspan 3 -sticky news -pady 3 -padx 3

  set xth(bac,bg) [$f.rdle cget -bg]

  set sw [winfo screenwidth .]
  set sh [winfo screenheight .]
  update idletasks
  wm geometry $f -$sw-$sh
  set x [expr {($sw - [winfo width $f])/2}]
  set y [expr {($sh - [winfo height $f])/2}]
  wm geometry $f +$x+$y
  update idletasks
  
}


if 0 {
  package require BWidget
  source ver.tcl
  source global.tcl
  wm withdraw .
  set xth(gui,bacw) .f
  xth_bac_init
} else {
  set xth(bac,age) ""
  set xth(bac,height) ""
  set xth(bac,weight) ""
  set xth(bac,volume) ""
  set xth(bac,level) ""
  set xth(bac,time) ""
}

set xth(bac,gender) 1
set xth(bac,BAC) ""
set xth(bac,ETA) ""
set xth(bac,MRS) ""
  
if 0 {
  set xth(bac,age) "25"
  set xth(bac,height) "193"
  set xth(bac,weight) "90"
  set xth(bac,volume) "500"
  set xth(bac,level) "5.5"
  set xth(bac,time) "0"
}

