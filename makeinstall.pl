$platform = $ARGV[0];

if ($platform eq 'WIN32') {
  $cdir = `cd`;
  $cdir =~ s/\s*$//;
  @paths = split(/\;/,$ENV{PATH});
  $tpth = "C:\\WINDOWS";
  foreach $pth (@paths) {
    if (($pth =~ /WINDOWS\s*$/) || ($pth =~ /WINNT\s*$/)) {
      $tpth = $pth;
      $tpth =~ s/\s*$//;
    }
  }
  open OUTPT, ">$tpth/therion.bat";
  print OUTPT "\@\"$cdir\\therion.exe\" %1 %2 %3 %4 %5 %6 %7 %8 %9";
  close OUTPT;
  open OUTPT, ">./xtherion/xtherion.tcl";
  $xthsrc = "source \"$cdir\\xtherion\\xtherion\"\n";
  $xthsrc =~ s/\\/\\\\/g;
  print OUTPT $xthsrc;
  close OUTPT;
} elsif ($platform eq 'MACOSX') {
  $cdir = `pwd`;
  $cdir =~ s/\s*$//;
  `rm --force /usr/bin/therion`;
  `rm --force /usr/bin/xtherion`;
  `cp --force $cdir/therion /usr/bin/therion`;
  `cp --force $cdir/xtherion/xtherion /usr/bin/xtherion`;
} else {
  $cdir = `pwd`;
  $cdir =~ s/\s*$//;
  `ln --force --symbolic $cdir/therion /usr/bin/therion`;
  `ln --force --symbolic $cdir/xtherion/xtherion /usr/bin/xtherion`;
}

