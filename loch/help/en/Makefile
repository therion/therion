all: loch.htb

loch.htb: loch.hhp loch.hhc loch.hhk loch.htm
	zip loch.htb loch.hhp loch.hhc loch.hhk loch.htm

clean:
	perl ../../../makefile.pl rm -q loch.htb
