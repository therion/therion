# common therion objects
CMNOBJECTS = \
  lxTR.o lxOGLFT.o lxSetup.o lxRender.o lxWX.o \
  lxImgIO.o lxLRUD.o lxFile.o \
  lxData.o lxMath.o lxSView.o lxSScene.o \
  lxGUI.o lxGLCo.o lxGLCs.o lxOptDlg.o lxAboutDlg.o \
  img.o

VTKLIBS = -lvtkHybrid -lvtkImaging -lvtkIO -lvtkGraphics -lvtkFiltering -lvtkCommon -lvtkjpeg -lvtkpng -lvtkzlib -lfreetype
POSTMAKE =
STRIPFLAG = -s 

# PLATFORM CONFIG

# PLATFORM LINUX
##CXX = c++
##CC = gcc
##POBJECTS = lxR2P.o
##VTKPATH = /usr/local/include/vtk-5.0
##VTKLIBPATH = /usr/local/lib/vtk-5.0
##CXXPFLAGS = -DLXLINUX $(shell wx-config --cxxflags) -Wno-deprecated $(shell freetype-config --cflags) -I$(VTKPATH)
##CCPFLAGS = -DLXLINUX  $(shell wx-config --cflags)
##LXLIBDIR = linux
##PLIBS = $(shell wx-config --libs --gl-libs) -L$(VTKPATH)/lib/vtk $(VTKLIBS) -lpthread -lz 
##LXPLATFORM = LINUX

# PLATFORM DEBIAN
CXX = c++
CC = gcc
POBJECTS = lxR2P.o
VTKPATH = /usr/include/vtk-5.0
VTKLIBPATH = /usr/lib/vtk-5.0
VTKLIBS = -lvtkHybrid -lvtkImaging -lvtkIO -lvtkGraphics -lvtkFiltering -lvtkCommon -lfreetype
CXXPFLAGS = -DLXLINUX $(shell wx-config --cxxflags) -Wno-deprecated $(shell freetype-config --cflags) -I$(VTKPATH)
CCPFLAGS = -DLXLINUX  $(shell wx-config --cflags)
LXLIBDIR = linux
PLIBS = $(shell wx-config --libs --gl-libs) -L$(VTKPATH)/lib/vtk $(VTKLIBS) -lpthread -lz 
LXPLATFORM = LINUX

# PLATFORM WIN32
##CXX = c++
##CC = gcc
##VTKPATH = /usr/local/include/vtk-5.0
##POBJECTS = loch.res lxR2D.o getline.o
##CXXPFLAGS = -W -Wall -DLXWIN32 $(shell wx-config --static --cxxflags) $(shell freetype-config --cflags) -I$(VTKPATH) -Wno-deprecated
##CCPFLAGS = -W -Wall -DLXWIN32 $(shell wx-config --static --cflags)
##LXLIBDIR = mingw
##PLIBS = $(shell freetype-config --libs) $(shell wx-config --static --libs --gl-libs) $(VTKLIBS) -lwxzlib-2.6
##LXPLATFORM = WIN32

# PLATFORM MACOSX
##CXX = c++
##CC = cc
##VTKPATH = /usr/local/include/vtk-5.0
##POBJECTS = getline.o
##CXXPFLAGS = -W -Wall -DLXMACOSX $(shell wx-config --static --cxxflags) -I$(VTKPATH) -Wno-deprecated -I/usr/X11R6/include -I/usr/X11R6/include/freetype2
##CCPFLAGS = -W -Wall -DLXMACOSX $(shell wx-config --static --cflags) -I/usr/X11R6/include
##LXLIBDIR = 
##PLIBS = -lz -L/usr/X11R6/lib $(shell wx-config --static --libs --gl-libs) $(VTKLIBS)
##POSTMAKE = cp -f ./loch ./loch.app/Contents/MacOS
##STRIPFLAG =

# PLATFORM ENDCONFIG


# BUILD CONFIG

# BUILD OZONE
##CCBFLAGS = -O3
##CXXBFLAGS = -O3
##LDBFLAGS = $(STRIPFLAG)

# BUILD OXYGEN
##CCBFLAGS = -O2
##CXXBFLAGS = -O2
##LDBFLAGS = $(STRIPFLAG)

# BUILD RELEASE
CCBFLAGS = 
CXXBFLAGS = 
LDBFLAGS = $(STRIPFLAG)

# BUILD DEBUG
##CCBFLAGS = -ggdb
##CXXBFLAGS = -ggdb -DLXDEBUG
##LDBFLAGS = 

# BUILD ENDCONFIG


# compiler settings
CXXFLAGS = -Wall -D_GNU_SOURCE -DLOCH $(CXXPFLAGS) $(CXXBFLAGS)
CCFLAGS = -Wall -D_GNU_SOURCE -DLOCH $(CCPFLAGS) $(CCBFLAGS)
OBJECTS = $(POBJECTS) $(CMNOBJECTS)

# linker settings
LIBS = $(PLIBS)
LDFLAGS = $(LDBFLAGS)


.SUFFIXES:
.SUFFIXES: .cxx .o .h .c

.cxx.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(CCFLAGS) -o $@ $<

all: loch hlp

hlp:
	make -C ./help

loch:	$(OBJECTS)
	$(CXX) -o loch -Wall $(OBJECTS) $(LIBS) $(LDFLAGS)
	$(POSTMAKE)

loch.res: loch.rc loch.ico
	windres --include-dir e:/MSVCProjects/wxWidgets-2.6.0/include -i loch.rc -I rc -o loch.res -O coff

graphs: 
	dot -Tps graph-vispipe.dot -o graph-vispipe.ps


depend:
	perl makedepend.pl > Makefile.dep
	perl maketest.pl Makefile.dep
	perl makefile.pl mv Makefile.dep Makefile
	$(CXX) -DLXDEPCHECK -DLOCH -MM *.cxx >> Makefile
	$(CC) -DLXDEPCHECK -DLOCH -MM *.c >> Makefile
  

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
  
config-macosx:
	perl makeconfig.pl PLATFORM MACOSX

clean:
	perl makefile.pl rm -q *~     
	perl makefile.pl rm -q help/*/*~
	perl makefile.pl rm -q loch.exe
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
  


# DEPENDENCIES
lxAboutDlg.o: lxAboutDlg.cxx lxWX.h icons/about.xpm ../thversion.h
lxData.o: lxData.cxx lxData.h lxMath.h lxImgIO.h lxFile.h lxLRUD.h
lxFile.o: lxFile.cxx lxFile.h lxMath.h img.h
lxGLCo.o: lxGLCo.cxx lxGLC.h lxMath.h lxGLCx.h lxGLCx.cxx lxSetup.h \
  lxData.h lxImgIO.h lxFile.h lxRender.h lxTR.h
lxGLCs.o: lxGLCs.cxx lxGLC.h lxMath.h lxGLCx.h lxGUI.h lxData.h lxImgIO.h \
  lxFile.h lxSetup.h lxSView.h lxWX.h lxFNT6x13_bdf.h lxFNT10x20_bdf.h \
  lxFNTFreeSans_ttf.h lxGLCx.cxx lxRender.h
lxGLCx.o: lxGLCx.cxx
lxGUI.o: lxGUI.cxx lxGUI.h lxGLC.h lxMath.h lxGLCx.h lxData.h lxImgIO.h \
  lxFile.h lxSetup.h lxRender.h lxOptDlg.h lxAboutDlg.h lxSView.h lxWX.h \
  lxSScene.h icons/open.xpm icons/render.xpm icons/reload.xpm \
  icons/stereo.xpm icons/rotation.xpm icons/lockrot.xpm icons/fit.xpm \
  icons/home.xpm icons/rendersetup.xpm icons/plan.xpm icons/profile.xpm \
  icons/fullscreen.xpm icons/orto.xpm icons/camera.xpm icons/scene.xpm \
  icons/viscline.xpm icons/vissurface.xpm icons/visbbox.xpm \
  icons/viswalls.xpm icons/visinds.xpm loch.xpm
lxImgIO.o: lxImgIO.cxx lxImgIO.h
lxLRUD.o: lxLRUD.cxx lxLRUD.h lxMath.h
lxMath.o: lxMath.cxx lxMath.h
lxOGLFT.o: lxOGLFT.cxx lxOGLFT.h
lxOptDlg.o: lxOptDlg.cxx lxWX.h lxGUI.h lxGLC.h lxMath.h lxGLCx.h
lxRender.o: lxRender.cxx lxRender.h lxWX.h lxGLC.h lxMath.h lxGLCx.h \
  lxGUI.h lxSetup.h lxData.h lxImgIO.h lxFile.h lxTR.h
lxSetup.o: lxSetup.cxx lxSetup.h lxData.h lxMath.h lxImgIO.h lxFile.h
lxSScene.o: lxSScene.cxx lxSScene.h lxWX.h lxGUI.h lxGLC.h lxMath.h \
  lxGLCx.h lxSetup.h lxData.h lxImgIO.h lxFile.h loch.xpm
lxSView.o: lxSView.cxx lxSView.h lxWX.h lxGUI.h lxGLC.h lxMath.h lxGLCx.h \
  lxSetup.h lxData.h lxImgIO.h lxFile.h loch.xpm
lxWX.o: lxWX.cxx lxWX.h
getline.o: getline.c
img.o: img.c img.h
lxR2D.o: lxR2D.c lxR2D.h
lxR2P.o: lxR2P.c lxR2P.h
lxTR.o: lxTR.c lxTR.h
