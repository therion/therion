#!/usr/bin/perl

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

print "reading therion.mp\n";

$thmpost_library = "";
open(INPT,"therion.mp");
while ($ln = <INPT>) {
  if ($ln =~ /^\s*input\s+(\S+)\s*\;/) {
    if (open(INPT2,"$1.mp") || open(INPT2,"$1")) {
      print "reading $1\n";
      $fname = $1;
      $thmpost_library .= "\n\"\\n\\n\\n%%%%% INPUT $fname %%%%%\\n\\n\\n\"";
      while($ln2 = <INPT2>) {
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
