#DEBUG=-DDEBUG
IMLIB2=-DIMLIB2
GL12=-DGL12
#GLUT=-DGLUT
#GLEXT=-DGLEXT
#GLSGIS=-DGLSGIS
GL=$(GL12) $(GLUT) $(GLEXT) $(GLSGIS)

TK_LIBS=-ldl -lm
TK_LIB_SPEC=-ltk
TCL_LIB_SPEC=-ltcl
X_LIBS=-L/usr/lib/ -L/usr/X11R6/lib -lX11 -lXext -lXmu -lXt -lXi -lSM -lICE
GL_LIBS=-lGL -lGLU
ifdef GLUT
GL_LIBS:=$(GL_LIBS) -lglut
endif

CC=gcc -Wall -s $(GL) -I/usr/X11R6/include $(IMLIB2)
ifdef DEBUG
CC:=$(CC) $(DEBUG) -g
endif

SRC=../src

OUT=Tom0.2

all: tom
	rm $(OUT)/*.o

# $(OUT)/tom
tom: $(OUT)/libtom.so

stom: $(OUT) $(OBJECTS) $(OUT)/stom

libtom: $(OUT) $(OUT)/libtom.so

OBJECTS=$(OUT)/tom.o $(OUT)/gl.o $(OUT)/glData.o\
	$(OUT)/glBuffer.o $(OUT)/glRaster.o $(OUT)/glName.o\
	$(OUT)/glLight.o $(OUT)/glMaterial.o $(OUT)/glMatrix.o\
	$(OUT)/glMesh.o $(OUT)/glVertex.o $(OUT)/glNormal.o\
	$(OUT)/glColor.o $(OUT)/glRect.o $(OUT)/glTex.o\
	$(OUT)/glList.o $(OUT)/glState.o \
	$(OUT)/glu.o $(OUT)/glut.o
ifdef IMLIB2
OBJECTS:=$(OBJECTS) $(OUT)/imlib2.o
endif

$(OUT):
	mkdir -p $(OUT)

$(OUT)/tom: $(OUT)/tkAppInit.o $(OBJECTS)
	$(CC) -o $@ $(OUT)/tkAppInit.o $(OUT)/libtom.so $(X_LIBS) $(GL_LIBS)\
		$(TK_LIB_SPEC) $(TCL_LIB_SPEC) $(TK_LIBS)

$(OUT)/stom: $(OUT)/tkAppInit.o $(OBJECTS)
	$(CC) -o $@ $(OUT)/tkAppInit.o $(OBJECTS) $(X_LIBS) $(GL_LIBS)\
		$(TK_LIB_SPEC) $(TCL_LIB_SPEC) $(TK_LIBS)

$(OUT)/libtom.so: $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) $(X_LIBS) $(GL_LIBS) $(TK_LIBS)

$(OUT)/%.o : $(SRC)/%.c
	$(CC) -c -o $@ $<

clean:
	rm -f $(OUT)/*.o
	rm -f $(OUT)/*.so
	rm -f $(OUT)/tom
	rm -f $(OUT)/stom
	find .. -name '*~' -exec rm -f '{}' ';'
