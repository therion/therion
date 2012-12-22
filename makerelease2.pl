if (open(VFL,"thversion.h")) {
  @verfl = <VFL>;
  $verfl[0] =~ /(\d+)\.(\d+)(\.(\d+))?/;
  ($v1,$v2,$v3) = ($1,$2,$4);
  if (!$v3) {
    $v3 = 0;
  }
  close(VFL);
  $v3++;
} else {
  $v1 = 0;
  $v2 = 0;
  $v3 = 0;
}
open(VFL,">thversion.h");
print VFL "#define THVERSION \"$v1.$v2.$v3\"\n";
close(VFL);
open(VFL,">thbook/version.tex");
print VFL "$v1.$v2.$v3";
close(VFL);
