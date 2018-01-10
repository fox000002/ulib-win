#
TOPDIR = ../../..

#
include $(TOPDIR)/make.inc

#
CFLAGS =
CXXFLAGS += -DBUILD_EXE
INCS = -I"$(TOPDIR)/inc" -I.
LIBS = -L"$(TOPDIR)/lib" -luwin -loleaut32


#
OBJ = typelib.o
BIN = typelib.exe
RC  = 
RES = ${RC:.rc=.res}
#
HEADER =
#
all: $(BIN)

#
typelib.exe : typelib.o
	$(CXX) -o $@ $< $(LIBS)
	cp -vf $@ $(BINDIR)/$@


#
$(RES) : $(RC) resource.h
	$(WINDRES) -i $(RC) -o $(RES) -O coff -I"$(ICODIR)" -I"$(RESDIR)"

#
.PHONY : test
test :
	$(WINE) $(BINDIR)/$(BIN)
	
#
.PHONY : clean
clean :
	$(RM) ${OBJ} ${RES} ${BIN}
