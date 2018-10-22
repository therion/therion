all: loch.htb loch.chm

loch.chm: loch.hhp loch.hhc loch.hhk loch.htm
	hhc loch.hhp

loch.htb: loch.hhp loch.hhc loch.hhk loch.htm
	zip loch.htb loch.hhp loch.hhc loch.hhk loch.htm

clean:
	perl ../../../makefile.pl rm -q loch.chm loch.htb
