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
    if {![regexp {\.(res|png|jpg|jpeg|o|3d|icns|chm|htb)$} $f]} {
      exec dos2unix -p -k -q $f
    } 
  }
}

procdir .
exec chmod 775 ./cclass.pl
exec chmod 775 ./install
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
exec chmod 775 xtherion/bac.tcl
exec chmod 775 xtherion/dbg.tcl 
exec chmod 775 xtherion/bac.tcl 
exec chmod 775 xtherion/source.tcl  
exec chmod 775 xtherion/lang/process.pl
