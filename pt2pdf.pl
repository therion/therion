#!/usr/bin/perl -w

use POSIX "ceil";
use Getopt::Std;

die "Usage: pt2pdf <file>.txt\n" unless $ARGV[0];
getopt('i');
$plan = 2;
$elev = 3;
$cm = 72/2.54;

## SETTINGS ##
$cave = "";
$team = "";
$date = "";
$columns = 3;
$scale[$plan] = 1/200;
$scale[$elev] = 1/400;
$rotate[$plan] = 0;
$rotate[$elev] = 1;

$paper_x = 20*$cm;
$paper_y = 30*$cm;
$margin = 20; # bp

$tex = "
\\pageframe{%
  \\sketch{0}{1}{\\data}%
  \\sketch{0.05}{0.03}{\\plan}%
  \\sketch{0.95}{0.03}{\\elev}%
}
\\autoframe{\\plan}
\\pageframe{%
  \\sketch{0}{1}{\\data}%
}
\\autoframe{\\data}
";
## END OF SETTINGS ##
if (defined $opt_i) {do $opt_i;}

$block = 0;
$poly_len = 0;
@DATA=();
$DATA[1] = "";
@CENTRELINE = ();
open IN, $ARGV[0] or die "can't open pockettopo file $ARGV[0]!\n";
while(<IN>) {
  s/\s+$//;
  if (/^$/) {next;}
  if (/^TRIP$/) {
    $block = 1;
#    $DATA[$block] = "";
    $subblock = 0;
  }
  elsif (/^PLAN$/) {
    $block = 2;
    reset_bbox($block);
  }
  elsif (/^ELEVATION$/) {
    if ($poly_len>0) {
      if ($poly_len==1) {
        $DATA[2] .= "\\pdfliteral{$poly_moveto l}%\n";
      }
      $DATA[2] .= "\\pdfliteral{S}%\n";
      $poly_len = 0;
    }
    $block = 3;
    reset_bbox($block);
  }
  elsif (/^DATE (\d{4}\-\d{2}\-\d{2})$/) {
    $date ||= $1;
  }
  elsif (/^DECLINATION$/) {
  }
  elsif (/^DATA$/) {
    $subblock = 1;
#    @CENTRELINE = ();
  }
  elsif (/^STATIONS$/) {
    $subblock = 1;
    $DATA[$block] .= "\\pdfliteral{2 w 0.5 G}%\n";
  }
  elsif (/^SHOTS$/) {
    $subblock = 2;
    $DATA[$block] .= "\\pdfliteral{0.5 w 0.5 G [2] 0 d}%\n";
  }
  elsif (/^POLYLINE ([A-Z]+)$/) {
    $subblock = 3;
    if ($poly_len>0) {
      if ($poly_len==1) {
        $DATA[$block] .= "\\pdfliteral{$poly_moveto l}%\n";
      }
      $DATA[$block] .= "\\pdfliteral{S}%\n";
      $poly_len = 0;
    } else {
      $DATA[$block] .= "\\pdfliteral{0.5 w [] 0 d}%\n";
    }
    $col = $1;
    if ($col eq "BLACK") {$color = "0 0 0";}
    elsif ($col eq "GRAY") {$color = "0.5 0.5 0.5";}
    elsif ($col eq "BROWN") {$color = "0.74 0.59 0.23";}
    elsif ($col eq "RED") {$color = "1 0 0";}
    elsif ($col eq "GREEN") {$color = "0.066 0.87 0.047";}
    elsif ($col eq "BLUE") {$color = "0 0 1";}
    else {$color = "0.78 0.28 0.89";}
    $DATA[$block] .= "\\pdfliteral{$color RG}%\n";
    $is_first_poly = 1;
  }
  else {
    if ($block == 1) {
      if ($subblock == 1) {
        @ldata = split /\t/;
        $cl_from = $ldata[0];
        $cl_to = $ldata[1];
        $cl_az = $ldata[2];
        $cl_cl = $ldata[3];
        $cl_len = $ldata[4];
        $tmp = "";
        if (!$cl_to) {$tmp .= "\\pdfliteral{0.6 g}";}
        $tmp .= "$cl_from&$cl_to&$cl_az&$cl_cl&$cl_len";
        if (!$cl_to) {$tmp .= "\\pdfliteral{0 g}";}
        push @CENTRELINE, $tmp;
      }
    }
    elsif ($block>1) {
      @ldata = split /\s+/;
      if ($subblock == 1) {
        $x = inscale($ldata[0]);
        $y = inscale($ldata[1]);
        $txt = $ldata[2];
        add_to_bbox($x,$y,$block);
        $DATA[$block] .= "\\pdfliteral{$x $y m $x $y l S}%\n";
        $DATA[$block] .= "\\pdfliteral{BT /F\\pdffontname\\f\\space 6 Tf $x $y Td ($txt) Tj ET}";
      }
      if ($subblock == 2) {
        $x = inscale($ldata[0]);
        $y = inscale($ldata[1]);
        $x2 = inscale($ldata[2]);
        $y2 = inscale($ldata[3]);
        add_to_bbox($x,$y,$block);
        add_to_bbox($x2,$y2,$block);
        $DATA[$block] .= "\\pdfliteral{$x $y m $x2 $y2 l S}%\n";
      }
      if ($subblock == 3) {
        $x = inscale($ldata[0]);
        $y = inscale($ldata[1]);
        add_to_bbox($x,$y,$block);
        if ($is_first_poly) {
          $DATA[$block] .= "\\pdfliteral{$x $y m}%\n";
          $poly_moveto = "$x $y";
          $is_first_poly = 0;
        } else {
          $DATA[$block] .= "\\pdfliteral{$x $y l}%\n";
        }
        $poly_len++;
      }
    }
  }
}

if ($poly_len>0) {
  if ($poly_len==1) {
    $DATA[$block] .= "\\pdfliteral{$poly_moveto l}%\n";
  }
  $DATA[$block] .= "\\pdfliteral{S}%\n";
}
close IN;

expand_bbox(2,$margin);
expand_bbox(3,$margin);

for $i (2..3) {
  if ($rotate[$i]==0) {
    $hsize[$i] = $maxx[$i]-$minx[$i];
    $vsize[$i] = $maxy[$i]-$miny[$i];
  } else {
    $vsize[$i] = $maxx[$i]-$minx[$i];
    $hsize[$i] = $maxy[$i]-$miny[$i];
  }
  if ($hsize[$i] > $paper_x || $vsize[$i] > $paper_y) {
    die "The sketch does not fit onto paper in the current scale!\n";
  }
}

if (defined $date) {$fname = $date;} else {$fname = "archive";}
$fname2 = $ARGV[0];
$fname2 =~ s/\.txt$//;
$fname .= "_" . $fname2;

open OUT, ">$fname.tex";
print OUT <<ENDTEX;
\\newcount\\data \\newcount\\plan \\newcount\\elev
\\newdimen\\sketchx
\\newdimen\\sketchy

\\def\\pageframe#1{\\setbox0=\\hbox to \\hsize{#1\\hfil}\\ht0=\\vsize \\box0\\eject}
\\def\\sketch#1#2#3{%
  \\setbox99=\\hbox{\\pdfrefxform#3}%
  \\sketchx=\\hsize \\advance\\sketchx by -\\wd99%
  \\sketchy=\\vsize \\advance\\sketchy by -\\ht99%
  \\rlap{\\kern#1\\sketchx\\raise#2\\sketchy\\box99}%
}
\\def\\autoframe#1{{\\setbox0=\\hbox{\\pdfrefxform#1}%
  \\pdfpagewidth=\\wd0
  \\pdfpageheight=\\ht0
  \\hsize=\\wd0
  \\vsize=\\ht0
  \\box0
  \\eject}}

\\pdfpagewidth=$paper_x bp
\\pdfpageheight=$paper_y bp
\\hsize=$paper_x bp
\\vsize=$paper_y bp
\\pdfhorigin=0bp
\\pdfvorigin=0bp
\\tabskip=9bp

\\font\\f=cmtt9 
\\pdfincludechars\\f{0123456789.:EPaeilnotv}
\\f
\\nopagenumbers
\\parindent=0pt
ENDTEX

print OUT "\\setbox0=\\vbox{\\kern${margin}bp\\hbox{\\kern${margin}bp\\vbox{\\hbox{", 
  "$cave}\\hbox{$team}\\hbox{$date}\\medskip", 
  make_table(), 
  "}\\kern${margin}bp}\\kern${margin}bp}\\pdfxform0\\data=\\pdflastxform\n";
for ($i=2; $i<=3; $i++) {
  print OUT "\\setbox0=\\vbox to $vsize[$i] bp {\\vfill\n";
  if ($rotate[$i]==0) {
    print OUT "\\pdfliteral{1 0 0 1 ",-$minx[$i]," ",-$miny[$i]," cm}%\n";
  } else {
    print OUT "\\pdfliteral{0 1 -1 0 ",$maxy[$i]," ",-$minx[$i]," cm}%\n";
  }
  print OUT "\\pdfliteral{1 j 1 J 1 M}%\n";
#  print OUT "\\pdfliteral{q 0.95 g $minx[$i] $miny[$i] ", $maxx[$i]-$minx[$i], " ", $maxy[$i]-$miny[$i], " re F Q}\n";
  print OUT "$DATA[$i]";
  print OUT "\\pdfliteral{BT /F\\pdffontname\\f\\space 8 Tf ", $minx[$i]+$margin/2, " ", $miny[$i]+$margin/2, " Td [(";
  if ($i==2) {print OUT "Plan";} else {print OUT "Elevation";} 
  print OUT   ")-500(1:",int(1/$scale[$i]+0.5),")] TJ ET}%\n";
  print OUT "\\pdfliteral{0.5 G $minx[$i] $miny[$i] ", $maxx[$i]-$minx[$i], " ", $maxy[$i]-$miny[$i], " re S}\n";
  print OUT "}\\wd0=$hsize[$i] bp\n" ;
  print OUT "\\pdfxform resources { /ProcSet [/PDF /Text] /Font << /F\\pdffontname\\f\\space\\pdffontobjnum\\f\\space 0 R >> } 0\n";
  if ($i==2) {print OUT "\\plan";} else {print OUT "\\elev";} 
  print OUT "=\\pdflastxform\n";
  print OUT "\n";
}

print OUT "$tex\n";

print OUT "\\end\n";
close OUT;

system("pdftex $fname.tex");
unlink("$fname.log");
unlink("$fname.tex");


sub add_to_bbox {
# global minmaxxy
  my ($x,$y,$b) = @_;
  if ($x < $minx[$b]) {$minx[$b] = $x;}
  if ($x > $maxx[$b]) {$maxx[$b] = $x;}
  if ($y < $miny[$b]) {$miny[$b] = $y;}
  if ($y > $maxy[$b]) {$maxy[$b] = $y;}
}

sub reset_bbox {
# global minmaxxy
  my $b = shift;
  $minx[$b] = 1e10;
  $maxx[$b] = -1e10;
  $miny[$b] = 1e10;
  $maxy[$b] = -1e10;
}

sub expand_bbox {
# global minmaxxy
  my ($b,$val) = @_;
  $minx[$b] -= $val;
  $maxx[$b] += $val;
  $miny[$b] -= $val;
  $maxy[$b] += $val;
}

sub inscale {
# global block
  my $n = shift;
  return $n * 100 * 72 /2.54 * $scale[$block];
}

sub make_table {
# global CENTRELINE, columns
  my $elem = @CENTRELINE;
  my $rows = ceil($elem/$columns);
  my $pos;
  my $s = "\\halign{\\f ";
  for ($j = 0; $j < $columns; $j++) {
    if ($j>0) {$s.="\\quad&";}
    $s .= "\\hfil#&\\hfil#&\\hfil#&\\hfil#&\\hfil#";
  }
  $s .= "\\cr\n";
  for ($j = 0; $j < $columns; $j++) {
    if ($j>0) {$s.="&";}
    $s .= "From&To&Compass&Clino&Length";
  }
  $s .= "\\cr\\noalign{\\smallskip\\hrule\\smallskip}\n";
  for ($i = 0; $i < $rows; $i++) {
    for ($j = 0; $j < $columns; $j++) {
      $pos = $i+$j*$rows;
      if ($j>0) {$s .= "&";}
      if ($pos<$elem) {
        $s .= $CENTRELINE[$pos];
      }
    }
    $s .= "\\cr\n";
  }
  $s .= "}\n";
  return $s;
}
