#!/usr/bin/perl -w

# Parsing and simplification of the Unicode database (character decomposition).
# The file `UnicodeData.txt' is not distributed with Therion. 
# Download it from http://www.unicode.org or don't use this program at all.
# The output is basechars.txt file.
#
# Copyright (C) 2003 Martin Budaj

open(IN, "UnicodeData.txt") or die "Can't open UnicodeData.txt: $!";
open (OUT, ">basechars.txt") or die;
while(<IN>) {
  chomp;
  ($n1,$n,$n,$n,$n,$n6) = split /;/;
  last if (length($n1)>4);                       # Therion doesn't support
                                                 # higher Unicode values
  if ($n6 && !(/<.+> /)) {
    $n6 =~ s/<\w+> //;
#    if (length($n6) > 4) {$n6 = substr($n6,0,4)} # we take only the base char.
    print OUT "$n1 $n6\n";
  }
}
close(IN);
close(OUT);
