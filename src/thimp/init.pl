#!/usr/bin/perl
#
#
# Init script

print STDOUT "This is therion importer.\n";
die "Usage: import file1 [file2 ... fileN]\n" unless (@ARGV);
print STDOUT "\n";
print "Loading packages: ";

%thCOMMANDS = ( 'survey' => '');

$thEXPORTFNAME = 'th__data.exp';

open thEXPORTFILE, '>' . $thEXPORTFNAME;
close thEXPORTFILE;

# end of init script




