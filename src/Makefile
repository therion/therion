# common therion objects
CMNOBJECTS = thexception.o thbuffer.o thmbuffer.o thlogfile.o thtmpdir.o \
  thparse.o thcmdline.o thconfig.o thinput.o thchenc.o thdatabase.o \
  thdataobject.o thdatareader.o thsurvey.o thendsurvey.o thdata.o \
  thdate.o thperson.o thtf.o thtfangle.o thtflength.o thtfpwf.o \
  thdataleg.o thobjectname.o thinfnan.o thlayout.o \
  thinit.o thdb1d.o thsvxctrl.o thdatastation.o thobjectid.o \
  thobjectsrc.o thgrade.o thlibrary.o \
  thexport.o thexporter.o thselector.o extern/img.o \
  thexpmodel.o thdb2d00.o \
  thdb2d.o thscrap.o thendscrap.o th2ddataobject.o thdb2dprj.o \
  thdb2dpt.o thdb2dlp.o thdb2dab.o thdb2dji.o thdb2dmi.o thdb2dcp.o \
  thdb2dxm.o thdb2dxs.o thscraplo.o thscraplp.o thscrapen.o \
  thpoint.o thline.o tharea.o \
  thjoin.o thmap.o thexpmap.o thlayoutln.o \
  therion.o


# PLATFORM CONFIG

# PLATFORM LINUX
CXX = g++
CC = gcc
POBJECTS =
CXXPFLAGS = -DTHLINUX
CCPFLAGS = -DTHLINUX
THPDFMKCMD = make -C ./thpdf thpdf-unix
THXTHMKCMD = ../bin/therion

# PLATFORM WIN32
##CXX = c++
##CC = gcc
##POBJECTS = extern/getopt.o extern/getopt1.o
##CXXPFLAGS = -DTHWIN32
##CCPFLAGS = -DTHWIN32
##THPDFMKCMD = make -C ./thpdf thpdf-win32
##THXTHMKCMD = ..\bin\therion

# PLATFORM MACOSX
##CXX = c++
##CC = cc
##POBJECTS = extern/getopt.o extern/getopt1.o
##CXXPFLAGS = -DTHMACOSX
##CCPFLAGS = -DTHMACOSX
##THPDFMKCMD = make -C ./thpdf thpdf-unix
##THXTHMKCMD = ../bin/therion

# PLATFORM ENDCONFIG


# BUILD CONFIG

# BUILD RELEASE
CCBFLAGS =
CXXBFLAGS = 

# BUILD DEBUG
##CCBFLAGS =
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
	$(CC) -c $(CXXFLAGS) -o $@ $<

all:	../bin/therion ../bin/theps2pdf xtherion thpdf

therion:
	make ../bin/therion

../bin/therion:	$(OBJECTS)
	$(CXX) -Wall -o ../bin/therion $(OBJECTS) $(LDFLAGS) $(LIBS)

../bin/theps2pdf: theps2pdf/theps2pdf.c theps2pdf/theps2pdf.h
	$(CC) theps2pdf/theps2pdf.c -Wall -o ../bin/theps2pdf -Itheps2pdf

init:
	../bin/therion --print-init-file > therion.ini
	perl makefile.pl mv therion.ini ../lib/therion.ini

release: clean
	perl makerelease.pl

archive: clean
	perl makearchive.pl

  
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

xtherion: ../bin/therion
	make -C ./xtherion

thpdf: ../bin/therion
	$(THPDFMKCMD)

doc:
	make -C ./thbook
  
clean:
	make -C ./xtherion clean
	perl makefile.pl rm ../bin/therion ../bin/thpdf ../bin/xtherion ../bin/therion.exe *~ *.log *.o thchencdata/*~
	perl makefile.pl rm theps2pdf/*~ ../bin/theps2pdf xtherion/*~
	perl makefile.pl rm extern/*.o extern/*~
	perl makefile.pl rm tests/*~
	perl makefile.pl rm core
	perl makefile.pl rm data.3d data.svx data.pos data.pts data.err data.plt
	perl makefile.pl rm cave.3d cave.pdf cave.svg therion.tcl cave_a.pdf cave_m.pdf
	perl makefile.pl rm ../doc/*~ ../doc/thbook.pdf ../lib/*~ ../lib/mpost/*~
	perl makefile.pl rmdir doc thTMPDIR
  
config-debug:
	perl makeconfig.pl BUILD DEBUG
  
config-release:
	perl makeconfig.pl BUILD RELEASE
  
config-linux:
	perl makeconfig.pl PLATFORM LINUX
  
config-win32:
	perl makeconfig.pl PLATFORM WIN32
  
config-macosx:
	perl makeconfig.pl PLATFORM MACOSX

# external sources
extern/getopt.o: extern/getopt.c
extern/getopt1.o: extern/getopt1.c
extern/img.o: extern/img.c

# DEPENDENCIES
th2ddataobject.o: th2ddataobject.cxx th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thperson.h thdate.h thexception.h thchenc.h thchencdata.h
tharea.o: tharea.cxx tharea.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thperson.h thdate.h thexception.h thchenc.h thchencdata.h \
  thexpmap.h thexport.h thline.h
thbuffer.o: thbuffer.cxx thbuffer.h
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
  thinfnan.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h thdatareader.h
thdatabase.o: thdatabase.cxx thdatabase.h thdataobject.h thperson.h \
  thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
  therion.h thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thsurvey.h thtfpwf.h thendsurvey.h thexception.h thdata.h \
  thtfangle.h thtf.h thtflength.h thcmdline.h thdatastation.h thgrade.h \
  thlayout.h thscrap.h th2ddataobject.h thpoint.h thline.h tharea.h \
  thjoin.h thmap.h thendscrap.h
thdata.o: thdata.cxx thdata.h thdataleg.h thparse.h thbuffer.h \
  thmbuffer.h thobjectname.h therion.h thobjectsrc.h thdataobject.h \
  thdatabase.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thperson.h thdate.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
  thexception.h thchenc.h thchencdata.h thsurvey.h thgrade.h \
  thdatareader.h thinput.h
thdataleg.o: thdataleg.cxx thdataleg.h thparse.h thbuffer.h thmbuffer.h \
  thobjectname.h therion.h thobjectsrc.h thinfnan.h
thdataobject.o: thdataobject.cxx thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thperson.h \
  thdate.h thexception.h thchenc.h thchencdata.h thsurvey.h thtfpwf.h
thdatareader.o: thdatareader.cxx thdatareader.h thdatabase.h \
  thdataobject.h thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h \
  thdataleg.h thobjectname.h therion.h thobjectsrc.h thdb1d.h \
  thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thinput.h \
  thexception.h
thdatastation.o: thdatastation.cxx thdatastation.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thperson.h thdate.h thexception.h thchenc.h thchencdata.h
thdate.o: thdate.cxx thdate.h thexception.h therion.h thbuffer.h
thdb1d.o: thdb1d.cxx thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thbuffer.h thmbuffer.h thobjectname.h therion.h thobjectsrc.h \
  thsurvey.h thdataobject.h thdatabase.h thdb2d.h thdb2dprj.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thperson.h \
  thdate.h thtfpwf.h thsvxctrl.h thexception.h thdata.h thtfangle.h \
  thtf.h thtflength.h
thdb2d00.o: thdb2d00.cxx thdb2d.h thinfnan.h thdb2dprj.h thparse.h \
  thbuffer.h thmbuffer.h thdb2dpt.h thdb2dlp.h thdb2dab.h thobjectname.h \
  therion.h thobjectsrc.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thdb1d.h \
  thobjectid.h thdataleg.h thexception.h thdatabase.h thdataobject.h \
  thperson.h thdate.h thtfangle.h thtf.h tharea.h th2ddataobject.h \
  thmap.h thjoin.h thpoint.h thline.h thscrap.h thsurvey.h thtfpwf.h \
  thlogfile.h thlayout.h thconfig.h thinput.h thexporter.h thexport.h \
  thselector.h
thdb2dab.o: thdb2dab.cxx thdb2dab.h thobjectname.h thmbuffer.h therion.h \
  thobjectsrc.h
thdb2dcp.o: thdb2dcp.cxx thdb2dcp.h thdb2dpt.h
thdb2d.o: thdb2d.cxx thdb2d.h thinfnan.h thdb2dprj.h thparse.h thbuffer.h \
  thmbuffer.h thdb2dpt.h thdb2dlp.h thdb2dab.h thobjectname.h therion.h \
  thobjectsrc.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thdb1d.h thobjectid.h \
  thdataleg.h thexception.h thdatabase.h thdataobject.h thperson.h \
  thdate.h thtfangle.h thtf.h tharea.h th2ddataobject.h thmap.h thjoin.h \
  thpoint.h thline.h thscrap.h thsurvey.h thtfpwf.h thlogfile.h \
  thlayout.h
thdb2dji.o: thdb2dji.cxx thdb2dji.h thobjectname.h thmbuffer.h therion.h \
  thdatabase.h thdataobject.h thperson.h thparse.h thbuffer.h thdate.h \
  thdataleg.h thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h
thdb2dlp.o: thdb2dlp.cxx thdb2dlp.h thdb2dpt.h thline.h th2ddataobject.h \
  thdataobject.h thdatabase.h thmbuffer.h thbuffer.h thdb1d.h \
  thobjectid.h thinfnan.h thdataleg.h thparse.h thobjectname.h therion.h \
  thobjectsrc.h thdb2d.h thdb2dprj.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thperson.h thdate.h thexpmap.h thexport.h
thdb2dmi.o: thdb2dmi.cxx thdb2dmi.h thobjectname.h thmbuffer.h therion.h \
  thobjectsrc.h thparse.h thbuffer.h thexception.h
thdb2dprj.o: thdb2dprj.cxx thdb2dprj.h thparse.h thbuffer.h thmbuffer.h \
  thinfnan.h
thdb2dpt.o: thdb2dpt.cxx thdb2dpt.h thexpmap.h thexport.h thparse.h \
  thbuffer.h thmbuffer.h thobjectsrc.h
thdb2dxm.o: thdb2dxm.cxx thdb2dxm.h thdb2dmi.h thobjectname.h thmbuffer.h \
  therion.h thobjectsrc.h thparse.h thbuffer.h
thdb2dxs.o: thdb2dxs.cxx thdb2dxs.h thdb2dmi.h thobjectname.h thmbuffer.h \
  therion.h thobjectsrc.h thparse.h thbuffer.h
thendscrap.o: thendscrap.cxx thendscrap.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thperson.h thdate.h thexception.h thchenc.h thchencdata.h
thendsurvey.o: thendsurvey.cxx thendsurvey.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thperson.h thdate.h thexception.h thchenc.h thchencdata.h
therion.o: therion.cxx therion.h thlogfile.h thbuffer.h thtmpdir.h \
  thcmdline.h thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h \
  thexport.h thobjectsrc.h thselector.h thchenc.h thchencdata.h \
  thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
  thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thdatareader.h thexception.h thlibrary.h thinit.h thgrade.h thdata.h \
  thtfangle.h thtf.h thtflength.h thtfpwf.h thlayout.h thpoint.h \
  th2ddataobject.h thline.h thversion.h
thexception.o: thexception.cxx thexception.h therion.h thbuffer.h
thexpmap.o: thexpmap.cxx thexpmap.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thexporter.h thexception.h therion.h \
  thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
  thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thtmpdir.h thscrap.h thpoint.h th2ddataobject.h thline.h thlayout.h \
  thmap.h thconfig.h thinput.h thselector.h thchenc.h thchencdata.h \
  thinit.h thlogfile.h
thexpmodel.o: thexpmodel.cxx thexpmodel.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thexception.h therion.h thdatabase.h \
  thdataobject.h thperson.h thdate.h thdataleg.h thobjectname.h thdb1d.h \
  thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thdata.h thtfangle.h \
  thtf.h thtflength.h thtfpwf.h thsurvey.h extern/img.h
thexport.o: thexport.cxx thexport.h thparse.h thbuffer.h thmbuffer.h \
  thobjectsrc.h thexception.h therion.h thconfig.h thinput.h thexporter.h \
  thselector.h thdatabase.h thdataobject.h thperson.h thdate.h \
  thdataleg.h thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h
thexporter.o: thexporter.cxx thexporter.h thexport.h thparse.h thbuffer.h \
  thmbuffer.h thobjectsrc.h thexception.h therion.h thconfig.h thinput.h \
  thselector.h thdatabase.h thdataobject.h thperson.h thdate.h \
  thdataleg.h thobjectname.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thexpmodel.h thexpmap.h
thgrade.o: thgrade.cxx thgrade.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thperson.h \
  thdate.h thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h \
  thexception.h thchenc.h thchencdata.h
thinfnan.o: thinfnan.cxx thinfnan.h
thinit.o: thinit.cxx thinit.h thbuffer.h thmbuffer.h thinput.h thparse.h \
  thchenc.h thchencdata.h therion.h thconfig.h thexporter.h thexport.h \
  thobjectsrc.h thselector.h thexception.h
thinput.o: thinput.cxx thinput.h thbuffer.h thmbuffer.h thparse.h \
  thchenc.h thchencdata.h therion.h thexception.h
thjoin.o: thjoin.cxx thjoin.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thperson.h \
  thdate.h thexception.h thchenc.h thchencdata.h
thlayout.o: thlayout.cxx thlayout.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thperson.h thdate.h thexception.h thchenc.h thchencdata.h thdata.h \
  thtfangle.h thtf.h thtflength.h thtfpwf.h
thlayoutln.o: thlayoutln.cxx thlayoutln.h
thlibrary.o: thlibrary.cxx thlibrary.h thlibrarydata.cxx thdatabase.h \
  thdataobject.h thperson.h thparse.h thbuffer.h thmbuffer.h thdate.h \
  thdataleg.h thobjectname.h therion.h thobjectsrc.h thdb1d.h \
  thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h \
  thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h \
  thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thchencdata.h \
  thlayout.h thgrade.h thdata.h thtfangle.h thtf.h thtflength.h thtfpwf.h
thlibrarydata.o: thlibrarydata.cxx thdatabase.h thdataobject.h thperson.h \
  thparse.h thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h \
  therion.h thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thchencdata.h thlayout.h thgrade.h thdata.h thtfangle.h \
  thtf.h thtflength.h thtfpwf.h
thline.o: thline.cxx thline.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thperson.h thdate.h thexception.h thchenc.h thchencdata.h \
  thexpmap.h thexport.h thtflength.h thtf.h thscrap.h
thlogfile.o: thlogfile.cxx thlogfile.h thbuffer.h therion.h
thmap.o: thmap.cxx thmap.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thperson.h \
  thdate.h thexception.h thchenc.h thchencdata.h thscrap.h
thmbuffer.o: thmbuffer.cxx thmbuffer.h
thobjectid.o: thobjectid.cxx thobjectid.h
thobjectname.o: thobjectname.cxx thobjectname.h thmbuffer.h therion.h \
  thexception.h thbuffer.h thparse.h thdatabase.h thdataobject.h \
  thperson.h thdate.h thdataleg.h thobjectsrc.h thdb1d.h thobjectid.h \
  thinfnan.h thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h \
  thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h \
  thlayoutln.h thscrapen.h thscraplp.h
thobjectsrc.o: thobjectsrc.cxx thobjectsrc.h
thparse.o: thparse.cxx thparse.h thbuffer.h thmbuffer.h therion.h \
  thinfnan.h thdatabase.h thdataobject.h thperson.h thdate.h thdataleg.h \
  thobjectname.h thobjectsrc.h thdb1d.h thobjectid.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thtflength.h thtf.h thexception.h
thperson.o: thperson.cxx thperson.h thdatabase.h thdataobject.h thparse.h \
  thbuffer.h thmbuffer.h thdate.h thdataleg.h thobjectname.h therion.h \
  thobjectsrc.h thdb1d.h thobjectid.h thinfnan.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thexception.h
thpoint.o: thpoint.cxx thpoint.h th2ddataobject.h thdataobject.h \
  thdatabase.h thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h \
  thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h \
  thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h \
  thscraplp.h thperson.h thdate.h thexception.h thchenc.h thchencdata.h \
  thexpmap.h thexport.h thtflength.h thtf.h
thscrap.o: thscrap.cxx thscrap.h thdataobject.h thdatabase.h thmbuffer.h \
  thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h thparse.h \
  thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h thdb2dpt.h \
  thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h thdb2dxs.h \
  thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h thperson.h \
  thdate.h thexception.h thchenc.h thchencdata.h thtflength.h thtf.h \
  th2ddataobject.h thline.h thpoint.h
thscrapen.o: thscrapen.cxx thscrapen.h
thscraplo.o: thscraplo.cxx thscraplo.h
thscraplp.o: thscraplp.cxx thscraplp.h thdb1d.h thobjectid.h thinfnan.h \
  thdataleg.h thparse.h thbuffer.h thmbuffer.h thobjectname.h therion.h \
  thobjectsrc.h
thselector.o: thselector.cxx thselector.h thexception.h therion.h \
  thbuffer.h thconfig.h thmbuffer.h thinput.h thparse.h thexporter.h \
  thexport.h thobjectsrc.h thdatabase.h thdataobject.h thperson.h \
  thdate.h thdataleg.h thobjectname.h thdb1d.h thobjectid.h thinfnan.h \
  thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thsurvey.h thtfpwf.h th2ddataobject.h thscrap.h \
  thchenc.h thchencdata.h thmap.h
thsurvey.o: thsurvey.cxx thsurvey.h thdataobject.h thdatabase.h \
  thmbuffer.h thbuffer.h thdb1d.h thobjectid.h thinfnan.h thdataleg.h \
  thparse.h thobjectname.h therion.h thobjectsrc.h thdb2d.h thdb2dprj.h \
  thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h thdb2dmi.h thdb2dcp.h \
  thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h thscrapen.h thscraplp.h \
  thperson.h thdate.h thtfpwf.h thexception.h thchenc.h thchencdata.h \
  thtfangle.h thtf.h
thsvxctrl.o: thsvxctrl.cxx thsvxctrl.h thdataleg.h thparse.h thbuffer.h \
  thmbuffer.h thobjectname.h therion.h thobjectsrc.h thdatabase.h \
  thdataobject.h thperson.h thdate.h thdb1d.h thobjectid.h thinfnan.h \
  thdb2d.h thdb2dprj.h thdb2dpt.h thdb2dlp.h thdb2dab.h thdb2dji.h \
  thdb2dmi.h thdb2dcp.h thdb2dxs.h thdb2dxm.h thscraplo.h thlayoutln.h \
  thscrapen.h thscraplp.h thtmpdir.h thdata.h thtfangle.h thtf.h \
  thtflength.h thtfpwf.h thexception.h thinit.h thinput.h thsurvey.h \
  thlogfile.h
thtfangle.o: thtfangle.cxx thparse.h thbuffer.h thmbuffer.h thtfangle.h \
  thtf.h thexception.h therion.h
thtf.o: thtf.cxx thtf.h thexception.h therion.h thbuffer.h
thtflength.o: thtflength.cxx thtflength.h thtf.h thexception.h therion.h \
  thbuffer.h thparse.h thmbuffer.h
thtfpwf.o: thtfpwf.cxx thtfpwf.h thexception.h therion.h thbuffer.h \
  thinfnan.h thparse.h thmbuffer.h
thtmpdir.o: thtmpdir.cxx thtmpdir.h thbuffer.h therion.h
