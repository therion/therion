#!/usr/bin/perl

%ifiles = (
  'ascii' => 'ASCII.TXT',
  'iso8859-1' => '8859-1.TXT',
  'iso8859-2' => '8859-2.TXT',
  'iso8859-5' => '8859-5.TXT',
  'iso8859-7' => '8859-7.TXT',
  'cp1250' => 'CP1250.TXT',
);

$undef_u = '0X005F';
$undef_c = 'LOW LINE';
$undef_a = '0X5F';


$thchenc_facc = '0X80';
$thchenc_fucc = '0X0080';

$thtt_encoding_uc = "";
$thtt_encoding_lc = "";
$thtt_enum = "\tTT_UNKNOWN_ENCODING = -1,\n";
$thtt_enum_val = -1;

foreach $ename (sort keys %ifiles) {

  $ENAME = uc $ename;
  $ekey = "TT_$ENAME";
  $ekey =~ s/\W/\_/g;
  $fname = $ifiles{$ename}, "\n";
  
  $thtt_encoding_uc .= "\t{\"$ENAME\",\t$ekey},\n";
  $thtt_encoding_lc .= "\t{\"$ename\",\t$ekey},\n";
  $thtt_enum_val++;
  $thtt_enum .= "\t$ekey = $thtt_enum_val,\n";
  print "$fname => $ename, $ENAME, $ekey ... ";
  
  # let's build the structures
  open(INP,"$fname");
  
  $id = $thtt_enum_val;
  LINE: while (<INP>) {
    next LINE if /^\s*\#/;
    
    ($d, $c) = split /\#/;
    $d =~ s/^\s+//;
    $d =~ s/\s+$//;
    $c =~ s/^\s*\#//;
    $c =~ s/^\s+//;
    $c =~ s/\s+$//g;
    
    ($a, $u) = split /\s+/, $d;
    unless (defined $u) {
      $u = $undef_u;
      $c = $undef_c;
    }
    
    $a = uc $a;
    $u = uc $u;
    $c = uc $c;

    $c2u{$c} = $u;
    $u2c{$u} = $c;
    $uu2aa{$u}[$id] = $a;
    $aa2uu{$a}[$id] = $u;
    
    if ($a lt $thchenc_facc) {
      if ($u ge $thchenc_fucc) {
        die "ERROR: You have to use lower facc and fucc !!!\n";
      }
      next LINE;
    }
    
    if ($u ne $undef_u) {
       $u2a{$u}[$id] = $a;
    }
    $a2u{$a}[$id] = $u;
    
  }
  
  close(INP);
  print "done.\n";
  
}

$thtt_enum_val++;
$maxid = $thtt_enum_val;
@ka2u = sort keys %a2u;
@ku2a = sort keys %u2a;
$maxascii = $#ka2u + 1;
$maxunicode = $#ku2a + 1;
$ENAME = 'UTF-8';
$ename = 'utf-8';
$ekey = 'TT_UTF_8';

$thtt_enum = "enum {\n$thtt_enum\t$ekey = $thtt_enum_val\n};\n";
$thtt_encoding = "static const thstok thtt_encoding[] = {\n" .
  "$thtt_encoding_uc\t{\"$ENAME\",\t$ekey},\n" .
  "$thtt_encoding_lc\t{\"$ename\",\t$ekey},\n" .
  "\t{NULL, TT_UNKNOWN_ENCODING}\n};\n";
  
# now let's generate encoding tables
print "processing conversion tables: E$maxid, A$maxascii, U$maxunicode ... ";
$thencode_tbl_def = "static const wchar_t thencode_tbl[][];\n";
$thencode_tbl = "static const wchar_t thencode_tbl[$maxascii][$maxid] = {\n";
foreach $av (@ka2u) {
  @current = @{$a2u{$av}};
  for($i = 0; $i < $maxid; $i++) {
    if ($current[$i] eq '') {
      $current[$i] = $undef_u;
    }
  }
  $thencode_tbl .= "\t{" . join(", ", @current) . "},\n";
}
$thencode_tbl .= "};\n";


# and now let's generate decoding tables
$thdecode_tbl_def = "static const unsigned char thdecode_tbl[][];\n";
$thdecode_tbl_idx_def = "static const wchar_t thdecode_tbl_idx[];\n";
$thdecode_tbl = "static const unsigned char thdecode_tbl[$maxunicode][$maxid] = {\n";
$thdecode_tbl_idx = "static const wchar_t thdecode_tbl_idx[$maxunicode] = {\n";
foreach $uv (@ku2a) {
  @current = @{$u2a{$uv}};
  for($i = 0; $i < $maxid; $i++) {
    
    if ($current[$i] eq '') {  
      $j = 0;
      while ($current[$i] eq '') {
        if ($j == 0) {
          if ($u2c{$uv} =~ /(LATIN (?:CAPITAL|SMALL) LETTER [A-Z])/) {
            $current[$i] = $uu2aa{$c2u{$1}}[$i];
          }
        } else {
          $current[$i] = $undef_a;
        }
        $j++;
      }
    }
  }
  $thdecode_tbl .= "\t{" . join(", ", @current) . "},    //$uv\n";
  $thdecode_tbl_idx .= "\t$uv,\n";
}
$thdecode_tbl_idx .= "};\n";
$thdecode_tbl .= "};\n";
print "done.\n";

  
  
open(OUTPT, ">../thchencdata.cxx");
print OUTPT <<ENDOUTPT;
/**
 * \@file thchencdata.cxx
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */  
 
#ifndef thchencdata_cxx
#define thchencdata_cxx

#include <sys/types.h>


/**
 * First ascii en-de-coded character.
 */
 
static const unsigned char thchenc_facc = $thchenc_facc;
 

/**
 * First unicode en-de-coded character.
 */
 
static const wchar_t thchenc_fucc = $thchenc_fucc;
 

/**
 * Size of decoding table.
 */
 
static const size_t thdecode_tbl_size = $maxunicode;


/**
 * Undefined ascii character.
 */

static const unsigned char thdecode_undef = $undef_a;
 

$thencode_tbl

$thdecode_tbl_idx

$thdecode_tbl

#endif
ENDOUTPT
close(OUTPT);


open(OUTPT, ">../thchencdata.h");
print OUTPT <<ENDOUTPT;
/**
 * \@file thchencdata.h
 * Therion character encodings module data.
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */
  
#ifndef thchencdata_h
#define thchencdata_h

#include \"thparse.h\"


/**
 * Encoding tokens.
 */

$thtt_enum

/**
 * Encoding parsing table.
 */
 
$thtt_encoding

#endif
ENDOUTPT

close(OUTPT);
