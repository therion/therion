if (open(VFL,"thversion.h")) {
  @verfl = <VFL>;
  $verfl[0] =~ /(\d+)\.(\d+)\.(\d+)/;
  ($v1,$v2,$v3) = ($1,$2,$3);
  close(VFL);
} else {
  $v1 = 0;
  $v2 = 3;
  $v3 = 0;
}
system("tar -cvzf ../therion-$v1.$v2.$v3.tar.gz -C .. therion");
