#
TOPDIR = ../..

#
include $(TOPDIR)/make.inc

#
INCS = -I"$(TOPDIR)/inc" -I./libssh-0.5.0/include
LIBS = -L"$(TOPDIR)/lib" -mwindows -L./libssh-0.5.0/lib -lssh
CFLAGS +=
CXXFLAGS += -DBUILD_EXE

#
OBJ = test_ussh.o


BIN = test_ussh.exe
RC  = ussh.rc
RES = ${RC:.rc=.res}
#
HEADER = 
#
all: $(BIN)

#
$(BIN) : $(OBJ) $(RES)
	$(CXX) -o $(BIN) $^ $(LIBS)
	cp -v $@ $(BINDIR)/$@
#
.PHONY : test
test :
	$(WINE) $(BINDIR)/$(BIN)     
#
.PHONY : clean
clean :
	$(RM) ${OBJ} ${RES} ${BIN}

