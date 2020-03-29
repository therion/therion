OUTDIR ?= .
ifneq ($(OUTDIR),.)
LOUTDIR = $(OUTDIR)/thbook
else
LOUTDIR = .
endif

# to make build reproducible, set PDF creation time to noon at a date taken
# from the file 'version.tex' (which is produced by set_version.py when the
# top-level make is run)
export SOURCE_DATE_EPOCH:=$(shell python3 -c "import datetime;print(int(12*60*60+datetime.datetime.strptime(open('version.tex').read()[-12:-2], '%Y-%m-%d').replace(tzinfo=datetime.timezone.utc).timestamp()))")
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
