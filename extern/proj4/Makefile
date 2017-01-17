AR ?= ar
CC ?= cc
OUTDIR ?= .
ifneq ($(OUTDIR),.)
LOUTDIR = $(OUTDIR)/extern/proj4
else
LOUTDIR = .
endif
OBJECTS = $(addprefix $(LOUTDIR)/,$(addsuffix .o,$(basename $(shell ls *.c))))


$(LOUTDIR)/%.o : %.c
	$(CC) -c $(CCFLAGS) -I. -o $@ $<


all: $(LOUTDIR)/libproj.a

$(LOUTDIR)/libproj.a: $(OBJECTS)
	$(AR) cr $(LOUTDIR)/libproj.a $(LOUTDIR)/*.o

remove:
	rm -f proj.c gen_cheb.c p_series.c jniproj.c cs2cs.c \
	cs2cs.c gen_cheb.c p_series.c nad2nad.c nad2bin.c \
	geod.c geod_set.c geod_for.c geod_inv.c geodesic.h

clean:
	perl ../../makefile.pl rm *.o
	perl ../../makefile.pl rm *~
	perl ../../makefile.pl rm *.a


