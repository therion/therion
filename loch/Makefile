OUTDIR ?= .
THPLATFORM ?= LINUX
ifneq ($(OUTDIR),.)
LOUTDIR = ../$(OUTDIR)/loch
else
LOUTDIR = .
endif



# common therion objects
CMNOBJECTS = \
  lxTR.o lxOGLFT.o lxSetup.o lxRender.o lxWX.o \
  lxImgIO.o lxLRUD.o lxFile.o icase.o lxSTree.o \
  lxData.o lxMath.o lxSView.o lxSScene.o \
  lxGUI.o lxGLC.o lxOptDlg.o lxAboutDlg.o lxPres.o lxSStats.o \
  lxTriGeom.o extern/img.o

EXT =

# notdir required in msys/mingw as it doesn't interpret leading slash correctly
WX_CONFIG ?= $(notdir $(shell which \
  wx-config-gtk3 \
  wx-config-3.1 \
  wx-config \
  i686-w64-mingw32.static-wx-config \
  x86_64-w64-mingw32.static-wx-config \
  2> /dev/null | head -n1))
ifeq ($(WX_CONFIG),)
$(error wx-config not found)
endif

VTKSUFFIX = $(shell tclsh ./getvtkver.tcl suffix)
VTKVERSION = $(shell tclsh ./getvtkver.tcl version)
VTKPATH = $(shell tclsh ./getvtkver.tcl incpath)
VTKLIBPATH = $(shell tclsh ./getvtkver.tcl libpath)
VTKV6 = $(shell tclsh ./getvtkver.tcl version6)
VTKV9 = $(shell tclsh ./getvtkver.tcl version9)
VTKLIBS = $(shell tclsh ./getvtkver.tcl libs)
VTKLIBSUFFIX=$(shell tclsh ./getvtkver.tcl libsuffix)
VTKVERSION =$(shell tclsh ./getvtkver.tcl)
ifeq ($(VTKV6),1)
VTKLIBS = \
    -lvtkCommonExecutionModel$(VTKLIBSUFFIX) \
    -lvtkCommonDataModel$(VTKLIBSUFFIX) \
    -lvtkCommonCore$(VTKLIBSUFFIX) \
    -lvtkIOPLY$(VTKLIBSUFFIX) \
    -lvtkFiltersCore$(VTKLIBSUFFIX) \
    -lvtkFiltersHybrid$(VTKLIBSUFFIX) \
    -lvtkIOLegacy$(VTKLIBSUFFIX) \
    -lvtksys$(VTKLIBSUFFIX) \
    $(shell pkg-config --libs freetype2)
else
VTKLIBS = -lvtkHybrid -lvtkImaging -lvtkIO -lvtkGraphics -lvtkFiltering -lvtkCommon -lvtkjpeg -lvtkpng -lvtkzlib -lvtksys -lfreetype
endif
VTKLIBS+= -lpng -ljpeg



# VTKVERSION = 5.4
# VTKPATH = /usr/local/include/vtk-$(VTKVERSION)
# VTKLIBPATH = /usr/local/lib/vtk-$(VTKVERSION)
POSTMAKE =
STRIPFLAG = -s

# PLATFORM CONFIG

# PLATFORM LINUX
##CXX = c++
##CC = gcc
##POBJECTS = lxR2P.o
##ifeq ($(VTKV6),1)
##VTKLIBS = -lvtkCommonExecutionModel -lvtkCommonDataModel -lvtkCommonCore -lvtkIOPLY -lvtkFiltersCore -lvtkFiltersHybrid -lvtkIOLegacy -lfreetype -lpng -ljpeg
##else
##VTKLIBS = -lvtkHybrid -lvtkImaging -lvtkIO -lvtkGraphics -lvtkFiltering -lvtkCommon -lfreetype
##endif
##CXXPFLAGS = -DLXLINUX -std=c++20 $(shell wx-config --cxxflags) -Wno-deprecated $(shell pkg-config freetype2 --cflags) -I$(VTKPATH)
##CCPFLAGS = -DLXLINUX  $(shell wx-config --cflags)
##LXLIBDIR = linux
##PLIBS = $(shell wx-config --libs --gl-libs) -L$(VTKLIBPATH) $(VTKLIBS) -lGLU -lGL -lpthread -lX11 -lz
##LXPLATFORM = LINUX

# PLATFORM DEBIAN
CXX ?= c++
CC ?= gcc
ifeq ($(VTKV6),1)
else
VTKLIBS = -lvtkHybrid -lvtkImaging -lvtkIO -lvtkGraphics -lvtkFiltering -lvtkCommon -lfreetype
endif
#ifeq ($(VTKV9),1)
#VTKLIBS += -lvtkpng$(VTKLIBSUFFIX) -lvtkjpeg$(VTKLIBSUFFIX)
#endif

CXXPFLAGS = \
    -std=c++20 \
    -Wno-deprecated \
    $(shell $(WX_CONFIG) --cxxflags) \
    $(shell pkg-config freetype2 --cflags) \
    -I$(VTKPATH)
CCPFLAGS = $(shell $(WX_CONFIG) --cflags)
LXLIBDIR = linux
PLIBS = $(shell $(WX_CONFIG) --libs --gl-libs) -L$(VTKLIBPATH) $(VTKLIBS) -lGLU -lGL -lpthread -lX11 -lz
LXPLATFORM = LINUX

# LXLINUX doesn't work with Arch Linux
ifneq ($(shell lsb_release -si),Arch)
  POBJECTS = lxR2P.o
  CXXPFLAGS += -DLXLINUX
  CCPFLAGS += -DLXLINUX
endif

# PLATFORM WIN32
##CXX = c++
##CC = gcc
##POBJECTS = loch.res lxR2D.o
##CXXPFLAGS = -W -Wall -DLXWIN32 $(shell $(WX_CONFIG) --static --cxxflags) $(shell pkg-config freetype2 --cflags) -I$(VTKPATH) -Wno-deprecated
##CCPFLAGS = -W -Wall -DLXWIN32 $(shell $(WX_CONFIG) --static --cflags)
##LXLIBDIR = mingw
##PLIBS = $(shell pkg-config freetype2 --libs) -lharfbuzz -lbrotlidec -lbrotlicommon -lgraphite2 -lusp10 -lbz2 -lz $(shell $(WX_CONFIG) --static --libs --gl-libs) -L$(VTKLIBPATH) $(VTKLIBS)
##LXPLATFORM = WIN32
##STRIPFLAG = -static-libgcc -s

# PLATFORM WIN32CROSS
##CROSS ?= i686-w64-mingw32.static-
##CROSS2 = $(shell echo $(CROSS) | sed 's/-$$//')
##EXT = .exe
##WX_CONFIG = wx-config
##VTKVERSION = 8.2
##VTKPATH = /usr/lib/mxe/usr/$(CROSS2)/include/vtk-$(VTKVERSION)
##VTKLIBPATH = /usr/lib/mxe/usr/$(CROSS2)/lib
##VTKLIBS = -lvtkCommonExecutionModel-$(VTKVERSION) -lvtkCommonDataModel-$(VTKVERSION) -lvtkIOPLY-$(VTKVERSION)\
##  -lvtkFiltersCore-$(VTKVERSION) -lvtkFiltersHybrid-$(VTKVERSION) \
##  -lvtkIOLegacy-$(VTKVERSION) -lvtkCommonCore-$(VTKVERSION) -lvtkIOCore-$(VTKVERSION) \
##  -lvtkCommonTransforms-$(VTKVERSION) -lvtkCommonMisc-$(VTKVERSION) -lvtkCommonMath-$(VTKVERSION) -lvtkCommonSystem-$(VTKVERSION) \
##  -lvtksys-$(VTKVERSION) -lvtkfreetype-$(VTKVERSION) -lvtkpng-$(VTKVERSION) -lvtkjpeg-$(VTKVERSION) -lvtkzlib-$(VTKVERSION)
##CXX = $(CROSS)c++
##CC = $(CROSS)gcc
##POBJECTS = loch.res lxR2D.o
##CXXPFLAGS = -W -Wall -std=c++20 -DLXWIN32 $(shell $(CROSS)wx-config --static --cxxflags) $(shell $(CROSS)pkg-config freetype2 --cflags) -I$(VTKPATH) -Wno-deprecated
##CCPFLAGS = -W -Wall -DLXWIN32 $(shell $(CROSS)wx-config --static --cflags)
##LXLIBDIR =
##PLIBS = $(shell $(CROSS)pkg-config freetype2 --libs) $(shell $(CROSS)wx-config --static --libs --gl-libs) -L$(VTKLIBPATH) $(VTKLIBS)
##LXPLATFORM = WIN32
##STRIPFLAG = -static-libgcc -static -s

# PLATFORM MACOSX
##CXX = c++
##CC = cc
##POBJECTS =
##CXXPFLAGS = -W -Wall -std=c++20 -DLXMACOSX $(shell wx-config --cxxflags) $(shell pkg-config freetype2 --cflags) -I$(VTKPATH) -Wno-deprecated -I/usr/X11R6/include -I/usr/X11R6/include/freetype2
##CCPFLAGS = -W -Wall -DLXMACOSX $(shell wx-config --cflags) -I/usr/X11R6/include
##LXLIBDIR =
##PLIBS = -lz -L/usr/X11R6/lib $(shell wx-config --libs --gl-libs) -L$(VTKLIBPATH) $(VTKLIBS)
##LOCHAPPBINDIR := $(shell mkdir -p ./loch.app/Contents/MacOS)
##POSTMAKE = cp -f ./loch ./loch.app/Contents/MacOS/loch
##STRIPFLAG =

# PLATFORM ENDCONFIG


# BUILD CONFIG

# BUILD OZONE
##CCBFLAGS = -O3
##CXXBFLAGS = -O3
##LDBFLAGS = $(STRIPFLAG)

# BUILD OXYGEN
CCBFLAGS = -O2 -DNDEBUG
CXXBFLAGS = -O2 -DNDEBUG
LDBFLAGS = $(STRIPFLAG)

# BUILD RELEASE
##CCBFLAGS = -DNDEBUG
##CXXBFLAGS = -DNDEBUG
##LDBFLAGS = $(STRIPFLAG)

# BUILD DEBUG
##CCBFLAGS = -ggdb
##CXXBFLAGS = -ggdb -DLXDEBUG
##LDBFLAGS =

# BUILD ENDCONFIG


# compiler settings
CXXFLAGS := -Wall -DLOCH $(CXXPFLAGS) $(CXXBFLAGS) -DIMG_API_VERSION=1 -I../extern/img
CCFLAGS := -Wall -DLOCH $(CCPFLAGS) $(CCBFLAGS) -DIMG_API_VERSION=1
OBJECTS = $(addprefix $(LOUTDIR)/,$(POBJECTS)) $(addprefix $(LOUTDIR)/,$(CMNOBJECTS))


# linker settings
LIBS = $(PLIBS) -lfmt
LDFLAGS = $(LDBFLAGS)


$(LOUTDIR)/%.o : %.cxx
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(LOUTDIR)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $<


all: $(LOUTDIR)/loch
	$(MAKE) -C ./help

hlp:
	$(MAKE) -C ./help

$(LOUTDIR)/extern/img.o: ../extern/img/img.c
	mkdir -p $(LOUTDIR)/extern
	$(CC) -c $(CCFLAGS) -o $@ $<

$(LOUTDIR)/loch: $(OBJECTS)
	$(CXX) -o $(LOUTDIR)/loch$(EXT) -Wall $(OBJECTS) $(LIBS) $(LDFLAGS)
	$(POSTMAKE)

$(LOUTDIR)/loch.res: loch.rc loch.ico
	$(CROSS)windres -i loch.rc $(shell $(CROSS)$(WX_CONFIG) --cflags | tr ' ' '\n' | grep "^-I" | tr '\n' ' ') -J rc -o $(LOUTDIR)/loch.res -O coff

graphs:
	dot -Tps graph-vispipe.dot -o graph-vispipe.ps


depend:
	perl makedepend.pl > Makefile.dep
	perl maketest.pl Makefile.dep
	perl makefile.pl mv Makefile.dep Makefile
	$(CXX) -DLXDEPCHECK -DLOCH -D'wxCHECK_VERSION(X,Y,Z)=1' -MM *.cxx >> Makefile
	$(CC) -DLXDEPCHECK -DLOCH -MM *.c >> Makefile
	perl makedepend2.pl


config-debug:
	perl makeconfig.pl BUILD DEBUG

test-release:
	zip -9 loch.zip loch.exe test.th test.jpg thconfig

config-release:
	perl makeconfig.pl BUILD RELEASE

config-oxygen:
	perl makeconfig.pl BUILD OXYGEN

config-ozone:
	perl makeconfig.pl BUILD OZONE

config-linux:
	perl makeconfig.pl PLATFORM LINUX

config-debian:
	perl makeconfig.pl PLATFORM DEBIAN

config-win32:
	perl makeconfig.pl PLATFORM WIN32

config-win32cross:
	perl makeconfig.pl PLATFORM WIN32CROSS

config-macosx:
	perl makeconfig.pl PLATFORM MACOSX

clean:
	perl makefile.pl rm -q *~
	perl makefile.pl rm -q help/*/*~
	perl makefile.pl rm -q loch.exe
	perl makefile.pl rm -q loch.res
	perl makefile.pl rm -q loch
	perl makefile.pl rm -q loch.app/Contents/MacOS/loch
	perl makefile.pl rm -q *.o
	perl makefile.pl rm -q *.log
	perl makefile.pl rm -q core
	perl makefile.pl rmdir -q .xvpics
	perl makefile.pl rm -q *.bmp
	perl makefile.pl rm -q *.ppm
	perl makefile.pl rm -q *.zip
	perl makefile.pl rm -q *.pdf
	perl makefile.pl rm -q *.png
	$(MAKE) -C help clean



# DEPENDENCIES
$(LOUTDIR)/lxAboutDlg.o: lxAboutDlg.cxx lxWX.h icons/about.xpm ../thversion.h
$(LOUTDIR)/lxData.o: lxData.cxx lxData.h lxMath.h lxImgIO.h lxFile.h lxLRUD.h
$(LOUTDIR)/lxFile.o: lxFile.cxx lxFile.h lxMath.h ../extern/img/img.h
$(LOUTDIR)/icase.o: icase.h
$(LOUTDIR)/lxGLC.o: lxGLC.cxx lxGLC.h lxMath.h lxGUI.h lxData.h lxImgIO.h lxFile.h \
 lxSetup.h lxSView.h lxWX.h lxFNT6x13_bdf.h lxFNT10x20_bdf.h \
 lxFNTFreeSans_ttf.h lxRender.h lxTR.h
$(LOUTDIR)/lxGUI.o: lxGUI.cxx lxGUI.h lxGLC.h lxMath.h lxData.h lxImgIO.h lxFile.h \
 lxSetup.h lxRender.h lxOptDlg.h lxAboutDlg.h lxSView.h lxWX.h lxSScene.h \
 lxSTree.h lxPres.h icons/open.xpm icons/render.xpm icons/reload.xpm \
 icons/stereo.xpm icons/rotation.xpm icons/lockrot.xpm icons/fit.xpm \
 icons/home.xpm icons/rendersetup.xpm icons/plan.xpm icons/profile.xpm \
 icons/fullscreen.xpm icons/orto.xpm icons/camera.xpm icons/scene.xpm \
 icons/viscline.xpm icons/vissurface.xpm icons/visbbox.xpm \
 icons/viswalls.xpm icons/visinds.xpm icons/visentrance.xpm \
 icons/visfix.xpm icons/visstation.xpm icons/vislabel.xpm loch.xpm
$(LOUTDIR)/lxImgIO.o: lxImgIO.cxx lxImgIO.h
$(LOUTDIR)/lxLRUD.o: lxLRUD.cxx lxLRUD.h lxMath.h
$(LOUTDIR)/lxMath.o: lxMath.cxx lxMath.h
$(LOUTDIR)/lxOGLFT.o: lxOGLFT.cxx lxOGLFT.h
$(LOUTDIR)/lxOptDlg.o: lxOptDlg.cxx lxWX.h lxGUI.h lxGLC.h lxMath.h
$(LOUTDIR)/lxPres.o: lxPres.cxx lxPres.h lxWX.h lxGUI.h lxGLC.h lxMath.h lxSetup.h \
 lxData.h lxImgIO.h lxFile.h loch.xpm
$(LOUTDIR)/lxRender.o: lxRender.cxx lxRender.h lxWX.h lxGLC.h lxMath.h lxGUI.h \
 lxSetup.h lxData.h lxImgIO.h lxFile.h lxTR.h
$(LOUTDIR)/lxSScene.o: lxSScene.cxx lxSScene.h lxWX.h lxGUI.h lxGLC.h lxMath.h \
 lxSetup.h lxData.h lxImgIO.h lxFile.h loch.xpm
$(LOUTDIR)/lxSTree.o: lxSTree.cxx lxSTree.h lxWX.h lxGUI.h lxGLC.h lxMath.h \
 lxSetup.h lxData.h lxImgIO.h lxFile.h loch.xpm
$(LOUTDIR)/lxSView.o: lxSView.cxx lxSView.h lxWX.h lxGUI.h lxGLC.h lxMath.h \
 lxSetup.h lxData.h lxImgIO.h lxFile.h loch.xpm
$(LOUTDIR)/lxSetup.o: lxSetup.cxx lxSetup.h lxData.h lxMath.h lxImgIO.h lxFile.h
$(LOUTDIR)/lxWX.o: lxWX.cxx lxWX.h
$(LOUTDIR)/extern/img.o: ../extern/img/img.c ../extern/img/img.h
$(LOUTDIR)/lxR2D.o: lxR2D.cxx lxR2D.h
$(LOUTDIR)/lxR2P.o: lxR2P.cxx lxR2P.h
$(LOUTDIR)/lxTR.o: lxTR.cxx lxTR.h
$(LOUTDIR)/lxTriGeom.o: lxTriGeom.cxx lxTriGeom.h lxMath.h
