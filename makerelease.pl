
#($sec,$min,$hour,$mday,$mon,$year,$wday,$yday) = gmtime(time);
#$year += 1900;
#$mon += 1;
#$datestr = sprintf("%04d%02d%02d",$year,$mon,$mday);

if (open(VFL,"thversion.h")) {
  @verfl = <VFL>;
  $verfl[0] =~ /(\d+)\.(\d+)\.(\d+)/;
  ($v1,$v2,$v3) = ($1,$2,$3);
  close(VFL);
  $v3++;
} else {
  $v1 = 0;
  $v2 = 2;
  $v3 = 1;
}
open(VFL,">thversion.h");
print VFL "#define THVERSION \"$v1.$v2.$v3\"\n";
close(VFL);

