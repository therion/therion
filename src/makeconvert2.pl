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
    if ($fln =~ /endcenterline/) {
      $toconv = 0;
    }
    if ($fln =~ /^\s*centerline\s*$/) {
      $toconv = 0;
    }
    if ($fln =~ /^\s*enddata/) {
      $numch++;
      $fln =~ s/data/centerline/;
    }  
    if ($fln =~ /^\s*data\s*$/) {
      $numch++;
      $fln =~ s/data/centerline/;
    }  
    if ($fln =~ /^\s*data\s*(\s+)$/) {
      if ($1 =~ /^\-/) {
        $numch++;
        $fln =~ s/data/centerline/;
      }
    }  
    push @flso,$fln;
  }
  if ($toconv && ($numch > 0)) {
    print "$fn ($numch changes)\n";
    open(FL,">$fn");
    print FL "@flso";
    close(FL);
  }
}