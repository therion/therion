## therion source code
##
## Convert.pm -- therion project file of type perl module
##
## functions for converting EPS to PDF
##
## $Date: 2003/03/10 11:57:26 $
## $RCSfile: Convert.pm,v $
## $Revision: 1.8 $
##
## Copyright (C) 2000-2002 Martin Budaj

# @MODULE
# ThPdf::Convert --
#
# Functions for converting MetaPost output (EPS) to PDF XObjects in 
# pdftex format. Writes data for pdftex to files
# th_fontdef.tex and th_formdef.tex;
# legend to legent.tex and some data structures to file th_hashdef.
# This all is done only when -q option is not used.



sub convert {
  $fontid = "a";
  $pattid = "a";

  print STDOUT "Converting scraps: ";
  read_color_defs();
  print_header();
  foreach $scrapname (keys %SCRAP) {
    print STDOUT "*";
    make_scrap($scrapname);
  }
  make_legend();
  print_postamble();
  read_pattern_defs();
  print_font_info();
  print_legend();
  print_hashes();
  print STDOUT "\n";
}


# @FUNCTION
# read_scraps --
#
# Initializes hashes %SCRAP and %LAYER from file specified as argument.
# Schematic structure, (*) means required:
# <PRE>
#        %SCRAP{name}{F} = scrap content to be clipped by boundary    (*)
#                    {E} = scrap content not to be clipped (walls...) (*)
#                    {G} = file with filled symbols (lakes...)
#                    {X} = file with texts
#                    {B} = file with stroked scrap boundary
#                    {I} = file with filled scrap boundary            (*)
#                    {P} = file with a text clipping path
#                    {C} = file with a scrap boundary clipping path   (*)
#                    {Y} = layer                                      (*)
#                    {V} = level                                      (*)
#                    {S} = shift
#                    {Z} = true for cross-section scraps
#                    {T} = team & date
#                    {D} = author
#                    {L} = links
#
#        %LAYER{name}{U} = layers which should be drawn over this layer
#                    {D} = layers down
#                    {N} = name; placed at the bottom of each page   (*)
#                    {T} = title; place at special page before layer
#                    {Z} = 1 for layers without chapters
#
#        %LEGEND{name}{F} = filename
#                     {N} = description
#
#        %MAP_PREVIEW{U} = list of layers to be displayed over the map
#                    {D} = ... below ...
#
#        $PATTERN_DEFS = directory with patterns definition file
#
# </PRE>
# <I>name</I> of the scrap and legend should consist only from letters (names 
# are used by TeX), <I>name</I> of the layer should be numeric
# value (layers are sorted from bottom to top)

sub read_scraps {
    my $filename = shift;
    do $filename or error ("Can't read scraps file");
}

# @FUNCTION
# make_scrap --
#
# sets $formid and $convert_mode variables and
# calls convert_scrap() function to 
#
# <pre>
#   convert:                  and write to XObject with a name:
#
#    $scrap                        THX$scrap
#    scrap data not to be clipped  THXE$scrap
#    background file               THXG$scrap
#    border                        THXB$scrap
#    filled border                 THXI$scrap
#    texts                         THXX$scrap
# </pre>
#
# (N.B.: with one base scrap name are identified four other files, 
# if they exist)

sub make_scrap {
    my $scrapname = shift;
    if (exists $SCRAP{$scrapname}{F}) {    # scrap contents to be clipped
        $formid = tex_Xname($scrapname);
        $convert_mode = 1;             
        convert_scrap($scrapname,$SCRAP{$scrapname}{F});
    }
    if (exists $SCRAP{$scrapname}{G}) {    # background
        $formid = tex_Xname("G$scrapname");
        $convert_mode = 1.1;
        convert_scrap($scrapname,$SCRAP{$scrapname}{G});
    }
    if (exists $SCRAP{$scrapname}{B}) {    # border
        $formid = tex_Xname("B$scrapname");
        $convert_mode = 1.2;
        convert_scrap($scrapname,$SCRAP{$scrapname}{B});
    }
    if (exists $SCRAP{$scrapname}{I}) {    # filled border
        $formid = tex_Xname("I$scrapname");
        $convert_mode = 1.3;
        convert_scrap($scrapname,$SCRAP{$scrapname}{I});
    }
    if (exists $SCRAP{$scrapname}{E}) {    # nonclipped scrap data
        $formid = tex_Xname("E$scrapname");
        $convert_mode = 1.4;
        convert_scrap($scrapname,$SCRAP{$scrapname}{E});
    }
    if (exists $SCRAP{$scrapname}{X}) {    # texts
        $formid = tex_Xname("X$scrapname");
        $convert_mode = 2;
        convert_scrap($scrapname,$SCRAP{$scrapname}{X});
    }
}

# @FUNCTION
# make_legend --
#
# for all legend symols generates XObject representation

sub make_legend {
    foreach $symbol (keys %LEGEND) {
      $formid = tex_Lname($symbol);
      $convert_mode = 3;
      convert_scrap("", $LEGEND{$symbol}{F});
    }
}

# @FUNCTION
# convert_scrap --
#
# This function does the main task -- EPS to PDF conversion.
# It reads file
# line by line, collects pattern definitions, preprocesses texts (calls
# replace_chars() function) and for converting calls external program
# theps2pdf written in C (to improve the speed). 
# For further use are set hashes %ALL_FONTS and %ALL_PATTERNS;
# information about coordinates of bounding boxes is written
# to hash %COORD. Some other scalar values are defined.
# Schematic structure:
# <pre>
#    %ALL_FONTS{tex_name} = unique identifier
#
#    %ALL_PATTERNS{EPS_pattern_name}{Matrix}
#                                   {BBox}
#                                   {XStep}
#                                   {YStep}
#                                   {Body}
#                                   {ID}
#
#    %COORD{name}{C} = coordinates of lower left corner of the scrap
#                {CU} =  -""- upper right -""-
#                {CG} = background
#                {CB} = border
#                {CI} = fill
#                {CX} = texts (lower left corner)
#                {CXU} = text (upper right corner)
#    </pre>
# 
# Name of the map sheet has form <I>a.b</I>, where <I>a</I> and <I>b</I>
# specify x and y position of the sheet in the imaginary grid.
# For details, see comments
# in function definition.

sub convert_scrap {
    my ($scrapname, $filename) = @_;
    open(IN, $filename) or error("Can't open $filename");
#    $Inbody = $Inpattern = 0; 
    $Inbody = 0; 
    $already_transp = 0;
    $transp_used = 0;
    
  LINE: while(<IN>) {
    chomp;
    if (/^%%BoundingBox: (\S+) (\S+) (\S+) (\S+)/) {
        $llx = $1; $lly = $2; $urx = $3; $ury = $4;
        
        if ($convert_mode < 3) {
          if ($convert_mode == 1) {
              $COORD{$scrapname}{C} = "$llx $lly";
              $COORD{$scrapname}{CU} = "$urx $ury";
          }
          elsif ($convert_mode == 1.1) {
              $COORD{$scrapname}{CG} = "$llx $lly";
              $COORD{$scrapname}{CGU} = "$urx $ury";
          }
          elsif ($convert_mode == 1.2) {
              $COORD{$scrapname}{CB} = "$llx $lly";
              $COORD{$scrapname}{CBU} = "$urx $ury";
          }
          elsif ($convert_mode == 1.3) {
              $COORD{$scrapname}{CI} = "$llx $lly";
              $COORD{$scrapname}{CIU} = "$urx $ury";
          }
          elsif ($convert_mode == 1.4) {
              $COORD{$scrapname}{CE} = "$llx $lly";
              $COORD{$scrapname}{CEU} = "$urx $ury";
          }
          elsif ($convert_mode == 2) {
              $COORD{$scrapname}{CX} = "$llx $lly";
              $COORD{$scrapname}{CXU} = "$urx $ury";
          }
          else {
              error("Unknown convert mode");
          }

        }
        
        $HS = coord($urx-$llx);
        $VS = coord($ury-$lly);
        print PDFTEX "%\n\\setbox\\xxx=\\hbox{\\vbox to${VS}bp{\\vfill\n";
        if ($convert_mode <= 1.1) {   # beginning of boundary cl.path definition
          print PDFTEX "\\PL{q}";     # for F and G scraps
          if (exists $SCRAP{$scrapname}{C}) {        
            open (TMPIN,"$SCRAP{$scrapname}{C}") or error("");
            while(<TMPIN>) {
              chomp;
              @data = split / /;
              print PDFTEX "\\PL{", coord($data[0]-$llx), " ", 
                 coord($data[1]-$lly), " ";
              if ($data[6]) {
                print PDFTEX
                  coord($data[2]-$llx), " ",
                  coord($data[3]-$lly), " ",
                  coord($data[4]-$llx), " ",
                  coord($data[5]-$lly), " ",
                  $data[6], "}%\n";
               }
              else {                                              
                print PDFTEX $data[2], "}%\n";
              }
            }
            close(TMPIN);
	  }
          print PDFTEX "\\PL{W n}";  # end of boundary clipping path definition
        }
        next LINE;
    }
    if (/^%%Page: 1 1$/) {
        $Inbody = 1;
        close(PDFTEX);
        open(PDFTEX, "| theps2pdf $llx $lly 0") or error("Can't open pipe");
        next LINE;
    }
    if (!$Inbody) {next LINE;}
#    if (/^% Pattern:.+:/) {
#        $Inpattern = 1;
#        $curr_pattern = $';
#    }
#    if ($Inpattern and not exists $ALL_PATTERNS{$curr_pattern}{"est"}) {
#        if (/BBox/) {
#            $ALL_PATTERNS{$curr_pattern}{BBox} = $';
#            close(PDFTEX);
#            open(PDFTEX, "| theps2pdf 0 0 1") or error("Can't open pipe");
#        }
#        if (/XStep/) {$ALL_PATTERNS{$curr_pattern}{XStep} = $'}
#        if (/YStep/) {$ALL_PATTERNS{$curr_pattern}{YStep} = $'}
#        if (/makepattern/) {
#            $ALL_PATTERNS{$curr_pattern}{Matrix} = $`;
#            $ALL_PATTERNS{$curr_pattern}{ID} = tex_Pname($pattid);
#            $pattid++;
#        }
#        if (/PaintProc/) {$pattern_body = 1; next LINE;}
#        if (/>>/) {
#            $pattern_body = 0; 
#        }
#        $ALL_PATTERNS{$curr_pattern}{Body} ||= "";
#        if ($pattern_body) {
#            print PDFTEX "$_\n" if $_;
#        }
#    } 
    if (/^showpage$/) {last LINE}
    else {                           # elsif (!$Inpattern) {
        if (/ (\w+) (.+ fshow)/) {
            $fname = $1;
            if (not exists $FORM_FONTS{$fname}) {
                $FORM_FONTS{$fname} = 1;
            }
            if (not exists $ALL_FONTS{$fname}) {
                $ALL_FONTS{$fname} = $fontid;
                $fontid++;
            }
            $texfname = tex_Fname($ALL_FONTS{$fname});
            $text = replace_chars($`);
            used_chars($`);
            $_ = "$text $texfname $2";
        }
	if (/0.00002 0.00018 (\S+) setrgbcolor/) {  # transparency
          $transp_used = 1;
	  if (!$already_transp) {
            print PDFTEX " /GS1 gstate\n";
	    $already_transp = 1;
	  }
	  $_ = "$RGB{$1} setrgbcolor";
	}
	elsif (/setrgbcolor/ || /setgray/) {        # all other occurences
	  if ($already_transp) {                    # terminate transparency
            print PDFTEX " /GS0 gstate\n";
	    $already_transp = 0;
	  }
	}
	if (/0.00002 0.00015 (\S+) setrgbcolor/) {  # patterns
            $pname = $1;
            if (not exists $FORM_PATTERNS{$pname}) {
                $FORM_PATTERNS{$pname} = 1;
            }
            if (not exists $ALL_PATTERNS{$pname}{id}) {
                $ALL_PATTERNS{$pname}{id} = tex_Pname($pattid);
                $pattid++;
            }
            $_ = "$1 setpattern";
        }
        if (/concat/ || /gsave \[/) {
            s/\[//g;
            s/\]//g;
        }
        print PDFTEX "$_\n";
    }
#    if (/^\/.+ exch def$/) {
#        $Inpattern = 0;
#        $FORM_PATTERNS{$curr_pattern} = 1;
#        $ALL_PATTERNS{$curr_pattern}{"est"} = 1;
#
#        close(PDFTEX);      # optimalizovat!!!!!!!!!!!!
#        open(PDFTEX, "| theps2pdf $llx $lly 0") or error("Can't open pipe");
#
#        open(PATTMP, "pat.tmp") or error("Can't open output file");
#        while(<PATTMP>) {
#            /\{(.+)\}/;     # odstranenie \pdfliteral; je to rychlejsie
#            $tmp = $1;
#            $ALL_PATTERNS{$curr_pattern}{Body} .= "$tmp\n";
#        }
#        close(PATTMP);
#        unlink("pat.tmp");
#    }
  }              # end of loop `LINE'

  close(IN);

  if (!$opt_q) { 
    close(PDFTEX);
    open(PDFTEX, ">>th_formdef.tex") or error("Can't open output file");
    if ($convert_mode <= 1.1) {
      print PDFTEX "\\PL{Q}";      # end of boundary cl.path
    }
    print PDFTEX "}}\\wd\\xxx=${HS}bp\n";
    print_fonts_and_patterns();
    print PDFTEX "\\message{*}%\n" if ($convert_mode == 1);
    %FORM_FONTS = ();
    %FORM_PATTERNS = ();
  }
}


# @FUNCTION
# replace_characters --
#
# in PostScript strings replace characters which cannot be processed
# by pdftex directly

sub replace_chars {
    my $tmp = shift;
    $tmp =~ s/\\\\/\\134/g;             
    $tmp =~ s/\\(\d\d\d)/\\string\\$1/g;  # \xxx octal chars
    $tmp =~ s/%/\\%/g;                    # %
    $tmp =~ s/#/\\#/g;                    # #
    $tmp =~ s/\\\(/\\string\\\(/g;        # \(
    $tmp =~ s/\\\)/\\string\\\)/g;        # \)
    $tmp =~ s/\}/\\string \}/g;           # }    overit funkcnost
    $tmp =~ s/\{/\\string \{/g;           # {
    return $tmp;
}

# @FUNCTION
# print_header --
#
# opens output file and prints some necessary definitions

sub print_header {
    open(PDFTEX, ">th_formdef.tex") or error("Can't write output file");
}

# @FUNCTION
# print_postamble --
#
# closes output file

sub print_postamble {
    close(PDFTEX);
}

# @FUNCTION
# print_font_info --
#
# prints to file list of all used fonts, characters and definitions of patterns
# for pdftex

sub print_font_info {
    open(DEFIN, ">th_fontdef.tex");
    foreach $font(sort keys %ALL_FONTS) {
        print DEFIN "\\font\\",tex_Fname($ALL_FONTS{$font}),"=$font\n";
    }
    foreach $font(sort keys %USED_CHARS) {
        print DEFIN "\\pdfincludechars\\",tex_Fname($ALL_FONTS{$font}),"\{";
        for $i(0..255) {
            if ($USED_CHARS{$font}[$i]) {
                if ($i>31) {
                    if ($i==35 or $i==37 or $i==123 or $i==125) {
                        print DEFIN "\\";
                    }
                    print DEFIN chr($i);
                    if ($i==92) {
                        print DEFIN " ";
                    }
                }
                else {
                    print DEFIN "^^", chr($i + 64);
                }
            } 
        }
        print DEFIN "}\n";
    }
    foreach $patt(sort keys %ALL_PATTERNS) {
      if (exists $ALL_PATTERNS{$patt}{id}) {
        print DEFIN "\\immediate\\pdfobj stream attr {/Type /Pattern\n";
        print DEFIN "/PaintType 1 /PatternType 1 /TilingType 1\n";
        print DEFIN "/Matrix $ALL_PATTERNS{$patt}{trans}\n";
        print DEFIN "/BBox $ALL_PATTERNS{$patt}{bbox}\n";
        print DEFIN "/XStep $ALL_PATTERNS{$patt}{xstep}\n";
        print DEFIN "/YStep $ALL_PATTERNS{$patt}{ystep}\n";
        print DEFIN "/Resources << /ProcSet [/PDF ] >> } {0 g 0 G "; # q was 1st
        print DEFIN "$ALL_PATTERNS{$patt}{body}";
        print DEFIN "} \\newcount \\$ALL_PATTERNS{$patt}{id}";    # Q was last
        print DEFIN "\\$ALL_PATTERNS{$patt}{id}=\\pdflastobj\n";
      }
    }
    close(DEFIN);
}

# @FUNCTION
# print_fonts_and_patterns --
#
# When scrap is converted, this function closes definition
# of XObject with listing of used fonts and patterns.

sub print_fonts_and_patterns {
    if (%FORM_FONTS or %FORM_PATTERNS or $transp_used) {
        print PDFTEX "\\immediate\\pdfxform resources { /ProcSet [/PDF /Text] ";
	if ($transp_used) {
	    print PDFTEX "/ExtGState \\the\\resid\\space 0 R ";
	}
        if (%FORM_FONTS) {
            print PDFTEX "/Font << ";
            foreach $font(keys %FORM_FONTS) {
                my $fn = tex_Fname($ALL_FONTS{$font});
                print PDFTEX "/F\\pdffontname\\$fn\\space\\pdffontobjnum\\$fn\\space 0 R ";
            }
            print PDFTEX ">> ";
        }
        if (%FORM_PATTERNS) {
            print PDFTEX "/Pattern << ";
            foreach $patt (keys %FORM_PATTERNS) {
                print PDFTEX "/$patt \\the\\$ALL_PATTERNS{$patt}{id}\\space 0 R ";
            }
            print PDFTEX ">> ";
            print PDFTEX "/ColorSpace << /CS1 [/Pattern /DeviceGray] >> ";
        }
        print PDFTEX "} \\xxx\n\\newcount\\$formid \\$formid=\\pdflastxform\n";
    }
    else {
        print PDFTEX "\\immediate\\pdfxform\\xxx\n\\newcount\\$formid \\$formid=\\pdflastxform\n";
    }        
}

# @FUNCTION
# used_chars --
#
# sets hash %USED_CHARS
#
# <pre>
# %USED_CHARS{font}[array of used characters]
# </pre>

sub used_chars {
    my $tmp = shift;
    $bsl_used = 0;
    if ($tmp =~ /\\\\/) {$bsl_used = 1;}     # backslash is used in text
    $tmp =~ s/\\\\//g;
    $tmp =~ s/\\(\d\d\d)/chr(oct($1))/eg;    # \xxx octal chars
    $tmp =~ s/\\\(/\(/g;
    $tmp =~ s/\\\)/\)/g;
    $tmp = substr $tmp, 1, -1;               # opening and closing parentheses
    $l = length($tmp) - 1;
    for $i(0..$l) {
        $cislo = ord(substr($tmp, $i, 1));
        $USED_CHARS{$fname}[$cislo] ||= 1;
    }
    if ($bsl_used) {$USED_CHARS{$fname}[92] ||= 1;}  # for backslash
}

# @FUNCTION
# print_hashes --
#
# prints contents of hashes %::COORD, %::SHEET and %::MINMAX
# to file for use with -q option, when nothing is converted. 

sub print_hashes {
    open(HSH, ">th_hashdef") or error("Can't open output file");
    print HSH "%COORD = (\n";
    foreach $item (keys %COORD) {
        print HSH " $item => {\n";
        foreach $itt (keys %{$COORD{$item}}) {
            print HSH "  $itt => \"$COORD{$item}{$itt}\",\n";
        }
        print HSH " },\n";
    }
    print HSH ");";
    close(HSH);
}

# @FUNCTION
# print_legend --
#
# Prints contents of hash %LEGEND to file <I>legend.tex</I>. For 
# fine-tuning of legend it's necessary to edit this file manually.

sub print_legend {
  open(LEG, ">legend.tex");
  foreach $symbol (keys %LEGEND) {
    print LEG "\\legendbox{\\",  tex_Lname($symbol),
                          "}{$LEGEND{$symbol}{N}}\n";
  }
  print LEG "\\eject\n";
  close(LEG);
}

sub read_pattern_defs {
 if ($PATTERN_DEFS) {
  open(PAT, "$PATTERN_DEFS/patterns.dat") or die;
  while(<PAT>) {
    chomp;
    ($p_num,$p_file,$p_bbox,$p_xstep,$p_ystep,$p_trans) = split /:/;
    $ALL_PATTERNS{$p_num}{bbox}=$p_bbox;
    $ALL_PATTERNS{$p_num}{xstep}=$p_xstep;
    $ALL_PATTERNS{$p_num}{ystep}=$p_ystep;
    $ALL_PATTERNS{$p_num}{trans}=$p_trans;
    if (exists $ALL_PATTERNS{$p_num}{id}) {
      open(PDFTEX, "| theps2pdf 0 0 1") or error("Can't open pipe");
      open(P_IN, "$PATTERN_DEFS/$p_file") or error("Can't open file for reading");
      while(<P_IN>) {                           
        if (!((/^%/) or (/^showpage$/))) {         # mozno spojit s hlavnym
          print PDFTEX;                            # cyklom
        }
      }
      close(P_IN);
      close(PDFTEX);
      open(PATTMP, "pat.tmp") or error("Can't open output file");
      while(<PATTMP>) {
          /\{(.+)\}/;     # odstranenie \pdfliteral; je to rychlejsie
          $tmp = $1;
          $ALL_PATTERNS{$p_num}{body} .= "$tmp\n";
      }
      close(PATTMP);
    }
    unlink("pat.tmp");
  }
  close(PAT);
 }
}

sub read_color_defs {
  if ($PATTERN_DEFS) {
    open(PAT, "$PATTERN_DEFS/rgbcolors.dat") or die;
    while(<PAT>) {
      chomp;
      ($rgb_num,$rgb_val) = split /:/;
      $RGB{$rgb_num} = "$rgb_val";
    }
    close(PAT);
  }
}  