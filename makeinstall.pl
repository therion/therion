$platform = $ARGV[0];

if ($platform eq 'WIN32') {
  `cp -f therion "/c/Program files/Therion"`;
  `cp -f xtherion/xtherion.tcl "/c/Program files/Therion"`;
  `cp -f loch/loch.exe "/c/Program files/Therion"`;
} else {
  $cdir = `pwd`;
  $cdir =~ s/\s*$//;
  `rm -f /usr/local/bin/therion`;
  `rm -f /usr/local/bin/xtherion`;
  `rm -f /usr/local/etc/therion.ini.new`;
  `rm -f /usr/local/etc/xtherion.ini.new`;
  `cp -f $cdir/therion /usr/local/bin/therion`;
  `cp -f $cdir/xtherion/xtherion /usr/local/bin/xtherion`;
  `cp -f $cdir/loch/loch /usr/local/bin/loch`;
  `cp -f $cdir/therion.ini /usr/local/etc/therion.ini.new`;
  `cp -f $cdir/xtherion/xtherion.ini /usr/local/etc/xtherion.ini.new`;
  if ((-s "/usr/local/etc/therion.ini") <= 0) {
    `cp -f $cdir/therion.ini /usr/local/etc/therion.ini`;
  }
  if ((-s "/usr/local/etc/xtherion.ini") <= 0) {
    `cp -f $cdir/xtherion/xtherion.ini /usr/local/etc/xtherion.ini`;
  }
}

