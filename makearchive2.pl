if (open(VFL,"thversion.h")) {
  @verfl = <VFL>;
  $verfl[0] =~ /(\d+)\.(\d+)(\.(\d+))?/;
  $version = $&;
  close(VFL);
} else {
  $version = "0.0";
}
system("tar -cvf ../therion-$version.tar -C .. therion");
system("gzip ../therion-$version.tar");
