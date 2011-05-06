all:	xtherion

xtherion: *.tcl
	tclsh mkall.tcl

survex:
	tclsh mkall.tcl survex
	tar -cvzf ../../../svxedit.tar.gz svxedit
	perl ../makefile.pl rm -q svxedit

clean:
	perl ../makefile.pl rm -q *~
	perl ../makefile.pl rm -q ../../bin/xtherion
	perl ../makefile.pl rm -q lang/*~
	perl ../makefile.pl rm -q svxedit
	perl ../makefile.pl rm -q xtherion.tar.gz
	perl ../makefile.pl rm -q svxedit.tar.gz

update:
	make -C ./lang update
  
msgxth.tcl: lang/xtexts.txt
	make -C ./lang

