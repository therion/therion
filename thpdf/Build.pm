## therion source code
##
## Build.pm -- therion project file of type perl module
##
## functions for building PDF atlas
##
## $Date: 2003/03/10 11:57:26 $
## $RCSfile: Build.pm,v $
## $Revision: 1.8 $
##
## Copyright (C) 2000-2002 Martin Budaj

# @MODULE
# ThPdf::Build --
#
# Functions for building 
#  1) PDF atlas with hyperlinks, outline, position navigator and 
#     cross-references
#  2) one-page map

sub build {
  read_config();  # for map mode only Transparency setting is used

#  if (!$opt_m) {
#    print STDOUT "Building pages: ";
#  } else {
#    print STDOUT "Building map...";
#  }

#  shift_coords();
  make_sheets();

  open(PAGEDEF, ">th_pagedef.tex") or error();
  open(PDFRES, ">th_resources.tex") or error();
  open(PAGE, ">th_pages.tex") or error();

  if (!$opt_m) {
    find_excluded_pages();
    sort_sheets();
    find_jumps();
  }

  build_pages();

  close(PAGEDEF);
  close(PDFRES);
  close(PAGE);
}

sub shift_coords {
  foreach $item (keys %COORD) {
    foreach $itt (keys %{$COORD{$item}}) {
      if (exists $SCRAP{$item}{S}) {
        my ($dx,$dy) = split / /, $SCRAP{$item}{S};
        my ($x,$y) = split / /, $COORD{$item}{$itt};
	$x += $dx;
	$y += $dy;
	$COORD{$item}{$itt} = "$x $y";
      }
    }
  }
}

#    %SHEET{layer}{name}{scraps} = level scrap level scrap &c.

sub make_sheets {
 foreach $scrapname (keys %SCRAP) {
  undef $llx; undef $lly; undef $urx; undef $ury;
  if (exists $SCRAP{$scrapname}{F}) {
    ($ellx, $elly) = split / /, $COORD{$scrapname}{C};
    ($eurx, $eury) = split / /, $COORD{$scrapname}{CU};
    if (defined $llx) {if ($ellx<$llx) {$llx=$ellx}} else {$llx=$ellx}
    if (defined $lly) {if ($elly<$lly) {$lly=$elly}} else {$lly=$elly}
    if (defined $urx) {if ($eurx>$urx) {$urx=$eurx}} else {$urx=$eurx}
    if (defined $ury) {if ($eury>$ury) {$ury=$eury}} else {$ury=$eury}
  }
  if (exists $SCRAP{$scrapname}{E}) {
    ($ellx, $elly) = split / /, $COORD{$scrapname}{CE};
    ($eurx, $eury) = split / /, $COORD{$scrapname}{CEU};
    if (defined $llx) {if ($ellx<$llx) {$llx=$ellx}} else {$llx=$ellx}
    if (defined $lly) {if ($elly<$lly) {$lly=$elly}} else {$lly=$elly}
    if (defined $urx) {if ($eurx>$urx) {$urx=$eurx}} else {$urx=$eurx}
    if (defined $ury) {if ($eury>$ury) {$ury=$eury}} else {$ury=$eury}
  }
  if (exists $SCRAP{$scrapname}{G}) {
    ($ellx, $elly) = split / /, $COORD{$scrapname}{CG};
    ($eurx, $eury) = split / /, $COORD{$scrapname}{CGU};
    if (defined $llx) {if ($ellx<$llx) {$llx=$ellx}} else {$llx=$ellx}
    if (defined $lly) {if ($elly<$lly) {$lly=$elly}} else {$lly=$elly}
    if (defined $urx) {if ($eurx>$urx) {$urx=$eurx}} else {$urx=$eurx}
    if (defined $ury) {if ($eury>$ury) {$ury=$eury}} else {$ury=$eury}
  }
  if (exists $SCRAP{$scrapname}{B}) {
    ($ellx, $elly) = split / /, $COORD{$scrapname}{CB};
    ($eurx, $eury) = split / /, $COORD{$scrapname}{CBU};
    if (defined $llx) {if ($ellx<$llx) {$llx=$ellx}} else {$llx=$ellx}
    if (defined $lly) {if ($elly<$lly) {$lly=$elly}} else {$lly=$elly}
    if (defined $urx) {if ($eurx>$urx) {$urx=$eurx}} else {$urx=$eurx}
    if (defined $ury) {if ($eury>$ury) {$ury=$eury}} else {$ury=$eury}
  }
  if (exists $SCRAP{$scrapname}{I}) {
    ($ellx, $elly) = split / /, $COORD{$scrapname}{CI};
    ($eurx, $eury) = split / /, $COORD{$scrapname}{CIU};
    if (defined $llx) {if ($ellx<$llx) {$llx=$ellx}} else {$llx=$ellx}
    if (defined $lly) {if ($elly<$lly) {$lly=$elly}} else {$lly=$elly}
    if (defined $urx) {if ($eurx>$urx) {$urx=$eurx}} else {$urx=$eurx}
    if (defined $ury) {if ($eury>$ury) {$ury=$eury}} else {$ury=$eury}
  }
  if (exists $SCRAP{$scrapname}{X}) {
    ($ellx, $elly) = split / /, $COORD{$scrapname}{CX};
    ($eurx, $eury) = split / /, $COORD{$scrapname}{CXU};
    if (defined $llx) {if ($ellx<$llx) {$llx=$ellx}} else {$llx=$ellx}
    if (defined $lly) {if ($elly<$lly) {$lly=$elly}} else {$lly=$elly}
    if (defined $urx) {if ($eurx>$urx) {$urx=$eurx}} else {$urx=$eurx}
    if (defined $ury) {if ($eury>$ury) {$ury=$eury}} else {$ury=$eury}
  }
  
  if (not defined $llx) {return;}
  
  $curr_layer = $SCRAP{$scrapname}{Y};
  $curr_level = $SCRAP{$scrapname}{V};

  if (!$opt_m) {
    $Llx = floor(($llx-$Overlap-$Hoffset) / $Hsize);
    $Lly = floor(($lly-$Overlap-$Voffset) / $Vsize);
    $Urx = floor(($urx+$Overlap-$Hoffset) / $Hsize);
    $Ury = floor(($ury+$Overlap-$Voffset) / $Vsize);


    for ($i = $Llx; $i <= $Urx; $i++) {
      for ($j = $Lly; $j <= $Ury; $j++) {
        if (not exists $LAYER{$curr_layer}{Z}) {
          $SHEET{$curr_layer}{"$i.$j"}{"scraps"} .= " $curr_level $scrapname";
          if (not exists $MINMAX{$curr_layer}{min_x}) {$MINMAX{$curr_layer}{min_x} = $Llx;}
          if (not exists $MINMAX{$curr_layer}{max_x}) {$MINMAX{$curr_layer}{max_x} = $Urx;}
          if (not exists $MINMAX{$curr_layer}{min_y}) {$MINMAX{$curr_layer}{min_y} = $Lly;}  
          if (not exists $MINMAX{$curr_layer}{max_y}) {$MINMAX{$curr_layer}{max_y} = $Ury;}
          if ($MINMAX{$curr_layer}{min_x} > $Llx) {$MINMAX{$curr_layer}{min_x} = $Llx;}
          if ($MINMAX{$curr_layer}{max_x} < $Urx) {$MINMAX{$curr_layer}{max_x} = $Urx;}
          if ($MINMAX{$curr_layer}{min_y} > $Lly) {$MINMAX{$curr_layer}{min_y} = $Lly;}
          if ($MINMAX{$curr_layer}{max_y} < $Ury) {$MINMAX{$curr_layer}{max_y} = $Ury;}
	} else {
          $PREVIEW{$curr_layer}{"$i.$j"}{"scraps"} .= " $curr_level $scrapname";
	}
      }
    }
  } else {
    $LAYER{$curr_layer}{"scraps"} .= " $curr_level $scrapname";
    if (not exists $LAYER{$curr_layer}{Z}) {
      if (not defined $MINX) {$MINX = $llx;}
      if (not defined $MAXX) {$MAXX = $urx;}
      if (not defined $MINY) {$MINY = $lly;}
      if (not defined $MAXY) {$MAXY = $ury;}
      if ($MINX > $llx) {$MINX = $llx;}
      if ($MAXX < $urx) {$MAXX = $urx;}
      if ($MINY > $lly) {$MINY = $lly;}
      if ($MAXY < $ury) {$MAXY = $ury;}
    }
  }
 }
}


# @FUNCTION
# find_excluded_pages --
#
# converts string $excl_list (specified in Config) to hash, where
# keys are names of excluded pages

sub find_excluded_pages {
    if (!$excl_pages) {return;}
    my @list = split /,/, $excl_list;
    my $tot_max = 0;
    foreach $item (@list) {
        if ($item =~ /-/) {
            my ($min,$max) = split /-/, $item;
            for $i ($min..$max) {
###                $i -= $own_pages;
                $EXCL{$i} = 1;
                if ($i > $tot_max) {$tot_max = $i;}
            }
        }
        else {
###            $item -= $own_pages;
            $EXCL{$item} = 1;
            if ($item > $tot_max) {$tot_max = $item;}
        }
    }
    $j = "aaaaa";
    for $i (1..$tot_max) {
        if ($EXCL{$i}) {
            $EXCLUDED{$j} = 1;            
        }
        $j++;
    }
    %EXCL = ();
}

# @FUNCTION
# sort_sheets --
#
# for each layer scans all possible sheets (between (min_x,max_y) and
# (max_x,min_y)) and marks used sheets. Also sets %BOOKMARKS hash to
# point to first sheet of each layer

sub sort_sheets {
# $name_z = $start_z;
  %BOOKMARKS = ();
  $pageid = "aaaaa";
  $pageno = 1 + $own_pages;
  foreach $curr_layer(reverse sort {$a <=> $b} keys %SHEET) {  # layers are numbered from bottom
    if ((exists $LAYER{$curr_layer}{T}) and $title_pages) {
      $SHEETNAME{$curr_layer}{$pageid} = "TITLE";
      if (! exists $EXCLUDED{$pageid}) {
##       $REFS{$pageid} = $pageno;
        $pageno++;
      }
      $pageid++;
    }
#   $name_y = $start_y;
    for ($j=$MINMAX{$curr_layer}{max_y}; 
    	 $j>=$MINMAX{$curr_layer}{min_y}; 
	 $j--) {
#     $name_x = $start_x;
      for ($i=$MINMAX{$curr_layer}{min_x}; 
           $i<=$MINMAX{$curr_layer}{max_x}; 
	   $i++) {
        $mapsheet = "$i.$j";
        if (exists $SHEET{$curr_layer}{$mapsheet}) {
          $SHEETNAME{$curr_layer}{$pageid} = $mapsheet;
          $SHEET{$curr_layer}{$mapsheet}{"name"} = $pageid;
          if (! exists $EXCLUDED{$pageid}) {
            $REFS{$pageid} = $pageno;
            $pageno++;
#           $SHEET{$curr_layer}{$mapsheet}{"sign"} = 
#                     "$name_y $name_x $name_z";
            if (not $BOOKMARKS{$curr_layer}) {
              print PAGE
                    "\\pdfoutline goto name {$pageid} count 0 {",
                    lat2_ascii($LAYER{$curr_layer}{N}),"}%\n";
              $BOOKMARKS{$curr_layer} = 1;
              $SHEET{$curr_layer}{$mapsheet}{dest} = 1;        
            }
          }
          $pageid++;
        }
#       $name_x++;
      }
#     $name_y++;
    }
#   $name_z++;
  }
}

# @FUNCTION
# find_jumps --
#
# for each sheets checks adjoining sheets and adds links and destination info
# to found sheets


sub find_jumps {
    foreach $curr_layer (keys %SHEET) {
      foreach $mapsheet (keys %{$SHEET{$curr_layer}}) {
        $W = move_mapsheet($mapsheet, -1, 0);
        $E = move_mapsheet($mapsheet, 1, 0);
        $N = move_mapsheet($mapsheet, 0, 1);
        $S = move_mapsheet($mapsheet, 0, -1);
        if ((exists $SHEET{$curr_layer}{$W}) && 
          (! exists $EXCLUDED{$SHEET{$curr_layer}{$W}{name}})) {
            $SHEET{$curr_layer}{$mapsheet}{jumpW} = 
                $SHEET{$curr_layer}{$W}{name}; 
            $SHEET{$curr_layer}{$W}{dest} = 1;
        }
        if ((exists $SHEET{$curr_layer}{$E}) &&
          (! exists $EXCLUDED{$SHEET{$curr_layer}{$E}{name}})) {
            $SHEET{$curr_layer}{$mapsheet}{jumpE} = 
                $SHEET{$curr_layer}{$E}{name}; 
            $SHEET{$curr_layer}{$E}{dest} = 1;
        }
        if ((exists $SHEET{$curr_layer}{$N}) &&
          (! exists $EXCLUDED{$SHEET{$curr_layer}{$N}{name}})) {
            $SHEET{$curr_layer}{$mapsheet}{jumpN} = 
                $SHEET{$curr_layer}{$N}{name}; 
            $SHEET{$curr_layer}{$N}{dest} = 1;
        }
        if ((exists $SHEET{$curr_layer}{$S}) &&
          (! exists $EXCLUDED{$SHEET{$curr_layer}{$S}{name}})) {
            $SHEET{$curr_layer}{$mapsheet}{jumpS} = 
                $SHEET{$curr_layer}{$S}{name}; 
            $SHEET{$curr_layer}{$S}{dest} = 1;
        }
        if (exists $LAYER{$curr_layer}{D}) {
            my @layer = split / /, $LAYER{$curr_layer}{D};
#            my $layer = shift @layer;
#            if ((exists $SHEET{$layer}{$mapsheet}) &&
#              (! exists $EXCLUDED{$SHEET{$layer}{$mapsheet}{name}})) {
#                $SHEET{$curr_layer}{$mapsheet}{jumpD} = 
#                    $SHEET{$layer}{$mapsheet}{name}; 
#                $SHEET{$layer}{$mapsheet}{dest} = 1;        
#            }
            foreach my $layer (@layer) {
              if ((exists $SHEET{$layer}{$mapsheet}) &&
                (! exists $EXCLUDED{$SHEET{$layer}{$mapsheet}{name}})) {
                  $SHEET{$curr_layer}{$mapsheet}{jumpD}{$layer} = 
                      $SHEET{$layer}{$mapsheet}{name}; 
                  $SHEET{$layer}{$mapsheet}{dest} = 1;        
              }
	    }
        }
        if (exists $LAYER{$curr_layer}{U}) {
            my @layer = split / /, $LAYER{$curr_layer}{U};
#            my $layer = shift @layer;
#            if ((exists $SHEET{$layer}{$mapsheet}) &&
#              (! exists $EXCLUDED{$SHEET{$layer}{$mapsheet}{name}})) {
#                $SHEET{$curr_layer}{$mapsheet}{jumpU} = 
#                    $SHEET{$layer}{$mapsheet}{name}; 
#                $SHEET{$layer}{$mapsheet}{dest} = 1;        
#            }
            foreach my $layer (@layer) {
              if ((exists $SHEET{$layer}{$mapsheet}) &&
                (! exists $EXCLUDED{$SHEET{$layer}{$mapsheet}{name}})) {
                  $SHEET{$curr_layer}{$mapsheet}{jumpU}{$layer} = 
                      $SHEET{$layer}{$mapsheet}{name}; 
                  $SHEET{$layer}{$mapsheet}{dest} = 1;        
              }
	    }
        }
      }
    }
}

# @FUNCTION
# build_pages --
#
# for all pages calls routines necessary for composing this page.<BR>
# NB: by page we mean complex page composed from map sheet,
# navigator window, texts etc.

sub build_pages {
  if ($transparency) {
    print PDFRES "\\ifnum\\pdftexversion<110\\pdfcatalog{/Version /1.4}\\fi\n";
    print PDFRES "\\immediate\\pdfobj{ << /GS0 ",
                 "<< /Type /ExtGState /ca 1 /BM /Normal >> ",
           " /GS1 << /Type /ExtGState /ca \\the\\opacity\\space /BM /Normal >> >> }\n";
    print PDFRES "\\newcount\\resid\\resid=\\pdflastobj\n";
    print PDFRES "\\immediate\\pdfobj{ << /S /Transparency /K true >> }\n";
    print PDFRES "\\newcount\\attrid\\attrid=\\pdflastobj\n";
  } else {
    print PDFRES "\\immediate\\pdfobj{ << /GS0 ",
                 "<< /Type /ExtGState >> ",
           " /GS1 << /Type /ExtGState >> >> }\n";
    print PDFRES "\\newcount\\resid\\resid=\\pdflastobj\n";
  }
  if (!$opt_m) {
    $HS = coord($Hsize + 2*$Overlap);
    $VS = coord($Vsize + 2*$Overlap);
    $exactHS = sprintf "%.6f", $Hsize + 2*$Overlap;
    $exactVS = sprintf "%.6f", $Vsize + 2*$Overlap;
    if ($page_numb) {
      print PAGE "\\pagenumberingtrue\n";
    }
  } else {
    if ($map_grid) {
      $MINX = $Hsize * floor ($MINX/$Hsize);
      $MINY = $Vsize * floor ($MINY/$Vsize);
      $MAXX = $Hsize * ceil ($MAXX/$Hsize);
      $MAXY = $Vsize * ceil ($MAXY/$Vsize);
    }
    $HS=coord($MAXX-$MINX);
    $VS=coord($MAXY-$MINY);
    if ($HS>14000 || $VS>14000) 
      {error("Map is too large (about 5m) for PDF format. Try another scale!")};
    print PAGEDEF "\\eject\n";
    print PAGEDEF "\\hsize=${HS}bp\n";
    print PAGEDEF "\\vsize=${VS}bp\n";
    print PAGEDEF "\\pdfpagewidth=\\hsize\\advance\\pdfpagewidth by ",
                                             coord(2*$Overlap), "bp\n";
    print PAGEDEF "\\pdfpageheight=\\vsize\\advance\\pdfpageheight by ",
                                             coord(2*$Overlap), "bp\n";
    print PAGEDEF "\\hoffset=0cm\n";
    print PAGEDEF "\\voffset=0cm\n";
    print PAGEDEF "\\pdfhorigin=",coord($Overlap),"bp\n";
    print PAGEDEF "\\pdfvorigin=",coord($Overlap),"bp\n";
#    if ($transparency) {
#      print PAGEDEF "\\pdfcatalog{/Version /1.4}";
#      print PAGEDEF "\\def\\pres#1{\\edef\\act{\\pdfpageresources=",
#                  "{/ExtGState #1 0 R}}\\act}";
#      print PAGEDEF "\\immediate\\pdfobj{ << /GS1 ",
#                  "<< /Type /ExtGState /ca \\the\\opacity\\space /BM /Normal >> >> }\n";
#      print PAGEDEF "\\pres{\\the\\pdflastobj}\n";
#      print PAGEDEF "\\immediate\\pdfobj{ << /S /Transparency /K true >> }\n";
#      print PAGEDEF "\\newcount\\attrid\\attrid=\\pdflastobj\n";
#    }
  }

  if (!$opt_m) {
    foreach $curr_layer (reverse sort {$a <=> $b} keys %SHEET) {
      foreach $nickname (sort keys %{$SHEETNAME{$curr_layer}}) {
        $mapsheet = $SHEETNAME{$curr_layer}{$nickname};
        if (! exists $EXCLUDED{$nickname}) {
          if ($mapsheet eq "TITLE") {
            print_title_page();
          }
          else {
            print_sheet_preamble();
            print PAGEDEF "\\PL{0.9 g 0 0 $HS $VS re f}%\n" if $background;
            print_map();
            print_grid();   # zmenit definiciu fcie
            print_sheet_postamble();
  
            print_navigator();
  
            compose_page();
          }
#          print "*";
        }
      }
    }
  } else {
    $HSHIFT = $MINX;
    $VSHIFT = $MINY;
    print PAGEDEF "\\setbox\\xxx=\\hbox to ${HS}bp{%\n";
    if (exists $MAP_PREVIEW{D}) {
      print_preview_down();
    }
    foreach $curr_layer (sort {$a <=> $b} keys %LAYER) {
      if (not exists $LAYER{$curr_layer}{Z}) {
        print_map();
      }
    }
    if (exists $MAP_PREVIEW{U}) {
      print_preview_up();
    }
    if ($map_grid) {
      print_map_grid();
    }
    print PAGEDEF "\\setbox\\xxx=\\hbox{";                     # map legend
    print PAGEDEF "\\vbox to ${VS}bp{\\maplegend\\vfill}}\n";
    print PAGEDEF "\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}\n";

    print PAGEDEF "\\hfill}\\ht\\xxx=${VS}bp\\dp\\xxx=0bp\n";
#    print PAGEDEF "\\box\\xxx\n";
    print PAGEDEF "\\pdfxform\\xxx\\PB{0}{0}{\\pdflastxform}%\n";
  }
  print "\n";
}

# @FUNCTION
# move_mapsheet --
#
# returns the name of mapsheet shifted in both axes from base sheet

sub move_mapsheet {
    my ($sheet, $a, $b) = @_;
    my ($x, $y) = split /\./, $sheet;
    $x += $a;
    $y += $b;
    return "$x.$y";
}

# @FUNCTION
# print_title_page --
#
# prints Title before new layer to special page

sub print_title_page {
    print PAGE "\\TITLE{$LAYER{$curr_layer}{T}}\n";
}

# @FUNCTION
# print_sheet_preamble --
#
# begins map sheet definition

sub print_sheet_preamble {
    print PAGEDEF "\\setbox\\xxx=\\hbox to ${HS}bp{%\n";
}

# @FUNCTION
# print_sheet_postamble --
#
# closes map sheet definition

sub print_sheet_postamble {
    print PAGEDEF "\\hfill}\\ht\\xxx=${VS}bp\\dp\\xxx=0bp\n";
    print PAGEDEF "\\immediate\\pdfxform";
#    if ($transparency) {
#      print PAGEDEF " resources{/ExtGState \\the\\resid\\space 0 R} ";
#    }
    print PAGEDEF "\\xxx\\newcount\\", tex_Sname($nickname), 
            " \\", tex_Sname($nickname), "=\\pdflastxform\n";
}

# @FUNCTION
# print_map --
#
# Puts together scraps which occur on this page. Simplified algorithm:
# <OL>
#    <LI>for current layer, top and bottom layers are ascertained
#    <LI>scraps from bottom layer(s) are filled gray
#    <LI>current layer may contain more levels; all are processed
#        separately. Each level forms a non-isolated knockout transparency
#        group, if transparency is enabled
#    <LI>for current level is determined text clipping path -- it's put
#        together from all clipping paths around texts in this and
#        upper levels of the current layer 
#    <LI>for each scrap in current level is drawn it's background
#    <LI>text clipping path is turned on
#    <LI>most scraps' data is drawn
#    <LI>text clipping path is turned off (this enables fine printing
#            of text labels
#    <LI>text scraps from current level are drawn
#    <LI>this is done for all levels of current layer
#    <LI>scraps from top layer(s) are stroked with thin pen
# </OL>


sub print_map {
  if (!$opt_m) {
    ($a, $b) = split /\./, $mapsheet;
    ($tmp = $SHEET{$curr_layer}{"$mapsheet"}{"scraps"}) =~ s/^ //;
    $HSHIFT = ($Hsize * $a + $Hoffset - $Overlap);
    $VSHIFT = ($Vsize * $b + $Voffset - $Overlap); 
  } else {
    ($tmp = $LAYER{$curr_layer}{"scraps"}) =~ s/^ //;
  }
    @tmp = split / /, $tmp;       
    %LEVEL = ();
    @page_text_scraps = ();
    while (@tmp) {
        $level = shift (@tmp);
        $scrap = shift (@tmp);
        $LEVEL{$level} .= " $scrap";
	if (exists $SCRAP{$scrap}{P}) {
            push @page_text_scraps, $scrap;
	}
    }
    
    if (!$opt_m && exists $LAYER{$curr_layer}{D}) {
      print_preview_down();
    }

    foreach $level (sort {$a <=> $b} keys %LEVEL) {
        ($tmp = $LEVEL{$level}) =~ s/^ //;
        @used_scraps = split / /, $tmp;       
	
	if ($transparency) {                  # transparency group beginning
	  print PAGEDEF "\\setbox\\xxx=\\hbox to ${HS}bp{%\n";
          print PAGEDEF "\\PL{/GS1 gs}%\n";   # beginning of transparency
	}

        print PAGEDEF "\\PL{q 1 g}%\n";      # white background of the scrap

        foreach $scrap (@used_scraps) {
            if (exists $SCRAP{$scrap}{I}) {        
                ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CI};
                $xcoord -= $HSHIFT; 
                    $XC = coord($xcoord);
                $ycoord -= $VSHIFT; 
                    $YC = coord($ycoord);
                print PAGEDEF "\\PB{$XC}{$YC}{\\",
                    tex_Xname("I$scrap"), "}%\n";
            }
        }
        print PAGEDEF "\\PL{Q}%\n"; # end of white color for filled bg

        foreach $scrap (@used_scraps) {        # colored area symbols
            if (exists $SCRAP{$scrap}{G}) {        
                              # orezane v Convert.pm
                ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CG};
                $xcoord -= $HSHIFT;
                    $XC = coord($xcoord);
                $ycoord -= $VSHIFT;
                    $YC = coord($ycoord);
                print PAGEDEF "\\PB{$XC}{$YC}{\\",
                    tex_Xname("G$scrap"), "}%\n";
                              # koniec orezavania obvodom
            }
        }

	if ($transparency) {
          print PAGEDEF "\\PL{/GS0 gs}%\n";  # end of default transparency
	}

        print PAGEDEF "\\PL{q 0 0 m $HS 0 l $HS $VS l 0 $VS l 0 0 l}";
                               # beginning of the text clipping path definition

        foreach $scrap (@page_text_scraps) {
            if ((exists $SCRAP{$scrap}{P}) and ($SCRAP{$scrap}{V}>=$level)) {
                ($xcoo,$ycoo) = ($xcoord, $ycoord) = 
                    split / /, $COORD{$scrap}{CX};
                if (exists $SCRAP{$scrap}{S}) {
                  my ($dx,$dy) = split / /, $SCRAP{$scrap}{S};
                  $xcoo -= $dx;
                  $ycoo -= $dy;
                }
                $xcoord -= $HSHIFT;
                    $XC = coord($xcoord);
                $ycoord -= $VSHIFT;
                    $YC = coord($ycoord);
                open (IN,"$SCRAP{$scrap}{P}") or error("");
                while(<IN>) {
                    chomp;
                    @data = split / /;
                    print PAGEDEF "\\PL{", coord($data[0]+$XC-$xcoo), " ", 
                            coord($data[1]+$YC-$ycoo), " ";
                    if ($data[6]) {
                        print PAGEDEF
                        coord($data[2]+$XC-$xcoo), " ",
                        coord($data[3]+$YC-$ycoo), " ",
                        coord($data[4]+$XC-$xcoo), " ",
                        coord($data[5]+$YC-$ycoo), " ",
                        $data[6], "}%\n";
                    }
                    else {                                              
                         print PAGEDEF $data[2], "}%\n";
                    }
                }
                close(IN);
            }
        }
    
        print PAGEDEF "\\PL{h W n}";  # end of text clipping path definition

        foreach $scrap (@used_scraps) {
          if (exists $SCRAP{$scrap}{F}) {        
            ($xcoord, $ycoord) = split / /, $COORD{$scrap}{C};
            $xcoord -= $HSHIFT;
              $XC = coord($xcoord);
            $ycoord -= $VSHIFT;
              $YC = coord($ycoord);
            print PAGEDEF "\\PB{$XC}{$YC}{\\",
              tex_Xname($scrap), "}%\n";
	  }
        }
	
        foreach $scrap (@used_scraps) {
          if (exists $SCRAP{$scrap}{E}) {        
            ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CE};
            $xcoord -= $HSHIFT;
                $XC = coord($xcoord);
            $ycoord -= $VSHIFT;
                $YC = coord($ycoord);
            print PAGEDEF "\\PB{$XC}{$YC}{\\",
                tex_Xname("E$scrap"), "}%\n";
          }
        }   

        print PAGEDEF "\\PL{Q}";  # end of clipping by text

        foreach $scrap(@used_scraps) {
          if (exists $SCRAP{$scrap}{X}) {        
            ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CX};
            $xcoord -= $HSHIFT;
                $XC = coord($xcoord);
            $ycoord -= $VSHIFT;
                $YC = coord($ycoord);
            print PAGEDEF "\\PB{$XC}{$YC}{\\",
                tex_Xname("X$scrap"), "}%\n";
	  }
        }   
        if ($transparency) {
          print PAGEDEF "\\hfill}\\ht\\xxx=${VS}bp\\dp\\xxx=0bp\n";
          print PAGEDEF "\\immediate\\pdfxform ";
          print PAGEDEF "attr{/Group \\the\\attrid\\space 0 R} ";
          print PAGEDEF "resources{/ExtGState \\the\\resid\\space 0 R}";
          print PAGEDEF "\\xxx\\PB{0}{0}{\\pdflastxform}%\n";
        }
    }

    if (!$opt_m && exists $LAYER{$curr_layer}{U}) {
	print_preview_up();
    }

}

sub print_preview_down {
        print PAGEDEF "\\PL{q .8 g}%\n"; # mozno zmenit na farbenie podla hlbky
	if (!$opt_m) {
          @used_layers = split / /, $LAYER{$curr_layer}{D};
	} else {
          @used_layers = split / /, $MAP_PREVIEW{D};
	}
        foreach $l (@used_layers) {
	  if (!$opt_m) {
            $tmp = $SHEET{"$l"}{"$mapsheet"}{"scraps"};
	    if (exists $PREVIEW{"$l"}{"$mapsheet"}{"scraps"}) {
              $tmp .= $PREVIEW{"$l"}{"$mapsheet"}{"scraps"};
	    }
	  } else {
            $tmp = $LAYER{$l}{"scraps"};
	  }
            if ($tmp) {
                @layer_scraps = split / /, $tmp;
                foreach $scrap (@layer_scraps) {
                  if ((exists $SCRAP{$scrap}{I}) && 
		      (not exists $SCRAP{$scrap}{Z})) {
                    ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CI};
                    $xcoord -= $HSHIFT; 
                        $XC = coord($xcoord);
                    $ycoord -= $VSHIFT; 
                        $YC = coord($ycoord);
                    print PAGEDEF "\\PB{$XC}{$YC}{\\",
                        tex_Xname("I$scrap"), "}%\n";
                  }
                }
            }
        }
        print PAGEDEF "\\PL{Q}%\n";
}

sub print_preview_up {
        print PAGEDEF "\\PL{q .1 w}%\n";
	if (!$opt_m) {
          @used_layers = split / /, $LAYER{$curr_layer}{U};
	} else {
          @used_layers = split / /, $MAP_PREVIEW{U};
	}
        foreach $l (@used_layers) {
	  if (!$opt_m) {
            $tmp = $SHEET{"$l"}{"$mapsheet"}{"scraps"};
	    if (exists $PREVIEW{"$l"}{"$mapsheet"}{"scraps"}) {
              $tmp .= $PREVIEW{"$l"}{"$mapsheet"}{"scraps"};
	    }
	  } else {
            $tmp = $LAYER{$l}{"scraps"};
	  }
            if ($tmp) {
                @layer_scraps = split / /, $tmp;
                foreach $scrap (@layer_scraps) {
                  if ((exists $SCRAP{$scrap}{B}) && 
		      (not exists $SCRAP{$scrap}{Z})) {
                    ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CB};
                    $xcoord -= $HSHIFT; 
                        $XC = coord($xcoord);
                    $ycoord -= $VSHIFT; 
                        $YC = coord($ycoord);
                    print PAGEDEF "\\PB{$XC}{$YC}{\\",
                        tex_Xname("B$scrap"), "}%\n";
                  }
                }
            }
        }
        print PAGEDEF "\\PL{Q}%\n";
}

# @FUNCTION
# print_navigator --
#
# Prints navigator window:
# <OL>
#    <LI>bottom layer filled gray
#    <LI>current layer fillad black
#    <LI>top layer stroked
# </OL>


sub print_navigator {
  $nav_x = 2*$nav_x_ext+1;
  $nav_y = 2*$nav_y_ext+1;
  $nf = sprintf "%.4f", 1/$nav_factor;

  $HSN = coord($Hsize/$nav_factor*$nav_x);
  $VSN = coord($Vsize/$nav_factor*$nav_y);
  print PAGEDEF "%\n\\setbox\\xxx=\\hbox to ",
    $HSN,
    "bp{%\n\\PL{q ",
    $nf,
    " 0 0 ",
    $nf,
    " 0 0 cm}%\n";
  ($a, $b) = split /\./, $mapsheet;
  %NAV_SCRAPS = ();
  if (exists $LAYER{$curr_layer}{D}) {
    print PAGEDEF "\\PL{.8 g}%\n";
    @used_layers = split / /, $LAYER{$curr_layer}{D};
    foreach $l (@used_layers) {
      for ($i = $a-$nav_x_ext; $i <= $a+$nav_x_ext; $i++) {
        for ($j = $b-$nav_y_ext; $j <= $b+$nav_y_ext; $j++) {
	  $tmp = "";
          if ($SHEET{$l}{"$i.$j"}{"scraps"}) {
            $tmp = $SHEET{$l}{"$i.$j"}{"scraps"};
	  }
          if ($PREVIEW{$l}{"$i.$j"}{"scraps"}) {
            $tmp .= $PREVIEW{$l}{"$i.$j"}{"scraps"};
	  }
          if ($SHEET{$l}{"$i.$j"}{"scraps"} || $PREVIEW{$l}{"$i.$j"}{"scraps"}) {
            $tmp =~ s/^ //;
            @tmp = split / /, $tmp;       
            while (@tmp) {
              shift (@tmp);    # redundant level information
              $scrap = shift (@tmp);
              if ((!$NAV_SCRAPS{$scrap}) and (exists $SCRAP{$scrap}{I}) and
	          (not exists $SCRAP{$scrap}{Z})) {
                ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CI};
                $xcoord -= ($Hsize * ($a-$nav_x_ext) + $Hoffset); 
                  $XC = coord($xcoord);
                $ycoord -= ($Vsize * ($b-$nav_y_ext) + $Voffset); 
                  $YC = coord($ycoord);
                print PAGEDEF "\\PB{$XC}{$YC}{\\",
                  tex_Xname("I$scrap"), "}%\n";
              }
              $NAV_SCRAPS{$scrap} = 1;
            }
	  }
        }
      }
    }
  }
  %NAV_SCRAPS = ();
  print PAGEDEF "\\PL{0 g}%\n";
  for ($i = $a-$nav_x_ext; $i <= $a+$nav_x_ext; $i++) {
    for ($j = $b-$nav_y_ext; $j <= $b+$nav_y_ext; $j++) {
      if ($SHEET{$curr_layer}{"$i.$j"}{"scraps"}) {
        ($tmp = $SHEET{$curr_layer}{"$i.$j"}{"scraps"}) =~ s/^ //;
        @tmp = split / /, $tmp;       
        while (@tmp) {
          shift (@tmp);
          $scrap = shift (@tmp);
          if ((not $NAV_SCRAPS{$scrap}) and (exists $SCRAP{$scrap}{I}) and
	          (not exists $SCRAP{$scrap}{Z})) {
            ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CI};
            $xcoord -= ($Hsize * ($a-$nav_x_ext) + $Hoffset); 
              $XC = coord($xcoord);
            $ycoord -= ($Vsize * ($b-$nav_y_ext) + $Voffset); 
              $YC = coord($ycoord);
            print PAGEDEF "\\PB{$XC}{$YC}{\\",
              tex_Xname("I$scrap"), "}%\n";
          }
          $NAV_SCRAPS{$scrap} = 1;
        }
      }
    }
  }
  %NAV_SCRAPS = ();
  if (exists $LAYER{$curr_layer}{U}) {
    print PAGEDEF "\\PL{0.2 w}%\n";
    @used_layers = split / /, $LAYER{$curr_layer}{U};
    foreach $l (@used_layers) {
      for ($i = $a-$nav_x_ext; $i <= $a+$nav_x_ext; $i++) {
        for ($j = $b-$nav_y_ext; $j <= $b+$nav_y_ext; $j++) {
	  $tmp = "";
          if ($SHEET{$l}{"$i.$j"}{"scraps"}) {
            $tmp = $SHEET{$l}{"$i.$j"}{"scraps"};
	  }
          if ($PREVIEW{$l}{"$i.$j"}{"scraps"}) {
            $tmp .= $PREVIEW{$l}{"$i.$j"}{"scraps"};
	  }
          if ($SHEET{$l}{"$i.$j"}{"scraps"} || $PREVIEW{$l}{"$i.$j"}{"scraps"}) {
            $tmp =~ s/^ //;
            @tmp = split / /, $tmp;       
            while (@tmp) {
              shift (@tmp);
              $scrap = shift (@tmp);
              if ((not $NAV_SCRAPS{$scrap}) and (exists $SCRAP{$scrap}{B}) and
	          (not exists $SCRAP{$scrap}{Z})) {
                ($xcoord, $ycoord) = split / /, $COORD{$scrap}{CB};
                $xcoord -= ($Hsize * ($a-$nav_x_ext) + $Hoffset); 
                  $XC = coord($xcoord);
                $ycoord -= ($Vsize * ($b-$nav_y_ext) + $Voffset); 
                  $YC = coord($ycoord);
                print PAGEDEF "\\PB{$XC}{$YC}{\\",
                  tex_Xname("B$scrap"), "}%\n";
              }
              $NAV_SCRAPS{$scrap} = 1;
            }
	  }
        }
      }
    }
  }
  print PAGEDEF "\\PL{Q}",
    "\\PL{0 0 $HSN $VSN re S 0.1 w}";
  for ($i = 1; $i < $nav_x; $i++) {
    print PAGEDEF "\\PL{",coord($HSN*$i/$nav_x)," 0 m ",
                                      coord($HSN*$i/$nav_x)," $VSN l S}%\n";
  }
  for ($i = 1; $i < $nav_y; $i++) {
    print PAGEDEF "\\PL{0 ",coord($VSN*$i/$nav_y)," m $HSN ",
                                        coord($VSN*$i/$nav_y)," l S}%\n";
  }
  print PAGEDEF "\\PL{0.4 w ",
    coord($HSN*$nav_x_ext/$nav_x), " ",
    coord($VSN*$nav_y_ext/$nav_y), " ",
    coord($HSN/$nav_x), " ",
    coord($VSN/$nav_y), " ",
    " re S}";
  print PAGEDEF "\\hfill}\\ht\\xxx=",
    $VSN, "bp\\dp\\xxx=0bp\n";
  print PAGEDEF "\\immediate\\pdfxform\\xxx\\newcount\\", 
            tex_Nname($nickname), 
            " \\", tex_Nname($nickname), "=\\pdflastxform\n";
}

# @FUNCTION
# compose_page --
#
# Makes final page. Function consists mostly of TeX commands for
# setting boxes in the right position. If you need special layout,
# define own dopage TeX macro. Knowledge of Plain TeX necessary!

sub compose_page {
    print PAGE "%\n% Page: $nickname\n%\n";
    if ($SHEET{$curr_layer}{$mapsheet}{dest}) {
        print PAGE "\\pdfdest name {$nickname} xyz\n";
    }                                     

    print PAGE "\\setbox\\mapbox=\\hbox to ${exactHS}bp{%\n";    # map
    print PAGE "\\rlap{\\pdfrefxform\\", tex_Sname($nickname), "}%\n";
    print_map_links();
    print PAGE "\\hfil}\\ht\\mapbox=${exactVS}bp%\n";

    my ($a, $b) = split /\./, $mapsheet;
#    print PAGE "\\pagelabel={$SHEET{$curr_layer}{$mapsheet}{sign}}%\n";
    print PAGE "\\pagelabel={",grid_name($start_y,-$b)," ",
                              grid_name($start_x,$a), ### " L", $curr_layer, 
			      "}%\n";
    if ($page_numb) {print PAGE "\\pagenum=$REFS{$nickname}%\n"} 
    
#    print PAGE "\\setbox\\arrowbox=\\hbox to 30 bp{%\n"; 
#    print PAGE "\\setbox\\updownbox=\\vbox{%\n"; 
    updownlinks();
#    print PAGE "\\hfil}%\n";
#    print PAGE "}%\n";

    print PAGE "\\pagename={$LAYER{$curr_layer}{N}}%\n";
    if ($page_numb) {
      set_map_hints();
      print_map_hints();
    }
    print PAGE "\\setbox\\navbox=\\hbox{%\n";
    print_navigator_links();
    print PAGE
        "\\pdfrefxform\\",tex_Nname($nickname),"}%\n";  # navigator window

    print PAGE "\\dopage\\eject\n";
}

# @FUNCTION
# print_map_links --
#
# Prints links to adjoining pages on the border of map sheet

sub print_map_links {
    $lw = 25;
    $lhy = $VS - 2*$lw;
    $lhx = $HS - 2*$lw;
    if ($SHEET{$curr_layer}{$mapsheet}{jumpW}) {
        print PAGE "\\flatlink{0}{$lw}{$lw}{$lhy}",
            "{$SHEET{$curr_layer}{$mapsheet}{jumpW}}%\n";
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpE}) {
        print PAGE "\\flatlink{",$HS-$lw,"}{$lw}{$lw}{$lhy}",
            "{$SHEET{$curr_layer}{$mapsheet}{jumpE}}%\n";
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpN}) {
        print PAGE "\\flatlink{$lw}{",$VS-$lw,"}{$lhx}{$lw}",
            "{$SHEET{$curr_layer}{$mapsheet}{jumpN}}%\n";
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpS}) {
        print PAGE "\\flatlink{$lw}{0}{$lhx}{$lw}",
            "{$SHEET{$curr_layer}{$mapsheet}{jumpS}}%\n";
    }
}

# @FUNCTION
# set_map_hints --
#
# Sets hint informations with numbers of neighbouring pages

sub set_map_hints {
    %HINT = ();
    if ($SHEET{$curr_layer}{$mapsheet}{jumpW}) {
      $HINT{W} = $REFS{$SHEET{$curr_layer}{$mapsheet}{jumpW}};
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpE}) {
      $HINT{E} = $REFS{$SHEET{$curr_layer}{$mapsheet}{jumpE}};
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpN}) {
      $HINT{N} = $REFS{$SHEET{$curr_layer}{$mapsheet}{jumpN}};
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpS}) {
      $HINT{S} = $REFS{$SHEET{$curr_layer}{$mapsheet}{jumpS}};
    }
#    if ($SHEET{$curr_layer}{$mapsheet}{jumpU}) {
#      $HINT{U} = $REFS{$SHEET{$curr_layer}{$mapsheet}{jumpU}};
#    }
#    if ($SHEET{$curr_layer}{$mapsheet}{jumpD}) {
#      $HINT{D} = $REFS{$SHEET{$curr_layer}{$mapsheet}{jumpD}};
#    }
}

# @FUNCTION
# print_map_hints --
#
# Prints <I>vbox</I> with numbers of adjoining pages

sub print_map_hints {
#  if ($HINT{W}) {print PAGE "\\hintW={$HINT{W}}%\n"}
#  else {print PAGE "\\hintW={notdef}%\n"}
#  if ($HINT{E}) {print PAGE "\\hintE={$HINT{E}}%\n"}
#  else {print PAGE "\\hintE={notdef}%\n"}
#  if ($HINT{N}) {print PAGE "\\hintN={$HINT{N}}%\n"}
#  else {print PAGE "\\hintN={notdef}%\n"}
#  if ($HINT{S}) {print PAGE "\\hintS={$HINT{S}}%\n"}
#  else {print PAGE "\\hintS={notdef}%\n"}
#  if ($HINT{U}) {print PAGE "\\hintU={$HINT{U}}%\n"}
#  else {print PAGE "\\hintU={notdef}%\n"}
#  if ($HINT{D}) {print PAGE "\\hintD={$HINT{D}}%\n"}
#  else {print PAGE "\\hintD={notdef}%\n"}
  if ($HINT{W}) {print PAGE "\\pointerW=$HINT{W}%\n"}
  else {print PAGE "\\pointerW=0%\n"}
  if ($HINT{E}) {print PAGE "\\pointerE=$HINT{E}%\n"}
  else {print PAGE "\\pointerE=0%\n"}
  if ($HINT{N}) {print PAGE "\\pointerN=$HINT{N}%\n"}
  else {print PAGE "\\pointerN=0%\n"}
  if ($HINT{S}) {print PAGE "\\pointerS=$HINT{S}%\n"}
  else {print PAGE "\\pointerS=0%\n"}
#  if ($HINT{U}) {print PAGE "\\hintU=$HINT{U}%\n"}
#  else {print PAGE "\\hintU=0%\n"}
#  if ($HINT{D}) {print PAGE "\\hintD=$HINT{D}%\n"}
#  else {print PAGE "\\hintD=0%\n"}
}

# @FUNCTION
# updownliks --
#
# Prints links to top and bottom layer if they exist

sub updownlinks {
    if ($SHEET{$curr_layer}{$mapsheet}{jumpU}) {
 #        print PAGE "\\link{20}{14}{\\linkUP}",
 #            "{$SHEET{$curr_layer}{$mapsheet}{jumpU}}%\n";
      print PAGE "\\pointerU={%\n";
      foreach $tmplayer (sort {$b <=> $a} 
                         keys %{$SHEET{$curr_layer}{$mapsheet}{jumpU}}) {
#	print PAGE "\\textlink{\$\\uparrow\$: $LAYER{$tmplayer}{N}: ",
#	  "$REFS{$SHEET{$curr_layer}{$mapsheet}{jumpU}{$tmplayer}}",
#	  "}{$SHEET{$curr_layer}{$mapsheet}{jumpU}{$tmplayer}}%\n";
	print PAGE "$LAYER{$tmplayer}{N}|",
	  "$REFS{$SHEET{$curr_layer}{$mapsheet}{jumpU}{$tmplayer}}|",
	  "$SHEET{$curr_layer}{$mapsheet}{jumpU}{$tmplayer}||%\n";
      }		 
      print PAGE "}%\n";
    } else {
      print PAGE "\\pointerU={notdef}%\n";
    }
    if ($SHEET{$curr_layer}{$mapsheet}{jumpD}) {
 #        print PAGE "\\link{20}{3}{\\linkDOWN}",
 #            "{$SHEET{$curr_layer}{$mapsheet}{jumpD}}%\n";
      print PAGE "\\pointerD={%\n";
      foreach $tmplayer (sort {$b <=> $a} 
                         keys %{$SHEET{$curr_layer}{$mapsheet}{jumpD}}) {
#	print PAGE "\\textlink{\$\\downarrow\$: $LAYER{$tmplayer}{N}: ",
#	  "$REFS{$SHEET{$curr_layer}{$mapsheet}{jumpD}{$tmplayer}}",
#	  "}{$SHEET{$curr_layer}{$mapsheet}{jumpD}{$tmplayer}}%\n";
	print PAGE "$LAYER{$tmplayer}{N}|",
	  "$REFS{$SHEET{$curr_layer}{$mapsheet}{jumpD}{$tmplayer}}|",
	  "$SHEET{$curr_layer}{$mapsheet}{jumpD}{$tmplayer}||%\n";
      }		 
      print PAGE "}%\n";
    } else {
      print PAGE "\\pointerD={notdef}%\n";
    }
}

# @FUNCTION
# print_navigator_links --
#
# Prints links to adjoining pages into navigator window

sub print_navigator_links {
    my ($a, $b) = split /\./, $mapsheet;
    for ($i=-$nav_x_ext; $i <= $nav_x_ext; $i++) {
        for ($j=-$nav_y_ext; $j <= $nav_y_ext; $j++) {
          if (not (($i==0) and ($j==0))) {
            $navsheet = move_mapsheet($mapsheet,$i,$j);
            if ((exists $SHEET{$curr_layer}{$navsheet}{name}) &&
               (! exists $EXCLUDED{$SHEET{$curr_layer}{$navsheet}{name}})) {
              my $navsheetname = $SHEET{$curr_layer}{$navsheet}{"name"};
              print PAGE "\\flatlink{",
                coord($HSN*($i+$nav_x_ext)/$nav_x), "}{",
                coord($VSN*($j+$nav_y_ext)/$nav_y), "}{",
                coord($HSN/$nav_x), "}{",
                coord($VSN/$nav_y), "}{$navsheetname}%\n";
            }
          }
        }
    }
}


# @FUNCTION
# print_grid --
#
# Prints a grid over map sheet according specifications in Config file.
# In the future could be generated by Metapost.

sub print_grid {    
    print PAGEDEF "\\PL{q}";
    print PAGEDEF "\\PL{3 w 0 0 $HS $VS re S}";
    $i = coord($Hsize + $Overlap); 
    $j = coord($Vsize + $Overlap);
    $ovrl = coord($Overlap);
    print PAGEDEF "\\PL{0.5 w}";
    print PAGEDEF "\\PL{0 $ovrl m $HS $ovrl l S}";
    print PAGEDEF "\\PL{0 $j m $HS $j l S}";
    print PAGEDEF "\\PL{$ovrl 0 m $ovrl $VS l S}";
    print PAGEDEF "\\PL{$i 0 m $i $VS l S}";
    $grid_min = ($Hsize * $a + $Hoffset - $Overlap);
    $grid_init = $Hgrid * floor($grid_min/$Hgrid) + $Hgridoffset - $Hgrid;
    print PAGEDEF "\\PL{[0 8] 0 d 1 J}";      # !!!!!!!!!!!!!!!!!!!!!!
    for ($i=$grid_init; $i <= $grid_min + $HS; $i += $Hgrid) {
        $j = coord($i - $grid_min);
        print PAGEDEF "\\PL{$j 0 m $j $VS l S}";
    }
    $grid_min = ($Vsize * $b + $Voffset - $Overlap);
    $grid_init = $Vgrid * floor($grid_min/$Vgrid) + $Vgridoffset - $Vgrid;
    for ($i=$grid_init; $i <= $grid_min + $VS; $i += $Vgrid) {
        $j = coord($i - $grid_min);
        print PAGEDEF "\\PL{0 $j m $HS $j l S}";
    }
    print PAGEDEF "\\PL{Q}%\n";
}

sub print_map_grid {
  print PAGEDEF "\\PL{q .4 w}%\n";
  print PAGEDEF "\\PL{0 0 $HS $VS re S}%\n";
  for ($i=0; $i <= $HS; $i += $Hsize) {
    $j = coord($i);
    print PAGEDEF "\\PL{$j 0 m $j $VS l S}%\n";
  }
  for ($i=0; $i <= $VS; $i += $Vsize) {
    $j = coord($i);
    print PAGEDEF "\\PL{0 $j m $HS $j l S}%\n";
  }
  print PAGEDEF "\\PL{Q}%\n";
}
