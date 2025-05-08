# common therion objects
CMNOBJECTS = thdate.o extern/shapelib/shpopen.o extern/shapelib/dbfopen.o extern/shapelib/safileio.o  \
  thexception.o thbuffer.o thmbuffer.o thlog.o thlogfile.o thtmpdir.o thlocale.o \
  thparse.o thcmdline.o thconfig.o thinput.o thchenc.o thdatabase.o \
  thdataobject.o thdatareader.o thsurvey.o thendsurvey.o thdata.o \
  thperson.o thtf.o thtfangle.o thtflength.o thtfpwf.o \
  thdataleg.o thobjectname.o thinfnan.o thlayout.o thlookup.o thcomment.o \
  thinit.o thdb1d.o thsvxctrl.o thdatastation.o thobjectid.o \
  thobjectsrc.o thgrade.o thlibrary.o thgeomag.o thbezier.o \
  thexport.o thexporter.o thselector.o extern/img/img.o \
  thexpmodel.o thdb2d00.o thscrapis.o thcs.o thcsdata.o thexptable.o \
  thdb2d.o thscrap.o thendscrap.o th2ddataobject.o thdb2dprj.o \
  thdb2dpt.o thdb2dlp.o thdb2dab.o thdb2dji.o thdb2dmi.o thdb2dcp.o \
  thdb2dxm.o thdb2dxs.o thscraplo.o thscraplp.o thscrapen.o \
  thpoint.o thline.o tharea.o thlegenddata.o thmpost.o thsymbolsets.o \
  thjoin.o thmap.o thexpmap.o thlayoutln.o thlayoutclr.o thexpsys.o thexpuni.o \
  thpdf.o thpdfdbg.o thpdfdata.o thtexfonts.o \
  thsymbolset.o thlang.o thmapstat.o thexpdb.o thpic.o thsketch.o thproj.o \
  extern/lxMath.o extern/lxFile.o extern/icase.o thdb3d.o thsurface.o thscan.o thimport.o thsvg.o thepsparse.o \
  thtrans.o thwarpp.o thwarppt.o thwarppme.o thwarp.o thexpshp.o thattr.o thtex.o \
  extern/poly2tri/common/shapes.o extern/poly2tri/sweep/advancing_front.o extern/poly2tri/sweep/sweep.o extern/poly2tri/sweep/cdt.o extern/poly2tri/sweep/sweep_context.o \
  extern/quickhull/QuickHull.o therion.o
TESTOBJECTS = utest-main.o utest-proj.o utest-str.o

EXT =

# Prefix to install to (override like so: make PREFIX=/usr)
PREFIX ?= /usr/local
# Directory to install config files in (override like so: make SYSCONFDIR=/etc)
SYSCONFDIR ?= $(PREFIX)/etc

# PLATFORM CONFIG

# PLATFORM LINUX
##CXX = c++
##CC = gcc
##POBJECTS =
##LOCHEXE = loch/loch
##CXXPFLAGS = -DTHLINUX
##CCPFLAGS = -DTHLINUX
##LDPFLAGS = -s
##export THPLATFORM = LINUX
##export OUTDIR = ../therion.bin
##THXTHMKCMD = $(OUTDIR)/therion


# PLATFORM DEBIAN
CXX ?= c++
CC ?= gcc
POBJECTS =
LOCHEXE = loch/loch
CXXPFLAGS = -DTHLINUX
CCPFLAGS = -DTHLINUX
LDPFLAGS = -s
export THPLATFORM = LINUX
export OUTDIR = .
THXTHMKCMD = $(OUTDIR)/therion


# PLATFORM WIN32
##EXT = .exe
##CXX ?= c++
##CC ?= gcc
##POBJECTS = therion.res
##LOCHEXE = loch/loch
##CXXPFLAGS = -DTHWIN32
##CCPFLAGS = -DTHWIN32
##LDPFLAGS = -static-libgcc -s
##export THPLATFORM = WIN32
##export OUTDIR ?= ../therion.bin
##THXTHMKCMD = $(OUTDIR)/therion

# PLATFORM WIN32CROSS
##CROSS ?= i686-w64-mingw32.static-
##EXT = .exe
##CXX = $(CROSS)c++
##export CC = $(CROSS)gcc
##export AR = $(CROSS)ar
##POBJECTS = therion.res
##LOCHEXE = loch/loch
##CXXPFLAGS = -DTHWIN32
##CCPFLAGS = -DTHWIN32
##LDPFLAGS = -static-libgcc -static -s
##export THPLATFORM = WIN32
##THXTHMKCMD = therion
##export OUTDIR ?= ../therion.bin

# PLATFORM MACOSX
##CXX = c++
##CC = cc
##LOCHEXE = loch/loch
##POBJECTS =
##CXXPFLAGS = -DTHMACOSX
##CCPFLAGS = -DTHMACOSX
##LDPFLAGS =
##export THPLATFORM = MACOSX
##export OUTDIR = .
##THXTHMKCMD = ./therion

# PLATFORM ENDCONFIG


# BUILD CONFIG

# BUILD OZONE
##CCBFLAGS = -O3
##CXXBFLAGS = -O3
##LDBFLAGS = $(LDPFLAGS)

# BUILD OXYGEN
CCBFLAGS = -O2
CXXBFLAGS = -O2
LDBFLAGS = $(LDPFLAGS)

# BUILD RELEASE
##CCBFLAGS =
##CXXBFLAGS =
##LDBFLAGS = $(LDPFLAGS)

# BUILD DEBUG
##CCBFLAGS = -ggdb -fsanitize=address
##CXXBFLAGS = -ggdb -DTHDEBUG -fsanitize=address
##LDBFLAGS =

# BUILD ENDCONFIG

# proj4 settings
PROJ_UNSUPPORTED = 5.0.0 5.0.1 6.0.0 6.1.0 6.1.1 6.2.0
PROJ_VER = $(shell $(CROSS)pkg-config proj --modversion)
ifneq ($(filter $(PROJ_VER),$(PROJ_UNSUPPORTED)),)
    $(error unsupported Proj version: $(PROJ_VER))
endif
PROJ_LIBS ?= $(shell $(CROSS)pkg-config proj --libs)
PROJ_MVER ?= $(shell echo $(PROJ_VER) | sed 's/\..*//')
CXXJFLAGS ?= -DPROJ_VER=$(PROJ_MVER) -I$(shell $(CROSS)pkg-config proj --variable=includedir)


# compiler settings
CXXFLAGS = -DIMG_API_VERSION=1 -DP2T_STATIC_EXPORTS -Wall $(CXXPFLAGS) $(CXXBFLAGS) $(CXXJFLAGS) -Iextern -Iextern/shapelib -Iextern/quickhull -Iextern/img -I$(shell $(CROSS)pkg-config fmt --variable=includedir) -std=c++17
CCFLAGS = -DIMG_API_VERSION=1 -Wall $(CCPFLAGS) $(CCBFLAGS)
OBJECTS = $(addprefix $(OUTDIR)/,$(POBJECTS)) $(addprefix $(OUTDIR)/,$(CMNOBJECTS))
TESTOBJECTS_P = $(addprefix $(OUTDIR)/,$(TESTOBJECTS))


# linker settings
LIBS = $(PROJ_LIBS) $(shell $(CROSS)pkg-config fmt --libs)
LDFLAGS = $(LDBFLAGS)


$(OUTDIR)/%.o : %.cxx
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OUTDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OUTDIR)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(OUTDIR)/%.o : %.c
	$(CC) -c $(CCFLAGS) -o $@ $<

all: version outdirs $(OUTDIR)/therion tests doc xtherion/xtherion $(LOCHEXE)

outdirs:
	mkdir -p $(OUTDIR)/extern/poly2tri/sweep/
	mkdir -p $(OUTDIR)/extern/poly2tri/common/
	mkdir -p $(OUTDIR)/extern/quickhull
	mkdir -p $(OUTDIR)/extern/shapelib
	mkdir -p $(OUTDIR)/extern/img
	mkdir -p $(OUTDIR)/loch
	mkdir -p $(OUTDIR)/loch/help
	mkdir -p $(OUTDIR)/loch/help/en
	mkdir -p $(OUTDIR)/loch/help/sk
	mkdir -p $(OUTDIR)/xtherion
	mkdir -p $(OUTDIR)/thbook

version: outdirs
	python3 set_version.py .
ifeq ($(THPLATFORM),WIN32)
	echo "[PROJ]" > $(OUTDIR)/innosetup.ini && echo "version=$(PROJ_MVER)" >> $(OUTDIR)/innosetup.ini
endif

thversion.h: version

$(OUTDIR)/therion: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)/therion$(EXT) therion-main.cxx $(OBJECTS) $(LDFLAGS) $(LIBS)

$(OUTDIR)/utest$(EXT): $(OBJECTS) $(TESTOBJECTS_P) 
	$(CXX) $(CXXFLAGS) -o $(OUTDIR)/utest$(EXT) $(OBJECTS) $(TESTOBJECTS_P) $(LDFLAGS) $(LIBS)

tests: $(OUTDIR)/utest$(EXT)
ifneq ($(THPLATFORM),WIN32)
	$(OUTDIR)/utest$(EXT)
endif

$(OUTDIR)/therion.res: therion.rc
	$(CROSS)windres -i therion.rc -J rc -o $(OUTDIR)/therion.res -O coff

init:
	./therion --print-init-file > therion.ini

install: all
	tclsh makeinstall.tcl $(THPLATFORM) $(DESTDIR)$(PREFIX) $(DESTDIR)$(SYSCONFDIR)

release:
	python3 make_release.py

depend:
	perl makedepend.pl > Makefile.dep
	perl maketest.pl Makefile.dep
	perl makefile.pl mv Makefile.dep Makefile
	$(CXX) -MM $(CXXFLAGS) *.cxx >> Makefile
	perl makedepend2.pl

library: thversion.h
	$(THXTHMKCMD) --print-library-src thlibrarydata.thcfg > thlibrarydata.log
	perl makelibrary.pl thlibrarydata.log > thlibrarydata.tmp
	perl maketest.pl thlibrarydata.tmp
	perl makefile.pl mv thlibrarydata.tmp thlibrarydata.cxx

ifeq ($(THXTHMKCMD),$(OUTDIR)/therion)
  THERION_TCL_DEPS += $(OUTDIR)/therion
endif

xtherion/therion.tcl: $(THERION_TCL_DEPS)
	$(THXTHMKCMD) --print-xtherion-src > xtherion/therion.tcl

xtherion/xtherion: version xtherion/therion.tcl xtherion/*.tcl
	$(MAKE) -C ./xtherion

loch/loch: version loch/*.h loch/*.cxx
	$(MAKE) -C ./loch

doc: $(OUTDIR)/thbook/thbook.pdf

thbook: version $(OUTDIR)/thbook/thbook.pdf

samples: $(OUTDIR)/samples.doc/index.tex

samples-quick:
	$(MAKE) -C samples quick
	touch thbook/version.tex
	$(MAKE) -C thbook

samples-html:
	$(MAKE) -C samples html

$(OUTDIR)/samples.doc/index.tex:
	$(MAKE) -C samples
	touch thbook/version.tex
	$(MAKE) -C thbook

$(OUTDIR)/thbook/thbook.pdf: thbook/*.tex thversion.h
	$(MAKE) -C thbook

clean:
	perl makefile.pl rm -q ./xtherion/therion.tcl
	perl makefile.pl rmdir -q samples.doc
	$(MAKE) -C ./samples clean
	$(MAKE) -C ./loch clean
	$(MAKE) cleanrest

cleanrest:
	$(MAKE) -C ./xtherion clean
	$(MAKE) -C ./loch clean
	perl makefile.pl rm -q thmpost.cxx thtex.h thlangdata.h thchencdata.cxx thcsdata.h thmpost.h thcsdata.cxx thtex.cxx thsymbolsetlist.h thsymbolsets.cxx thsymbolsets.h thlangdatafields.h thchencdata.h SYMBOLS.txt
	perl makefile.pl rm -q therion ./xtherion/xtherion ./xtherion/xtherion.tcl therion.exe *~ *.log *.o thchencdata/*~ .xtherion.dat ./xtherion/ver.tcl thversion.h thbook/version.tex
	perl makefile.pl rm -q xtherion/*~ .xth_thconfig_xth xtherion/screendump thlang/*~
	perl makefile.pl rm -q extern/*.o extern/*~ extern/quickhull/*.o extern/shapelib/*.o extern/poly2tri/common/*.o extern/poly2tri/sweep/*.o samples/*~ samples/*.log
	perl makefile.pl rm -q symbols.html therion.res innosetup.ini
	perl makefile.pl rm -q tri/*.o tri/*~
	perl makefile.pl rm -q utest
	perl makefile.pl rm -q tex/*~
	perl makefile.pl rm -q us.stackdump loch/us.stackdump samples/us.stackdump xtherion/us.stackdump
	perl makefile.pl rm -q mpost/*~ examples/*~ examples/therion.log
	perl makefile.pl rm -q core symbols.xhtml cave.kml
	perl makefile.pl rm -q data.3d data.svx data.pos data.pts data.err data.plt
	perl makefile.pl rm -q cave.3d cave.lox cave.thm cave.pdf cave.sql cave.xhtml therion.tcl cave_a.pdf cave_m.pdf cave.vrml cave.wrl cave.3dmf cave.svg cave.tlx
	perl makefile.pl rm -q ./thbook/*~ ./thbook/thbook.log ./thbook/thbook.pdf ./lib/*~ ./mpost/*~ ./tex/*~
	perl makefile.pl rmdir -q doc thTMPDIR samples.doc symbols cave.shp tests/.doc
	perl makefile.pl rmdir -q doc symbols cave.shp tests/.doc
	perl makefile.pl rmdir -q thTMPDIR samples/*/thTMPDIR samples/*/*/thTMPDIR

thmpost.h: mpost/*.mp
	$(MAKE) -C ./mpost

thmpost.cxx: thmpost.h
thsymbolsets.h: thmpost.h thsymbolsetlist.h
thsymbolsets.cxx: thsymbolsets.h

thsymbolsetlist.h: thsymbolsetlist.pl mpost/thTrans.mp
	perl thsymbolsetlist.pl

thtex.h: tex/*.tex
	$(MAKE) -C ./tex

thtex.cxx: thtex.h

thchencdata.cxx: thchencdata.h
thchencdata.h: thchencdata/*.TXT
	$(MAKE) -C ./thchencdata

thcsdata.h: thcsdata.tcl
	tclsh thcsdata.tcl $(shell $(CROSS)pkg-config proj --variable=prefix)/share/proj

update:
	$(MAKE) -C ./thlang update

unixify: clean
	tclsh makeunixify.tcl

thlangdata.h: thlang/texts.txt
	$(MAKE) -C ./thlang

thlangdatafields.h: thlangdata.h
	$(MAKE) -C ./thlang

config-debug:
	perl makeconfig.pl BUILD DEBUG
	cd loch; perl makeconfig.pl BUILD DEBUG

config-release:
	perl makeconfig.pl BUILD RELEASE
	cd loch; perl makeconfig.pl BUILD RELEASE

config-oxygen:
	perl makeconfig.pl BUILD OXYGEN
	cd loch; perl makeconfig.pl BUILD OXYGEN

config-ozone:
	perl makeconfig.pl BUILD OZONE
	cd loch; perl makeconfig.pl BUILD OZONE

config-debian:
	perl makeconfig.pl PLATFORM DEBIAN
	cd loch; perl makeconfig.pl PLATFORM DEBIAN

config-linux:
	perl makeconfig.pl PLATFORM LINUX
	cd loch; perl makeconfig.pl PLATFORM LINUX

config-win32:
	perl makeconfig.pl PLATFORM WIN32
	cd loch; perl makeconfig.pl PLATFORM WIN32

config-win32cross:
	perl makeconfig.pl PLATFORM WIN32CROSS
	cd loch; perl makeconfig.pl PLATFORM WIN32CROSS

config-macosx:
	perl makeconfig.pl PLATFORM MACOSX
	cd loch; perl makeconfig.pl PLATFORM MACOSX

# external sources
$(OUTDIR)/extern/lxMath.o: loch/lxMath.h loch/lxMath.cxx
	$(CXX) -c $(CXXFLAGS) -o $(OUTDIR)/extern/lxMath.o loch/lxMath.cxx

$(OUTDIR)/extern/lxFile.o: loch/lxFile.h loch/lxFile.cxx
	$(CXX) -c $(CXXFLAGS) -o $(OUTDIR)/extern/lxFile.o loch/lxFile.cxx

$(OUTDIR)/extern/icase.o: loch/icase.h loch/icase.cxx
	$(CXX) -c $(CXXFLAGS) -o $(OUTDIR)/extern/icase.o loch/icase.cxx

extern/img/img.o: extern/img/img.c extern/img/img.h
extern/poly2tri/common/shapes.o: extern/poly2tri/common/shapes.cc extern/poly2tri/common/shapes.h
extern/poly2tri/sweep/advancing_front.o: extern/poly2tri/sweep/advancing_front.cc
extern/poly2tri/sweep/sweep.o: extern/poly2tri/sweep/sweep.cc
extern/poly2tri/sweep/cdt.o: extern/poly2tri/sweep/cdt.cc
extern/poly2tri/sweep/sweep_context.o: extern/poly2tri/sweep/sweep_context.cc


# DEPENDENCIES
$(OUTDIR)/th2ddataobject.o: th2ddataobject.cxx th2ddataobject.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h thexception.h \
 thsymbolset.h thsymbolsetlist.h thdatabase.h thmbuffer.h thbuffer.h \
 thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h
$(OUTDIR)/tharea.o: tharea.cxx tharea.h th2ddataobject.h thdataobject.h thperson.h \
 thdate.h thlayoutclr.h thstok.h thobjectsrc.h thdb2dab.h thobjectname.h \
 thmbuffer.h thexception.h thexpmap.h thexport.h thbuffer.h thlayout.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thlang.h thlangdata.h thline.h thdatabase.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thdb3d.h loch/lxMath.h thattr.h thchenc.h \
 thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h
$(OUTDIR)/thattr.o: thattr.cxx thattr.h thmbuffer.h thchenc.h thchencdata.h \
 thparse.h thbuffer.h thstok.h extern/shapelib/shapefil.h therion.h \
 thlog.h
$(OUTDIR)/thbezier.o: thbezier.cxx thbezier.h
$(OUTDIR)/thbuffer.o: thbuffer.cxx thbuffer.h
$(OUTDIR)/thchenc.o: thchenc.cxx thchenc.h thchencdata.h thparse.h thbuffer.h \
 thmbuffer.h thstok.h thchencdata.cxx therion.h thlog.h thexception.h
$(OUTDIR)/thchencdata.o: thchencdata.cxx
$(OUTDIR)/thcmdline.o: thcmdline.cxx thcmdline.h therion.h thlog.h thlogfile.h \
 thbuffer.h thconfig.h thmbuffer.h thinput.h thexporter.h thexport.h \
 thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thselector.h thtmpdir.h
$(OUTDIR)/thcomment.o: thcomment.cxx thcomment.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thdata.h thdataleg.h thobjectname.h \
 thmbuffer.h thinfnan.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
 thdatabase.h thbuffer.h thdb1d.h thobjectid.h thdb3d.h loch/lxMath.h \
 thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h
$(OUTDIR)/thconfig.o: thconfig.cxx thconfig.h thbuffer.h thmbuffer.h thinput.h \
 thexporter.h thexport.h thobjectsrc.h thlayout.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h loch/icase.h thselector.h therion.h thlog.h \
 thlang.h thlangdata.h thchenc.h thchencdata.h thparse.h thexception.h \
 thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h \
 thdb3d.h loch/lxMath.h thattr.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h \
 thdatareader.h thcsdata.h thproj.h thlogfile.h thinit.h thgeomag.h \
 thgeomagdata.h thsketch.h thpic.h thwarp.h thcs.h
$(OUTDIR)/thcs.o: thcs.cxx thcs.h thcsdata.h thstok.h thexception.h thproj.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thobjectname.h thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h \
 thscraplo.h thlayoutln.h thscrapen.h thscraplp.h
$(OUTDIR)/thcsdata.o: thcsdata.cxx thcsdata.h thstok.h thcs.h
$(OUTDIR)/thdata.o: thdata.cxx thdata.h thdataleg.h thstok.h thobjectname.h \
 thmbuffer.h thobjectsrc.h thinfnan.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thtfangle.h thtf.h thtflength.h thtfpwf.h thexception.h \
 thchenc.h thchencdata.h thparse.h thbuffer.h thsurvey.h thgrade.h \
 thcsdata.h thdatareader.h thinput.h thdatabase.h thdb1d.h thobjectid.h \
 thdb3d.h loch/lxMath.h thattr.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h therion.h thlog.h loch/icase.h
$(OUTDIR)/thdatabase.o: thdatabase.cxx thdatabase.h thmbuffer.h thbuffer.h thdb1d.h \
 thobjectid.h thinfnan.h thdataleg.h thstok.h thobjectname.h \
 thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thsurvey.h thdataobject.h thtfpwf.h thdata.h \
 thtfangle.h thtf.h thtflength.h thendsurvey.h thexception.h \
 thdatastation.h thlookup.h thlocale.h thgrade.h thcomment.h thlayout.h \
 thsymbolset.h thsymbolsetlist.h thscrap.h thsketch.h thpic.h thwarp.h \
 thtrans.h th2ddataobject.h thpoint.h thline.h tharea.h thjoin.h thmap.h \
 thimport.h thsurface.h thendscrap.h thconfig.h thinput.h thexporter.h \
 thexport.h loch/icase.h thselector.h thproj.h therion.h thlog.h
$(OUTDIR)/thdataleg.o: thdataleg.cxx thdataleg.h thstok.h thobjectname.h \
 thmbuffer.h thobjectsrc.h thinfnan.h thcsdata.h thparse.h thbuffer.h
$(OUTDIR)/thdataobject.o: thdataobject.cxx thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thexception.h thchenc.h \
 thchencdata.h thparse.h thbuffer.h thmbuffer.h thsurvey.h thtfpwf.h \
 thdata.h thdataleg.h thobjectname.h thinfnan.h thtfangle.h thtf.h \
 thtflength.h thconfig.h thinput.h thexporter.h thexport.h thlayout.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thselector.h thcsdata.h thproj.h thcs.h thdatabase.h thdb1d.h \
 thobjectid.h thdb3d.h loch/lxMath.h thattr.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h \
 thscraplp.h
$(OUTDIR)/thdatareader.o: thdatareader.cxx thdatareader.h thinput.h thbuffer.h \
 thmbuffer.h thexception.h thobjectsrc.h thdataobject.h thperson.h \
 thdate.h thlayoutclr.h thstok.h thdatabase.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h
$(OUTDIR)/thdatastation.o: thdatastation.cxx thdatastation.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h
$(OUTDIR)/thdate.o: thdate.cxx thdate.h thexception.h
$(OUTDIR)/thdb1d.o: thdb1d.cxx thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thstok.h thobjectname.h thmbuffer.h thobjectsrc.h thdb3d.h loch/lxMath.h \
 thattr.h thchenc.h thchencdata.h thparse.h thbuffer.h thsurvey.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thtfpwf.h thdata.h \
 thtfangle.h thtf.h thtflength.h thsvxctrl.h thdatabase.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h thpoint.h \
 th2ddataobject.h thlogfile.h thsurface.h thlocale.h thinit.h thinput.h \
 thconfig.h thexporter.h thexport.h thlayout.h thsymbolset.h \
 thsymbolsetlist.h loch/icase.h thselector.h thtrans.h thcsdata.h \
 thgeomagdata.h therion.h thlog.h extern/quickhull/QuickHull.hpp \
 extern/quickhull/Structs/Vector3.hpp extern/quickhull/Structs/Plane.hpp \
 extern/quickhull/Structs/Vector3.hpp extern/quickhull/Structs/Pool.hpp \
 extern/quickhull/Structs/Mesh.hpp extern/quickhull/Structs/Plane.hpp \
 extern/quickhull/Structs/Pool.hpp \
 extern/quickhull/Structs/VertexDataSource.hpp \
 extern/quickhull/ConvexHull.hpp \
 extern/quickhull/Structs/VertexDataSource.hpp \
 extern/quickhull/HalfEdgeMesh.hpp extern/quickhull/MathUtils.hpp \
 extern/quickhull/Structs/Ray.hpp
$(OUTDIR)/thdb2d.o: thdb2d.cxx thdb2d.h thinfnan.h thdb2dprj.h thstok.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thmbuffer.h thbuffer.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thobjectname.h thobjectsrc.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thdb1d.h thobjectid.h thdataleg.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thexception.h \
 thdatabase.h thtfangle.h thtf.h tharea.h th2ddataobject.h thdataobject.h \
 thmap.h thjoin.h thpoint.h thline.h thscrap.h thsketch.h thpic.h \
 thwarp.h thtrans.h thsurvey.h thtfpwf.h thdata.h thtflength.h \
 thlogfile.h thlayout.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 thexpmap.h thexport.h loch/icase.h thlang.h thlangdata.h thtmpdir.h \
 thinit.h thinput.h thfilehandle.h therion.h thlog.h
$(OUTDIR)/thdb2d00.o: thdb2d00.cxx thdb2d.h thinfnan.h thdb2dprj.h thstok.h \
 thmapstat.h thdate.h thperson.h thlegenddata.h thmbuffer.h thbuffer.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thobjectname.h thobjectsrc.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thdb1d.h thobjectid.h thdataleg.h \
 thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h \
 thdatabase.h thmap.h thdataobject.h thscrap.h thsketch.h thpic.h \
 thwarp.h thtrans.h thsurvey.h thtfpwf.h thdata.h thtfangle.h thtf.h \
 thtflength.h thlayout.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 thconfig.h thinput.h thexporter.h thexport.h loch/icase.h thselector.h
$(OUTDIR)/thdb2dab.o: thdb2dab.cxx thdb2dab.h thobjectname.h thmbuffer.h \
 thobjectsrc.h
$(OUTDIR)/thdb2dcp.o: thdb2dcp.cxx thdb2dcp.h thdb2dpt.h
$(OUTDIR)/thdb2dji.o: thdb2dji.cxx thdb2dji.h thobjectname.h thmbuffer.h \
 thdatabase.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thstok.h thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h \
 thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thexception.h
$(OUTDIR)/thdb2dlp.o: thdb2dlp.cxx thdb2dlp.h thdb2dpt.h thline.h th2ddataobject.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h \
 thparse.h thbuffer.h thmbuffer.h thinfnan.h thexpmap.h thexport.h \
 thlayout.h thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 loch/icase.h thlang.h thlangdata.h
$(OUTDIR)/thdb2dmi.o: thdb2dmi.cxx thdb2dmi.h thobjectname.h thmbuffer.h \
 thobjectsrc.h thstok.h
$(OUTDIR)/thdb2dprj.o: thdb2dprj.cxx thdb2dprj.h thstok.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thinfnan.h
$(OUTDIR)/thdb2dpt.o: thdb2dpt.cxx thdb2dpt.h thexpmap.h thexport.h thbuffer.h \
 thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thlang.h thlangdata.h
$(OUTDIR)/thdb2dxm.o: thdb2dxm.cxx thdb2dxm.h thdb2dmi.h thobjectname.h thmbuffer.h \
 thobjectsrc.h thstok.h thlayoutclr.h
$(OUTDIR)/thdb2dxs.o: thdb2dxs.cxx thdb2dxs.h thdb2dmi.h thobjectname.h thmbuffer.h \
 thobjectsrc.h thstok.h
$(OUTDIR)/thdb3d.o: thdb3d.cxx thdb3d.h loch/lxMath.h
$(OUTDIR)/thendscrap.o: thendscrap.cxx thendscrap.h thdataobject.h thperson.h \
 thdate.h thlayoutclr.h thstok.h thobjectsrc.h thexception.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h
$(OUTDIR)/thendsurvey.o: thendsurvey.cxx thendsurvey.h thdataobject.h thperson.h \
 thdate.h thlayoutclr.h thstok.h thobjectsrc.h thexception.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h
$(OUTDIR)/thepsparse.o: thepsparse.cxx thepsparse.h thpdfdbg.h thpdfdata.h \
 thtexfonts.h therion.h thlog.h thdouble.h thexception.h
therion-main.o: therion-main.cxx therion.h thlog.h thcmdline.h thconfig.h \
 thbuffer.h thmbuffer.h thinput.h thexporter.h thexport.h thobjectsrc.h \
 thlayout.h thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thselector.h thdatareader.h thlibrary.h thinit.h thversion.h \
 thtexfonts.h thbezier.h thlogfile.h thproj.h thdatabase.h thdb1d.h \
 thobjectid.h thinfnan.h thdataleg.h thobjectname.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thscrapen.h thscraplp.h
$(OUTDIR)/therion.o: therion.cxx therion.h thlog.h thconfig.h thbuffer.h \
 thmbuffer.h thinput.h thexporter.h thexport.h thobjectsrc.h thlayout.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h thlayoutln.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h thselector.h \
 thinit.h thpoint.h th2ddataobject.h thdb2dpt.h thobjectname.h thline.h \
 tharea.h thdb2dab.h thlang.h thlangdata.h thparse.h
$(OUTDIR)/thexception.o: thexception.cxx thexception.h
$(OUTDIR)/thexpdb.o: thexpdb.cxx thexpdb.h thexport.h thbuffer.h thobjectsrc.h \
 thlayout.h thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thexception.h thscrap.h thdb2dpt.h thdb2dcp.h thscraplo.h thscraplp.h \
 thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h thmbuffer.h \
 thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h \
 thscrapen.h thmapstat.h thlegenddata.h thsketch.h thpic.h thwarp.h \
 thtrans.h thmap.h thdatabase.h thdb2d.h thdb2dprj.h thdb2dlp.h \
 thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dxs.h thdb2dxm.h thdata.h \
 thtfangle.h thtf.h thtflength.h thtfpwf.h thinit.h thinput.h thsurvey.h \
 therion.h thlog.h
$(OUTDIR)/thexpmap.o: thexpmap.cxx thexpmap.h thexport.h thbuffer.h thobjectsrc.h \
 thlayout.h thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thlang.h thlangdata.h thexporter.h thexception.h thdatabase.h \
 thmbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h \
 thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h \
 thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h \
 thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
 thscraplo.h thscrapen.h thscraplp.h thtmpdir.h thscrap.h thsketch.h \
 thpic.h thwarp.h thtrans.h thpoint.h th2ddataobject.h thline.h tharea.h \
 thmap.h thconfig.h thinput.h thselector.h thlookup.h thinit.h \
 thlogfile.h thpdf.h thpdfdata.h thepsparse.h thmpost.h thtex.h \
 thcmdline.h thtexfonts.h thsurvey.h thtfpwf.h thdata.h thtfangle.h \
 thtf.h thtflength.h thcsdata.h thproj.h thsurface.h therion.h thlog.h \
 thsvg.h extern/img/img.h
$(OUTDIR)/thexpmodel.o: thexpmodel.cxx thexpmodel.h thexport.h thbuffer.h \
 thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thsurvey.h thtfpwf.h thdata.h thdataleg.h \
 thobjectname.h thmbuffer.h thinfnan.h thtfangle.h thtf.h thtflength.h \
 thexception.h thdatabase.h thdb1d.h thobjectid.h thdb3d.h loch/lxMath.h \
 thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h \
 thscraplp.h extern/img/img.h thscrap.h thsketch.h thpic.h thwarp.h \
 thtrans.h thsurface.h loch/lxFile.h thconfig.h thinput.h thexporter.h \
 thselector.h thcsdata.h thproj.h thcs.h thtexfonts.h thlang.h \
 thlangdata.h thfilehandle.h therion.h thlog.h
$(OUTDIR)/thexport.o: thexport.cxx thexport.h thbuffer.h thobjectsrc.h thlayout.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h thlayoutln.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h thexception.h \
 thconfig.h thmbuffer.h thinput.h thexporter.h thselector.h thdatabase.h \
 thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thscrapen.h thscraplp.h thcs.h thcsdata.h
$(OUTDIR)/thexporter.o: thexporter.cxx thexporter.h thexport.h thbuffer.h \
 thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thexception.h thconfig.h thmbuffer.h thinput.h \
 thselector.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h thexpmodel.h thsurvey.h \
 thtfpwf.h thdata.h thtfangle.h thtf.h thtflength.h thexpmap.h thlang.h \
 thlangdata.h thexpdb.h thexpsys.h thexptable.h therion.h thlog.h
$(OUTDIR)/thexpshp.o: thexpshp.cxx thexpmap.h thexport.h thbuffer.h thobjectsrc.h \
 thlayout.h thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thlang.h thlangdata.h thdatabase.h thmbuffer.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h \
 thmap.h thconfig.h thinput.h thexporter.h thselector.h thsurvey.h \
 thtfpwf.h thdata.h thtfangle.h thtf.h thtflength.h \
 extern/shapelib/shapefil.h thexpmodel.h thcsdata.h thcs.h thproj.h \
 therion.h thlog.h thexpshp.h thscrap.h thsketch.h thpic.h thwarp.h \
 thtrans.h thpoint.h th2ddataobject.h thline.h tharea.h
$(OUTDIR)/thexpsys.o: thexpsys.cxx thexpsys.h thexport.h thbuffer.h thobjectsrc.h \
 thlayout.h thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 therion.h thlog.h
$(OUTDIR)/thexptable.o: thexptable.cxx thexptable.h thexport.h thbuffer.h \
 thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thattr.h thmbuffer.h thchenc.h thchencdata.h \
 thparse.h thexception.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h \
 thscraplp.h thexporter.h thsurvey.h thtfpwf.h thdata.h thtfangle.h \
 thtf.h thtflength.h thscrap.h thsketch.h thpic.h thwarp.h thtrans.h \
 thpoint.h th2ddataobject.h thcsdata.h thproj.h thconfig.h thinput.h \
 thselector.h thcs.h therion.h thlog.h
$(OUTDIR)/thexpuni.o: thexpuni.cxx thexpmap.h thexport.h thbuffer.h thobjectsrc.h \
 thlayout.h thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thlang.h thlangdata.h thdatabase.h thmbuffer.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h \
 thmap.h thconfig.h thinput.h thexporter.h thselector.h thcsdata.h \
 thsurvey.h thtfpwf.h thdata.h thtfangle.h thtf.h thtflength.h \
 thexpmodel.h thexpuni.h thscrap.h thsketch.h thpic.h thwarp.h thtrans.h \
 thpoint.h th2ddataobject.h thline.h tharea.h thproj.h thtexfonts.h \
 therion.h thlog.h
$(OUTDIR)/thgeomag.o: thgeomag.cxx thgeomagdata.h
$(OUTDIR)/thgrade.o: thgrade.cxx thgrade.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thdata.h thdataleg.h thobjectname.h \
 thmbuffer.h thinfnan.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
 thdatabase.h thbuffer.h thdb1d.h thobjectid.h thdb3d.h loch/lxMath.h \
 thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h therion.h thlog.h
$(OUTDIR)/thimport.o: thimport.cxx loch/icase.h thimport.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h thcsdata.h \
 thexception.h thdata.h thdataleg.h thobjectname.h thmbuffer.h thinfnan.h \
 thtfangle.h thtf.h thtflength.h thtfpwf.h thsurvey.h thbuffer.h \
 thdatabase.h thdb1d.h thobjectid.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h therion.h thlog.h extern/img/img.h
$(OUTDIR)/thinfnan.o: thinfnan.cxx thinfnan.h
$(OUTDIR)/thinit.o: thinit.cxx thinit.h thbuffer.h thmbuffer.h thinput.h thchenc.h \
 thchencdata.h thparse.h thstok.h therion.h thlog.h thconfig.h \
 thexporter.h thexport.h thobjectsrc.h thlayout.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h loch/icase.h thselector.h thexception.h \
 thtexfonts.h thlang.h thlangdata.h thtmpdir.h thcs.h thcsdata.h thproj.h \
 thpdfdbg.h
$(OUTDIR)/thinput.o: thinput.cxx thinput.h thbuffer.h thmbuffer.h thchencdata.h \
 thparse.h thstok.h therion.h thlog.h thexception.h thversion.h
$(OUTDIR)/thjoin.o: thjoin.cxx thjoin.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thexception.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h
$(OUTDIR)/thlang.o: thlang.cxx thlang.h thlangdata.h thstok.h thlangdatafields.h \
 thinit.h thbuffer.h thmbuffer.h thinput.h thconfig.h thexporter.h \
 thexport.h thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 loch/icase.h thselector.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h thattr.h thchenc.h \
 thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h thexception.h
$(OUTDIR)/thlayout.o: thlayout.cxx thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thlookup.h thmapstat.h thlegenddata.h \
 thinfnan.h thexception.h thchenc.h thchencdata.h thparse.h thbuffer.h \
 thmbuffer.h thtfangle.h thtf.h thpdfdata.h thepsparse.h thtflength.h \
 thlang.h thlangdata.h thcsdata.h thconfig.h thinput.h thexporter.h \
 thexport.h loch/icase.h thselector.h th2ddataobject.h thdatabase.h \
 thdb1d.h thobjectid.h thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h \
 thattr.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thscrapen.h thscraplp.h therion.h thlog.h
$(OUTDIR)/thlayoutclr.o: thlayoutclr.cxx thlayoutclr.h thstok.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h \
 thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thexception.h thepsparse.h
$(OUTDIR)/thlayoutln.o: thlayoutln.cxx thlayoutln.h thlayoutclr.h thstok.h \
 thlayout.h thdataobject.h thperson.h thdate.h thobjectsrc.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h
$(OUTDIR)/thlegenddata.o: thlegenddata.cxx thlegenddata.h
$(OUTDIR)/thlibrary.o: thlibrary.cxx thlibrary.h thlibrarydata.cxx thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thstok.h thobjectname.h thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h \
 thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thlayout.h \
 thdataobject.h thsymbolset.h thsymbolsetlist.h thlocale.h thlang.h \
 thlangdata.h thgrade.h thdata.h thtfangle.h thtf.h thtflength.h \
 thtfpwf.h
$(OUTDIR)/thlibrarydata.o: thlibrarydata.cxx thdatabase.h thmbuffer.h thbuffer.h \
 thdb1d.h thobjectid.h thinfnan.h thdataleg.h thstok.h thobjectname.h \
 thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thlayout.h thdataobject.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thlang.h thlangdata.h thgrade.h thdata.h \
 thtfangle.h thtf.h thtflength.h thtfpwf.h
$(OUTDIR)/thline.o: thline.cxx thline.h th2ddataobject.h thdataobject.h thperson.h \
 thdate.h thlayoutclr.h thstok.h thobjectsrc.h thexception.h thchenc.h \
 thchencdata.h thparse.h thbuffer.h thmbuffer.h thdb2dlp.h thdb2dpt.h \
 thexpmap.h thexport.h thlayout.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h loch/icase.h thlang.h thlangdata.h \
 thtflength.h thtf.h thscrap.h thdb2dcp.h thscraplo.h thscraplp.h \
 thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h thdb3d.h \
 loch/lxMath.h thattr.h thscrapen.h thmapstat.h thlegenddata.h thsketch.h \
 thpic.h thwarp.h thtrans.h thdatabase.h thdb2d.h thdb2dprj.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dxs.h thdb2dxm.h
$(OUTDIR)/thlocale.o: thlocale.cxx thlocale.h thparse.h thbuffer.h thmbuffer.h \
 thstok.h thexception.h thlang.h thlangdata.h thinit.h thinput.h
$(OUTDIR)/thlog.o: thlog.cxx thlog.h thlogfile.h thbuffer.h
$(OUTDIR)/thlogfile.o: thlogfile.cxx thlogfile.h thbuffer.h therion.h thlog.h
$(OUTDIR)/thlookup.o: thlookup.cxx thlookup.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thlocale.h thmapstat.h \
 thlegenddata.h thinfnan.h thexception.h thchenc.h thchencdata.h \
 thparse.h thbuffer.h thmbuffer.h thpdfdata.h thepsparse.h thlang.h \
 thlangdata.h thscrap.h thdb2dpt.h thdb2dcp.h thscraplo.h thscraplp.h \
 thdb1d.h thobjectid.h thdataleg.h thobjectname.h thdb3d.h loch/lxMath.h \
 thattr.h thscrapen.h thsketch.h thpic.h thwarp.h thtrans.h thmap.h \
 thlayout.h thlayoutln.h thsymbolset.h thsymbolsetlist.h thtexfonts.h \
 thdatabase.h thdb2d.h thdb2dprj.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dxs.h thdb2dxm.h therion.h thlog.h
$(OUTDIR)/thmap.o: thmap.cxx thmap.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thmapstat.h thlegenddata.h \
 thobjectname.h thmbuffer.h thexception.h thdb2dmi.h thscrap.h thdb2dpt.h \
 thdb2dcp.h thscraplo.h thscraplp.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thbuffer.h thscrapen.h thsketch.h thpic.h thwarp.h thtrans.h \
 thtflength.h thtf.h thconfig.h thinput.h thexporter.h thexport.h \
 thlayout.h thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 loch/icase.h thselector.h thdatabase.h thdb2d.h thdb2dprj.h thdb2dlp.h \
 thdb2dab.h thdb2dji.h thdb2dxs.h thdb2dxm.h
$(OUTDIR)/thmapstat.o: thmapstat.cxx thmapstat.h thdate.h thperson.h thlegenddata.h \
 thscrap.h thdataobject.h thlayoutclr.h thstok.h thobjectsrc.h thdb2dpt.h \
 thdb2dcp.h thscraplo.h thscraplp.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thobjectname.h thmbuffer.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thbuffer.h thscrapen.h thsketch.h \
 thpic.h thwarp.h thtrans.h thdata.h thtfangle.h thtf.h thtflength.h \
 thtfpwf.h thmap.h thlayout.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thlang.h thlangdata.h thversion.h \
 thtexfonts.h thconfig.h thinput.h thexporter.h thexport.h loch/icase.h \
 thselector.h thcs.h thcsdata.h thproj.h thdb2dmi.h thdatabase.h thdb2d.h \
 thdb2dprj.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dxs.h thdb2dxm.h
$(OUTDIR)/thmbuffer.o: thmbuffer.cxx thmbuffer.h
$(OUTDIR)/thmpost.o: thmpost.cxx thmpost.h
$(OUTDIR)/thobjectid.o: thobjectid.cxx thobjectid.h
$(OUTDIR)/thobjectname.o: thobjectname.cxx thobjectname.h thmbuffer.h thexception.h \
 thdatabase.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thstok.h thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h \
 thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thdataobject.h thsurvey.h thtfpwf.h \
 thdata.h thtfangle.h thtf.h thtflength.h
$(OUTDIR)/thobjectsrc.o: thobjectsrc.cxx thobjectsrc.h
$(OUTDIR)/thparse.o: thparse.cxx therion.h thlog.h thlang.h thlangdata.h thstok.h \
 thtexfonts.h thinfnan.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h \
 thobjectid.h thdataleg.h thobjectname.h thobjectsrc.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h \
 thdb2dprj.h thmapstat.h thdate.h thperson.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thtflength.h thtf.h thexception.h loch/icase.h
$(OUTDIR)/thpdf.o: thpdf.cxx thpdfdbg.h thconfig.h thbuffer.h thmbuffer.h thinput.h \
 thexporter.h thexport.h thobjectsrc.h thlayout.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h loch/icase.h thselector.h thpdfdata.h \
 thepsparse.h thtexfonts.h thlang.h thlangdata.h thversion.h thdouble.h \
 therion.h thlog.h thcs.h thcsdata.h thproj.h
$(OUTDIR)/thpdfdata.o: thpdfdata.cxx thpdfdata.h thepsparse.h thlangdata.h
$(OUTDIR)/thpdfdbg.o: thpdfdbg.cxx thpdfdbg.h thpdfdata.h thepsparse.h
$(OUTDIR)/thperson.o: thperson.cxx thperson.h thdatabase.h thmbuffer.h thbuffer.h \
 thdb1d.h thobjectid.h thinfnan.h thdataleg.h thstok.h thobjectname.h \
 thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thexception.h
$(OUTDIR)/thpic.o: thpic.cxx thpic.h thbuffer.h thdatabase.h thmbuffer.h thdb1d.h \
 thobjectid.h thinfnan.h thdataleg.h thstok.h thobjectname.h \
 thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thinit.h thinput.h thtmpdir.h thexception.h \
 thconfig.h thexporter.h thexport.h thlayout.h thdataobject.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h thselector.h \
 therion.h thlog.h
$(OUTDIR)/thpoint.o: thpoint.cxx thpoint.h th2ddataobject.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h thdb2dpt.h \
 thobjectname.h thmbuffer.h thexception.h thchenc.h thchencdata.h \
 thparse.h thbuffer.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thdb3d.h loch/lxMath.h thattr.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thexpmap.h thexport.h thlayout.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h loch/icase.h thlang.h thlangdata.h \
 thtflength.h thtf.h thtexfonts.h thscrap.h thsketch.h thpic.h thwarp.h \
 thtrans.h therion.h thlog.h
$(OUTDIR)/thproj.o: thproj.cxx therion.h thlog.h thproj.h thlogfile.h thbuffer.h \
 thcs.h thcsdata.h thstok.h
$(OUTDIR)/thscan.o: thscan.cxx thscan.h thdb3d.h loch/lxMath.h thdataleg.h thstok.h \
 thobjectname.h thmbuffer.h thobjectsrc.h thinfnan.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thtflength.h thtf.h thexception.h \
 thdatabase.h thbuffer.h thdb1d.h thobjectid.h thattr.h thchenc.h \
 thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thcsdata.h thdatareader.h thinput.h thproj.h therion.h thlog.h
$(OUTDIR)/thscrap.o: thscrap.cxx thscrap.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thdb2dpt.h thdb2dcp.h thscraplo.h \
 thscraplp.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h \
 thmbuffer.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thbuffer.h thscrapen.h thmapstat.h thlegenddata.h thsketch.h \
 thpic.h thwarp.h thtrans.h thexception.h thtflength.h thtf.h thdb2d.h \
 thdb2dprj.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dxs.h \
 thdb2dxm.h thlayoutln.h th2ddataobject.h thline.h thpoint.h thscrapis.h \
 thsurvey.h thtfpwf.h thdata.h thtfangle.h thsymbolsetlist.h thcsdata.h \
 thdatabase.h therion.h thlog.h
$(OUTDIR)/thscrapen.o: thscrapen.cxx thscrapen.h
$(OUTDIR)/thscrapis.o: thscrapis.cxx thscrapis.h thdb3d.h loch/lxMath.h thscrap.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h \
 thdb2dpt.h thdb2dcp.h thscraplo.h thscraplp.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thobjectname.h thmbuffer.h thattr.h thchenc.h \
 thchencdata.h thparse.h thbuffer.h thscrapen.h thmapstat.h \
 thlegenddata.h thsketch.h thpic.h thwarp.h thtrans.h therion.h thlog.h \
 thdb2dlp.h thline.h th2ddataobject.h thconfig.h thinput.h thexporter.h \
 thexport.h thlayout.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thselector.h extern/poly2tri/poly2tri.h \
 extern/poly2tri/common/shapes.h extern/poly2tri/common/dll_symbol.h \
 extern/poly2tri/sweep/cdt.h extern/poly2tri/sweep/advancing_front.h \
 extern/poly2tri/sweep/sweep_context.h extern/poly2tri/sweep/sweep.h \
 thsurvey.h thtfpwf.h thdata.h thtfangle.h thtf.h thtflength.h
$(OUTDIR)/thscraplo.o: thscraplo.cxx thscraplo.h
$(OUTDIR)/thscraplp.o: thscraplp.cxx thscraplp.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thstok.h thobjectname.h thmbuffer.h thobjectsrc.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thbuffer.h \
 thscrap.h thdataobject.h thperson.h thdate.h thlayoutclr.h thdb2dpt.h \
 thdb2dcp.h thscraplo.h thscrapen.h thmapstat.h thlegenddata.h thsketch.h \
 thpic.h thwarp.h thtrans.h thexpmap.h thexport.h thlayout.h thlayoutln.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h thlang.h \
 thlangdata.h thdatabase.h thdb2d.h thdb2dprj.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dxs.h thdb2dxm.h
$(OUTDIR)/thselector.o: thselector.cxx thselector.h thlayoutclr.h thstok.h \
 thexception.h thconfig.h thbuffer.h thmbuffer.h thinput.h thexporter.h \
 thexport.h thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h \
 thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h \
 thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h \
 thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
 thscraplo.h thscrapen.h thscraplp.h thsurvey.h thtfpwf.h thdata.h \
 thtfangle.h thtf.h thtflength.h thscrap.h thsketch.h thpic.h thwarp.h \
 thtrans.h thmap.h therion.h thlog.h
$(OUTDIR)/thsketch.o: thsketch.cxx thsketch.h thpic.h thwarp.h thwarpp.h \
 thwarppme.h thwarppdef.h therion.h thlog.h thtrans.h thinfnan.h \
 thwarppt.h thobjectsrc.h thobjectname.h thmbuffer.h thconfig.h \
 thbuffer.h thinput.h thexporter.h thexport.h thlayout.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h loch/icase.h thselector.h
$(OUTDIR)/thsurface.o: thsurface.cxx thsurface.h thdb3d.h loch/lxMath.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h \
 thobjectname.h thmbuffer.h thtflength.h thtf.h thexception.h \
 thdatabase.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thattr.h thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thcsdata.h thdatareader.h thinput.h thproj.h \
 therion.h thlog.h
$(OUTDIR)/thsurvey.o: thsurvey.cxx thsurvey.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thobjectsrc.h thtfpwf.h thdata.h thdataleg.h \
 thobjectname.h thmbuffer.h thinfnan.h thtfangle.h thtf.h thtflength.h \
 thbuffer.h thexception.h thchenc.h thchencdata.h thparse.h thdatabase.h \
 thdb1d.h thobjectid.h thdb3d.h loch/lxMath.h thattr.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h
$(OUTDIR)/thsvg.o: thsvg.cxx thepsparse.h thpdfdbg.h thpdfdata.h therion.h thlog.h \
 thversion.h thconfig.h thbuffer.h thmbuffer.h thinput.h thexporter.h \
 thexport.h thobjectsrc.h thlayout.h thdataobject.h thperson.h thdate.h \
 thlayoutclr.h thstok.h thlayoutln.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h loch/icase.h thselector.h thlegenddata.h thtexfonts.h \
 thdouble.h
$(OUTDIR)/thsvxctrl.o: thsvxctrl.cxx thsvxctrl.h thdataleg.h thstok.h \
 thobjectname.h thmbuffer.h thobjectsrc.h thinfnan.h thdatabase.h \
 thbuffer.h thdb1d.h thobjectid.h thdb3d.h loch/lxMath.h thattr.h \
 thchenc.h thchencdata.h thparse.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h \
 thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thtmpdir.h thdata.h \
 thdataobject.h thtfangle.h thtf.h thtflength.h thtfpwf.h thexception.h \
 thinit.h thinput.h thconfig.h thexporter.h thexport.h thlayout.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h thselector.h \
 thsurvey.h thcsdata.h thlogfile.h therion.h thlog.h extern/img/img.h
$(OUTDIR)/thsymbolset.o: thsymbolset.cxx thsymbolset.h thsymbolsetlist.h \
 thlayoutclr.h thstok.h thpoint.h th2ddataobject.h thdataobject.h \
 thperson.h thdate.h thobjectsrc.h thdb2dpt.h thobjectname.h thmbuffer.h \
 thline.h tharea.h thdb2dab.h thexception.h thpdfdata.h thepsparse.h \
 thlayout.h thlayoutln.h thlocale.h thtexfonts.h thlang.h thlangdata.h \
 thtmpdir.h thcmdline.h thmpost.h thinit.h thbuffer.h thinput.h \
 thsymbolsets.h thlogfile.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dlp.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thscrapen.h thscraplp.h therion.h thlog.h
$(OUTDIR)/thsymbolsets.o: thsymbolsets.cxx thsymbolsets.h thsymbolsetlist.h
$(OUTDIR)/thtex.o: thtex.cxx thtex.h
$(OUTDIR)/thtexfonts.o: thtexfonts.cxx thtexfonts.h thtexenc.h thpdfdbg.h thinit.h \
 thbuffer.h thmbuffer.h thinput.h therion.h thlog.h
$(OUTDIR)/thtf.o: thtf.cxx thtf.h thexception.h thparse.h thbuffer.h thmbuffer.h \
 thstok.h
$(OUTDIR)/thtfangle.o: thtfangle.cxx therion.h thlog.h thparse.h thbuffer.h \
 thmbuffer.h thstok.h thtfangle.h thtf.h thexception.h thinfnan.h
$(OUTDIR)/thtflength.o: thtflength.cxx thtflength.h thtf.h thstok.h thexception.h \
 thparse.h thbuffer.h thmbuffer.h
$(OUTDIR)/thtfpwf.o: thtfpwf.cxx thtfpwf.h thexception.h thinfnan.h thparse.h \
 thbuffer.h thmbuffer.h thstok.h
$(OUTDIR)/thtmpdir.o: thtmpdir.cxx thtmpdir.h therion.h thlog.h thinit.h thbuffer.h \
 thmbuffer.h thinput.h
$(OUTDIR)/thtrans.o: thtrans.cxx thtrans.h thinfnan.h thdatabase.h thmbuffer.h \
 thbuffer.h thdb1d.h thobjectid.h thdataleg.h thstok.h thobjectname.h \
 thobjectsrc.h thdb3d.h loch/lxMath.h thattr.h thchenc.h thchencdata.h \
 thparse.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutclr.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h
$(OUTDIR)/thwarp.o: thwarp.cxx thwarp.h thpic.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thstok.h thobjectname.h thmbuffer.h thobjectsrc.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thbuffer.h \
 thscrap.h thdataobject.h thperson.h thdate.h thlayoutclr.h thdb2dpt.h \
 thdb2dcp.h thscraplo.h thscraplp.h thscrapen.h thmapstat.h \
 thlegenddata.h thsketch.h thtrans.h th2ddataobject.h thpoint.h \
 thconfig.h thinput.h thexporter.h thexport.h thlayout.h thlayoutln.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h thselector.h \
 thdatabase.h thdb2d.h thdb2dprj.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dxs.h thdb2dxm.h therion.h thlog.h
$(OUTDIR)/thwarpp.o: thwarpp.cxx thwarpp.h thwarp.h thpic.h thwarppme.h \
 thwarppdef.h therion.h thlog.h thtrans.h thinfnan.h thwarppt.h \
 thobjectsrc.h thobjectname.h thmbuffer.h thscrap.h thdataobject.h \
 thperson.h thdate.h thlayoutclr.h thstok.h thdb2dpt.h thdb2dcp.h \
 thscraplo.h thscraplp.h thdb1d.h thobjectid.h thdataleg.h thdb3d.h \
 loch/lxMath.h thattr.h thchenc.h thchencdata.h thparse.h thbuffer.h \
 thscrapen.h thmapstat.h thlegenddata.h thsketch.h th2ddataobject.h \
 thpoint.h thconfig.h thinput.h thexporter.h thexport.h thlayout.h \
 thlayoutln.h thsymbolset.h thsymbolsetlist.h thlocale.h loch/icase.h \
 thselector.h thdatabase.h thdb2d.h thdb2dprj.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dxs.h thdb2dxm.h
$(OUTDIR)/thwarppme.o: thwarppme.cxx thinfnan.h thwarppme.h thwarppdef.h therion.h \
 thlog.h thtrans.h
$(OUTDIR)/thwarppt.o: thwarppt.cxx thexception.h thwarppt.h thtrans.h thinfnan.h \
 thwarppme.h thwarppdef.h therion.h thlog.h
utest-main.o: utest-main.cxx extern/catch2/catch.hpp thinit.h thbuffer.h \
 thmbuffer.h thinput.h thproj.h thcsdata.h thstok.h
utest-proj.o: utest-proj.cxx extern/catch2/catch.hpp thproj.h thcsdata.h \
 thstok.h thcs.h
utest-str.o: utest-str.cxx extern/catch2/catch.hpp loch/icase.h thparse.h \
 thbuffer.h thmbuffer.h thstok.h thcsdata.h thsvg.h thlegenddata.h
utest-thdatastation.o: utest-thdatastation.cxx thdatastation.h \
 thdataobject.h thperson.h thdate.h thlayoutclr.h thstok.h thobjectsrc.h \
 extern/catch2/catch.hpp
utest-thdb2dab.o: utest-thdb2dab.cxx thdb2dab.h thobjectname.h \
 thmbuffer.h thobjectsrc.h extern/catch2/catch.hpp
utest-thdouble.o: utest-thdouble.cxx thdouble.h extern/catch2/catch.hpp
utest-thexception.o: utest-thexception.cxx thexception.h \
 extern/catch2/catch.hpp
utest-thlayoutln.o: utest-thlayoutln.cxx thlayoutln.h thlayoutclr.h \
 thstok.h thlayout.h thdataobject.h thperson.h thdate.h thobjectsrc.h \
 thsymbolset.h thsymbolsetlist.h thlocale.h extern/catch2/catch.hpp
utest-thlogfile.o: utest-thlogfile.cxx thlogfile.h thbuffer.h \
 extern/catch2/catch.hpp
utest-thobjectid.o: utest-thobjectid.cxx thobjectid.h \
 extern/catch2/catch.hpp
utest-thobjectsrc.o: utest-thobjectsrc.cxx thobjectsrc.h \
 extern/catch2/catch.hpp
utest-thscrapen.o: utest-thscrapen.cxx thscrapen.h \
 extern/catch2/catch.hpp
utest-thscraplo.o: utest-thscraplo.cxx thscraplo.h \
 extern/catch2/catch.hpp
