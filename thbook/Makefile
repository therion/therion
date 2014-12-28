OUTDIR ?= .
ifneq ($(OUTDIR),.)
LOUTDIR = $(OUTDIR)/thbook
else
LOUTDIR = .
endif

all: $(LOUTDIR)/thbook.pdf

$(LOUTDIR)/thbook.pdf: *.tex
	pdftex thbook.tex
	pdftex thbook.tex    
	pdftex thbook.tex
	perl ../makefile.pl rm ./thbook.log
ifneq ($(OUTDIR),.)
	perl ../makefile.pl mv ./thbook.pdf $(LOUTDIR)
endif
    

