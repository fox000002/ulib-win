#
TOPDIR = ../..
#
include $(TOPDIR)/make.inc

#
INCS = -I"$(TOPDIR)/inc"
LIBS = -L"$(TOPDIR)/lib" -llibuwin
CFLAGS +=
CXXFLAGS += -DBUILD_EXE

#
OBJ = test_async.o
BIN = test_async.exe
RC  =
RES = ${RC:.rc=.res}
#
HEADER = 
#
all: $(BIN)

#
$(BIN) : $(OBJ) $(RES)
	$(CXX) -o $(BIN) $^ $(LIBS) $(RES)
	cp -vf $@ $(BINDIR)/$@

#
.PHONY : test
test:
	$(WINE) $(BINDIR)/$(BIN)

.PHONY : clean
clean :
	${RM} ${OBJ} ${BIN} $(RES)
