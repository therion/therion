package map;

print STDOUT "map, ";

$main::thCOMMANDS{'map'} = ' map::export_map';

sub get_point_coord {
    unless (shift =~ /^([0-9\-\.]+[pm])?([0-9\-\.]+)?\/?([0-9\-\.dg]*)\/?([0-9\-\.dg]*)(".*")?$/) {return};
    my ($dv,$l,$a,$i,$name) = ($1, $2, $3, $4, $5);
    if ($a eq '') {$a = '-'};
    if ($i eq '') {$i = '-'};
    if ($dv eq '') {$dv = '-'};
    if ($l eq '') {$l = '-'};
    if ($name !~ /".*"/) {$name = '""';};
    return (join ';', ($mapshot_f, $mapshot_t, $dv, $l, $a, $i, $name));
};

sub get_line_params {
    my @cmds = split /\s+/, shift;

    #backup shot points
    my ($mshot_bf, $mshot_bt) = ($mapshot_f, $mapshot_t);

    #get line name
    my $line_name = '""';
    if ($cmds[0] =~ /^(\w+):$/) {
       $line_name = '"' . $1 . '"';
       shift @cmds;
    };

    my $line_cycle = '0';

    my $cmds_count = @cmds;
    my $cmds_cur = 0;
    my $cpt = -1;
    my @pts = (['','-','-','..']);

    COMMAND:
    while ($cmds_cur < $cmds_count) {
          my $ccom = $cmds[$cmds_cur];

          # set shot points
          if ($ccom =~ /\[(.+)\]/) {
             $mapshot_f = main::export_norm_point($1);
             $mapshot_t = $mshot_bt;
             $ccc = $cmds_cur + 1;
             while ($ccc < $cmds_count) {
                   if ($cmds[$ccc] =~ /\[(.+)\]/) {
                      $mapshot_t = main::export_norm_point($1);
                      last;
                   };
                   $ccc++;
             };
          }
          elsif ($ccom eq 'cycle') {$line_cycle = '1'}
          elsif (($ccom =~ /^([\.\-]{2,3})$/) && ($cpt>=0)) {$pts[$cpt][3] = $1}
          elsif ($ccom =~ /^(\w+)>>$/) {$pts[$cpt+1][1] = $1}
          elsif (($ccom =~ /^>>(\w+)$/) && ($cpt>=0)) {$pts[$cpt][2] = $1}
          else {
               my $ccor = get_point_coord $ccom;
               if ($ccor) {
                  $cpt++;
                  $pts[$cpt][0] = $ccor;
                  $pts[$cpt+1] = ['','-','-','..'];
               };
          };
    } continue {
    $cmds_cur++;
    };

    #restore shot points
    ($mapshot_f, $mapshot_t) = ($mshot_bf, $mshot_bt);

    #create output string

    my $line_points = '';
    for my $ccc (0 .. $cpt) {
        $line_points .= ' ' . join(';', @{$pts[$ccc]});
        };

    return join(' ',($line_name,$line_cycle,$line_points));

};

sub export_stone {
    my @stones = split /;\s*/ ,shift;
    foreach my $stone (@stones) {
            my @params = split /\s+/, $stone;
            my $crd = get_point_coord shift @params;
            if ($crd) {
               $crd .= " " . join " ", @params;
               print thEXPORTFILE "*stone $crd\n";
            };
    };
};

sub export_sand {
    my @sands = split /;\s+/ ,shift;
    foreach my $sand (@sands) {
            my @params = split /\s+/, $sand;
            my $crd = get_point_coord shift @params;
            if ($crd) {
               $crd .= " " . join " ", @params;
               print thEXPORTFILE "*sand $crd\n";
            };
    };
};


sub set_shot {
    my @points = split /\s+/, shift;
    $mapshot_f = main::export_norm_point (shift @points);
    $mapshot_t = main::export_norm_point (shift @points);
};


sub export_wall {
    my @walls = split /;\s+/, shift;
    foreach my $wall (@walls) {
            my $wallpar = get_line_params $wall;
            print thEXPORTFILE "*wall $wallpar\n";
    };
};

sub export_pit {
    my @pits = split /;\s+/, shift;
    foreach my $pit (@pits) {
            my $pitpar = get_line_params $pit;
            print thEXPORTFILE "*pit $pitpar\n";
    };
};

sub export_border {
    my @borders = split /;\s+/, shift;
    foreach my $border (@borders) {
            my $borderpar = get_line_params $border;
            print thEXPORTFILE "*border $borderpar\n";
    };
};

sub export_lake {
    my @paths = split /\s+/, shift;
    my $paths = join ' ', @paths;
    print thEXPORTFILE "*lake $paths\n";
};

sub export_label {
    my @params = split /\s+\:\s+/, shift;
    my @posp = split /\s+/, shift @params;
    my $text = join ' : ', @params;
    my $crd = get_point_coord shift @posp;
    unless ($posp[0]) {$posp[0] = '90'};
    unless ($posp[1]) {$posp[1] = 'LD'};
    $pars = join ' ', ($crd, $posp[0], $posp[1]);
    print thEXPORTFILE "*label $pars $text\n";
};

sub export_title {
    my @params = split /\s+\:\s+/, shift;
    my $path = shift @params;
    my $text = join ' : ', @params;
    my $ppth = get_line_params $path;
    print thEXPORTFILE "*title $ppth : $text\n";
};

sub export_polygon {
    my @pts = split /\s+/, shift;
    my $lpt = pop @pts;
    my $pth = '';
    foreach $ppp (@pts) {$pth .= "[$ppp] 0p0 --- "};
    $pth .= "100p0 [$lpt]";
    $pth = get_line_params $pth;
    print thEXPORTFILE "*polygon $pth\n";
};

sub export_map {

    open thEXPORTFILE, '>>' . $main::thEXPORTFNAME;
    print thEXPORTFILE "map::\n";

    my $numlines = @_;
    my $linenum = 0;

    LINE:
    while ($linenum < $numlines) {
          my $line = @_[$linenum];
          if ($line =~ /^\s*\\(\w+)\s*(.*)$/) {
             my ($command, $parameter) = ($1, $2);
             if ($command eq 'stone') {export_stone $parameter}
             elsif ($command eq 'sand') {export_sand $parameter}
             elsif ($command eq 'shot') {set_shot $parameter}
             elsif ($command eq 'break') {print thEXPORTFILE "map::\n"}
             elsif ($command eq 'wall') {export_wall $parameter}
             elsif ($command eq 'pit') {export_pit $parameter}
             elsif ($command eq 'border') {export_border $parameter}
             elsif ($command eq 'lake') {export_lake $parameter}
             elsif ($command eq 'label') {export_label $parameter}
             elsif ($command eq 'polygon') {export_polygon $parameter}
             elsif ($command eq 'title') {export_title $parameter}
             };
          } continue {
          $linenum++;
          };

    close thEXPORTFILE;
    }

# end of map package



