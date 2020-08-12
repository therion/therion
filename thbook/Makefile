OUTDIR ?= .
ifneq ($(OUTDIR),.)
LOUTDIR = $(OUTDIR)/thbook
else
LOUTDIR = .
endif

# initialize \year etc. from SOURCE_DATE_EPOCH, if set
export FORCE_SOURCE_DATE=1

all: $(LOUTDIR)/thbook.pdf

$(LOUTDIR)/thbook.pdf: *.tex
	pdftex thbook.tex
	pdftex thbook.tex
	pdftex thbook.tex
	perl ../makefile.pl rm ./thbook.log
ifneq ($(OUTDIR),.)
	perl ../makefile.pl mv ./thbook.pdf $(LOUTDIR)
endif
