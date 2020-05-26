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
  my ($s1, $s2, $s3, %ss);
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
	$ss{$s3} = 1;
      }
    }
  }
  close(OUT);
  my $i = 0;
  my $ssl;
  foreach $s3 (sort keys %ss) {
    $ssl .= "\"$s3\",\n";
    $ss{$s3} = $i;
    $i++;
  }

  open(OUT, ">../thsymbolsets.h");
  print OUT "#ifndef thsymbolsets_h\n#define thsymbolsets_h\n\n";
  print OUT "#include \"thsymbolsetlist.h\"\n\n";
  print OUT "#include <map>\n";
  print OUT "#include <string>\n\n";
  print OUT "\n#define thsymsets_size $i\n\n";
  print OUT "extern int thsymsets_symbols [thsymbolset_size][thsymsets_size];\n\n";
  print OUT "extern int thsymsets_figure [thsymbolset_size][thsymsets_size+1];\n\n";
  print OUT "extern int thsymsets_order[thsymbolset_size];\n\n";
  print OUT "extern int thsymsets_count[thsymsets_size];\n\n";
  print OUT "extern std::map<unsigned, std::string> thsymsets_comment;\n\n";
  print OUT "\nstatic const thsymbolset__char_ptr thsymsets [] = {\n$ssl};\n\nvoid thsymsets_symbols_init();\n\n";
  print OUT "#endif\n\n";
  close(OUT);

  open(OUT, ">../thsymbolsets.cxx");
  print OUT "#include \"thsymbolsets.h\"\n\n";
  print OUT "int thsymsets_symbols [thsymbolset_size][thsymsets_size];\n\n";
  print OUT "int thsymsets_figure [thsymbolset_size][thsymsets_size+1];\n\n";
  print OUT "int thsymsets_order[thsymbolset_size];\n\n";
  print OUT "int thsymsets_count[thsymsets_size];\n\n";
  print OUT "std::map<unsigned, std::string> thsymsets_comment;\n\n";
  print OUT "\n\n\nvoid thsymsets_symbols_init() {\n";
  print OUT "\tsize_t i, j;\n\tfor(i = 0; i < thsymbolset_size; i++)\n";
  print OUT "\t\tfor(j = 0; j < thsymsets_size; j++) \n\t\t\tthsymsets_symbols[i][j] = 0;\n\t\t\n\t\n";
  foreach $s1 (reverse sort keys %SYMBOLS) {
    if (($s1 eq "p") || ($s1 eq "a") || ($s1 eq "l")) {
      foreach $s2 (sort keys %{$SYMBOLS{$s1}}) {
        foreach $s3 (sort keys %{$SYMBOLS{$s1}{$s2}}) {
          print OUT "\tthsymsets_symbols[SYM" . uc($s1) . "_" . uc($s2) . "][$ss{$s3}] = 1;\n";
        }
      }
    }
  }
  print OUT "}\n";
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

extern const char * thmpost_library;


#endif
ENDOUTPT
close(OUTPT);

open(OUTPT,">../thmpost.cxx");
print OUTPT <<ENDOUTPT;
/**
 * \@file thmpost.cxx
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */

#include "thmpost.h"

#ifndef THMSVC

const char * thmpost_library = $thmpost_library;

#else

const char * thmpost_library = "input therion;";

#endif

ENDOUTPT
close(OUTPT);

print "done\n";
