#!/usr/bin/perl -w

if (scalar @ARGV != 1) {die "Run with .afm file as an argument!";}

$infile = $ARGV[0];
if ($infile =~ /\.afm$/) {
  $infile = $`;
}
else {
  die "Is the file $infile really an AFM file?";
}

open(IN, "$infile.afm") or die "Can't open file $infile.afm: $!";
while(<IN>) {
  chomp;
  if (/C\s+(\d+)\s*;.*;\s*N\s+(\w+)\s*;/) {
    if ($1>=0) {
      $CHAR[$1] = $2;
    }
  }
}
close(IN);



open(OUT, ">$infile.enc") or die "Can't write file $infile.enc: $!";
print OUT "/$infile [\n";
for ($i=0; $i<256; $i++) {
  if (defined $CHAR[$i]) {
    print OUT "/$CHAR[$i]\n";
  }
  else {
    print OUT "/.notdef\n";
  }
}
print OUT "] def\n";
close(OUT);
