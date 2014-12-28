OUTDIR ?= .
ifneq ($(OUTDIR),.)
LOUTDIR = $(OUTDIR)
else
LOUTDIR = ..
endif


all:
ifneq ($(LOUTDIR),..)
	rm -rf $(LOUTDIR)/samples
	rm -rf $(LOUTDIR)/samples.doc
	cp -rf ../samples $(LOUTDIR)
endif
	tclsh samples.tcl $(LOUTDIR) $(LOUTDIR)/samples
ifneq ($(LOUTDIR),..)
	perl ../makefile.pl mv samples.log $(LOUTDIR)/samples
	rm -f samples.log
endif
    
clean:
	tclsh samples.tcl . clean
    