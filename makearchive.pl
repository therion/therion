
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday) = gmtime(time);
$year += 1900;
$mon += 1;
$datestr = sprintf("%04d%02d%02d",$year,$mon,$mday);

system("tar -cvf ../therion-$datestr.tar -C .. therion");
system("bzip2 -f ../therion-$datestr.tar");