#
TOPDIR = ../../..

#
include $(TOPDIR)/make.inc

#
CFLAGS =
CXXFLAGS += -DBUILD_EXE
INCS = -I"$(TOPDIR)/inc" -I.
LIBS = -L"$(TOPDIR)/lib" -lole32 -loleaut32


#
OBJ = regtypelib.o
BIN = regtypelib.exe
RC  = 
RES = ${RC:.rc=.res}
#
HEADER =
#
all: $(BIN)

#
regtypelib.exe : regtypelib.o
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
