#!/usr/bin/perl -w

open (IN, $ARGV[0]) or die;

while (<IN>) {
  chomp;
  if (/^\W*therion:/) {
    foreach $l (keys %LOC) {
      $GLOB{$l} += 1;
    }
    $GLOB{"therion"} += 1;
    %LOC = ();
  }
  elsif (/^\W*([a-z]{2})(_[A-Z]{2})?:/) {
    $LOC{$1} = 1;
  }
}
foreach $l (keys %LOC) {
  $GLOB{$l} += 1;
}

foreach $l (keys %GLOB) {
  print "$l: " . int(100*$GLOB{$l}/$GLOB{"therion"}) . "\n";
}

