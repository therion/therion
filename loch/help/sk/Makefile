OUTDIR ?= .
ifneq ($(OUTDIR),.)
HOUTDIR = ../../../$(OUTDIR)/loch/help/sk
else
HOUTDIR = .
endif


all: loch.htb

loch.htb: loch.hhp loch.hhc loch.hhk loch.htm
	zip $(HOUTDIR)/loch.htb loch.hhp loch.hhc loch.hhk loch.htm

clean:
	perl ../../../makefile.pl rm -q loch.htb
