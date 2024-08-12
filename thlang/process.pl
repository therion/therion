#!/usr/bin/perl
## usage:
## ./process.pl - generate .h sources from texts.txt
## ./process.pl update - find new strings to translate and load all
##   texts_xy.txt into texts.txt
## ./process.pl export[-empty] xy - export texts_xy.txt with texts
##   for translation. If export-empty is used, only untranslated strings
##   are exported

@trans_sources = (
  "../thsymbolset.cxx",
  "../thexpmap.cxx",
  "../thlocale.cxx",
  "../thmapstat.cxx",
  "../thpdf.cxx",
);

sub read_lang_file {
  # nacita subor do hashu a vrati na neho referenciu
  my $fn = shift;
  my $lcode = shift;
  my $src = shift;
  open(INP,"$fn") || die("error: can't open $fn for input\n");
  if (defined($lcode)) {
    $lcode = check_language($lcode);
  } else {
    $lcode = "";
  }
  my %res;
  if (defined($src)) {
    %res = %{$src};
  } else {
    %res = ();
  }
  my $itm = "";
  my $lnum = 0;
  my $cmnt = "";
  while ($ln = <INP>) {
    $lnum++;
#    print "LINE: $ln";
    if ($ln =~ /^\s*(\#.*)/) {
      $cmnt .= "$1\n";
#      print "COMMENT\n";
      next;
    } elsif ($ln =~ /^\s*therion\:\s*(.*\S)\s*$/) {
      $itm = $1;
      $res{$itm}{therion} = $lnum;
      $res{$itm}{"therion-cmnt"} = $cmnt;
      $cmnt = "";
#      print "THERION: $itm\n";
    } elsif (($ln =~ /^\s*(\w+)\:\s*(.*\S)\s*$/) && (length($itm) > 0)) {
      ($lng, $trn) = ($1, $2);
      $clng = check_language($lng);
      if (length($lng) == 0) {
        warn("warning: invalid language \"$lng\" ($fn:$lnum)\n");
      }
      if ((length($lcode) == 0) || (substr($clng,0,2) eq $lcode)) {
        $res{$itm}{$clng} = $trn;
        $res{$itm}{"$clng-cmnt"} = $cmnt;
      }
      $cmnt = "";
#      print "LANGUAGE: $clng => $trn\n";
    } elsif ($ln !~ /^\s*$/) {
      warn("warning: error parsing $fn:$lnum\n");
    }
  }
  close(INP);
  return \%res;
}

sub check_language {
  my $lng = shift;
  if ($lng =~ /^([A-Za-z]{2})([\_\-]{1}([A-Za-z]{2})){0,1}$/) {
    if (length($3) > 0) {
      return (lc($1) . "_" . uc($3))
    } else {
      return (lc($1))
    }
  } else {
    return "";
  }
}

sub write_lang_file {
  my $fn = shift;
  my $href = shift;
  my %hr = %{$href};

  open(OUT,">$fn") || die("error: can't open $fn for output\n");

  my %sh = ();
  for $key (keys %hr) {
#    print "SORTING: $key -> $hr{$key}{therion}\n";
    $sh{$hr{$key}{therion}} = $key;
  }

  @nkeys = sort {$a <=> $b} (keys %sh);
  for $key (@nkeys) {
    print OUT $hr{$sh{$key}}{"therion-cmnt"};
    print OUT "therion: $sh{$key}\n";
    for $lkey (sort keys %{$hr{$sh{$key}}}) {
      if (($lkey !~ /^the/) && ($lkey !~ /\-cmnt/)) {
        print OUT $hr{$sh{$key}}{"$lkey-cmnt"};
        print OUT "$lkey: ". $hr{$sh{$key}}{$lkey} ."\n";
      }
    }
    print OUT "\n";
  }

  close(OUT);

}

sub write_sources {
  my $href = shift;
  my %hr = %{$href};

  # vytvorime si zoznam jazykov a ich alternativ
  my %lngs = ();
  my $key;
  my $lkey;
  foreach $key (keys %hr) {
    foreach $lkey (keys %{$hr{$key}}) {
      if (($lkey !~ /^the/) && ($lkey !~ /\-cmnt/)) {
        $lngs{$lkey} = $lkey;
        if (length($lkey) > 2) {
          $nlkey = substr($lkey,0,2);
          $lngs{$nlkey} = $nlkey;
        }
      }
    }
  }

  my @langs = (sort keys %lngs);
  my $i;
  my $lcode;
  $languages = "enum {\n  THLANG_SYSTEM = -2,\n  THLANG_UNKNOWN = -1,\n";
  $langcxxid = "static const thlang_pchar thlang_cxxids []  = {\n";
  $langparse = "static const thstok thtt_lang [] = {\n";
  for ($i = 0; $i <= $#langs; $i++) {
    $lcode = "THLANG_" . uc($langs[$i]);
    $langcxxid .= "  \"$lcode\",\n";
    $langparse .= "  {\"$langs[$i]\", $lcode},\n";
    $languages .= "  $lcode = $i,\n";
    $lngs{$langs[$i]} = $lcode;
  }
  $languages .= "};\n";
  $langcxxid .= "};\n";
  $langparse .= "  {NULL, THLANG_UNKNOWN},\n};\n";

  my $alternatives = "static const int thlang_alternatives [] = {\n";
  for ($i = 0; $i <= $#langs; $i++) {
    $lkey = $langs[$i];
    if ((length($lkey) > 2) && (defined($lngs{substr($lkey,0,2)}))) {
      $alternatives .= "  " . $lngs{substr($lkey,0,2)} . ",\n";
    } else {
      $alternatives .= "  THLANG_UNKNOWN,\n";
    }
  }
  $alternatives .= "};\n";

  @texts = sort keys %hr;

  $textparse = "static const thstok thtt_texts [" . ($#texts + 2) . "] = {\n";
  $texttable = "static thlang_pchar thlang_translations [" . ($#texts + 1) . "][" . ($#langs + 1) . "] = {\n";
  $i = 0;
  my $nlkey;
  foreach $key (@texts) {
    $textparse .= "  {\"$key\",$i},\n";
    $texttable .= "  {\n";
    # priradi ll_LL -> ll ak ll neni definovane
    foreach $lkey (@langs) {
      if (length($lkey) > 2) {
        $nlkey = substr($lkey,0,2);
        if (!defined($hr{$key}{$nlkey})) {
          $hr{$key}{$nlkey} = $hr{$key}{$lkey};
        }
      }
    }
    foreach $lkey (@langs) {
      if (defined($hr{$key}{$lkey})) {
        $texttable .= "    \"$hr{$key}{$lkey}\",\n";
      } else {
        $texttable .= "    NULL,\n";
      }
    }
    $texttable .= "  },\n";
    $i++;
  }
  $textparse .= "  {NULL, -1},\n};\n";
  $texttable .= "};\n";

  # exportujeme thlangdata.h
  open(OUT,">../thlangdata.h") || die("error: can't open thlangdata.h for output\n");
  print OUT <<ENDOUT;
/**
 * \@file thlangdata.h
 * Therion language translations module.
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */
#pragma once

$languages


ENDOUT
  close(OUT);

  # exportujeme thlangdatafields.h
  open(OUT,">../thlangdatafields.h") || die("error: can't open thlangdatafields.h for output\n");
  print OUT <<ENDOUT;
/**
 * \@file thlangdatafields.h
 * Therion language translations module.
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */

#include "thlangdata.h"
#include "thstok.h"
#include <cstddef>

typedef const char * thlang_pchar;


$langparse


$langcxxid


$alternatives


$textparse


$texttable


ENDOUT

  close(OUT);

}


sub update_todo_list {
  my $href = shift;
  my %hr = %{$href};

  # prejde vsetky kluce a priradi im vysoke cisla
  my $maxnm = -1;
  foreach $key (keys %hr) {
    if ($hr{$key}{therion} > $maxnm) {
      $maxnm = $hr{$key}{therion};
    }
  }
  $maxnm = -1 - $maxnm;

  foreach $key (keys %hr) {
    $hr{$key}{therion} += $maxnm;
  }

  my $i = 0;
  foreach $fn (@trans_sources) {
    print "updating definitions from $fn ...";
    open(INP,"$fn") || die("error: can't open $fn for input\n");
    my @lines = <INP>;
    close(INP);
    my $ln = join('',@lines);
    $ln =~ s/thT\(/\nthT\(/sg;
    @lines = split(/\n/,$ln);
    foreach $ln (@lines) {
      $i++;
      if ($ln =~ /^thT\(\"([^"]+)\"/) {
        $hr{$1}{therion} = $i;
      }
    }
    print " done\n";
  }

  #vypise warningy
  foreach $key (keys %hr) {
    if ($hr{$key}{therion} < 0) {
      warn("warning: expression \"$key\" doesn't need to be translated\n");
    }
  }

  return \%hr;
}


sub backup_file {
  my $fn = shift;

  open(INP,"$fn") || die("error: can't open $fn for input\n");
  my @lines = <INP>;
  close(INP);

  open(OUT,">$fn~") || die("error: can't open $fn~ for output\n");
  print OUT @lines;
  close(OUT);

}

sub export_language {
  (my $fn, my $lng, my $href, my $onlyempty) = (shift, shift, shift, shift);
  my %hr = %{$href};
  open(OUT,">$fn") || die("error: can't open $fn for output\n");

  my %sh = ();

  for $key (keys %hr) {
    $sh{$hr{$key}{therion}} = $key;
  }

  my @nkeys = sort {$a <=> $b} (keys %sh);
  for $key (@nkeys) {
    my $toexp = 1;
    if ($onlyempty) {
      for $lkey (sort keys %{$hr{$sh{$key}}}) {
        if ((substr($lkey,0,2) eq $lng) && ($lkey !~ /^the/) && ($lkey !~ /\-cmnt/)) {
          $toexp = 0;
        }
      }
    }
    if ($toexp) {
      print OUT $hr{$sh{$key}}{"therion-cmnt"};
      print OUT "therion: $sh{$key}\n";
      my $posc = "$lng:\n";
      for $lkey (sort keys %{$hr{$sh{$key}}}) {
        if ((substr($lkey,0,2) eq $lng) && ($lkey !~ /^the/) && ($lkey !~ /\-cmnt/)) {
          print OUT $hr{$sh{$key}}{"$lkey-cmnt"};
          print OUT "$lkey: ". $hr{$sh{$key}}{$lkey} ."\n";
          $posc = "";
        }
      }
      print OUT $posc;
      print OUT "\n";
    }
  }

  close(OUT);

}



$rf = read_lang_file("texts.txt");
# ak update
if ($ARGV[0] =~ /^update$/i) {
  opendir(CDR,".");
  @updfiles = grep /^texts\_[a-z]{2}\.txt$/, readdir(CDR);
  foreach $ufn (@updfiles) {
    my $lcode = substr($ufn,6,2);
    print "updating $lcode translations ...";
    $rf = read_lang_file($ufn,$lcode,$rf);
    backup_file("$ufn");
    unlink($ufn) || $quietdel || warn("warning: can't delete $ufn\n");
    print " done\n";
  }
  closedir(CDR);
  backup_file("texts.txt");
  $rf = update_todo_list($rf);
  write_lang_file("texts.txt",$rf);
  write_sources($rf);
} elsif ($ARGV[0] =~ /^export(\-empty)?$/i) {
  my $onlyempty = $1;
  my $lng = check_language($ARGV[1]);
  if (length($lng) != 2) {
    die("error: invalid language \"$ARGV[1]\"\n");
  }
  export_language("texts_$lng.txt",$lng,$rf,$onlyempty);
} else {
  write_sources($rf);
}

