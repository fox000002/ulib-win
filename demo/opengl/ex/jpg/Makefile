#
TOPDIR = ../../../..

include $(TOPDIR)/make.inc

#
INCS = -I"$(INCDIR)"
LIBS = -L"$(LIBDIR)" -mwindows -luwin -lopengl32 -lglu32
CFLAGS += $(INCS)
CXXFLAGS += $(INCS)
LDFLAGS = $(LIBS)
#
OBJ = jpg_opengl.o

BIN = jpg_opengl.exe
RC  = jpg.rc
RES = ${RC:.rc=.res}
#
HEADER = $(INCDIR)/ucontrol.h \
		 $(INCDIR)/udialogx.h
#
all: $(BIN)

#
$(BIN) : $(OBJ) $(RES)
	$(CXX) -o $(BIN) $^ $(LIBS)
	cp -v $@ $(BINDIR)/$@

#
jpg_opengl.o : jpg_opengl.cpp $(HEADER)
	$(CXX) -c -o $@ -DBUILD_EXE $< $(CXXFLAGS) $(INC)

#
#$(RES) : $(RC) resource.h
#	$(WINDRES) -i $(RC) -o $(RES) -O coff -I"$(ICODIR)" -I"$(RESDIR)"
#
.PHONY : test
test :
	$(WINE) $(BINDIR)/$(BIN)
#
.PHONY : clean
clean :
	$(RM) ${OBJ} ${RES} ${BIN}

