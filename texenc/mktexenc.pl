#!/usr/bin/perl -w

# This script generates C++ files and headers containing encoding vectors
# with Unicode values
#
# Copyright (C) 2003 Martin Budaj

sub find_unicode_value {
  my $str = shift;
  my $ch;
  $str = substr ($str,1);   # leading slash is removed
  $str =~ s/\..*//; # Adobe step 1
  $str =~ s/_//;    # a little bit odd handling of ligatures -- cf. Adobe step 2
                    # simplified Adobe step 3 follows

  if (exists $AGL{$str}) {$ch = $AGL{$str};}
  elsif ($str =~ /^uni([0-9a-fA-F]{4})/) {$ch = $1;}  # simplified again
  elsif ($str =~ /^u([0-9a-fA-F]{4,6})$/) {$ch = $1;}
  else {$ch = "0000";}
  
  $USED_CHARS{$ch} = 1;
  return $ch;
}

# we read Adobe Glyph List first

open(IN, "glyphlist.txt") or die "Can't open AGL: $!";
while(<IN>) {
  next if /^#/;
  chomp;
  next if /^$/;
  ($n,$u) = split /;/;
  if (length($u) > 4) {$u = substr($u,0,4)}   # we consider only base character 
  $AGL{$n} = $u;
}
close(IN);

# read User's Glyph List

open(IN, "glyphlist.usr") or die "Can't open UGL: $!";
while(<IN>) {
  next if /^#/;
  chomp;
  next if /^$/;
  ($n,$u) = split /;/;
  if (length($u) > 4) {$u = substr($u,0,4)}   # this shouldn't be used
  $AGL{$n} = $u;
}
close(IN);

# we read the preprocessed Unicode database

open(IN, "basechars.txt") or die "Can't open basechars.txt: $!";
while(<IN>) {
  chomp;
  ($n,$u,$a) = split / /;
  $UNI{$n}{"base"} = $u;
  $UNI{$n}{"acc"} = ($a ? $a : "0000");
}
close(IN);

# read and parse all files with .enc extension in the directory `encodings'

@files = glob("encodings/*.enc");
foreach $f (@files) {
  open (IN, "$f") or die "This can't happen!";
  $i = 0;
  $f =~ /encodings\/(\w+)\./;
  $enc = $1;
  $ENCODINGS{$enc} = 1;
  while(<IN>) {
    next if /^%/;
    s/%.*$//;
    s/^\s+//;
    s/\s+$//;
    @toks = split /\s+/;
    foreach $tok (@toks) {
      if ($i>1 && $i<258) {
        ${$enc}[$i-2] = find_unicode_value($tok);
      }
      $i++;
    }
  }
  if ($i != 260) {die "Can't parse file $f!"};
  close(IN);
}

# find relevant base characters (base character is in some encoding(s))

foreach $composite (keys %UNI) {
  $ch = $composite;
#  if (!exists $USED_CHARS{$ch}) {   # we don't need decompose characters
#                                    # already present in one of the fonts
   TRY:
    while(exists $UNI{$ch}{"base"}) {
      $ch = $UNI{$ch}{"base"};
      if (exists $USED_CHARS{$ch}) {
        $UNI2{$composite}{"base"} = $ch;
        $UNI2{$composite}{"acc"} = $UNI{$composite}{"acc"};
        last TRY;
      }
    }
#  }
}


# finally write C++ files

$maxenc = keys %ENCODINGS;
$maxbase = keys %UNI2;
$banner = "// This file is generated automatically; don't modify it\n\n";

open(OUT, ">../thtexenc.cxx") or die;
print OUT $banner;
print OUT "static const int max_enc = $maxenc;\n";
print OUT "static const int max_base = $maxbase;\n\n";

print OUT "static const char* encodings[$maxenc] = {\n";
foreach $enc (sort keys %ENCODINGS) {
  print OUT "  \"$enc\",\n";
}
print OUT "};\n\n";

print OUT "static const int texenc[256][$maxenc] = {\n";
for ($i=0; $i < 256; $i++) {
  @tmp = ();
  foreach $enc (sort keys %ENCODINGS) {
    push(@tmp,"0x${$enc}[$i]");
  }
  print OUT "  {", join(", ",@tmp), "},\n";
}
print OUT "};\n\n";

print OUT "static const int unibase[$maxbase][3] = {\n";
foreach $ch (sort keys %UNI2) {
  print OUT "  {0x$ch, 0x$UNI2{$ch}{base}, 0x$UNI2{$ch}{acc}},\n";
}
print OUT "};\n";
close OUT;

