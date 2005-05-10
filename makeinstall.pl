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
} else {
  $cdir = `pwd`;
  $cdir =~ s/\s*$//;
  `rm --force /usr/bin/therion`;
  `rm --force /usr/bin/xtherion`;
  `rm --force /etc/therion.ini.new`;
  `rm --force /etc/xtherion.ini.new`;
  `cp --force $cdir/therion /usr/bin/therion`;
  `cp --force $cdir/xtherion/xtherion /usr/bin/xtherion`;
  `cp --force $cdir/therion.ini /etc/therion.ini.new`;
  `cp --force $cdir/xtherion/xtherion.ini /etc/xtherion.ini.new`;
  if ((-s "/etc/therion.ini") <= 0) {
    `cp --force $cdir/therion.ini /etc/therion.ini`;
  }
  if ((-s "/etc/xtherion.ini") <= 0) {
    `cp --force $cdir/xtherion/xtherion.ini /etc/xtherion.ini`;
  }
}

