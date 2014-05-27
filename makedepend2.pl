# read make file
open(MF,"Makefile");
@mflines = <MF>;
close(MF);

open(MF,">Makefile");
$ind = 0;
foreach $ln (@mflines) {
  if ($ln =~ /^\s*\#\s*DEPENDENCIES/) {
    $ind = 1;
  }
  if ($ind && ($ln =~ /^\w+\.o\:\s+/)) {
    print MF "\$(OUTDIR)/$ln";
  } else {
    print MF $ln;
  }
}
close(MF);

