package eps2pdf;
use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(convert_line coord);

sub convert_line {
    $epsline = shift;
    ($llx, $lly) = @_;
    if ($epsline =~ /^%/) {return ""};

    $pdfline = "";
    $epsline =~ s/^ //;

    $prev_font ||= "";    

    @buffer = split / /, $epsline;
    if (@prev_buffer) {
        @buffer = (@prev_buffer, @buffer);
        @prev_buffer = ();
    }

    if ($intext) {
        $pdfline .= "ET ";
        $intext = 0;
    }

    while (@buffer) {
        $token = shift(@buffer);
        if ($token eq 'moveto') {
            $stack = two_coord(@stack); $pdfline .= "$stack m ";
            $lastmove = $stack;
    	    @stack = ();
        }
        elsif ($token eq 'curveto') {
            $stack = six_coord(@stack); $pdfline .= "$stack c ";
	        @stack = ();
        }
        elsif ($token eq 'lineto') {
            $stack = two_coord(@stack); $pdfline .= "$stack l ";
	        @stack = ();
            }
        elsif ($token eq 'rlineto') {
            $pdfline .= "$lastmove l ";
    	    @stack = ();
        }

        elsif ($token eq 'newpath') {
    	    @stack = ();
        }
        elsif ($token eq 'closepath') {
            $pdfline .= "h ";
    	    @stack = ();
        }
        elsif ($token eq 'fill') {
            $pdfline .= "f ";
    	    @stack = ();
        }
        elsif ($token eq 'stroke') {
            $pdfline .= "S ";
    	    @stack = ();
        }
        elsif ($token eq 'clip') {
            $pdfline .= "W n ";
    	    @stack = ();
        }

        elsif ($token eq 'setlinejoin') {
            $pdfline .= "@stack j ";
    	    @stack = ();
        }
        elsif ($token eq 'setlinecap') {
            $pdfline .= "@stack J ";
    	    @stack = ();
        }
        elsif ($token eq 'setmiterlimit') {
            $pdfline .= "@stack M ";
    	    @stack = ();
        }
        elsif ($token eq 'setgray') {
            $pdfline .= "@stack g @stack G ";
    	    @stack = ();
        }
        elsif ($token eq 'setrgbcolor') {
            $pdfline .= "@stack rg @stack RG ";
    	    @stack = ();
        }
        elsif ($token eq 'setdash') {
            $pdfline .= "@stack d ";
    	    @stack = ();
        }
        elsif ($token eq 'setlinewidth') {
            $buf = $stack[0] || $stack[1];
            $pdfline .= "$buf w ";
            if ($stack[1]) {shift(@buffer)}
    	    @stack = ();
        }
        elsif ($token eq 'setpattern') {
            $pdfline .= "/CS1 cs /$stack[0] scn ";
    	    @stack = ();
        }

        elsif ($token eq 'gsave') {
            $pdfline .= "q ";
            @stack = ();
        }
        elsif ($token eq 'grestore') {
            $pdfline .= "Q ";
    	    @stack = ();
        }

        elsif ($token eq 'translate') {
            $stack = two_coord(@stack);
            $pdfline .= "1 0 0 1 $stack cm ";
    	    @stack = ();
        }
        elsif ($token eq 'scale') {
            $pdfline .= "$stack[0] 0 0 $stack[1] 0 0 cm ";
    	    @stack = ();
        }

        elsif ($token eq 'fshow') {
            if ($prev_pdfline !~ /ET/) {
                $pdfline .= "BT ";
                $prev_font = "";
            }
            $fsize = pop(@stack);
            $fname = pop(@stack);
            if (not exists $main::FONTS{$fname}) {
                $main::FONTS{$fname} = 1;
            }
            if (not exists $main::ALLFONTS{$fname}) {
                $main::ALLFONTS{$fname} = $main::fontid;
                $main::fontid++;
            }

            if ($prev_font ne "$fsize $fname") {
                $pdfline .= "/F\\pdffontname\\$main::ALLFONTS{$fname}\\space $fsize Tf ";
            }
            if ($concat) {
                $pdfline .= "$fntmatr Tm\n";
            }
            else {
                $pdfline .= "1 0 0 1 $lastmove Tm\n"; 
            }
            $TMP = $tmp = "@stack";
            $tmp =~ s/\\\\/\\134/g;              # osetrit \
            $tmp =~ s/\\(\d\d\d)/\\string\\$1/g;        # \xxx octal chars
            $tmp =~ s/%/\\%/g;                    # %
            $tmp =~ s/#/\\#/g;                    # #
            $tmp =~ s/\\\(/\\string\\\(/g;        # \(
            $tmp =~ s/\\\)/\\string\\\)/g;        # \)
            $pdfline .= "$tmp Tj ";
            $bsl_used = 0;
            if ($TMP =~ /\\\\/) {$bsl_used = 1;}
            $TMP =~ s/\\\\//g;
            $TMP =~ s/\\(\d\d\d)/chr(oct($1))/eg;        # \xxx octal chars
            $TMP =~ s/\\\(/\(/g;
            $TMP =~ s/\\\)/\)/g;
            &used_chars($TMP);
            $intext = 1;
            $concat = 0;
            @buffer = ();                                # grestore
    	    @stack = ();
            $prev_font = "$fsize $fname";
            $prev_pdfline = "";
        }
        elsif ($token eq 'concat') {
            pop(@stack);            # last `]'
            $tmp2 = pop(@stack);
            $tmp1 = pop(@stack);
            $stack = two_coord($tmp1, $tmp2);
            $fntmatr = "@stack $stack";
            $fntmatr =~ s/\[//;  # first `['
            $concat = 1;
            @buffer = ();
    	    @stack = ();
        }
        else {push @stack, $token}
    }
    if (@stack) {
        print "Splitted line found\n";
        @prev_buffer = @stack;
    }
    $tmp = $prev_pdfline;
    $tmp =~ s/ $// if $prev_pdfline;
    $prev_pdfline = $pdfline;
    return $tmp;
}

sub coord {
    my $tmp = shift;
    $tmp = sprintf "%.2f", $tmp;
    return $tmp;
}

sub two_coord {
    my ($a, $b) = @_;
    $a -= $llx; $b -= $lly;
    return coord($a) . " " . coord($b);
}

sub six_coord {
    my ($a, $b, $c, $d, $e, $f) = @_;
    $ret = two_coord($a, $b) .' '. two_coord($c, $d) .' '. two_coord($e, $f);
}

sub used_chars_old {
    $tmp2 = shift; 
    $tmp2 =~ s/^\(//; 
    $tmp2 =~ s/\)$//; 
    $tmp2 =~ s/\\\\/\\/g;
    $tmp2 =~ s/\\\(/(/g;
    $tmp2 =~ s/\\\)/)/g;
    $tmp2 =~ s/ //g;
    @chars = split //, $tmp2;
    foreach $ch (@chars) {
        $USEDCHARS{$fname} ||= "";
        $chch = $ch;
        if (($ch eq "(") or ($ch eq ")") or ($ch eq "\\") or ($ch eq "?")) {
            $chch = "\\$ch";
        }
	if ($ch lt ' ') {$chch = sprintf "%o", "$ch"; $chch = "\\$chch";}
        if ($USEDCHARS{$fname} !~ /$chch/) {$USEDCHARS{$fname} .= "$ch";}
    }
}

sub used_chars {
    my $tmp = shift;
    $tmp = substr $tmp, 1, -1;    # zatvorky na zac. a konci
    $l = length($tmp) - 1;
    for $i(0..$l) {
        $cislo = ord(substr($tmp, $i, 1));
        $USEDCHARS{$fname}[$cislo] ||= 1;
    }
    if ($bsl_used) {$USEDCHARS{$fname}[92] ||= 1;} # spec. pre backslash
}
