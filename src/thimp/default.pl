package default;

sub polyfix_PXYZ_MMM {
    return split /\s+/, shift;
    };

sub polydata_FTAIL_GGM {
    my ($f, $t, $a, $i, $l) = split /\s+/, shift;
    ($a, $i) *= 0.9;
    return ($f, $t, $l, $a, $i);
    };

