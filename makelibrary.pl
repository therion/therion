$a = 0;
open(INP,$ARGV[0]);
while(<INP>) {
  
  if (/\/\*\*/) {
    $a = 1;
  } 
  
  if ($a) {
    print;
  }
  
}
close(INP);
