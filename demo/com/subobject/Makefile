#
TOPDIR = ../../..

#
include $(TOPDIR)/make.inc

#
CFLAGS =
CXXFLAGS = 
INCS = -I"$(TOPDIR)/inc" -I"."
LIBS = -L"$(TOPDIR)/lib" 

#
OBJ = test_subobject.o
BIN = test_subobject.exe
RC  = 
RES = ${RC:.rc=.res}
#
HEADERS =

#
all: $(BIN) 

#
test_subobject.exe : test_subobject.o
	$(CXX) -o $@ $< -loleaut32
  
#
clean :
	$(RM) ${OBJ} ${RES} ${BIN}
