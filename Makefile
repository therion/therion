# common therion objects
CMNOBJECTS = thdate.o \
  thexception.o thbuffer.o thmbuffer.o thlogfile.o thtmpdir.o \
  thparse.o thcmdline.o thconfig.o thinput.o thchenc.o thdatabase.o \
  thdataobject.o thdatareader.o thsurvey.o thendsurvey.o thdata.o \
  thperson.o thtf.o thtfangle.o thtflength.o thtfpwf.o \
  thdataleg.o thobjectname.o thinfnan.o thlayout.o \
  thinit.o thdb1d.o thsvxctrl.o thdatastation.o thobjectid.o \
  thobjectsrc.o thgrade.o thlibrary.o \
  thexport.o thexporter.o thselector.o extern/img.o \
  thexpmodel.o thdb2d00.o thscrapis.o \
  thdb2d.o thscrap.o thendscrap.o th2ddataobject.o thdb2dprj.o \
  thdb2dpt.o thdb2dlp.o thdb2dab.o thdb2dji.o thdb2dmi.o thdb2dcp.o \
  thdb2dxm.o thdb2dxs.o thscraplo.o thscraplp.o thscrapen.o \
  thpoint.o thline.o tharea.o \
  thjoin.o thmap.o thexpmap.o thlayoutln.o \
  thconvert.o thpdf.o thpdfdbg.o thpdfdata.o thtexfonts.o \
  thsymbolset.o thlang.o thmapstat.o thexpdb.o \
  thcdt.o thdb3d.o \
  therion.o

# PLATFORM CONFIG

# PLATFORM LINUX
CXX = g++
CC = gcc
POBJECTS =
CXXPFLAGS = -DTHLINUX
CCPFLAGS = -DTHLINUX
THPLATFORM = LINUX
THXTHMKCMD = ./therion

# PLATFORM WIN32
##CXX = c++
##CC = gcc
##POBJECTS = extern/getopt.o extern/getopt1.o therion.res
##CXXPFLAGS = -DTHWIN32
##CCPFLAGS = -DTHWIN32
##THPLATFORM = WIN32
##THXTHMKCMD = therion

# PLATFORM MACOSX
##CXX = c++
##CC = cc
##POBJECTS = extern/getopt.o extern/getopt1.o
##CXXPFLAGS = -DTHMACOSX
##CCPFLAGS = -DTHMACOSX
##THPLATFORM = MACOSX
##THXTHMKCMD = ./therion

# PLATFORM ENDCONFIG


# BUILD CONFIG

# BUILD OZONE
##CCBFLAGS = -O3
##CXXBFLAGS = -O3

# BUILD OXYGEN
##CCBFLAGS = -O2
##CXXBFLAGS = -O2

# BUILD RELEASE
CCBFLAGS = 
CXXBFLAGS = 

# BUILD DEBUG
##CCBFLAGS = -ggdb
##CXXBFLAGS = -ggdb -DTHDEBUG

# BUILD ENDCONFIG


# compiler settings
CXXFLAGS = -Wall -D_GNU_SOURCE $(CXXPFLAGS) $(CXXBFLAGS)
CCFLAGS = -Wall -D_GNU_SOURCE $(CCPFLAGS) $(CCBFLAGS)
OBJECTS = $(POBJECTS) $(CMNOBJECTS)

# linker settings
LIBS =
LDFLAGS =


.SUFFIXES:
.SUFFIXES: .cxx .o .h .c

.cxx.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(CCFLAGS) -o $@ $<

all: therion xtherion/xtherion doc

therion:	$(OBJECTS)
	$(CXX) -Wall -o therion $(OBJECTS) $(LDFLAGS) $(LIBS) 


therion.res: therion.rc
	windres -i therion.rc -I rc -o therion.res -O coff

init:
	./therion --print-init-file > therion.ini

install: all
	perl makeinstall.pl $(THPLATFORM)


release: clean
	perl makerelease.pl

archive: clean
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


xtherion/xtherion: xtherion/*.tcl
	make -C ./xtherion

doc: thbook/thbook.pdf

thbook/thbook.pdf: thbook/*.tex
	make -C ./thbook
  
clean:
	make -C ./xtherion clean
	perl makefile.pl rm -q therion ./xtherion/xtherion ./xtherion/xtherion.tcl therion.exe *~ *.log *.o thchencdata/*~
	perl makefile.pl rm -q xtherion/*~ .xth-thconfig xtherion/screendump thlang/*~
	perl makefile.pl rm -q extern/*.o extern/*~
	perl makefile.pl rm -q tri/*.o tri/*~
	perl makefile.pl rm -q tests/*~
	perl makefile.pl rm -q tex/*~
	perl makefile.pl rm -q mpost/*~
	perl makefile.pl rm -q core
	perl makefile.pl rm -q data.3d data.svx data.pos data.pts data.err data.plt
	perl makefile.pl rm -q cave.3d cave.thm cave.pdf cave.sql cave.svg therion.tcl cave_a.pdf cave_m.pdf cave.vrml cave.wrl cave.3dmf
	perl makefile.pl rm -q ./thbook/*~ ./thbook/thbook.log ./thbook/thbook.pdf ./lib/*~ ./mpost/*~ ./tex/*~
	perl makefile.pl rm -q ./thtom/linux/Tom0.2/*.so ./thtom/linux/Tom0.2/*.o ./thtom/linux/Tom0.2/tom ./thtom/linux/Tom0.2/stom
	perl makefile.pl rm -q ./thtom/win/Tom0.2/*.dll ./thtom/win/Tom0.2/*.o ./thtom/win/Tom0.2/tom.exe ./thtom/win/Tom0.2/stom.exe
	perl makefile.pl rmdir -q doc thTMPDIR

thmpost.h: mpost/*.mp
	make -C ./mpost

thsymbolsetlist.h: thsymbolsetlist.pl mpost/thTrans.mp
	perl thsymbolsetlist.pl
  
thtex.h: tex/*.tex
	make -C ./tex
  
update:
	make -C ./thlang update
  
thlangdata.cxx: thlang/texts.txt
	make -C ./thlang

config-debug:
	perl makeconfig.pl BUILD DEBUG
  
config-release:
	perl makeconfig.pl BUILD RELEASE

config-oxygen:
	perl makeconfig.pl BUILD OXYGEN
  
config-ozone:
	perl makeconfig.pl BUILD OZONE
  
config-linux:
	perl makeconfig.pl PLATFORM LINUX
  
config-win32:
	perl makeconfig.pl PLATFORM WIN32
  
config-macosx:
	perl makeconfig.pl PLATFORM MACOSX

# external sources
extern/getopt.o: extern/getopt.c extern/getopt.h
extern/getopt1.o: extern/getopt1.c
extern/img.o: extern/img.c extern/img.h
tri/construct.o: tri/triangulate.h tri/construct.c
tri/misc.o: tri/triangulate.h tri/misc.c
tri/monotone.o: tri/triangulate.h tri/monotone.c
tri/tri.o: tri/triangulate.h tri/tri.c

# DEPENDENCIES
orient.o: orient.cxx
th2ddataobject.o: th2ddataobject.cxx th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h thchenc.h thchencdata.h
tharea.o: tharea.cxx tharea.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h thchenc.h thchencdata.h thexpmap.h thexport.h \
  thsymbolset.h thsymbolsetlist.h thlayout.h thlang.h thlangdata.h \
  thline.h
thbuffer.o: thbuffer.cxx thbuffer.h
thcdt.o: thcdt.cxx thcdt.h thcdt/geom2d.h thcdt/myMacro.h thcdt/cdt.h \
  thcdt/cdt.cpp
thchenc.o: thchenc.cxx thchenc.h thchencdata.h thparse.h thbuffer.h \
  thmbuffer.h thchencdata.cxx therion.h thexception.h
thchencdata.o: thchencdata.cxx
thcmdline.o: thcmdline.cxx thcmdline.h therion.h thlogfile.h thbuffer.h \
  thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h thexport.h \
  thobjectsrc.h thselector.h extern/getopt.h thtmpdir.h
thconfig.o: thconfig.cxx thconfig.h thbuffer.h thmbuffer.h thinput.h \
  thparse.h thexporter.h thexport.h thobjectsrc.h thselector.h therion.h \
  thchenc.h thchencdata.h thexception.h thdatabase.h thdataobject.h \
  thperson.h thdate.h thdataleg.h thobjectname.h thdb1d.h thobjectid.h \
  thinfnan.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thdatareader.h
thconvert.o: thconvert.cxx thpdfdbg.h thexception.h therion.h thbuffer.h \
  thpdfdata.h thtexfonts.h
thdatabase.o: thdatabase.cxx thdatabase.h thdataobject.h thperson.h \
  thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
  therion.h thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thsurvey.h thtfpwf.h thendsurvey.h \
  thexception.h thdata.h thtfangle.h thtf.h thtflength.h thcmdline.h \
  thdatastation.h thgrade.h thlayout.h thsymbolset.h thsymbolsetlist.h \
  thscrap.h th2ddataobject.h thpoint.h thline.h tharea.h thjoin.h thmap.h \
  thendscrap.h
thdata.o: thdata.cxx thdata.h thdataleg.h thparse.h thbuffer.h \
  thmbuffer.h thobjectname.h therion.h thobjectsrc.h thdataobject.h \
  thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thtfangle.h thtf.h \
  thtflength.h thtfpwf.h thexception.h thchenc.h thchencdata.h thsurvey.h \
  thgrade.h thdatareader.h thinput.h
thdataleg.o: thdataleg.cxx thdataleg.h thparse.h thbuffer.h thmbuffer.h \
  thobjectname.h therion.h thobjectsrc.h thinfnan.h
thdataobject.o: thdataobject.cxx thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thexception.h thchenc.h \
  thchencdata.h thsurvey.h thtfpwf.h
thdatareader.o: thdatareader.cxx thdatareader.h thdatabase.h \
  thdataobject.h thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h \
  thdataleg.h thobjectname.h therion.h thobjectsrc.h thdb1d.h \
  thobjectid.h thinfnan.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thinput.h thexception.h
thdatastation.o: thdatastation.cxx thdatastation.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h thchenc.h thchencdata.h
thdate.o: thdate.cxx thdate.h thparse.h thbuffer.h thmbuffer.h \
  thexception.h therion.h
thdb1d.o: thdb1d.cxx thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thbuffer.h thmbuffer.h thobjectname.h therion.h thobjectsrc.h \
  thdb3d.h thsurvey.h thdataobject.h thdatabase.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thtfpwf.h thsvxctrl.h \
  thexception.h thdata.h thtfangle.h thtf.h thtflength.h thlogfile.h
thdb2d00.o: thdb2d00.cxx thdb2d.h thinfnan.h thdb2dprj.h thparse.h \
  thbuffer.h thmbuffer.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thobjectname.h therion.h thobjectsrc.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thdb1d.h thobjectid.h thdataleg.h thdb3d.h \
  thexception.h thdatabase.h thdataobject.h thtfangle.h thtf.h tharea.h \
  th2ddataobject.h thmap.h thjoin.h thpoint.h thline.h thscrap.h \
  thsurvey.h thtfpwf.h thlogfile.h thlayout.h thsymbolset.h \
  thsymbolsetlist.h thconfig.h thinput.h thexporter.h thexport.h \
  thselector.h
thdb2dab.o: thdb2dab.cxx thdb2dab.h thobjectname.h thmbuffer.h therion.h \
  thobjectsrc.h
thdb2dcp.o: thdb2dcp.cxx thdb2dcp.h thdb2dpt.h
thdb2d.o: thdb2d.cxx thdb2d.h thinfnan.h thdb2dprj.h thparse.h thbuffer.h \
  thmbuffer.h thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thobjectname.h therion.h thobjectsrc.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thdb1d.h thobjectid.h thdataleg.h thdb3d.h thexception.h \
  thdatabase.h thdataobject.h thtfangle.h thtf.h tharea.h \
  th2ddataobject.h thmap.h thjoin.h thpoint.h thline.h thscrap.h \
  thsurvey.h thtfpwf.h thlogfile.h thlayout.h thsymbolset.h \
  thsymbolsetlist.h
thdb2dji.o: thdb2dji.cxx thdb2dji.h thobjectname.h thmbuffer.h therion.h \
  thdatabase.h thdataobject.h thperson.h thparse.h thbuffer.h thdate.h \
  thdataleg.h thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thexception.h
thdb2dlp.o: thdb2dlp.cxx thdb2dlp.h thdb2dpt.h thline.h th2ddataobject.h \
  thdataobject.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h \
  thobjectid.h thinfnan.h thdataleg.h thparse.h thobjectname.h therion.h \
  thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h thdate.h \
  thperson.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thexpmap.h \
  thexport.h thsymbolset.h thsymbolsetlist.h thlayout.h thlang.h \
  thlangdata.h
thdb2dmi.o: thdb2dmi.cxx thdb2dmi.h thobjectname.h thmbuffer.h therion.h \
  thobjectsrc.h thparse.h thbuffer.h thexception.h
thdb2dprj.o: thdb2dprj.cxx thdb2dprj.h thparse.h thbuffer.h thmbuffer.h \
  thmapstat.h thdate.h thperson.h thinfnan.h
thdb2dpt.o: thdb2dpt.cxx thdb2dpt.h thexpmap.h thexport.h thparse.h \
  thbuffer.h thmbuffer.h thobjectsrc.h thsymbolset.h thsymbolsetlist.h \
  thlayout.h thdataobject.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thobjectname.h therion.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dlp.h thdb2dab.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thlang.h thlangdata.h
thdb2dxm.o: thdb2dxm.cxx thdb2dxm.h thdb2dmi.h thobjectname.h thmbuffer.h \
  therion.h thobjectsrc.h thparse.h thbuffer.h
thdb2dxs.o: thdb2dxs.cxx thdb2dxs.h thdb2dmi.h thobjectname.h thmbuffer.h \
  therion.h thobjectsrc.h thparse.h thbuffer.h
thdb3d.o: thdb3d.cxx thdb3d.h
thendscrap.o: thendscrap.cxx thendscrap.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thexception.h \
  thchenc.h thchencdata.h
thendsurvey.o: thendsurvey.cxx thendsurvey.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thexception.h \
  thchenc.h thchencdata.h
therion.o: therion.cxx therion.h thlogfile.h thbuffer.h thtmpdir.h \
  thcmdline.h thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h \
  thexport.h thobjectsrc.h thselector.h thchenc.h thchencdata.h \
  thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
  thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thdatareader.h thexception.h thlibrary.h \
  thinit.h thgrade.h thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
  thlayout.h thsymbolset.h thsymbolsetlist.h thpoint.h th2ddataobject.h \
  thline.h tharea.h thversion.h thtexfonts.h
thexception.o: thexception.cxx thexception.h therion.h thbuffer.h
thexpdb.o: thexpdb.cxx thexpdb.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thexception.h therion.h thdatabase.h \
  thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thdb1d.h \
  thobjectid.h thinfnan.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h thinit.h thinput.h \
  thsurvey.h thchenc.h thchencdata.h
thexpmap.o: thexpmap.cxx thexpmap.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thsymbolset.h thsymbolsetlist.h thlayout.h \
  thdataobject.h thdatabase.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thobjectname.h therion.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thlang.h thlangdata.h thexporter.h \
  thexception.h thtmpdir.h thscrap.h thpoint.h th2ddataobject.h thline.h \
  thmap.h thconfig.h thinput.h thselector.h thchenc.h thchencdata.h \
  thinit.h thlogfile.h thconvert.h thpdf.h thpdfdata.h thmpost.h thtex.h \
  thcmdline.h thtexfonts.h thsurvey.h thtfpwf.h
thexpmodel.o: thexpmodel.cxx thexpmodel.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thexception.h therion.h thdatabase.h \
  thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thdb1d.h \
  thobjectid.h thinfnan.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h thsurvey.h \
  extern/img.h thchenc.h thchencdata.h thscrap.h
thexport.o: thexport.cxx thexport.h thparse.h thbuffer.h thmbuffer.h \
  thobjectsrc.h thexception.h therion.h thconfig.h thinput.h thexporter.h \
  thselector.h thdatabase.h thdataobject.h thperson.h thdate.h \
  thdataleg.h thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h
thexporter.o: thexporter.cxx thexporter.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thexception.h therion.h thconfig.h thinput.h \
  thselector.h thdatabase.h thdataobject.h thperson.h thdate.h \
  thdataleg.h thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thexpmodel.h thexpmap.h \
  thsymbolset.h thsymbolsetlist.h thlayout.h thlang.h thlangdata.h \
  thexpdb.h
thgrade.o: thgrade.cxx thgrade.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thdata.h thtfangle.h thtf.h \
  thtflength.h thtfpwf.h thexception.h thchenc.h thchencdata.h
thinfnan.o: thinfnan.cxx thinfnan.h
thinit.o: thinit.cxx thinit.h thbuffer.h thmbuffer.h thinput.h thparse.h \
  thchenc.h thchencdata.h therion.h thconfig.h thexporter.h thexport.h \
  thobjectsrc.h thselector.h thexception.h thtexfonts.h thlang.h \
  thlangdata.h
thinput.o: thinput.cxx thinput.h thbuffer.h thmbuffer.h thparse.h \
  thchenc.h thchencdata.h therion.h thexception.h
thjoin.o: thjoin.cxx thjoin.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thexception.h thchenc.h \
  thchencdata.h
thlang.o: thlang.cxx thlang.h thlangdata.h thparse.h thbuffer.h \
  thmbuffer.h thlangdata.cxx thinit.h thinput.h
thlangdata.o: thlangdata.cxx
thlayout.o: thlayout.cxx thlayout.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thsymbolset.h \
  thsymbolsetlist.h thexception.h thchenc.h thchencdata.h thdata.h \
  thtfangle.h thtf.h thtflength.h thtfpwf.h thpdfdata.h thlang.h \
  thlangdata.h
thlayoutln.o: thlayoutln.cxx thlayoutln.h thlayout.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thscrapen.h thscraplp.h thsymbolset.h \
  thsymbolsetlist.h
thlibrary.o: thlibrary.cxx thlibrary.h thlibrarydata.cxx thdatabase.h \
  thdataobject.h thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h \
  thdataleg.h thobjectname.h therion.h thobjectsrc.h thdb1d.h \
  thobjectid.h thinfnan.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thchencdata.h thlayout.h thsymbolset.h thsymbolsetlist.h thlang.h \
  thlangdata.h thgrade.h thdata.h thtfangle.h thtf.h thtflength.h \
  thtfpwf.h
thlibrarydata.o: thlibrarydata.cxx thdatabase.h thdataobject.h thperson.h \
  thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
  therion.h thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thchencdata.h thlayout.h \
  thsymbolset.h thsymbolsetlist.h thlang.h thlangdata.h thgrade.h \
  thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h
thline.o: thline.cxx thline.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h thchenc.h thchencdata.h thexpmap.h thexport.h \
  thsymbolset.h thsymbolsetlist.h thlayout.h thlang.h thlangdata.h \
  thtflength.h thtf.h thtexfonts.h thscrap.h
thlogfile.o: thlogfile.cxx thlogfile.h thbuffer.h therion.h
thmap.o: thmap.cxx thmap.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thexception.h thchenc.h \
  thchencdata.h thscrap.h
thmapstat.o: thmapstat.cxx thmapstat.h thdate.h thperson.h thscrap.h \
  thdataobject.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h \
  thobjectid.h thinfnan.h thdataleg.h thparse.h thobjectname.h therion.h \
  thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thdata.h thtfangle.h \
  thtf.h thtflength.h thtfpwf.h thmap.h thlayout.h thsymbolset.h \
  thsymbolsetlist.h thlang.h thlangdata.h thtexfonts.h thsurvey.h
thmbuffer.o: thmbuffer.cxx thmbuffer.h
thobjectid.o: thobjectid.cxx thobjectid.h
thobjectname.o: thobjectname.cxx thobjectname.h thmbuffer.h therion.h \
  thexception.h thbuffer.h thparse.h thdatabase.h thdataobject.h \
  thperson.h thdate.h thdataleg.h thobjectsrc.h thdb1d.h thobjectid.h \
  thinfnan.h thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h
thobjectsrc.o: thobjectsrc.cxx thobjectsrc.h
thparse.o: thparse.cxx thparse.h thbuffer.h thmbuffer.h therion.h \
  thinfnan.h thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
  thobjectname.h thobjectsrc.h thdb1d.h thobjectid.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thtflength.h thtf.h thexception.h
thpdf.o: thpdf.cxx thpdfdbg.h thexception.h therion.h thbuffer.h \
  thpdfdata.h thtexfonts.h thlang.h thlangdata.h thchenc.h thchencdata.h \
  thparse.h thmbuffer.h
thpdfdata.o: thpdfdata.cxx thpdfdata.h thlang.h thlangdata.h
thpdfdbg.o: thpdfdbg.cxx thpdfdbg.h thexception.h therion.h thbuffer.h \
  thpdfdata.h
thperson.o: thperson.cxx thperson.h thdatabase.h thdataobject.h thparse.h \
  thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h therion.h \
  thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thexception.h
thpoint.o: thpoint.cxx thpoint.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h \
  thdb2d.h thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h thchenc.h thchencdata.h thexpmap.h thexport.h \
  thsymbolset.h thsymbolsetlist.h thlayout.h thlang.h thlangdata.h \
  thtflength.h thtf.h thtexfonts.h
thscrap.o: thscrap.cxx thscrap.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thexception.h thchenc.h \
  thchencdata.h thtflength.h thtf.h th2ddataobject.h thline.h thpoint.h \
  thscrapis.h
thscrapen.o: thscrapen.cxx thscrapen.h
thscrapis.o: thscrapis.cxx thscrapis.h thdb3d.h therion.h thdb2dlp.h \
  thdb2dpt.h thscraplo.h thline.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h thobjectsrc.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thlayoutln.h thscrapen.h thscraplp.h \
  thcdt.h
thscraplo.o: thscraplo.cxx thscraplo.h
thscraplp.o: thscraplp.cxx thscraplp.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thbuffer.h thmbuffer.h thobjectname.h therion.h \
  thobjectsrc.h thdb3d.h
thselector.o: thselector.cxx thselector.h thexception.h therion.h \
  thbuffer.h thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h \
  thexport.h thobjectsrc.h thdatabase.h thdataobject.h thperson.h \
  thdate.h thdataleg.h thobjectname.h thdb1d.h thobjectid.h thinfnan.h \
  thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thsurvey.h thtfpwf.h \
  th2ddataobject.h thscrap.h thchenc.h thchencdata.h thmap.h
thsurvey.o: thsurvey.cxx thsurvey.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h \
  thdb2dprj.h thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thtfpwf.h \
  thexception.h thchenc.h thchencdata.h thtfangle.h thtf.h
thsvxctrl.o: thsvxctrl.cxx thsvxctrl.h thdataleg.h thparse.h thbuffer.h \
  thmbuffer.h thobjectname.h therion.h thobjectsrc.h thdatabase.h \
  thdataobject.h thperson.h thdate.h thdb1d.h thobjectid.h thinfnan.h \
  thdb3d.h thdb2d.h thdb2dprj.h thmapstat.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thtmpdir.h thdata.h \
  thtfangle.h thtf.h thtflength.h thtfpwf.h thexception.h thinit.h \
  thinput.h thsurvey.h thlogfile.h extern/img.h
thsymbolset.o: thsymbolset.cxx thsymbolset.h thsymbolsetlist.h thparse.h \
  thbuffer.h thmbuffer.h thpoint.h th2ddataobject.h thdataobject.h \
  thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thobjectname.h therion.h thobjectsrc.h thdb3d.h thdb2d.h thdb2dprj.h \
  thmapstat.h thdate.h thperson.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thline.h tharea.h thexception.h \
  thpdfdata.h thlayout.h thtexfonts.h thlang.h thlangdata.h
thtexenc.o: thtexenc.cxx
thtexfonts.o: thtexfonts.cxx thtexfonts.h thtexenc.cxx thpdfdbg.h \
  thexception.h therion.h thbuffer.h
thtfangle.o: thtfangle.cxx thparse.h thbuffer.h thmbuffer.h thtfangle.h \
  thtf.h thexception.h therion.h
thtf.o: thtf.cxx thtf.h thexception.h therion.h thbuffer.h thparse.h \
  thmbuffer.h
thtflength.o: thtflength.cxx thtflength.h thtf.h thparse.h thbuffer.h \
  thmbuffer.h thexception.h therion.h
thtfpwf.o: thtfpwf.cxx thtfpwf.h thexception.h therion.h thbuffer.h \
  thinfnan.h thparse.h thmbuffer.h
thtmpdir.o: thtmpdir.cxx thtmpdir.h thbuffer.h therion.h thinit.h \
  thmbuffer.h thinput.h thparse.h
