package polygon;

print STDOUT "polygon, ";

$export_file_name = 'th__data.svx';

$main::thCOMMANDS{'polygon'} = 'polygon::export_survex';

open thPOLYGONFILE, '>' . $export_file_name;
print thPOLYGONFILE ";Therion survex export\n";
close thPOLYGONFILE;

sub svx_export_norm_point {
    my $point = @_[0];
    $point = main::export_norm_point($point);
    $point =~ s/\@/\./;
    return $point;
    };

sub export_svx_shot {
    my $data = shift;
    my ($f, $t, $l, $a, $i) = &{$polygon_data_trans_sub}($data);
    $f = svx_export_norm_point($f);
    $t = svx_export_norm_point($t);
    print thPOLYGONFILE "$f\t$t\t$l\t$a\t$i\n";
    };

sub export_svx_fix {
    my ($point, $XC, $YC, $ZC) = &{$polygon_fix_trans_sub}(@_[0]);
    $point = svx_export_norm_point($point);
    print thPOLYGONFILE "*fix $point $XC $YC $ZC\n";
    };

sub export_svx_equate {
    my @points = split /\s+/, @_[0];
    print thPOLYGONFILE '*equate';
    foreach my $point (@points) {
            $point = svx_export_norm_point($point);
            print thPOLYGONFILE " $point";
            };
    print thPOLYGONFILE "\n";
    };

sub export_survex {

    ($polygon_data_trans_sub, $polygon_fix_trans_sub) =
    ('default::polydata_FTAIL_GGM','default::polyfix_PXYZ_MMM');

    open thPOLYGONFILE, '>>' . $export_file_name;
    my $numlines = @_;
    my $linenum = 0;

    LINE:
    while ($linenum < $numlines) {
          my $line = @_[$linenum];

          if ($line =~ /^\s*\\(\w+)\s*(.*)$/) {

             my ($command, $parameter) = ($1, $2);
             if ($command =~ 'equate') {export_svx_equate($parameter);
                next LINE;};
             if ($command =~ 'fix') {export_svx_fix($parameter);
                next LINE;};
             if ($command =~ 'chdir') {&main::update_csurveydir($parameter);
                next LINE;};
             if ($command =~ 'transform') {&main::define_transform($parameter);
                next LINE;};
             if ($command =~ 'polytrans') {$polygon_data_trans_sub = $parameter;
                next LINE;};
             if ($command =~ 'fixtrans') {$polygon_fix_trans_sub = $parameter;
                next LINE;};
             next LINE;
             };

          if ($line =~ /^\s*$/) {next LINE};
          export_svx_shot ($line);

          } continue {
          $linenum++;
          };

    close thPOLYGONFILE;
    }

# end of polygon package



