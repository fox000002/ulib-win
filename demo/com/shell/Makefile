#
TOPDIR = ../../..
#
include $(TOPDIR)/make.inc

#
CFLAGS =
CXXFLAGS = -DBUILD_EXE 
INCS = -I"$(TOPDIR)/inc" -I"."
LIBS = -L"$(TOPDIR)/lib" -luwin -luuid -lole32 -loleaut32 -lcomctl32 -mwindows 

#
OBJS = test_shell.o
BIN = test_shell.exe
RC  = shell.rc
RES = ${RC:.rc=.res}
#
HEADER =
#
all: $(BIN) 

#
$(BIN) : $(OBJS) $(RES)
	$(CXX) -o $@ $^ $(LIBS)
	cp -vf $@ $(BINDIR)/$@
	
#
.PHONY : test
test:
	$(WINE) $(BINDIR)/$(BIN)

#
clean :
	$(RM) ${OBJS} ${RES} ${BIN}
