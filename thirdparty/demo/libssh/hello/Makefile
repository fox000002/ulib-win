#
TOPDIR = ../../../..

#
include $(TOPDIR)/make.inc

THIRDPARTY_DIR = $(TOPDIR)/thirdparty
EXPAT_INC = $(THIRDPARTY_DIR)/include
EXPAT_LIB = -L$(THIRDPARTY_DIR)/lib -lexpat
#
INC = -I"$(INCDIR)" -I"$(EXPAT_INC)"
LIBS = -L"$(LIBDIR)" $(EXPAT_LIB) -lssh
CFLAGS += 
CXXFLAGS += $(INC) -DBUILD_EXE
LDFLAGS += $(LIBS)
#
OBJ = libssh_hello.o
BIN = libssh_hello.exe
RC  =
RES = ${RC:.rc=.res}
#
HEADER =
#
all: $(BIN)

#
$(BIN) : $(OBJ) $(RES)
	$(CXX) -o $(BIN) $^ $(LIBS)
	cp -v $@ $(BINDIR)/$@

.PHONY : test
test :
	$(WINE) $(BINDIR)/$(BIN)
    
#
.PHONY : clean
clean :
	$(RM) ${OBJ} ${RES} ${BIN}

