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
  copyfile 1 ../therion.bin/therion.exe "c:/Program files/Therion/therion.exe"
  copyfile 1 ../therion.bin/xtherion/xtherion.tcl "c:/Program files/Therion/xtherion.tcl"
  copyfile 1 ../therion.bin/loch/loch.exe "c:/Program files/Therion/loch.exe"
} elseif {[string equal $platform MACOSX]} {
  copyfile 1 therion /usr/local/bin/therion
  file attributes /usr/local/bin/therion -permissions 0755
  copyfile 1 xtherion/xtherion /usr/local/bin/xtherion
  file attributes /usr/local/bin/xtherion -permissions 0755
  file delete -force /Applications/loch.app
  copyfile 1 loch/loch.app /Applications
  file attributes /Applications/loch.app -permissions 0755
  file attributes /Applications/loch.app/Contents -permissions 0755
  file attributes /Applications/loch.app/Contents/Info.plist -permissions 0644
  file attributes /Applications/loch.app/Contents/PkgInfo -permissions 0644
  file attributes /Applications/loch.app/Contents/MacOS -permissions 0755
  file attributes /Applications/loch.app/Contents/MacOS/loch -permissions 0755
  file attributes /Applications/loch.app/Contents/Resources -permissions 0755
  file attributes /Applications/loch.app/Contents/Resources/loch.icns -permissions 0644
  copyfile 1 therion.ini /etc/therion.ini.new
  file attributes /etc/therion.ini.new -permissions 0644
  copyfile 1 xtherion/xtherion.ini /etc/xtherion.ini.new
  file attributes /etc/xtherion.ini.new -permissions 0644
  copyfile 0 therion.ini /etc/therion.ini
  file attributes /etc/therion.ini -permissions 0644
  copyfile 0 xtherion/xtherion.ini /etc/xtherion.ini
  file attributes /etc/xtherion.ini -permissions 0644
} else {
  copyfile 1 therion $instdir/bin/therion
  copyfile 1 xtherion/xtherion $instdir/bin/xtherion
  copyfile 1 loch/loch $instdir/bin/loch
  copyfile 1 therion.ini $instdir/etc/therion.ini.new
  copyfile 1 xtherion/xtherion.ini $instdir/etc/xtherion.ini.new
  copyfile 0 therion.ini $instdir/etc/therion.ini
  copyfile 0 xtherion/xtherion.ini $instdir/etc/xtherion.ini
}
