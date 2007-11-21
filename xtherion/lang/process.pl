#!/usr/bin/perl
## usage: 
## ./process.pl - generate .tcl sources from xtexts.txt
## ./process.pl update - find new strings to translate and load all 
##   xtexts_xy.txt into xtexts.txt
## ./process.pl export[-empty] xy - export xtexts_xy.txt with texts
##   for translation. If export-empty is used, only untranslated strings
##   are exported

@trans_sources = (
  "../app.tcl",
  "../bac.tcl",
  "../cp.tcl",
  "../cp_procs.tcl",
  "../ctrl.tcl",
  "../me.tcl",
  "../me_cmds.tcl",
  "../me_imgs.tcl",
  "../me_cmds2.tcl",
  "../te.tcl",
  "../help.tcl",
  "../init.tcl",
  "../tools.tcl",
  "../global.tcl",
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

sub str2esc {
  $str = shift;
  $ret = "";
  for($i = 0; $i < length($str); $i++) {
    $co = ord(substr($str,$i,1));
    if (($co > 31) && ($co < 128)) {
      $ret .= chr($co);
    } else {
      $ret .= sprintf("\\%o", $co);
    }
  }
  return $ret;
}


sub write_sources {
  my $href = shift;
  my %hr = %{$href};
  open(OUT,">../msgxth.tcl") || die("error: can't open ../msgxth.tcl for output\n");
  print OUT "# file generated automatically - do not modify!\n\n";

  my %sh = ();
  
  for $key (keys %hr) {
    $sh{$hr{$key}{therion}} = $key;
  }  
  my @nkeys = sort {$a <=> $b} (keys %sh);
  
  for $key (@nkeys) {
    for $lkey (sort keys %{$hr{$sh{$key}}}) {
      if (($lkey !~ /^the/) && ($lkey !~ /\-cmnt/)) {
        print OUT "  ::msgcat::mcset $lkey \"$sh{$key}\" [encoding convertfrom utf-8 \"". str2esc($hr{$sh{$key}}{$lkey}) ."\"]\n";
      }
    }
    print OUT "\n";
  }
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
    $ln =~ s/\[mc\ /\n\[mc\ /sg;
    @lines = split(/\n/,$ln);
    foreach $ln (@lines) {
      $i++;
      if ($ln =~ /^\[mc\ \"(.*)\"/) {
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


$rf = read_lang_file("xtexts.txt");
# ak update
if ($ARGV[0] =~ /^update$/i) {
  opendir(CDR,".");
  @updfiles = grep /^xtexts\_[a-z]{2}\.txt$/, readdir(CDR);
  foreach $ufn (@updfiles) {
    my $lcode = substr($ufn,7,2);
    print "updating $lcode translations ...";
    $rf = read_lang_file($ufn,$lcode,$rf);
    backup_file("$ufn");
    unlink($ufn) || $quietdel || warn("warning: can't delete $ufn\n");
    print " done\n";
  }
  closedir(CDR);
  backup_file("xtexts.txt");
  $rf = update_todo_list($rf);
  write_lang_file("xtexts.txt",$rf);
  write_sources($rf);
} elsif ($ARGV[0] =~ /^export(\-empty)?$/i) {
  my $onlyempty = $1;
  my $lng = check_language($ARGV[1]);
  if (length($lng) != 2) {
    die("error: invalid language \"$ARGV[1]\"\n");
  }
  export_language("xtexts_$lng.txt",$lng,$rf,$onlyempty);
} else {
  write_sources($rf);
}

