all:	xtherion

xtherion:
	tclsh mkall.tcl

survex:
	tclsh mkall.tcl survex
	tar -cvzf ../../../svxedit.tar.gz svxedit
	perl ../makefile.pl rm svxedit

clean:
	perl ../makefile.pl rm *~
	perl ../makefile.pl rm ../../bin/xtherion
	perl ../makefile.pl rm svxedit
	perl ../makefile.pl rm xtherion.tar.gz
	perl ../makefile.pl rm svxedit.tar.gz
