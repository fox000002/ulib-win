#
TOPDIR = ../..
#
include $(TOPDIR)/make.inc

#
INCS = -I"$(TOPDIR)/inc"
LIBS = -L"$(TOPDIR)/lib" -llz32
CFLAGS +=
CXXFLAGS += -DBUILD_EXE
#
OBJ = test_zip.o
BIN = test_zip.exe
RC  = zip.rc
RES = ${RC:.rc=.res}
#
HEADERS = 
#
all: $(BIN)

#
$(BIN) : $(OBJ) $(RES)
	$(CXX) -o $(BIN) $^ $(LIBS)
	cp -vf $(BIN) $(BINDIR)/$(BIN)

#
.PHONY : test
test :
	$(WINE) $(BINDIR)/$(BIN) google_shortcuts_all_google_services_at_a_glance-2.1.7.1-fx.xpi
#
clean :
	$(RM) ${OBJ} ${RES} ${BIN}
