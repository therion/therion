# read make file
open(MF,"Makefile");
@mflines = <MF>;
close(MF);

# backup makefile
open(MFB,">Makefile~");
print MFB @mflines;
close(MFB);

foreach $ln (@mflines) {
  print $ln;
  if ($ln =~ /^\s*\#\s*DEPENDENCIES/) {
    last;
  }
}

