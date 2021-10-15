set ver 5.4
set suffix -$ver
set incpath "/usr/local/include/vtk-$ver"
set libpath "/usr/local/lib"
set vv1 0
set vv2 0

set searchdirs {/usr /usr/local}
set gccstr ""
catch {
    set gccstr [exec sh -c "pkg-config --cflags freetype2"]
}
set mingwpath ""
if {[regexp {^.*-I(.*)/include/freetype2} $gccstr dum mingwpath]} {
  lappend searchdirs "$mingwpath"
}

foreach d $searchdirs {
    set ll [glob -nocomplain -directory "$d/include" -types d vtk*]
    foreach l $ll {
	if {[regexp {vtk-(\d+)\.(\d+)$} $l dum v1 v2]} {
	    if {($v1 >= $vv1) && ($v2 >= $vv2)} {
		set vv1 $v1
		set vv2 $v2
		set ver $vv1.$vv2
		set suffix -$ver
		set incpath "$d/include/vtk-$vv1.$vv2"
		set libpath "$d/lib"
	    }
	} elseif {[regexp {vtk$} $l]} {
	    if [catch {set fp [open $l/vtkVersionMacros.h]}] {
		set ver "unknown"
	    } else {
		while {[gets $fp line] >= 0} {
		    if {[regexp {VTK_MAJOR_VERSION (\d+)} $line dum v1]} {
			set vv1 $v1
		    } elseif {[regexp {VTK_MINOR_VERSION (\d+)} $line dum v2]} {
			set vv2 $v2
		    }
		}
		close $fp
		set ver $vv1.$vv2
	    }
	    set suffix ""
	    set incpath "$d/include/vtk"
	    set libpath "$d/lib"
	}
    }
}

switch [lindex $argv 0] {
    incpath {
	puts $incpath
    }
    libpath {
	puts $libpath
    }
    version6 {
        puts [expr $vv1 >= 6]
    }
    version9 {
        puts [expr $vv1 >= 9]
    }
    suffix {
	puts $suffix
    }
    libsuffix {
        if {[string length $mingwpath]} {
	  puts $suffix.dll
        } else {
	  puts $suffix
	}
	
    }
    default {
	puts $ver
    }
}
exit
