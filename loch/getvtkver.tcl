set ver 5.4
set incpath "/usr/local/include/vtk-$ver"
set libpath "/usr/local/lib/vtk-$ver"
set vv1 0
set vv2 0

foreach d {/usr /usr/local} {
    set ll [glob -nocomplain -directory "$d/include" -types d vtk*]
    foreach l $ll {
	if {[regexp {vtk-(\d+)\.(\d+)$} $l dum v1 v2]} {
	    if {($v1 >= $vv1) && ($v2 >= $vv2)} {
		set vv1 $v1
		set vv2 $v2
		set ver $vv1.$vv2
		set incpath "$d/include/vtk-$vv1.$vv2"
		set libpath "$d/lib/vtk-$vv1.$vv2"
	    }
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
    default {
	puts $ver
    }
}
exit
