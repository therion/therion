#!/usr/bin/perl

@ls = `ls -1 -R`;
@flist = ();
$cdir = '.';
foreach $ln (@ls) {
  if ($ln =~ /(.*)\:$/) {
    $cdir = $1;
  } elsif ($ln =~ /\S/) {
    $ln =~ s/^\s+//;
    $ln =~ s/\s+$//;
    if ($ln =~ /(\.th$|\.th2$)/) {
      push @flist, "$cdir/$ln";
    }
  }
}

foreach $fn (@flist) {
  open(FL,"$fn");
  @fls = <FL>;
  close(FL);
  @flso = ();
  $toconv = 1;
  $numch = 0;
  foreach $fln (@fls) {
    if ($fln =~ /\d\s+station\s+\-name\s+\S+/) {
      $toconv = 0;
    }
    if ($fln =~ /\s+\-id\s+\S+/) {
      $toconv = 0;
    }
    if ($fln =~ /(\s+)\-name(\s+\S+)/) {$numch++};
    $fln =~ s/(\s+)\-name(\s+\S+)/\1\-id\2/g;
    if ($fln =~ /(^\s*)name(\s+\S+\s*$)/) {$numch++};
    $fln =~ s/(^\s*)name(\s+\S+\s*$)/\1\id\2/g;
    if ($fln =~ /(\s+)\-station(\s+\S+)/) {$numch++};
    $fln =~ s/(\s+)\-station(\s+\S+)/\1\-name\2/g;
    push @flso,$fln;
  }
  if ($toconv && ($numch > 0)) {
    print "$fn ($numch changes)\n";
    open(FL,">$fn");
    print FL "@flso";
    close(FL);
  }
}