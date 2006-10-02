# read make file
open(MF,"Makefile");
@mflines = <MF>;
close(MF);

# backup makefile
open(MFB,">Makefile~");
print MFB @mflines;
close(MFB);

open(MF,">Makefile");
$insidecfg = 0;
$insideprm = 0;
$config = $ARGV[0];
$param = $ARGV[1];
foreach $ln (@mflines) {

  if ($ln =~ /^\s*\#\s*$config\s+CONFIG\s*$/) {
    $insidecfg = 1;
  }

  if ($ln =~ /^\s*\#\s*$config\s+ENDCONFIG\s*$/) {
    $insidecfg = 0;
  }
  
  if ($insidecfg && ($ln =~ /^\s*\#\s*$config\s+$param\s*$/)) {
    $insideprm = 1;
  } elsif ($insidecfg && ($ln =~ /^\s*\#\s*$config\s+\S+\s*$/)) {
    $insideprm = 0;
  }
  
  if ($ln =~ /\S/) {
    if ($insidecfg) {
      if ($insideprm) {
        $ln =~ s/^\s*\#\#//;
      } else {
        if ($ln =~ /^\s*[^\#]/) {
          $ln = "##$ln";
        }
      }
    }
  }
  
  print MF $ln;
  
}
close(MF);
