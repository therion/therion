if (open(VFL,"thversion.h")) {
  @verfl = <VFL>;
  $verfl[0] =~ /(\d+)\.(\d+)(\.(\d+))?/;
  ($v1,$v2) = ($1,$2);
  close(VFL);
  $v2++;
} else {
  $v1 = 0;
  $v2 = 0;
}
open(VFL,">thversion.h");
print VFL "#define THVERSION \"$v1.$v2\"\n";
close(VFL);
open(VFL,">thbook/version.tex");
print VFL "$v1.$v2";
close(VFL);

