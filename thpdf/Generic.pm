## therion source code
##
## Generic.pm -- therion project file of type perl module
##
## general functions for thpdf program
##
## $Date: 2003/03/10 11:57:26 $
## $RCSfile: Generic.pm,v $
## $Revision: 1.5 $
##
## Copyright (C) 2000 Martin Budaj

# @MODULE
# ThPdf::Generic --
#
# genaral functions used while converting EPS to PDF and writing
# atlas


# @FUNCTION
# floor --
#
# returns floor of the numeric argument

sub floor {                 # floor (0.1) = floor (0.9) = 0
    my $tmp = shift;        # floor (-.1) = floor (-.9) = -1
    if ($tmp >= 0) {
        return int($tmp);
    }
    else {
        return int($tmp) - 1;
    }
}

sub ceil {
  my $tmp = shift;
  return -floor(-$tmp);
}

# @FUNCTION
# coord --
#
# returns numeric argument rounded to one decimal digit (used for
# all coordinates written for pdftex)

sub coord {
    my $tmp = shift;
    $tmp = sprintf "%.1f", $tmp;
    return $tmp;
}

# @FUNCTION
# error --
#
# Produces error message specified as a string argument and ends the program

sub error {
    my $tmp = shift;
    die "\n$tmp: $!\n\n";
}

# @FUNCTION
# read_config --
#
# Reads parameters from configuration file and sets variables in main
# package

sub read_config {
    open(CONF, "Config") or error("Can't open Config file");
    while(<CONF>) {
        chomp;
        if (/^hsize:/i) {$Hsize = units($')};
        if (/^vsize:/i) {$Vsize = units($')};
#        if (/^hoffset: */i) {if ($') {$Hoffset = units($')} 
#                                else {$Hoffset = 0}};
#        if (/^voffset: */i) {if ($') {$Voffset = units($')} 
#                                else {$Voffset = 0}};
        if (/^overlap:/i) {$Overlap = units($')};
        if (/^hgrid:/i) {$Hgrid = units($')};
        if (/^vgrid:/i) {$Vgrid = units($')};
        if (/^hgridoffset: */i) {if ($') {$Hgridoffset = units($')} 
                                else {$Hgridoffset = 0}};
        if (/^vgridoffset: */i) {if ($') {$Vgridoffset = units($')} 
                                else {$Vgridoffset = 0}};
        if (/^background: */i) {$background = $'};
        if (/^startx: */i) {$start_x = $'};
        if (/^starty: */i) {$start_y = $'};
#        if (/^startz: */i) {$start_z = $'};
        if (/^navright: */i) {$nav_x_ext = $'};
        if (/^navup: */i) {$nav_y_ext = $'};
        if (/^navfactor: */i) {$nav_factor = $'};
        if (/^exclpages: */i) {$excl_pages = $'};
        if (/^excllist: */i) {$excl_list = $'};
        if (/^ownpages: */i) {$own_pages = $'};
        if (/^titlepages: */i) {$title_pages = $'};
        if (/^pagenumbering: */i) {$page_numb = $'};
        if (/^transparency: */i) {$transparency = $'};
        if (/^mapgrid: */i) {$map_grid = $'};
    }
    error("Config not correct") unless $Hsize * $Vsize;
    close(CONF);
    $Hoffset = 0;
    $Voffset = 0;
}

# @FUNCTION
# units --
#
# Converts units specified in pt, bp, in, pc, mm, cm, dd, cc to
# PostScript points

sub units ($) {
    my $tmp = shift;
    my ($Num, $Unit);
    $tmp =~ /[a-z]+/;
    $Num = $`; $Unit = $&;
    $Num =~ s/ //g; $Unit =~ s/ //g;
    if ($Unit eq "bp") {return $Num}                    # PostScript points
    elsif ($Unit eq "pt") {return ($Num * 72 / 72.27)}  # points
    elsif ($Unit eq "pc") {return ($Num * 864 / 72.27)} # picas
    elsif ($Unit eq "in") {return ($Num * 72)}          # inches
    elsif ($Unit eq "cm") {return ($Num * 72 / 2.54)}   
    elsif ($Unit eq "mm") {return ($Num * 72 / 25.4)}
    elsif ($Unit eq "dd") {return ($Num * 1.0660111)}   # Didot points
    elsif ($Unit eq "cc") {return ($Num * 12.792133)}   # cicero = 12 dd
    else {error("Unknown length unit in Config file!")}
}

# @FUNCTION
# tex_Xname --
#
# Prints argument with a prefix to get unique tex variable. Used for
# XObject forms

sub tex_Xname {
    my $tmp = shift;
    return "THX$tmp";
}

# @FUNCTION
# tex_Sname --
#
# Prints argument with a prefix to get unique tex variable. Used for
# definitions of map sheets

sub tex_Sname {
    my $tmp = shift;
    return "THS$tmp";
}

# @FUNCTION
# tex_Fname --
#
# Prints argument with a prefix to get unique tex variable. Used for 
# fonts

sub tex_Fname {
    my $tmp = shift;
    return "THF$tmp";
}

# @FUNCTION
# tex_Pname --
#
# Prints argument with a prefix to get unique tex variable. Used for
# patterns

sub tex_Pname {
    my $tmp = shift;
    return "THP$tmp";
}

# @FUNCTION
# tex_Nname --
#
# Prints argument with a prefix to get unique tex variable. Used for
# definition of navigator window

sub tex_Nname {
    my $tmp = shift;
    return "THN$tmp";
}

# @FUNCTION
# tex_Lname --
#
# Prints argument with a prefix to get unique tex variable. Used for
# legend previews

sub tex_Lname {
    my $tmp = shift;
    return "THL$tmp";
}


# @FUNCTION
# lat2_ascii --
#
# Converts string in Latin 2 encoding to first 127 positions of 
# ASCII table. Used for PDF bookmarks. For other languages
# define own functions. Not necessary for Latin 1.

sub lat2_ascii {
    my $tmp = shift;
    $tmp =~ tr [áäèïéìíåµòóôàø¹»úùý¾ÁÄÈÏÉÌÍÅ¥ÒÓÔÀØ©«ÚÙÝ®]
               [aacdeeillnoorrstuuyzAACDEEILLNOORRSTUUYZ];
    return $tmp;
}

sub grid_name {
  my ($x,$d) = @_;
#  $x =~ s/\s//g;
  if ($x =~ /^[0-9]+$/) {
    return $x + $d;
  }
  elsif (length($x)==1 && ord($x) >= 65 && ord($x) <= 90) {
    if (ord($x) + $d >= 65 && ord($x) + $d <= 90) {
      return chr(ord($x) + $d)
    } else {
      return "?";
    }
  }
  elsif (length($x)==1 && ord($x) >= 97 && ord($x) <= 122) {
    if (ord($x) + $d >= 97 && ord($x) + $d <= 122) {
      return chr(ord($x) + $d)
    } else {
      return "?";
    }
  } else {
    return "?";
  }
}
