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
  foreach $f (@ARGV) {
    if ($f =~ /\*/) {
      @fl = glob($f);
      foreach $ff (@fl) {
        unlink($ff) || warn("$0: can't delete $ff\n");
      }
    } else {
      unlink($f) || warn("$0: can't delete $f\n");
    }
  }
}

elsif ($ARGV[0] =~ /^\s*rmdir\s*$/i) {
  $dum = shift @ARGV;
  foreach $f (@ARGV) {
    rmtree($f) || warn("$0: can't delete $f\n");;
  }
}
