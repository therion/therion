$platform = lc($ARGV[0]);

#($sec,$min,$hour,$mday,$mon,$year,$wday,$yday) = gmtime(time);
#$year += 1900;
#$mon += 1;
#$datestr = sprintf("%04d%02d%02d",$year,$mon,$mday);

if (open(VFL,"thversion.h")) {
  @verfl = <VFL>;
  $verfl[0] =~ /(\d+)\.(\d+)\.(\d+)/;
  ($v1,$v2,$v3) = ($1,$2,$3);
  close(VFL);
} else {
  $v1 = 0;
  $v2 = 2;
  $v3 = 1;
}

$dd = "therion-temp";
$d = "../$dd";
system("mkdir $d");
system("cp ./therion $d");
system("cp ./therion.exe $d");
system("cp ./therion.ini $d");
system("cp ./xtherion/xtherion $d");
system("cp ./xtherion/xtherion.tcl $d");
system("cp ./xtherion/xtherion.ini $d");
system("cp ./thbook/thbook.pdf $d");
system("cp ./man/therion.1 $d");
system("cp ./man/xtherion.1 $d");
system("cp ./README $d");
system("cp ./COPYING $d");
if ($platform eq "win32") {
    system("zip -jr ../therion-$platform-$v1.$v2.$v3.zip $d");
} else {
    system("tar -cvf ../therion-linux-$v1.$v2.$v3.tar -C $d therion therion.ini xtherion xtherion.ini thbook.pdf therion.1 xtherion.1 README COPYING");
    system("bzip2 -f ../therion-linux-$v1.$v2.$v3.tar");
}
system("rm -R $d");
