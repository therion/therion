#!/usr/bin/perl -w

sub encodecxx {
  $str = shift;
  $str =~ s/\x0D//g;
  $str =~ s/\x0C//g;
  $str =~ s/\x0A//g;
  $str =~ s/\\/\\\\/g;
  $str =~ s/\n/\\n/g;
  $str =~ s/\t/\\t/g;
  $str =~ s/\"/\\"/g;
  return $str;
}

sub init_symbol {
  my $s = shift;
#  if ($s =~ /def\s+((p|l|a)_([a-z]+_){1,2}[A-Z]+)/) {
  if ($s =~ /(def|let)\s+(p|l|a|s)_([a-z_]+)_([A-Z]+)\W/) {
    $SYMBOLS{$2}{$3}{$4} = 1;
  }
#  elsif ($s =~ /beginpattern\(a_([a-z]+)_([A-Z]+)\)/) {
#    $SYMBOLS{"a"}{$1}{$2} = 1;
#  }
}

sub process_symbols {
  my ($s1, $s2, $s3);
  open(OUT, ">../SYMBOLS.txt");
  $thmpost_library .= "\n";
  foreach $s1 (reverse sort keys %SYMBOLS) {
    print OUT "\n\n[", 
          ($s1 eq "p") ? "Point" : (($s1 eq "l") ? "Line" : 
	  (($s1 eq "s") ? "Special" : "Area")), 
              " symbols]\n";
    foreach $s2 (sort keys %{$SYMBOLS{$s1}}) {
      print OUT "\n$s2: ";
      foreach $s3 (sort keys %{$SYMBOLS{$s1}{$s2}}) {
        print OUT "$s3 ";
        $thmpost_library .= "\"initsymbol(\\\"$s1\_$s2\_$s3\\\");\\n\"\n";
      }
    }
  }
  close(OUT);
}

print "reading therion.mp\n";

$thmpost_library = "";
open(INPT,"therion.mp");
while ($ln = <INPT>) {
  init_symbol($ln);
  if ($ln =~ /^\s*input\s+(\S+)\s*\;/) {
    if (open(INPT2,"$1.mp") || open(INPT2,"$1")) {
      print "reading $1\n";
      $fname = $1;
      $thmpost_library .= "\n\"\\n\\n\\n%%%%% INPUT $fname %%%%%\\n\\n\\n\"";
      while($ln2 = <INPT2>) {
        init_symbol($ln2);
        if ($ln2 !~ /\s*endinput\s*\;\s*$/) {
          $thmpost_library .= "\n\"" . encodecxx($ln2) . "\\n\"";
        }
      }
      close(INPT2);
      $thmpost_library .= "\n\"\\n\\n\\n%%%%% ENDINPUT $fname %%%%%\\n\\n\\n\"";
    } else {
      $thmpost_library .= "\n\"" . encodecxx($ln) . "\\n\"";
    }
  } else {
    $thmpost_library .= "\n\"" . encodecxx($ln) . "\\n\"";
  }
}
close(INPT);

process_symbols();

open(OUTPT,">../thmpost.h");
print OUTPT <<ENDOUTPT;
/**
 * \@file thmpost.h
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */  
 
#ifndef thmpost_h
#define thmpost_h

/**
 * Metapost source file.
 */
 
const char * thmpost_library = $thmpost_library;
 

#endif
ENDOUTPT
close(OUTPT);


print "done\n";
