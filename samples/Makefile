OUTDIR ?= .

all:
ifneq ($(OUTDIR),.)
	rm -rf ../$(OUTDIR)/samples
	rm -rf ../$(OUTDIR)/samples.doc
	cp -rf ../samples ../$(OUTDIR)
endif
	tclsh samples.tcl $(OUTDIR) ../$(OUTDIR)/samples
ifneq ($(OUTDIR),.)
	perl ../makefile.pl mv samples.log ../$(OUTDIR)/samples
	rm -f samples.log
endif
    
clean:
	tclsh samples.tcl . clean
    