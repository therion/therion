#!/usr/bin/perl
open(I,"mpost/thTrans.mp");
while (<I>) {
  if (/^\s*(\S+)\s+([plas]{1}[a-z_]+)/) {
    $SYMHASH{$2} = "";
  }
}
close(I);

$SYMHASH{a_} = "noassign hidden";
$SYMHASH{a_zzz} = "noassign hidden";

$SYMHASH{l_border_invisible} = "hidden";
$SYMHASH{l_wall_invisible} = "hidden";
# ??? $SYMHASH{l_survey} = "noassign";
$SYMHASH{l_undefined} = "noassign";
$SYMHASH{l_label} = "noassign";
$SYMHASH{l_rope} = "noassign hidden";
$SYMHASH{l_} = "noassign hidden";
$SYMHASH{l_zzz} = "noassign hidden";

$SYMHASH{p_undefined} = "noassign";
$SYMHASH{p_section} = "noassign";
$SYMHASH{p_label} = "noassign";
$SYMHASH{p_remark} = "noassign";
$SYMHASH{p_mapconnection} = "noassign";
$SYMHASH{p_stationname} = "noassign";
$SYMHASH{p_date} = "noassign";
$SYMHASH{p_altitude} = "noassign";
$SYMHASH{p_wallaltitude} = "noassign";
$SYMHASH{p_height_positive} = "noassign";
$SYMHASH{p_height_negative} = "noassign";
$SYMHASH{p_height_unsigned} = "noassign";
$SYMHASH{p_passageheight_positive} = "noassign";
$SYMHASH{p_passageheight_negative} = "noassign";
$SYMHASH{p_passageheight_both} = "noassign";
$SYMHASH{p_passageheight_unsigned} = "noassign";
$SYMHASH{p_surfacestation} = "noassign";
$SYMHASH{p_cavestation} = "noassign";
$SYMHASH{p_entrancestation} = "hidden";
$SYMHASH{p_continuationstation} = "hidden";
$SYMHASH{p_} = "noassign hidden";
$SYMHASH{p_zzz} = "noassign hidden";

$SYMHASH{s_} = "noassign hidden";
$SYMHASH{s_zzz} = "noassign hidden";

$SYMHASH{x_} = "noassign hidden";
$SYMHASH{x_all} = "hidden";
$SYMHASH{x_centerline} = "hidden";
$SYMHASH{x_sections} = "hidden";
$SYMHASH{x_surfacecenterline} = "noassign hidden";
$SYMHASH{x_cavecenterline} = "noassign hidden";

$SYMHASH{x_line_wall} = "hidden";
$SYMHASH{x_line_border} = "hidden";
$SYMHASH{x_line_waterflow} = "hidden";
$SYMHASH{x_line_survey} = "hidden";
$SYMHASH{x_point_waterflow} = "hidden";
$SYMHASH{x_point_station} = "hidden";
$SYMHASH{x_point_height} = "hidden";
$SYMHASH{x_point_passageheight} = "hidden";
$SYMHASH{x_point_airdraught} = "hidden";


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

#ifndef thsymbolsetlist_h
#define thsymbolsetlist_h

#define thsymbolset_size $i

enum {
$TK};

typedef const char * thsymbolset__char_ptr;

static const thsymbolset__char_ptr thsymbolset__mp [] = {
$MP};

static const bool thsymbolset__assign [] = {
$EX};

static const bool thsymbolset__hidden [] = {
$HN};

static const thsymbolset__char_ptr thsymbolset__src [] = {
$TS};

#endif

ENDO
close(O);
