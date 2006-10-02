use File::Glob;
use File::Copy;
use File::Path;

if ($ARGV[0] =~ /^\s*cp\s*$/i) {
  copy($ARGV[1],$ARGV[2]) || die("$0: error -- cp $ARGV[1] -> $ARGV[2]\n");
}

elsif ($ARGV[0] =~ /^\s*mv\s*$/i) {
  copy($ARGV[1],$ARGV[2]) || die("$0: error -- cp $ARGV[1] -> $ARGV[2]\n");
  unlink($ARGV[1]) || warn("$0: can't delete $ARGV[1]\n");;
}

elsif ($ARGV[0] =~ /^\s*rm\s*$/i) {
  $dum = shift @ARGV;
  $quietdel = '';
  if ($ARGV[0] =~ /^\s*\-q\s*$/) {
    $dum = shift @ARGV;
    $quietdel = 1;
  }
  foreach $f (@ARGV) {
    if ($f =~ /\*/) {
      @fl = glob($f);
      foreach $ff (@fl) {
        unlink($ff) || $quietdel || warn("$0: can't delete $ff\n");
      }
    } else {
      unlink($f) || $quietdel || warn("$0: can't delete $f\n");
    }
  }
}

elsif ($ARGV[0] =~ /^\s*rmdir\s*$/i) {
  $dum = shift @ARGV;
  $quietdel = '';
  if ($ARGV[0] =~ /^\s*\-q\s*$/) {
    $dum = shift @ARGV;
    $quietdel = 1;
  }
  foreach $f (@ARGV) {
    rmtree($f) || $quietdel || warn("$0: can't delete $f\n");;
  }
}
