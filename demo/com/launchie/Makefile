#
TOPDIR = ../../..

#
include $(TOPDIR)/make.inc

#
CFLAGS =
CXXFLAGS = 
INCS = -I"$(TOPDIR)/inc" -I.
LIBS = -L"$(TOPDIR)/lib" -luuid -lole32 -loleaut32 -mwindows


#
OBJ = launchie.o
BIN = launchie.exe
RC  = 
RES = ${RC:.rc=.res}
#
HEADER =
#
all: $(BIN)

#
launchie.exe : launchie.o
	$(CXX) -o $@ $< $(LIBS)


#
$(RES) : $(RC) resource.h
	$(WINDRES) -i $(RC) -o $(RES) -O coff -I"$(ICODIR)" -I"$(RESDIR)"

#
clean :
	$(RM) ${OBJ} ${RES} ${BIN}
