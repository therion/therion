# common therion objects
CMNOBJECTS = thdate.o extern/shpopen.o extern/dbfopen.o \
  thexception.o thbuffer.o thmbuffer.o thlogfile.o thtmpdir.o thlocale.o \
  thparse.o thcmdline.o thconfig.o thinput.o thchenc.o thdatabase.o \
  thdataobject.o thdatareader.o thsurvey.o thendsurvey.o thdata.o \
  thperson.o thtf.o thtfangle.o thtflength.o thtfpwf.o \
  thdataleg.o thobjectname.o thinfnan.o thlayout.o \
  thinit.o thdb1d.o thsvxctrl.o thdatastation.o thobjectid.o \
  thobjectsrc.o thgrade.o thlibrary.o thgeomag.o thbezier.o \
  thexport.o thexporter.o thselector.o extern/img.o \
  thexpmodel.o thdb2d00.o thscrapis.o thcsdata.o thexptable.o \
  thdb2d.o thscrap.o thendscrap.o th2ddataobject.o thdb2dprj.o \
  thdb2dpt.o thdb2dlp.o thdb2dab.o thdb2dji.o thdb2dmi.o thdb2dcp.o \
  thdb2dxm.o thdb2dxs.o thscraplo.o thscraplp.o thscrapen.o \
  thpoint.o thline.o tharea.o thlegenddata.o thmpost.o thsymbolsets.o \
  thjoin.o thmap.o thexpmap.o thlayoutln.o thlayoutclr.o thexpsys.o thexpuni.o \
  thconvert.o thpdf.o thpdfdbg.o thpdfdata.o thtexfonts.o \
  thsymbolset.o thlang.o thmapstat.o thexpdb.o thpic.o thsketch.o thproj.o \
  extern/lxMath.o extern/lxFile.o thcdt.o thdb3d.o thsurface.o thimport.o thsvg.o thepsparse.o \
  thtrans.o thwarpp.o thwarppt.o thwarppme.o thwarp.o thexpshp.o thattr.o thtex.o \
  therion.o extern/proj4/libproj.a 

# PLATFORM CONFIG

# PLATFORM LINUX
##CXX = g++
##CC = gcc
##POBJECTS =
##LOCHEXE = loch/loch
##CXXPFLAGS = -DTHLINUX
##CCPFLAGS = -DTHLINUX
##LDPFLAGS = -s
##THPLATFORM = LINUX
##THXTHMKCMD = ./therion


# PLATFORM DEBIAN
CXX = c++
CC = gcc
POBJECTS =
LOCHEXE = loch/loch
CXXPFLAGS = -DTHLINUX
CCPFLAGS = -DTHLINUX
LDPFLAGS = -s
THPLATFORM = LINUX
THXTHMKCMD = ./therion


# PLATFORM WIN32
##CXX = c++
##CC = gcc
##POBJECTS = extern/getopt.o extern/getopt1.o therion.res extern/getline.o 
##LOCHEXE = loch/loch
##CXXPFLAGS = -DTHWIN32
##CCPFLAGS = -DTHWIN32
##LDPFLAGS = -static-libgcc -static -s
##THPLATFORM = WIN32
##THXTHMKCMD = therion

# PLATFORM MACOSX
##CXX = c++
##CC = cc
##LOCHEXE =
##POBJECTS = extern/getopt.o extern/getopt1.o extern/getline.o
##CXXPFLAGS = -DTHMACOSX
##CCPFLAGS = -DTHMACOSX
##LDPFLAGS =
##THPLATFORM = MACOSX
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
##CCBFLAGS = -ggdb
##CXXBFLAGS = -ggdb -DTHDEBUG
##LDBFLAGS = 

# BUILD ENDCONFIG


# compiler settings
CXXFLAGS = -Wall $(CXXPFLAGS) $(CXXBFLAGS)
CCFLAGS = -Wall $(CCPFLAGS) $(CCBFLAGS)
OBJECTS = $(POBJECTS) $(CMNOBJECTS)


# linker settings
LIBS = 
LDFLAGS = $(LDBFLAGS)


.SUFFIXES:
.SUFFIXES: .cxx .o .h .c

.cxx.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(CCFLAGS) -o $@ $<

all: therion doc xtherion/xtherion $(LOCHEXE)

extern/proj4/libproj.a: extern/proj4/*.c extern/proj4/*.h
	make -C ./extern/proj4

therion:	$(OBJECTS)
	$(CXX) -Wall -o therion $(OBJECTS) $(LDFLAGS) $(LIBS) 


therion.res: therion.rc
	windres -i therion.rc -J rc -o therion.res -O coff

init:
	./therion --print-init-file > therion.ini

install: all
	tclsh makeinstall.tcl $(THPLATFORM)


minor-release:
	perl makerelease.pl

archive: clean
	perl makearchive2.pl

release: clean
	perl makearchive.pl

binary: all doc
	perl makebinary.pl $(THPLATFORM)
  
depend:
	perl makedepend.pl > Makefile.dep
	perl maketest.pl Makefile.dep
	perl makefile.pl mv Makefile.dep Makefile
	$(CXX) -MM $(CXXFLAGS) *.cxx >> Makefile

library:
	$(THXTHMKCMD) --print-library-src thlibrarydata.thcfg > thlibrarydata.log
	$(THXTHMKCMD) --print-xtherion-src > xtherion/therion.tcl
	perl makelibrary.pl thlibrarydata.log > thlibrarydata.tmp
	perl maketest.pl thlibrarydata.tmp
	perl makefile.pl mv thlibrarydata.tmp thlibrarydata.cxx

thlibrarydata.cxx: thlang/texts.txt
	make library

xtherion/xtherion: xtherion/*.tcl
	make -C ./xtherion

loch/loch: loch/*.h loch/*.cxx loch/*.c
	make -C ./loch

doc: thbook/thbook.pdf

thbook: thbook/thbook.pdf

samples: samples.doc/index.tex

samples.doc/index.tex:
	make -C ./samples

thbook/thbook.pdf: thbook/*.tex
	make -C ./thbook
  
clean:
	make -C ./xtherion clean
	make -C ./loch clean
	make -C ./samples clean
	make -C ./extern/proj4 clean
	perl makefile.pl rm -q therion ./xtherion/xtherion ./xtherion/xtherion.tcl therion.exe *~ *.log *.o thchencdata/*~ .xtherion.dat
	perl makefile.pl rm -q xtherion/*~ .xth_thconfig_xth xtherion/screendump thlang/*~
	perl makefile.pl rm -q extern/*.o extern/*~ samples/*~ samples/*.log
	perl makefile.pl rm -q symbols.html therion.res
	perl makefile.pl rm -q tri/*.o tri/*~
	perl makefile.pl rm -q tex/*~
	perl makefile.pl rm -q mpost/*~ examples/*~ examples/therion.log
	perl makefile.pl rm -q core symbols.xhtml cave.kml
	perl makefile.pl rm -q data.3d data.svx data.pos data.pts data.err data.plt
	perl makefile.pl rm -q cave.3d cave.lox cave.thm cave.pdf cave.sql cave.xhtml therion.tcl cave_a.pdf cave_m.pdf cave.vrml cave.wrl cave.3dmf cave.svg cave.tlx
	perl makefile.pl rm -q ./thbook/*~ ./thbook/thbook.log ./thbook/thbook.pdf ./lib/*~ ./mpost/*~ ./tex/*~
	perl makefile.pl rmdir -q doc thTMPDIR samples.doc symbols cave.shp tests/.doc

thmpost.h: mpost/*.mp
	make -C ./mpost

thsymbolsetlist.h: thsymbolsetlist.pl mpost/thTrans.mp
	perl thsymbolsetlist.pl
  
thtex.h: tex/*.tex
	make -C ./tex

thcsdata.h: thcsdata.tcl
	tclsh thcsdata.tcl
  
update:
	make -C ./thlang update

unixify: clean
	tclsh makeunixify.tcl
  
thlangdata.cxx: thlang/texts.txt
	make -C ./thlang

config-debug:
	perl makeconfig.pl BUILD DEBUG
	make -C ./loch config-debug
  
config-release:
	perl makeconfig.pl BUILD RELEASE
	make -C ./loch config-release

config-oxygen:
	perl makeconfig.pl BUILD OXYGEN
	make -C ./loch config-oxygen
  
config-ozone:
	perl makeconfig.pl BUILD OZONE
	make -C ./loch config-ozone

config-debian:
	perl makeconfig.pl PLATFORM DEBIAN
	make -C ./loch config-debian
  
config-linux:
	perl makeconfig.pl PLATFORM LINUX
	make -C ./loch config-linux
  
config-win32:
	perl makeconfig.pl PLATFORM WIN32
	make -C ./loch config-win32
  
config-macosx:
	perl makeconfig.pl PLATFORM MACOSX
	make -C ./loch config-macosx

# external sources
extern/getopt.o: extern/getopt.c extern/getopt.h
extern/getopt1.o: extern/getopt1.c
extern/img.o: extern/img.c extern/img.h
tri/construct.o: tri/triangulate.h tri/construct.c
tri/misc.o: tri/triangulate.h tri/misc.c
tri/monotone.o: tri/triangulate.h tri/monotone.c
tri/tri.o: tri/triangulate.h tri/tri.c

# DEPENDENCIES
th2ddataobject.o: th2ddataobject.cxx th2ddataobject.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h thsymbolset.h \
 thsymbolsetlist.h
tharea.o: tharea.cxx tharea.h th2ddataobject.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h thexpmap.h thexport.h \
 thsymbolset.h thsymbolsetlist.h thlayout.h thlocale.h thlang.h \
 thlangdata.h thline.h
thattr.o: thattr.cxx thattr.h thmbuffer.h thchenc.h thchencdata.h \
 thparse.h thbuffer.h extern/shapefil.h thexception.h therion.h
thbezier.o: thbezier.cxx thbezier.h
thbuffer.o: thbuffer.cxx thbuffer.h
thcdt.o: thcdt.cxx thcdt.h thcdt/geom2d.h thcdt/myMacro.h thcdt/cdt.h \
 thcdt/cdt.cpp
thchenc.o: thchenc.cxx thchenc.h thchencdata.h thparse.h thbuffer.h \
 thmbuffer.h thchencdata.cxx therion.h thexception.h
thchencdata.o: thchencdata.cxx
thcmdline.o: thcmdline.cxx thcmdline.h therion.h thlogfile.h thbuffer.h \
 thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h thexport.h \
 thobjectsrc.h thselector.h thtmpdir.h extern/getopt.h
thconfig.o: thconfig.cxx thconfig.h thbuffer.h thmbuffer.h thinput.h \
 thparse.h thexporter.h thexport.h thobjectsrc.h thselector.h therion.h \
 thlang.h thlangdata.h thchenc.h thchencdata.h thexception.h thdatabase.h \
 thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thinfnan.h \
 thdb1d.h thobjectid.h thdb3d.h thattr.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thdatareader.h thcsdata.h thproj.h thlogfile.h thinit.h \
 thgeomag.h thgeomagdata.h thlayout.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h thsketch.h thpic.h
thconvert.o: thconvert.cxx thpdfdbg.h thexception.h therion.h thbuffer.h \
 thpdfdata.h thepsparse.h thtexfonts.h
thcsdata.o: thcsdata.cxx thcsdata.h thparse.h thbuffer.h thmbuffer.h
thdata.o: thdata.cxx thdata.h thdataleg.h thparse.h thbuffer.h \
 thmbuffer.h thobjectname.h therion.h thobjectsrc.h thinfnan.h \
 thdataobject.h thdatabase.h thdb1d.h thobjectid.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
 thexception.h thsurvey.h thgrade.h thcsdata.h thdatareader.h thinput.h
thdatabase.o: thdatabase.cxx thdatabase.h thdataobject.h thperson.h \
 thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
 therion.h thobjectsrc.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thsurvey.h thtfpwf.h thendsurvey.h thexception.h thdata.h \
 thtfangle.h thtf.h thtflength.h thcmdline.h thdatastation.h thgrade.h \
 thlayout.h thsymbolset.h thsymbolsetlist.h thlocale.h thscrap.h \
 thsketch.h thpic.h thtrans.h th2ddataobject.h thpoint.h thline.h \
 tharea.h thjoin.h thmap.h thimport.h thsurface.h thendscrap.h
thdataleg.o: thdataleg.cxx thdataleg.h thparse.h thbuffer.h thmbuffer.h \
 thobjectname.h therion.h thobjectsrc.h thinfnan.h
thdataobject.o: thdataobject.cxx thdataobject.h thdatabase.h thmbuffer.h \
 thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
 thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thexception.h thsurvey.h thtfpwf.h thconfig.h thinput.h \
 thexporter.h thexport.h thselector.h thcsdata.h thproj.h
thdatareader.o: thdatareader.cxx thdatareader.h thdatabase.h \
 thdataobject.h thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h \
 thdataleg.h thobjectname.h therion.h thobjectsrc.h thinfnan.h thdb1d.h \
 thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thinput.h thexception.h
thdatastation.o: thdatastation.cxx thdatastation.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h
thdate.o: thdate.cxx thdate.h thparse.h thbuffer.h thmbuffer.h \
 thexception.h therion.h
thdb1d.o: thdb1d.cxx thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thparse.h thbuffer.h thmbuffer.h thobjectname.h therion.h thobjectsrc.h \
 thdb3d.h thattr.h thchenc.h thchencdata.h thsurvey.h thdataobject.h \
 thdatabase.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thtfpwf.h thsvxctrl.h thexception.h thdata.h thtfangle.h \
 thtf.h thtflength.h thpoint.h th2ddataobject.h thlogfile.h thsurface.h \
 thlocale.h thinit.h thinput.h thconfig.h thexporter.h thexport.h \
 thselector.h thtrans.h extern/lxMath.h
thdb2d.o: thdb2d.cxx thdb2d.h thinfnan.h thdb2dprj.h thparse.h thbuffer.h \
 thmbuffer.h thmapstat.h thdate.h thperson.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thobjectname.h therion.h thobjectsrc.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thdb1d.h thobjectid.h thdataleg.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thexception.h thdatabase.h \
 thdataobject.h thtfangle.h thtf.h tharea.h th2ddataobject.h thmap.h \
 thjoin.h thpoint.h thline.h thscrap.h thsketch.h thpic.h thtrans.h \
 thsurvey.h thtfpwf.h thlogfile.h thlayout.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thexpmap.h thexport.h thlang.h thlangdata.h \
 thconfig.h thinput.h thexporter.h thselector.h thtmpdir.h thinit.h
thdb2d00.o: thdb2d00.cxx thdb2d.h thinfnan.h thdb2dprj.h thparse.h \
 thbuffer.h thmbuffer.h thmapstat.h thdate.h thperson.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thobjectname.h therion.h thobjectsrc.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thdb1d.h thobjectid.h thdataleg.h \
 thdb3d.h thattr.h thchenc.h thchencdata.h thexception.h thdatabase.h \
 thdataobject.h thtfangle.h thtf.h tharea.h th2ddataobject.h thmap.h \
 thjoin.h thpoint.h thline.h thscrap.h thsketch.h thpic.h thtrans.h \
 thsurvey.h thtfpwf.h thlogfile.h thlayout.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thconfig.h thinput.h thexporter.h \
 thexport.h thselector.h
thdb2dab.o: thdb2dab.cxx thdb2dab.h thobjectname.h thmbuffer.h therion.h \
 thobjectsrc.h
thdb2dcp.o: thdb2dcp.cxx thdb2dcp.h thdb2dpt.h
thdb2dji.o: thdb2dji.cxx thdb2dji.h thobjectname.h thmbuffer.h therion.h \
 thdatabase.h thdataobject.h thperson.h thparse.h thbuffer.h thdate.h \
 thdataleg.h thobjectsrc.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thexception.h
thdb2dlp.o: thdb2dlp.cxx thdb2dlp.h thdb2dpt.h thline.h th2ddataobject.h \
 thdataobject.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h \
 thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h \
 thmapstat.h thdate.h thperson.h thlegenddata.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thexpmap.h thexport.h thsymbolset.h \
 thsymbolsetlist.h thlayout.h thlocale.h thlang.h thlangdata.h
thdb2dmi.o: thdb2dmi.cxx thdb2dmi.h thobjectname.h thmbuffer.h therion.h \
 thobjectsrc.h thparse.h thbuffer.h thexception.h
thdb2dprj.o: thdb2dprj.cxx thdb2dprj.h thparse.h thbuffer.h thmbuffer.h \
 thmapstat.h thdate.h thperson.h thlegenddata.h thinfnan.h
thdb2dpt.o: thdb2dpt.cxx thdb2dpt.h thexpmap.h thexport.h thparse.h \
 thbuffer.h thmbuffer.h thobjectsrc.h thsymbolset.h thsymbolsetlist.h \
 thlayout.h thdataobject.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thobjectname.h therion.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thlocale.h thlang.h thlangdata.h
thdb2dxm.o: thdb2dxm.cxx thdb2dxm.h thdb2dmi.h thobjectname.h thmbuffer.h \
 therion.h thobjectsrc.h thparse.h thbuffer.h
thdb2dxs.o: thdb2dxs.cxx thdb2dxs.h thdb2dmi.h thobjectname.h thmbuffer.h \
 therion.h thobjectsrc.h thparse.h thbuffer.h
thdb3d.o: thdb3d.cxx thdb3d.h
thendscrap.o: thendscrap.cxx thendscrap.h thdataobject.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thexception.h
thendsurvey.o: thendsurvey.cxx thendsurvey.h thdataobject.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thexception.h
thepsparse.o: thepsparse.cxx thepsparse.h thpdfdbg.h thexception.h \
 therion.h thbuffer.h thpdfdata.h thtexfonts.h thconvert.h
therion.o: therion.cxx therion.h thlogfile.h thbuffer.h thtmpdir.h \
 thcmdline.h thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h \
 thexport.h thobjectsrc.h thselector.h thchenc.h thchencdata.h \
 thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
 thobjectname.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h thattr.h \
 thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h \
 thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
 thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thdatareader.h \
 thexception.h thlibrary.h thinit.h thgrade.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thlayout.h thsymbolset.h thsymbolsetlist.h \
 thlocale.h thpoint.h th2ddataobject.h thline.h tharea.h thversion.h \
 thtexfonts.h thlang.h thlangdata.h
thexception.o: thexception.cxx thexception.h therion.h thbuffer.h
thexpdb.o: thexpdb.cxx thexpdb.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thexception.h therion.h thdatabase.h \
 thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thinfnan.h \
 thdb1d.h thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thinit.h thinput.h thsurvey.h
thexpmap.o: thexpmap.cxx thexpmap.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thsymbolset.h thsymbolsetlist.h thlayout.h \
 thdataobject.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h therion.h thdb3d.h thattr.h thchenc.h thchencdata.h \
 thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thlocale.h thlang.h thlangdata.h thexporter.h thexception.h thtmpdir.h \
 thscrap.h thsketch.h thpic.h thtrans.h thpoint.h th2ddataobject.h \
 thline.h thmap.h thconfig.h thinput.h thselector.h thinit.h thlogfile.h \
 thconvert.h thpdf.h thpdfdata.h thepsparse.h thmpost.h thtex.h \
 thcmdline.h thtexfonts.h thsurvey.h thtfpwf.h thcsdata.h thproj.h \
 thsurface.h thtflength.h thtf.h extern/lxMath.h thsvg.h extern/img.h
thexpmodel.o: thexpmodel.cxx thexpmodel.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thexception.h therion.h thdatabase.h \
 thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thinfnan.h \
 thdb1d.h thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thsurvey.h extern/img.h thscrap.h thsketch.h \
 thpic.h thtrans.h thsurface.h extern/lxFile.h thconfig.h thinput.h \
 thexporter.h thselector.h thcsdata.h thproj.h
thexport.o: thexport.cxx thexport.h thparse.h thbuffer.h thmbuffer.h \
 thobjectsrc.h thexception.h therion.h thconfig.h thinput.h thexporter.h \
 thselector.h thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
 thobjectname.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h
thexporter.o: thexporter.cxx thexporter.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thexception.h therion.h thconfig.h thinput.h \
 thselector.h thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
 thobjectname.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thexpmodel.h thexpmap.h thsymbolset.h thsymbolsetlist.h thlayout.h \
 thlocale.h thlang.h thlangdata.h thexpdb.h thexpsys.h thexptable.h
thexpshp.o: thexpshp.cxx thexpmap.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thsymbolset.h thsymbolsetlist.h thlayout.h \
 thdataobject.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h therion.h thdb3d.h thattr.h thchenc.h thchencdata.h \
 thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thlocale.h thlang.h thlangdata.h thexporter.h thexception.h thmap.h \
 thsketch.h thpic.h thconfig.h thinput.h thselector.h thtmpdir.h thinit.h \
 thlogfile.h thcmdline.h thsurvey.h thtfpwf.h thsurface.h thtflength.h \
 thtf.h extern/lxMath.h extern/shapefil.h thexpmodel.h thcsdata.h \
 thexpshp.h thscrap.h thtrans.h thpoint.h th2ddataobject.h thline.h \
 tharea.h
thexpsys.o: thexpsys.cxx thexpsys.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thexception.h therion.h thdatabase.h \
 thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thinfnan.h \
 thdb1d.h thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h
thexptable.o: thexptable.cxx thexptable.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thattr.h thchenc.h thchencdata.h thexception.h \
 therion.h thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
 thobjectname.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thexporter.h thinit.h thinput.h thsurvey.h \
 thscrap.h thsketch.h thpic.h thtrans.h thpoint.h th2ddataobject.h \
 thcsdata.h thproj.h thconfig.h thselector.h
thexpuni.o: thexpuni.cxx thexpmap.h thexport.h thparse.h thbuffer.h \
 thmbuffer.h thobjectsrc.h thsymbolset.h thsymbolsetlist.h thlayout.h \
 thdataobject.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thobjectname.h therion.h thdb3d.h thattr.h thchenc.h thchencdata.h \
 thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thlocale.h thlang.h thlangdata.h thexporter.h thexception.h thmap.h \
 thsketch.h thpic.h thconfig.h thinput.h thselector.h thtmpdir.h \
 thcsdata.h thinit.h thlogfile.h thcmdline.h thsurvey.h thtfpwf.h \
 thsurface.h thtflength.h thtf.h extern/lxMath.h extern/shapefil.h \
 thexpmodel.h thexpuni.h thscrap.h thtrans.h thpoint.h th2ddataobject.h \
 thline.h tharea.h thproj.h
thgeomag.o: thgeomag.cxx thgeomagdata.h
thgrade.o: thgrade.cxx thgrade.h thdataobject.h thdatabase.h thmbuffer.h \
 thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
 thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
 thexception.h
thimport.o: thimport.cxx thimport.h thdataobject.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thexception.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thsurvey.h thendsurvey.h extern/img.h
thinfnan.o: thinfnan.cxx thinfnan.h
thinit.o: thinit.cxx thinit.h thbuffer.h thmbuffer.h thinput.h thparse.h \
 thchenc.h thchencdata.h therion.h thconfig.h thexporter.h thexport.h \
 thobjectsrc.h thselector.h thexception.h thtexfonts.h thlang.h \
 thlangdata.h thlocale.h thtmpdir.h
thinput.o: thinput.cxx thinput.h thbuffer.h thmbuffer.h thparse.h \
 thchenc.h thchencdata.h therion.h thexception.h
thjoin.o: thjoin.cxx thjoin.h thdataobject.h thdatabase.h thmbuffer.h \
 thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
 thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thexception.h
thlang.o: thlang.cxx thlang.h thlangdata.h thparse.h thbuffer.h \
 thmbuffer.h thlangdata.cxx thinit.h thinput.h thconfig.h thexporter.h \
 thexport.h thobjectsrc.h thselector.h thdatabase.h thdataobject.h \
 thperson.h thdate.h thdataleg.h thobjectname.h therion.h thinfnan.h \
 thdb1d.h thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h
thlangdata.o: thlangdata.cxx
thlayout.o: thlayout.cxx thlayout.h thdataobject.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 thexception.h thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
 thpdfdata.h thepsparse.h thlang.h thlangdata.h thcsdata.h thconfig.h \
 thinput.h thexporter.h thexport.h thselector.h
thlayoutln.o: thlayoutln.cxx thlayoutln.h thlayout.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thscrapen.h thscraplp.h thsymbolset.h thsymbolsetlist.h thlocale.h
thlegenddata.o: thlegenddata.cxx thlegenddata.h
thlibrary.o: thlibrary.cxx thlibrary.h thlibrarydata.cxx thdatabase.h \
 thdataobject.h thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h \
 thdataleg.h thobjectname.h therion.h thobjectsrc.h thinfnan.h thdb1d.h \
 thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thlayout.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thlang.h thlangdata.h thgrade.h thdata.h \
 thtfangle.h thtf.h thtflength.h thtfpwf.h
thlibrarydata.o: thlibrarydata.cxx thdatabase.h thdataobject.h thperson.h \
 thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
 therion.h thobjectsrc.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thlayout.h thsymbolset.h thsymbolsetlist.h thlocale.h \
 thlang.h thlangdata.h thgrade.h thdata.h thtfangle.h thtf.h thtflength.h \
 thtfpwf.h
thline.o: thline.cxx thline.h th2ddataobject.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h thexpmap.h thexport.h \
 thsymbolset.h thsymbolsetlist.h thlayout.h thlocale.h thlang.h \
 thlangdata.h thtflength.h thtf.h thtexfonts.h thscrap.h thsketch.h \
 thpic.h thtrans.h
thlocale.o: thlocale.cxx thlocale.h thparse.h thbuffer.h thmbuffer.h \
 thexception.h therion.h thlang.h thlangdata.h thinit.h thinput.h
thlogfile.o: thlogfile.cxx thlogfile.h thbuffer.h therion.h
thmap.o: thmap.cxx thmap.h thdataobject.h thdatabase.h thmbuffer.h \
 thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
 thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thexception.h thscrap.h thsketch.h thpic.h thtrans.h \
 thtflength.h thtf.h
thmapstat.o: thmapstat.cxx thmapstat.h thdate.h thperson.h thlegenddata.h \
 thscrap.h thdataobject.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h \
 thobjectid.h thinfnan.h thdataleg.h thparse.h thobjectname.h therion.h \
 thobjectsrc.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thsketch.h thpic.h thtrans.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thmap.h thlayout.h thsymbolset.h \
 thsymbolsetlist.h thlocale.h thlang.h thlangdata.h thtexfonts.h \
 thsurvey.h
thmbuffer.o: thmbuffer.cxx thmbuffer.h
thmpost.o: thmpost.cxx thmpost.h
thobjectid.o: thobjectid.cxx thobjectid.h
thobjectname.o: thobjectname.cxx thobjectname.h thmbuffer.h therion.h \
 thexception.h thbuffer.h thparse.h thdatabase.h thdataobject.h \
 thperson.h thdate.h thdataleg.h thobjectsrc.h thinfnan.h thdb1d.h \
 thobjectid.h thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h \
 thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thsurvey.h thtfpwf.h
thobjectsrc.o: thobjectsrc.cxx thobjectsrc.h
thparse.o: thparse.cxx thparse.h thbuffer.h thmbuffer.h therion.h \
 thlang.h thlangdata.h thtexfonts.h thinfnan.h thdatabase.h \
 thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h \
 thobjectsrc.h thdb1d.h thobjectid.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h thdb2dpt.h \
 thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thtflength.h \
 thtf.h thexception.h
thpdf.o: thpdf.cxx thpdfdbg.h thexception.h therion.h thbuffer.h \
 thpdfdata.h thepsparse.h thtexfonts.h thlang.h thlangdata.h thparse.h \
 thmbuffer.h thversion.h thchenc.h thchencdata.h
thpdfdata.o: thpdfdata.cxx thpdfdata.h thepsparse.h thlang.h thlangdata.h \
 thparse.h thbuffer.h thmbuffer.h
thpdfdbg.o: thpdfdbg.cxx thpdfdbg.h thexception.h therion.h thbuffer.h \
 thpdfdata.h thepsparse.h
thperson.o: thperson.cxx thperson.h thdatabase.h thdataobject.h thparse.h \
 thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h therion.h \
 thobjectsrc.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thexception.h
thpic.o: thpic.cxx thpic.h thbuffer.h thdatabase.h thdataobject.h \
 thperson.h thparse.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
 therion.h thobjectsrc.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thinit.h thinput.h thtmpdir.h thexception.h
thpoint.o: thpoint.cxx thpoint.h th2ddataobject.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thexception.h thexpmap.h thexport.h \
 thsymbolset.h thsymbolsetlist.h thlayout.h thlocale.h thlang.h \
 thlangdata.h thtflength.h thtf.h thtexfonts.h thscrap.h thsketch.h \
 thpic.h thtrans.h
thproj.o: thproj.cxx thexception.h therion.h thbuffer.h \
 extern/proj4/proj_api.h
thscrap.o: thscrap.cxx thscrap.h thdataobject.h thdatabase.h thmbuffer.h \
 thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
 thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thsketch.h thpic.h thtrans.h thexception.h thtflength.h \
 thtf.h th2ddataobject.h thline.h thpoint.h thscrapis.h thsurvey.h \
 thtfpwf.h thsymbolset.h thsymbolsetlist.h thcsdata.h
thscrapen.o: thscrapen.cxx thscrapen.h
thscrapis.o: thscrapis.cxx thscrapis.h thdb3d.h therion.h thdb2dlp.h \
 thdb2dpt.h thscraplo.h thline.h th2ddataobject.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h thobjectsrc.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
 thdb2dxm.h thlayoutln.h thscrapen.h thscraplp.h thconfig.h thinput.h \
 thexporter.h thexport.h thselector.h thcdt.h
thscraplo.o: thscraplo.cxx thscraplo.h
thscraplp.o: thscraplp.cxx thscraplp.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thbuffer.h thmbuffer.h thobjectname.h therion.h \
 thobjectsrc.h thdb3d.h thattr.h thchenc.h thchencdata.h thscrap.h \
 thdataobject.h thdatabase.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thsketch.h thpic.h thtrans.h thexpmap.h thexport.h \
 thsymbolset.h thsymbolsetlist.h thlayout.h thlocale.h thlang.h \
 thlangdata.h extern/lxMath.h
thselector.o: thselector.cxx thselector.h thexception.h therion.h \
 thbuffer.h thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h \
 thexport.h thobjectsrc.h thdatabase.h thdataobject.h thperson.h thdate.h \
 thdataleg.h thobjectname.h thinfnan.h thdb1d.h thobjectid.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thsurvey.h thtfpwf.h th2ddataobject.h thscrap.h thsketch.h \
 thpic.h thtrans.h thmap.h
thsketch.o: thsketch.cxx therion.h thsketch.h thpic.h thscrap.h \
 thdataobject.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thparse.h thobjectname.h thobjectsrc.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thtrans.h thwarpp.h thwarp.h \
 thwarppme.h thwarppdef.h thwarppt.h thconfig.h thinput.h thexporter.h \
 thexport.h thselector.h
thsurface.o: thsurface.cxx thsurface.h thdb3d.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
 thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thtflength.h thtf.h thexception.h thcsdata.h \
 thdatareader.h thinput.h
thsurvey.o: thsurvey.cxx thsurvey.h thdataobject.h thdatabase.h \
 thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
 thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thattr.h \
 thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
 thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thtfpwf.h thexception.h thtfangle.h thtf.h \
 thdata.h thtflength.h
thsvg.o: thsvg.cxx thepsparse.h thpdfdbg.h thexception.h therion.h \
 thbuffer.h thpdfdata.h thversion.h thlegenddata.h
thsvxctrl.o: thsvxctrl.cxx thsvxctrl.h thdataleg.h thparse.h thbuffer.h \
 thmbuffer.h thobjectname.h therion.h thobjectsrc.h thinfnan.h \
 thdatabase.h thdataobject.h thperson.h thdate.h thdb1d.h thobjectid.h \
 thdb3d.h thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h \
 thmapstat.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
 thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
 thscrapen.h thscraplp.h thtmpdir.h thdata.h thtfangle.h thtf.h \
 thtflength.h thtfpwf.h thexception.h thinit.h thinput.h thconfig.h \
 thexporter.h thexport.h thselector.h thsurvey.h thlogfile.h extern/img.h
thsymbolset.o: thsymbolset.cxx thsymbolset.h thsymbolsetlist.h thparse.h \
 thbuffer.h thmbuffer.h thpoint.h th2ddataobject.h thdataobject.h \
 thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thobjectname.h \
 therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h thchencdata.h \
 thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thlegenddata.h \
 thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
 thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
 thline.h tharea.h thexception.h thpdfdata.h thepsparse.h thlayout.h \
 thlocale.h thtexfonts.h thlang.h thlangdata.h thtmpdir.h thcmdline.h \
 thmpost.h thinit.h thinput.h thsymbolsets.h thlogfile.h
thsymbolsets.o: thsymbolsets.cxx thsymbolsets.h thsymbolsetlist.h
thtex.o: thtex.cxx thtex.h
thtexenc.o: thtexenc.cxx
thtexfonts.o: thtexfonts.cxx thtexfonts.h thtexenc.cxx thpdfdbg.h \
 thexception.h therion.h thbuffer.h thinit.h thmbuffer.h thinput.h \
 thparse.h thpdfdata.h thepsparse.h
thtf.o: thtf.cxx thtf.h thexception.h therion.h thbuffer.h thparse.h \
 thmbuffer.h
thtfangle.o: thtfangle.cxx thparse.h thbuffer.h thmbuffer.h thtfangle.h \
 thtf.h thexception.h therion.h thinfnan.h
thtflength.o: thtflength.cxx thtflength.h thtf.h thparse.h thbuffer.h \
 thmbuffer.h thexception.h therion.h
thtfpwf.o: thtfpwf.cxx thtfpwf.h thexception.h therion.h thbuffer.h \
 thinfnan.h thparse.h thmbuffer.h
thtmpdir.o: thtmpdir.cxx thtmpdir.h thbuffer.h therion.h thinit.h \
 thmbuffer.h thinput.h thparse.h
thtrans.o: thtrans.cxx thtrans.h thinfnan.h thdatabase.h thdataobject.h \
 thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h \
 thobjectname.h therion.h thobjectsrc.h thdb1d.h thobjectid.h thdb3d.h \
 thattr.h thchenc.h thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h
thwarp.o: thwarp.cxx thwarp.h thpic.h thsketch.h thdb1d.h thobjectid.h \
 thinfnan.h thdataleg.h thparse.h thbuffer.h thmbuffer.h thobjectname.h \
 therion.h thobjectsrc.h thdb3d.h thattr.h thchenc.h thchencdata.h \
 thscrap.h thdataobject.h thdatabase.h thdb2d.h thdb2dprj.h thmapstat.h \
 thdate.h thperson.h thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
 thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
 thlayoutln.h thscrapen.h thscraplp.h thtrans.h th2ddataobject.h \
 thpoint.h
thwarpp.o: thwarpp.cxx thwarpp.h thwarp.h thpic.h thsketch.h thwarppme.h \
 thwarppdef.h therion.h thtrans.h thinfnan.h thwarppt.h thdataobject.h \
 thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thdataleg.h \
 thparse.h thobjectname.h thobjectsrc.h thdb3d.h thattr.h thchenc.h \
 thchencdata.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h \
 thlegenddata.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
 thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
 thscraplp.h thscrap.h th2ddataobject.h thpoint.h
thwarppme.o: thwarppme.cxx thinfnan.h thwarppme.h thwarppdef.h therion.h \
 thtrans.h
thwarppt.o: thwarppt.cxx thwarppt.h thtrans.h thinfnan.h thwarppme.h \
 thwarppdef.h therion.h
