#!/usr/bin/perl
open(I,"mpost/thTrans.mp");
while (<I>) {
  if (/^\s*(\S+)\s+([pla]{1}[a-z_]+)/) {
    $SYMHASH{$2} = "";
  }
}
close(I);
$SYMHASH{l_border_invisible} = "hidden";
$SYMHASH{l_wall_invisible} = "hidden";
$SYMHASH{l_survey} = "noassign";
$SYMHASH{l_undefined} = "noassign";

$SYMHASH{p_undefined} = "noassign";
$SYMHASH{p_camp} = "";
$SYMHASH{p_label} = "noassign";
$SYMHASH{p_remark} = "noassign";
$SYMHASH{p_stationname} = "noassign";
$SYMHASH{p_height} = "noassign";
$SYMHASH{p_date} = "noassign";
$SYMHASH{p_altitude} = "noassign";
$SYMHASH{p_wall_altitude} = "noassign";
$SYMHASH{p_passageheight} = "noassign";
$SYMHASH{p_passageheight_pos} = "noassign";
$SYMHASH{p_passageheight_neg} = "noassign";
$SYMHASH{l_label} = "noassign";

$i = 0;
foreach $k (sort keys(SYMHASH)) {
  $TK .= "  SYM" . uc($k). " = $i,\n";
  $TS .= "  \"SYM" . uc($k). "\",\n";
  $MP .= "  \"$k\",\n";
  $EX .= ($SYMHASH{$k} =~ /noassign/ ? "  false,\n" : "  true,\n");
  $HN .= ($SYMHASH{$k} =~ /hidden/ ? "  true,\n" : "  false,\n");
  $i++;
}

open(O,">thsymbolsetlist.h");
print O <<ENDO;
/**
 * @file thsymbolsetlist.h
 *
 * THIS FILE IS GENERATED AUTOMATICALLY, DO NOT MODIFY IT !!!
 */  

#define thsymbolset_size $i

enum {
$TK};

typedef char * thsymbolset__char_ptr;

static const thsymbolset__char_ptr thsymbolset__mp [] = {
$MP};

static const bool thsymbolset__assign [] = {
$EX};

static const bool thsymbolset__hidden [] = {
$HN};

static const thsymbolset__char_ptr thsymbolset__src [] = {
$TS};

ENDO
close(O);
