set platform LINUX
set instdir /usr/local
if {$argc > 0} {
  set platform [lindex $argv 0]
} else {
  if {[string equal $tcl_platform(platform) windows]} {
    set platform WIN32
  }
}

proc copyfile {force src dst} {
  if {$force} {
    if {[catch { 
      file copy -force -- $src $dst
    }]} {
      puts stderr "installation error: could not write $dst"
    }
  } else {
    if {![file exists $dst]} {
      copyfile 1 $src $dst
    }
  }
}

if {[string equal $platform WIN32]} {
  copyfile 1 therion.exe "c:/Program files/Therion/therion.exe"
  copyfile 1 xtherion/xtherion.tcl "c:/Program files/Therion/xtherion.tcl"
  copyfile 1 loch/loch.exe "c:/Program files/Therion/loch.exe"
} else {
  copyfile 1 therion $instdir/bin/therion
  copyfile 1 xtherion/xtherion $instdir/bin/xtherion
  copyfile 1 loch/loch $instdir/bin/loch
  copyfile 1 therion.ini $instdir/etc/therion.ini.new
  copyfile 1 xtherion/xtherion.ini $instdir/etc/xtherion.ini.new
  copyfile 0 therion.ini $instdir/etc/therion.ini
  copyfile 0 xtherion/xtherion.ini $instdir/etc/xtherion.ini
}
