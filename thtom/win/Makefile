# Under MSYS: make and make install tcltk
# than make

LIBS=-L/usr/local/lib -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -ltcl84 -ltk84 -lopengl32 -lglu32 
TK_LIBS=-ltk84 -ltcl84
CC=gcc -Wall -s -I/usr/local/include -DWIN32 -DMINGW
SRC=../src
OUT=Tom0.2

all: tom
	rm $(OUT)/*.o

tom: $(OUT)/libtom.dll

stom: $(OUT) $(OBJECTS) $(OUT)/stom

libtom: $(OUT)/libtom.dll

OBJECTS=$(OUT)/tom.o $(OUT)/gl.o $(OUT)/glData.o\
	$(OUT)/glBuffer.o $(OUT)/glRaster.o $(OUT)/glName.o\
	$(OUT)/glLight.o $(OUT)/glMaterial.o $(OUT)/glMatrix.o\
	$(OUT)/glMesh.o $(OUT)/glVertex.o $(OUT)/glNormal.o\
	$(OUT)/glColor.o $(OUT)/glRect.o $(OUT)/glTex.o\
	$(OUT)/glList.o $(OUT)/glState.o\
	$(OUT)/glu.o $(OUT)/glut.o
ifdef IMLIB2
OBJECTS:=$(OBJECTS) $(OUT)/imlib2.o
endif

$(OUT):
	mkdir $(OUT)

$(OUT)/tom: $(OUT)/tkAppInit.o $(OBJECTS)
	$(CC) -o $@ $(OUT)/tkAppInit.o $(OUT)/libtom.dll $(LIBS) \
		$(TK_LIB_SPEC) $(TCL_LIB_SPEC) $(TK_LIBS)

$(OUT)/stom: $(OUT)/tkAppInit.o $(OBJECTS)
	$(CC) -o $@ $(OUT)/tkAppInit.o $(OBJECTS) $(X_LIBS) $(GL_LIBS)\
		$(TK_LIB_SPEC) $(TCL_LIB_SPEC) $(TK_LIBS)

$(OUT)/libtom.dll: $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) $(LIBS) $(TK_LIBS)

$(OUT)/%.o : $(SRC)/%.c
	$(CC) -c -o $@ $<

clean:
	rm -f $(OUT)/*.o 
	rm -f $(OUT)/*.dll 
	rm -f $(OUT)/tom.exe
	rm -f $(OUT)/stom.exe
	find .. -name '*~' -exec rm -f '{}' ';'
