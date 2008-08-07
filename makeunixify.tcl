proc procdir {dir} {
  set dirs [glob -nocomplain -directory $dir -types d *]
  set files [glob -nocomplain -directory $dir -types f *]
  foreach d $dirs {
    exec chmod 775 $d
    procdir $d
  }
  puts "\n\n\nDIR: $dir"
  foreach f $files {
    puts $f
    exec chmod 664 $f
    dos2unix $f
  }
}

proc dos2unix {fnm} {
  set doit 0
  if {[regexp {\.(c|cxx|h|tcl|svx|th|thcfg|th2|thc|cof|xpm|txt|pl|mp|tex|TXT|sty|usr|enc|htm|xvi)$} $fnm]} {
    set doit 1
  } elseif {[regexp {(Makefile|TODO\.\w|README|COPYING|CHANGES|thconfig|thconfig\..*)$} $fnm]} {
    set doit 1
  }
  if {!$doit} return;
  set fmtime [file mtime $fnm]
  set fid [open $fnm r]
  fconfigure $fid -translation binary
  set fdt [read $fid]
  close $fid
  if {[regexp {\15} $fdt]} {
    puts "  ...converted to UNIX format"
    regsub -all {\15} $fdt {} fdt
    set fid [open $fnm w]
    fconfigure $fid -translation binary
    puts -nonewline $fid $fdt
    close $fid
    file mtime $fnm $fmtime
  }
}

procdir .
exec chmod 775 ./cclass.pl
exec chmod 775 makeconvert.pl
exec chmod 775 makeconvert2.pl
exec chmod 775 thcsdata.tcl
exec chmod 775 threpair-files
exec chmod 775 thsymbolsetlist.pl
exec chmod 775 geomag/cof2c.pl
exec chmod 775 mpost/genmpost.pl
exec chmod 775 samples/samples.tcl
exec chmod 775 tex/gentex.pl
exec chmod 775 texenc/afm2enc.pl
exec chmod 775 texenc/mktexenc.pl
exec chmod 775 texenc/unidata.pl
exec chmod 775 thchencdata/generate.pl
exec chmod 775 thlang/process.pl
exec chmod 775 thlang/thlang_stats.pl
exec chmod 775 xtherion/bac.tcl
exec chmod 775 xtherion/dbg.tcl 
exec chmod 775 xtherion/bac.tcl 
exec chmod 775 xtherion/source.tcl  
exec chmod 775 xtherion/lang/process.pl
